#include "TextureImage.hpp"

#include "Engine.hpp"
#include "TextureStorage.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureImage::TextureImage( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
	{
	}

	void TextureImage::SetSource( Point3ui const & p_dimensions, PxBufferBaseSPtr p_buffer )
	{
		m_depth = p_dimensions[2];
		Size l_size{ p_dimensions[0], p_dimensions[1] };
		m_staticSource = true;

		if ( DoAdjustDimensions( l_size, m_depth ) )
		{
			Image l_img( cuT( "Tmp" ), *p_buffer );
			m_pixelBuffer = l_img.Resample( l_size ).GetPixels();
		}
		else
		{
			m_pixelBuffer = p_buffer;
		}
	}

	void TextureImage::SetSource( PxBufferBaseSPtr p_buffer )
	{
		m_depth = 1;
		Size l_size{ p_buffer->dimensions() };
		m_staticSource = true;

		if ( DoAdjustDimensions( l_size, m_depth ) )
		{
			Image l_img( cuT( "Tmp" ), *p_buffer );
			m_pixelBuffer = l_img.Resample( l_size ).GetPixels();
		}
		else
		{
			m_pixelBuffer = p_buffer;
		}
	}

	void TextureImage::SetSource( Size const & p_size, ePIXEL_FORMAT p_format )
	{
		m_depth = 1;
		Size l_size{ p_size };
		DoAdjustDimensions( l_size, m_depth );
		m_pixelBuffer = PxBufferBase::create( l_size, p_format );
		m_staticSource = false;
	}

	void TextureImage::SetSource( Point3ui const & p_size, ePIXEL_FORMAT p_format )
	{
		m_depth = p_size[2];
		Size l_size{ p_size[0], p_size[1] };
		DoAdjustDimensions( l_size, m_depth );
		m_pixelBuffer = PxBufferBase::create( l_size, p_format );
		m_staticSource = false;
	}

	void TextureImage::Resize( Size const & p_size )
	{
		m_depth = 1;
		DoResize( p_size );
	}

	void TextureImage::Resize( Point3ui const & p_size )
	{
		m_depth = p_size[2];
		DoResize( Size{ p_size[0], p_size[1] } );
	}

	bool TextureImage::Initialise( eTEXTURE_TYPE p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		bool l_return = false;

		if ( !m_storage )
		{
			try
			{
				m_storage = GetEngine()->GetRenderSystem()->CreateTextureStorage( p_type, *this, p_cpuAccess, p_gpuAccess );
				l_return = true;
			}
			catch ( std::exception & p_exc )
			{
				Logger::LogError( StringStream() << cuT( "TextureImage::Initialise - Error encountered while allocating storage: " ) << string::string_cast< xchar >( p_exc.what() ) );
			}
		}

		ENSURE( m_storage );
		return l_return;
	}

	void TextureImage::Cleanup()
	{
		m_storage.reset();
	}

	bool TextureImage::Bind( uint32_t p_index )const
	{
		REQUIRE( m_storage );
		return m_storage->Bind( p_index );
	}

	void TextureImage::Unbind( uint32_t p_index )const
	{
		REQUIRE( m_storage );
		m_storage->Unbind( p_index );
	}

	uint8_t * TextureImage::Lock( uint32_t p_lock )
	{
		REQUIRE( m_storage );
		return m_storage->Lock( p_lock );
	}

	void TextureImage::Unlock( bool p_modified )
	{
		REQUIRE( m_storage );
		m_storage->Unlock( p_modified );
	}

	void TextureImage::DoResize( Size const & p_size )
	{
		REQUIRE( !m_staticSource );
		Size l_size{ p_size };
		DoAdjustDimensions( l_size, m_depth );

		if ( m_pixelBuffer && m_pixelBuffer->dimensions() != p_size )
		{
			m_pixelBuffer = PxBufferBase::create( p_size, m_pixelBuffer->format() );

			if ( m_storage )
			{
				auto l_cpuAccess = m_storage->GetCPUAccess();
				auto l_gpuAccess = m_storage->GetGPUAccess();
				auto l_type = m_storage->GetType();
				Cleanup();
				Initialise( l_type, l_cpuAccess, l_gpuAccess );
			}
		}
	}

	bool TextureImage::DoAdjustDimensions( Castor::Size & p_size, uint32_t & p_depth )
	{
		bool l_return = false;

		if ( !GetEngine()->GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
		{
			p_depth = GetNext2Pow( p_depth );
			p_size.set( GetNext2Pow( p_size.width() ), GetNext2Pow( p_size.height() ) );
			l_return = true;
		}

		p_size[1] *= p_depth;
		return l_return;
	}
}
