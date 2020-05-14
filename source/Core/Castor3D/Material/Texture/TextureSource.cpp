#include "Castor3D/Material/Texture/TextureSource.hpp"

#include "Castor3D/Engine.hpp"

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>

#define C3D_HasNonPOT 1

namespace castor3d
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

		inline castor::PxBufferBaseSPtr getBuffer( uint32_t level )const override
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

		inline void setBuffer( castor::PxBufferBaseSPtr buffer
			, uint32_t level )override
		{
			CU_Require( level < m_buffers.size() );
			m_buffers[level] = buffer;
			m_format = convert( buffer->getFormat() );
			m_size = VkExtent3D{ buffer->getWidth(), buffer->getHeight(), 1u };
		}

	protected:
		castor::PxBufferPtrArray m_buffers;
	};

	//*********************************************************************************************

	class Static2DTextureSource
		: public StaticTextureSource
	{
	public:
		explicit Static2DTextureSource( Engine & engine
			, castor::PxBufferBaseSPtr buffer )
			: StaticTextureSource{ engine }
		{
			auto size = VkExtent3D{ buffer->getWidth(), buffer->getHeight(), 1u };

			if ( doAdjustDimensions( size ) )
			{
				castor::Image img( cuT( "Tmp" ), *buffer );
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

		castor::String toString()const override
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
			, castor::Path const & folder
			, castor::Path const & relative )
			: StaticTextureSource{ engine }
			, m_folder{ folder }
			, m_relative{ relative }
		{
			castor::PxBufferPtrArray buffers;

			if ( castor::File::fileExists( folder / relative ) )
			{
				castor::String name{ relative };

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

					if ( !castor::PF::isCompressed( buffers.front()->getFormat() ) )
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

		castor::String toString()const override
		{
			return m_folder / m_relative;
		}

		inline void setBuffer( castor::PxBufferBaseSPtr buffer
			, uint32_t level )override
		{
			CU_Require( level < m_buffers.size() );
			auto & myBuffer = m_buffers[level];
			auto srcFormat = buffer->getFormat();
			auto dstFormat = srcFormat;

			switch ( srcFormat )
			{
			case castor::PixelFormat::eR8G8B8_UNORM:
				dstFormat = castor::PixelFormat::eR8G8B8A8_UNORM;
				break;
			case castor::PixelFormat::eB8G8R8_UNORM:
				dstFormat = castor::PixelFormat::eA8B8G8R8_UNORM;
				break;
			case castor::PixelFormat::eR8G8B8_SRGB:
				dstFormat = castor::PixelFormat::eR8G8B8A8_SRGB;
				break;
			case castor::PixelFormat::eB8G8R8_SRGB:
				dstFormat = castor::PixelFormat::eA8B8G8R8_SRGB;
				break;
			case castor::PixelFormat::eR16G16B16_SFLOAT:
				dstFormat = castor::PixelFormat::eR16G16B16A16_SFLOAT;
				break;
			case castor::PixelFormat::eR32G32B32_SFLOAT:
				dstFormat = castor::PixelFormat::eR32G32B32A32_SFLOAT;
				break;
			default:
				// No conversion
				break;
			}

			if ( srcFormat != dstFormat )
			{
				myBuffer = castor::PxBufferBase::create( buffer->getDimensions()
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
		castor::Path m_folder;
		castor::Path m_relative;
	};

	//*********************************************************************************************

	class Static3DTextureSource
		: public StaticTextureSource
	{
	public:
		Static3DTextureSource( Engine & engine
			, castor::Point3ui const & dimensions
			, castor::PxBufferBaseSPtr buffer )
			: StaticTextureSource{ engine }
		{
			auto size = VkExtent3D{ dimensions[0], dimensions[1], dimensions[2] };

			if ( doAdjustDimensions( size ) )
			{
				castor::Image img( cuT( "Tmp" ), *buffer );
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

		castor::String toString()const override
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
			, castor::Size const & dimensions
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

		inline castor::PxBufferBaseSPtr getBuffer( uint32_t level )const override
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
					m_buffers.front() = castor::PxBufferBase::create( { size.width, size.height }, convert( m_format ) );
				}
			}
			else
			{
				m_buffers.emplace_back( castor::PxBufferBase::create( { m_size.width, m_size.height }, convert( m_format ) ) );
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

		inline void setBuffer( castor::PxBufferBaseSPtr buffer
			, uint32_t level )override
		{
			CU_Require( level == 0u );
			m_format = convert( buffer->getFormat() );
			m_size = VkExtent3D{ buffer->getWidth(), buffer->getHeight(), 1u };
		}

	protected:
		mutable castor::PxBufferPtrArray m_buffers;
	};

	//*********************************************************************************************

	class Dynamic2DTextureSource
		: public DynamicTextureSource
	{
	public:
		Dynamic2DTextureSource( Engine & engine
			, castor::Size const & dimensions
			, VkFormat format )
			: DynamicTextureSource{ engine, dimensions, format }
		{
			doAdjustDimensions( m_size );
		}

		virtual uint32_t getDepth()const
		{
			return 1u;
		}

		castor::String toString()const override
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
			, castor::Point3ui const & dimensions
			, VkFormat format )
			: DynamicTextureSource{ engine, castor::Size{ dimensions[0], dimensions[1] }, format }
		{
			m_size.depth = dimensions[2];
			doAdjustDimensions( m_size );
		}

		virtual uint32_t getDepth()const
		{
			return m_size.depth;
		}

		castor::String toString()const override
		{
			auto stream = castor::makeStringStream();
			stream << m_size.width << cuT( "x" ) << m_size.height << cuT( "x" ) << m_size.depth << cuT( "_" ) << ashes::getName( m_format );
			return stream.str();
		}
	};

	//*********************************************************************************************

	TextureSourceSPtr TextureSource::create( Engine & engine
		, castor::Path const & folder
		, castor::Path const & relative )
	{
		return std::make_shared< StaticFileTextureSource >( engine
			, folder
			, relative );
	}

	TextureSourceSPtr TextureSource::create( Engine & engine
		, castor::PxBufferBaseSPtr buffer
		, uint32_t depth )
	{
		if ( depth )
		{
			return std::make_shared< Static3DTextureSource >( engine
				, castor::Point3ui{ buffer->getWidth(), buffer->getHeight(), depth }
				, buffer );
		}

		return std::make_shared< Static2DTextureSource >( engine
			, buffer );
	}

	TextureSourceSPtr TextureSource::create( Engine & engine
		, VkExtent3D const & extent
		, VkFormat format )
	{
		if ( extent.depth > 1 )
		{
			return std::make_shared< Dynamic3DTextureSource >( engine
				, castor::Point3ui{ extent.width, extent.height, extent.depth }
				, format );
		}

		return std::make_shared< Dynamic2DTextureSource >( engine
			, castor::Size{ extent.width, extent.height }
			, format );
	}

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
}
