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
		if ( !GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
		{
			m_depth = GetNext2Pow( p_dimensions[2] );
			Size l_size( GetNext2Pow( p_dimensions[0] ), GetNext2Pow( p_dimensions[1] ) * m_depth );
			Castor::Image l_img( cuT( "Tmp" ), *p_buffer );
			m_pixelBuffer = l_img.Resample( l_size ).GetPixels();
		}
		else
		{
			m_depth = p_dimensions[2];
			m_pixelBuffer = p_buffer;
		}
	}

	bool StaticTexture::Initialise()
	{
		if ( !m_initialised )
		{
			m_initialised = DoInitialise();

			if ( m_initialised && m_pixelBuffer && m_pixelBuffer->ptr() )
			{
				m_size = m_pixelBuffer->dimensions();
				m_pixelFormat = m_pixelBuffer->format();
				m_pixelBuffer->clear();
				m_pixelBuffer.reset();
			}

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
}
