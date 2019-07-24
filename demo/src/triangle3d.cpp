#include <app.h>

constexpr const nr_uint dim = 3;

nr::Simplex<dim> h_triangle
{
	nr::Point<dim>{ -5, -2.5,   3, 1 },
	nr::Point<dim>{  0,	   5, 2.5, 1 },
	nr::Point<dim>{  5, -2.5,   2, 1 }
};

nr_float h_near[3]
{
    -3, -3, 0.5
};

nr_float h_far[3]
{
    3, 3, 10
};

class Triangle3dApp : public App
{
public:
	Triangle3dApp()
		: App("Triangle3d", nr::ScreenDimension{ 640, 480 }, 3, 1)
	{
		setNearPlane(h_near);
		setFarPlane(h_far);
	}

protected:
	void update() override
	{
		auto simplex = getHostSimplexes<dim>();
		*simplex = h_triangle;
	}
};

int main(const int argc, const char* argv[])
{
	if (!App::init()) return EXIT_FAILURE;

	auto app = Triangle3dApp();
	auto ret = app.run();

	App::deinit();

	return ret;
}
