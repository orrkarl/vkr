#include <app.h>

constexpr const nr_uint dim = 3;
constexpr const nr_uint count = 2;

typedef nr::Simplex<dim> Triangle;

Triangle h_triangles[count] =
{
	Triangle {
		nr::Point<dim> { -0.1f, -0.0865f, 1.0f },
		nr::Point<dim> {  0.0f,  0.0865f, 1.0f },
		nr::Point<dim> {  0.1f, -0.0865f, 1.0f }
	}, 
	Triangle {
		nr::Point<dim> { -0.6f, -0.5f, 1.0f },
		nr::Point<dim> { -0.3f,  0.5f, 1.0f },
		nr::Point<dim> { -0.3f, -0.5f, 1.0f }
	}
};

nr_float h_near[3]
{
	-1, -1, 0.5
};

nr_float h_far[3]
{
	1, 1, 10
};

class Triangles3dApp : public App
{
public:
	Triangles3dApp()
		: App("Triangle3d", nr::ScreenDimension{ 640, 480 }, dim, count)
	{
		setNearPlane(h_near);
		setFarPlane(h_far);

		auto simplexes = getHostSimplexes<dim>();

		for (auto i = 0u; i < count; ++i)
		{
			simplexes[i] = h_triangles[i];
		}
	}

protected:
	void update() override
	{
	}
};

int main(const int argc, const char* argv[])
{
	if (!App::init()) return EXIT_FAILURE;

	auto app = Triangles3dApp();
	auto ret = app.run();

	App::deinit();

	return ret;
}
