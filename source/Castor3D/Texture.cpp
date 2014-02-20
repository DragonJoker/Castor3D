#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Texture.hpp"
#include "Castor3D/Sampler.hpp"

using namespace Castor3D;
using namespace Castor;

//*********************************************************************************************

TextureBase :: TextureBase( eTEXTURE_TYPE p_eType, RenderSystem * p_pRenderSystem )
	:	m_bInitialised	( false						)
	,	m_eType			( p_eType					)
	,	m_eDimension	( eTEXTURE_DIMENSION_2D		)
	,	m_eMapMode		( eTEXTURE_MAP_MODE_NONE	)
	,	m_uiIndex		( 0							)
	,	m_pRenderSystem	( p_pRenderSystem			)
{
}

TextureBase :: ~TextureBase()
{
}

void TextureBase :: SetImage( Castor::PxBufferBaseSPtr p_pBuffer )
{
	if( !m_pRenderSystem->HasNonPowerOfTwoTextures() )
	{
		Castor::Image l_img( cuT( "Tmp" ), *p_pBuffer );
		Castor::Size l_size = p_pBuffer->dimensions();
		l_size.set( GetNext2Pow( l_size.width() ), GetNext2Pow( l_size.height() ) );
		m_pPixelBuffer = l_img.Resample( l_size ).GetPixels();
	}
	else
	{
		m_pPixelBuffer = p_pBuffer;
	}
}

//*********************************************************************************************

StaticTexture :: StaticTexture( RenderSystem * p_pRenderSystem )
	:	TextureBase	( eTEXTURE_TYPE_STATIC, p_pRenderSystem	)
{
}

StaticTexture :: ~StaticTexture()
{
}

bool StaticTexture :: Initialise( uint32_t p_uiIndex )
{
	if( !m_bInitialised )
	{
		m_uiIndex			= p_uiIndex;
		m_bInitialised		= DoInitialise();
	}
	else
	{
		Logger::LogWarning( cuT( "Calling StaticTexture::Initialise on an already initialised texture" ) );
	}

	return m_bInitialised;
}

void StaticTexture :: Cleanup()
{
	if( m_bInitialised )
	{
		m_bInitialised = false;
	}
}

bool StaticTexture :: Bind()
{
	bool l_bReturn = false;

	if( m_bInitialised )
	{
		if( m_pPixelBuffer && m_pPixelBuffer->ptr() )
		{
			m_size = m_pPixelBuffer->dimensions();
			m_ePixelFormat = m_pPixelBuffer->format();
			m_pPixelBuffer->clear();
			m_pPixelBuffer.reset();
		}

		l_bReturn = DoBind();

		if( l_bReturn && GetSampler() )
		{
			l_bReturn = GetSampler()->Bind( m_eDimension, m_uiIndex );
		}
	}

	return l_bReturn;
}

void StaticTexture :: Unbind()
{
	if( GetSampler() )
	{
		GetSampler()->Unbind();
	}

	DoUnbind();
}

//*********************************************************************************************

DynamicTexture :: DynamicTexture( RenderSystem * p_pRenderSystem )
	:	TextureBase		( eTEXTURE_TYPE_DYNAMIC, p_pRenderSystem	)
	,	m_bRenderTarget	( false										)
	,	m_iSamplesCount	( 0											)
{
}

DynamicTexture :: ~DynamicTexture()
{
}

bool DynamicTexture :: Initialise( uint32_t p_uiIndex )
{
	if( !m_bInitialised )
	{
		m_uiIndex			= p_uiIndex;
		m_bInitialised		= DoInitialise();

		if( GetSampler() )
		{
			GetSampler()->Initialise();
		}
	}

	return m_bInitialised;
}

void DynamicTexture :: Cleanup()
{
	if( m_bInitialised )
	{
		m_bInitialised = false;
	}
}

bool DynamicTexture :: Bind()
{
	bool l_bReturn = false;

	if( m_bInitialised )
	{
		l_bReturn = DoBind();

		if( l_bReturn && GetSampler() )
		{
			l_bReturn = GetSampler()->Bind( m_eDimension, m_uiIndex );
		}
	}

	return l_bReturn;
}

void DynamicTexture :: Unbind()
{
	if( GetSampler() )
	{
		GetSampler()->Unbind();
	}

	DoUnbind();
}

void DynamicTexture :: Resize( Castor::Size const & p_size )
{
	m_uiDepth = 1;

	if( m_pPixelBuffer && m_pPixelBuffer->dimensions() != p_size )
	{
		Castor::Size l_size = p_size;

		if( !m_pRenderSystem->HasNonPowerOfTwoTextures() )
		{
			l_size.set( GetNext2Pow( l_size.width() ), GetNext2Pow( l_size.height() ) );
		}

		m_pPixelBuffer = PxBufferBase::create( l_size, m_pPixelBuffer->format() );
		Cleanup();
		m_bInitialised = DoInitialise();
	}
}

void DynamicTexture :: Resize( Castor::Point3ui const & p_size )
{
	Size l_size;

	if( !m_pRenderSystem->HasNonPowerOfTwoTextures() )
	{
		m_uiDepth = GetNext2Pow( p_size[2] );
		l_size.set( GetNext2Pow( p_size[0] ), GetNext2Pow( p_size[1] ) * m_uiDepth );
	}
	else
	{
		m_uiDepth = p_size[2];
		l_size.set( p_size[0], p_size[1] * m_uiDepth );
	}

	if( m_pPixelBuffer && m_pPixelBuffer->dimensions() != l_size )
	{
		m_pPixelBuffer = PxBufferBase::create( l_size, m_pPixelBuffer->format() );
		Cleanup();
		m_bInitialised = DoInitialise();
	}
}

void DynamicTexture :: SetImage( Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_ePixelFormat )
{
	m_uiDepth = 1;
	Castor::Size l_size = p_size;

	if( !m_pRenderSystem->HasNonPowerOfTwoTextures() )
	{
		l_size.set( GetNext2Pow( l_size.width() ), GetNext2Pow( l_size.height() ) );
	}

	m_pPixelBuffer = Castor::PxBufferBase::create( l_size, p_ePixelFormat );
}

void DynamicTexture :: SetImage( Castor::Point3ui const & p_size, Castor::ePIXEL_FORMAT p_ePixelFormat )
{
	Size l_size;

	if( !m_pRenderSystem->HasNonPowerOfTwoTextures() )
	{
		m_uiDepth = GetNext2Pow( p_size[2] );
		l_size.set( GetNext2Pow( p_size[0] ), GetNext2Pow( p_size[1] ) * m_uiDepth );
	}
	else
	{
		m_uiDepth = p_size[2];
		l_size.set( p_size[0], p_size[1] * m_uiDepth );
	}

	m_pPixelBuffer = Castor::PxBufferBase::create( l_size, p_ePixelFormat );
}

//*********************************************************************************************
