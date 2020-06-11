#ifndef ___CU_PerlinNoise_HPP___
#define ___CU_PerlinNoise_HPP___

#include "NoiseModule.hpp"

#include <cstdint>
#include <random>

namespace castor
{
	template< typename T >
	class PerlinNoiseT
	{
	public:
		using TypeT = T;

	public:
		PerlinNoiseT();
		TypeT noise( TypeT x, TypeT y, TypeT z );

	private:
		uint32_t m_permutations[512];

		int32_t fastfloor( TypeT fp );
		TypeT fade( TypeT t );
		TypeT lerp( TypeT t, TypeT a, TypeT b );
		TypeT grad( int hash, TypeT x, TypeT y, TypeT z );
	};

	template< typename TypeT >
	PerlinNoiseT< TypeT >::PerlinNoiseT()
	{
		// Generate random lookup for permutations containing all numbers from 0..255
		std::vector<uint8_t> plookup;
		plookup.resize( 256 );
		std::iota( plookup.begin(), plookup.end(), 0 );
		std::default_random_engine rndEngine( std::random_device{}( ) );
		std::shuffle( plookup.begin(), plookup.end(), rndEngine );

		for ( uint32_t i = 0; i < 256; i++ )
		{
			m_permutations[i] = m_permutations[256 + i] = plookup[i];
		}
	}

	template< typename TypeT >
	TypeT PerlinNoiseT< TypeT >::noise( TypeT x, TypeT y, TypeT z )
	{
		// Find unit cube that contains point.
		int32_t X = ( int )fastfloor( x ) & 255;
		int32_t Y = ( int )fastfloor( y ) & 255;
		int32_t Z = ( int )fastfloor( z ) & 255;
		// Find relative x,y,z of point in cube.
		x -= fastfloor( x );
		y -= fastfloor( y );
		z -= fastfloor( z );
		// Compute fade curves for each of x,y,z.
		TypeT u = fade( x );
		TypeT v = fade( y );
		TypeT w = fade( z );
		// Hash coordinates of the 8 cube corners,
		int32_t A = m_permutations[X] + Y;
		int32_t AA = m_permutations[A] + Z;
		int32_t AB = m_permutations[A + 1] + Z;
		int32_t B = m_permutations[X + 1] + Y;
		int32_t BA = m_permutations[B] + Z;
		int32_t BB = m_permutations[B + 1] + Z;
		// and add blended results from 8 corners of cube
		return lerp( w
			, lerp( v
				, lerp( u
					, grad( m_permutations[AA], x, y, z )
					, grad( m_permutations[BA], x - 1, y, z ) )
				, lerp( u
					, grad( m_permutations[AB], x, y - 1, z )
					, grad( m_permutations[BB], x - 1, y - 1, z ) )
			)
			, lerp( v
				, lerp( u
					, grad( m_permutations[AA + 1], x, y, z - 1 )
					, grad( m_permutations[BA + 1], x - 1, y, z - 1 ) )
				, lerp( u
					, grad( m_permutations[AB + 1], x, y - 1, z - 1 )
					, grad( m_permutations[BB + 1], x - 1, y - 1, z - 1 ) )
			)
		);
	}

	template< typename TypeT >
	int32_t PerlinNoiseT< TypeT >::fastfloor( TypeT fp )
	{
		int32_t i = static_cast< int32_t >( fp );
		return ( fp < i ) ? ( i - 1 ) : ( i );
	}

	template< typename TypeT >
	TypeT PerlinNoiseT< TypeT >::fade( TypeT t )
	{
		return t * t * t * ( t * ( t * 6 - 15 ) + 10 );
	}

	template< typename TypeT >
	TypeT PerlinNoiseT< TypeT >::lerp( TypeT t, TypeT a, TypeT b )
	{
		return a + t * ( b - a );
	}

	template< typename TypeT >
	TypeT PerlinNoiseT< TypeT >::grad( int hash, TypeT x, TypeT y, TypeT z )
	{
		int h = hash & 15;
		// Convert low 4 bits of hash code into 12 gradient directions.
		TypeT u = h < 8 || h == 12 || h == 13 ? x : y;
		TypeT v = h < 4 || h == 12 || h == 13 ? y : z;
		return ( ( h & 1 ) == 0 ? u : -u ) + ( ( h & 2 ) == 0 ? v : -v );
	}
}

#endif
