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
			explicit Static2DTextureSource( PxBufferBaseSPtr p_buffer )
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
			StaticFileTextureSource( Path const & p_folder, Path const & p_relative )
				: m_folder{ p_folder }
				, m_relative{ p_relative }
			{
				if ( File::FileExists( p_folder / p_relative ) )
				{
					String l_name{ p_relative.GetFileName() };

					if ( Engine::GetInstance().GetImageCache().has( l_name ) )
					{
						auto l_image = Engine::GetInstance().GetImageCache().find( l_name );
						m_buffer = l_image->GetPixels();
					}
					else
					{
						auto l_image = Engine::GetInstance().GetImageCache().Add( l_name, p_folder / p_relative );
						auto l_buffer = l_image->GetPixels();
						Size l_size{ l_buffer->dimensions() };
						uint32_t l_depth{ 1u };

						if ( DoAdjustDimensions( l_size, l_depth ) )
						{
							m_buffer = l_image->Resample( l_size ).GetPixels();
						}
						else
						{
							m_buffer = l_buffer;
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

				m_format = m_buffer->format();
				m_size = l_size;
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
			DynamicTextureSource( Size const & p_dimensions, PixelFormat p_format )
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
				Size l_size{ p_size };
				DoAdjustDimensions( l_size, p_depth );
				bool l_return = m_size != p_size;

				if ( l_return )
				{
					m_size = p_size;
				}

				return l_return;
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
			Dynamic2DTextureSource( Size const & p_dimensions, PixelFormat p_format )
				: DynamicTextureSource{ p_dimensions, p_format }
			{
				uint32_t l_depth{ 1u };
				DoAdjustDimensions( m_size, l_depth );
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
			Dynamic3DTextureSource( Point3ui const & p_dimensions, PixelFormat p_format )
				: DynamicTextureSource{ Size{ p_dimensions[0], p_dimensions[1] }, p_format }
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
		bool l_return = false;

		if ( !Engine::GetInstance().GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
		{
			uint32_t l_depth{ GetNext2Pow( p_depth ) };
			l_return = p_depth != l_depth;
			p_depth = l_depth;
			Size l_size{ GetNext2Pow( p_size.width() ), GetNext2Pow( p_size.height() ) };
			l_return |= l_size != p_size;
		}

		p_size[1] *= p_depth;
		return l_return;
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
		m_source = std::make_unique< StaticFileTextureSource >( p_folder, p_relative );
		GetOwner()->DoUpdateFromFirstImage( m_source->GetDimensions(), m_source->GetPixelFormat() );
	}

	void TextureImage::InitialiseSource( PxBufferBaseSPtr p_buffer )
	{
		if ( GetOwner()->GetDepth() > 1 )
		{
			m_source = std::make_unique< Static3DTextureSource >( Point3ui{ GetOwner()->GetWidth(), GetOwner()->GetHeight(), GetOwner()->GetDepth() }, p_buffer );
		}
		else
		{
			m_source = std::make_unique< Static2DTextureSource >( p_buffer );
		}

		GetOwner()->DoUpdateFromFirstImage( m_source->GetDimensions(), m_source->GetPixelFormat() );
	}

	void TextureImage::InitialiseSource()
	{
		m_source = std::make_unique< Dynamic2DTextureSource >( Size{ GetOwner()->GetWidth(), GetOwner()->GetHeight() }, GetOwner()->GetPixelFormat() );
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
