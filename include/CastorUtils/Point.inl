namespace Castor
{	namespace Math
{
	template <typename T, size_t Count>
	Point <T, Count> :: Point( const T * p_pValues = NULL)
		:	m_coords( new T[Count]),
			m_bOwnCoords( true)
	{
		if (p_pValues == NULL)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				Templates::Policy<T>::init( m_coords[i]);
			}
		}
		else
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				Templates::Policy<T>::assign( m_coords[i], p_pValues[i]);
			}
		}
	}
	template <typename T, size_t Count>
	Point <T, Count> :: Point( const T & p_vA)
		:	m_coords( new T[Count]),
			m_bOwnCoords( true)
	{
		if (Count > 0)
		{
			Templates::Policy<T>::assign( m_coords[0], p_vA);

			for (size_t i = 1 ; i < Count ; i++)
			{
				Templates::Policy<T>::init( m_coords[i]);
			}
		}
	}
	template <typename T, size_t Count>
	Point <T, Count> :: Point( const T & p_vA, const T & p_vB)
		:	m_coords( new T[Count]),
			m_bOwnCoords( true)
	{
		if (Count > 0)
		{
			Templates::Policy<T>::assign( m_coords[0], p_vA);
		}
		if (Count > 1)
		{
			Templates::Policy<T>::assign( m_coords[1], p_vB);

			for (size_t i = 2 ; i < Count ; i++)
			{
				Templates::Policy<T>::init( m_coords[i]);
			}
		}
	}
	template <typename T, size_t Count>
	Point <T, Count> :: Point( const T & p_vA, const T & p_vB, const T & p_vC)
		:	m_coords( new T[Count]),
			m_bOwnCoords( true)
	{
		if (Count > 0)
		{
			Templates::Policy<T>::assign( m_coords[0], p_vA);
		}
		if (Count > 1)
		{
			Templates::Policy<T>::assign( m_coords[1], p_vB);
		}
		if (Count > 2)
		{
			Templates::Policy<T>::assign( m_coords[2], p_vC);

			for (size_t i = 3 ; i < Count ; i++)
			{
				Templates::Policy<T>::init( m_coords[i]);
			}
		}
	}
	template <typename T, size_t Count>
	Point <T, Count> :: Point( const T & p_vA, const T & p_vB, const T & p_vC, const T & p_vD)
		:	m_coords( new T[Count]),
			m_bOwnCoords( true)
	{
		if (Count > 0)
		{
			Templates::Policy<T>::assign( m_coords[0], p_vA);
		}
		if (Count > 1)
		{
			Templates::Policy<T>::assign( m_coords[1], p_vB);
		}
		if (Count > 2)
		{
			Templates::Policy<T>::assign( m_coords[2], p_vC);
		}
		if (Count > 3)
		{
			Templates::Policy<T>::assign( m_coords[3], p_vD);

			for (size_t i = 4 ; i < Count ; i++)
			{
				Templates::Policy<T>::init( m_coords[i]);
			}
		}
	}
	template <typename T, size_t Count>
	Point <T, Count> :: Point( const Point<T, Count> & p_ptPoint)
		:	m_coords( new T[Count]),
			m_bOwnCoords( true)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::assign( m_coords[i], p_ptPoint[i]);
		}
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
	inline Point <T, Count> & Point <T, Count> :: operator +=( const Point<T, Count> & p_pt)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_add( m_coords[i], p_pt[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator +=( const T * p_coords)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_add( m_coords[i], p_coords[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator -=( const Point<T, Count> & p_pt)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_substract( m_coords[i], p_pt[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator -=( const T * p_coords)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_substract( m_coords[i], p_coords[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator *=( const Point<T, Count> & p_pt)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_multiply( m_coords[i], p_pt[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator *=( const T * p_coords)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_multiply( m_coords[i], p_coords[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator *=( const T & p_value)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_multiply( m_coords[i], p_value);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator /=( const Point<T, Count> & p_pt)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_divide( m_coords[i], p_pt[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> & Point <T, Count> :: operator /=( const T & p_value)
	{
		if ( ! Templates::Policy<T>::is_null( p_value))
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				Templates::Policy<T>::ass_divide( m_coords[i], p_value);
			}
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point<T, Count> & Point <T, Count> :: operator =( const Point<T, Count> & p_pt)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::assign( m_coords[i], p_pt[i]);
		}

		return * this;
	}
	template <typename T, size_t Count>
	inline Point<T, Count> Point <T, Count> :: operator +( const Point<T, Count> & p_pt)const
	{
		Point<T, Count> l_ptReturn;

		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::assign( l_ptReturn[i], Templates::Policy<T>::add( m_coords[i], p_pt[i]));
		}

		return l_ptReturn;
	}
	template <typename T, size_t Count>
	inline Point<T, Count> Point <T, Count> :: operator +( const T & p_value)const
	{
		Point<T, Count> l_ptReturn;

		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::assign( l_ptReturn[i], Templates::Policy<T>::add( m_coords[i], p_value));
		}

		return l_ptReturn;
	}
	template <typename T, size_t Count>
	inline Point<T, Count> Point <T, Count> :: operator -( const Point<T, Count> & p_pt)const
	{
		Point<T, Count> l_ptReturn;

		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::assign( l_ptReturn[i], Templates::Policy<T>::substract( m_coords[i], p_pt[i]));
		}

		return l_ptReturn;
	}
	template <typename T, size_t Count>
	inline Point<T, Count> Point <T, Count> :: operator -( const T & p_value)const
	{
		Point<T, Count> l_ptReturn;

		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::assign( l_ptReturn[i], Templates::Policy<T>::substract( m_coords[i], p_value));
		}

		return l_ptReturn;
	}
	template <typename T, size_t Count>
	inline Point<T, Count> Point <T, Count> :: operator *( const Point<T, Count> & p_pt)const
	{
		Point<T, Count> l_ptReturn;

		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::assign( l_ptReturn[i], Templates::Policy<T>::multiply( m_coords[i], p_pt[i]));
		}

		return l_ptReturn;
	}
	template <typename T, size_t Count>
	inline Point<T, Count> Point <T, Count> :: operator *( const T & p_value)const
	{
		Point<T, Count> l_ptReturn( * this);

		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::assign( l_ptReturn[i], Templates::Policy<T>::multiply( m_coords[i], p_value));
		}

		return l_ptReturn;
	}
	template <typename T, size_t Count>
	inline Point<T, Count> Point <T, Count> :: operator /( const Point<T, Count> & p_pt)const
	{
		Point<T, Count> l_ptReturn;

		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::assign( l_ptReturn[i], Templates::Policy<T>::divide( m_coords[i], p_pt[i]));
		}

		return l_ptReturn;
	}
	template <typename T, size_t Count>
	inline Point<T, Count> Point <T, Count> :: operator /( const T & p_value)const
	{
		Point<T, Count> l_ptReturn( * this);

		if ( ! Templates::Policy<T>::is_null( p_value))
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				Templates::Policy<T>::assign( l_ptReturn[i], Templates::Policy<T>::divide( m_coords[i], p_value));
			}
		}

		return l_ptReturn;
	}
	template <typename T, size_t Count>
	inline bool Point <T, Count> :: operator ==( const Point<T, Count> & p_pt)const
	{
		bool l_bReturn = true;

		for (size_t i = 0 ; i < Count && l_bReturn ; i++)
		{
			l_bReturn = Templates::Policy<T>::equals( m_coords[i], p_pt[i]);
		}

		return l_bReturn;
	}
	template <typename T, size_t Count>
	inline bool Point <T, Count> :: operator !=( const Point<T, Count> & p_pt)const
	{
		return ! operator ==( p_pt);
	}
	template <typename T, size_t Count>
	inline std::ostream & Point <T, Count> :: operator << ( std::ostream & l_streamOut)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			l_streamOut << "\t" << m_coords[i];
		}

		l_streamOut << std::endl;
		return l_streamOut;
	}
	template <typename T, size_t Count>
	inline std::istream & Point <T, Count> :: operator >> ( std::istream & l_streamIn)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			l_streamIn >> m_coords[i];
		}

		return l_streamIn;
	}
	template <typename T, size_t Count>
	inline Point<T, Count> Point <T, Count> :: operator ^( const Point<T, Count> & p_vertex)const
	{
		Point<T, Count> l_result( * this);

		if (Count == 3)
		{
			T l_valuesA[Count], l_valuesB[Count];
			p_vertex.ToValues( l_valuesA);
			ToValues( l_valuesB);
			Templates::Policy<T>::assign( l_result[0], Templates::Policy<T>::substract( Templates::Policy<T>::multiply( l_valuesB[1], l_valuesA[2]), Templates::Policy<T>::multiply( l_valuesA[1], l_valuesB[2])));
			Templates::Policy<T>::assign( l_result[1], Templates::Policy<T>::substract( Templates::Policy<T>::multiply( l_valuesB[2], l_valuesA[0]), Templates::Policy<T>::multiply( l_valuesA[2], l_valuesB[0])));
			Templates::Policy<T>::assign( l_result[2], Templates::Policy<T>::substract( Templates::Policy<T>::multiply( l_valuesB[0], l_valuesA[1]), Templates::Policy<T>::multiply( l_valuesA[0], l_valuesB[1])));
		}

		return l_result;
	}
	template <typename T, size_t Count>
	inline void Point <T, Count> :: LinkCoords( const void * p_pCoords)
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
	inline void Point <T, Count> :: UnlinkCoords()
	{
		if ( ! m_bOwnCoords)
		{
			m_coords = new T[Count];
		}

		m_bOwnCoords = true;
	}
	template <typename T, size_t Count>
	inline void Point <T, Count> :: ToValues( T * p_pResult)const
	{
		if (Count >= 1)
		{
			Templates::Policy<T>::assign( p_pResult[0], m_coords[0]);

			for (size_t i = 0 ; i < Count ; i++)
			{
				Templates::Policy<T>::assign( p_pResult[i], m_coords[i]);
			}
		}
	}
	template <typename T, size_t Count>
	inline void Point <T, Count> :: Reverse()
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_negate( m_coords[i]);
		}
	}
	template <typename T, size_t Count>
	inline void Point <T, Count> :: Normalise()
	{
		T l_length = GetLength();

		if (Templates::Policy<T>::is_null( l_length))
		{
			return;
		}

		operator /=( l_length);
	}
	template <typename T, size_t Count>
	inline Point<T, Count> Point <T, Count> :: GetNormalised()const
	{
		Point<T, Count> l_ptReturn( * this);
		l_ptReturn.Normalise();
		return l_ptReturn;
	}
	template <typename T, size_t Count>
	inline T Point <T, Count> :: Dot( const Point<T, Count> & p_vertex)const
	{
		T l_tReturn;
		Templates::Policy<T>::init( l_tReturn);

		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_add( l_tReturn, Templates::Policy<T>::multiply( m_coords[i], p_vertex[i]));
		}

		return l_tReturn;
	}
	template <typename T, size_t Count>
	inline T Point <T, Count> :: GetSquaredLength()const
	{
		T l_tReturn;
		Templates::Policy<T>::init( l_tReturn);

		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_add( l_tReturn, Templates::Policy<T>::multiply( m_coords[i], m_coords[i]));
		}

		return l_tReturn;
	}
	template <typename T, size_t Count>
	inline real Point <T, Count> :: GetLength()const
	{
		return sqrt( (real)GetSquaredLength());
	}
	template <typename T, size_t Count>
	inline real Point <T, Count> :: GetCosTheta( const Point<T, Count> & p_vector)const
	{
		return Dot( p_vector) / (GetLength() * p_vector.GetLength());
	}
	template <typename T, size_t Count>
	bool Point <T, Count> :: Write( Utils::File & p_file)const
	{
		return (p_file.WriteArray( m_coords, Count) == Count * sizeof( T));
	}
	template <typename T, size_t Count>
	bool Point <T, Count> :: Read( Utils::File & p_file)
	{
		return (p_file.ReadArray( m_coords, Count) == Count * sizeof( T));
	}
	template <typename T, size_t Count>
	inline const T & Point <T, Count> :: operator[]( size_t p_pos)const
	{
		return m_coords[p_pos];
	}
	template <typename T, size_t Count>
	inline T & Point <T, Count> :: operator[]( size_t p_pos)
	{
		return m_coords[p_pos];
	}
	template <typename T, size_t Count>
	inline T * Point <T, Count> :: ptr()
	{
		return m_coords;
	}
	template <typename T, size_t Count>
	inline const T * Point <T, Count> :: const_ptr()const
	{
		return m_coords;
	}




	template <typename T, size_t Count>
	inline std::ostream & operator << ( std::ostream & l_streamOut, const Point <T, Count> & p_point)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			l_streamOut << "\t" << p_point[i];
		}

		l_streamOut << std::endl;
		return l_streamOut;
	}
	template <typename T, size_t Count>
	inline std::istream & operator >> ( std::istream & l_streamIn, Point <T, Count> & p_point)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			l_streamIn >> p_point[i];
		}

		return l_streamIn;
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator *( int p_value, const Point <T, Count> & p_ptPoint)
	{
		return p_ptPoint.operator *( p_value);
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator +( int p_value, const Point <T, Count> & p_ptPoint)
	{
		return p_ptPoint.operator +( p_value);
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator -( int p_value, const Point <T, Count> & p_ptPoint)
	{
		return p_ptPoint.operator -( p_value);
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator /( int p_value, const Point <T, Count> & p_ptPoint)
	{
		return p_ptPoint.operator /( p_value);
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator *( real p_value, const Point <T, Count> & p_ptPoint)
	{
		return p_ptPoint.operator *( p_value);
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator +( real p_value, const Point <T, Count> & p_ptPoint)
	{
		return p_ptPoint.operator +( p_value);
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator -( real p_value, const Point <T, Count> & p_ptPoint)
	{
		return p_ptPoint.operator -( p_value);
	}
	template <typename T, size_t Count>
	inline Point <T, Count> operator /( real p_value, const Point <T, Count> & p_ptPoint)
	{
		return p_ptPoint.operator /( p_value);
	}
	template <typename T, size_t Count, typename _Ty>
	inline Point <T, Count> operator +( const Point <T, Count> & p_ptA, const Point<_Ty, Count> & p_ptB)
	{
		Point <T, Count> l_ptReturn( p_ptA);

		for (size_t i = 0 ; i < Count ; i++)
		{
			Templates::Policy<T>::ass_add( l_ptReturn[i], T( p_ptB[i]));
		}

		return l_ptReturn;
	}
	template <typename T, size_t Count, size_t _Count>
	inline Point <T, Count> operator +( const Point <T, Count> & p_ptA, const Point<T, _Count> & p_ptB)
	{
		Point <T, Count> l_ptReturn( p_ptA);

		for (size_t i = 0 ; i < std::min<size_t>( Count, _Count) ; i++)
		{
			Templates::Policy<T>::ass_add( l_ptReturn[i], p_ptB[i]);
		}

		return l_ptReturn;
	}
	template <typename T, size_t Count, typename _Ty, size_t _Count>
	inline Point <T, Count> operator +( const Point <T, Count> & p_ptA, const Point<_Ty, _Count> & p_ptB)
	{
		Point <T, Count> l_ptReturn( p_ptA);

		for (size_t i = 0 ; i < std::min<size_t>( Count, _Count) ; i++)
		{
			Templates::Policy<T>::ass_add( l_ptReturn[i], T( p_ptB[i]));
		}

		return l_ptReturn;
	}
}
}