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
#ifndef ___Gl_TextureRenderer___
#define ___Gl_TextureRenderer___

#include "Module_GlRender.hpp"
#include "GlBuffer.hpp"

#include <Castor3D/Sampler.hpp>
#include <Castor3D/Texture.hpp>

namespace GlRender
{
	class GlPixelBuffer : public GlBufferBase< uint8_t >
	{
	protected:
		eGL_BUFFER_TARGET				m_ePackMode;
		Castor3D::eBUFFER_ACCESS_TYPE	m_eAccessType;
		Castor3D::eBUFFER_ACCESS_NATURE m_eAccessNature;
		uint8_t	*						m_pPixels;
		uint32_t						m_uiPixelsSize;
		GlRenderSystem *				m_pRenderSystem;

	public:
		GlPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, uint8_t * p_pPixels, uint32_t p_uiPixelsSize, eGL_BUFFER_TARGET p_ePackMode, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );
		virtual ~GlPixelBuffer();

		virtual bool Activate();
		virtual void Deactivate();
		bool Fill( uint8_t * p_pBuffer, ptrdiff_t p_iSize );

		virtual void Initialise() = 0;
	};

	class GlPackPixelBuffer : public GlPixelBuffer
	{
	public:
		GlPackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, uint8_t * p_pPixels, uint32_t p_uiPixelsSize );
		GlPackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, Castor::PxBufferBaseSPtr p_pPixels );
		virtual ~GlPackPixelBuffer();

		virtual void Initialise();
	};

	class GlUnpackPixelBuffer : public GlPixelBuffer
	{
	public:
		GlUnpackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, uint8_t * p_pPixels, uint32_t p_uiPixelsSize );
		GlUnpackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, Castor::PxBufferBaseSPtr p_pPixels );
		virtual ~GlUnpackPixelBuffer();

		virtual void Initialise();
	};

	class GlSamplerRenderer : public Castor3D::SamplerRenderer, CuNonCopyable
	{
	private:
		typedef std::function< void() > PUnbindFunction;
		typedef std::function< bool( eGL_TEXDIM p_eDimension, uint32_t p_uiIndex ) > PBindFunction;

		PBindFunction	m_pfnBind;
		PUnbindFunction	m_pfnUnbind;
		uint32_t		m_uiSamplerId;
		OpenGl &		m_gl;

	public:
		GlSamplerRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem );
		virtual ~GlSamplerRenderer();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual bool Bind( Castor3D::eTEXTURE_DIMENSION p_eDimension, uint32_t p_uiIndex );
		virtual void Unbind();
	};

	class GlStaticTexture : public Castor3D::StaticTexture, CuNonCopyable
	{
	private:
		uint32_t				m_uiGlName;
		GlPixelBuffer *			m_pLockedIoBuffer;
		GlPixelBuffer *			m_pIoBuffer;
		int						m_iCurrentPbo;
		GlRenderSystem *		m_pGlRenderSystem;
		eGL_TEXTURE_INDEX		m_eIndex;
		eGL_TEXDIM				m_eGlDimension;
		OpenGl &				m_gl;

	public:
		GlStaticTexture( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem );
		~GlStaticTexture();

		virtual bool		Create			();
		virtual void		Destroy			();
		virtual void		Cleanup			();
		virtual uint8_t *	Lock			( uint32_t p_uiLock );
		virtual void		Unlock			( bool p_bModified );
		virtual void		GenerateMipmaps	();

		inline uint32_t GetGlName()const { return m_uiGlName; }

	private:
		virtual bool DoBind();
		virtual void DoUnbind();
		virtual bool DoInitialise();
		void DoDeletePbos();
		void DoCleanupPbos();
		void DoInitialisePbos();
	};

	class GlDynamicTexture : public Castor3D::DynamicTexture, CuNonCopyable
	{
	private:
		uint32_t				m_uiGlName;
		GlPixelBuffer *			m_pLockedIoBuffer;
		GlPixelBuffer *			m_pIoBuffer;
		int						m_iCurrentPbo;
		GlRenderSystem *		m_pGlRenderSystem;
		eGL_TEXTURE_INDEX		m_eIndex;
		eGL_TEXDIM				m_eGlDimension;
		OpenGl &				m_gl;

	public:
		GlDynamicTexture( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem );
		~GlDynamicTexture();

		virtual bool		Create			();
		virtual void		Destroy			();
		virtual void		Cleanup			();
		virtual uint8_t *	Lock			( uint32_t p_uiLock );
		virtual void		Unlock			( bool p_bModified );
		virtual void		GenerateMipmaps	();
		virtual void		Fill			( uint8_t const * p_pBuffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_eFormat );

		inline uint32_t GetGlName()const { return m_uiGlName; }

	private:
		virtual bool DoBind();
		virtual void DoUnbind();
		virtual bool DoInitialise();
		void DoDeletePbos();
		void DoCleanupPbos();
		void DoInitialisePbos();
	};

	class GlTextureRenderer : public Castor3D::TextureRenderer, CuNonCopyable
	{
	private:
		typedef std::function< void() > VoidFunction;
		typedef std::function< bool() > BoolFunction;

		BoolFunction	m_pfnRender;
		VoidFunction	m_pfnEndRender;
		OpenGl &		m_gl;

	public:
		GlTextureRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem );
		virtual ~GlTextureRenderer();

		virtual bool Render();
		virtual void EndRender();
	};
}

#endif
