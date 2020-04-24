#ifndef ___CU_FractalNoise_HPP___
#define ___CU_FractalNoise_HPP___

#include <cstdint>
#include <utility>

namespace castor
{
	template< typename NoiseT >
	class FractalNoiseT
	{
	private:
		using TypeT = typename NoiseT::TypeT;

	public:
		FractalNoiseT( uint32_t octaves
			, NoiseT noise )
			: m_noise{ std::move( noise ) }
			, m_octaves{ octaves }
			, m_frequency{ TypeT( 1.0 ) }
			, m_amplitude{ TypeT( 1.0 ) }
			, m_persistence{ TypeT( 0.5 ) }
		{
		}

		TypeT noise( TypeT x, TypeT y, TypeT z )
		{
			TypeT sum = 0;
			TypeT max = TypeT( 0 );
			auto amplitude = m_amplitude;
			auto frequency = m_frequency;

			for ( uint32_t i = 0u; i < m_octaves; i++ )
			{
				sum += amplitude * m_noise.noise( x * frequency
					, y * frequency
					, z * frequency );
				max += amplitude;
				amplitude *= m_persistence;
				frequency *= TypeT( 2 );
			}

			sum = sum / max;
			return sum;
		}

	private:
		NoiseT m_noise;
		uint32_t m_octaves;
		TypeT m_frequency;
		TypeT m_amplitude;
		TypeT m_persistence;
	};

	template< typename NoiseT >
	FractalNoiseT< NoiseT > makeFractalNoise( uint32_t octaves
		, NoiseT noise )
	{
		return FractalNoiseT< NoiseT >{ octaves
			, std::move( noise ) };
	}
}

#endif