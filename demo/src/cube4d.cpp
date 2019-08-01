// Some compilers (msvc...) don't include the math constants (e.g. PI) unless this macro is defined
#define _USE_MATH_DEFINES
#include <cmath>

#include <app.h>
#include <linalg.h>
#include <utils.h>

const nr_uint dim = 4;

Vector h_cube[]
{
    Vector(-1, -1, -1, -1, 1),
    Vector(-1, -1, -1,  1, 1),
    Vector(-1, -1,  1, -1, 1),
    Vector(-1, -1,  1,  1, 1),
    Vector(-1,  1, -1, -1, 1),
    Vector(-1,  1, -1,  1, 1),
    Vector(-1,  1,  1, -1, 1),
    Vector(-1,  1,  1,  1, 1),
    Vector( 1, -1, -1, -1, 1),
    Vector( 1, -1, -1,  1, 1),
    Vector( 1, -1,  1, -1, 1),
    Vector( 1, -1,  1,  1, 1),
    Vector( 1,  1, -1, -1, 1),
    Vector( 1,  1, -1,  1, 1),
    Vector( 1,  1,  1, -1, 1),
    Vector( 1,  1,  1,  1, 1),
};

nr_float h_near[]
{
    -5, -5, 0.5, 0.5
};

nr_float h_far[]
{
    5, 5, 10, 10
};

void transform(Tetrahedron simplexes[48], const nr_float angle)
{
    Matrix r = Matrix::rotation(Y, Z, angle);
    Matrix t = Matrix::translation(1.5, 1.5, 3, 2);
    
    Matrix op = t * r;

    Vector cube[16];
    for (auto i = 0; i < 16; ++i)
    {
		cube[i] = op * h_cube[i];
    }

	//std::cout << "Cube:\n";
	//for (auto v = 0u; v < 16; ++v)
	//{
	//	std::cout << "\tvertex " << v << " - " << cube[v] << std::endl;
	//}

	cube4dToSimplices(cube, simplexes);
}

class DynamicCube4dApp : public App
{
public:
	DynamicCube4dApp(const nr_uint divisions)
		: App("Dynamic Cube4d", nr::ScreenDimension{ 640, 480 }, dim, 48), divisions(divisions)
	{
		setNearPlane(h_near);
		setFarPlane(h_far);
	}

protected:
	void update() override
	{
		const nr_float angle = (tick++) * (2 * M_PI) / divisions;

		if (glfwGetKey(getWindow(), GLFW_KEY_S) == GLFW_PRESS)
		{
			std::cout << "Tick: " << tick << '\n';
			std::cout << "Angle: " << angle << " (rad)" << std::endl;
			std::cout << "Angle: " << 180 * angle / nr_float(M_PI) << " (deg)" << std::endl;
		}

		transform(reinterpret_cast<Tetrahedron*>(getHostSimplexes<dim>()), angle);
	}

private:
	nr_uint tick = 0;
	const nr_uint divisions;
};

class StaticCube4dApp : public App
{
public:
	StaticCube4dApp(const nr_float angle)
		: App("Static Cube4d", nr::ScreenDimension{ 640, 480 }, dim, 48), angle(angle)
	{
		setNearPlane(h_near);
		setFarPlane(h_far);
	}

	StaticCube4dApp(const nr_uint tick, const nr_uint divisions)
		: StaticCube4dApp(tick * 2 * M_PI / divisions)
	{
	}

protected:
	void update() override
	{
		if (glfwGetKey(getWindow(), GLFW_KEY_R) == GLFW_PRESS)
		{
			std::cout << "Angle: " << angle << " (rad)" << std::endl;
		}
		if (glfwGetKey(getWindow(), GLFW_KEY_D) == GLFW_PRESS)
		{
			std::cout << "Angle: " << 180 * angle / nr_float(M_PI) << " (deg)" << std::endl;
		}

		transform(reinterpret_cast<Tetrahedron*>(getHostSimplexes<dim>()), angle);
	}

private:
	const nr_float angle;
};

int main(const int argc, const char* argv[])
{
	if (!App::init()) return EXIT_FAILURE;

	auto app = DynamicCube4dApp(40);
	auto ret = app.run();

	App::deinit();

	return ret;
}