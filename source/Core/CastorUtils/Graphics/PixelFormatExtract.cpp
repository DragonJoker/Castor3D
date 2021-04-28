#include "CastorUtils/Graphics/PixelFormat.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#include <ashes/common/Format.hpp>

namespace castor
{
	namespace
	{
		template< PixelFormat PfT >
		struct MaxValueT
		{
			using TypeT = typename PixelComponentsT< PfT >::Type;
			static TypeT constexpr value = std::numeric_limits< TypeT >::max();
		};

		template< PixelFormat PfSrcT
			, PixelComponent PcSrcT
			, PixelComponent PcDstT >
		void copyComponentT( uint8_t const * srcBuffer
			, uint32_t srcPixelSize
			, uint8_t * dstBuffer
			, uint32_t dstPixelSize
			, PixelFormat dstPixelFormat
			, uint32_t componentSize
			, size_t count )
		{
			auto dstIndex = getComponentIndex( PcDstT, dstPixelFormat );
			dstBuffer += dstIndex * componentSize;

			if ( hasComponent( PfSrcT, PcSrcT ) )
			{
				auto srcIndex = getComponentIndex( PcSrcT, PfSrcT );
				srcBuffer += srcIndex * componentSize;

				for ( size_t i = 0; i < count; ++i )
				{
					std::memcpy( dstBuffer, srcBuffer, componentSize );
					dstBuffer += dstPixelSize;
					srcBuffer += srcPixelSize;
				}
			}
			else if constexpr ( PcDstT == PixelComponent::eAlpha )
			{
				for ( size_t i = 0; i < count; ++i )
				{
					std::memcpy( dstBuffer, &MaxValueT< PfSrcT >::value, componentSize );
					dstBuffer += dstPixelSize;
				}
			}
			else
			{
				for ( size_t i = 0; i < count; ++i )
				{
					std::memset( dstBuffer, 0, componentSize );
					dstBuffer += dstPixelSize;
				}
			}
		}

		template< PixelFormat PfSrcT
			, PixelComponent PcSrcT >
		void copyComponentSrcT( uint8_t const * srcBuffer
			, uint32_t srcPixelSize
			, uint8_t * dstBuffer
			, uint32_t dstPixelSize
			, PixelComponent dstComponent
			, PixelFormat dstPixelFormat
			, uint32_t componentSize
			, size_t count )
		{
			switch ( dstComponent )
			{
			case PixelComponent::eRed:
				copyComponentT< PfSrcT, PcSrcT, PixelComponent::eRed >( srcBuffer
					, srcPixelSize
					, dstBuffer
					, dstPixelSize
					, dstPixelFormat
					, componentSize
					, count );
				break;
			case PixelComponent::eGreen:
				copyComponentT< PfSrcT, PcSrcT, PixelComponent::eGreen >( srcBuffer
					, srcPixelSize
					, dstBuffer
					, dstPixelSize
					, dstPixelFormat
					, componentSize
					, count );
				break;
			case PixelComponent::eBlue:
				copyComponentT< PfSrcT, PcSrcT, PixelComponent::eBlue >( srcBuffer
					, srcPixelSize
					, dstBuffer
					, dstPixelSize
					, dstPixelFormat
					, componentSize
					, count );
				break;
			case PixelComponent::eAlpha:
				copyComponentT< PfSrcT, PcSrcT, PixelComponent::eAlpha >( srcBuffer
					, srcPixelSize
					, dstBuffer
					, dstPixelSize
					, dstPixelFormat
					, componentSize
					, count );
				break;
			}
		}

		template< PixelFormat PfSrcT >
		void copyComponentFmtT( uint8_t const * srcBuffer
			, uint32_t srcPixelSize
			, PixelComponent srcComponent
			, uint8_t * dstBuffer
			, uint32_t dstPixelSize
			, PixelComponent dstComponent
			, PixelFormat dstPixelFormat
			, uint32_t componentSize
			, size_t count )
		{
			switch ( srcComponent )
			{
			case PixelComponent::eRed:
				copyComponentSrcT< PfSrcT, PixelComponent::eRed >( srcBuffer
					, srcPixelSize
					, dstBuffer
					, dstPixelSize
					, dstComponent
					, dstPixelFormat
					, componentSize
					, count );
				break;
			case PixelComponent::eGreen:
				copyComponentSrcT< PfSrcT, PixelComponent::eGreen >( srcBuffer
					, srcPixelSize
					, dstBuffer
					, dstPixelSize
					, dstComponent
					, dstPixelFormat
					, componentSize
					, count );
				break;
			case PixelComponent::eBlue:
				copyComponentSrcT< PfSrcT, PixelComponent::eBlue >( srcBuffer
					, srcPixelSize
					, dstBuffer
					, dstPixelSize
					, dstComponent
					, dstPixelFormat
					, componentSize
					, count );
				break;
			case PixelComponent::eAlpha:
				copyComponentSrcT< PfSrcT, PixelComponent::eAlpha >( srcBuffer
					, srcPixelSize
					, dstBuffer
					, dstPixelSize
					, dstComponent
					, dstPixelFormat
					, componentSize
					, count );
				break;
			}
		}

		void copyComponent( PxBufferBase const & src
			, PixelComponent srcComponent
			, PxBufferBase & dst
			, PixelComponent dstComponent )
		{
			switch ( src.getFormat() )
			{
#define CUPF_ENUM_VALUE_COLOR( name, value, components, alpha )\
		case PixelFormat::e##name:\
			copyComponentFmtT< PixelFormat::e##name >( src.getConstPtr()\
				, PixelDefinitionsT< PixelFormat::e##name >::Size\
				, srcComponent\
				, dst.getPtr()\
				, getBytesPerPixel( dst.getFormat() )\
				, dstComponent\
				, dst.getFormat()\
				, getBytesPerPixel( singleComponentV< PixelFormat::e##name > )\
				, dst.getCount() );\
			break;
#include "CastorUtils/Graphics/PixelFormat.enum"
			default:
				break;
			}
		}
	}

	PxBufferBaseSPtr extractComponent( PxBufferBaseSPtr src
		, PixelComponent component )
	{
		src = decompressBuffer( src );
		auto result = PxBufferBase::create( src->getDimensions()
			, getSingleComponent( src->getFormat() ) );
		copyComponent( *src
			, component
			, *result
			, PixelComponent::eRed );
		return result;
	}

	PxBufferBaseSPtr extractComponents( PxBufferBaseSPtr src
		, PixelComponents components )
	{
		src = decompressBuffer( src );
		auto result = PxBufferBase::create( src->getDimensions()
			, getPixelFormat( src->getFormat(), components ) );
		auto dstIndex = 0u;

		for ( auto component : components )
		{
			copyComponent( *src
				, component
				, *result
				, getIndexComponent( dstIndex, result->getFormat() ) );
			++dstIndex;
		}

		return result;
	}

	void copyBufferComponents( PixelComponents srcComponents
		, PixelComponents dstComponents
		, PxBufferBase const & srcBuffer
		, PxBufferBase & dstBuffer )
	{
		auto srcComponentIt = srcComponents.begin();
		auto dstComponentIt = dstComponents.begin();

		while ( srcComponentIt != srcComponents.end()
			&& dstComponentIt != dstComponents.end() )
		{
			copyComponent( srcBuffer
				, *srcComponentIt
				, dstBuffer
				, *dstComponentIt );
			++srcComponentIt;
			++dstComponentIt;
		}
	}
}
