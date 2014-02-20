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
#ifndef ___C3DDX9_FrameBuffer___
#define ___C3DDX9_FrameBuffer___

#include "Module_DxRender.hpp"

#if defined( _WIN32 )
namespace Dx11Render
{
	class DxRenderBuffer : CuNonCopyable
	{
	private:
		Castor::Size				m_size;
		ID3D11View *				m_pSurface;
		ID3D11View *				m_pOldSurface;
		DxRenderSystem *			m_pRenderSystem;
		DXGI_FORMAT					m_eFormat;
		Castor3D::eBUFFER_COMPONENT	m_eComponent;
		Castor3D::RenderBuffer &	m_renderBuffer;
		ID3D11Texture2D *			m_pTexture;

	public:
		DxRenderBuffer( DxRenderSystem * p_pRenderSystem, DXGI_FORMAT p_eFormat, Castor3D::eBUFFER_COMPONENT p_eComponent, Castor3D::RenderBuffer & p_renderBuffer );
		~DxRenderBuffer();

		bool Create();
		void Destroy();

		bool Initialise( Castor::Size const & p_size );
		void Cleanup();

		bool Bind();
		void Unbind();

		bool Resize( Castor::Size const & p_size );
		
		inline ID3D11View * GetSurface()const { return m_pSurface; }
	};

	class DxColourRenderBuffer : public Castor3D::ColourRenderBuffer
	{
	private:
		DxRenderBuffer	m_dxRenderBuffer;

	public:
		DxColourRenderBuffer( DxRenderSystem * p_pRenderSystem, Castor::ePIXEL_FORMAT p_eFormat );
		virtual ~DxColourRenderBuffer();

		virtual bool Create		();
		virtual void Destroy	();
		virtual bool Initialise	( Castor::Size const & p_size );
		virtual void Cleanup	();
		virtual bool Bind		();
		virtual void Unbind		();
		virtual bool Resize		( Castor::Size const & p_size );

		DxRenderBuffer & GetDxRenderBuffer() { return m_dxRenderBuffer; }
	};

	class DxDepthStencilRenderBuffer : public Castor3D::DepthRenderBuffer
	{
	private:
		DxRenderBuffer	m_dxRenderBuffer;

	public:
		DxDepthStencilRenderBuffer( DxRenderSystem * p_pRenderSystem, Castor::ePIXEL_FORMAT p_eFormat );
		virtual ~DxDepthStencilRenderBuffer();

		virtual bool Create		();
		virtual void Destroy	();
		virtual bool Initialise	( Castor::Size const & p_size );
		virtual void Cleanup	();
		virtual bool Bind		();
		virtual void Unbind		();
		virtual bool Resize		( Castor::Size const & p_size );

		DxRenderBuffer & GetDxRenderBuffer() { return m_dxRenderBuffer; }
	};

	class DxRenderBufferAttachment : public Castor3D::RenderBufferAttachment, CuNonCopyable
	{
	private:
		DWORD				m_dwAttachment;
		DxFrameBufferWPtr	m_pFrameBuffer;
		DxRenderBuffer &	m_dxRenderBuffer;
		DxRenderSystem *	m_pRenderSystem;

	public:
		DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxColourRenderBufferSPtr p_pRenderBuffer );
		DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxDepthStencilRenderBufferSPtr p_pRenderBuffer );
		virtual ~DxRenderBufferAttachment();

		virtual bool	DownloadBuffer	( Castor::PxBufferBaseSPtr p_pBuffer );
		virtual bool	Blit			( Castor3D::FrameBufferSPtr p_pBuffer, Castor::Rect const & p_rectSrc, Castor::Rect const & p_rectDst, Castor3D::eINTERPOLATION_MODE p_eInterpolation );
		
		ID3D11View * GetSurface()const;

		bool	Bind	();
		void	Unbind	();

	private:
		virtual bool DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::FrameBufferSPtr p_pFrameBuffer );
		virtual void DoDetach();
	};

	class DxTextureAttachment : public Castor3D::TextureAttachment
	{
	private:
		DWORD						m_dwAttachment;
		DxFrameBufferWPtr			m_pFrameBuffer;
		ID3D11RenderTargetView *	m_pOldSurface;
		DxRenderSystem *			m_pRenderSystem;
		DxDynamicTextureWPtr		m_pDxTexture;

	public:
		DxTextureAttachment( DxRenderSystem * p_pRenderSystem, Castor3D::DynamicTextureSPtr p_pTexture );
		virtual ~DxTextureAttachment();

		virtual bool	DownloadBuffer	( Castor::PxBufferBaseSPtr p_pBuffer );
		virtual bool	Blit			( Castor3D::FrameBufferSPtr p_pBuffer, Castor::Rect const & p_rectSrc, Castor::Rect const & p_rectDst, Castor3D::eINTERPOLATION_MODE p_eInterpolation );

		ID3D11View * GetSurface()const;

		bool	Bind	();
		void	Unbind	();

	private:
		virtual bool DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::FrameBufferSPtr p_pFrameBuffer );
		virtual void DoDetach();
	};
	
	class DxFrameBuffer : public Castor3D::FrameBuffer
	{
	private:
		typedef std::pair< ID3D11View *, ID3D11View * > SrcDstPair;
		DECLARE_VECTOR( SrcDstPair, SrcDstPair );
		DECLARE_MAP( Castor3D::eATTACHMENT_POINT,	DxDynamicTextureSPtr,	DxTexturePtrAttach	);
		DECLARE_MAP( Castor3D::eATTACHMENT_POINT,	DxRenderBufferSPtr,		DxBufferPtrAttach	);
		DECLARE_MAP( DxFrameBufferSPtr,				SrcDstPairArray,		BlitScheme			);
		DECLARE_VECTOR( ID3D11RenderTargetView *, D3D11RenderTargetView );

		DxRenderSystem *			m_pRenderSystem;
		D3D11RenderTargetViewArray	m_arrayOldRenderTargets;
		ID3D11DepthStencilView *	m_pOldDepthStencilView;

	public:
		DxFrameBuffer( DxRenderSystem * p_pRenderSystem );
		virtual ~DxFrameBuffer();

		virtual bool	Create			( int p_iSamplesCount );
		virtual void	Destroy			();
		virtual bool	SetDrawBuffers	( uint32_t p_uiSize, Castor3D::eATTACHMENT_POINT const * p_eAttaches );
		virtual bool	SetDrawBuffers	();
		virtual bool	SetReadBuffer	( Castor3D::eATTACHMENT_POINT ) { return true; }
		virtual bool	IsComplete		() { return true; }

		virtual Castor3D::ColourRenderBufferSPtr	CreateColourRenderBuffer	( Castor::ePIXEL_FORMAT p_ePixelFormat );
		virtual Castor3D::DepthRenderBufferSPtr		CreateDepthRenderBuffer		( Castor::ePIXEL_FORMAT p_ePixelFormat );
		virtual Castor3D::StencilRenderBufferSPtr	CreateStencilRenderBuffer	( Castor::ePIXEL_FORMAT p_ePixelFormat );

		ID3D11View * GetSurface( Castor3D::eATTACHMENT_POINT );

	private:
		virtual bool DoBind			( Castor3D::eFRAMEBUFFER_TARGET p_eTarget );
		virtual void DoUnbind		();
		virtual void DoAttach		( Castor3D::TextureAttachmentRPtr p_pAttach ) { return DoAttachFba( p_pAttach ); }
		virtual void DoDetach		( Castor3D::TextureAttachmentRPtr p_pAttach ) { return DoDetachFba( p_pAttach ); }
		virtual void DoAttach		( Castor3D::RenderBufferAttachmentRPtr p_pAttach ) { return DoAttachFba( p_pAttach ); }
		virtual void DoDetach		( Castor3D::RenderBufferAttachmentRPtr p_pAttach ) { return DoDetachFba( p_pAttach ); }
		virtual void DoAttachFba	( Castor3D::FrameBufferAttachmentRPtr p_pAttach );
		virtual void DoDetachFba	( Castor3D::FrameBufferAttachmentRPtr p_pAttach );
		virtual bool DoAttach		( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::DynamicTextureSPtr p_pTexture, Castor3D::eTEXTURE_TARGET p_eTarget, int p_iLayer=0 );
		virtual bool DoAttach		( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::RenderBufferSPtr p_pRenderBuffer );
		virtual void DoDetachAll	();
		virtual bool DoStretchInto	( Castor3D::FrameBufferSPtr p_pBuffer, Castor::Rect const & p_rectSrc, Castor::Rect const & p_rectDst, uint32_t p_uiComponents, Castor3D::eINTERPOLATION_MODE p_eInterpolation );
	};
}
#endif

#endif
