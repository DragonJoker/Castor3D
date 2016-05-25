#include "TextureImage.hpp"

#include "Engine.hpp"
#include "TextureStorage.hpp"

#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"

#include <Image.hpp>

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

			virtual String ToString()const
			{
				return String{};
			}
		};

		//*********************************************************************************************

		class StaticFileTextureSource
			: public StaticTextureSource
		{
		public:
			StaticFileTextureSource( Path const & p_folder, Path const & p_relative )
				: m_folder{ p_folder }
				, m_relative{ p_relative }
			{
				ImageSPtr l_image;

				if ( File::FileExists( p_folder / p_relative ) )
				{
					l_image = Engine::GetInstance().GetImageManager().create( p_relative.GetFileName(), p_folder / p_relative );
				}

				if ( !l_image )
				{
					CASTOR_EXCEPTION( cuT( "TextureImage::SetSource - Couldn't load image " ) + p_relative );
				}

				auto l_buffer = l_image->GetPixels();
				Size l_size{ l_buffer->dimensions() };
				uint32_t l_depth{ 1u };

				if ( DoAdjustDimensions( l_size, l_depth ) )
				{
					Image l_img( cuT( "Tmp" ), *l_buffer );
					m_buffer = l_img.Resample( l_size ).GetPixels();
				}
				else
				{
					m_buffer = l_buffer;
				}
			}

			virtual uint32_t GetDepth()const
			{
				return 1u;
			}

			virtual String ToString()const
			{
				return m_folder / m_relative;
			}

		private:
			Path m_folder;
			Path m_relative;
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

			virtual String ToString()const
			{
				return String{};
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
				Size l_size{ p_dimensions };
				uint32_t l_depth{ 1u };
				DoAdjustDimensions( l_size, l_depth );
				m_buffer = PxBufferBase::create( l_size, p_format );
			}

			virtual uint32_t GetDepth()const
			{
				return 1u;
			}

			virtual String ToString()const
			{
				return string::to_string( m_buffer->dimensions().width() ) + cuT( "x" ) + string::to_string( m_buffer->dimensions().height() );
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

			virtual String ToString()const
			{
				return string::to_string( m_buffer->dimensions().width() ) + cuT( "x" ) + string::to_string( m_buffer->dimensions().height() ) + cuT( "x" ) + string::to_string( m_depth );
			}

		private:
			uint32_t m_depth{ 1u };
		};

		//*********************************************************************************************

		TextureStorageType GetStorageType( TextureType p_type, uint32_t p_index )
		{
			TextureStorageType l_return = TextureStorageType::Count;

			switch ( p_type )
			{
			case TextureType::Buffer:
				l_return = TextureStorageType::Buffer;
				break;

			case TextureType::OneDimension:
				l_return = TextureStorageType::OneDimension;
				break;

			case TextureType::OneDimensionArray:
			case TextureType::TwoDimensions:
				l_return = TextureStorageType::TwoDimensions;
				break;

			case TextureType::TwoDimensionsArray:
				l_return = TextureStorageType::ThreeDimensions;
				break;

			case TextureType::TwoDimensionsMS:
				l_return = TextureStorageType::TwoDimensionsMS;
				break;

			case TextureType::ThreeDimensions:
				l_return = TextureStorageType::ThreeDimensions;
				break;

			case TextureType::Cube:
				l_return = TextureStorageType( uint32_t( TextureStorageType::CubeMapPositiveX ) + p_index );
				break;

			default:
				FAILURE( "The given texture type doesn't have any associated storage type" );
				CASTOR_EXCEPTION( cuT( "The given texture type doesn't have any associated storage type" ) );
				break;
			}

			return l_return;
		}
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

	TextureImage::TextLoader::TextLoader( String const & p_tabs )
		: Castor::TextLoader< TextureImage >{ p_tabs }
	{
	}

	bool TextureImage::TextLoader::operator()( TextureImage const & p_obj, TextFile & p_file )
	{
		return p_file.WriteText( p_obj.m_source->ToString() ) > 0;
	}

	//*********************************************************************************************

	TextureImage::TextureImage( Engine & p_engine, uint32_t p_index )
		: OwnedBy< Engine >{ p_engine }
		, m_index{ p_index }
	{
	}

	void TextureImage::SetSource( Path const & p_folder, Path const & p_relative )
	{
		m_source = std::make_unique< StaticFileTextureSource >( p_folder, p_relative );
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

	bool TextureImage::Initialise( TextureType p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		return DoCreateStorage( GetStorageType( p_type, m_index ), p_cpuAccess, p_gpuAccess );
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

	bool TextureImage::DoResetStorage()
	{
		bool l_return = true;

		if ( m_storage )
		{
			auto l_cpuAccess = m_storage->GetCPUAccess();
			auto l_gpuAccess = m_storage->GetGPUAccess();
			auto l_type = m_storage->GetType();
			Cleanup();
			l_return = DoCreateStorage( l_type, l_cpuAccess, l_gpuAccess );
		}

		return l_return;
	}

	bool TextureImage::DoCreateStorage( TextureStorageType p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
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
}
