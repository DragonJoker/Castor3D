#ifndef ___GlFrameBufferObject___
#define ___GlFrameBufferObject___

#include "Module_GlRender.hpp"

#include <Castor3D/FrameBuffer.hpp>

namespace GlRender
{
	class GlRenderBuffer : CuNonCopyable
	{
	private:
		uint32_t					m_uiGlName;
		eGL_RENDERBUFFER_STORAGE	m_eInternal;
		Castor::Size				m_size;
		Castor3D::RenderBuffer &	m_renderBuffer;
		OpenGl &					m_gl;

	public:
		GlRenderBuffer( OpenGl & p_gl, eGL_RENDERBUFFER_STORAGE p_eInternal, Castor3D::RenderBuffer & p_renderBuffer );
		~GlRenderBuffer();

		bool Create();
		void Destroy();

		bool Initialise( Castor::Size const & p_size );
		void Cleanup();

		bool Bind();
		void Unbind();

		bool Resize( Castor::Size const & p_size );

		inline uint32_t				GetGlName		()const { return m_uiGlName; }
		inline uint32_t				GetInternal		()const { return m_eInternal; }
	};

	class GlColourRenderBuffer : public Castor3D::ColourRenderBuffer
	{
	private:
		GlRenderBuffer	m_glRenderBuffer;

	public:
		GlColourRenderBuffer( OpenGl & p_gl, Castor::ePIXEL_FORMAT p_eFormat );
		virtual ~GlColourRenderBuffer();

		virtual bool Create		();
		virtual void Destroy	();
		virtual bool Initialise	( Castor::Size const & p_size );
		virtual void Cleanup	();
		virtual bool Bind		();
		virtual void Unbind		();
		virtual bool Resize		( Castor::Size const & p_size );

		inline uint32_t	GetGlName()const { return m_glRenderBuffer.GetGlName(); }
	};

	class GlDepthRenderBuffer : public Castor3D::DepthRenderBuffer
	{
	private:
		GlRenderBuffer	m_glRenderBuffer;

	public:
		GlDepthRenderBuffer( OpenGl & p_gl, Castor::ePIXEL_FORMAT p_eFormat );
		virtual ~GlDepthRenderBuffer();

		virtual bool Create		();
		virtual void Destroy	();
		virtual bool Initialise	( Castor::Size const & p_size );
		virtual void Cleanup	();
		virtual bool Bind		();
		virtual void Unbind		();
		virtual bool Resize		( Castor::Size const & p_size );

		inline uint32_t	GetGlName()const { return m_glRenderBuffer.GetGlName(); }
	};

	class GlStencilRenderBuffer : public Castor3D::StencilRenderBuffer
	{
	private:
		GlRenderBuffer	m_glRenderBuffer;

	public:
		GlStencilRenderBuffer( OpenGl & p_gl, Castor::ePIXEL_FORMAT p_eFormat );
		virtual ~GlStencilRenderBuffer();

		virtual bool Create		();
		virtual void Destroy	();
		virtual bool Initialise	( Castor::Size const & p_size );
		virtual void Cleanup	();
		virtual bool Bind		();
		virtual void Unbind		();
		virtual bool Resize		( Castor::Size const & p_size );

		inline uint32_t	GetGlName()const { return m_glRenderBuffer.GetGlName(); }
	};

	class GlRenderBufferAttachment : public Castor3D::RenderBufferAttachment, CuNonCopyable
	{
	private:
		eGL_RENDERBUFFER_ATTACHMENT	m_eGlAttachmentPoint;
		eGL_FRAMEBUFFER_STATUS		m_eGlStatus;
		GlFrameBufferWPtr			m_pGlFrameBuffer;
		OpenGl &					m_gl;

	public:
		GlRenderBufferAttachment( OpenGl & p_gl, Castor3D::RenderBufferSPtr p_pRenderBuffer );
		virtual ~GlRenderBufferAttachment();

		virtual bool	DownloadBuffer	( Castor::PxBufferBaseSPtr p_pBuffer );
		virtual bool	Blit			( Castor3D::FrameBufferSPtr p_pBuffer, Castor::Rect const & p_rectSrc, Castor::Rect const & p_rectDst, Castor3D::eINTERPOLATION_MODE p_eInterpolation );

		inline eGL_FRAMEBUFFER_STATUS		GetGlStatus				()const	{ return m_eGlStatus; }
		inline eGL_RENDERBUFFER_ATTACHMENT	GetGlAttachmentPoint	()const	{ return m_eGlAttachmentPoint; }

	private:
		virtual bool DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::FrameBufferSPtr p_pFrameBuffer );
		virtual void DoDetach();
	};

	class GlTextureAttachment : public Castor3D::TextureAttachment, CuNonCopyable
	{
	private:
		eGL_TEXTURE_ATTACHMENT	m_eGlAttachmentPoint;
		eGL_FRAMEBUFFER_STATUS	m_eGlStatus;
		GlFrameBufferWPtr		m_pGlFrameBuffer;
		OpenGl &				m_gl;

	public:
		GlTextureAttachment( OpenGl & p_gl, Castor3D::DynamicTextureSPtr p_pTexture );
		virtual ~GlTextureAttachment();

		virtual bool	DownloadBuffer	( Castor::PxBufferBaseSPtr p_pBuffer );
		virtual bool	Blit			( Castor3D::FrameBufferSPtr p_pBuffer, Castor::Rect const & p_rectSrc, Castor::Rect const & p_rectDst, Castor3D::eINTERPOLATION_MODE p_eInterpolation );

		inline eGL_FRAMEBUFFER_STATUS	GetGlStatus				()const	{ return m_eGlStatus; }
		inline eGL_TEXTURE_ATTACHMENT	GetGlAttachmentPoint	()const	{ return m_eGlAttachmentPoint; }

	private:
		virtual bool DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::FrameBufferSPtr p_pFrameBuffer );
		virtual void DoDetach();
	};

	DECLARE_VECTOR( uint32_t, UInt );

	class GlFrameBuffer : public Castor3D::FrameBuffer, CuNonCopyable
	{
	private:
		uint32_t				m_uiGlName;
		eGL_FRAMEBUFFER_MODE	m_eGlBindingMode;
		Castor3D::UIntArray		m_arrayGlAttaches;
		OpenGl &				m_gl;

	public:
		GlFrameBuffer( OpenGl & p_gl, Castor3D::Engine * p_pEngine );
		virtual ~GlFrameBuffer();

		virtual bool	Create			( int p_iSamplesCount );
		virtual void	Destroy			();
		virtual bool	SetDrawBuffers	( uint32_t p_uiSize, Castor3D::eATTACHMENT_POINT const * p_eAttaches );
		virtual bool	SetDrawBuffers	();
		virtual bool	SetReadBuffer	( Castor3D::eATTACHMENT_POINT p_eAttach );
		virtual bool	IsComplete		();
		
		virtual Castor3D::ColourRenderBufferSPtr	CreateColourRenderBuffer	( Castor::ePIXEL_FORMAT p_ePixelFormat );
		virtual Castor3D::DepthRenderBufferSPtr		CreateDepthRenderBuffer		( Castor::ePIXEL_FORMAT p_ePixelFormat );
		virtual Castor3D::StencilRenderBufferSPtr	CreateStencilRenderBuffer	( Castor::ePIXEL_FORMAT p_ePixelFormat );

		inline uint32_t	GetGlName()const { return m_uiGlName; }

	private:
		virtual bool DoBind			( Castor3D::eFRAMEBUFFER_TARGET p_eTarget );
		virtual void DoUnbind		();
		virtual void DoAttach		( Castor3D::TextureAttachmentRPtr p_pAttach ) { return DoAttachFba( p_pAttach ); }
		virtual void DoDetach		( Castor3D::TextureAttachmentRPtr p_pAttach ) { return DoDetachFba( p_pAttach ); }
		virtual void DoAttach		( Castor3D::RenderBufferAttachmentRPtr p_pAttach ) { return DoAttachFba( p_pAttach ); }
		virtual void DoDetach		( Castor3D::RenderBufferAttachmentRPtr p_pAttach ) { return DoDetachFba( p_pAttach ); }
		virtual bool DoAttach		( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::DynamicTextureSPtr p_pTexture, Castor3D::eTEXTURE_TARGET p_eTarget, int p_iLayer=0 );
		virtual bool DoAttach		( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::RenderBufferSPtr p_pRenderBuffer );
		virtual void DoDetachAll	();
		virtual bool DoStretchInto	( Castor3D::FrameBufferSPtr p_pBuffer, Castor::Rect const & p_rectSrc, Castor::Rect const & p_rectDst, uint32_t p_uiComponents, Castor3D::eINTERPOLATION_MODE p_eInterpolationMode );
		virtual void DoGlAttach		( Castor3D::eATTACHMENT_POINT p_eAttachment );
		virtual void DoGlDetach		( Castor3D::eATTACHMENT_POINT p_eAttachment );

		void DoAttachFba( Castor3D::FrameBufferAttachmentRPtr p_pAttach );
		void DoDetachFba( Castor3D::FrameBufferAttachmentRPtr p_pAttach );
	};
}

#endif
