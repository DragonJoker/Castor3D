/*
See LICENSE file in root folder
*/
#include "GraphicsModule.hpp"
#include "PixelFormat.hpp"

namespace castor
{
	namespace lanczos
	{
		inline float sinc( float fX )
		{
			if ( 0.0 == fX )
			{
				return 1.0;
			}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
			return float( sin( Pi< float > *fX ) / ( Pi< float > *fX ) );
#pragma GCC diagnostic pop
		}

		inline float weight( float fN, float fDistance )
		{
			if ( fDistance <= fN )
			{
				return sinc( fDistance ) * sinc( fDistance / fN );
			}

			return 0.0f;
		}

		template< PixelFormat PFT, typename T >
		inline T * getPixel( T * buffer
			, int32_t x
			, int32_t y
			, uint32_t width
			, uint32_t height )
		{
			return buffer + ( ( y * width ) + x ) * getBytesPerPixel( PFT );
		}

		template< PixelFormat PFT >
		void compute( int32_t x
			, int32_t y
			, uint32_t width
			, uint32_t height
			, uint8_t const * srcBuffer
			, uint8_t * dstPixel )
		{
			using MyPixelComponentsT = PixelComponentsT< PFT >;
			using MyTypeT = typename MyPixelComponentsT::Type;
			using MyLargerTypeT = LargerTypeT< MyTypeT >;
			static float constexpr coeffN = 5.0f;

			float sampleCount = 0;
			std::array< MyLargerTypeT, 4u > totalBlock{};
			auto radius = int32_t( coeffN );

			for ( int32_t j = -radius + 1; j <= radius; j++ )
			{
				for ( int32_t i = -radius + 1; i <= radius; i++ )
				{
					auto curX = int32_t( x + i );
					auto curY = int32_t( y + j );

					if ( curX >= 0
						&& curY >= 0
						&& uint32_t( curX ) <= width - 1
						&& uint32_t( curY ) <= height - 1 )
					{
						auto * srcPixel = getPixel< PFT >( srcBuffer, curX, curY, width, height );
						auto deltaX = float( x - curX );
						auto deltaY = float( y - curY );
						auto curWeight = lanczos::weight( coeffN, float( fabs( deltaX ) ) )
							* lanczos::weight( coeffN, float( fabs( deltaY ) ) );

						totalBlock[0] += MyLargerTypeT( curWeight * MyPixelComponentsT::R( srcPixel ) );

						if constexpr ( getComponentsCount( PFT ) >= 2 )
						{
							totalBlock[1] += MyLargerTypeT( curWeight * MyPixelComponentsT::G( srcPixel ) );

							if constexpr ( getComponentsCount( PFT ) >= 3 )
							{
								totalBlock[2] += MyLargerTypeT( curWeight * MyPixelComponentsT::B( srcPixel ) );

								if constexpr ( getComponentsCount( PFT ) >= 4 )
								{
									totalBlock[3] += MyLargerTypeT( curWeight * MyPixelComponentsT::A( srcPixel ) );
								}
							}
						}

						sampleCount += curWeight;
					}
				}
			}

			auto scaleFactor = float( 1.0 / sampleCount );

			MyPixelComponentsT::R( dstPixel
				, MyTypeT( scaleFactor * totalBlock[0] ) );

			if constexpr ( getComponentsCount( PFT ) >= 2 )
			{
				MyPixelComponentsT::G( dstPixel
					, MyTypeT( scaleFactor * totalBlock[1] ) );

				if constexpr ( getComponentsCount( PFT ) >= 3 )
				{
					MyPixelComponentsT::B( dstPixel
						, MyTypeT( scaleFactor * totalBlock[2] ) );

					if constexpr ( getComponentsCount( PFT ) >= 4 )
					{
						MyPixelComponentsT::A( dstPixel
							, MyTypeT( scaleFactor * totalBlock[3] ) );
					}
				}
			}
		}
	}

	template< PixelFormat PFT >
	void KernelLanczosFilterT< PFT >::compute( VkExtent2D const & fullExtent
		, uint8_t const * srcBuffer
		, uint8_t * dstBuffer
		, uint32_t level
		, uint32_t levelSize )
	{
		auto srcLevelExtent = ashes::getSubresourceDimensions( fullExtent, level - 1u, VkFormat( PFT ) );
		auto dstLevelExtent = ashes::getSubresourceDimensions( fullExtent, level, VkFormat( PFT ) );
		auto pixelSize = getBytesPerPixel( PFT );
		auto dstLineSize = pixelSize * dstLevelExtent.width;

		for ( int32_t y = 0; y < srcLevelExtent.width; y += 2 )
		{
			auto dstLine = dstBuffer;

			for ( int32_t x = 0; x < srcLevelExtent.height; x += 2 )
			{
				lanczos::compute< PFT >( x
					, y
					, srcLevelExtent.width
					, srcLevelExtent.height
					, srcBuffer
					, dstLine );
				dstLine += pixelSize;
			}

			dstBuffer += dstLineSize;
		}
	}
}
