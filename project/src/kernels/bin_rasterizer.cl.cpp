#include "bin_rasterizer.cl.h"

namespace nr
{

namespace detail
{

extern const char* BIN_RASTER_KERNEL_NAME = "bin_rasterize";

namespace clcode
{

extern const char* bin_rasterizer = R"__CODE__(

// -------------------------------------- Types and defines -------------------------------------- 

#ifndef BATCH_COUNT
	#define BATCH_COUNT (256)
#endif

// ----------------------------------------------------------------------------

// -------------------------------------- Utilities --------------------------------------


// Finds the minimum of 3 values
float min3(const float a, const float b, const float c)
{
    return min(a, min(b, c));
}

// Finds the maximum of 3 values
float max3(const float a, const float b, const float c)
{
    return max(a, max(b, c));
}

// Returns the bounding box of a 2d triangle (x0, y0, x1, y1)
float4 mk_triangle_bounding_rect(const local float x[3], const local float y[3])
{
    return (float4)(min3(x[0], x[1], x[2]), min3(y[0], y[1], y[2]), max3(x[0], x[1], x[2]), max3(y[0], y[1], y[2]));
}


// Calculates triangle bounding box, than checkes it it has any intersection with the bin
bool is_triangle_in_bin(const local float x[3], const local float y[3], const bin_t bin, const screen_dimension_t dim)
{
    float4 triangle_bounds = mk_triangle_bounding_rect(x, y);
    float4 bin_bounds = (float4)(
        axis_ndc_from_screen(bin.x, dim.width), axis_ndc_from_screen(bin.y, dim.height),
        axis_ndc_from_screen(bin.x + bin.width, dim.width), axis_ndc_from_screen(bin.y + bin.height, dim.height));   
    return bin_bounds.x <= triangle_bounds.z && triangle_bounds.x <= bin_bounds.z && bin_bounds.y <= triangle_bounds.w && triangle_bounds.y <= bin_bounds.w;
}

bin_t make_bin(const screen_dimension_t dim, const uint index_x, const uint index_y, const uint bin_width, const uint bin_height)
{
    bin_t ret;
    ret.x = index_x * bin_width;
    ret.y = index_y * bin_height;
    ret.width = min(dim.width - ret.x, bin_width);
    ret.height = min(dim.height - ret.y, bin_height); 
    return ret;
}

// Copies the contents of the global triangle buffer to local memory for a given batch
void reduce_triangle_buffer(
    const global triangle_record_t* triangles, 
    const uint triangle_count, 
    const uint offset, 
    local float* result_x, 
    local float* result_y)
{
    const global float* src_base = ((const global float*) (triangles + offset)); 

    const uint raw_copy_count = triangle_count * 3; // one for each point in the triangle buffer
	
	if (get_local_id(0) == 0)
	{
		for (uint i = 0; i < raw_copy_count; ++i)
		{
			result_x[i] = src_base[i * 4];
			result_y[i] = src_base[i * 4 + 1];
		}
	}
}

// ----------------------------------------------------------------------------

kernel void bin_rasterize(
    const global triangle_record_t* triangle_data,
    const uint triangle_count,
    const screen_dimension_t dim,
    const bin_queue_config_t config,
    global bool* has_overflow,
    global index_t* bin_queues,
    global uint* g_batch_index)
{
    local float reduced_triangles_x[BATCH_COUNT * 3];
    local float reduced_triangles_y[BATCH_COUNT * 3];
    local uint current_batch_index;

    // Workaround for that weird compiler bug
    private const screen_dimension_t screen_dim = dim;
    
    private uint index_x = get_local_id(0);
    private uint index_y = get_local_id(1);

    private bool is_init_manager = !index_x && !index_y;
    
    private const uint bins_count_x = ceil(((float) screen_dim.width) / config.bin_width);
    private const uint bins_count_y = ceil(((float) screen_dim.height) / config.bin_height);
    private uint bin_queue_base = (config.queue_size + 1) * 
        (bins_count_x * bins_count_y * (get_group_id(1) * get_num_groups(0) + get_group_id(0))
         + bins_count_x * index_y + index_x); 
    private uint current_queue_index = bin_queue_base + 1;

    private uint batch_actual_size;

    private const bin_t current_bin = make_bin(
        screen_dim, 
        index_x, 
        index_y, 
        config.bin_width, 
        config.bin_height);

    if (!get_global_id(0) && !get_global_id(1))
    {
        *g_batch_index = 0;
        *has_overflow = false;
    }

	// wait for GLOBAL batch index initialization
    barrier(CLK_GLOBAL_MEM_FENCE);

    if (is_init_manager)
    {
        current_batch_index = get_group_id(0) * BATCH_COUNT;
    }

	bin_queues[bin_queue_base] = 1;

    // wait for LOCAL batch index initialization
    barrier(CLK_LOCAL_MEM_FENCE);

    for(;;)
    {
        if (*has_overflow)
        {
            return;
        }
        
        // no more batches to process, this work group has no more work to do
        if (current_batch_index >= triangle_count)
        {
            return;
        }
        
        batch_actual_size = min((uint) BATCH_COUNT, triangle_count - current_batch_index);

        reduce_triangle_buffer(
            triangle_data, 
            batch_actual_size, 
            current_batch_index, 
            reduced_triangles_x, 
            reduced_triangles_y
		);

		barrier(CLK_LOCAL_MEM_FENCE);

        for (private uint i = 0; i < batch_actual_size; ++i)
        {
            if (
                is_triangle_in_bin(
                    reduced_triangles_x + i * 3, 
                    reduced_triangles_y + i * 3, 
                    current_bin, 
                    screen_dim))
            {   
                bin_queues[current_queue_index++] = current_batch_index + i;

                // Queue is not empty
                bin_queues[bin_queue_base] = 0;
            }

            // An overflowing queue was detected
            if (current_queue_index >= config.queue_size + bin_queue_base + 1)
            {
                *has_overflow = true;
                break;
            }
        }

        if (current_queue_index != bin_queue_base + 1 + config.queue_size)
        {
            // Queue is finished
            bin_queues[current_queue_index] = 0;
        }

        // Aquire a batch (update the local batch index)
        if (is_init_manager)
        {
            current_batch_index += get_num_groups(0) * BATCH_COUNT;
        }

		barrier(CLK_LOCAL_MEM_FENCE);
    }    
}

// -------------------------------------- Tests --------------------------------------

#ifdef _TEST_BINNING

#ifndef TOTAL_TRIANGLE_COUNT
    #error "TOTAL_TRIANGLE_COUNT has to be defined in binning test configuration"
#endif 

kernel void is_triangle_in_bin_test(
    const global float triangle_x[3],
    const global float triangle_y[3], 
    const bin_t bin,
    const screen_dimension_t dim,
    global bool* result)
{
    local float x[3];
    local float y[3];

    x[0] = triangle_x[0]; 
    x[1] = triangle_x[1]; 
    x[2] = triangle_x[2]; 

    y[0] = triangle_y[0]; 
    y[1] = triangle_y[1]; 
    y[2] = triangle_y[2]; 

    *result = is_triangle_in_bin(x, y, bin, dim);
}

kernel void reduce_triangle_buffer_test(
    const global triangle_record_t triangle_data[TOTAL_TRIANGLE_COUNT],
    const uint offset,
    global ndc_position_t result[TOTAL_TRIANGLE_COUNT * 3])
{
    local float res_x[TOTAL_TRIANGLE_COUNT * 3];
    local float res_y[TOTAL_TRIANGLE_COUNT * 3];
	const uint copiedTriangleCount = TOTAL_TRIANGLE_COUNT - offset;

    reduce_triangle_buffer(triangle_data, copiedTriangleCount, offset, res_x, res_y);

    if (get_global_id(0) == 0)
    {
        for (uint i = 0; i < 3 * (TOTAL_TRIANGLE_COUNT - offset); ++i)
        {
            result[i].x = res_x[i];
            result[i].y = res_y[i];
        }
    }
}

#endif // _TEST_BINNING

)__CODE__";

}

}

}