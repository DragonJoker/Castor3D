#include "TextureImage.hpp"

#include "Engine.hpp"
#include "TextureLayout.hpp"

#include <Graphics/Image.hpp>

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
			StaticTextureSource( Engine & p_engine )
				: TextureSource{ p_engine }
			{
			}

			virtual bool IsStatic()const
			{
				return true;
			}

			bool Resize( Size const & p_size, uint32_t p_depth )
			{
				FAILURE( "Can't call Resize on a static texture source." );
				return false;
			}

			inline PxBufferBaseSPtr GetBuffer()const
			{
				return m_buffer;
			}

			inline void SetBuffer( PxBufferBaseSPtr p_buffer )
			{
				m_buffer = p_buffer;
			}

		protected:
			PxBufferBaseSPtr m_buffer;
		};

		//*********************************************************************************************

		class Static2DTextureSource
			: public StaticTextureSource
		{
		public:
			explicit Static2DTextureSource( Engine & p_engine, PxBufferBaseSPtr p_buffer )
				: StaticTextureSource{ p_engine }
			{
				Size size{ p_buffer->dimensions() };
				uint32_t depth{ 1u };

				if ( DoAdjustDimensions( size, depth ) )
				{
					Image img( cuT( "Tmp" ), *p_buffer );
					m_buffer = img.Resample( size ).GetPixels();
				}
				else
				{
					m_buffer = p_buffer;
				}

				m_format = m_buffer->format();
				m_size = m_buffer->dimensions();
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
			StaticFileTextureSource( Engine & p_engine, Path const & p_folder, Path const & p_relative )
				: StaticTextureSource{ p_engine }
				, m_folder{ p_folder }
				, m_relative{ p_relative }
			{
				if ( File::FileExists( p_folder / p_relative ) )
				{
					String name{ p_relative.GetFileName() };

					if ( m_engine.GetImageCache().has( name ) )
					{
						auto image = m_engine.GetImageCache().find( name );
						m_buffer = image->GetPixels();
					}
					else
					{
						auto image = m_engine.GetImageCache().Add( name, p_folder / p_relative );
						auto buffer = image->GetPixels();
						Size size{ buffer->dimensions() };
						uint32_t depth{ 1u };

						if ( DoAdjustDimensions( size, depth ) )
						{
							m_buffer = image->Resample( size ).GetPixels();
						}
						else
						{
							m_buffer = buffer;
						}
					}
				}

				if ( !m_buffer )
				{
					CASTOR_EXCEPTION( cuT( "TextureImage::SetSource - Couldn't load image " ) + p_relative );
				}

				m_format = m_buffer->format();
				m_size = m_buffer->dimensions();
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
			Static3DTextureSource( Engine & p_engine, Point3ui const & p_dimensions, PxBufferBaseSPtr p_buffer )
				: StaticTextureSource{ p_engine }
			{
				m_depth = p_dimensions[2];
				Size size{ p_dimensions[0], p_dimensions[1] };

				if ( DoAdjustDimensions( size, m_depth ) )
				{
					Image img( cuT( "Tmp" ), *p_buffer );
					m_buffer = img.Resample( size ).GetPixels();
				}
				else
				{
					m_buffer = p_buffer;
				}

				m_format = m_buffer->format();
				m_size = size;
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
			DynamicTextureSource( Engine & p_engine, Size const & p_dimensions, PixelFormat p_format )
				: TextureSource{ p_engine }
			{
				m_format = p_format;
				m_size = p_dimensions;
			}

			virtual bool IsStatic()const
			{
				return false;
			}

			bool Resize( Size const & p_size, uint32_t p_depth )
			{
				Size size{ p_size };
				DoAdjustDimensions( size, p_depth );
				bool result = m_size != p_size;

				if ( result )
				{
					m_size = p_size;
				}

				return result;
			}

			inline PxBufferBaseSPtr GetBuffer()const
			{
				if ( !m_buffer || m_buffer->dimensions() != m_size )
				{
					m_buffer = PxBufferBase::create( m_size, m_format );
				}

				return m_buffer;
			}

			inline void SetBuffer( PxBufferBaseSPtr p_buffer )
			{
				m_size = p_buffer->dimensions();
				m_format = p_buffer->format();
			}

		protected:
			mutable PxBufferBaseSPtr m_buffer;
		};

		//*********************************************************************************************

		class Dynamic2DTextureSource
			: public DynamicTextureSource
		{
		public:
			Dynamic2DTextureSource( Engine & p_engine, Size const & p_dimensions, PixelFormat p_format )
				: DynamicTextureSource{ p_engine, p_dimensions, p_format }
			{
				uint32_t depth{ 1u };
				DoAdjustDimensions( m_size, depth );
			}

			virtual uint32_t GetDepth()const
			{
				return 1u;
			}

			virtual String ToString()const
			{
				return string::to_string( m_size.width() )
					+ cuT( "x" ) + string::to_string( m_size.height() );
			}
		};

		//*********************************************************************************************

		class Dynamic3DTextureSource
			: public DynamicTextureSource
		{
		public:
			Dynamic3DTextureSource( Engine & p_engine, Point3ui const & p_dimensions, PixelFormat p_format )
				: DynamicTextureSource{ p_engine, Size{ p_dimensions[0], p_dimensions[1] }, p_format }
			{
				m_depth = p_dimensions[2];
				DoAdjustDimensions( m_size, m_depth );
			}

			virtual uint32_t GetDepth()const
			{
				return m_depth;
			}

			virtual String ToString()const
			{
				return string::to_string( m_size.width() )
					+ cuT( "x" ) + string::to_string( m_size.height() )
					+ cuT( "x" ) + string::to_string( m_depth );
			}

		private:
			uint32_t m_depth{ 1u };
		};
	}

	//*********************************************************************************************

	bool TextureSource::DoAdjustDimensions( Castor::Size & p_size, uint32_t & p_depth )
	{
		bool result = false;

		if ( !m_engine.GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
		{
			uint32_t depth{ GetNext2Pow( p_depth ) };
			result = p_depth != depth;
			p_depth = depth;
			Size size{ GetNext2Pow( p_size.width() ), GetNext2Pow( p_size.height() ) };
			result |= size != p_size;
		}

		p_size[1] *= p_depth;
		return result;
	}

	//*********************************************************************************************

	TextureImage::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< TextureImage >{ p_tabs }
	{
	}

	bool TextureImage::TextWriter::operator()( TextureImage const & p_obj, TextFile & p_file )
	{
		return p_file.WriteText( p_obj.m_source->ToString() ) > 0;
	}

	//*********************************************************************************************

	TextureImage::TextureImage( TextureLayout & p_layout, uint32_t p_index )
		: OwnedBy< TextureLayout >{ p_layout }
		, m_index{ p_index }
	{
	}

	void TextureImage::InitialiseSource( Path const & p_folder, Path const & p_relative )
	{
		m_source = std::make_unique< StaticFileTextureSource >( *GetOwner()->GetRenderSystem()->GetEngine()
			, p_folder
			, p_relative );
		GetOwner()->DoUpdateFromFirstImage( m_source->GetDimensions(), m_source->GetPixelFormat() );
	}

	void TextureImage::InitialiseSource( PxBufferBaseSPtr p_buffer )
	{
		if ( GetOwner()->GetDepth() > 1 )
		{
			m_source = std::make_unique< Static3DTextureSource >( *GetOwner()->GetRenderSystem()->GetEngine()
				, Point3ui{ GetOwner()->GetWidth()
					, GetOwner()->GetHeight()
					, GetOwner()->GetDepth() }
				, p_buffer );
		}
		else
		{
			m_source = std::make_unique< Static2DTextureSource >( *GetOwner()->GetRenderSystem()->GetEngine()
				, p_buffer );
		}

		GetOwner()->DoUpdateFromFirstImage( m_source->GetDimensions(), m_source->GetPixelFormat() );
	}

	void TextureImage::InitialiseSource()
	{
		m_source = std::make_unique< Dynamic2DTextureSource >( *GetOwner()->GetRenderSystem()->GetEngine()
			, Size{ GetOwner()->GetWidth(), GetOwner()->GetHeight() }
			, GetOwner()->GetPixelFormat() );
		GetOwner()->DoUpdateFromFirstImage( m_source->GetDimensions(), m_source->GetPixelFormat() );
	}

	void TextureImage::Resize( Size const & p_size )
	{
		m_source->Resize( p_size, 1u );
	}

	void TextureImage::Resize( Point3ui const & p_size )
	{
		m_source->Resize( Size{ p_size[0], p_size[1] }, p_size[2] );
	}
}
