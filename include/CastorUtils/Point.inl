namespace Castor
{	namespace Math
{
//*************************************************************************************************

	template <typename T>
	DynPoint<T> :: DynPoint( size_t p_uiCount)
		:	m_coords( NULL)
		,	m_bOwnCoords( true)
		,	m_uiCount( p_uiCount)
	{
		if (m_uiCount > 0)
		{
			m_coords = new T[m_uiCount];
		}
	}
	template <typename T>
	template <typename U>
	DynPoint<T> :: DynPoint( size_t p_uiCount, U const * p_pData)
		:	m_coords( NULL)
		,	m_bOwnCoords( false)
		,	m_uiCount( p_uiCount)
	{
		m_coords = (T *)p_pData;
	}

	template <typename T>
	template <typename U>
	DynPoint<T> :: DynPoint( U const & p_vA, U const & p_vB)
		:	m_coords( NULL)
		,	m_bOwnCoords( true)
		,	m_uiCount( 2)
	{
		m_coords = new T[m_uiCount];
		m_coords[0] = p_vA;
		m_coords[1] = p_vB;
	}

	template <typename T>
	template <typename U>
	DynPoint<T> :: DynPoint( U const & p_vA, U const & p_vB, U const & p_vC)
		:	m_coords( NULL)
		,	m_bOwnCoords( true)
		,	m_uiCount( 3)
	{
		m_coords = new T[m_uiCount];
		m_coords[0] = p_vA;
		m_coords[1] = p_vB;
		m_coords[2] = p_vC;
	}

	template <typename T>
	template <typename U>
	DynPoint<T> :: DynPoint( U const & p_vA, U const & p_vB, U const & p_vC, U const & p_vD)
		:	m_coords( NULL)
		,	m_bOwnCoords( true)
		,	m_uiCount( 4)
	{
		m_coords = new T[m_uiCount];
		m_coords[0] = p_vA;
		m_coords[1] = p_vB;
		m_coords[2] = p_vC;
		m_coords[3] = p_vD;
	}

	template <typename T>
	template <typename U>
	DynPoint<T> :: DynPoint( DynPoint<U> const & p_ptPoint)
		:	m_coords( NULL)
		,	m_bOwnCoords( true)
		,	m_uiCount( p_ptPoint.m_uiCount)
	{
		if (m_uiCount > 0)
		{
			m_coords = new T[m_uiCount];
		}

		for (size_t i = 0 ; i < m_uiCount ; i++)
		{
			Policy<T>::assign( m_coords[i], p_ptPoint[i]);
		}
	}

	template <typename T>
	inline DynPoint <T> :: ~DynPoint()
	{
		if (m_bOwnCoords)
		{
			delete [] m_coords;
		}
	}
	template <typename T>
	inline void DynPoint <T> :: link( void const * p_pCoords)
	{
		if (m_bOwnCoords)
		{
			delete [] m_coords;
			m_coords = NULL;
		}

		m_coords = (T *)p_pCoords;
		m_bOwnCoords = false;
	}
	template <typename T>
	inline void DynPoint <T> :: unlink()
	{
		if ( ! m_bOwnCoords)
		{
			m_coords = new T[m_uiCount];
		}

		m_bOwnCoords = true;
	}
	template <typename T>
	inline void DynPoint <T> :: to_values( T * p_pResult)const
	{
		if (m_coords)
		{
			Policy<T>::assign( p_pResult[0], m_coords[0]);

			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				Policy<T>::assign( p_pResult[i], m_coords[i]);
			}
		}
	}

	template <typename T>
	DynPoint<T> :: DynPoint( DynPoint<T> const & p_pt)
		:	m_coords( NULL)
		,	m_bOwnCoords( p_pt.m_bOwnCoords)
		,	m_uiCount( p_pt.m_uiCount)
	{
		if (m_bOwnCoords)
		{
			if (m_uiCount > 0)
			{
				m_coords = new T[m_uiCount];
			}

			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				m_coords[i] = p_pt[i];
			}
		}
		else
		{
			m_coords = p_pt.m_coords;
		}
	}
	template <typename T>
	DynPoint<T> & DynPoint <T> :: operator =( DynPoint<T> const & p_pt)
	{
		if (m_bOwnCoords)
		{
			delete [] m_coords;
			m_coords = NULL;
		}

		m_bOwnCoords = p_pt.m_bOwnCoords;
		m_uiCount = p_pt.m_uiCount;

		if (m_bOwnCoords)
		{
			if (m_uiCount > 0)
			{
				m_coords = new T[m_uiCount];
			}

			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				m_coords[i] = p_pt[i];
			}
		}
		else
		{
			m_coords = p_pt.m_coords;
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	DynPoint<T> & DynPoint <T> :: operator =( DynPoint<U> const & p_pt)
	{
		if (m_bOwnCoords)
		{
			delete [] m_coords;
			m_coords = NULL;
		}

		m_bOwnCoords = p_pt.m_bOwnCoords;
		m_uiCount = p_pt.m_uiCount;

		if (m_bOwnCoords)
		{
			m_coords = new T[m_uiCount];
			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				Policy<T>::assign( m_coords[i], p_pt.m_coords[i]);
			}
		}
		else
		{
			m_coords = p_pt.m_coords;
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator +=( DynPoint<U> const & p_pt)
	{
		for (size_t i = 0 ; i < m_uiCount && i < p_pt.m_uiCount ; i++)
		{
			Policy<T>::ass_add( at( i), p_pt[i]);
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator +=( U const * p_coords)
	{
		for (size_t i = 0 ; i < m_uiCount ; i++)
		{
			Policy<T>::ass_add( at( i), p_coords[i]);
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator +=( U const & p_coord)
	{
		for (size_t i = 0 ; i < m_uiCount ; i++)
		{
			Policy<T>::ass_add( at( i), p_coord);
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator -=( DynPoint<U> const & p_pt)
	{
		for (size_t i = 0 ; i < m_uiCount && i < p_pt.m_uiCount ; i++)
		{
			Policy<T>::ass_substract( at( i), p_pt[i]);
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator -=( U const * p_coords)
	{
		for (size_t i = 0 ; i < m_uiCount ; i++)
		{
			Policy<T>::ass_substract( at( i), p_coords[i]);
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator -=( U const & p_coord)
	{
		for (size_t i = 0 ; i < m_uiCount ; i++)
		{
			Policy<T>::ass_substract( at( i), p_coord);
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator *=( DynPoint<U> const & p_pt)
	{
		for (size_t i = 0 ; i < m_uiCount && i < p_pt.m_uiCount ; i++)
		{
			Policy<T>::ass_multiply( at( i), p_pt[i]);
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator *=( U const * p_coords)
	{
		for (size_t i = 0 ; i < m_uiCount ; i++)
		{
			Policy<T>::ass_multiply( at( i), p_coords[i]);
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator *=( U const & p_coord)
	{
		for (size_t i = 0 ; i < m_uiCount ; i++)
		{
			Policy<T>::ass_multiply( at( i), p_coord);
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator /=( DynPoint<U> const & p_pt)
	{
		for (size_t i = 0 ; i < m_uiCount && i < p_pt.m_uiCount ; i++)
		{
			if ( ! Policy<T>::is_null( p_pt[i]))
			{
				Policy<T>::ass_divide( at( i), p_pt[i]);
			}
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator /=( U const * p_coords)
	{
		for (size_t i = 0 ; i < m_uiCount ; i++)
		{
			if ( ! Policy<T>::is_null( p_coords[i]))
			{
				Policy<T>::ass_divide( at( i), p_coords[i]);
			}
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator /=( U const & p_coord)
	{
		if ( ! Policy<T>::is_null( p_coord))
		{
			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				Policy<T>::ass_divide( at( i), p_coord);
			}
		}

		return * this;
	}
	template <typename T>
	template <typename U>
	inline DynPoint <T> & DynPoint <T> :: operator ^=( DynPoint<U> const & p_pt)
	{
		if (m_uiCount == 3 && p_pt.m_uiCount == 3)
		{
			T l_valuesA[3], l_valuesB[3];
			p_pt.to_values( l_valuesA);
			to_values( l_valuesB);
			Policy<T>::assign( m_coords[0], Policy<T>::substract( Policy<T>::multiply( l_valuesB[1], l_valuesA[2]), Policy<T>::multiply( l_valuesA[1], l_valuesB[2])));
			Policy<T>::assign( m_coords[1], Policy<T>::substract( Policy<T>::multiply( l_valuesB[2], l_valuesA[0]), Policy<T>::multiply( l_valuesA[2], l_valuesB[0])));
			Policy<T>::assign( m_coords[2], Policy<T>::substract( Policy<T>::multiply( l_valuesB[0], l_valuesA[1]), Policy<T>::multiply( l_valuesA[0], l_valuesB[1])));
		}

		return * this;
	}
	template <typename T>
	void DynPoint <T> :: swap( DynPoint<T> const & p_pt)
	{
		std::swap( m_uiCount, p_pt.m_uiCount);
		std::swap( m_coords, p_pt.m_coords);
		std::swap( m_bOwnCoords, p_pt.m_bOwnCoords);
	}
	template <typename T>
	void DynPoint <T> :: flip()
	{
		for (size_t i = 0 ; i < m_uiCount / 2 ; i++)
		{
			std::swap( m_coords[i], m_coords[m_uiCount - 1 - i]);
		}
	}

//*************************************************************************************************

	template <typename T, typename U>
	inline bool operator ==( DynPoint<T> const & p_ptA, DynPoint<U> const & p_ptB)
	{
		bool l_bReturn = (p_ptA.GetElementCount() == p_ptB.GetElementCount());

		for (size_t i = 0 ; i < p_ptA.GetElementCount() && l_bReturn ; i++)
		{
			l_bReturn = Policy<T>::equals( p_ptA[i], p_ptB[i]);
		}

		return l_bReturn;
	}
	template <typename T, typename U>
	inline bool operator !=( DynPoint<T> const & p_ptA, DynPoint<U> const & p_ptB)
	{
		return ! (p_ptA == p_ptB);
	}
	template <typename T>
	inline std::ostream & operator << ( std::ostream & l_streamOut,DynPoint<T> const & p_pt)
	{
		for (size_t i = 0 ; i < p_pt.GetElementCount() ; i++)
		{
			l_streamOut << "\t" << p_pt[i];
		}

		l_streamOut << std::endl;
		return l_streamOut;
	}
	template <typename T>
	inline std::istream & operator >> ( std::istream & l_streamIn, DynPoint<T> & p_pt)
	{
		for (size_t i = 0 ; i < p_pt.GetElementCount() ; i++)
		{
			l_streamIn >> p_pt[i];
		}

		return l_streamIn;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator +( DynPoint<T> const & p_ptA, DynPoint<U> const & p_ptB)
	{
		DynPoint <T> l_ptResult( p_ptA);
		l_ptResult += p_ptB;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator +( DynPoint<T> const & p_pt, U const * p_coords)
	{
		DynPoint <T> l_ptResult( p_pt);
		l_ptResult += p_coords;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator +( DynPoint<T> const & p_pt, U const & p_coord)
	{
		DynPoint <T> l_ptResult( p_pt);
		l_ptResult += p_coord;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator -( DynPoint<T> const & p_ptA, DynPoint<U> const & p_ptB)
	{
		DynPoint <T> l_ptResult( p_ptA);
		l_ptResult -= p_ptB;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator -( DynPoint<T> const & p_pt, U const * p_coords)
	{
		DynPoint <T> l_ptResult( p_pt);
		l_ptResult -= p_coords;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator -( DynPoint<T> const & p_pt, U const & p_coord)
	{
		DynPoint <T> l_ptResult( p_pt);
		l_ptResult -= p_coord;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator *( DynPoint<T> const & p_ptA, DynPoint<U> const & p_ptB)
	{
		DynPoint <T> l_ptResult( p_ptA);
		l_ptResult *= p_ptB;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator *( DynPoint<T> const & p_pt, U const * p_coords)
	{
		DynPoint <T> l_ptResult( p_pt);
		l_ptResult *= p_coords;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator *( DynPoint<T> const & p_pt, U const & p_coord)
	{
		DynPoint <T> l_ptResult( p_pt);
		l_ptResult *= p_coord;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator /( DynPoint<T> const & p_ptA, DynPoint<U> const & p_ptB)
	{
		DynPoint <T> l_ptResult( p_ptA);
		l_ptResult /= p_ptB;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator /( DynPoint<T> const & p_pt, U const * p_coords)
	{
		DynPoint <T> l_ptResult( p_pt);
		l_ptResult /= p_coords;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator /( DynPoint<T> const & p_pt, U const & p_coord)
	{
		DynPoint <T> l_ptResult( p_pt);
		l_ptResult /= p_coord;
		return l_ptResult;
	}
	template <typename T, typename U>
	inline DynPoint <T> operator ^( DynPoint<T> const & p_ptA, DynPoint<U> const & p_ptB)
	{
		DynPoint <T> l_ptResult( p_ptA);
		l_ptResult ^= p_ptB;
		return l_ptResult;
	}

	template <typename T>
	inline DynPoint <T> operator *( int p_value, const DynPoint <T> & p_ptPoint)
	{
		return p_ptPoint * p_value;
	}
	template <typename T>
	inline DynPoint <T> operator +( int p_value, const DynPoint <T> & p_ptPoint)
	{
		return p_ptPoint + p_value;
	}
	template <typename T>
	inline DynPoint <T> operator -( int p_value, const DynPoint <T> & p_ptPoint)
	{
		return p_ptPoint - p_value;
	}
	template <typename T>
	inline DynPoint <T> operator /( int p_value, const DynPoint <T> & p_ptPoint)
	{
		return p_ptPoint / p_value;
	}
	template <typename T>
	inline DynPoint <T> operator *( double p_value, const DynPoint <T> & p_ptPoint)
	{
		return p_ptPoint * p_value;
	}
	template <typename T>
	inline DynPoint <T> operator +( double p_value, const DynPoint <T> & p_ptPoint)
	{
		return p_ptPoint + p_value;
	}
	template <typename T>
	inline DynPoint <T> operator -( double p_value, const DynPoint <T> & p_ptPoint)
	{
		return p_ptPoint - p_value;
	}
	template <typename T>
	inline DynPoint <T> operator /( double p_value, const DynPoint <T> & p_ptPoint)
	{
		return p_ptPoint / p_value;
	}

//*************************************************************************************************

	template <typename T, size_t Count>
	Point <T, Count> :: Point()
		:	m_coords( NULL)
		,	m_bOwnCoords( true)
	{
		if (Count > 0)
		{
			m_coords = new T[Count];
		}

		for (size_t i = 0 ; i < Count ; i++)
		{
			Policy<T>::init( at( i));
		}
	}

	template <typename T, size_t Count>
	template <typename U>
	Point <T, Count> :: Point( U const * p_pValues)
		:	m_coords( NULL)
		,	m_bOwnCoords( true)
	{
		if (Count > 0)
		{
			m_coords = new T[Count];
		}

		if (p_pValues == NULL)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				Policy<T>::init( at( i));
			}
		}
		else
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				Policy<T>::assign( at( i), p_pValues[i]);
			}
		}
	}
	template <typename T, size_t Count>
	template <typename U>
	Point <T, Count> :: Point( U const & p_vA, U const & p_vB)
		:	m_coords( NULL)
		,	m_bOwnCoords( true)
	{
		if (Count > 0)
		{
			m_coords = new T[Count];
			Policy<T>::assign( at( 0), p_vA);
		}
		if (Count > 1)
		{
			Policy<T>::assign( at( 1), p_vB);

			for (size_t i = 2 ; i < Count ; i++)
			{
				Policy<T>::init( at( i));
			}
		}
	}
	template <typename T, size_t Count>
	template <typename U>
	Point <T, Count> :: Point( U const & p_vA, U const & p_vB, U const & p_vC)
		:	m_coords( NULL)
		,	m_bOwnCoords( true)
	{
		if (Count > 0)
		{
			m_coords = new T[Count];
			Policy<T>::assign( at( 0), p_vA);
		}
		if (Count > 1)
		{
			Policy<T>::assign( at( 1), p_vB);
		}
		if (Count > 2)
		{
			Policy<T>::assign( at( 2), p_vC);

			for (size_t i = 3 ; i < Count ; i++)
			{
				Policy<T>::init( at( i));
			}
		}
	}
	template <typename T, size_t Count>
	template <typename U>
	Point <T, Count> :: Point( U const & p_vA, U const & p_vB, U const & p_vC, U const & p_vD)
		:	m_coords( NULL)
		,	m_bOwnCoords( true)
	{
		if (Count > 0)
		{
			m_coords = new T[Count];
			Policy<T>::assign( at( 0), p_vA);
		}
		if (Count > 1)
		{
			Policy<T>::assign( at( 1), p_vB);
		}
		if (Count > 2)
		{
			Policy<T>::assign( at( 2), p_vC);
		}
		if (Count > 3)
		{
			Policy<T>::assign( at( 3), p_vD);

			for (size_t i = 4 ; i < Count ; i++)
			{
				Policy<T>::init( at( i));
			}
		}
	}
	template <typename T, size_t Count>
	Point <T, Count> :: Point( Point<T, Count> const & p_pt)
		:	m_coords( NULL)
		,	m_bOwnCoords( true)
	{
		if (Count > 0)
		{
			m_coords = new T[Count];
		}

		copy( p_pt);
	}
	template <typename T, size_t Count>
	inline Point <T, Count> :: ~Point()
	{
		if (m_bOwnCoords)
		{
			delete [] m_coords;
		}
	}
	template <typename T, size_t Count>
	void Point <T, Count> :: clone( Point<T, Count> const & p_ptPoint)
	{
		LinkCoords( p_ptPoint.m_coords);
	}
	template <typename T, size_t Count>
	void Point <T, Count> :: clopy( Point<T, Count> const & p_ptPoint)
	{
		if (m_bOwnCoords)
		{
			delete [] m_coords;
			m_coords = NULL;
		}

		m_bOwnCoords = p_ptPoint.m_bOwnCoords;

		if (m_bOwnCoords)
		{
			if (Count > 0)
			{
				m_coords = new T[Count];
			}

			size_t i;

			for (i = 0 ; i < Count ; i++)
			{
				m_coords[i] = p_ptPoint[i];
			}
		}
		else
		{
			m_coords = p_ptPoint.m_coords;
		}
	}
	template <typename T, size_t Count>
	void Point <T, Count> :: copy( Point<T, Count> const & p_ptPoint)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			m_coords[i] = p_ptPoint[i];
		}
	}
	template <typename T, size_t Count>
	inline Point<T, Count> & Point <T, Count> :: operator =( Point<T, Count> const & p_pt)
	{
		copy( p_pt);
		return * this;
	}
	template <typename T, size_t Count>
	template <typename U, size_t _Count>
	inline Point <T, Count> & Point <T, Count> :: operator +=( Point<U, _Count> const & p_pt)
	{
		for (size_t i = 0 ; i < Count && i< _Count ; i++)
		{
			Policy<T>::ass_add( at( i), p_pt[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	template <typename U>
	inline Point <T, Count> & Point <T, Count> :: operator +=( U const * p_coords)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Policy<T>::ass_add( at( i), p_coords[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator +=( T const & p_coord)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Policy<T>::ass_add( at( i), p_coord);
		}

		return * this;
	}
	template <typename T, size_t Count>
	template <typename U, size_t _Count>
	inline Point <T, Count> & Point <T, Count> :: operator -=( Point<U, _Count> const & p_pt)
	{
		for (size_t i = 0 ; i < Count && i < _Count ; i++)
		{
			Policy<T>::ass_substract( at( i), p_pt[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	template <typename U>
	inline Point <T, Count> & Point <T, Count> :: operator -=( U const * p_coords)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Policy<T>::ass_substract( at( i), p_coords[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator -=( T const & p_coord)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Policy<T>::ass_substract( at( i), p_coord);
		}

		return * this;
	}
	template <typename T, size_t Count>
	template <typename U, size_t _Count>
	inline Point <T, Count> & Point <T, Count> :: operator *=( Point<U, _Count> const & p_pt)
	{
		for (size_t i = 0 ; i < Count && i< _Count ; i++)
		{
			Policy<T>::ass_multiply( at( i), p_pt[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	template <typename U>
	inline Point <T, Count> & Point <T, Count> :: operator *=( U const * p_coords)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Policy<T>::ass_multiply( at( i), p_coords[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator *=( T const & p_coord)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Policy<T>::ass_multiply( at( i), p_coord);
		}

		return * this;
	}
	template <typename T, size_t Count>
	template <typename U, size_t _Count>
	inline Point <T, Count> & Point <T, Count> :: operator /=( Point<U, _Count> const & p_pt)
	{
		for (size_t i = 0 ; i < Count && i< _Count ; i++)
		{
			if ( ! Policy<T>::is_null( p_pt[i]))
			{
				Policy<T>::ass_divide( at( i), p_pt[i]);
			}
		}

		return * this;
	}
	template <typename T, size_t Count>
	template <typename U>
	inline Point <T, Count> & Point <T, Count> :: operator /=( U const * p_coords)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			if ( ! Policy<T>::is_null( p_coords[i]))
			{
				Policy<T>::ass_divide( at( i), p_coords[i]);
			}
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator /=( T const & p_coord)
	{
		if ( ! Policy<T>::is_null( p_coord))
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				Policy<T>::ass_divide( at( i), p_coord);
			}
		}

		return * this;
	}
	template <typename T, size_t Count>
	template <typename U, size_t _Count>
	inline Point <T, Count> & Point <T, Count> :: operator ^=( Point<U, _Count> const & p_pt)
	{
		if (Count == 3)
		{
			T l_valuesA[Count], l_valuesB[Count];
			p_pt.to_values( l_valuesA);
			to_values( l_valuesB);
			Policy<T>::assign( m_coords[0], Policy<T>::substract( Policy<T>::multiply( l_valuesB[1], l_valuesA[2]), Policy<T>::multiply( l_valuesA[1], l_valuesB[2])));
			Policy<T>::assign( m_coords[1], Policy<T>::substract( Policy<T>::multiply( l_valuesB[2], l_valuesA[0]), Policy<T>::multiply( l_valuesA[2], l_valuesB[0])));
			Policy<T>::assign( m_coords[2], Policy<T>::substract( Policy<T>::multiply( l_valuesB[0], l_valuesA[1]), Policy<T>::multiply( l_valuesA[0], l_valuesB[1])));
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline void Point <T, Count> :: negate()
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Policy<T>::ass_negate( at( i));
		}
	}
	template <typename T, size_t Count>
	inline void Point <T, Count> :: normalise()
	{
		T l_length = T( length());

		if (Policy<T>::is_null( l_length))
		{
			return;
		}

		operator /=( l_length);
	}
	template <typename T, size_t Count>
	inline Point<T, Count> Point <T, Count> :: get_normalised()const
	{
		Point<T, Count> l_ptReturn;
		l_ptReturn.copy( * this);
		l_ptReturn.normalise();
		return l_ptReturn;
	}
	template <typename T, size_t Count>
	inline T Point <T, Count> :: dot( Point<T, Count> const & p_vertex)const
	{
		T l_tReturn;
		Policy<T>::init( l_tReturn);

		for (size_t i = 0 ; i < Count ; i++)
		{
			Policy<T>::ass_add( l_tReturn, Policy<T>::multiply( at( i), p_vertex[i]));
		}

		return l_tReturn;
	}
	template <typename T, size_t Count>
	inline double Point <T, Count> :: length_squared()const
	{
		double l_dReturn = 0.0;

		for (size_t i = 0 ; i < Count ; i++)
		{
			l_dReturn += Policy<T>::multiply( at( i), at( i));
		}

		return l_dReturn;
	}
	template <typename T, size_t Count>
	inline double Point <T, Count> :: length()const
	{
		return sqrt( this->length_squared());
	}
	template <typename T, size_t Count>
	inline double Point <T, Count> :: length_manhattan()const
	{
		double l_dReturn = 0.0;

		for (size_t i = 0 ; i < Count ; i++)
		{
			l_dReturn += abs( at( i));
		}

		return l_dReturn;
	}
	template <typename T, size_t Count>
	inline double Point <T, Count> :: length_minkowski( double p_dOrder)const
	{
		double l_dReturn = 0.0;

		for (size_t i = 0 ; i < Count ; i++)
		{
			l_dReturn += pow( double( abs( at( i))), p_dOrder);
		}

		l_dReturn = pow( l_dReturn, 1.0 / p_dOrder);

		return l_dReturn;
	}
	template <typename T, size_t Count>
	inline double Point <T, Count> :: length_chebychev()const
	{
		double l_dReturn = 0.0;

		for (size_t i = 0 ; i < Count ; i++)
		{
			l_dReturn = std::max( l_dReturn, double( abs( at( i))));
		}

		return l_dReturn;
	}
	template <typename T, size_t Count>
	inline double Point <T, Count> :: cos_theta( Point<T, Count> const & p_vector)const
	{
		return dot( p_vector) / (length() * p_vector.length());
	}
	template <typename T, size_t Count>
	inline void Point <T, Count> :: link( void const * p_pCoords)
	{
		if (m_bOwnCoords)
		{
			delete [] m_coords;
			m_coords = NULL;
		}

		m_coords = (T *)p_pCoords;
		m_bOwnCoords = false;
	}
	template <typename T, size_t Count>
	inline void Point <T, Count> :: unlink()
	{
		if ( ! m_bOwnCoords)
		{
			m_coords = new T[Count];
		}

		m_bOwnCoords = true;
	}
	template <typename T, size_t Count>
	void Point <T, Count> :: swap( Point<T, Count> & p_pt)
	{
		std::swap( m_coords, p_pt.m_coords);
		std::swap( m_bOwnCoords, p_pt.m_bOwnCoords);
	}
	template <typename T, size_t Count>
	void Point <T, Count> :: flip()
	{
		for (size_t i = 0 ; i < Count / 2 ; i++)
		{
			std::swap( m_coords[i], m_coords[Count - 1 - i]);
		}
	}
	template <typename T, size_t Count>
	bool Point <T, Count> :: Save( Castor::Utils::File & p_file)const
	{
		bool l_bReturn = true;

		for (size_t i = 0 ; i < Count && l_bReturn ; i++)
		{
			l_bReturn = p_file.Write( m_coords[i]) == sizeof( T);
		}

		return l_bReturn;
	}
	template <typename T, size_t Count>
	bool Point <T, Count> :: Load( Castor::Utils::File & p_file)
	{
		bool l_bReturn = true;

		for (size_t i = 0 ; i < Count && l_bReturn ; i++)
		{
			l_bReturn = p_file.Read( m_coords[i]) == sizeof( T);
		}

		return l_bReturn;
	}
	template <typename T, size_t Count>
	inline void Point <T, Count> :: to_values( T * p_pResult)const
	{
		if (m_coords)
		{
			Policy<T>::assign( p_pResult[0], m_coords[0]);

			for (size_t i = 0 ; i < Count ; i++)
			{
				Policy<T>::assign( p_pResult[i], m_coords[i]);
			}
		}
	}

//*************************************************************************************************

	template <typename T, size_t Count, typename U, size_t _Count>
	inline bool operator ==( Point<T, Count> const & p_ptA, Point<U, _Count> const & p_ptB)
	{
		bool l_bReturn = (Count == _Count);

		for (size_t i = 0 ; i < Count && l_bReturn ; i++)
		{
			l_bReturn = Policy<T>::equals( p_ptA[i], p_ptB[i]);
		}

		return l_bReturn;
	}
	template <typename T, size_t Count, typename U, size_t _Count>
	inline bool operator !=( Point<T, Count> const & p_ptA, Point<U, _Count> const & p_ptB)
	{
		return ! (p_ptA == p_ptB);
	}
	template <typename T, size_t Count>
	inline std::ostream & operator << ( std::ostream & l_streamOut, Point<T, Count> const & p_pt)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			l_streamOut << "\t" << p_pt[i];
		}

		l_streamOut << std::endl;
		return l_streamOut;
	}
	template <typename T, size_t Count>
	inline std::istream & operator >> ( std::istream & l_streamIn, Point<T, Count> & p_pt)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			l_streamIn >> p_pt[i];
		}

		return l_streamIn;
	}
	template <typename T, size_t Count, typename U, size_t _Count>
	inline Point <T, Count> operator +( Point<T, Count> const & p_ptA, Point<U, _Count> const & p_ptB)
	{
		Point <T, Count> l_ptResult( p_ptA);
		l_ptResult += p_ptB;
		return l_ptResult;
	}
	template <typename T, size_t Count, typename U>
	inline Point <T, Count> operator +( Point<T, Count> const & p_pt, U const * p_coords)
	{
		Point <T, Count> l_ptResult( p_pt);
		l_ptResult += p_coords;
		return l_ptResult;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator +( Point<T, Count> const & p_pt, T const & p_coord)
	{
		Point <T, Count> l_ptResult( p_pt);
		l_ptResult += p_coord;
		return l_ptResult;
	}
	template <typename T, size_t Count, typename U, size_t _Count>
	inline Point <T, Count> operator -( Point<T, Count> const & p_ptA, Point<U, _Count> const & p_ptB)
	{
		Point <T, Count> l_ptResult( p_ptA);
		l_ptResult -= p_ptB;
		return l_ptResult;
	}
	template <typename T, size_t Count, typename U>
	inline Point <T, Count> operator -( Point<T, Count> const & p_pt, U const * p_coords)
	{
		Point <T, Count> l_ptResult( p_pt);
		l_ptResult -= p_coords;
		return l_ptResult;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator -( Point<T, Count> const & p_pt, T const & p_coord)
	{
		Point <T, Count> l_ptResult( p_pt);
		l_ptResult -= p_coord;
		return l_ptResult;
	}
	template <typename T, size_t Count, typename U, size_t _Count>
	inline Point <T, Count> operator *( Point<T, Count> const & p_ptA, Point<U, _Count> const & p_ptB)
	{
		Point <T, Count> l_ptResult( p_ptA);
		l_ptResult *= p_ptB;
		return l_ptResult;
	}
	template <typename T, size_t Count, typename U>
	inline Point <T, Count> operator *( Point<T, Count> const & p_pt, U const * p_coords)
	{
		Point <T, Count> l_ptResult( p_pt);
		l_ptResult *= p_coords;
		return l_ptResult;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator *( Point<T, Count> const & p_pt, T const & p_coord)
	{
		Point <T, Count> l_ptResult( p_pt);
		l_ptResult *= p_coord;
		return l_ptResult;
	}
	template <typename T, size_t Count, typename U, size_t _Count>
	inline Point <T, Count> operator /( Point<T, Count> const & p_ptA, Point<U, _Count> const & p_ptB)
	{
		Point <T, Count> l_ptResult( p_ptA);
		l_ptResult /= p_ptB;
		return l_ptResult;
	}
	template <typename T, size_t Count, typename U>
	inline Point <T, Count> operator /( Point<T, Count> const & p_pt, U const * p_coords)
	{
		Point <T, Count> l_ptResult( p_pt);
		l_ptResult /= p_coords;
		return l_ptResult;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator /( Point<T, Count> const & p_pt, T const & p_coord)
	{
		Point <T, Count> l_ptResult( p_pt);
		l_ptResult /= p_coord;
		return l_ptResult;
	}
	template <typename T, size_t Count, typename U, size_t _Count>
	inline Point <T, Count> operator ^( Point<T, Count> const & p_ptA, Point<U, _Count> const & p_ptB)
	{
		Point <T, Count> l_ptResult;
		l_ptResult.copy( p_ptA);
		l_ptResult ^= p_ptB;
		return l_ptResult;
	}

	template <typename T, size_t Count>
	inline Point <T, Count> operator *( int p_value, Point <T, Count> const & p_ptPoint)
	{
		return p_ptPoint * p_value;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator +( int p_value, Point <T, Count> const & p_ptPoint)
	{
		return p_ptPoint + p_value;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator -( int p_value, Point <T, Count> const & p_ptPoint)
	{
		return p_ptPoint - p_value;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator /( int p_value, Point <T, Count> const & p_ptPoint)
	{
		return p_ptPoint / p_value;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator *( double p_value, Point <T, Count> const & p_ptPoint)
	{
		return p_ptPoint * p_value;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator +( double p_value, Point <T, Count> const & p_ptPoint)
	{
		return p_ptPoint + p_value;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator -( double p_value, Point <T, Count> const & p_ptPoint)
	{
		return p_ptPoint - p_value;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator /( double p_value, Point <T, Count> const & p_ptPoint)
	{
		return p_ptPoint / p_value;
	}

//*************************************************************************************************
}
}
