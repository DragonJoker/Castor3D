#ifndef ___CU_FractalNoise_HPP___
#define ___CU_FractalNoise_HPP___

#include "NoiseModule.hpp"

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
			, NoiseT noise );
		TypeT noise( TypeT x, TypeT y, TypeT z );

	private:
		NoiseT m_noise;
		uint32_t m_octaves;
		TypeT m_frequency{ TypeT( 1.0 ) };
		TypeT m_amplitude{ TypeT( 1.0 ) };
		TypeT m_persistence{ TypeT( 0.5 ) };
	};

	template< typename NoiseT >
	FractalNoiseT< NoiseT >::FractalNoiseT( uint32_t octaves
		, NoiseT noise )
		: m_noise{ castor::move( noise ) }
		, m_octaves{ octaves }
	{
	}

	template< typename NoiseT >
	typename NoiseT::TypeT FractalNoiseT< NoiseT >::noise( typename NoiseT::TypeT x
		, typename NoiseT::TypeT y
		, typename NoiseT::TypeT z )
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

	template< typename NoiseT >
	FractalNoiseT< NoiseT > makeFractalNoise( uint32_t octaves
		, NoiseT noise )
	{
		return FractalNoiseT< NoiseT >{ octaves
			, castor::move( noise ) };
	}
}

#endif
