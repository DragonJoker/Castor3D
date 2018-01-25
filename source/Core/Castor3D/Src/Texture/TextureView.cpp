#include "TextureView.hpp"

#include "Engine.hpp"
#include "TextureLayout.hpp"

#include <Core/Device.hpp>
#include <Image/Texture.hpp>

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
			explicit StaticTextureSource( Engine & engine )
				: TextureSource{ engine }
			{
			}

			bool isStatic()const override
			{
				return true;
			}

			inline PxBufferBaseSPtr getBuffer()const override
			{
				return m_buffer;
			}

			inline void setBuffer( PxBufferBaseSPtr buffer ) override
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

			String toString()const override
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
						Size adjustedSize{ getNext2Pow( size.getWidth() ), getNext2Pow( size.getHeight() ) };

						if ( adjustedSize != size )
						{
							m_buffer = image->resample( adjustedSize ).getPixels();
						}
						else
						{
							m_buffer = buffer;
						}
					}
				}

				if ( !m_buffer )
				{
					CASTOR_EXCEPTION( cuT( "TextureView::setSource - Couldn't load image " ) + relative );
				}

				m_format = m_buffer->format();
				m_size = m_buffer->dimensions();
			}

			virtual uint32_t getDepth()const
			{
				return 1u;
			}

			String toString()const override
			{
				return m_folder / m_relative;
			}

			inline void setBuffer( PxBufferBaseSPtr buffer ) override
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

			String toString()const override
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

			bool isStatic()const override
			{
				return false;
			}

			inline PxBufferBaseSPtr getBuffer()const override
			{
				if ( !m_buffer || m_buffer->dimensions() != m_size )
				{
					m_buffer = PxBufferBase::create( m_size, m_format );
				}

				return m_buffer;
			}

			inline void setBuffer( PxBufferBaseSPtr buffer ) override
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

			String toString()const override
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

			String toString()const override
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

	TextureView::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< TextureView >{ tabs }
	{
	}

	bool TextureView::TextWriter::operator()( TextureView const & obj, TextFile & file )
	{
		return file.writeText( obj.m_source->toString() ) > 0;
	}

	//*********************************************************************************************

	TextureView::TextureView( TextureLayout & layout
		, renderer::TextureType type
		, uint32_t baseMipLevel
		, uint32_t levelCount
		, uint32_t baseArrayLayer
		, uint32_t layerCount
		, uint32_t index )
		: OwnedBy< TextureLayout >{ layout }
		, m_index{ index }
		, m_baseMipLevel{ baseMipLevel }
		, m_levelCount{ levelCount }
		, m_baseArrayLayer{ baseArrayLayer }
		, m_layerCount{ layerCount }
		, m_type{ type }
	{
	}

	bool TextureView::initialise()
	{
		auto & device = *getOwner()->getRenderSystem()->getCurrentDevice();
		m_view = getOwner()->getTexture().createView( m_type
			, getOwner()->getPixelFormat()
			, m_baseMipLevel
			, m_levelCount
			, m_baseArrayLayer
			, m_layerCount );
		return m_view != nullptr;
	}

	void TextureView::cleanup()
	{
		m_view.reset();
	}

	void TextureView::initialiseSource( Path const & folder
		, Path const & relative )
	{
		m_source = std::make_unique< StaticFileTextureSource >( *getOwner()->getRenderSystem()->getEngine()
			, folder
			, relative );
		getOwner()->doUpdateFromFirstImage( m_source->getDimensions(), m_source->getPixelFormat() );
	}

	void TextureView::initialiseSource( PxBufferBaseSPtr buffer )
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

	void TextureView::initialiseSource()
	{
		if ( getOwner()->getType() == renderer::TextureType::e3D )
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

	void TextureView::setBuffer( PxBufferBaseSPtr buffer )
	{
		m_source->setBuffer( buffer );
		getOwner()->doUpdateFromFirstImage( m_source->getDimensions(), m_source->getPixelFormat() );
	}
}
