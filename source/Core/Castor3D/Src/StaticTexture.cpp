#include "StaticTexture.hpp"
#include "Sampler.hpp"

#include <Logger.hpp>
#include <Image.hpp>

using namespace Castor;

namespace Castor3D
{
	StaticTexture::StaticTexture( RenderSystem * p_pRenderSystem )
		:	TextureBase( eTEXTURE_TYPE_STATIC, p_pRenderSystem )
	{
	}

	StaticTexture::~StaticTexture()
	{
	}

	bool StaticTexture::Initialise( uint32_t p_uiIndex )
	{
		if ( !m_bInitialised )
		{
			if ( !m_pRenderSystem->HasNonPowerOfTwoTextures() )
			{
				Castor::Image l_img( cuT( "Tmp" ), *m_pPixelBuffer );
				Castor::Size l_size = m_pPixelBuffer->dimensions();
				l_size.set( GetNext2Pow( l_size.width() ), GetNext2Pow( l_size.height() ) );
				m_pPixelBuffer = l_img.Resample( l_size ).GetPixels();
			}

			m_uiIndex			= p_uiIndex;
			m_bInitialised		= DoInitialise();
		}
		else
		{
			Logger::LogWarning( cuT( "Calling StaticTexture::Initialise on an already initialised texture" ) );
		}

		return m_bInitialised;
	}


	void StaticTexture::SetImage( Castor::Point3ui const & p_dimensions, Castor::PxBufferBaseSPtr p_pBuffer )
	{
		Size l_size;

		if ( !m_pRenderSystem->HasNonPowerOfTwoTextures() )
		{
			m_uiDepth = GetNext2Pow( p_dimensions[2] );
			l_size.set( GetNext2Pow( p_dimensions[0] ), GetNext2Pow( p_dimensions[1] ) * m_uiDepth );
		}
		else
		{
			m_uiDepth = p_dimensions[2];
			l_size.set( p_dimensions[0], p_dimensions[1] * m_uiDepth );
		}

		if ( !m_pRenderSystem->HasNonPowerOfTwoTextures() )
		{
			Castor::Image l_img( cuT( "Tmp" ), *p_pBuffer );
			m_pPixelBuffer = l_img.Resample( l_size ).GetPixels();
		}
		else
		{
			m_pPixelBuffer = p_pBuffer;
		}
	}

	void StaticTexture::Cleanup()
	{
		if ( m_bInitialised )
		{
			m_bInitialised = false;
		}
	}

	bool StaticTexture::Bind()
	{
		bool l_bReturn = false;

		if ( m_bInitialised )
		{
			if ( m_pPixelBuffer && m_pPixelBuffer->ptr() )
			{
				m_size = m_pPixelBuffer->dimensions();
				m_ePixelFormat = m_pPixelBuffer->format();
				m_pPixelBuffer->clear();
				m_pPixelBuffer.reset();
			}

			l_bReturn = DoBind();

			if ( l_bReturn && GetSampler() )
			{
				l_bReturn = GetSampler()->Bind( m_eDimension, m_uiIndex );
			}
		}

		return l_bReturn;
	}

	void StaticTexture::Unbind()
	{
		if ( GetSampler() )
		{
			GetSampler()->Unbind();
		}

		DoUnbind();
	}
}
