/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
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
