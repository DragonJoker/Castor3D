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
				m_format = convert( buffer->format() );
				m_size = renderer::Extent3D{ buffer->dimensions()[0], buffer->dimensions()[1], 1u };
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
				auto size = renderer::Extent3D{ buffer->dimensions()[0], buffer->dimensions()[1], 1u };

				if ( doAdjustDimensions( size ) )
				{
					Image img( cuT( "Tmp" ), *buffer );
					m_buffer = img.resample( { size.width, size.height } ).getPixels();
				}
				else
				{
					m_buffer = buffer;
				}

				m_format = convert( m_buffer->format() );
				m_size = renderer::Extent3D{ m_buffer->dimensions().getWidth(), m_buffer->dimensions().getHeight(), 1u };
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

				m_format = convert( m_buffer->format() );
				m_size = renderer::Extent3D{ m_buffer->dimensions().getWidth(), m_buffer->dimensions().getHeight(), 1u };
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
				m_format = convert( m_buffer->format() );
				m_size = renderer::Extent3D{ m_buffer->dimensions().getWidth(), m_buffer->dimensions().getHeight(), 1u };
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
				auto size = renderer::Extent3D{ dimensions[0], dimensions[1], dimensions[2] };

				if ( doAdjustDimensions( size ) )
				{
					Image img( cuT( "Tmp" ), *buffer );
					m_buffer = img.resample( { size.width, size.height } ).getPixels();
				}
				else
				{
					m_buffer = buffer;
				}

				m_format = convert( m_buffer->format() );
				m_size = size;
			}

			virtual uint32_t getDepth()const
			{
				return m_size.depth;
			}

			String toString()const override
			{
				return String{};
			}
		};

		//*********************************************************************************************

		class DynamicTextureSource
			: public TextureSource
		{
		public:
			DynamicTextureSource( Engine & engine
				, Size const & dimensions
				, renderer::Format format )
				: TextureSource{ engine }
			{
				m_format = format;
				m_size = renderer::Extent3D{ dimensions[0], dimensions[1], 1u };
			}

			bool isStatic()const override
			{
				return false;
			}

			inline PxBufferBaseSPtr getBuffer()const override
			{
				auto size = renderer::Extent3D{ m_buffer->dimensions()[0]
					, m_buffer->dimensions()[1]
					, m_buffer->dimensions()[2] };

				if ( !m_buffer || size!= m_size )
				{
					m_buffer = PxBufferBase::create( { m_size.width, m_size.height }, convert( m_format ) );
				}

				return m_buffer;
			}

			inline void setBuffer( PxBufferBaseSPtr buffer ) override
			{
				m_format = convert( buffer->format() );
				m_size = renderer::Extent3D{ buffer->dimensions()[0], buffer->dimensions()[1], 1u };
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
				, renderer::Format format )
				: DynamicTextureSource{ engine, dimensions, format }
			{
				doAdjustDimensions( m_size );
			}

			virtual uint32_t getDepth()const
			{
				return 1u;
			}

			String toString()const override
			{
				return string::toString( m_size.width )
					+ cuT( "x" ) + string::toString( m_size.height );
			}
		};

		//*********************************************************************************************

		class Dynamic3DTextureSource
			: public DynamicTextureSource
		{
		public:
			Dynamic3DTextureSource( Engine & engine
				, Point3ui const & dimensions
				, renderer::Format format )
				: DynamicTextureSource{ engine, Size{ dimensions[0], dimensions[1] }, format }
			{
				m_size.depth = dimensions[2];
				doAdjustDimensions( m_size );
			}

			virtual uint32_t getDepth()const
			{
				return m_size.depth;
			}

			String toString()const override
			{
				return string::toString( m_size.width )
					+ cuT( "x" ) + string::toString( m_size.height )
					+ cuT( "x" ) + string::toString( m_size.depth );
			}
		};
	}

	//*********************************************************************************************

	bool TextureSource::doAdjustDimensions( renderer::Extent3D & extent )
	{
		bool result = false;

		if ( !m_engine.getRenderSystem()->getGpuInformations().hasNonPowerOfTwoTextures() )
		{
			renderer::Extent3D adjustedExtent{ getNext2Pow( extent.width )
				, getNext2Pow( extent.height )
				, getNext2Pow( extent.depth ) };
			result = adjustedExtent != extent;
			extent = adjustedExtent;
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
		, renderer::ImageViewCreateInfo info
		, uint32_t index )
		: OwnedBy< TextureLayout >{ layout }
		, m_index{ index }
		, m_info{ std::move( info ) }
	{
	}

	bool TextureView::initialise()
	{
		auto & device = *getOwner()->getRenderSystem()->getCurrentDevice();
		m_view = getOwner()->getTexture().createView( m_info );
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
		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
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

		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
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

		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}

	void TextureView::setBuffer( PxBufferBaseSPtr buffer )
	{
		m_source->setBuffer( buffer );
		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}
}
