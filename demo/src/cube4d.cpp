// Some compilers (msvc...) don't include the math constants (e.g. PI) unless this macro is defined
#define _USE_MATH_DEFINES
#include <cmath>

#include "app.h"
#include "linalg.h"
#include "utils.h"

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

void transform(const Matrix& mat, Tetrahedron simplices[48])
{
	Vector cube[16];
	for (auto i = 0; i < 16; ++i)
	{
		cube[i] = mat * h_cube[i];
	}

	cube4dToSimplices(cube, simplices);
}

void transform(const nr_float angle, Tetrahedron simplices[48])
{
    Matrix r = Matrix::rotation(Y, Z, angle);
    Matrix t = Matrix::translation(1.5, 1.5, 3, 3);
	transform(t * r, simplices);
}

class RotatingCube4dApp : public App
{
public:
	RotatingCube4dApp(const nr_uint divisions)
		: App("Rotating Cube4d", nr::ScreenDimension{ 640, 480 }, dim), divisions(divisions), m_hostVertecies(new Tetrahedron[48])
	{
	}

protected:
	nr_status update(const nr::CommandQueue& queue) override
	{
		const nr_float angle = tick * (2 * M_PI) / divisions;

		transform(angle, m_hostVertecies.get());
		nr_status ret = queue.enqueueBufferWriteCommand(m_vertecies, false, 48, m_hostVertecies.get());
		if (nr::error::isFailure(ret)) return ret;
		ret = draw(m_vertecies, nr::Primitive::SIMPLEX, 48);
		if (nr::error::isFailure(ret)) return ret;

		if (isKeyPressed(GLFW_KEY_S))
		{
			std::cout << "Tick: " << tick << '\n';
			std::cout << "Angle: " << angle << " (rad)" << std::endl;
			std::cout << "Angle: " << 180 * angle / nr_float(M_PI) << " (deg)" << std::endl;
		}

		tick = (tick + 1) % divisions;

		return CL_SUCCESS;
	}

	nr_status init(const nr::Context& renderContext, nr::Pipeline& pipeline) override
	{
		nr_status ret = CL_SUCCESS;
		auto pret = &ret;

		m_vertecies = nr::VertexBuffer::make<dim>(renderContext, 48, pret);
		if (nr::error::isFailure(ret)) return ret;

		ret = pipeline.setFarPlane(h_far);
		if (nr::error::isFailure(ret)) return ret;

		ret = pipeline.setNearPlane(h_near);
		if (nr::error::isFailure(ret)) return ret;

		pipeline.setClearColor({ 0, 0, 0, 0 });
		pipeline.setClearDepth(1.0f);

		return ret;
	}

private:
	nr_uint tick = 0;
	const nr_uint divisions;
	nr::VertexBuffer m_vertecies;
	std::unique_ptr<Tetrahedron[]> m_hostVertecies;
};

class StaticCube4dApp : public App
{
public:
	StaticCube4dApp(const nr_float angle)
		: App("Static Cube4d", nr::ScreenDimension{ 640, 480 }, dim), angle(angle), m_hostVertecies(new Tetrahedron[48])
	{
	}

	StaticCube4dApp(const nr_uint tick, const nr_uint divisions)
		: StaticCube4dApp(tick * 2 * M_PI / divisions)
	{
	}

protected:
	nr_status update(const nr::CommandQueue& queue) override
	{
		if (firstRun)
		{
			firstRun = false;
			return draw(queue);
		}

		if (isKeyPressed(GLFW_KEY_R))
		{
			std::cout << "Angle: " << angle << " (rad)" << std::endl;
		}

		if (isKeyPressed(GLFW_KEY_D))
		{
			std::cout << "Angle: " << 180 * angle / nr_float(M_PI) << " (deg)" << std::endl;
		}

		return CL_SUCCESS;
	}
	
	nr_status init(const nr::Context& renderContext, nr::Pipeline& pipeline) override
	{
		nr_status ret = CL_SUCCESS;
		auto pret = &ret;

		m_vertecies = nr::VertexBuffer::make<dim>(renderContext, 48, pret);
		if (nr::error::isFailure(ret)) return ret;

		ret = pipeline.setFarPlane(h_far);
		if (nr::error::isFailure(ret)) return ret;

		ret = pipeline.setNearPlane(h_near);
		if (nr::error::isFailure(ret)) return ret;

		pipeline.setClearColor({ 0, 0, 0, 0 });
		pipeline.setClearDepth(1.0f);

		return ret;
	}

private:
	nr_status draw(const nr::CommandQueue& queue)
	{
		transform(angle, m_hostVertecies.get());
		nr_status ret = queue.enqueueBufferWriteCommand(m_vertecies, false, 48, m_hostVertecies.get());
		if (nr::error::isFailure(ret)) return ret;
		return App::draw(m_vertecies, nr::Primitive::SIMPLEX, 48);
	}

	const nr_float angle;
	bool firstRun = true;
	nr::VertexBuffer m_vertecies;
	std::unique_ptr<Tetrahedron[]> m_hostVertecies;
};

class DynamicCube4dApp : public App
{
public:
	DynamicCube4dApp()
		: App("Dynamic Cube4d", nr::ScreenDimension{ 640, 480 }, dim), m_offsetAngle(0), m_offsetW(3), m_offsetX(1.5), m_offsetY(1.5), m_offsetZ(3), m_hostVertecies(new Tetrahedron[48])
	{
	}

protected:
	nr_status update(const nr::CommandQueue& queue) override
	{
		m_offsetX += DELTA_X * isKeyPressed(GLFW_KEY_D);
		m_offsetX -= DELTA_X * isKeyPressed(GLFW_KEY_A);

		m_offsetY += DELTA_Y * isKeyPressed(GLFW_KEY_W);
		m_offsetY -= DELTA_Y * isKeyPressed(GLFW_KEY_S);

		m_offsetZ += DELTA_Z * isKeyPressed(GLFW_KEY_DOWN);
		m_offsetZ -= DELTA_Z * isKeyPressed(GLFW_KEY_UP);

		m_offsetW += DELTA_W * isKeyPressed(GLFW_KEY_RIGHT);
		m_offsetW -= DELTA_W * isKeyPressed(GLFW_KEY_LEFT);

		m_offsetAngle += DELTA_ANGLE * isKeyPressed(GLFW_KEY_SPACE);

		return draw(queue);
	}

	nr_status init(const nr::Context& renderContext, nr::Pipeline& pipeline) override
	{
		nr_status ret = CL_SUCCESS;
		auto pret = &ret;

		m_vertecies = nr::VertexBuffer::make<dim>(renderContext, 48, pret);
		if (nr::error::isFailure(ret)) return ret;

		ret = pipeline.setFarPlane(h_far);
		if (nr::error::isFailure(ret)) return ret;

		ret = pipeline.setNearPlane(h_near);
		if (nr::error::isFailure(ret)) return ret;

		pipeline.setClearColor({ 0, 0, 0, 0 });
		pipeline.setClearDepth(1.0f);

		return ret;
	}
private:
	static constexpr nr_float DELTA_ANGLE = 2 * M_PI / 40;
	static constexpr nr_float DELTA_X = 0.3;
	static constexpr nr_float DELTA_Y = 0.3;
	static constexpr nr_float DELTA_Z = 0.3;
	static constexpr nr_float DELTA_W = 0.3;

	nr_status draw(const nr::CommandQueue& queue)
	{
		transform(Matrix::translation(m_offsetX, m_offsetY, m_offsetZ, m_offsetW) * Matrix::rotation(X, Y, m_offsetAngle), m_hostVertecies.get());
		auto err = queue.enqueueBufferWriteCommand(m_vertecies, false, 48, m_hostVertecies.get());
		if (nr::error::isFailure(err)) return err;
		return App::draw(m_vertecies, nr::Primitive::SIMPLEX, 48);
	}

	std::unique_ptr<Tetrahedron[]> m_hostVertecies;
	nr_float m_offsetAngle, m_offsetW, m_offsetX, m_offsetY, m_offsetZ;
	nr::VertexBuffer m_vertecies;
};

int main(const int argc, const char* argv[])
{
	if (!App::init()) return EXIT_FAILURE;

	auto app = DynamicCube4dApp();
	auto ret = app.run();

	App::deinit();

	return ret;
}