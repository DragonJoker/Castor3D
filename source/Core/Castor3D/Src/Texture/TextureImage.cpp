#include "TextureImage.hpp"

#include "Engine.hpp"
#include "TextureStorage.hpp"

#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"

using namespace Castor;
namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		class StaticTextureSource
			: public TextureSource
		{
		public:
			virtual bool IsStatic()const
			{
				return true;
			}

			bool Resize( Size const & p_size, uint32_t p_depth )
			{
				FAILURE( "Can't call Resize on a static texture source." );
				return false;
			}
		};

		//*********************************************************************************************

		class Static2DTextureSource
			: public StaticTextureSource
		{
		public:
			Static2DTextureSource( PxBufferBaseSPtr p_buffer )
			{
				Size l_size{ p_buffer->dimensions() };
				uint32_t l_depth{ 1u };

				if ( DoAdjustDimensions( l_size, l_depth ) )
				{
					Image l_img( cuT( "Tmp" ), *p_buffer );
					m_buffer = l_img.Resample( l_size ).GetPixels();
				}
				else
				{
					m_buffer = p_buffer;
				}
			}

			virtual uint32_t GetDepth()const
			{
				return 1u;
			}
		};

		//*********************************************************************************************

		class Static3DTextureSource
			: public StaticTextureSource
		{
		public:
			Static3DTextureSource( Point3ui const & p_dimensions, PxBufferBaseSPtr p_buffer )
			{
				m_depth = p_dimensions[2];
				Size l_size{ p_dimensions[0], p_dimensions[1] };

				if ( DoAdjustDimensions( l_size, m_depth ) )
				{
					Image l_img( cuT( "Tmp" ), *p_buffer );
					m_buffer = l_img.Resample( l_size ).GetPixels();
				}
				else
				{
					m_buffer = p_buffer;
				}
			}

			virtual uint32_t GetDepth()const
			{
				return m_depth;
			}

		private:
			uint32_t m_depth{ 1u };
		};

		//*********************************************************************************************

		class DynamicTextureSource
			: public TextureSource
		{
		public:
			virtual bool IsStatic()const
			{
				return false;
			}

			bool Resize( Size const & p_size, uint32_t p_depth )
			{
				Size l_size{ p_size };
				DoAdjustDimensions( l_size, p_depth );
				bool l_return = m_buffer && m_buffer->dimensions() != p_size;

				if ( l_return )
				{
					m_buffer = PxBufferBase::create( p_size, m_buffer->format() );
				}

				return l_return;
			}
		};

		//*********************************************************************************************

		class Dynamic2DTextureSource
			: public DynamicTextureSource
		{
		public:
			Dynamic2DTextureSource( Size const & p_dimensions, ePIXEL_FORMAT p_format )
			{
				auto l_size{ p_dimensions };
				uint32_t l_depth{ 1u };
				DoAdjustDimensions( l_size, l_depth );
				m_buffer = PxBufferBase::create( l_size, p_format );
			}

			virtual uint32_t GetDepth()const
			{
				return 1u;
			}
		};

		//*********************************************************************************************

		class Dynamic3DTextureSource
			: public DynamicTextureSource
		{
		public:
			Dynamic3DTextureSource( Point3ui const & p_dimensions, ePIXEL_FORMAT p_format )
			{
				m_depth = p_dimensions[2];
				Size l_size{ p_dimensions[0], p_dimensions[1] };
				DoAdjustDimensions( l_size, m_depth );
				m_buffer = PxBufferBase::create( l_size, p_format );
			}

			virtual uint32_t GetDepth()const
			{
				return m_depth;
			}

		private:
			uint32_t m_depth{ 1u };
		};
	}

	//*********************************************************************************************

	bool TextureSource::DoAdjustDimensions( Castor::Size & p_size, uint32_t & p_depth )
	{
		bool l_return = false;

		if ( !Engine::GetInstance().GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
		{
			p_depth = GetNext2Pow( p_depth );
			p_size.set( GetNext2Pow( p_size.width() ), GetNext2Pow( p_size.height() ) );
			l_return = true;
		}

		p_size[1] *= p_depth;
		return l_return;
	}

	//*********************************************************************************************

	TextureImage::TextureImage( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
	{
	}

	void TextureImage::SetSource( PxBufferBaseSPtr p_buffer )
	{
		m_source = std::make_unique< Static2DTextureSource >( p_buffer );
	}

	void TextureImage::SetSource( Point3ui const & p_dimensions, PxBufferBaseSPtr p_buffer )
	{
		m_source = std::make_unique< Static3DTextureSource >( p_dimensions, p_buffer );
	}

	void TextureImage::SetSource( Size const & p_size, ePIXEL_FORMAT p_format )
	{
		m_source = std::make_unique< Dynamic2DTextureSource >( p_size, p_format );
	}

	void TextureImage::SetSource( Point3ui const & p_size, ePIXEL_FORMAT p_format )
	{
		m_source = std::make_unique< Dynamic3DTextureSource >( p_size, p_format );
	}

	void TextureImage::Resize( Size const & p_size )
	{
		if ( m_source->Resize( p_size, 1u ) )
		{
			DoResetStorage();
		}
	}

	void TextureImage::Resize( Point3ui const & p_size )
	{
		if ( m_source->Resize( Size{ p_size[0], p_size[1] }, p_size[2] ) )
		{
			DoResetStorage();
		}
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

	void TextureImage::DoResetStorage()
	{
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
