#include <app.h>
#include <linalg.h>
#include <utils.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <functional>
#include <utility>
#include <random>

constexpr const nr_uint dim = 4;

nr_float h_near[dim]
{
	-5, -5, 0.5, 0.5
};

nr_float h_far[dim]
{
	5, 5, 10, 10
};

class SimplexesApp : public App
{
public:
	SimplexesApp()
		: App("Simplexes", nr::ScreenDimension{ 640, 480 }, dim, SimplexesApp::COUNT)
	{
		setNearPlane(h_near);
		setFarPlane(h_far);

		setup(reinterpret_cast<Tetrahedron*>(getHostSimplexes<dim>()));
	}

	static void setup(Tetrahedron* result)
	{
		Tetrahedron base{
			Vector{ -0.5f, -0.433f, -0.433f, 1.0f, 1.0f },
			Vector{  0.0f,  0.433f, -0.433f, 1.0f, 1.0f },
			Vector{  0.5f, -0.433f, -0.433f, 1.0f, 1.0f },
			Vector{  0.0f,    0.0f,  0.433f, 1.0f, 1.0f }
		};
		
		const auto globalPosition = Matrix::translation(0.0f, 0.0f, 2.0f, 2.0f);

		std::vector<Matrix> t(SimplexesApp::COUNT);
		for (auto i = 0; i < SimplexesApp::COUNT; ++i)
		{
			t[i] = Matrix::identity();
		}
		t[0] = Matrix::translation(6.0f, 3.0f, 1.0f, 1.0f);
		t[1] = Matrix::translation(-6.0f, -3.0f, 1.0f, 1.0f);
		t[2] = Matrix::translation(-6.0f, 3.0f, 1.0f, 1.0f);
		t[3] = Matrix::translation(6.0f, -3.0f, 1.0f, 1.0f);
		t[4] = Matrix::translation(0.0f, 0.0f, 1.0f, 1.0f);

		Matrix op;
		
		for (auto i = 0u; i < SimplexesApp::COUNT; ++i)
		{
			op = globalPosition * t[i];

			for (auto j = 0u; j < dim; ++j)
			{
				result[i].points[j] = op * base.points[j];
			}
		}
	}

protected:
	void update() override
	{
	}

private:
	static const nr_uint COUNT = 5;
};

int main(const int argc, const char* argv[])
{
	if (!App::init()) return EXIT_FAILURE;

	auto app = SimplexesApp();
	auto ret = app.run();

	App::deinit();

	return ret;
}
