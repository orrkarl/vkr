typedef struct _RasterizeInfo
{
    const uint dimension;
    const uint width, height;
} RasterizeInfo;

void ndcToScreen(const RasterizeInfo* data, const float x, const float y, write_only uchar* dest)
{
    uint index_x = (uint) (data->width * 0.5 * (1 + x))
    uint index_y = (uint) (data->height * 0.5 * (1 + y))
    uint idx = index_y * data->width + index_x;
    
    dest[idx] = 255;
    dest[idx + 1] = 0;
    dest[idx + 2] = 0;
}

kernel void rasterize(constant RasterizeInfo* info, read_only const float* src, write_only uchar* dest)
{
    const uint i = get_global_id(0);
    ndcToScreen(data, src[i], src[i + 1], dest);
}
