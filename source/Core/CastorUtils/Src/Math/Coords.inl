#include "PointOperators.hpp"
#include <cstring>
#include <numeric>

namespace castor
{
	template< typename T, uint32_t Count >
	Coords< T, Count >::TextLoader::TextLoader()
		: castor::TextLoader< Coords< T, Count > >()
	{
	}

	template< typename T, uint32_t Count >
	bool Coords< T, Count >::TextLoader::operator()( Coords< T, Count > & p_object, TextFile & p_file )
	{
		String strWord;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			if ( p_file.readLine( strWord, 1024, cuT( " \r\n;\t" ) ) > 0 )
			{
				StringStream streamWord( strWord );
				streamWord >> p_object[i];
			}

			xchar cDump;
			p_file.readChar( cDump );
		}

		return true;
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	Coords< T, Count >::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< Coords< T, Count > >( p_tabs )
	{
	}

	template< typename T, uint32_t Count >
	bool Coords< T, Count >::TextWriter::operator()( Coords< T, Count > const & p_object, TextFile & p_file )
	{
		StringStream streamWord;
		streamWord.setf( std::ios::boolalpha );
		streamWord.setf( std::ios::showpoint );

		for ( uint32_t i = 0; i < Count; ++i )
		{
			if ( !streamWord.str().empty() )
			{
				streamWord << cuT( " " );
			}

			streamWord << p_object[i];
		}

		bool result = p_file.print( 1024, cuT( "%s%s" ), this->m_tabs.c_str(), streamWord.str().c_str() ) > 0;
		castor::TextWriter< Coords< T, Count > >::checkError( result, "Coords value" );
		return result;
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords()
		: m_coords( nullptr )
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( T * p_pValues )
		: m_coords( p_pValues )
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( Coords< T, Count > const & p_pt )
		: m_coords( p_pt.m_coords )
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( Coords< T, Count > && p_pt )
		: m_coords( nullptr )
	{
		m_coords = std::move( p_pt.m_coords );
		p_pt.m_coords = nullptr;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count >::~Coords()
	{
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator=( Coords< T, Count > const & p_pt )
	{
		m_coords = p_pt.m_coords;
		return *this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator=( Coords< T, Count > && p_pt )
	{
		if ( this != &p_pt )
		{
			m_coords = std::move( p_pt.m_coords );
			p_pt.m_coords = nullptr;
		}

		return *this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator=( Point< T, Count > const & p_pt )
	{
		memcpy( m_coords, p_pt.constPtr(), binary_size );
		return *this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator=( T * p_pValues )
	{
		m_coords = p_pValues;
		return *this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator+=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::add( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator-=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::sub( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator*=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::mul( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator/=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::div( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator+=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::add( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator-=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::sub( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator*=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::mul( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator/=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::div( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator+=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::add( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator-=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::sub( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator*=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::mul( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator/=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::div( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator*=( T const & p_coord )
	{
		return PtAssignOperators< T, T, Count, Count >::mul( *this, p_coord );
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator/=( T const & p_coord )
	{
		return PtAssignOperators< T, T, Count, Count >::div( *this, p_coord );
	}

	template< typename T, uint32_t Count >
	void Coords< T, Count >::swap( Coords< T, Count > & p_pt )
	{
		std::swap( m_coords, p_pt.m_coords );
	}

	template< typename T, uint32_t Count >
	void Coords< T, Count >::flip()
	{
		for ( uint32_t i = 0; i < Count / 2; i++ )
		{
			std::swap( m_coords[i], m_coords[Count - 1 - i] );
		}
	}

	template< typename T, uint32_t Count >
	inline void Coords< T, Count >::toValues( T * p_pResult )const
	{
		if ( m_coords )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				p_pResult[i] = m_coords[i];
			}
		}
	}

	template< typename T, uint32_t Count >
	T const & Coords< T, Count >::at( uint32_t p_pos )const
	{
		REQUIRE( p_pos < Count );
		return m_coords[p_pos];
	}

	template< typename T, uint32_t Count >
	T & Coords< T, Count >::at( uint32_t p_pos )
	{
		REQUIRE( p_pos < Count );
		return m_coords[p_pos];
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator==( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return result;
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator!=( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator+( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator-( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator*( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator/( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, Count >::div( p_ptA, p_ptB );
	}

	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator+( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::add( p_pt, p_coords );
	}

	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator-( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::sub( p_pt, p_coords );
	}

	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator*( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::mul( p_pt, p_coords );
	}

	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator/( Coords< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::div( p_pt, p_coords );
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > operator*( Coords< T, Count > const & p_pt, T const & p_coord )
	{
		return PtOperators< T, T, Count, Count >::mul( p_pt, p_coord );
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > operator/( Coords< T, Count > const & p_pt, T const & p_coord )
	{
		return PtOperators< T, T, Count, Count >::div( p_pt, p_coord );
	}

	template< typename T, typename U >
	inline Point< T, 3 > operator^( Coords< T, 3 > const & p_ptA, Coords< U, 3 > const & p_ptB )
	{
		return Point< T, 3 >(
				   ( p_ptA[1] * p_ptB[2] ) - ( p_ptA[2] * p_ptB[1] ),
				   ( p_ptA[2] * p_ptB[0] ) - ( p_ptA[0] * p_ptB[2] ),
				   ( p_ptA[0] * p_ptB[1] ) - ( p_ptA[1] * p_ptB[0] )
			   );
	}

	namespace point
	{
		namespace
		{
			template< typename T, uint32_t Count >
			struct Computedot
			{
				static T calc( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
				{
					T tReturn;
					Policy< T >::initialise( tReturn );

					for ( uint32_t i = 0; i < Count; i++ )
					{
						tReturn += p_ptA[i] * p_ptB[i];
					}

					return tReturn;
				}
			};

			template< typename T >
			struct Computedot< T, 2 >
			{
				static T calc( Coords< T, 2 > const & p_ptA, Coords< T, 2 > const & p_ptB )
				{
					return ( p_ptA[0] * p_ptB[0] ) + ( p_ptA[1] * p_ptB[1] );
				}
			};

			template< typename T >
			struct Computedot< T, 3 >
			{
				static T calc( Coords< T, 3 > const & p_ptA, Coords< T, 3 > const & p_ptB )
				{
					return ( p_ptA[0] * p_ptB[0] ) + ( p_ptA[1] * p_ptB[1] ) + ( p_ptA[2] * p_ptB[2] );
				}
			};

			template< typename T >
			struct Computedot< T, 4 >
			{
				static T calc( Coords< T, 4 > const & p_ptA, Coords< T, 4 > const & p_ptB )
				{
					return ( p_ptA[0] * p_ptB[0] ) + ( p_ptA[1] * p_ptB[1] ) + ( p_ptA[2] * p_ptB[2] ) + ( p_ptA[3] * p_ptB[3] );
				}
			};
		}

		template< typename T, uint32_t Count >
		T dot( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
		{
			return Computedot< T, Count >::calc( p_ptA, p_ptB );
		}

		template< typename T, uint32_t Count >
		double cosTheta( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
		{
			double result = double( length( p_ptA ) * length( p_ptB ) );

			if ( result != 0 )
			{
				result = dot( p_ptA, p_ptB ) / result;
			}
			else
			{
				result = dot( p_ptA, p_ptB );
			}

			return result;
		}

		template< typename T, uint32_t Count >
		inline void negate( Coords< T, Count > & p_point )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				p_point[i] = -p_point[i];
			}
		}

		template< typename T, uint32_t Count >
		void normalise( Coords< T, Count > & p_point )
		{
			T l = T( length( p_point ) );

			if ( !Policy< T >::isNull( l ) )
			{
				std::transform( p_point.constPtr(), p_point.constPtr() + Count, p_point.ptr(), [l]( T const & p_value )
				{
					return p_value / l;
				} );
			}
		}

		template< typename T, uint32_t Count >
		double lengthSquared( Coords< T, Count > const & p_point )
		{
			return std::accumulate( p_point.constPtr(), p_point.constPtr() + Count, 0.0, []( double a, T const & b )
			{
				return a + double( b * b );
			} );
		}

		template< typename T, uint32_t Count >
		double length( Coords< T, Count > const & p_point )
		{
			return sqrt( lengthSquared( p_point ) );
		}

		template< typename T, uint32_t Count >
		inline double lengthManhattan( Coords< T, Count > const & p_point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result += abs( p_point[i] );
			}

			return result;
		}

		template< typename T, uint32_t Count >
		double lengthMinkowski( Coords< T, Count > const & p_point, double p_order )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result += pow( double( abs( p_point[i] ) ), p_order );
			}

			result = pow( result, 1.0 / p_order );
			return result;
		}

		template< typename T, uint32_t Count >
		double lengthChebychev( Coords< T, Count > const & p_point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result = std::max( result, double( abs( p_point[i] ) ) );
			}

			return result;
		}

		template< typename T, uint32_t Count >
		double distanceSquared( Coords< T, Count > const & p_lhs, Coords< T, Count > const & p_rhs )
		{
			return lengthSquared( p_rhs - p_lhs );
		}

		template< typename T, uint32_t Count >
		double distance( Coords< T, Count > const & p_lhs, Coords< T, Count > const & p_rhs )
		{
			return length( p_rhs - p_lhs );
		}

		template< typename T, uint32_t Count >
		inline double distanceManhattan( Coords< T, Count > const & p_lhs, Coords< T, Count > const & p_rhs )
		{
			return lengthManhattan( p_rhs - p_lhs );
		}

		template< typename T, uint32_t Count >
		double distanceMinkowski( Coords< T, Count > const & p_lhs, Coords< T, Count > const & p_rhs, double p_order )
		{
			return lengthMinkowski( p_rhs - p_lhs, p_order );
		}

		template< typename T, uint32_t Count >
		double distanceChebychev( Coords< T, Count > const & p_lhs, Coords< T, Count > const & p_rhs )
		{
			return lengthChebychev( p_rhs - p_lhs );
		}
	}
}

//*************************************************************************************************

template< typename T, uint32_t Count >
inline castor::String & operator<< ( castor::String & p_strOut, castor::Coords< T, Count > const & p_pt )
{
	castor::StringStream streamOut;

	if ( Count )
	{
		streamOut << p_pt[0];

		for ( uint32_t i = 0; i < Count; i++ )
		{
			streamOut << cuT( "\t" ) << p_pt[i];
		}
	}

	p_strOut += streamOut.str();
	return p_strOut;
}

template< typename T, uint32_t Count >
inline castor::String & operator>> ( castor::String & p_strIn, castor::Coords< T, Count > & p_pt )
{
	castor::StringStream streamIn( p_strIn );

	for ( uint32_t i = 0; i < Count; i++ )
	{
		streamIn >> p_pt[i];
	}

	p_strIn = streamIn.str();
	return p_strIn;
}

template< typename T, uint32_t Count, typename CharType >
inline std::basic_ostream< CharType > & operator<< ( std::basic_ostream< CharType > & p_streamOut, castor::Coords< T, Count > const & p_pt )
{
	if ( Count )
	{
		p_streamOut << p_pt[0];

		for ( uint32_t i = 0; i < Count; i++ )
		{
			p_streamOut << "\t" << p_pt[i];
		}
	}

	return p_streamOut;
}

template< typename T, uint32_t Count, typename CharType >
inline std::basic_istream< CharType > & operator>> ( std::basic_istream< CharType > & p_streamIn, castor::Coords< T, Count > & p_pt )
{
	for ( uint32_t i = 0; i < Count; i++ )
	{
		p_streamIn >> p_pt[i];
	}

	return p_streamIn;
}

//*************************************************************************************************
