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
#ifndef ___Dx10_RenderSystem___
#define ___Dx10_RenderSystem___

#include "Module_DxRender.hpp"
#include "DxPipeline.hpp"
#include "DxBuffer.hpp"
#include "DxContext.hpp"
#include "DxShaderProgram.hpp"

namespace Dx10Render
{
	class DxRenderSystem : public Castor3D::RenderSystem
	{
	protected:
		std::set< int >							m_setAvailableIndexes;
		std::set< Castor3D::LightRendererSPtr >	m_setLightRenderers;
		IDXGIFactory *							m_pFactory;
		ID3D10Device *							m_pDevice;
#if !defined( NDEBUG )
		ID3D10Debug *							m_pDebug;
#endif

	public:
		DxRenderSystem( Castor3D::Engine * p_pEngine, Castor::Logger * p_pLogger );
		virtual ~DxRenderSystem();

		virtual void								Delete						();
		virtual bool								CheckSupport				( Castor3D::eSHADER_MODEL p_eProfile );
		virtual int									LockLight					();
		virtual void								UnlockLight					( int p_iIndex);
		virtual void								BeginOverlaysRendering		();
		virtual void								EndOverlaysRendering		();
		virtual Castor3D::ContextSPtr				CreateContext				();
		virtual Castor3D::GeometryBuffersSPtr		CreateGeometryBuffers		( Castor3D::VertexBufferSPtr p_pVertexBuffer, Castor3D::IndexBufferSPtr p_pIndexBuffer, Castor3D::MatrixBufferSPtr p_pMatrixBuffer );
		virtual Castor3D::DepthStencilStateSPtr		CreateDepthStencilState		();
		virtual Castor3D::RasteriserStateSPtr		CreateRasteriserState		();
		virtual Castor3D::BlendStateSPtr			CreateBlendState			();
		void										CheckShaderSupport			();
		virtual	bool								NeedsMatrixTransposition	()const { return false; }
		virtual bool								SupportsDepthBuffer			()const { return true; }
		virtual Castor3D::FrameVariableBufferSPtr	CreateFrameVariableBuffer	( Castor::String const & p_strName );
		virtual Castor3D::BillboardListSPtr			CreateBillboardsList		( Castor3D::SceneRPtr p_pScene );

		void										Resize						( Castor::Point2i const & p_ptNewSize);
		bool										InitialiseDevice			( HWND p_hWnd, DXGI_SWAP_CHAIN_DESC & p_swapChainDesc );

		inline IDXGIFactory *						GetDXGIFactory				()const { return m_pFactory; }
		inline ID3D10Device *						GetDevice					()		{ return m_pDevice; }
#if !defined( NDEBUG )
		inline ID3D10Debug *						GetDebug					()		{ return m_pDebug; }
#endif

	private:
		virtual void DoInitialise();
		virtual void DoCleanup();
		virtual void DoRenderAmbientLight( Castor::Colour const & p_clColour );
		virtual Castor3D::ShaderProgramBaseSPtr DoCreateGlslShaderProgram() { return nullptr; }
		virtual Castor3D::ShaderProgramBaseSPtr DoCreateHlslShaderProgram();
		virtual Castor3D::ShaderProgramBaseSPtr DoCreateShaderProgram();
		virtual Castor3D::SubmeshRendererSPtr DoCreateSubmeshRenderer();
		virtual Castor3D::TextureRendererSPtr DoCreateTextureRenderer();
		virtual Castor3D::PassRendererSPtr DoCreatePassRenderer();
		virtual Castor3D::CameraRendererSPtr DoCreateCameraRenderer();
		virtual Castor3D::LightRendererSPtr DoCreateLightRenderer();
		virtual Castor3D::WindowRendererSPtr DoCreateWindowRenderer();
		virtual Castor3D::OverlayRendererSPtr DoCreateOverlayRenderer();
		virtual Castor3D::TargetRendererSPtr DoCreateTargetRenderer();
		virtual Castor3D::SamplerRendererSPtr DoCreateSamplerRenderer();
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > DoCreateIndexBuffer( std::shared_ptr< Castor3D::CpuBuffer<uint32_t> > p_pBuffer);
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > DoCreateVertexBuffer( Castor3D::BufferDeclaration const & p_declaration, std::shared_ptr< Castor3D::CpuBuffer< uint8_t > > p_pBuffer);
		virtual std::shared_ptr< Castor3D::GpuBuffer< real > > DoCreateMatrixBuffer( std::shared_ptr< Castor3D::CpuBuffer<real> > p_pBuffer);
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > DoCreateTextureBuffer( std::shared_ptr< Castor3D::CpuBuffer<uint8_t> > p_pBuffer);
		virtual Castor3D::StaticTextureSPtr DoCreateStaticTexture();
		virtual Castor3D::DynamicTextureSPtr DoCreateDynamicTexture();
	};
}

#endif
