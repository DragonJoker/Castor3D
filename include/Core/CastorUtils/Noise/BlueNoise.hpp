#ifndef ___CU_BlueNoise_HPP___
#define ___CU_BlueNoise_HPP___

#include "NoiseModule.hpp"

#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Graphics/PixelFormat.hpp"
#include "CastorUtils/Math/Point.hpp"
#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <random>
#include <array>

namespace castor
{
	class BlueNoise
	{
	public:
		static float generate( uint32_t x, uint32_t y )
		{
			auto x0 = x;
			auto y0 = y;
			adjust( x, y );

			// constants of 2d Roberts sequence rounded to nearest primes
			const uint32_t r0 = 3242174893u;// prime[(2^32-1) / phi_2  ]
			const uint32_t r1 = 2447445397u;// prime[(2^32-1) / phi_2^2]

			// h = high-freq dither noise
			uint32_t h = ( x * r0 ) + ( y * r1 );

			uint32_t l;
			{
				x = x0 >> 2u;
				y = y0 >> 2u;
				adjust( x, y );

				uint32_t h = ( x * r0 ) + ( y * r1 );
				h = h ^ 0xE2E17FDCu;

				l = h;

				{
					x = x0 >> 4u;
					y = y0 >> 4u;
					adjust( x, y );

					uint32_t h = ( x * r0 ) + ( y * r1 );
					h = h ^ 0x1B98264Du;

					l += h;
				}
			}

			// combine low and high
			return float( float( l + h * 1u ) * ( 1.0 / 4294967296.0 ) );
		}

	private:
		static inline void adjust( uint32_t & x, uint32_t & y )
		{
			if ( ( ( x ^ y ) & 4u ) == 0u )
			{
			// flip every other tile to reduce anisotropy
				std::swap( x, y );
			}

			if ( ( ( y ) & 4u ) == 0u )
			{
				x = -x;// more iso but also more low-freq content
			}
		}
	};
}

#endif
