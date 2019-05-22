#include <general/predefs.h>

namespace nr
{

namespace type
{
	NRuint getByteSize(const Type& type)
	{
		switch (type)
		{
			case Type::DOUBLE:
				return sizeof(NRdouble);

			case Type::FLOAT: 
				return sizeof(NRfloat);

			case Type::ULONG:
				return sizeof(NRulong);

			case Type::LONG:
				return sizeof(NRlong);

			case Type::UINT: 
				return sizeof(NRuint);

			case Type::INT:
				return sizeof(NRint);

			case Type::USHORT: 
				return sizeof(NRushort);
				
			case Type::SHORT:
				return sizeof(NRshort);

			case Type::CHAR:
				return sizeof(NRchar);

			case Type::BOOL:
				return sizeof(NRbool); 
			
			default:
				return 0u;
		}
	}
}

namespace error
{
	NRbool isSuccess(const cl_int& err) { return err == CL_SUCCESS; }
	NRbool isFailure(const cl_int& err) { return err != CL_SUCCESS; }
}


}
