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
		DxRenderSystem( Castor3D::Engine * p_engine );
		virtual ~DxRenderSystem();

		void CheckShaderSupport();
		bool InitialiseDevice( HWND p_hWnd, DXGI_SWAP_CHAIN_DESC & p_swapChainDesc );

		virtual bool CheckSupport( Castor3D::eSHADER_MODEL p_eProfile );
		virtual Castor3D::ContextSPtr CreateContext();
		virtual Castor3D::GeometryBuffersSPtr CreateGeometryBuffers( Castor3D::VertexBufferUPtr p_pVertexBuffer, Castor3D::IndexBufferUPtr p_pIndexBuffer, Castor3D::MatrixBufferUPtr p_pMatrixBuffer );
		virtual Castor3D::DepthStencilStateSPtr CreateDepthStencilState();
		virtual Castor3D::RasteriserStateSPtr CreateRasteriserState();
		virtual Castor3D::BlendStateSPtr CreateBlendState();
		virtual Castor3D::SamplerSPtr CreateSampler( Castor::String const & p_name );
		virtual Castor3D::RenderTargetSPtr CreateRenderTarget( Castor3D::eTARGET_TYPE p_type );
		virtual Castor3D::RenderWindowSPtr CreateRenderWindow();
		virtual Castor3D::ShaderProgramBaseSPtr CreateHlslShaderProgram();
		virtual Castor3D::ShaderProgramBaseSPtr CreateShaderProgram();
		virtual Castor3D::OverlayRendererSPtr CreateOverlayRenderer();
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > CreateIndexBuffer( Castor3D::CpuBuffer<uint32_t> * p_pBuffer );
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > CreateVertexBuffer( Castor3D::BufferDeclaration const & p_declaration, Castor3D::CpuBuffer< uint8_t > * p_pBuffer );
		virtual std::shared_ptr< Castor3D::GpuBuffer< real > > CreateMatrixBuffer( Castor3D::CpuBuffer< real > * p_pBuffer );
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > CreateTextureBuffer( Castor3D::CpuBuffer<uint8_t > * p_pBuffer );
		virtual Castor3D::StaticTextureSPtr CreateStaticTexture();
		virtual Castor3D::DynamicTextureSPtr CreateDynamicTexture();
		virtual Castor3D::FrameVariableBufferSPtr CreateFrameVariableBuffer( Castor::String const & p_name );
		virtual Castor3D::BillboardListSPtr CreateBillboardsList( Castor3D::SceneSPtr p_scene );

		virtual Castor3D::ShaderProgramBaseSPtr CreateGlslShaderProgram()
		{
			return nullptr;
		}

		virtual	bool NeedsMatrixTransposition()const
		{
			return true;
		}

		virtual bool SupportsDepthBuffer()const
		{
			return true;
		}

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

	public:
		inline ID3D11Debug * GetDebug()
		{
			return m_pDebug;
		}

		void Track( ID3D11Device * p_object, std::string const & p_name, std::string const & p_file, int line );
		void Track( ID3D11DeviceChild * p_object, std::string const & p_name, std::string const & p_file, int line );
		void Track( IDXGIDeviceSubObject * p_object, std::string const & p_name, std::string const & p_file, int line );
		void Untrack( ID3D11Device * p_object );
		void Untrack( ID3D11DeviceChild * p_object );
		void Untrack( IDXGIDeviceSubObject * p_object );

#endif
	};
}

#endif
