#include "StaticTexture.hpp"

#include "RenderSystem.hpp"
#include "Sampler.hpp"

#include <Logger.hpp>
#include <Image.hpp>

using namespace Castor;

namespace Castor3D
{
	StaticTexture::StaticTexture( RenderSystem & p_renderSystem )
		: Texture( eTEXTURE_BASE_TYPE_STATIC, p_renderSystem, eACCESS_TYPE_READ, eACCESS_TYPE_READ )
	{
	}

	StaticTexture::~StaticTexture()
	{
	}

	void StaticTexture::SetImage( Castor::Point3ui const & p_dimensions, Castor::PxBufferBaseSPtr p_buffer )
	{
		if ( !GetRenderSystem()->HasNonPowerOfTwoTextures() )
		{
			m_uiDepth = GetNext2Pow( p_dimensions[2] );
			Size l_size( GetNext2Pow( p_dimensions[0] ), GetNext2Pow( p_dimensions[1] ) * m_uiDepth );
			Castor::Image l_img( cuT( "Tmp" ), *p_buffer );
			m_pPixelBuffer = l_img.Resample( l_size ).GetPixels();
		}
		else
		{
			m_uiDepth = p_dimensions[2];
			m_pPixelBuffer = p_buffer;
		}
	}

	bool StaticTexture::Initialise()
	{
		if ( !m_initialised )
		{
			m_initialised = DoInitialise();
		}
		else
		{
			Logger::LogWarning( cuT( "Calling StaticTexture::Initialise on an already initialised texture" ) );
		}

		return m_initialised;
	}

	void StaticTexture::Cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;
		}
	}

	bool StaticTexture::Bind( uint32_t p_index )
	{
		bool l_return = false;

		if ( m_initialised )
		{
			if ( m_pPixelBuffer && m_pPixelBuffer->ptr() )
			{
				m_size = m_pPixelBuffer->dimensions();
				m_ePixelFormat = m_pPixelBuffer->format();
				m_pPixelBuffer->clear();
				m_pPixelBuffer.reset();
			}

			l_return = DoBind( p_index );

			if ( l_return && GetSampler() )
			{
				l_return = GetSampler()->Bind( m_type, p_index );
			}
		}

		return l_return;
	}

	void StaticTexture::Unbind( uint32_t p_index )
	{
		if ( GetSampler() )
		{
			GetSampler()->Unbind();
		}

		DoUnbind( p_index );
	}
}
