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
		Castor3D::GeometryBuffersSPtr CreateGeometryBuffers( Castor3D::eTOPOLOGY p_topology, Castor3D::ShaderProgram const & p_program )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateDepthStencilState
		 */
		Castor3D::DepthStencilStateSPtr CreateDepthStencilState()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateRasteriserState
		 */
		Castor3D::RasteriserStateSPtr CreateRasteriserState()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateBlendState
		 */
		Castor3D::BlendStateSPtr CreateBlendState()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateSampler
		 */
		Castor3D::SamplerSPtr CreateSampler( Castor::String const & p_name )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateShaderProgram
		 */
		Castor3D::ShaderProgramSPtr CreateShaderProgram()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateIndexBuffer
		 */
		std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > CreateIndexBuffer( Castor3D::CpuBuffer< uint32_t > * p_buffer )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateVertexBuffer
		 */
		std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > CreateVertexBuffer( Castor3D::CpuBuffer< uint8_t > * p_buffer )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateTexture
		 */
		Castor3D::TextureLayoutSPtr CreateTexture( Castor3D::TextureType p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateTextureStorage
		 */
		Castor3D::TextureStorageUPtr CreateTextureStorage( Castor3D::TextureStorageType p_type, Castor3D::TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateFrameVariableBuffer
		 */
		Castor3D::FrameVariableBufferSPtr CreateFrameVariableBuffer( Castor::String const & p_name )override;
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
		Castor3D::GpuQuerySPtr CreateQuery( Castor3D::eQUERY_TYPE p_type )override;
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
	};
}

#endif
