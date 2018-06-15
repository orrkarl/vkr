#include "win_main.h"

#include <cstdio>

#include "io_stream.h"
#include "vector.h"

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
	typedef math::algebra::Vector<15> Vector15;
	typedef math::algebra::Vector<2> Vector2;

	Vector2 vec2{ 1, -1 };
	Vector2 vec3{ 1, 1 };
	Vector15 vec4{ 1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1 };
	Vector15 vec5{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

	std::cout << vec2.AsUnitVector().Norm() << std::endl;
	std::cout << vec3.AsUnitVector().Norm() << std::endl;
	std::cout << vec4.AsUnitVector().IsUnitVector() << std::endl;
	std::cout << vec5.AsUnitVector().IsUnitVector() << std::endl;

	std::cout << vec2[-1] << std::endl;

	getchar();
	return 0;
}

