#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlTextureRenderer.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/GlBuffer.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

GlPixelBuffer :: GlPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, uint8_t * p_pPixels, uint32_t p_uiPixelsSize, eGL_BUFFER_TARGET p_ePackMode, eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature )
	:	GlBufferBase< uint8_t >	( p_gl, p_ePackMode		)
	,	m_uiPixelsSize			( p_uiPixelsSize		)
	,	m_pPixels				( p_pPixels				)
	,	m_ePackMode				( p_ePackMode			)
	,	m_eAccessType			( p_eType				)
	,	m_eAccessNature			( p_eNature				)
	,	m_pRenderSystem			( p_pRenderSystem		)
{
}

GlPixelBuffer :: ~GlPixelBuffer()
{
}

bool GlPixelBuffer :: Activate()
{
	return GlBufferBase::Bind();
}

void GlPixelBuffer :: Deactivate()
{
	GlBufferBase::Unbind();
}

bool GlPixelBuffer :: Fill( uint8_t * p_pBuffer, ptrdiff_t p_iSize)
{
	return GlBufferBase::Fill( p_pBuffer, p_iSize, m_eAccessType, m_eAccessNature );
}

//*************************************************************************************************

GlPackPixelBuffer :: GlPackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, uint8_t * p_pPixels, uint32_t p_uiPixelsSize)
	:	GlPixelBuffer	( p_gl, p_pRenderSystem, p_pPixels, p_uiPixelsSize, eGL_BUFFER_TARGET_PIXEL_PACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_READ	)
{
}

GlPackPixelBuffer :: GlPackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, PxBufferBaseSPtr p_pPixels)
	:	GlPixelBuffer	( p_gl, p_pRenderSystem, p_pPixels->ptr(), p_pPixels->size(), eGL_BUFFER_TARGET_PIXEL_PACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_READ	)
{
}

GlPackPixelBuffer :: ~GlPackPixelBuffer()
{
}

void GlPackPixelBuffer :: Initialise()
{
	Create();

	if( Bind() )
	{
		Fill( m_pPixels, m_uiPixelsSize );
		Unbind();
	}
}

//*************************************************************************************************

GlUnpackPixelBuffer :: GlUnpackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, uint8_t * p_pPixels, uint32_t p_uiPixelsSize)
	:	GlPixelBuffer	( p_gl, p_pRenderSystem, p_pPixels, p_uiPixelsSize, eGL_BUFFER_TARGET_PIXEL_UNPACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW	)
{
}

GlUnpackPixelBuffer :: GlUnpackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, PxBufferBaseSPtr p_pPixels)
	:	GlPixelBuffer	( p_gl, p_pRenderSystem, p_pPixels->ptr(), p_pPixels->size(), eGL_BUFFER_TARGET_PIXEL_UNPACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW	)
{
}

GlUnpackPixelBuffer :: ~GlUnpackPixelBuffer()
{
}

void GlUnpackPixelBuffer :: Initialise()
{
	Create();

	if( Bind() )
	{
		Fill( NULL, m_uiPixelsSize );
		Unbind();
	}
}

//*************************************************************************************************

GlSamplerRenderer :: GlSamplerRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	SamplerRenderer	( p_pRenderSystem	)
	,	m_uiSamplerId	( eGL_INVALID_INDEX	)
	,	m_gl			( p_gl				)
{
}

GlSamplerRenderer :: ~GlSamplerRenderer()
{
}

bool GlSamplerRenderer :: Initialise()
{
	if( !m_pfnBind )
	{
		if( m_gl.HasSpl() && m_uiSamplerId == eGL_INVALID_INDEX )
		{
			m_gl.GenSamplers( 1, &m_uiSamplerId );
		}

		if( m_uiSamplerId != eGL_INVALID_INDEX )
		{
			eGL_INTERPOLATION_MODE l_eMinMode = m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN	) );

			if( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIP ) != eINTERPOLATION_MODE_UNDEFINED  )
			{
				eGL_INTERPOLATION_MODE l_eMipMode = m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIP	) );

				if( l_eMinMode == eGL_INTERPOLATION_MODE_LINEAR )
				{
					if( l_eMipMode == eGL_INTERPOLATION_MODE_LINEAR )
					{
						l_eMinMode = eGL_INTERPOLATION_MODE_LINEAR_MIPMAP_LINEAR;
					}
					else
					{
						l_eMinMode = eGL_INTERPOLATION_MODE_LINEAR_MIPMAP_NEAREST;
					}
				}
				else
				{
					if( l_eMipMode == eGL_INTERPOLATION_MODE_LINEAR )
					{
						l_eMinMode = eGL_INTERPOLATION_MODE_NEAREST_MIPMAP_LINEAR;
					}
					else
					{
						l_eMinMode = eGL_INTERPOLATION_MODE_NEAREST_MIPMAP_NEAREST;
					}
				}
			}

			m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MINLOD,			float( m_target->GetMinLod() )											);
			m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MAXLOD,			float( m_target->GetMaxLod() )											);
			m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_LODBIAS,			float( m_target->GetLodBias() )											);
			m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_UWRAP,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_U					) )	);
			m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_VWRAP,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_V					) )	);
			m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_WWRAP,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_W					) )	);
			m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MINFILTER,		l_eMinMode																);
			m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MAGFILTER,		m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG	) )	);
			m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_BORDERCOLOUR,	m_target->GetBorderColour().const_ptr()									);
			m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MAXANISOTROPY,	float( m_target->GetMaxAnisotropy() )									);

			m_pfnBind = PBindFunction( [&]( eGL_TEXDIM CU_PARAM_UNUSED( p_eDimension ), uint32_t p_uiIndex )
			{
				return m_gl.BindSampler( p_uiIndex, m_uiSamplerId );
			} );
			m_pfnUnbind = PUnbindFunction( [&](){} );
		}
		else
		{
			m_pfnBind = PBindFunction( [&]( eGL_TEXDIM p_eDimension, uint32_t CU_PARAM_UNUSED( p_uiIndex ) )
			{
				bool l_bReturn = true;
				eGL_INTERPOLATION_MODE l_eMinMode = m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN	) );

				if( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIP ) != eINTERPOLATION_MODE_UNDEFINED  )
				{
					eGL_INTERPOLATION_MODE l_eMipMode = m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIP	) );

					if( l_eMinMode == eGL_INTERPOLATION_MODE_LINEAR )
					{
						if( l_eMipMode == eGL_INTERPOLATION_MODE_LINEAR )
						{
							l_eMinMode = eGL_INTERPOLATION_MODE_LINEAR_MIPMAP_LINEAR;
						}
						else
						{
							l_eMinMode = eGL_INTERPOLATION_MODE_LINEAR_MIPMAP_NEAREST;
						}
					}
					else
					{
						if( l_eMipMode == eGL_INTERPOLATION_MODE_LINEAR )
						{
							l_eMinMode = eGL_INTERPOLATION_MODE_NEAREST_MIPMAP_LINEAR;
						}
						else
						{
							l_eMinMode = eGL_INTERPOLATION_MODE_NEAREST_MIPMAP_NEAREST;
						}
					}
				}

				l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MINLOD,			float( m_target->GetMinLod() )											);
				l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MAXLOD,			float( m_target->GetMaxLod() )											);
				l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_LODBIAS,		float( m_target->GetLodBias() )											);
				l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_WRAP_S,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_U					) )	);
				l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_WRAP_T,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_V					) )	);
				l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_WRAP_R,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_W					) )	);
				l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MIN_FILTER,		l_eMinMode																);
				l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MAG_FILTER,		m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG	) )	);
//				l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_BORDERCOLOUR,	m_target->GetBorderColour().const_ptr()									);
				return l_bReturn;
			} );
			m_pfnUnbind = PUnbindFunction( [&](){} );
		}
	}

	return true;
}

void GlSamplerRenderer :: Cleanup()
{
	if( m_uiSamplerId != eGL_INVALID_INDEX )
	{
		m_gl.DeleteSamplers( 1, &m_uiSamplerId );
		m_uiSamplerId = eGL_INVALID_INDEX;
	}

	m_pfnBind = nullptr;
	m_pfnUnbind = nullptr;
}

bool GlSamplerRenderer :: Bind( eTEXTURE_DIMENSION p_eDimension, uint32_t p_uiIndex )
{
	return m_pfnBind( m_gl.Get( p_eDimension ), p_uiIndex );
}

void GlSamplerRenderer :: Unbind()
{
	m_pfnUnbind();
}

//*************************************************************************************************

GlStaticTexture :: GlStaticTexture( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	StaticTexture		( p_pRenderSystem				)
	,	m_uiGlName			( uint32_t( eGL_INVALID_INDEX )	)
	,	m_iCurrentPbo		( 0								)
	,	m_pGlRenderSystem	( p_pRenderSystem				)
	,	m_pLockedIoBuffer	( NULL							)
	,	m_pIoBuffer			( NULL							)
	,	m_gl				( p_gl							)
{
}

GlStaticTexture :: ~GlStaticTexture()
{
	DoDeletePbos();
}

bool GlStaticTexture :: Create()
{
	bool l_bReturn = true;

	if( m_uiGlName == eGL_INVALID_INDEX )
	{
		l_bReturn = m_gl.GenTextures( 1, &m_uiGlName );
	}

	return l_bReturn;
}

void GlStaticTexture :: Destroy()
{
	if( m_uiGlName != eGL_INVALID_INDEX )
	{
		m_gl.DeleteTextures( 1, &m_uiGlName );
		m_uiGlName = uint32_t( eGL_INVALID_INDEX );
	}
}

void GlStaticTexture :: Cleanup()
{
	StaticTexture::Cleanup();
	DoCleanupPbos();
	DoDeletePbos();
}

uint8_t * GlStaticTexture :: Lock( uint32_t p_uiMode )
{
	uint8_t * l_pReturn = NULL;
	eGL_LOCK l_eLock = m_gl.GetLockFlags( p_uiMode );

	if( !m_pLockedIoBuffer )
	{
		m_pLockedIoBuffer = m_pIoBuffer;

		if( m_pLockedIoBuffer )
		{
			m_pLockedIoBuffer->Activate();
			m_pLockedIoBuffer->Fill( NULL, m_pPixelBuffer->size() );
			l_pReturn = m_pLockedIoBuffer->Lock( l_eLock );
		}
	}
	else
	{
		Logger::LogWarning( cuT( "Can't lock image : it is already locked" ) );
	}

	return l_pReturn;
}

void GlStaticTexture :: Unlock( bool p_bModified )
{
	if( m_pLockedIoBuffer )
	{
		m_pLockedIoBuffer->Unlock();

		if( p_bModified )
		{
			OpenGl::PixelFmt l_glPixelFmt = m_gl.Get( m_pPixelBuffer->format() );

			switch( m_eGlDimension )
			{
			case eGL_TEXDIM_1D:
				m_gl.TexSubImage1D( m_eGlDimension, 0, 0, m_pPixelBuffer->width(), l_glPixelFmt.Format, l_glPixelFmt.Type, NULL );
				break;

			case eGL_TEXDIM_2D:
				m_gl.TexSubImage2D( m_eGlDimension, 0, 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_glPixelFmt.Format, l_glPixelFmt.Type, NULL );
				break;

			case eGL_TEXDIM_3D:
				m_gl.TexSubImage3D( m_eGlDimension, 0, 0, 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, l_glPixelFmt.Format, l_glPixelFmt.Type, NULL );
				break;
			}
		}

		m_pLockedIoBuffer->Deactivate();
		m_pLockedIoBuffer = NULL;
	}
	else
	{
		Logger::LogWarning( cuT( "Can't unlock image : it is not locked" ) );
	}
}

void GlStaticTexture :: GenerateMipmaps()
{
	if( m_uiGlName != eGL_INVALID_INDEX )
	{
		m_gl.GenerateMipmap( m_eGlDimension );
	}
}

bool GlStaticTexture :: DoBind()
{
	bool l_bReturn = m_gl.ActiveTexture( m_eIndex );

	if( l_bReturn )
	{
		l_bReturn = m_gl.BindTexture( m_eGlDimension, m_uiGlName );
	}

	return l_bReturn;
}

void GlStaticTexture :: DoUnbind()
{
 	m_gl.ActiveTexture( m_eIndex );
 	m_gl.BindTexture( m_eGlDimension, 0 );
}

bool GlStaticTexture :: DoInitialise()
{
	OpenGl::PixelFmt	l_glPixelFmt;
	bool				l_bReturn;

	DoInitialisePbos();

	m_eIndex			= eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + m_uiIndex );
	m_eGlDimension		= m_gl.Get( m_eDimension );
	l_glPixelFmt		= m_gl.Get( m_pPixelBuffer->format() );
	l_bReturn			= m_gl.BindTexture( m_eGlDimension, m_uiGlName );

	switch( m_eGlDimension )
	{
	case eGL_TEXDIM_1D:
		l_bReturn &= m_gl.TexImage1D(		m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
		break;

	case eGL_TEXDIM_2D:
		l_bReturn &= m_gl.TexImage2D(		m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
		break;

	case eGL_TEXDIM_3D:
		l_bReturn &= m_gl.TexImage3D(		m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
		break;
	}

	return l_bReturn;
}

void GlStaticTexture :: DoDeletePbos()
{
	if( m_pIoBuffer )
	{
		delete m_pIoBuffer;
		m_pIoBuffer = NULL;
	}

	m_pLockedIoBuffer = NULL;
}

void GlStaticTexture :: DoCleanupPbos()
{
	if( m_pIoBuffer )
	{
		m_pIoBuffer->Destroy();
	}

	m_pLockedIoBuffer = NULL;
}

void GlStaticTexture :: DoInitialisePbos()
{
	m_pIoBuffer = new GlUnpackPixelBuffer( m_gl, m_pGlRenderSystem, m_pPixelBuffer );
	m_pIoBuffer->Initialise();
}

//*************************************************************************************************

GlDynamicTexture :: GlDynamicTexture( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	DynamicTexture		( p_pRenderSystem				)
	,	m_uiGlName			( uint32_t( eGL_INVALID_INDEX )	)
	,	m_iCurrentPbo		( 0								)
	,	m_pGlRenderSystem	( p_pRenderSystem				)
	,	m_pLockedIoBuffer	( NULL							)
	,	m_pIoBuffer			( NULL							)
	,	m_gl				( p_gl							)
{
}

GlDynamicTexture :: ~GlDynamicTexture()
{
	DoDeletePbos();
}

bool GlDynamicTexture :: Create()
{
	bool l_bReturn = true;

	if( m_uiGlName == eGL_INVALID_INDEX )
	{
		l_bReturn = m_gl.GenTextures( 1, &m_uiGlName );
	}

	return l_bReturn;
}

void GlDynamicTexture :: Destroy()
{
	if( m_uiGlName != eGL_INVALID_INDEX )
	{
		m_gl.DeleteTextures( 1, &m_uiGlName );
		m_uiGlName = uint32_t( eGL_INVALID_INDEX );
	}
}

void GlDynamicTexture :: Cleanup()
{
	DynamicTexture::Cleanup();
	DoCleanupPbos();
	DoDeletePbos();
}

uint8_t * GlDynamicTexture :: Lock( uint32_t p_uiMode )
{
	uint8_t * l_pReturn = NULL;
	eGL_LOCK l_eLock = m_gl.GetLockFlags( p_uiMode );

	if( !m_pLockedIoBuffer )
	{
		m_pLockedIoBuffer = m_pIoBuffer;

		if( m_pLockedIoBuffer )
		{
			m_pLockedIoBuffer->Activate();
			m_pLockedIoBuffer->Fill( NULL, m_pPixelBuffer->size() );
			l_pReturn = m_pLockedIoBuffer->Lock( l_eLock );
		}
	}
	else
	{
		Logger::LogWarning( cuT( "Can't lock image : it is already locked" ) );
	}

	return l_pReturn;
}

void GlDynamicTexture :: Unlock( bool p_bModified )
{
	if( m_pLockedIoBuffer )
	{
		m_pLockedIoBuffer->Unlock();

		if( p_bModified )
		{
			OpenGl::PixelFmt l_glPixelFmt = m_gl.Get( m_pPixelBuffer->format() );

			switch( m_eGlDimension )
			{
			case eGL_TEXDIM_1D:
				m_gl.TexSubImage1D( m_eGlDimension, 0, 0, m_pPixelBuffer->width(), l_glPixelFmt.Format, l_glPixelFmt.Type, NULL );
				break;

			case eGL_TEXDIM_2D:
				m_gl.TexSubImage2D( m_eGlDimension, 0, 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_glPixelFmt.Format, l_glPixelFmt.Type, NULL );
				break;

			case eGL_TEXDIM_3D:
				m_gl.TexSubImage3D( m_eGlDimension, 0, 0, 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, l_glPixelFmt.Format, l_glPixelFmt.Type, NULL );
				break;
			}
		}

		m_pLockedIoBuffer->Deactivate();
		m_pLockedIoBuffer = NULL;
	}
	else
	{
		Logger::LogWarning( cuT( "Can't unlock image : it is not locked" ) );
	}
}

void GlDynamicTexture :: GenerateMipmaps()
{
	if( m_uiGlName != eGL_INVALID_INDEX && m_eGlDimension != eGL_TEXDIM_2D_MULTISAMPLE )
	{
		m_gl.GenerateMipmap( m_eGlDimension );
	}
}

void GlDynamicTexture :: Fill( uint8_t const * p_pBuffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_eFormat )
{
	OpenGl::PixelFmt l_glPixelFmt = m_gl.Get( p_eFormat );

	switch( m_eGlDimension )
	{
	case eGL_TEXDIM_1D:
		m_gl.TexImage1D( m_eGlDimension, 0, l_glPixelFmt.Internal, p_size.width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_pBuffer );
		break;

	case eGL_TEXDIM_2D:
		m_gl.TexImage2D( m_eGlDimension, 0, l_glPixelFmt.Internal, p_size, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_pBuffer );
		break;

	case eGL_TEXDIM_3D:
		m_gl.TexImage3D( m_eGlDimension, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / m_uiDepth, m_uiDepth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_pBuffer );
		break;
	}
}

bool GlDynamicTexture :: DoBind()
{
	bool l_bReturn = m_gl.ActiveTexture( m_eIndex );

	if( l_bReturn )
	{
		l_bReturn = m_gl.BindTexture( m_eGlDimension, m_uiGlName );
	}

	return l_bReturn;
}

void GlDynamicTexture :: DoUnbind()
{
 	m_gl.ActiveTexture( m_eIndex );
 	m_gl.BindTexture( m_eGlDimension, 0 );
}

bool GlDynamicTexture :: DoInitialise()
{
	OpenGl::PixelFmt	l_glPixelFmt;
	bool				l_bReturn;

	DoInitialisePbos();

	m_eIndex			= eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + m_uiIndex );
	m_eGlDimension		= m_gl.Get( m_eDimension );
	l_glPixelFmt		= m_gl.Get( m_pPixelBuffer->format() );
	l_bReturn			= m_gl.BindTexture( m_eGlDimension, m_uiGlName );

	switch( m_eGlDimension )
	{
	case eGL_TEXDIM_1D:
		l_bReturn &= m_gl.TexImage1D(			m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
		break;

	case eGL_TEXDIM_2D:
		l_bReturn &= m_gl.TexImage2D(			m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
		break;

	case eGL_TEXDIM_3D:
		l_bReturn &= m_gl.TexImage3D(			m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
		break;

	case eGL_TEXDIM_2D_MULTISAMPLE:
		l_bReturn &= m_gl.TexImage2DMultisample(	m_eGlDimension, GetSamplesCount(), l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0 );
		break;
	}

	return l_bReturn;
}

void GlDynamicTexture :: DoDeletePbos()
{
	if( m_pIoBuffer )
	{
		delete m_pIoBuffer;
		m_pIoBuffer = NULL;
	}

	m_pLockedIoBuffer = NULL;
}

void GlDynamicTexture :: DoCleanupPbos()
{
	if( m_pIoBuffer )
	{
		m_pIoBuffer->Destroy();
	}

	m_pLockedIoBuffer = NULL;
}

void GlDynamicTexture :: DoInitialisePbos()
{
	m_pIoBuffer = new GlUnpackPixelBuffer( m_gl, m_pGlRenderSystem, m_pPixelBuffer );
	m_pIoBuffer->Initialise();
}

//*************************************************************************************************

GlTextureRenderer :: GlTextureRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	TextureRenderer		( p_pRenderSystem	)
	,	m_gl				( p_gl				)
#if C3DGL_USE_TBO
	,	m_uiGlName			( eGL_INVALID_INDEX	)
#endif
{
}

GlTextureRenderer :: ~GlTextureRenderer()
{
}

bool GlTextureRenderer :: Render()
{
	if( !m_pfnRender )
	{
#if C3DGL_USE_TBO
		if( m_gl.HasTbo() )
		{
			m_pfnRender		= &GlTextureRenderer::DoRenderTbo;
			m_pfnEndRender	= &GlTextureRenderer::DoEndRenderTbo;
		}
		else
#endif
		if( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGlMajor() >= 3 )
		{
			m_pfnRender = BoolFunction( [&](){ return true; } );
			m_pfnEndRender = VoidFunction( [&](){} );
		}
		else
		{
			m_pfnRender = BoolFunction( [&]()
			{
				if( m_pRenderSystem->UseMultiTexturing() )
				{
					if( m_target->GetRgbFunction() != eRGB_BLEND_FUNC_NONE || m_target->GetAlpFunction() != eALPHA_BLEND_FUNC_NONE )
					{
						m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_TEXTURE_ENV_MODE, eGL_TEXENV_PARAM_COMBINE);

						if( m_target->GetRgbFunction() != eRGB_BLEND_FUNC_NONE )
						{
							m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_COMBINE_RGB, m_gl.Get( m_target->GetRgbFunction() ) );
							m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE0_RGB, m_gl.Get( m_target->GetRgbArgument( eBLEND_SRC_INDEX_0 ) ) );
							m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND0_RGB, eGL_BLEND_FACTOR_SRC_COLOR);

							if( m_target->GetRgbFunction() != eRGB_BLEND_FUNC_FIRST_ARG )
							{
								m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE1_RGB, m_gl.Get( m_target->GetRgbArgument( eBLEND_SRC_INDEX_1 ) ) );
								m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND1_RGB, eGL_BLEND_FACTOR_SRC_COLOR );
							}

							if( m_target->GetRgbFunction() == eRGB_BLEND_FUNC_INTERPOLATE )
							{
								m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE2_RGB, m_gl.Get( m_target->GetRgbArgument( eBLEND_SRC_INDEX_2 ) ) );
								m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND2_RGB, eGL_BLEND_FACTOR_SRC_COLOR );
							}
						}

						if( m_target->GetAlpFunction() != eALPHA_BLEND_FUNC_NONE )
						{
							m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_COMBINE_ALPHA, m_gl.Get( m_target->GetAlpFunction() ) );
							m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE0_ALPHA, m_gl.Get( m_target->GetAlpArgument( eBLEND_SRC_INDEX_0 ) ) );
							m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND0_ALPHA, eGL_BLEND_FACTOR_SRC_ALPHA);

							if( m_target->GetAlpFunction() != eALPHA_BLEND_FUNC_FIRST_ARG )
							{
								m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE1_ALPHA, m_gl.Get( m_target->GetAlpArgument( eBLEND_SRC_INDEX_1 ) ) );
								m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND1_ALPHA, eGL_BLEND_FACTOR_SRC_ALPHA );
							}

							if( m_target->GetAlpFunction() == eALPHA_BLEND_FUNC_INTERPOLATE )
							{
								m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE2_ALPHA, m_gl.Get( m_target->GetAlpArgument( eBLEND_SRC_INDEX_2 ) ) );
								m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND2_ALPHA, eGL_BLEND_FACTOR_SRC_ALPHA );
							}
						}
					}
				}

				m_gl.Enable( m_gl.Get( m_target->GetDimension() ) );

				if( m_target->GetMappingMode() != eTEXTURE_MAP_MODE_NONE )
				{
					if( m_target->GetMappingMode() == eTEXTURE_MAP_MODE_REFLEXION )
					{
						m_gl.TexGeni( eGL_TEXGEN_COORD_S, eGL_TEXGEN_PARAM_TEXTURE_GEN_MODE, eGL_TEXGEN_MODE_REFLECTION_MAP );
						m_gl.TexGeni( eGL_TEXGEN_COORD_T, eGL_TEXGEN_PARAM_TEXTURE_GEN_MODE, eGL_TEXGEN_MODE_REFLECTION_MAP );
					}
					else if( m_target->GetMappingMode() == eTEXTURE_MAP_MODE_SPHERE )
					{
						m_gl.TexGeni( eGL_TEXGEN_COORD_S, eGL_TEXGEN_PARAM_TEXTURE_GEN_MODE, eGL_TEXGEN_MODE_SPHERE_MAP );
						m_gl.TexGeni( eGL_TEXGEN_COORD_T, eGL_TEXGEN_PARAM_TEXTURE_GEN_MODE, eGL_TEXGEN_MODE_SPHERE_MAP );
					}

					m_gl.Enable( eGL_TWEAK_TEXTURE_GEN_S );
					m_gl.Enable( eGL_TWEAK_TEXTURE_GEN_T );
				}

				if( m_target->GetAlphaFunc() != eALPHA_FUNC_ALWAYS )
				{
					m_gl.Enable( eGL_TWEAK_ALPHA_TEST );
					m_gl.AlphaFunc( m_gl.Get( m_target->GetAlphaFunc() ), m_target->GetAlphaValue() );
				}

				return true;
			} );

			m_pfnEndRender = VoidFunction( [&]()
			{
				if( m_target->GetAlphaFunc() != eALPHA_FUNC_ALWAYS )
				{
					m_gl.Disable( eGL_TWEAK_ALPHA_TEST );
				}

				if( m_target->GetMappingMode() != eTEXTURE_MAP_MODE_NONE )
				{
					m_gl.Disable( eGL_TWEAK_TEXTURE_GEN_S );
					m_gl.Disable( eGL_TWEAK_TEXTURE_GEN_T );
				}
			} );
		}
	}

	return m_pfnRender();
}

void GlTextureRenderer :: EndRender()
{
	m_pfnEndRender();
}

//*************************************************************************************************
