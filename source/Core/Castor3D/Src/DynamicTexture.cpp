#include "DynamicTexture.hpp"

#include "RenderSystem.hpp"
#include "Sampler.hpp"

#include <Image.hpp>
#include <PixelBufferBase.hpp>

using namespace Castor;

namespace Castor3D
{
	DynamicTexture::DynamicTexture( RenderSystem & p_renderSystem, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: Texture( eTEXTURE_BASE_TYPE_DYNAMIC, p_renderSystem, p_cpuAccess, p_gpuAccess )
		, m_samplesCount( 0 )
	{
	}

	DynamicTexture::~DynamicTexture()
	{
	}

	bool DynamicTexture::Initialise()
	{
		if ( !m_initialised )
		{
			if ( !GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
			{
				Castor::Image l_img( cuT( "Tmp" ), *m_pixelBuffer );
				Castor::Size l_size = m_pixelBuffer->dimensions();
				l_size.set( GetNext2Pow( l_size.width() ), GetNext2Pow( l_size.height() ) );
				m_pixelBuffer = l_img.Resample( l_size ).GetPixels();
			}

			m_initialised = DoInitialise();
		}

		return m_initialised;
	}

	void DynamicTexture::Cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;
		}
	}

	void DynamicTexture::Resize( Castor::Size const & p_size )
	{
		m_depth = 1;

		if ( m_pixelBuffer && m_pixelBuffer->dimensions() != p_size )
		{
			Castor::Size l_size = p_size;

			if ( !GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
			{
				l_size.set( GetNext2Pow( l_size.width() ), GetNext2Pow( l_size.height() ) );
			}

			m_pixelBuffer = PxBufferBase::create( l_size, m_pixelBuffer->format() );
			Cleanup();
			m_initialised = DoInitialise();
		}
	}

	void DynamicTexture::Resize( Castor::Point3ui const & p_size )
	{
		Size l_size;

		if ( !GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
		{
			m_depth = GetNext2Pow( p_size[2] );
			l_size.set( GetNext2Pow( p_size[0] ), GetNext2Pow( p_size[1] ) * m_depth );
		}
		else
		{
			m_depth = p_size[2];
			l_size.set( p_size[0], p_size[1] * m_depth );
		}

		if ( m_pixelBuffer && m_pixelBuffer->dimensions() != l_size )
		{
			m_pixelBuffer = PxBufferBase::create( l_size, m_pixelBuffer->format() );
			Cleanup();
			m_initialised = DoInitialise();
		}
	}

	void DynamicTexture::SetImage( Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		m_depth = 1;
		Castor::Size l_size = p_size;

		if ( !GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
		{
			l_size.set( GetNext2Pow( l_size.width() ), GetNext2Pow( l_size.height() ) );
		}

		m_pixelBuffer = Castor::PxBufferBase::create( l_size, p_format );
	}

	void DynamicTexture::SetImage( Castor::Point3ui const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		Size l_size;

		if ( !GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
		{
			m_depth = GetNext2Pow( p_size[2] );
			l_size.set( GetNext2Pow( p_size[0] ), GetNext2Pow( p_size[1] ) * m_depth );
		}
		else
		{
			m_depth = p_size[2];
			l_size.set( p_size[0], p_size[1] * m_depth );
		}

		m_pixelBuffer = Castor::PxBufferBase::create( l_size, p_format );
	}
}
