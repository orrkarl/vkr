// Some compilers (msvc...) don't include the math constants (e.g. PI) unless this macro is defined
#define _USE_MATH_DEFINES
#include <cmath>

#include "app.h"
#include "linalg.h"
#include "utils.h"

Vector h_cube[]
{
	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f, -1.0f }, 
	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{  1.0f, -1.0f,  1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{  1.0f,  1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{  1.0f, -1.0f,  1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f, -1.0f, -1.0f },
	{  1.0f,  1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{  1.0f,  1.0f,  1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f }
};

nr_float h_near[]
{
    -5, -5, 0.5
};

nr_float h_far[]
{
    5, 5, 10
};

void transform(const Matrix& mat, Triangle simplices[12])
{
	for (auto i = 0; i < 12; ++i)
	{
		simplices[i].points[0] = mat * h_cube[3 * i];
		simplices[i].points[1] = mat * h_cube[3 * i + 1];
		simplices[i].points[2] = mat * h_cube[3 * i + 2];
	}
}

void transform(const nr_float angle, Triangle simplices[12])
{
    Matrix r = Matrix::rotation(Y, Z, angle);
    Matrix t = Matrix::translation(1.5, 1.5, 3);
	transform(t * r, simplices);
}

class RotatingCubeApp : public App
{
public:
	RotatingCubeApp(const nr_uint divisions)
		: App("Rotating Cube", nr::ScreenDimension{ 640, 480 }), divisions(divisions), m_hostVertecies(new Triangle[12])
	{
	}

protected:
	nr_status update(const nr::CommandQueue& queue) override
	{
		const nr_float angle = static_cast<nr_float>(tick * (2 * M_PI) / divisions);

		transform(angle, m_hostVertecies.get());
		nr_status ret = queue.enqueueBufferWriteCommand(m_vertecies, false, 12, m_hostVertecies.get());
		if (nr::error::isFailure(ret)) return ret;
		ret = draw(m_vertecies, nr::Primitive::TRIANGLE, 12);
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

		m_vertecies = nr::VertexBuffer::make(renderContext, 12, ret);
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
	std::unique_ptr<Triangle[]> m_hostVertecies;
};

class StaticCubeApp : public App
{
public:
	StaticCubeApp(const nr_float angle)
		: App("Static Cube", nr::ScreenDimension{ 640, 480 }), angle(angle), m_hostVertecies(new Triangle[12])
	{
	}

	StaticCubeApp(const nr_uint tick, const nr_uint divisions)
		: StaticCubeApp(tick * 2 * M_PI / divisions)

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
			std::cout << "Angle: " << 180 * angle / M_PI << " (deg)" << std::endl;
		}

		return CL_SUCCESS;
	}
	
	nr_status init(const nr::Context& renderContext, nr::Pipeline& pipeline) override
	{
		nr_status ret = CL_SUCCESS;

		m_vertecies = nr::VertexBuffer::make(renderContext, 12, ret);
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
		nr_status ret = queue.enqueueBufferWriteCommand(m_vertecies, false, 12, m_hostVertecies.get());
		if (nr::error::isFailure(ret)) return ret;
		return App::draw(m_vertecies, nr::Primitive::TRIANGLE, 12);
	}

	const nr_float angle;
	bool firstRun = true;
	nr::VertexBuffer m_vertecies;
	std::unique_ptr<Triangle[]> m_hostVertecies;
};

class DynamicCubeApp : public App
{
public:
	DynamicCubeApp()
		: App("Dynamic Cube", nr::ScreenDimension{ 640, 480 }), m_offsetAngle(0), m_offsetX(1.5), m_offsetY(1.5), m_offsetZ(3), m_hostVertecies(new Triangle[12])
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

		m_offsetAngle += DELTA_ANGLE * isKeyPressed(GLFW_KEY_SPACE);

		return draw(queue);
	}

	nr_status init(const nr::Context& renderContext, nr::Pipeline& pipeline) override
	{
		nr_status ret = CL_SUCCESS;

		m_vertecies = nr::VertexBuffer::make(renderContext, 12, ret);
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
	static constexpr nr_float DELTA_X = 0.3f;
	static constexpr nr_float DELTA_Y = 0.3f;
	static constexpr nr_float DELTA_Z = 0.3f;

	nr_status draw(const nr::CommandQueue& queue)
	{
		transform(Matrix::translation(m_offsetX, m_offsetY, m_offsetZ) * Matrix::rotation(X, Y, m_offsetAngle), m_hostVertecies.get());
		auto err = queue.enqueueBufferWriteCommand(m_vertecies, false, 12, m_hostVertecies.get());
		if (nr::error::isFailure(err)) return err;
		return App::draw(m_vertecies, nr::Primitive::TRIANGLE, 12);
	}

	std::unique_ptr<Triangle[]> m_hostVertecies;
	nr_float m_offsetAngle, m_offsetX, m_offsetY, m_offsetZ;
	nr::VertexBuffer m_vertecies;
};

int main(const int argc, const char* argv[])
{
	if (!App::init()) return EXIT_FAILURE;

	auto app = DynamicCubeApp();
	auto ret = app.run();

	App::deinit();

	return ret;
}