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
			StaticTextureSource( Engine & engine )
				: TextureSource{ engine }
			{
			}

			virtual bool IsStatic()const
			{
				return true;
			}

			bool Resize( Size const & size, uint32_t depth )
			{
				FAILURE( "Can't call Resize on a static texture source." );
				return false;
			}

			inline PxBufferBaseSPtr GetBuffer()const
			{
				return m_buffer;
			}

			inline void SetBuffer( PxBufferBaseSPtr buffer )
			{
				m_buffer = buffer;
				m_format = buffer->format();
				m_size = buffer->dimensions();
			}

		protected:
			PxBufferBaseSPtr m_buffer;
		};

		//*********************************************************************************************

		class Static2DTextureSource
			: public StaticTextureSource
		{
		public:
			explicit Static2DTextureSource( Engine & engine
				, PxBufferBaseSPtr buffer )
				: StaticTextureSource{ engine }
			{
				Size size{ buffer->dimensions() };
				uint32_t depth{ 1u };

				if ( DoAdjustDimensions( size, depth ) )
				{
					Image img( cuT( "Tmp" ), *buffer );
					m_buffer = img.Resample( size ).GetPixels();
				}
				else
				{
					m_buffer = buffer;
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
			StaticFileTextureSource( Engine & engine
				, Path const & folder
				, Path const & relative )
				: StaticTextureSource{ engine }
				, m_folder{ folder }
				, m_relative{ relative }
			{
				if ( File::FileExists( folder / relative ) )
				{
					String name{ relative.GetFileName() };

					if ( m_engine.GetImageCache().has( name ) )
					{
						auto image = m_engine.GetImageCache().find( name );
						m_buffer = image->GetPixels();
					}
					else
					{
						auto image = m_engine.GetImageCache().Add( name, folder / relative );
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
					CASTOR_EXCEPTION( cuT( "TextureImage::SetSource - Couldn't load image " ) + relative );
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

			inline void SetBuffer( PxBufferBaseSPtr buffer )
			{
				m_buffer = buffer;
				m_format = buffer->format();
				m_size = buffer->dimensions();
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
			Static3DTextureSource( Engine & engine
				, Point3ui const & dimensions
				, PxBufferBaseSPtr buffer )
				: StaticTextureSource{ engine }
			{
				m_depth = dimensions[2];
				Size size{ dimensions[0], dimensions[1] };

				if ( DoAdjustDimensions( size, m_depth ) )
				{
					Image img( cuT( "Tmp" ), *buffer );
					m_buffer = img.Resample( size ).GetPixels();
				}
				else
				{
					m_buffer = buffer;
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
			DynamicTextureSource( Engine & engine
				, Size const & dimensions
				, PixelFormat format )
				: TextureSource{ engine }
			{
				m_format = format;
				m_size = dimensions;
			}

			virtual bool IsStatic()const
			{
				return false;
			}

			bool Resize( Size const & size, uint32_t p_depth )
			{
				Size adjusted{ size };
				DoAdjustDimensions( adjusted, p_depth );
				bool result = m_size != size;

				if ( result )
				{
					m_size = size;
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

			inline void SetBuffer( PxBufferBaseSPtr buffer )
			{
				m_size = buffer->dimensions();
				m_format = buffer->format();
			}

		protected:
			mutable PxBufferBaseSPtr m_buffer;
		};

		//*********************************************************************************************

		class Dynamic2DTextureSource
			: public DynamicTextureSource
		{
		public:
			Dynamic2DTextureSource( Engine & engine
				, Size const & dimensions
				, PixelFormat format )
				: DynamicTextureSource{ engine, dimensions, format }
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
			Dynamic3DTextureSource( Engine & engine
				, Point3ui const & dimensions
				, PixelFormat format )
				: DynamicTextureSource{ engine, Size{ dimensions[0], dimensions[1] }, format }
			{
				m_depth = dimensions[2];
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

	bool TextureSource::DoAdjustDimensions( Castor::Size & size
		, uint32_t & depth )
	{
		bool result = false;

		if ( !m_engine.GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
		{
			uint32_t adjustedDepth{ GetNext2Pow( depth ) };
			result = depth != adjustedDepth;
			depth = adjustedDepth;
			Size adjustedSize{ GetNext2Pow( size.width() ), GetNext2Pow( size.height() ) };
			result |= adjustedSize != size;
		}

		size[1] *= depth;
		return result;
	}

	//*********************************************************************************************

	TextureImage::TextWriter::TextWriter( String const & tabs )
		: Castor::TextWriter< TextureImage >{ tabs }
	{
	}

	bool TextureImage::TextWriter::operator()( TextureImage const & obj, TextFile & file )
	{
		return file.WriteText( obj.m_source->ToString() ) > 0;
	}

	//*********************************************************************************************

	TextureImage::TextureImage( TextureLayout & layout, uint32_t index )
		: OwnedBy< TextureLayout >{ layout }
		, m_index{ index }
	{
	}

	void TextureImage::InitialiseSource( Path const & folder
		, Path const & relative )
	{
		m_source = std::make_unique< StaticFileTextureSource >( *GetOwner()->GetRenderSystem()->GetEngine()
			, folder
			, relative );
		GetOwner()->DoUpdateFromFirstImage( m_source->GetDimensions(), m_source->GetPixelFormat() );
	}

	void TextureImage::InitialiseSource( PxBufferBaseSPtr buffer )
	{
		if ( GetOwner()->GetDepth() > 1 )
		{
			m_source = std::make_unique< Static3DTextureSource >( *GetOwner()->GetRenderSystem()->GetEngine()
				, Point3ui{ GetOwner()->GetWidth()
					, GetOwner()->GetHeight()
					, GetOwner()->GetDepth() }
				, buffer );
		}
		else
		{
			m_source = std::make_unique< Static2DTextureSource >( *GetOwner()->GetRenderSystem()->GetEngine()
				, buffer );
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

	void TextureImage::Resize( Size const & size )
	{
		m_source->Resize( size, 1u );
	}

	void TextureImage::Resize( Point3ui const & size )
	{
		m_source->Resize( Size{ size[0], size[1] }, size[2] );
	}

	void TextureImage::SetBuffer( PxBufferBaseSPtr buffer )
	{
		m_source->SetBuffer( buffer );
		GetOwner()->DoUpdateFromFirstImage( m_source->GetDimensions(), m_source->GetPixelFormat() );
	}
}
