#include "PointOperators.hpp"
#include <cstring>
#include <numeric>

namespace Castor
{
	template< typename T, uint32_t Count >
	Coords< T, Count >::TextLoader::TextLoader()
		: Castor::TextLoader< Coords< T, Count > >()
	{
	}

	template< typename T, uint32_t Count >
	bool Coords< T, Count >::TextLoader::operator()( Coords< T, Count > & p_object, TextFile & p_file )
	{
		String l_strWord;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			if ( p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t" ) ) > 0 )
			{
				StringStream l_streamWord( l_strWord );
				l_streamWord >> p_object[i];
			}

			xchar l_cDump;
			p_file.ReadChar( l_cDump );
		}

		return true;
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	Coords< T, Count >::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Coords< T, Count > >( p_tabs )
	{
	}

	template< typename T, uint32_t Count >
	bool Coords< T, Count >::TextWriter::operator()( Coords< T, Count > const & p_object, TextFile & p_file )
	{
		StringStream l_streamWord;
		l_streamWord.setf( std::ios::boolalpha );
		l_streamWord.setf( std::ios::showpoint );

		for ( uint32_t i = 0; i < Count; ++i )
		{
			if ( !l_streamWord.str().empty() )
			{
				l_streamWord << cuT( " " );
			}

			l_streamWord << p_object[i];
		}

		bool l_return = p_file.Print( 1024, cuT( "%s%s" ), this->m_tabs.c_str(), l_streamWord.str().c_str() ) > 0;
		Castor::TextWriter< Coords< T, Count > >::CheckError( l_return, "Coords value" );
		return l_return;
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
		memcpy( m_coords, p_pt.const_ptr(), binary_size );
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
	inline void Coords< T, Count >::to_values( T * p_pResult )const
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
		bool l_return = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && l_return; i++ )
		{
			l_return = Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return l_return;
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
			struct ComputeDot
			{
				static T Calc( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
				{
					T l_tReturn;
					Policy< T >::init( l_tReturn );

					for ( uint32_t i = 0; i < Count; i++ )
					{
						l_tReturn += p_ptA[i] * p_ptB[i];
					}

					return l_tReturn;
				}
			};

			template< typename T >
			struct ComputeDot< T, 2 >
			{
				static T Calc( Coords< T, 2 > const & p_ptA, Coords< T, 2 > const & p_ptB )
				{
					return ( p_ptA[0] * p_ptB[0] ) + ( p_ptA[1] * p_ptB[1] );
				}
			};

			template< typename T >
			struct ComputeDot< T, 3 >
			{
				static T Calc( Coords< T, 3 > const & p_ptA, Coords< T, 3 > const & p_ptB )
				{
					return ( p_ptA[0] * p_ptB[0] ) + ( p_ptA[1] * p_ptB[1] ) + ( p_ptA[2] * p_ptB[2] );
				}
			};

			template< typename T >
			struct ComputeDot< T, 4 >
			{
				static T Calc( Coords< T, 4 > const & p_ptA, Coords< T, 4 > const & p_ptB )
				{
					return ( p_ptA[0] * p_ptB[0] ) + ( p_ptA[1] * p_ptB[1] ) + ( p_ptA[2] * p_ptB[2] ) + ( p_ptA[3] * p_ptB[3] );
				}
			};
		}

		template< typename T, uint32_t Count >
		T dot( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
		{
			return ComputeDot< T, Count >::Calc( p_ptA, p_ptB );
		}

		template< typename T, uint32_t Count >
		double cos_theta( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
		{
			double l_result = double( length( p_ptA ) * length( p_ptB ) );

			if ( l_result != 0 )
			{
				l_result = dot( p_ptA, p_ptB ) / l_result;
			}
			else
			{
				l_result = dot( p_ptA, p_ptB );
			}

			return l_result;
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
			T l_length = T( length( p_point ) );

			if ( !Policy< T >::is_null( l_length ) )
			{
				std::transform( p_point.const_ptr(), p_point.const_ptr() + Count, p_point.ptr(), [l_length]( T const & p_value )
				{
					return p_value / l_length;
				} );
			}
		}

		template< typename T, uint32_t Count >
		double length_squared( Coords< T, Count > const & p_point )
		{
			return std::accumulate( p_point.const_ptr(), p_point.const_ptr() + Count, 0.0, []( double a, T const & b )
			{
				return a + double( b * b );
			} );
		}

		template< typename T, uint32_t Count >
		double length( Coords< T, Count > const & p_point )
		{
			return sqrt( length_squared( p_point ) );
		}

		template< typename T, uint32_t Count >
		inline double length_manhattan( Coords< T, Count > const & p_point )
		{
			double l_result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_result += abs( p_point[i] );
			}

			return l_result;
		}

		template< typename T, uint32_t Count >
		double length_minkowski( Coords< T, Count > const & p_point, double p_order )
		{
			double l_result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_result += pow( double( abs( p_point[i] ) ), p_order );
			}

			l_result = pow( l_result, 1.0 / p_order );
			return l_result;
		}

		template< typename T, uint32_t Count >
		double length_chebychev( Coords< T, Count > const & p_point )
		{
			double l_result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_result = std::max( l_result, double( abs( p_point[i] ) ) );
			}

			return l_result;
		}

		template< typename T, uint32_t Count >
		double distance_squared( Coords< T, Count > const & p_lhs, Coords< T, Count > const & p_rhs )
		{
			return length_squared( p_rhs - p_lhs );
		}

		template< typename T, uint32_t Count >
		double distance( Coords< T, Count > const & p_lhs, Coords< T, Count > const & p_rhs )
		{
			return length( p_rhs - p_lhs );
		}

		template< typename T, uint32_t Count >
		inline double distance_manhattan( Coords< T, Count > const & p_lhs, Coords< T, Count > const & p_rhs )
		{
			return length_manhattan( p_rhs - p_lhs );
		}

		template< typename T, uint32_t Count >
		double distance_minkowski( Coords< T, Count > const & p_lhs, Coords< T, Count > const & p_rhs, double p_order )
		{
			return length_minkowski( p_rhs - p_lhs, p_order );
		}

		template< typename T, uint32_t Count >
		double distance_chebychev( Coords< T, Count > const & p_lhs, Coords< T, Count > const & p_rhs )
		{
			return length_chebychev( p_rhs - p_lhs );
		}
	}
}

//*************************************************************************************************

template< typename T, uint32_t Count >
inline Castor::String & operator<< ( Castor::String & p_strOut, Castor::Coords< T, Count > const & p_pt )
{
	Castor::StringStream l_streamOut;

	if ( Count )
	{
		l_streamOut << p_pt[0];

		for ( uint32_t i = 0; i < Count; i++ )
		{
			l_streamOut << cuT( "\t" ) << p_pt[i];
		}
	}

	p_strOut += l_streamOut.str();
	return p_strOut;
}

template< typename T, uint32_t Count >
inline Castor::String & operator>> ( Castor::String & p_strIn, Castor::Coords< T, Count > & p_pt )
{
	Castor::StringStream l_streamIn( p_strIn );

	for ( uint32_t i = 0; i < Count; i++ )
	{
		l_streamIn >> p_pt[i];
	}

	p_strIn = l_streamIn.str();
	return p_strIn;
}

template< typename T, uint32_t Count, typename CharType >
inline std::basic_ostream< CharType > & operator<< ( std::basic_ostream< CharType > & p_streamOut, Castor::Coords< T, Count > const & p_pt )
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
inline std::basic_istream< CharType > & operator>> ( std::basic_istream< CharType > & p_streamIn, Castor::Coords< T, Count > & p_pt )
{
	for ( uint32_t i = 0; i < Count; i++ )
	{
		p_streamIn >> p_pt[i];
	}

	return p_streamIn;
}

//*************************************************************************************************
