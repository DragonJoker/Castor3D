/* See LICENSE file in root folder */
#ifndef ___TRS_RenderSystem_H___
#define ___TRS_RenderSystem_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Render/RenderSystem.hpp>

namespace TestRender
{
	class TestRenderSystem
		: public castor3d::RenderSystem
	{
	public:
		explicit TestRenderSystem( castor3d::Engine & engine );
		virtual ~TestRenderSystem();

		static castor3d::RenderSystemUPtr create( castor3d::Engine & engine );
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
		 *\copydoc		castor3d::RenderSystem::CreatePipeline
		 */
		castor3d::RenderPipelineUPtr createRenderPipeline( castor3d::DepthStencilState && p_dsState
			, castor3d::RasteriserState && p_rsState
			, castor3d::BlendState && p_bdState
			, castor3d::MultisampleState && p_msState
			, castor3d::ShaderProgram & p_program
			, castor3d::PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		castor3d::RenderSystem::CreatePipeline
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
		castor3d::UniformBufferBindingUPtr createUniformBufferBinding( castor3d::UniformBuffer & p_ubo
			, castor3d::ShaderProgram const & p_program )override;
		/**
		 *\copydoc		castor3d::RenderSystem::createShaderProgram
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
		castor3d::GpuBufferSPtr doCreateBuffer( castor3d::BufferType p_type )override;

	public:
		C3D_Test_API static castor::String Name;
		C3D_Test_API static castor::String Type;
	};
}

#endif
