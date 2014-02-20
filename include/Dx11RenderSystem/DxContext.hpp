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
#ifndef ___Dx11_Context___
#define ___Dx11_Context___

#include "Module_DxRender.hpp"
#include <Castor3D/DepthStencilState.hpp>
#include <Castor3D/RasterState.hpp>
#include <Castor3D/BlendState.hpp>

#if defined( _WIN32 )
namespace Dx11Render
{
	class DxBlendState : public Castor3D::BlendState
	{
	private:
		DxRenderSystem *	m_pRenderSystem;
		ID3D11BlendState *	m_pBlendState;

	public:
		DxBlendState( DxRenderSystem * p_pRenderSystem );
		virtual ~DxBlendState();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual bool Apply();
	};
	class DxRasteriserState : public Castor3D::RasteriserState
	{
	private:
		DxRenderSystem *		m_pRenderSystem;
		ID3D11RasterizerState *	m_pRasteriserState;

	public:
		DxRasteriserState( DxRenderSystem * p_pRenderSystem );
		virtual ~DxRasteriserState();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual bool Apply();
	};
	class DxDepthStencilState : public Castor3D::DepthStencilState
	{
	private:
		DxRenderSystem *			m_pRenderSystem;
		ID3D11DepthStencilState *	m_pDepthStencilState;

	public:
		DxDepthStencilState( DxRenderSystem * p_pRenderSystem );
		virtual ~DxDepthStencilState();

		 virtual bool Initialise();
		 virtual void Cleanup();
		 virtual bool Apply();
	};

	class DxContext : public Castor3D::Context
    {
	private:
		IDXGISwapChain *			m_pSwapChain;
		ID3D11RenderTargetView *	m_pRenderTargetView;
		ID3D11DepthStencilView *	m_pDepthStencilView;
		HWND						m_hWnd;
		Castor::Size				m_size;
		DXGI_SWAP_CHAIN_DESC		m_deviceParams;
		FLOAT						m_fClearColour[4];

    public:
        DxContext();
		virtual ~DxContext();
		
		virtual void UpdateFullScreen	( bool p_bVal );
		ID3D11RenderTargetView *	GetRenderTargetView()const { return m_pRenderTargetView; }
		ID3D11DepthStencilView *	GetDepthStencilView()const { return m_pDepthStencilView; }

	private:
		virtual bool DoInitialise		();
		virtual void DoCleanup			();
		virtual void DoSetCurrent		();
		virtual void DoEndCurrent		();
		virtual void DoSwapBuffers		();
		virtual void DoSetClearColour	( Castor::Colour const & p_clrClear );
		virtual void DoClear			( uint32_t p_uiTargets );
		virtual void DoBind				( Castor3D::eBUFFER p_eBuffer, Castor3D::eFRAMEBUFFER_TARGET p_eTarget );
		virtual void DoSetAlphaFunc		( Castor3D::eALPHA_FUNC p_eFunc, uint8_t p_byValue );
		virtual void DoCullFace			( Castor3D::eFACE p_eCullFace );
		
		void DoInitVolatileResources();
		void DoFreeVolatileResources();
		HRESULT DoInitPresentParameters();
    };
}
#endif

#endif
