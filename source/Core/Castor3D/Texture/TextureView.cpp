#include "Castor3D/Texture/TextureView.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Image/Image.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#define C3D_HasNonPOT 1

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

			inline PxBufferBaseSPtr getBuffer( uint32_t level )const override
			{
				CU_Require( level < m_buffers.size() );
				return m_buffers[level];
			}

			inline castor::PxBufferPtrArray const & getBuffers()const override
			{
				return m_buffers;
			}

			inline castor::PxBufferPtrArray & getBuffers()override
			{
				return m_buffers;
			}

			inline void setBuffer( PxBufferBaseSPtr buffer
				, uint32_t level )override
			{
				CU_Require( level < m_buffers.size() );
				m_buffers[level] = buffer;
				m_format = convert( buffer->getFormat() );
				m_size = VkExtent3D{ buffer->getWidth(), buffer->getHeight(), 1u };
			}

		protected:
			PxBufferPtrArray m_buffers;
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
				auto size = VkExtent3D{ buffer->getWidth(), buffer->getHeight(), 1u };

				if ( doAdjustDimensions( size ) )
				{
					Image img( cuT( "Tmp" ), *buffer );
					m_buffers.emplace_back( img.resample( { size.width, size.height } ).getPixels() );
				}
				else
				{
					m_buffers.emplace_back( buffer );
				}

				auto & myBuffer = m_buffers.front();
				m_format = convert( myBuffer->getFormat() );
				m_size = VkExtent3D{ myBuffer->getWidth(), myBuffer->getHeight(), 1u };
			}

			virtual uint32_t getDepth()const
			{
				return 1u;
			}

			String toString()const override
			{
				auto stream = castor::makeStringStream();
				stream << m_size.width << cuT( "x" ) << m_size.height << cuT( "_" ) << ashes::getName( m_format );
				return stream.str();
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
				PxBufferPtrArray buffers;

				if ( File::fileExists( folder / relative ) )
				{
					String name{ relative };

					if ( m_engine.getImageCache().has( name ) )
					{
						auto image = m_engine.getImageCache().find( name );
						buffers = image->getBuffers();
					}
					else
					{
						auto image = m_engine.getImageCache().add( name
							, folder / relative );
						buffers = image->getBuffers();

#if !C3D_HasNonPOT

						if ( !PF::isCompressed( buffers.front()->getFormat() ) )
						{
							Size size{ buffer->getDimensions() };
							Size adjustedSize{ getNext2Pow( size.getWidth() ), getNext2Pow( size.getHeight() ) };

							if ( adjustedSize != size )
							{
								buffer = image->resample( adjustedSize ).getPixels();
							}
						}

#endif
					}
				}

				if ( buffers.empty() )
				{
					CU_Exception( cuT( "TextureView::setSource - Couldn't load image " ) + relative );
				}

				m_buffers.resize( buffers.size() );
				uint32_t level = 0u;

				for ( auto & buffer : buffers )
				{
					setBuffer( buffer, level++ );
				}
			}

			virtual uint32_t getDepth()const
			{
				return 1u;
			}

			String toString()const override
			{
				return m_folder / m_relative;
			}

			inline void setBuffer( PxBufferBaseSPtr buffer
				, uint32_t level )override
			{
				CU_Require( level < m_buffers.size() );
				auto & myBuffer = m_buffers[level];
				auto srcFormat = buffer->getFormat();
				auto dstFormat = srcFormat;

				switch ( srcFormat )
				{
				case PixelFormat::eR8G8B8_UNORM:
					dstFormat = PixelFormat::eR8G8B8A8_UNORM;
					break;
				case PixelFormat::eB8G8R8_UNORM:
					dstFormat = PixelFormat::eA8B8G8R8_UNORM;
					break;
				case PixelFormat::eR8G8B8_SRGB:
					dstFormat = PixelFormat::eR8G8B8A8_SRGB;
					break;
				case PixelFormat::eB8G8R8_SRGB:
					dstFormat = PixelFormat::eA8B8G8R8_SRGB;
					break;
				case PixelFormat::eR16G16B16_SFLOAT:
					dstFormat = PixelFormat::eR16G16B16A16_SFLOAT;
					break;
				case PixelFormat::eR32G32B32_SFLOAT:
					dstFormat = PixelFormat::eR32G32B32A32_SFLOAT;
					break;
				default:
					// No conversion
					break;
				}

				if ( srcFormat != dstFormat )
				{
					myBuffer = PxBufferBase::create( buffer->getDimensions()
						, dstFormat
						, buffer->getConstPtr()
						, srcFormat );
				}
				else
				{
					myBuffer = buffer;
				}

				if ( level == 0u )
				{
					auto & myBuffer = m_buffers.front();
					m_format = convert( myBuffer->getFormat() );
					m_size = VkExtent3D{ myBuffer->getWidth(), myBuffer->getHeight(), 1u };
				}
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
				auto size = VkExtent3D{ dimensions[0], dimensions[1], dimensions[2] };

				if ( doAdjustDimensions( size ) )
				{
					Image img( cuT( "Tmp" ), *buffer );
					m_buffers.emplace_back( img.resample( { size.width, size.height } ).getPixels() );
				}
				else
				{
					m_buffers.emplace_back( buffer );
				}

				auto & myBuffer = m_buffers.front();
				m_format = convert( myBuffer->getFormat() );
				m_size = size;
			}

			virtual uint32_t getDepth()const
			{
				return m_size.depth;
			}

			String toString()const override
			{
				auto stream = castor::makeStringStream();
				stream << m_size.width << cuT( "x" ) << m_size.height << cuT( "x" ) << m_size.depth << cuT( "_" ) << ashes::getName( m_format );
				return stream.str();
			}
		};

		//*********************************************************************************************

		class DynamicTextureSource
			: public TextureSource
		{
		public:
			DynamicTextureSource( Engine & engine
				, Size const & dimensions
				, VkFormat format )
				: TextureSource{ engine }
			{
				m_format = format;
				m_size = VkExtent3D{ dimensions[0], dimensions[1], 1u };
			}

			bool isStatic()const override
			{
				return false;
			}

			inline PxBufferBaseSPtr getBuffer( uint32_t level )const override
			{
				using ashes::operator!=;

				CU_Require( level == 0u );

				if ( !m_buffers.empty() )
				{
					auto & myBuffer = m_buffers.front();
					auto size = VkExtent3D{ myBuffer->getWidth()
						, myBuffer->getHeight()
						, 1u };

					if ( size != m_size )
					{
						m_buffers.front() = PxBufferBase::create( { size.width, size.height }, convert( m_format ) );
					}
				}
				else
				{
					m_buffers.emplace_back( PxBufferBase::create( { m_size.width, m_size.height }, convert( m_format ) ) );
				}

				return m_buffers.front();
			}

			inline castor::PxBufferPtrArray const & getBuffers()const override
			{
				getBuffer( 0u );
				return m_buffers;
			}

			inline castor::PxBufferPtrArray & getBuffers()override
			{
				getBuffer( 0u );
				return m_buffers;
			}

			inline void setBuffer( PxBufferBaseSPtr buffer
				, uint32_t level )override
			{
				CU_Require( level == 0u );
				m_format = convert( buffer->getFormat() );
				m_size = VkExtent3D{ buffer->getWidth(), buffer->getHeight(), 1u };
			}

		protected:
			mutable PxBufferPtrArray m_buffers;
		};

		//*********************************************************************************************

		class Dynamic2DTextureSource
			: public DynamicTextureSource
		{
		public:
			Dynamic2DTextureSource( Engine & engine
				, Size const & dimensions
				, VkFormat format )
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
				auto stream = castor::makeStringStream();
				stream << m_size.width << cuT( "x" ) << m_size.height << cuT( "_" ) << ashes::getName( m_format );
				return stream.str();
			}
		};

		//*********************************************************************************************

		class Dynamic3DTextureSource
			: public DynamicTextureSource
		{
		public:
			Dynamic3DTextureSource( Engine & engine
				, Point3ui const & dimensions
				, VkFormat format )
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
				auto stream = castor::makeStringStream();
				stream << m_size.width << cuT( "x" ) << m_size.height << cuT( "x" ) << m_size.depth << cuT( "_" ) << ashes::getName( m_format );
				return stream.str();
			}
		};
	}

	//*********************************************************************************************

	bool TextureSource::doAdjustDimensions( VkExtent3D & extent )
	{
		bool result = false;

#if !C3D_HasNonPOT

		VkExtent3D adjustedExtent{ getNext2Pow( extent.width )
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
		auto & renderSystem = *getOwner()->getRenderSystem();
		auto & device = getCurrentRenderDevice( *getOwner() );
		m_info->subresourceRange.levelCount = std::min( m_info->subresourceRange.levelCount
			, getOwner()->getTexture().getMipmapLevels() );
		m_info->image = getOwner()->getTexture();
		m_view = getOwner()->getTexture().createView( m_info );
		m_needsMipmapsGeneration = false;

		if ( m_source && m_source->isStatic() )
		{
			auto staging = device->createStagingTexture( m_source->getPixelFormat()
				, { m_source->getDimensions().width, m_source->getDimensions().height } );
			ashes::ImageViewCreateInfo viewInfo
			{
				0u,
				m_info->image,
				m_view->viewType,
				m_view->format,
				m_view->components,
				m_view->subresourceRange
			};
			viewInfo->subresourceRange.baseMipLevel = 0u;
			viewInfo->subresourceRange.levelCount = 1u;

			for ( auto & buffer : m_source->getBuffers() )
			{
				auto view = m_view.image->createView( viewInfo );
				staging->uploadTextureData( *device.graphicsQueue
					, *device.graphicsCommandPool
					, m_source->getPixelFormat()
					, buffer->getConstPtr()
					, view );
				viewInfo->subresourceRange.baseMipLevel++;
			}

			m_needsMipmapsGeneration = getLevelCount() <= 1u
				|| ( getLevelCount() > 1
					&& viewInfo->subresourceRange.baseMipLevel < getLevelCount() );
		}
		else
		{
			m_needsMipmapsGeneration = m_info->subresourceRange.levelCount > 1u;
		}

		return m_view != nullptr;
	}

	void TextureView::cleanup()
	{
	}

	void TextureView::initialiseSource( Path const & folder
		, Path const & relative )
	{
		m_source = std::make_unique< StaticFileTextureSource >( *getOwner()->getRenderSystem()->getEngine()
			, folder
			, relative );
		m_info->format = m_source->getPixelFormat();
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
			m_info->format = m_source->getPixelFormat();
		}
		else
		{
			m_source = std::make_unique< Static2DTextureSource >( *getOwner()->getRenderSystem()->getEngine()
				, buffer );
			m_info->format = m_source->getPixelFormat();
		}

		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}

	void TextureView::initialiseSource()
	{
		if ( getOwner()->getType() == VK_IMAGE_TYPE_3D )
		{
			m_source = std::make_unique< Dynamic3DTextureSource >( *getOwner()->getRenderSystem()->getEngine()
				, Point3ui{ getOwner()->getWidth()
					, getOwner()->getHeight()
					, getOwner()->getDepth() }
				, getOwner()->getPixelFormat() );
			m_info->format = m_source->getPixelFormat();
		}
		else
		{
			m_source = std::make_unique< Dynamic2DTextureSource >( *getOwner()->getRenderSystem()->getEngine()
				, Size{ getOwner()->getWidth(), getOwner()->getHeight() }
				, getOwner()->getPixelFormat() );
			m_info->format = m_source->getPixelFormat();
		}

		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}

	void TextureView::setBuffer( PxBufferBaseSPtr buffer )
	{
		m_source->setBuffer( buffer );
		m_info->format = m_source->getPixelFormat();
		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}

	castor::String TextureView::toString()const
	{
		castor::String result;

		if ( m_source )
		{
			result = m_source->toString();
		}
		else
		{
			auto stream = castor::makeStringStream();
			stream << ashes::getName( m_info->format )
				<< cuT( "_" ) << m_info->subresourceRange.baseArrayLayer
				<< cuT( "_" ) << m_info->subresourceRange.layerCount
				<< cuT( "_" ) << m_info->subresourceRange.baseMipLevel
				<< cuT( "_" ) << m_info->subresourceRange.levelCount;
			result = stream.str();
		}

		return result;
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
