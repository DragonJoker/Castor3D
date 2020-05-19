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
	private:
		static size_t constexpr c_blueNoiseSampleMultiplier = 1;

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
			const uint32_t samples1 = 256;   // 16x16
			const uint32_t samples2 = 1024;  // 32x32
			const uint32_t samples3 = 4096; // 128x128

			auto buffer = PxBufferBase::create( Size{ imgSize, imgSize }
				, PixelFormat::eR8G8B8A8_UNORM );
			uint32_t bpp = PF::getBytesPerPixel( buffer->getFormat() );
			auto pitch = imgSize * bpp;
			std::vector< Point2ui > samplesPos;

			// init random number generator
			std::random_device rd;
			std::mt19937 rng( rd() );
			std::uniform_int_distribution< int > dist( 0, imgSize - 1 );

			uint32_t percent = ( uint32_t )-1;

			for ( uint32_t i = 1; i <= samples3; ++i )
			{
				uint32_t newPercent;

				if ( i <= samples1 )
				{
					newPercent = uint32_t( 100.0f * float( i ) / float( samples1 ) );
				}
				else if ( i <= samples2 )
				{
					newPercent = uint32_t( 100.0f * float( i - samples1 ) / float( samples2 - samples1 ) );
				}
				else
				{
					newPercent = uint32_t( 100.0f * float( i - samples2 ) / float( samples3 - samples2 ) );
				}

				if ( percent != newPercent )
				{
					percent = newPercent;
					Logger::logTrace( "Generating Blue Noise Samples: " + string::toString( percent ) );
				}

				// keep the candidate that is farthest from it's closest point
				uint32_t numCandidates = samplesPos.size() * c_blueNoiseSampleMultiplier + 1;
				float bestDistance = 0.0f;
				uint32_t bestCandidateX = 0;
				uint32_t bestCandidateY = 0;

				for ( uint32_t candidate = 0; candidate < numCandidates; ++candidate )
				{
					uint32_t x = dist( rng );
					uint32_t y = dist( rng );

					// calculate the closest distance from this point to an existing sample
					float minDist = FLT_MAX;

					for ( auto & samplePos : samplesPos )
					{
						minDist = std::min( minDist
							, distance( x, y, samplePos[0], samplePos[1], imgSize ) );
					}

					if ( minDist > bestDistance )
					{
						bestDistance = minDist;
						bestCandidateX = x;
						bestCandidateY = y;
					}
				}
				samplesPos.push_back( { bestCandidateX, bestCandidateY } );

				Color * pixel = ( Color * )&buffer->getPtr()[bestCandidateY * pitch + bestCandidateX * bpp];
				pixel->R = 255;
				pixel->G = 255;
				pixel->B = 255;
				pixel->A = 255;
			}

			return buffer;
		}

	private:
		inline static float distance( uint32_t x1, uint32_t y1
			, uint32_t x2, uint32_t y2
			, int imageWidth )
		{
			// this returns the toroidal distance between the points
			// aka the interval [0, width) wraps around
			float dx = std::abs( float( x2 ) - float( x1 ) );
			float dy = std::abs( float( y2 ) - float( y1 ) );

			if ( dx > float( imageWidth / 2 ) )
				dx = float( imageWidth ) - dx;

			if ( dy > float( imageWidth / 2 ) )
				dy = float( imageWidth ) - dy;

			// returning squared distance cause why not
			return dx * dx + dy * dy;
		}
	};
}

#endif
