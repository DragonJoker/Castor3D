#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlTextureRenderer.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/GlBuffer.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

GlTextureRenderer :: GlTextureRenderer()
	:	TextureRenderer	()
	,	m_glTexName		( GL_INVALID_INDEX)
	,	m_pfnCleanup	( nullptr)
	,	m_pfnInitialise	( nullptr)
	,	m_pfnRender		( nullptr)
	,	m_pfnEndRender	( nullptr)
	,	m_pPixelBuffer	( NULL)
{
}

GlTextureRenderer :: ~GlTextureRenderer()
{
	Cleanup();
}


void GlTextureRenderer :: Cleanup()
{
	if (m_pfnCleanup != NULL)
	{
		(this->*m_pfnCleanup)();
	}
}

bool GlTextureRenderer :: Initialise()
{
	CASTOR_TRACK;
	if (GlRenderSystem::GetOpenGlMajor() < 3)
	{
		m_pfnCleanup	= & GlTextureRenderer::_cleanup;
		m_pfnInitialise	= & GlTextureRenderer::_initialise;
		m_pfnRender		= & GlTextureRenderer::_render;
		m_pfnEndRender	= & GlTextureRenderer::_endRender;
	}
#if CASTOR_USE_OPENGL4
	else if (GlRenderSystem::GetOpenGlMajor() < 4)
#else
	else
#endif
	{
		m_pfnCleanup	= PVoidFunction( & GlTextureRenderer::_cleanupGl3);
		m_pfnInitialise	= PBoolFunction( & GlTextureRenderer::_initialiseGl3);
		m_pfnRender		= PVoidFunction( & GlTextureRenderer::_renderGl3);
		m_pfnEndRender	= PVoidFunction( & GlTextureRenderer::_endRenderGl3);
	}
#if CASTOR_USE_OPENGL4
	else
	{
		m_pfnCleanup	= PVoidFunction( & GlTextureRenderer::_cleanupGl4);
		m_pfnInitialise	= PBoolFunction( & GlTextureRenderer::_initialiseGl4);
		m_pfnRender		= PVoidFunction( & GlTextureRenderer::_renderGl4);
		m_pfnEndRender	= PVoidFunction( & GlTextureRenderer::_endRenderGl4);
	}
#endif
	return (this->*m_pfnInitialise)();
}

void GlTextureRenderer :: Render()
{
	CASTOR_TRACK;
	(this->*m_pfnRender)();
}

void GlTextureRenderer :: EndRender()
{
	CASTOR_TRACK;
	(this->*m_pfnEndRender)();
}

void GlTextureRenderer :: UploadImage()
{
	if (m_pPixelBuffer != NULL)
	{
		void * l_pData = LockImage( GL_WRITE_ONLY);
		memcpy( l_pData, m_target->GetImagePixels(), m_target->GetImageSize());
		UnlockImage( true);
	}
}

void GlTextureRenderer :: DownloadImage()
{
	if (m_pPixelBuffer != NULL)
	{
		void * l_pData = LockImage( GL_READ_ONLY);
		memcpy( m_target->GetImagePixels(), l_pData, m_target->GetImageSize());
		UnlockImage( false);
	}
}

void * GlTextureRenderer :: LockImage( GLenum p_mode)
{
	void * l_pReturn = NULL;

	if (m_pPixelBuffer != NULL)
	{
		m_pPixelBuffer->Activate();
		m_pPixelBuffer->Fill( NULL, m_target->GetImageSize());
		l_pReturn = m_pPixelBuffer->Lock( p_mode);
	}

	return l_pReturn;
}

void GlTextureRenderer :: UnlockImage( bool p_bModified)
{
	if (m_pPixelBuffer != NULL)
	{
		m_pPixelBuffer->Unlock();

		if (p_bModified)
		{
			OpenGl::TexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, m_target->GetWidth(), m_target->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, 0);
		}

		m_pPixelBuffer->Deactivate();
	}
}

void  GlTextureRenderer :: _cleanup()
{
	CASTOR_TRACK;
	if (OpenGl::IsTexture( m_glTexName))
	{
		OpenGl::DeleteTextures( 1, & m_glTexName);
	}

	_deletePbos();
}

bool GlTextureRenderer :: _initialise()
{
	CASTOR_TRACK;
	_cleanupPbos();
	_deletePbos();

	m_pPixelBuffer = new GlUnpackPixelBuffer( m_target->GetImagePixels(), m_target->GetImageSize());
	m_pPixelBuffer->Initialise();

	bool l_bReturn = OpenGl::GenTextures( 1, & m_glTexName);

	if (m_glTexName == 0)
	{
		Logger::LogError( cuT( "GlTextureRenderer :: Initialise - Not a valid texture name"));
		l_bReturn = false;
	}
	else
	{
		int l_textureType = GlEnum::Get( m_target->GetDimension());
		GlEnum::GlPixelFmt l_glPixelFmt = GlEnum::Get( m_target->GetPixelFormat());

		l_bReturn &= OpenGl::BindTexture( l_textureType, m_glTexName);
		l_bReturn &= OpenGl::TexParameteri( l_textureType, GL_TEXTURE_WRAP_S, GlEnum::Get( m_target->GetWrapMode( TextureUnit::eUVW_U)));
		l_bReturn &= OpenGl::TexParameteri( l_textureType, GL_TEXTURE_WRAP_T, GlEnum::Get( m_target->GetWrapMode( TextureUnit::eUVW_V)));
		l_bReturn &= OpenGl::TexParameteri( l_textureType, GL_TEXTURE_MIN_FILTER, GlEnum::Get( m_target->GetInterpolationMode( TextureUnit::eINTERPOLATION_FILTER_MIN)));
		l_bReturn &= OpenGl::TexParameteri( l_textureType, GL_TEXTURE_MAG_FILTER, GlEnum::Get( m_target->GetInterpolationMode( TextureUnit::eINTERPOLATION_FILTER_MAG)));
		l_bReturn &= OpenGl::TexImage2D( l_textureType, 0, l_glPixelFmt.Internal, m_target->GetWidth(), m_target->GetHeight(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_target->GetImagePixels());
		l_bReturn &= OpenGl::GenerateMipmap( l_textureType);
	}

	return l_bReturn;
}

void GlTextureRenderer :: _render()
{
	CASTOR_TRACK;
	bool l_bReturn = true;
	l_bReturn &= OpenGl::ActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
	l_bReturn &= OpenGl::BindTexture( GlEnum::Get( m_target->GetDimension()), m_glTexName);
	l_bReturn &= OpenGl::TexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, m_target->GetPrimaryColour().const_ptr());

	if (RenderSystem::UseMultiTexturing())
	{
		if (m_target->GetRgbFunction() != TextureUnit::eRGB_BLEND_FUNC_NONE || m_target->GetAlpFunction() != TextureUnit::eALPHA_BLEND_FUNC_NONE)
		{
			l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

			if (m_target->GetRgbFunction() != TextureUnit::eRGB_BLEND_FUNC_NONE)
			{
				l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GlEnum::Get( m_target->GetRgbFunction()));
				l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, GlEnum::Get( m_target->GetRgbArgument( TextureUnit::eBLEND_SRC_INDEX_0)));
				l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);

				if (m_target->GetRgbFunction() != TextureUnit::eRGB_BLEND_FUNC_FIRST_ARG)
				{
					l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB, GlEnum::Get( m_target->GetRgbArgument( TextureUnit::eBLEND_SRC_INDEX_1)));
        			l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
				}

				if (m_target->GetRgbFunction() == TextureUnit::eRGB_BLEND_FUNC_INTERPOLATE)
				{
					l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_SOURCE2_RGB, GlEnum::Get( m_target->GetRgbArgument( TextureUnit::eBLEND_SRC_INDEX_2)));
					l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
				}
			}

			if (m_target->GetAlpFunction() != TextureUnit::eALPHA_BLEND_FUNC_NONE)
			{
				l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GlEnum::Get( m_target->GetAlpFunction()));
				l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GlEnum::Get( m_target->GetAlpArgument( TextureUnit::eBLEND_SRC_INDEX_0)));
				l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

				if (m_target->GetAlpFunction() != TextureUnit::eALPHA_BLEND_FUNC_FIRST_ARG)
				{
					l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GlEnum::Get( m_target->GetAlpArgument( TextureUnit::eBLEND_SRC_INDEX_1)));
					l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
				}

				if (m_target->GetAlpFunction() == TextureUnit::eALPHA_BLEND_FUNC_INTERPOLATE)
				{
					l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GlEnum::Get( m_target->GetAlpArgument( TextureUnit::eBLEND_SRC_INDEX_2)));
					l_bReturn &= OpenGl::TexEnvi( GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, GL_SRC_ALPHA);
				}
			}
		}
	}

	l_bReturn &= OpenGl::Enable( GlEnum::Get( m_target->GetDimension()));

	if (m_target->GetTextureMapMode() != TextureUnit::eMAP_MODE_NONE)
	{
		if (m_target->GetTextureMapMode() == TextureUnit::eMAP_MODE_REFLEXION)
		{
			l_bReturn &= OpenGl::TexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			l_bReturn &= OpenGl::TexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		}
		else if (m_target->GetTextureMapMode() == TextureUnit::eMAP_MODE_SPHERE)
		{
			l_bReturn &= OpenGl::TexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			l_bReturn &= OpenGl::TexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		}

		l_bReturn &= OpenGl::Enable( GL_TEXTURE_GEN_S);
		l_bReturn &= OpenGl::Enable( GL_TEXTURE_GEN_T);
	}

	if (m_target->GetAlphaFunc() != TextureUnit::eALPHA_FUNC_ALWAYS)
	{
		l_bReturn &= OpenGl::Enable( GL_ALPHA_TEST);
		l_bReturn &= OpenGl::AlphaFunc( GlEnum::Get( m_target->GetAlphaFunc()), m_target->GetAlphaValue());
	}

	Pipeline::MatrixMode( Pipeline::eMODE( Pipeline::eMODE_TEXTURE0 + m_target->GetIndex()));
	Pipeline::LoadIdentity();
	//TODO : rotations de texture
}

void GlTextureRenderer :: _endRender()
{
	CASTOR_TRACK;
	Pipeline::MatrixMode( Pipeline::eMODE_MODELVIEW);
	bool l_bReturn = true;

	if (m_target->GetAlphaFunc() != TextureUnit::eALPHA_FUNC_ALWAYS)
	{
		l_bReturn &= OpenGl::Disable( GL_ALPHA_TEST);
	}

	if (RenderSystem::UseMultiTexturing())
	{
		l_bReturn &= OpenGl::ActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
	}

	l_bReturn &= OpenGl::Disable( GlEnum::Get( m_target->GetDimension()));

	if (m_target->GetTextureMapMode() != TextureUnit::eMAP_MODE_NONE)
	{
		l_bReturn &= OpenGl::Disable( GL_TEXTURE_GEN_S);
		l_bReturn &= OpenGl::Disable( GL_TEXTURE_GEN_T);
	}
}


void  GlTextureRenderer :: _cleanupGl3()
{
	CASTOR_TRACK;
	if (glIsTexture( m_glTexName))
	{
		OpenGl::DeleteTextures( 1, & m_glTexName);
	}

	_deletePbos();
}

bool GlTextureRenderer :: _initialiseGl3()
{
	CASTOR_TRACK;
	bool l_bReturn = true;
	l_bReturn &= OpenGl::GenTextures( 1, & m_glTexName);

	if (m_glTexName == 0)
	{
		Logger::LogError( cuT( "GlTextureRenderer :: Initialise - Not a valid texture name"));
		return false;
	}

	_cleanupPbos();
	_deletePbos();

	m_pPixelBuffer = new GlUnpackPixelBuffer( m_target->GetImagePixels(), m_target->GetImageSize());
	m_pPixelBuffer->Initialise();

	int l_textureType = GlEnum::Get( m_target->GetDimension());
	GlEnum::GlPixelFmt l_glPixelFmt = GlEnum::Get( m_target->GetPixelFormat());

	l_bReturn &= OpenGl::BindTexture( l_textureType, m_glTexName);
	l_bReturn &= OpenGl::TexParameteri( l_textureType, GL_TEXTURE_WRAP_S, GlEnum::Get( m_target->GetWrapMode( TextureUnit::eUVW_U)));
	l_bReturn &= OpenGl::TexParameteri( l_textureType, GL_TEXTURE_WRAP_T, GlEnum::Get( m_target->GetWrapMode( TextureUnit::eUVW_V)));
	l_bReturn &= OpenGl::TexParameteri( l_textureType, GL_TEXTURE_MIN_FILTER, GlEnum::Get( m_target->GetInterpolationMode( TextureUnit::eINTERPOLATION_FILTER_MIN)));
	l_bReturn &= OpenGl::TexParameteri( l_textureType, GL_TEXTURE_MAG_FILTER, GlEnum::Get( m_target->GetInterpolationMode( TextureUnit::eINTERPOLATION_FILTER_MAG)));
	l_bReturn &= OpenGl::TexImage2D(	l_textureType, 0, l_glPixelFmt.Internal, m_target->GetWidth(), m_target->GetHeight(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_target->GetImagePixels());
	l_bReturn &= OpenGl::GenerateMipmap( l_textureType);
	return true;
}

void GlTextureRenderer :: _renderGl3()
{
	CASTOR_TRACK;
	bool l_bReturn = true;

	if (RenderSystem::UseMultiTexturing())
	{
		l_bReturn &= OpenGl::ActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
		l_bReturn &= OpenGl::BindTexture( GlEnum::Get( m_target->GetDimension()), m_glTexName);
	}
	else
	{
		l_bReturn &= OpenGl::BindTexture( GlEnum::Get( m_target->GetDimension()), m_glTexName);
	}

	Pipeline::MatrixMode( Pipeline::eMODE( Pipeline::eMODE_TEXTURE0 + m_target->GetIndex()));
	Pipeline::LoadIdentity();
	//TODO : transformations de texture
}

void GlTextureRenderer :: _endRenderGl3()
{
	CASTOR_TRACK;
	Pipeline::MatrixMode( Pipeline::eMODE_MODELVIEW);

	if (m_target->IsTextured() && m_target->TextureInitialised())
	{
		if (RenderSystem::UseMultiTexturing())
		{
			OpenGl::ActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
		}
	}
}
#if CASTOR_USE_OPENGL4
void  GlTextureRenderer :: _cleanupGl4()
{
	CASTOR_TRACK;
}

bool GlTextureRenderer :: _initialiseGl4()
{
	CASTOR_TRACK;
	m_pTextureBufferObject = Root::GetSingletonPtr()->GetBufferManager()->CreateBuffer<unsigned char, GlTextureBufferObject>();
	bool l_bReturn = m_pTextureBufferObject->Initialise( ePIXEL_FORMAT_A8R8G8B8, m_target->GetWidth() * m_target->GetHeight(), m_target->GetImagePixels());

	if (m_pTextureBufferObject->GetIndex() != GL_INVALID_INDEX)
	{
		l_bReturn &= OpenGl::GenTextures( 1, & m_glTexName);

		if (m_glTexName != GL_INVALID_INDEX)
		{
			l_bReturn &= OpenGl::BindTexture( GL_TEXTURE_BUFFER, m_glTexName);
			l_bReturn &= OpenGl::TexBuffer( GL_TEXTURE_BUFFER, GL_RGBA8UI, m_pTextureBufferObject->GetIndex());
			l_bReturn &= OpenGl::BindTexture(GL_TEXTURE_BUFFER, 0);
		}
	}

	return m_glTexName != GL_INVALID_INDEX;
}

void GlTextureRenderer :: _renderGl4()
{
	CASTOR_TRACK;
	bool l_bReturn = true;
	l_bReturn &= OpenGl::ActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
	l_bReturn &= OpenGl::BindTexture( GL_TEXTURE_BUFFER, m_glTexName);

	Pipeline::MatrixMode( Pipeline::eMODE( Pipeline::eMODE_TEXTURE0 + m_target->GetIndex()));
	Pipeline::LoadIdentity();
	//TODO : rotations de texture
}

void GlTextureRenderer :: _endRenderGl4()
{
	CASTOR_TRACK;
	Pipeline::MatrixMode( Pipeline::eMODE_MODELVIEW);
}
#endif


void GlTextureRenderer :: _deletePbos()
{
	if (m_pPixelBuffer != NULL)
	{
		delete m_pPixelBuffer;
		m_pPixelBuffer = NULL;
	}
}

void GlTextureRenderer :: _cleanupPbos()
{
	if (m_pPixelBuffer != NULL)
	{
		m_pPixelBuffer->Cleanup();
	}
}