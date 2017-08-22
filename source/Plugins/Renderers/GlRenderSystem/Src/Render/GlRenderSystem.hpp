/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GL_RENDER_SYSTEM_H___
#define ___GL_RENDER_SYSTEM_H___

#include "GlRenderSystemPrerequisites.hpp"
#include "Render/GlRenderPipeline.hpp"
#include "Render/GlContext.hpp"

#include <Render/RenderSystem.hpp>

namespace GlRender
{
	class GlRenderSystem
		: public castor3d::RenderSystem
	{
	public:
		GlRenderSystem( castor3d::Engine & engine );
		virtual ~GlRenderSystem();

		static castor3d::RenderSystemUPtr create( castor3d::Engine & engine );

		//!< Initialize OpenGL Extensions
		bool InitOpenGlExtensions();
		/**
		 *\copydoc		castor3d::RenderSystem::createContext
		 */
		castor3d::ContextSPtr createContext()override;
		/**
		 *\copydoc		castor3d::RenderSystem::createGeometryBuffers
		 */
		castor3d::GeometryBuffersSPtr createGeometryBuffers( castor3d::Topology p_topology
			, castor3d::ShaderProgram const & p_program )override;
		/**
		 *\copydoc		castor3d::RenderSystem::createRenderPipeline
		 */
		castor3d::RenderPipelineUPtr createRenderPipeline( castor3d::DepthStencilState && p_dsState
			, castor3d::RasteriserState && p_rsState
			, castor3d::BlendState && p_bdState
			, castor3d::MultisampleState && p_msState
			, castor3d::ShaderProgram & p_program
			, castor3d::PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		castor3d::RenderSystem::createComputePipeline
		 */
		castor3d::ComputePipelineUPtr createComputePipeline( castor3d::ShaderProgram & p_program )override;
		/**
		 *\copydoc		castor3d::RenderSystem::createSampler
		 */
		castor3d::SamplerSPtr createSampler( castor::String const & p_name )override;
		/**
		 *\copydoc		castor3d::RenderSystem::createShaderProgram
		 */
		castor3d::ShaderProgramSPtr createShaderProgram()override;
		/**
		 *\copydoc		castor3d::RenderSystem::createUniformBufferBinding
		 */
		castor3d::UniformBufferBindingUPtr createUniformBufferBinding(castor3d::UniformBuffer & p_ubo
			, castor3d::ShaderProgram const & p_program )override;
		/**
		 *\copydoc		castor3d::RenderSystem::createTransformFeedback
		 */
		castor3d::TransformFeedbackUPtr createTransformFeedback( castor3d::BufferDeclaration const & p_computed
			, castor3d::Topology p_topology
			, castor3d::ShaderProgram & p_program )override;
		/**
		 *\copydoc		castor3d::RenderSystem::createTexture
		 */
		castor3d::TextureLayoutSPtr createTexture( castor3d::TextureType p_type
			, castor3d::AccessTypes const & p_cpuAccess
			, castor3d::AccessTypes const & p_gpuAccess )override;
		/**
		*\copydoc		castor3d::RenderSystem::createTexture
		*/
		castor3d::TextureLayoutSPtr createTexture( castor3d::TextureType p_type
			, castor3d::AccessTypes const & p_cpuAccess
			, castor3d::AccessTypes const & p_gpuAccess
			, castor::PixelFormat p_format
			, castor::Size const & p_size )override;
		/**
		 *\copydoc		castor3d::RenderSystem::createTexture
		 */
		castor3d::TextureLayoutSPtr createTexture( castor3d::TextureType p_type
			, castor3d::AccessTypes const & p_cpuAccess
			, castor3d::AccessTypes const & p_gpuAccess
			, castor::PixelFormat p_format
			, castor::Point3ui const & p_size )override;
		/**
		 *\copydoc		castor3d::RenderSystem::createTextureStorage
		 */
		castor3d::TextureStorageUPtr createTextureStorage( castor3d::TextureStorageType p_type
			, castor3d::TextureLayout & p_layout
			, castor3d::AccessTypes const & p_cpuAccess
			, castor3d::AccessTypes const & p_gpuAccess )override;
		/**
		 *\copydoc		castor3d::RenderSystem::createFrameBuffer
		 */
		castor3d::FrameBufferSPtr createFrameBuffer()override;
		/**
		 *\copydoc		castor3d::RenderSystem::createBackBuffers
		 */
		castor3d::BackBuffersSPtr createBackBuffers()override;
		/**
		 *\copydoc		castor3d::RenderSystem::createQuery
		 */
		castor3d::GpuQueryUPtr createQuery( castor3d::QueryType p_type )override;
		/**
		 *\copydoc		castor3d::RenderSystem::createViewport
		 */
		castor3d::IViewportImplUPtr createViewport( castor3d::Viewport & p_viewport )override;

		inline bool IsExtensionInit()
		{
			return m_extensionsInit;
		}

		inline int getOpenGlMajor()
		{
			return m_openGlMajor;
		}

		inline int getOpenGlMinor()
		{
			return m_openGlMinor;
		}

		inline void setOpenGlVersion( int p_iMajor, int p_iMinor )
		{
			m_openGlMajor = p_iMajor;
			m_openGlMinor = p_iMinor;
		}

		inline OpenGl & getOpenGl()
		{
			return m_openGl;
		}

	private:
		/**
		 *\copydoc		castor3d::RenderSystem::doInitialise
		 */
		void doInitialise()override;
		/**
		 *\copydoc		castor3d::RenderSystem::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::RenderSystem::doCreateBuffer
		 */
		castor3d::GpuBufferUPtr doCreateBuffer( castor3d::BufferType p_type )override;

	public:
		C3D_Gl_API static castor::String Name;
		C3D_Gl_API static castor::String Type;

	protected:
		bool m_useVertexBufferObjects;
		bool m_extensionsInit;
		int m_openGlMajor;
		int m_openGlMinor;
		OpenGl m_openGl;

#if C3D_TRACE_OBJECTS

	public:
		bool track( void * p_object, std::string const & p_name, std::string const & p_file, int line );
		bool untrack( void * p_object );

#else

	public:
		bool track( void * p_object, std::string const & p_name, std::string const & p_file, int line )
		{
		}
		bool untrack( void * p_object )
		{
		}

#endif
	};
}

#endif
