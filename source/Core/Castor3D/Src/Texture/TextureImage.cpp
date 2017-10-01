#include "TextureImage.hpp"

#include "Engine.hpp"
#include "TextureLayout.hpp"

#include <Graphics/Image.hpp>

using namespace castor;
namespace castor3d
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

			virtual bool isStatic()const
			{
				return true;
			}

			bool resize( Size const & size, uint32_t depth )
			{
				FAILURE( "Can't call Resize on a static texture source." );
				return false;
			}

			inline PxBufferBaseSPtr getBuffer()const
			{
				return m_buffer;
			}

			inline void setBuffer( PxBufferBaseSPtr buffer )
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

				if ( doAdjustDimensions( size, depth ) )
				{
					Image img( cuT( "Tmp" ), *buffer );
					m_buffer = img.resample( size ).getPixels();
				}
				else
				{
					m_buffer = buffer;
				}

				m_format = m_buffer->format();
				m_size = m_buffer->dimensions();
			}

			virtual uint32_t getDepth()const
			{
				return 1u;
			}

			virtual String toString()const
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
				if ( File::fileExists( folder / relative ) )
				{
					String name{ relative.getFileName() };

					if ( m_engine.getImageCache().has( relative ) )
					{
						auto image = m_engine.getImageCache().find( relative );
						m_buffer = image->getPixels();
					}
					else
					{
						auto image = m_engine.getImageCache().add( relative, folder / relative );
						auto buffer = image->getPixels();
						Size size{ buffer->dimensions() };
						uint32_t depth{ 1u };

						if ( doAdjustDimensions( size, depth ) )
						{
							m_buffer = image->resample( size ).getPixels();
						}
						else
						{
							m_buffer = buffer;
						}
					}
				}

				if ( !m_buffer )
				{
					CASTOR_EXCEPTION( cuT( "TextureImage::setSource - Couldn't load image " ) + relative );
				}

				m_format = m_buffer->format();
				m_size = m_buffer->dimensions();
			}

			virtual uint32_t getDepth()const
			{
				return 1u;
			}

			virtual String toString()const
			{
				return m_folder / m_relative;
			}

			inline void setBuffer( PxBufferBaseSPtr buffer )
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

				if ( doAdjustDimensions( size, m_depth ) )
				{
					Image img( cuT( "Tmp" ), *buffer );
					m_buffer = img.resample( size ).getPixels();
				}
				else
				{
					m_buffer = buffer;
				}

				m_format = m_buffer->format();
				m_size = size;
			}

			virtual uint32_t getDepth()const
			{
				return m_depth;
			}

			virtual String toString()const
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

			virtual bool isStatic()const
			{
				return false;
			}

			bool resize( Size const & size, uint32_t p_depth )
			{
				Size adjusted{ size };
				doAdjustDimensions( adjusted, p_depth );
				bool result = m_size != size;

				if ( result )
				{
					m_size = size;
				}

				return result;
			}

			inline PxBufferBaseSPtr getBuffer()const
			{
				if ( !m_buffer || m_buffer->dimensions() != m_size )
				{
					m_buffer = PxBufferBase::create( m_size, m_format );
				}

				return m_buffer;
			}

			inline void setBuffer( PxBufferBaseSPtr buffer )
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
				doAdjustDimensions( m_size, depth );
			}

			virtual uint32_t getDepth()const
			{
				return 1u;
			}

			virtual String toString()const
			{
				return string::toString( m_size.getWidth() )
					+ cuT( "x" ) + string::toString( m_size.getHeight() );
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
				doAdjustDimensions( m_size, m_depth );
			}

			virtual uint32_t getDepth()const
			{
				return m_depth;
			}

			virtual String toString()const
			{
				return string::toString( m_size.getWidth() )
					+ cuT( "x" ) + string::toString( m_size.getHeight() )
					+ cuT( "x" ) + string::toString( m_depth );
			}

		private:
			uint32_t m_depth{ 1u };
		};
	}

	//*********************************************************************************************

	bool TextureSource::doAdjustDimensions( castor::Size & size
		, uint32_t & depth )
	{
		bool result = false;

		if ( !m_engine.getRenderSystem()->getGpuInformations().hasNonPowerOfTwoTextures() )
		{
			uint32_t adjustedDepth{ getNext2Pow( depth ) };
			result = depth != adjustedDepth;
			depth = adjustedDepth;
			Size adjustedSize{ getNext2Pow( size.getWidth() ), getNext2Pow( size.getHeight() ) };
			result |= adjustedSize != size;
		}

		return result;
	}

	//*********************************************************************************************

	TextureImage::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< TextureImage >{ tabs }
	{
	}

	bool TextureImage::TextWriter::operator()( TextureImage const & obj, TextFile & file )
	{
		return file.writeText( obj.m_source->toString() ) > 0;
	}

	//*********************************************************************************************

	TextureImage::TextureImage( TextureLayout & layout, uint32_t index )
		: OwnedBy< TextureLayout >{ layout }
		, m_index{ index }
	{
	}

	void TextureImage::initialiseSource( Path const & folder
		, Path const & relative )
	{
		m_source = std::make_unique< StaticFileTextureSource >( *getOwner()->getRenderSystem()->getEngine()
			, folder
			, relative );
		getOwner()->doUpdateFromFirstImage( m_source->getDimensions(), m_source->getPixelFormat() );
	}

	void TextureImage::initialiseSource( PxBufferBaseSPtr buffer )
	{
		if ( getOwner()->getDepth() > 1 )
		{
			m_source = std::make_unique< Static3DTextureSource >( *getOwner()->getRenderSystem()->getEngine()
				, Point3ui{ getOwner()->getWidth()
					, getOwner()->getHeight()
					, getOwner()->getDepth() }
				, buffer );
		}
		else
		{
			m_source = std::make_unique< Static2DTextureSource >( *getOwner()->getRenderSystem()->getEngine()
				, buffer );
		}

		getOwner()->doUpdateFromFirstImage( m_source->getDimensions(), m_source->getPixelFormat() );
	}

	void TextureImage::initialiseSource()
	{
		if ( getOwner()->getType() == TextureType::eThreeDimensions )
		{
			m_source = std::make_unique< Dynamic3DTextureSource >( *getOwner()->getRenderSystem()->getEngine()
				, Point3ui{ getOwner()->getWidth()
					, getOwner()->getHeight()
					, getOwner()->getDepth() }
				, getOwner()->getPixelFormat() );
		}
		else
		{
			m_source = std::make_unique< Dynamic2DTextureSource >( *getOwner()->getRenderSystem()->getEngine()
				, Size{ getOwner()->getWidth(), getOwner()->getHeight() }
				, getOwner()->getPixelFormat() );
		}

		getOwner()->doUpdateFromFirstImage( m_source->getDimensions(), m_source->getPixelFormat() );
	}

	void TextureImage::resize( Size const & size )
	{
		m_source->resize( size, 1u );
	}

	void TextureImage::resize( Point3ui const & size )
	{
		m_source->resize( Size{ size[0], size[1] }, size[2] );
	}

	void TextureImage::setBuffer( PxBufferBaseSPtr buffer )
	{
		m_source->setBuffer( buffer );
		getOwner()->doUpdateFromFirstImage( m_source->getDimensions(), m_source->getPixelFormat() );
	}
}
