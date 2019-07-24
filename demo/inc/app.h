#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

#include <base/CommandQueue.h>
#include <base/Context.h>

#include <pipeline/BinRasterizer.h>
#include <pipeline/FineRasterizer.h>
#include <pipeline/SimplexReducer.h>
#include <pipeline/VertexShader.h>

#include <general/predefs.h>

#include <rendering/Render.h>

class App
{
public:
	static void deinit();

	static bool init();

	App(const nr::string& name, const nr::ScreenDimension& dim, const nr_uint renderDimension, const nr_uint simplexCount);

	nr_int run();

protected:
	virtual void update() = 0;
	
	template <nr_uint dim>
	nr::Simplex<dim>* getHostSimplexes()
	{
		return (nr::Simplex<dim>*) m_h_simplexes.get();
	}

	nr_uint getSimplexCount();

	void setNearPlane(const nr_float* near);

	void setFarPlane(const nr_float* far);

private:
	void clearBuffers(cl_status* err);
	void destroy();
	void draw(cl_status* err);
	bool initCL();
	bool initialize();
	bool initGL();
	bool initRenderingPipeline();
	void loadData(cl_status* err);
	void loop(cl_status* err);

	nr::__internal::BinRasterizer m_binRasterizer;
	std::array<nr_size, 2>		  m_binRasterizerGlobalSize;
	std::array<nr_size, 2>		  m_binRasterizerLocalSize;

	nr::__internal::FineRasterizer m_fineRasterizer;
	std::array<nr_size, 2>		   m_fineRasterizerGlobalSize;
	std::array<nr_size, 2>		   m_fineRasterizerLocalSize;

	nr::__internal::SimplexReducer m_simplexReducer;
	std::array<nr_size, 1>		   m_simplexReducerGlobalSize;
	std::array<nr_size, 1>		   m_simplexReducerLocalSize;

	nr::__internal::VertexShader m_vertexShader;
	std::array<nr_size, 1>		 m_vertexShaderGlobalSize;
	std::array<nr_size, 1>		 m_vertexShaderLocalSize;

	std::unique_ptr<nr::RawColorRGBA[]>	m_bitmap; 
	nr::CommandQueue					m_commandQueue;
	nr::Context							m_context;
	nr::Device							m_device;
	std::unique_ptr<nr_float[]>			m_far;
	const nr::string					m_name;
	std::unique_ptr<nr_float[]>			m_near;
	const nr_uint						m_renderDimension;
	nr::ScreenDimension					m_screenDim;
	const nr_uint						m_simplexCount;
	std::unique_ptr<nr_float[]>			m_h_simplexes;
	GLFWwindow*							m_window;
};