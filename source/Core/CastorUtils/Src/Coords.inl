#include <cstring>

namespace Castor
{
	template< typename T, uint32_t Count >
	Coords< T, Count >::BinaryLoader::BinaryLoader()
		:	Loader< Coords< T, Count >, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
	{
	}

	template< typename T, uint32_t Count >
	bool Coords< T, Count >::BinaryLoader::operator()( Coords< T, Count > & p_object, BinaryFile & p_file )
	{
		bool l_bReturn = true;

		for ( uint32_t i = 0; i < Count && l_bReturn; ++i )
		{
			l_bReturn = p_file.Read( p_object[i] ) == sizeof( T );
		}

		return l_bReturn;
	}

	template< typename T, uint32_t Count >
	bool Coords< T, Count >::BinaryLoader::operator()( Coords< T, Count > const & p_object, BinaryFile & p_file )
	{
		bool l_bReturn = true;

		for ( uint32_t i = 0; i < Count && l_bReturn; ++i )
		{
			l_bReturn = p_file.Write( p_object[i] ) == sizeof( T );
		}

		return l_bReturn;
	}

//*************************************************************************************************

	template< typename T, uint32_t Count >
	Coords< T, Count >::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< Coords< T, Count >, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
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

	template< typename T, uint32_t Count >
	bool Coords< T, Count >::TextLoader::operator()( Coords< T, Count > const & p_object, TextFile & p_file )
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

		bool l_bReturn = p_file.Print( 1024, cuT( "%s" ), l_streamWord.str().c_str() ) > 0;
		return l_bReturn;
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords()
		:	m_coords( NULL	)
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( T * p_pValues )
		:	m_coords( p_pValues	)
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( Coords< T, Count > const & p_pt )
		:	m_coords( p_pt.m_coords )
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( Coords< T, Count > && p_pt )
		:	m_coords( NULL	)
	{
		m_coords		= std::move( p_pt.m_coords );
		p_pt.m_coords	= NULL;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count >::~Coords()
	{
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( Coords< T, Count > const & p_pt )
	{
		m_coords = p_pt.m_coords;
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( Coords< T, Count > && p_pt )
	{
		if ( this != & p_pt )
		{
			m_coords		= std::move( p_pt.m_coords );
			p_pt.m_coords	= NULL;
		}

		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( Point< T, Count > const & p_pt )
	{
		memcpy( m_coords, p_pt.const_ptr(), binary_size );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator =( T * p_pValues )
	{
		m_coords = p_pValues;
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator +=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this + p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator -=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this - p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this * p_pt );
		return *this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( Coords< U, _Count > const & p_pt )
	{
		*this = ( *this / p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator +=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this + p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator -=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this - p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this * p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( Point< U, _Count > const & p_pt )
	{
		*this = ( *this / p_pt );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator +=( U const * p_coords )
	{
		*this = ( *this + p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator -=( U const * p_coords )
	{
		*this = ( *this - p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( U const * p_coords )
	{
		*this = ( *this * p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( U const * p_coords )
	{
		*this = ( *this / p_coords );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator *=( T const & p_coord )
	{
		*this = ( *this * p_coord );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator /=( T const & p_coord )
	{
		*this = ( *this / p_coord );
		return * this;
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
		if ( p_pos >= Count )
		{
			CASTOR_ASSERT( false );
			throw std::range_error( "Coords subscript out of range" );
		}

		return m_coords[p_pos];
	}

	template< typename T, uint32_t Count >
	T & Coords< T, Count >::at( uint32_t p_pos )
	{
		if ( p_pos >= Count )
		{
			CASTOR_ASSERT( false );
			throw std::range_error( "Coords subscript out of range" );
		}

		return m_coords[p_pos];
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator ==( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		bool l_bReturn = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && l_bReturn; i++ )
		{
			l_bReturn = Policy< T >::equals( p_ptA[i], p_ptB[i] );
		}

		return l_bReturn;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator !=( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB )
	{
		return !( p_ptA == p_ptB );
	}

	namespace point
	{
		template< typename T, uint32_t Count >
		T dot( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
		{
			T l_tReturn;
			Policy< T >::init( l_tReturn );

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_tReturn += p_ptA[i] * p_ptB[i];
			}

			return l_tReturn;
		}

		template< typename T, uint32_t Count >
		double cos_theta( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB )
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
		inline void negate( Coords< T, Count > & p_ptPoint )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				p_ptPoint[i] = -p_ptPoint[i];
			}
		}

		template< typename T, uint32_t Count >
		void normalise( Coords< T, Count > & p_ptPoint )
		{
			T l_tLength = T( distance( p_ptPoint ) );

			if ( ! Policy< T >::is_null( l_tLength ) )
			{
				p_ptPoint /= l_tLength;
			}
		}

		template< typename T, uint32_t Count >
		double distance_squared( Coords< T, Count > const & p_ptPoint )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn += p_ptPoint[i] * p_ptPoint[i];
			}

			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		double distance( Coords< T, Count > const & p_ptPoint )
		{
			return sqrt( distance_squared( p_ptPoint ) );
		}

		template< typename T, uint32_t Count >
		inline double distance_manhattan( Coords< T, Count > const & p_ptPoint )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn += abs( p_ptPoint[i] );
			}

			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		double distance_minkowski( Coords< T, Count > const & p_ptPoint, double p_dOrder )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn += pow( double( abs( p_ptPoint[i] ) ), p_dOrder );
			}

			l_dReturn = pow( l_dReturn, 1.0 / p_dOrder );
			return l_dReturn;
		}

		template< typename T, uint32_t Count >
		double distance_chebychev( Coords< T, Count > const & p_ptPoint )
		{
			double l_dReturn = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_dReturn = std::max( l_dReturn, double( abs( p_ptPoint[i] ) ) );
			}

			return l_dReturn;
		}
	}
}

//*************************************************************************************************

template< typename T, uint32_t Count >
inline Castor::String & operator << ( Castor::String & p_strOut, Castor::Coords< T, Count > const & p_pt )
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
inline Castor::String & operator >> ( Castor::String & p_strIn, Castor::Coords< T, Count > & p_pt )
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
inline std::basic_ostream< CharType > & operator << ( std::basic_ostream< CharType > & p_streamOut, Castor::Coords< T, Count > const & p_pt )
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
inline std::basic_istream< CharType > & operator >> ( std::basic_istream< CharType > & p_streamIn, Castor::Coords< T, Count > & p_pt )
{
	for ( uint32_t i = 0; i < Count; i++ )
	{
		p_streamIn >> p_pt[i];
	}

	return p_streamIn;
}

//*************************************************************************************************
