#include <chrono>
// Some compilers (msvc...) don't include the math constants (e.g. PI) unless this macro is defined
#define _USE_MATH_DEFINES
#include <cmath>
#include <thread>

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

class CubeApp : public App
{
public:
	CubeApp(const nr::string& name)
		: App("Rotating Cube", nr::ScreenDimension{ 640, 480 }), m_hostVertecies(new Triangle[12])
	{
	}

protected:
	nr_status init(const nr::Context& renderContext, nr::Pipeline& pipeline) override
	{
		nr_status ret = CL_SUCCESS;

		const nr_float zNear = 0.5;
		const nr_float zFar = 20;

		m_vertecies = nr::VertexBuffer::make(renderContext, 12, ret);
		if (nr::error::isFailure(ret)) return ret;

		ret = pipeline.setFieldOfView(2 * M_PI / 4);
		if (nr::error::isFailure(ret)) return ret;

		ret = pipeline.setZNearPlane(zNear);
		if (nr::error::isFailure(ret)) return ret;

		ret = pipeline.setZFarPlane(zFar);
		if (nr::error::isFailure(ret)) return ret;

		pipeline.setClearColor({ 0, 0, 0, 0 });
		pipeline.setClearDepth(1.0f);
		
		return ret;
	}

	nr_status draw(const nr::CommandQueue& queue)
	{
		nr_status ret = queue.enqueueBufferWriteCommand(m_vertecies, false, 12, m_hostVertecies.get());
		if (nr::error::isFailure(ret)) return ret;
		return App::draw(m_vertecies, nr::Primitive::TRIANGLE, 12);
	}

	void transform(const Matrix& mat)
	{
		for (auto i = 0; i < 12; ++i)
		{
			m_hostVertecies[i].points[0] = mat * h_cube[3 * i];
			m_hostVertecies[i].points[1] = mat * h_cube[3 * i + 1];
			m_hostVertecies[i].points[2] = mat * h_cube[3 * i + 2];
		}
	}

	void transform(const nr_float angle)
	{
		Matrix r = Matrix::rotation(Y, Z, angle);
		Matrix t = Matrix::translation(6, 3.6, 7);
		transform(t * r);
	}

	nr::VertexBuffer m_vertecies;
	std::unique_ptr<Triangle[]> m_hostVertecies;
};

class RotatingCubeApp : public CubeApp
{
public:
	RotatingCubeApp(const nr_uint divisions)
		: CubeApp("Rotating Cube"), divisions(divisions)
	{
	}

protected:
	nr_status update(const nr::CommandQueue& queue) override
	{
		const nr_float angle = static_cast<nr_float>(tick * (2 * M_PI) / divisions);

		transform(angle);
		nr_status ret = draw(queue);
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

private:
	nr_uint tick = 0;
	const nr_uint divisions;
};

class StaticCubeApp : public CubeApp
{
public:
	StaticCubeApp(const nr_float angle)
		: CubeApp("Static Cube"), angle(angle)
	{
	}

	StaticCubeApp()
		: StaticCubeApp(0.0f)
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
			transform(angle);
			return draw(queue);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::microseconds(16));
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

private:
	const nr_float angle;
	bool firstRun = true;
};

class DynamicCubeApp : public CubeApp
{
public:
	DynamicCubeApp()
		: CubeApp("Dynamic Cube"), m_offsetAngle(0), m_offsetX(1.5), m_offsetY(1.5), m_offsetZ(3)
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
		
		transform(Matrix::translation(m_offsetX, m_offsetY, m_offsetZ) * Matrix::rotation(X, Y, m_offsetAngle));
		return draw(queue);
	}

private:
	static constexpr nr_float DELTA_ANGLE = 2 * M_PI / 40;
	static constexpr nr_float DELTA_X = 0.3f;
	static constexpr nr_float DELTA_Y = 0.3f;
	static constexpr nr_float DELTA_Z = 0.3f;

	nr_float m_offsetAngle, m_offsetX, m_offsetY, m_offsetZ;
};

int main(const int argc, const char* argv[])
{
	if (!App::init()) return EXIT_FAILURE;

	auto app = DynamicCubeApp();
	auto ret = app.run();

	App::deinit();

	return ret;
}