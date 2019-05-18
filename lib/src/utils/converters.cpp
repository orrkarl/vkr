#include <utils/converters.h>

namespace nr
{

namespace utils
{

GLenum fromNRType(Type type)
{
    switch(type)
    {
        case Type::BYTE:    return GL_BYTE;
        case Type::UBYTE:   return GL_UNSIGNED_BYTE;
        
        case Type::SHORT:   return GL_SHORT;
        case Type::USHORT:  return GL_UNSIGNED_SHORT;
        
        case Type::BOOL:    return GL_BOOL;

        case Type::INT:     return GL_INT;
        case Type::UINT:    return GL_UNSIGNED_INT;

        case Type::FLOAT:   return GL_FLOAT;
        case Type::DOUBLE:  return GL_DOUBLE;
        
        case Type::RAW:     
            return 0; // this is not the default because i will probably change it in the future

        default:
            return 0; // type is not supported in core GL 
    }
}

Error fromCLError(const cl_int& err)
{
    switch(err)
    {
        case CL_SUCCESS:
            return Error::NO_ERROR;

        case CL_INVALID_PROGRAM:
        case CL_INVALID_PROGRAM_EXECUTABLE:
        case CL_INVALID_KERNEL_NAME:
        case CL_INVALID_VALUE:
        case CL_INVALID_COMMAND_QUEUE:
        case CL_INVALID_CONTEXT:
        case CL_INVALID_MEM_OBJECT:
            return Error::INVALID_VALUE;

        case CL_INVALID_KERNEL_ARGS:
            return Error::INVALID_ARGS;

        case CL_INVALID_KERNEL_DEFINITION:
            return Error::INVALID_TYPE;

        case CL_OUT_OF_RESOURCES:
            return Error::DEVIDE_OUT_OF_MEMORY; 
        case CL_OUT_OF_HOST_MEMORY:
            return Error::HOST_OUT_OF_MEMORY;
        
        case CL_INVALID_OPERATION:
            return Error::INVALID_OPERATION;

        case CL_INVALID_BUILD_OPTIONS:
            return Error::INVALID_COMPILER_OPTIONS;
        case CL_BUILD_PROGRAM_FAILURE:
            return Error::COMPILATION_FAILURE;
        case CL_COMPILER_NOT_AVAILABLE:
            return Error::INVALID_COMPILER;

        default:
            return Error::UNKNOWN_ERROR;
    }
}

const string stringFromNRError(const Error& err)
{
    switch(err)
    {
        case Error::NO_ERROR: return "NO_ERROR";
	    case Error::INVALID_TYPE: return "INVALID_TYPE";
	    case Error::INVALID_VALUE: return "INVALID_VALUE";
	    case Error::INVALID_OPERATION: return "INVALID_OPERATION";
	    case Error::HOST_OUT_OF_MEMORY: return "HOST_OUT_OF_MEMORY";
	    case Error::DEVIDE_OUT_OF_MEMORY: return "DEVIDE_OUT_OF_MEMORY";
	    case Error::INVALID_COMPILER_OPTIONS: return "INVALID_COMPILER_OPTIONS";
	    case Error::COMPILATION_FAILURE: return "COMPILATION_FAILURE";
	    case Error::INVALID_COMPILER: return "INVALID_COMPILER";
	    case Error::INVALID_ARGS: return "INVALID_ARGS";
        case Error::UNKNOWN_ERROR: return "UNKNOWN_ERROR";

        default: return "Unrecognized error value";
    }
}

const string stringFromCLError(const cl_int& error)
{
switch(error){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}

}

}
