#include "win_main.h"

#include <cstdio>

#include "io_stream.h"
#include "vector.h"
#include "matrix.h"
#include "lu_decomposition.h"

int main()
{
	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(0);
	return WinMain(HINSTANCE(GetModuleHandle(nullptr)), nullptr, nullptr, SW_SHOW);
}

	


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int mCmdShow)
{
	if (hPrevInstance != nullptr) return -1;

	const auto iden4 = math::algebra::Matrix::MakeIdentity(2);
	const math::algebra::Matrix mat0(3, 1, 2, 3, 4, 5, 7, 7, 8, 9);
	math::algebra::LUDecomposition decomp(mat0);

	std::cout << (decomp.Inverse() * mat0).IsIdentity() << std::endl;

	getchar();

	return 0;
}

