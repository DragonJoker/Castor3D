#include "PointOperators.hpp"
#include <cstring>

namespace Castor
{
	//*************************************************************************************************

	namespace
	{
		template< typename T, uint32_t Count, uint32_t Index, typename U, typename ... Values >
		void construct( Point< T, Count > & p_result, U p_current, Values ... );

		template< typename T, uint32_t Count, uint32_t Index >
		void construct( Point< T, Count > & p_result )
		{
		}

		template< typename T, uint32_t Count, uint32_t Index, typename U >
		void construct( Point< T, Count > & p_result, U p_last )
		{
			if ( Index < Count )
			{
				p_result[Index] = T( p_last );
			}
		}

		template< typename T, uint32_t Count, uint32_t Index, typename U, typename ... Values >
		void construct( Point< T, Count > & p_result, U p_current, Values ... p_values )
		{
			if ( Index < Count )
			{
				p_result[Index] = T( p_current );
				construct < T, Count, Index + 1, Values... > ( p_result, p_values... );
			}
		}
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	Point< T, Count >::TextLoader::TextLoader()
		: Castor::TextLoader< Point< T, Count > >()
	{
	}

	template< typename T, uint32_t Count >
	bool Point< T, Count >::TextLoader::operator()( Point< T, Count > & p_object, TextFile & p_file )
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
	Point< T, Count >::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Point< T, Count > >( p_tabs )
	{
	}

	template< typename T, uint32_t Count >
	bool Point< T, Count >::TextWriter::operator()( Point< T, Count > const & p_object, TextFile & p_file )
	{
		StringStream l_streamWord;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			if ( !l_streamWord.str().empty() )
			{
				l_streamWord << cuT( " " );
			}

			l_streamWord << p_object[i];
		}

		bool l_return = p_file.Print( 1024, cuT( "%s%s" ), this->m_tabs.c_str(), l_streamWord.str().c_str() ) > 0;
		Castor::TextWriter< Point< T, Count > >::CheckError( l_return, "Point value" );
		return l_return;
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	Point< T, Count >::Point()
	{
		std::memset( this->m_coords, 0, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const * p_pValues )
	{
		if ( !p_pValues )
		{
			std::memset( this->m_coords, 0, binary_size );
		}
		else
		{
			std::memcpy( this->m_coords, p_pValues, binary_size );
		}
	}

	template< typename T, uint32_t Count >
	template< typename U >
	Point< T, Count >::Point( U const * p_pValues )
	{
		if ( !p_pValues )
		{
			std::memset( this->m_coords, 0, binary_size );
		}
		else
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				this->m_coords[i] = T( p_pValues[i] );
			}
		}
	}

	template< typename T, uint32_t Count >
	template< typename ValueA, typename ValueB, typename ... Values >
	Point< T, Count >::Point( ValueA p_valueA, ValueB p_valueB, Values ... p_values )
	{
		std::memset( this->m_coords, 0, binary_size );
		construct< T, Count, 0, ValueA, ValueB, Values... >( *this, p_valueA, p_valueB, p_values... );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	Point< T, Count >::Point( Point< U, Count > const & p_pt )
	{
		if ( std::is_same< T, U >::value )
		{
			std::memcpy( this->m_coords, p_pt.m_coords, binary_size );
		}
		else
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				this->m_coords[i] = static_cast< T >( p_pt.m_coords[i] );
			}
		}
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Point< T, Count > const & p_pt )
	{
		std::memcpy( this->m_coords, p_pt.m_coords, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Point< T, Count > && p_pt )
	{
		std::memcpy( this->m_coords, p_pt.m_coords, binary_size );
		std::memset( p_pt.m_coords, 0, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Coords< T, Count > const & p_pt )
	{
		std::memcpy( this->m_coords, p_pt.const_ptr(), binary_size );
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count >::~Point()
	{
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator=( Point< T, Count > const & p_pt )
	{
		std::memcpy( this->m_coords, p_pt.m_coords, binary_size );
		return *this;
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator=( Point< T, Count > && p_pt )
	{
		if ( this != &p_pt )
		{
			std::memcpy( this->m_coords, p_pt.m_coords, binary_size );
			std::memset( p_pt.m_coords, 0, binary_size );
		}

		return *this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator+=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::add( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator-=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::sub( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator*=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::mul( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator/=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::div( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator+=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::add( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator-=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::sub( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator*=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::mul( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator/=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::div( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator+=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::add( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator-=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::sub( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator*=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::mul( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator/=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::div( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator*=( T const & p_coord )
	{
		return PtAssignOperators< T, T, Count, Count >::mul( *this, p_coord );
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator/=( T const & p_coord )
	{
		return PtAssignOperators< T, T, Count, Count >::div( *this, p_coord );
	}

	template< typename T, uint32_t Count >
	void Point< T, Count >::swap( Point< T, Count > & p_pt )
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			std::swap( this->m_coords[i], p_pt.m_coords[i] );
		}
	}

	template< typename T, uint32_t Count >
	void Point< T, Count >::flip()
	{
		for ( uint32_t i = 0; i < Count / 2; i++ )
		{
			std::swap( this->m_coords[i], this->m_coords[Count - 1 - i] );
		}
	}

	template< typename T, uint32_t Count >
	inline void Point< T, Count >::to_values( T * p_pResult )const
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			p_pResult[i] = this->m_coords[i];
		}
	}

	template< typename T, uint32_t Count >
	T const & Point< T, Count >::at( uint32_t p_pos )const
	{
		REQUIRE( p_pos < Count );
		return this->m_coords[p_pos];
	}

	template< typename T, uint32_t Count >
	T & Point< T, Count >::at( uint32_t p_pos )
	{
		REQUIRE( p_pos < Count );
		return this->m_coords[p_pos];
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator==( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		bool l_return = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && l_return; i++ )
		{
			l_return = Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return l_return;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator!=( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator+( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator-( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator*( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator/( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::div( p_ptA, p_ptB );
	}
	template< typename T, typename U >
	inline Point< T, 3 > operator^( Point< T, 3 > const & p_ptA, Point< U, 3 > const & p_ptB )
	{
		return Point< T, 3 >(
				   ( p_ptA[1] * p_ptB[2] ) - ( p_ptA[2] * p_ptB[1] ),
				   ( p_ptA[2] * p_ptB[0] ) - ( p_ptA[0] * p_ptB[2] ),
				   ( p_ptA[0] * p_ptB[1] ) - ( p_ptA[1] * p_ptB[0] )
			   );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator+( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::add( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator-( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::sub( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator*( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::mul( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator/( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::div( p_pt, p_coords );
	}
	template< typename T, uint32_t Count, typename U >
	inline Point< T, Count > operator*( Point< T, Count > const & p_pt, U const & p_coord )
	{
		return PtOperators< T, U, Count, Count >::mul( p_pt, p_coord );
	}
	template< typename T, uint32_t Count, typename U >
	inline Point< T, Count > operator/( Point< T, Count > const & p_pt, U const & p_coord )
	{
		return PtOperators< T, U, Count, Count >::div( p_pt, p_coord );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator==( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		bool l_return = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && l_return; i++ )
		{
			l_return = Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return l_return;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator!=( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator+( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator-( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator*( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator/( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::div( p_ptA, p_ptB );
	}
	template< typename T, typename U >
	inline Point< T, 3 > operator^( Coords< T, 3 > const & p_ptA, Point< U, 3 > const & p_ptB )
	{
		return Point< T, 3 >(
				   ( p_ptA[1] * p_ptB[2] ) - ( p_ptA[2] * p_ptB[1] ),
				   ( p_ptA[2] * p_ptB[0] ) - ( p_ptA[0] * p_ptB[2] ),
				   ( p_ptA[0] * p_ptB[1] ) - ( p_ptA[1] * p_ptB[0] )
			   );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator==( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		bool l_return = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && l_return; i++ )
		{
			l_return = Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return l_return;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator!=( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator+( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::add( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator-( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::sub( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator*( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::mul( p_ptA, p_ptB );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator/( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return PtOperators< T, U, Count, _Count >::div( p_ptA, p_ptB );
	}
	template< typename T, typename U >
	inline Point< T, 3 > operator^( Point< T, 3 > const & p_ptA, Coords< U, 3 > const & p_ptB )
	{
		return Point< T, 3 >(
				   ( p_ptA[1] * p_ptB[2] ) - ( p_ptA[2] * p_ptB[1] ),
				   ( p_ptA[2] * p_ptB[0] ) - ( p_ptA[0] * p_ptB[2] ),
				   ( p_ptA[0] * p_ptB[1] ) - ( p_ptA[1] * p_ptB[0] )
			   );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator-( Point< T, Count > const & p_pt )
	{
		Point< T, Count > l_return;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			l_return[i] = -p_pt[i];
		}

		return l_return;
	}

	//*************************************************************************************************

	namespace point
	{
		template< typename T, uint32_t Count >
		inline void negate( Point< T, Count > & p_point )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				p_point[i] = -p_point[i];
			}
		}

		template< typename T, uint32_t Count >
		void normalise( Point< T, Count > & p_point )
		{
			T l_tLength = T( distance( p_point ) );

			if ( !Policy< T >::is_null( l_tLength ) )
			{
				p_point /= l_tLength;
			}
		}

		template< typename T, uint32_t Count >
		Point< T, Count > get_normalised( Point< T, Count > const & p_point )
		{
			Point< T, Count > l_return( p_point );
			normalise( l_return );
			return l_return;
		}

		template< typename T, typename U, uint32_t Count >
		T dot( Point< T, Count > const & p_ptA, Point< U, Count > const & p_ptB )
		{
			T l_tReturn;
			Policy< T >::init( l_tReturn );

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_tReturn += T( p_ptA[i] * p_ptB[i] );
			}

			return l_tReturn;
		}

		template< typename T, uint32_t Count >
		double cos_theta( Point< T, Count > const & p_ptA, Point< T, Count > const & p_ptB )
		{
			double l_dReturn = double( distance( p_ptA ) * distance( p_ptB ) );

			if ( l_dReturn != 0 )
			{
				l_dReturn = dot( p_ptA, p_ptB ) / l_dReturn;
			}
			else
			{
				l_dReturn = dot( p_ptA, p_ptB );
			}

			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		double distance_squared( Point< T, Count > const & p_point )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn += p_point[i] * p_point[i];
			}

			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		double distance( Point< T, Count > const & p_point )
		{
			return sqrt( distance_squared( p_point ) );
		}

		template< typename T, uint32_t Count >
		inline double distance_manhattan( Point< T, Count > const & p_point )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn += abs( p_point[i] );
			}

			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		double distance_minkowski( Point< T, Count > const & p_point, double p_dOrder )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn += pow( double( abs( p_point[i] ) ), p_dOrder );
			}

			l_dReturn = pow( l_dReturn, 1.0 / p_dOrder );
			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		double distance_chebychev( Point< T, Count > const & p_point )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn = std::max( l_dReturn, double( abs( p_point[i] ) ) );
			}

			return l_dReturn;
		}

		template< typename T, typename U, uint32_t Count >
		T dot( Point< T, Count > const & p_ptA, Coords< U, Count > const & p_ptB )
		{
			T l_tReturn = T();

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_tReturn += T( p_ptA[i] * p_ptB[i] );
			}

			return l_tReturn;
		}

		template< typename T, uint32_t Count >
		double cos_theta( Point< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
		{
			double l_dReturn = double( distance( p_ptA ) * distance( p_ptB ) );

			if ( l_dReturn != 0 )
			{
				l_dReturn = dot( p_ptA, p_ptB ) / l_dReturn;
			}
			else
			{
				l_dReturn = dot( p_ptA, p_ptB );
			}

			return l_dReturn;
		}

		template< typename T, typename U, uint32_t Count >
		T dot( Coords< T, Count > const & p_ptA, Point< U, Count > const & p_ptB )
		{
			T l_tReturn;
			Policy< T >::init( l_tReturn );

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_tReturn += T( p_ptA[i] * p_ptB[i] );
			}

			return l_tReturn;
		}

		template< typename T, uint32_t Count >
		double cos_theta( Coords< T, Count > const & p_ptA, Point< T, Count > const & p_ptB )
		{
			double l_dReturn = double( distance( p_ptA ) * distance( p_ptB ) );

			if ( l_dReturn != 0 )
			{
				l_dReturn = dot( p_ptA, p_ptB ) / l_dReturn;
			}
			else
			{
				l_dReturn = dot( p_ptA, p_ptB );
			}

			return l_dReturn;
		}
	}
}

//*************************************************************************************************

template< typename T, uint32_t Count >
inline Castor::String & operator<<( Castor::String & p_strOut, Castor::Point< T, Count > const & p_pt )
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
inline Castor::String & operator>>( Castor::String & p_strIn, Castor::Point< T, Count > & p_pt )
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
inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & p_streamOut, Castor::Point< T, Count > const & p_pt )
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
inline std::basic_istream< CharType > & operator>>( std::basic_istream< CharType > & p_streamIn, Castor::Point< T, Count > & p_pt )
{
	for ( uint32_t i = 0; i < Count; i++ )
	{
		p_streamIn >> p_pt[i];
	}

	return p_streamIn;
}

//*************************************************************************************************
