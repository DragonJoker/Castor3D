#include "TextureImage.hpp"

#include "Engine.hpp"
#include "TextureStorage.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureImage::TextureImage( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, m_depth{ 1 }
	{
	}

	TextureImage::~TextureImage()
	{
	}

	void TextureImage::SetImage( Castor::Point3ui const & p_dimensions, Castor::PxBufferBaseSPtr p_buffer )
	{
		if ( !GetEngine()->GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
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

	void TextureImage::SetImage( Castor::PxBufferBaseSPtr p_buffer )
	{
		m_depth = 1;
		m_pixelBuffer = p_buffer;
	}

	void TextureImage::SetImage( Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		m_depth = 1;
		Castor::Size l_size = p_size;

		if ( !GetEngine()->GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
		{
			l_size.set( GetNext2Pow( l_size.width() ), GetNext2Pow( l_size.height() ) );
		}

		m_pixelBuffer = Castor::PxBufferBase::create( l_size, p_format );
	}

	void TextureImage::SetImage( Castor::Point3ui const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		Size l_size;

		if ( !GetEngine()->GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
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

	bool TextureImage::Initialise( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		bool l_return = false;

		if ( m_storage )
		{
			m_storage = GetEngine()->GetRenderSystem()->CreateStorage( p_cpuAccess, p_gpuAccess );
		}

		return l_return;
	}

	void TextureImage::Cleanup()
	{
		m_storage.reset();
	}

	bool TextureImage::Bind( uint32_t p_index )const
	{
		bool l_return = false;

		if ( m_storage )
		{
			l_return = m_storage->Bind( p_index );
		}

		return l_return;
	}

	void TextureImage::Unbind( uint32_t p_index )const
	{
		if ( m_storage )
		{
			m_storage->Unbind( p_index );
		}
	}

	uint8_t * TextureImage::Lock( uint32_t p_lock )
	{
		uint8_t * l_return = nullptr;

		if ( m_storage )
		{
			l_return = m_storage->Lock( p_lock );
		}

		return l_return;
	}

	void TextureImage::Unlock( bool p_modified )
	{
		if ( m_storage )
		{
			m_storage->Unlock( p_modified );
		}
	}
}
