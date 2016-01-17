/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___DX11_RENDER_SYSTEM_H___
#define ___DX11_RENDER_SYSTEM_H___

#include "Dx11RenderSystemPrerequisites.hpp"
#include "Dx11Pipeline.hpp"
#include "Dx11Context.hpp"
#include "Dx11ShaderProgram.hpp"

#include <RenderSystem.hpp>

namespace Dx11Render
{
	class DxRenderSystem
		:	public Castor3D::RenderSystem
	{
	public:
		DxRenderSystem( Castor3D::Engine & p_engine );
		virtual ~DxRenderSystem();

		bool InitialiseDevice( HWND p_hWnd, DXGI_SWAP_CHAIN_DESC & p_swapChainDesc );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateContext
		 */
		virtual Castor3D::ContextSPtr CreateContext();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateGeometryBuffers
		 */
		virtual Castor3D::GeometryBuffersSPtr CreateGeometryBuffers( Castor3D::VertexBufferUPtr p_pVertexBuffer, Castor3D::IndexBufferUPtr p_pIndexBuffer, Castor3D::MatrixBufferUPtr p_pMatrixBuffer, Castor3D::eTOPOLOGY p_topology );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateDepthStencilState
		 */
		virtual Castor3D::DepthStencilStateSPtr CreateDepthStencilState();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateRasteriserState
		 */
		virtual Castor3D::RasteriserStateSPtr CreateRasteriserState();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateBlendState
		 */
		virtual Castor3D::BlendStateSPtr CreateBlendState();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateSampler
		 */
		virtual Castor3D::SamplerSPtr CreateSampler( Castor::String const & p_name );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateShaderProgram
		 */
		virtual Castor3D::ShaderProgramBaseSPtr CreateShaderProgram();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateOverlayRenderer
		 */
		virtual Castor3D::OverlayRendererSPtr CreateOverlayRenderer();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateIndexBuffer
		 */
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > CreateIndexBuffer( Castor3D::CpuBuffer<uint32_t> * p_buffer );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateVertexBuffer
		 */
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > CreateVertexBuffer( Castor3D::BufferDeclaration const & p_declaration, Castor3D::CpuBuffer< uint8_t > * p_buffer );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateMatrixBuffer
		 */
		virtual std::shared_ptr< Castor3D::GpuBuffer< real > > CreateMatrixBuffer( Castor3D::CpuBuffer< real > * p_buffer );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateStaticTexture
		 */
		virtual Castor3D::StaticTextureSPtr CreateStaticTexture();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateDynamicTexture
		 */
		virtual Castor3D::DynamicTextureSPtr CreateDynamicTexture( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateFrameVariableBuffer
		 */
		virtual Castor3D::FrameVariableBufferSPtr CreateFrameVariableBuffer( Castor::String const & p_name );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateBillboardsList
		 */
		virtual Castor3D::BillboardListSPtr CreateBillboardsList( Castor3D::SceneSPtr p_scene );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateFrameBuffer
		 */
		virtual Castor3D::FrameBufferSPtr CreateFrameBuffer();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateBackBuffers
		 */
		virtual Castor3D::BackBuffersSPtr CreateBackBuffers();
		/**
		 *\copydoc		Castor3D::RenderSystem::NeedsMatrixTransposition
		 */
		virtual	bool NeedsMatrixTransposition()const
		{
			return true;
		}
		/**
		 *\copydoc		Castor3D::RenderSystem::SupportsDepthBuffer
		 */
		virtual bool SupportsDepthBuffer()const
		{
			return true;
		}
		/**
		 *\copydoc		Castor3D::RenderSystem::GetPipelineImpl
		 */
		virtual Castor3D::IPipelineImplSPtr GetPipelineImpl()
		{
			return m_pipelineImpl;
		}

		inline ID3D11Device * GetDevice()
		{
			return m_pDevice;
		}

		inline D3D_FEATURE_LEVEL GetFeatureLevel()const
		{
			return m_featureLevel;
		}

	private:
		virtual void DoInitialise();
		virtual void DoCleanup();

	protected:
		ID3D11Device * m_pDevice;
		D3D_FEATURE_LEVEL m_featureLevel;
		DXGI_ADAPTER_DESC m_adapterDesc;
		std::shared_ptr< DxPipelineImpl > m_pipelineImpl;

#if !defined( NDEBUG )

		ID3D11Debug * m_pDebug;

#endif
#if C3D_TRACE_OBJECTS

	public:
		template< typename T >
		void Track( T * p_object, std::string const & p_type, std::string const & p_file, int p_line )
		{
			std::string l_name;

			if ( DoTrack( p_object, p_type, p_file, p_line, l_name ) )
			{
				p_object->SetPrivateData( WKPDID_D3DDebugObjectName, UINT( l_name.size() - 1 ), l_name.c_str() );
			}
		}

		template< typename T >
		void Untrack( T * p_object )
		{
			ObjectDeclaration l_declaration;

			if ( DoUntrack( p_object, l_declaration ) )
			{
			}
		}

#else

	public:
		template< typename T >
		void Track( T * p_object, std::string const & p_name, std::string const & p_file, int line )
		{
		}

		template< typename T >
		void Untrack( T * p_object )
		{
		}

#endif
	};
}

#endif
