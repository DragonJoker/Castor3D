#ifndef ___CU_WhiteNoise_HPP___
#define ___CU_WhiteNoise_HPP___

#include "NoiseModule.hpp"

#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Graphics/PixelFormat.hpp"

#include <cstdint>
#include <random>

namespace castor
{
	class WhiteNoise
	{
	private:
		struct Color
		{
			uint8_t R;
			uint8_t G;
			uint8_t B;
			uint8_t A;
		};

	public:
		static PxBufferBaseSPtr generate( uint32_t imgSize )
		{
			const uint32_t samples = 4096; // 128x128

			auto buffer = PxBufferBase::create( Size{ imgSize, imgSize }
				, PixelFormat::eR8G8B8A8_UNORM );
			uint32_t bpp = getBytesPerPixel( buffer->getFormat() );
			auto pitch = imgSize * bpp;

			// init random number generator
			std::random_device rd;
			std::mt19937 rng( rd() );
			std::uniform_int_distribution< int > dist( 0, imgSize - 1 );

			for ( uint32_t i = 1; i <= samples; ++i )
			{
				uint32_t x = dist( rng );
				uint32_t y = dist( rng );

				Color * pixel = ( Color * )&buffer->getPtr()[y * pitch + x * bpp];
				pixel->R = 255;
				pixel->G = 255;
				pixel->B = 255;
				pixel->A = 255;
			}
		}
	};
}

#endif
