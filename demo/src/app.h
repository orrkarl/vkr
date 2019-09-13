#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

#include <NR/nr.h>

class App
{
public:
	static void deinit();

	static bool init();

	App(const nr::string& name, const nr::ScreenDimension& screenDim);

	nr_status run();

protected:
	virtual nr_status update(const nr::CommandQueue& queue) = 0;

	virtual nr_status init(const nr::Context& renderContext, nr::Pipeline& pipeline) = 0;

	nr_status draw(const nr::VertexBuffer& vb, const nr::Primitive& type, const nr_uint primitiveCount);

	bool isKeyPressed(int key);

	const nr::ScreenDimension& getScreenDimension() const;

private:
	void destroy();
	bool initRenderingPipeline();
	bool initialize();
	bool initGL();
	void loop(cl_status& err);

	std::unique_ptr<nr::RawColorRGBA[]>	m_bitmap; 
	nr::CommandQueue					m_commandQueue;
	nr::Context							m_context;
	const nr::string					m_name;
	nr::ScreenDimension					m_screenDim;
	nr::Pipeline						m_pipeline;
	GLFWwindow*							m_window;
};