#include "TextureView.hpp"

#include "Engine.hpp"
#include "TextureLayout.hpp"

#include <Ashes/Image/StagingTexture.hpp>
#include <Ashes/Core/Device.hpp>
#include <Ashes/Image/Texture.hpp>
#include <Ashes/Miscellaneous/BufferImageCopy.hpp>

#include <Graphics/Image.hpp>

#define C3D_HasNonPOT 1

using namespace castor;
namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		String getName( ImageComponents value )
		{
			switch ( value )
			{
			case castor3d::ImageComponents::eR:
				return "R";
			case castor3d::ImageComponents::eRG:
				return "RG";
			case castor3d::ImageComponents::eRGB:
				return "RGB";
			case castor3d::ImageComponents::eA:
				return "A";
			case castor3d::ImageComponents::eAll:
				return "All";
			default:
				CU_Failure( "Unsupported ImageComponents" );
				return "Unknown";
			}
		}

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
				m_size = ashes::Extent3D{ buffer->dimensions()[0], buffer->dimensions()[1], 1u };
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
				auto size = ashes::Extent3D{ buffer->dimensions()[0], buffer->dimensions()[1], 1u };

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
				m_size = ashes::Extent3D{ m_buffer->dimensions().getWidth(), m_buffer->dimensions().getHeight(), 1u };
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
				, Path const & relative
				, ImageComponents components )
				: StaticTextureSource{ engine }
				, m_folder{ folder }
				, m_relative{ relative }
			{
				PxBufferBaseSPtr buffer;

				if ( File::fileExists( folder / relative ) )
				{
					String name{ relative + getName( components ) };

					if ( m_engine.getImageCache().has( name ) )
					{
						auto image = m_engine.getImageCache().find( name );
						buffer = image->getPixels();
					}
					else
					{
						auto image = m_engine.getImageCache().add( name
							, folder / relative
							, ( components != ImageComponents::eA
								&& components != ImageComponents::eAll ) );
						buffer = image->getPixels();
						Size size{ buffer->dimensions() };
						//Size adjustedSize{ getNext2Pow( size.getWidth() ), getNext2Pow( size.getHeight() ) };

						//if ( adjustedSize != size )
						//{
						//	buffer = image->resample( adjustedSize ).getPixels();
						//}
					}
				}

				if ( !buffer )
				{
					CU_Exception( cuT( "TextureView::setSource - Couldn't load image " ) + relative );
				}

				setBuffer( buffer );
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
				if ( buffer->format() == PixelFormat::eR8G8B8_UNORM )
				{
					m_buffer = PxBufferBase::create( buffer->dimensions()
						, PixelFormat::eR8G8B8A8_UNORM
						, buffer->constPtr()
						, buffer->format() );
				}
				else if ( buffer->format() == PixelFormat::eR8G8B8_SRGB )
				{
					m_buffer = PxBufferBase::create( buffer->dimensions()
						, PixelFormat::eR8G8B8A8_SRGB
						, buffer->constPtr()
						, buffer->format() );
				}
				else if ( buffer->format() == PixelFormat::eB8G8R8_UNORM )
				{
					m_buffer = PxBufferBase::create( buffer->dimensions()
						, PixelFormat::eA8B8G8R8_UNORM
						, buffer->constPtr()
						, buffer->format() );
				}
				else if ( buffer->format() == PixelFormat::eB8G8R8_SRGB )
				{
					m_buffer = PxBufferBase::create( buffer->dimensions()
						, PixelFormat::eA8B8G8R8_SRGB
						, buffer->constPtr()
						, buffer->format() );
				}
				else if ( buffer->format() == PixelFormat::eR32G32B32_SFLOAT )
				{
					m_buffer = PxBufferBase::create( buffer->dimensions()
						, PixelFormat::eR32G32B32A32_SFLOAT
						, buffer->constPtr()
						, buffer->format() );
				}
				else
				{
					m_buffer = std::move( buffer );
				}

				m_format = convert( m_buffer->format() );
				m_size = ashes::Extent3D{ m_buffer->dimensions().getWidth(), m_buffer->dimensions().getHeight(), 1u };
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
				auto size = ashes::Extent3D{ dimensions[0], dimensions[1], dimensions[2] };

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
				, ashes::Format format )
				: TextureSource{ engine }
			{
				m_format = format;
				m_size = ashes::Extent3D{ dimensions[0], dimensions[1], 1u };
			}

			bool isStatic()const override
			{
				return false;
			}

			inline PxBufferBaseSPtr getBuffer()const override
			{
				auto size = ashes::Extent3D{ m_buffer->dimensions()[0]
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
				m_size = ashes::Extent3D{ buffer->dimensions()[0], buffer->dimensions()[1], 1u };
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
				, ashes::Format format )
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
				, ashes::Format format )
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

	bool TextureSource::doAdjustDimensions( ashes::Extent3D & extent )
	{
		bool result = false;

#if !C3D_HasNonPOT
		ashes::Extent3D adjustedExtent{ getNext2Pow( extent.width )
			, getNext2Pow( extent.height )
			, getNext2Pow( extent.depth ) };
		result = adjustedExtent != extent;
		extent = adjustedExtent;
#endif

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
		, ashes::ImageViewCreateInfo info
		, uint32_t index )
		: OwnedBy< TextureLayout >{ layout }
		, m_index{ index }
		, m_info{ std::move( info ) }
	{
	}

	bool TextureView::initialise()
	{
		auto & device = getCurrentDevice( *getOwner() );
		m_view = getOwner()->getTexture().createView( m_info );

		if ( m_source && m_source->isStatic() )
		{
			auto staging = device.createStagingTexture( m_source->getPixelFormat()
				, { m_source->getDimensions().width, m_source->getDimensions().height } );
			auto commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
			staging->uploadTextureData( *commandBuffer
				, m_source->getPixelFormat()
				, m_source->getBuffer()->constPtr()
				, *m_view );
		}

		return m_view != nullptr;
	}

	void TextureView::cleanup()
	{
		m_view.reset();
	}

	void TextureView::initialiseSource( Path const & folder
		, Path const & relative
		, ImageComponents components )
	{
		m_source = std::make_unique< StaticFileTextureSource >( *getOwner()->getRenderSystem()->getEngine()
			, folder
			, relative
			, components );
		m_info.format = m_source->getPixelFormat();
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
			m_info.format = m_source->getPixelFormat();
		}
		else
		{
			m_source = std::make_unique< Static2DTextureSource >( *getOwner()->getRenderSystem()->getEngine()
				, buffer );
			m_info.format = m_source->getPixelFormat();
		}

		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}

	void TextureView::initialiseSource()
	{
		if ( getOwner()->getType() == ashes::TextureType::e3D )
		{
			m_source = std::make_unique< Dynamic3DTextureSource >( *getOwner()->getRenderSystem()->getEngine()
				, Point3ui{ getOwner()->getWidth()
					, getOwner()->getHeight()
					, getOwner()->getDepth() }
				, getOwner()->getPixelFormat() );
			m_info.format = m_source->getPixelFormat();
		}
		else
		{
			m_source = std::make_unique< Dynamic2DTextureSource >( *getOwner()->getRenderSystem()->getEngine()
				, Size{ getOwner()->getWidth(), getOwner()->getHeight() }
				, getOwner()->getPixelFormat() );
			m_info.format = m_source->getPixelFormat();
		}

		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}

	void TextureView::setBuffer( PxBufferBaseSPtr buffer )
	{
		m_source->setBuffer( buffer );
		m_info.format = m_source->getPixelFormat();
		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}

	void TextureView::doUpdate( ashes::ImageViewCreateInfo info )
	{
		m_info = info;

		if ( m_view )
		{
			m_view = getOwner()->getTexture().createView( m_info );
		}
	}
}
