#pragma once

#ifdef _MSC_VER
	#define NR_SHARED __declspec(dllexport)
#else
	#define NR_SHARED
#endif // _MSC_VER
