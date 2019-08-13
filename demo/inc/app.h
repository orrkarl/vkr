#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

#include <base/CommandQueue.h>
#include <base/Context.h>

#include <general/predefs.h>

#include <pipeline/Pipeline.h>

#include <rendering/Render.h>

class App
{
public:
	static void deinit();

	static bool init();

	App(const nr::string& name, const nr::ScreenDimension& screenDim, const nr_uint renderDimension);

	nr_status run();

protected:
	virtual nr_status update(const nr::CommandQueue& queue, nr::Pipeline& pipeline) = 0;

	virtual nr_status init(const nr::Context& renderContext, nr::Pipeline& pipeline) = 0;

	nr_status draw(const nr::VertexBuffer& vb, const nr::Primitive& type, const nr_uint primitiveCount);

	GLFWwindow* getWindow();

private:
	void destroy();
	bool initRenderingPipeline();
	bool initialize();
	bool initGL();
	void loop(cl_status* err);

	std::unique_ptr<nr::RawColorRGBA[]>	m_bitmap; 
	nr::CommandQueue					m_commandQueue;
	nr::Context							m_context;
	const nr::string					m_name;
	const nr_uint						m_renderDimension;
	nr::ScreenDimension					m_screenDim;
	nr::Pipeline						m_pipeline;
	GLFWwindow*							m_window;
};