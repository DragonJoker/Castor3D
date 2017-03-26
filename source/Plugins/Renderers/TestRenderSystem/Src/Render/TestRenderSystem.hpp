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
#ifndef ___TRS_RenderSystem_H___
#define ___TRS_RenderSystem_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Render/RenderSystem.hpp>

namespace TestRender
{
	class TestRenderSystem
		: public Castor3D::RenderSystem
	{
	public:
		TestRenderSystem( Castor3D::Engine & p_engine );
		virtual ~TestRenderSystem();

		static Castor3D::RenderSystemUPtr Create( Castor3D::Engine & p_engine );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateContext
		 */
		Castor3D::ContextSPtr CreateContext()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateGeometryBuffers
		 */
		Castor3D::GeometryBuffersSPtr CreateGeometryBuffers( Castor3D::Topology p_topology
			, Castor3D::ShaderProgram const & p_program )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreatePipeline
		 */
		Castor3D::RenderPipelineUPtr CreateRenderPipeline( Castor3D::DepthStencilState && p_dsState
			, Castor3D::RasteriserState && p_rsState
			, Castor3D::BlendState && p_bdState
			, Castor3D::MultisampleState && p_msState
			, Castor3D::ShaderProgram & p_program
			, Castor3D::PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreatePipeline
		 */
		Castor3D::ComputePipelineUPtr CreateComputePipeline( Castor3D::ShaderProgram & p_program )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateSampler
		 */
		Castor3D::SamplerSPtr CreateSampler( Castor::String const & p_name )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateShaderProgram
		 */
		Castor3D::ShaderProgramSPtr CreateShaderProgram()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateUniformBufferBinding
		 */
		Castor3D::UniformBufferBindingUPtr CreateUniformBufferBinding( Castor3D::UniformBuffer & p_ubo
			, Castor3D::ShaderProgram const & p_program )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateShaderProgram
		 */
		Castor3D::TransformFeedbackUPtr CreateTransformFeedback( Castor3D::BufferDeclaration const & p_computed
			, Castor3D::Topology p_topology
			, Castor3D::ShaderProgram & p_program )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateUInt8Buffer
		 */
		std::unique_ptr< Castor3D::GpuBuffer< uint8_t > > CreateUInt8Buffer( Castor3D::BufferType p_type )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateUInt16Buffer
		 */
		std::unique_ptr< Castor3D::GpuBuffer< uint16_t > > CreateUInt16Buffer( Castor3D::BufferType p_type )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateUInt32Buffer
		 */
		std::unique_ptr< Castor3D::GpuBuffer< uint32_t > > CreateUInt32Buffer( Castor3D::BufferType p_type )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateFloatBuffer
		 */
		std::unique_ptr< Castor3D::GpuBuffer< float > > CreateFloatBuffer( Castor3D::BufferType p_type )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateTexture
		 */
		Castor3D::TextureLayoutSPtr CreateTexture( Castor3D::TextureType p_type
			, Castor3D::AccessTypes const & p_cpuAccess
			, Castor3D::AccessTypes const & p_gpuAccess )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateTexture
		 */
		Castor3D::TextureLayoutSPtr CreateTexture( Castor3D::TextureType p_type
			, Castor3D::AccessTypes const & p_cpuAccess
			, Castor3D::AccessTypes const & p_gpuAccess
			, Castor::PixelFormat p_format
			, Castor::Size const & p_size )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateTexture
		 */
		Castor3D::TextureLayoutSPtr CreateTexture( Castor3D::TextureType p_type
			, Castor3D::AccessTypes const & p_cpuAccess
			, Castor3D::AccessTypes const & p_gpuAccess
			, Castor::PixelFormat p_format
			, Castor::Point3ui const & p_size )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateTextureStorage
		 */
		Castor3D::TextureStorageUPtr CreateTextureStorage( Castor3D::TextureStorageType p_type
			, Castor3D::TextureLayout & p_layout
			, Castor3D::AccessTypes const & p_cpuAccess
			, Castor3D::AccessTypes const & p_gpuAccess )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateFrameBuffer
		 */
		Castor3D::FrameBufferSPtr CreateFrameBuffer()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateBackBuffers
		 */
		Castor3D::BackBuffersSPtr CreateBackBuffers()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateQuery
		 */
		Castor3D::GpuQueryUPtr CreateQuery( Castor3D::QueryType p_type )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateViewport
		 */
		Castor3D::IViewportImplUPtr CreateViewport( Castor3D::Viewport & p_viewport )override;

	private:
		/**
		 *\copydoc		Castor3D::RenderSystem::DoInitialise
		 */
		void DoInitialise()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::DoCleanup
		 */
		void DoCleanup()override;

	public:
		C3D_Test_API static Castor::String Name;
		C3D_Test_API static Castor::String Type;
	};
}

#endif
