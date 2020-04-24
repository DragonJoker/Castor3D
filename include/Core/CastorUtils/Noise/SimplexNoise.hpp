#ifndef ___CU_SimplexNoise_HPP___
#define ___CU_SimplexNoise_HPP___

#include <cstdint>
#include <random>

namespace castor
{
	template< typename T >
	class SimplexNoiseT
	{
	public:
		using TypeT = T;

	private:
		uint32_t m_permutations[512];

		uint8_t hash( int32_t i )
		{
			return m_permutations[static_cast< uint8_t >( i )];
		}

		int32_t fastfloor( TypeT fp )
		{
			int32_t i = static_cast< int32_t >( fp );
			return ( fp < i ) ? ( i - 1 ) : ( i );
		}

		TypeT grad( int32_t hash, TypeT x, TypeT y, TypeT z )
		{
			// Convert LO 4 bits of hash code into 12 gradient directions
			int h = hash & 15;
			TypeT u = h < 8 ? x : y;
			TypeT v = h < 4 ? y : h == 12 || h == 14 ? x : z;
			return ( ( h & 1 ) == 0 ? u : -u ) + ( ( h & 2 ) == 0 ? v : -v );
		}

	public:
		SimplexNoiseT()
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

		TypeT noise( TypeT x, TypeT y, TypeT z )
		{
			TypeT n0, n1, n2, n3; // Noise contributions from the four corners

			// Skewing/Unskewing factors for 3D
			static constexpr TypeT F3 = TypeT( 1.0 / 3.0 );
			static constexpr TypeT G3 = TypeT( 1.0 / 6.0 );

			// Skew the input space to determine which simplex cell we're in
			TypeT s = ( x + y + z ) * F3; // Very nice and simple skew factor for 3D
			int32_t i = fastfloor( x + s );
			int32_t j = fastfloor( y + s );
			int32_t k = fastfloor( z + s );
			TypeT t = ( i + j + k ) * G3;
			TypeT X0 = i - t; // Unskew the cell origin back to (x,y,z) space
			TypeT Y0 = j - t;
			TypeT Z0 = k - t;
			TypeT x0 = x - X0; // The x,y,z distances from the cell origin
			TypeT y0 = y - Y0;
			TypeT z0 = z - Z0;

			// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
			// Determine which simplex we are in.
			int32_t i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
			int32_t i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
			if ( x0 >= y0 )
			{
				if ( y0 >= z0 )
				{
					i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // X Y Z order
				}
				else if ( x0 >= z0 )
				{
					i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; // X Z Y order
				}
				else
				{
					i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; // Z X Y order
				}
			}
			else
			{ // x0<y0
				if ( y0 < z0 )
				{
					i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; // Z Y X order
				}
				else if ( x0 < z0 )
				{
					i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; // Y Z X order
				}
				else
				{
					i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // Y X Z order
				}
			}

			// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
			// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
			// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
			// c = 1/6.
			TypeT x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
			TypeT y1 = y0 - j1 + G3;
			TypeT z1 = z0 - k1 + G3;
			TypeT x2 = x0 - i2 + TypeT( 2 ) * G3; // Offsets for third corner in (x,y,z) coords
			TypeT y2 = y0 - j2 + TypeT( 2 ) * G3;
			TypeT z2 = z0 - k2 + TypeT( 2 ) * G3;
			TypeT x3 = x0 - TypeT( 1 ) + TypeT( 3 ) * G3; // Offsets for last corner in (x,y,z) coords
			TypeT y3 = y0 - TypeT( 1 ) + TypeT( 3 ) * G3;
			TypeT z3 = z0 - TypeT( 1 ) + TypeT( 3 ) * G3;

			// Work out the hashed gradient indices of the four simplex corners
			int gi0 = hash( i + hash( j + hash( k ) ) );
			int gi1 = hash( i + i1 + hash( j + j1 + hash( k + k1 ) ) );
			int gi2 = hash( i + i2 + hash( j + j2 + hash( k + k2 ) ) );
			int gi3 = hash( i + 1 + hash( j + 1 + hash( k + 1 ) ) );

			// Calculate the contribution from the four corners
			TypeT t0 = TypeT( 0.6 ) - x0 * x0 - y0 * y0 - z0 * z0;
			if ( t0 < 0 )
			{
				n0 = 0.0;
			}
			else
			{
				t0 *= t0;
				n0 = t0 * t0 * grad( gi0, x0, y0, z0 );
			}

			TypeT t1 = TypeT( 0.6 ) - x1 * x1 - y1 * y1 - z1 * z1;
			if ( t1 < 0 )
			{
				n1 = 0.0;
			}
			else
			{
				t1 *= t1;
				n1 = t1 * t1 * grad( gi1, x1, y1, z1 );
			}

			TypeT t2 = TypeT( 0.6 ) - x2 * x2 - y2 * y2 - z2 * z2;
			if ( t2 < 0 )
			{
				n2 = 0.0;
			}
			else
			{
				t2 *= t2;
				n2 = t2 * t2 * grad( gi2, x2, y2, z2 );
			}

			TypeT t3 = TypeT( 0.6 ) - x3 * x3 - y3 * y3 - z3 * z3;
			if ( t3 < 0 )
			{
				n3 = 0.0;
			}
			else
			{
				t3 *= t3;
				n3 = t3 * t3 * grad( gi3, x3, y3, z3 );
			}

			// Add contributions from each corner to get the final noise value.
			// The result is scaled to stay just inside [-1,1]
			return TypeT( 32.0 * ( n0 + n1 + n2 + n3 ) );
		}
	};
}

#endif
