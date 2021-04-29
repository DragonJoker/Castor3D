/*
See LICENSE file in root folder
*/
namespace castor
{
	namespace box
	{
		template< PixelFormat PFT >
		void compute( uint8_t const * srcPixel1
			, uint8_t const * srcPixel2
			, uint8_t const * srcPixel3
			, uint8_t const * srcPixel4
			, uint8_t * dstPixel )
		{
			using MyPixelComponentsT = PixelComponentsT< PFT >;
			using MyTypeT = typename MyPixelComponentsT::Type;
			using MyLargerTypeT = LargerTypeT< MyTypeT >;

			MyPixelComponentsT::R( dstPixel
				, MyTypeT( ( MyLargerTypeT( MyPixelComponentsT::R( srcPixel1 ) )
					+ MyLargerTypeT( MyPixelComponentsT::R( srcPixel2 ) )
					+ MyLargerTypeT( MyPixelComponentsT::R( srcPixel3 ) )
					+ MyLargerTypeT( MyPixelComponentsT::R( srcPixel4 ) ) )
					/ 4u ) );

			if constexpr ( getComponentsCount( PFT ) >= 2 )
			{
				MyPixelComponentsT::G( dstPixel
					, MyTypeT( ( MyLargerTypeT( MyPixelComponentsT::G( srcPixel1 ) )
						+ MyLargerTypeT( MyPixelComponentsT::G( srcPixel2 ) )
						+ MyLargerTypeT( MyPixelComponentsT::G( srcPixel3 ) )
						+ MyLargerTypeT( MyPixelComponentsT::G( srcPixel4 ) ) )
						/ 4u ) );

				if constexpr ( getComponentsCount( PFT ) >= 3 )
				{
					MyPixelComponentsT::B( dstPixel
						, MyTypeT( ( MyLargerTypeT( MyPixelComponentsT::B( srcPixel1 ) )
							+ MyLargerTypeT( MyPixelComponentsT::B( srcPixel2 ) )
							+ MyLargerTypeT( MyPixelComponentsT::B( srcPixel3 ) )
							+ MyLargerTypeT( MyPixelComponentsT::B( srcPixel4 ) ) )
							/ 4u ) );

					if constexpr ( getComponentsCount( PFT ) >= 4 )
					{
						MyPixelComponentsT::A( dstPixel
							, MyTypeT( ( MyLargerTypeT( MyPixelComponentsT::A( srcPixel1 ) )
								+ MyLargerTypeT( MyPixelComponentsT::A( srcPixel2 ) )
								+ MyLargerTypeT( MyPixelComponentsT::A( srcPixel3 ) )
								+ MyLargerTypeT( MyPixelComponentsT::A( srcPixel4 ) ) )
								/ 4u ) );
					}
				}
			}
		}
	}

	template< PixelFormat PFT >
	void KernelBoxFilterT< PFT >::compute( VkExtent2D const & fullExtent
		, uint8_t const * srcBuffer
		, uint8_t * dstBuffer
		, uint32_t level
		, uint32_t levelSize )
	{
		auto pixelSize = getBytesPerPixel( PFT );
		auto srcLevelExtent = ashes::getSubresourceDimensions( fullExtent, level - 1u, VkFormat( PFT ) );
		auto dstLevelExtent = ashes::getSubresourceDimensions( fullExtent, level, VkFormat( PFT ) );
		auto srcLineSize = pixelSize * srcLevelExtent.width;
		auto dstLineSize = pixelSize * dstLevelExtent.width;

		for ( auto y = 0u; y < dstLevelExtent.height; ++y )
		{
			auto srcLine0 = srcBuffer;
			auto srcLine1 = srcBuffer + srcLineSize;
			auto dstLine = dstBuffer;

			for ( auto x = 0u; x < dstLevelExtent.width; ++x )
			{
				box::compute< PFT >( srcLine0
					, srcLine0 + pixelSize
					, srcLine1
					, srcLine1 + pixelSize
					, dstLine );
				srcLine0 += 2 * pixelSize;
				srcLine1 += 2 * pixelSize;
				dstLine += pixelSize;
			}

			srcBuffer += 2 * srcLineSize;
			dstBuffer += dstLineSize;
		}
	}
}
