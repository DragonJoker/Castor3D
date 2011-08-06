namespace Castor
{	namespace Math
{
//*************************************************************************************************

	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> :: Matrix()
		:	m_pPointer( new T[Rows * Columns])
		,	m_bOwnCoords( true)
	{
		Init();
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> :: Matrix( T const & p_value)
		:	m_pPointer( new T[Rows * Columns])
		,	m_bOwnCoords( true)
	{
		Init( p_value);
	}
	template <typename T, size_t Rows, size_t Columns>
	template <typename Type>
	inline Matrix<T, Rows, Columns> :: Matrix( Type const * p_pMatrix)
		:	m_pPointer( new T[Rows * Columns])
		,	m_bOwnCoords( true)
	{
		Init();
		for (size_t i = 0 ; i < Columns ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				Templates::Policy<T>::assign( m_matrix[i][j], p_pMatrix[i * Rows + j]);
			}
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	Matrix<T, Rows, Columns> :: Matrix( Matrix<T, Columns, Rows> const & p_matrix)
		:	m_pPointer( new T[Rows * Columns])
		,	m_bOwnCoords( true)
	{
		Init();
		for (size_t i = 0 ; i < Columns && i < Rows ; i++)
		{
			m_matrix[i].link( & m_pPointer[i * Rows]);

			for (size_t j = 0 ; j < Rows && i < Columns ; j++)
			{
				Templates::Policy<T>::assign( m_matrix[i][j], p_matrix[i][j]);
			}
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	template <typename Type>
	inline Matrix<T, Rows, Columns> :: Matrix( Matrix<Type, Columns, Rows> const & p_matrix)
		:	m_pPointer( new T[Rows * Columns])
		,	m_bOwnCoords( true)
	{
		Init();
		for (size_t i = 0 ; i < Columns && i < Rows ; i++)
		{
			m_matrix[i].link( & m_pPointer[i * Rows]);

			for (size_t j = 0 ; j < Rows && i < Columns ; j++)
			{
				Templates::Policy<T>::assign( m_matrix[i][j], p_matrix[i][j]);
			}
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> :: ~Matrix()
	{
		if (m_bOwnCoords)
		{
			delete [] m_pPointer;
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: Init( T const & p_value)
	{
		for (size_t i = 0 ; i < Columns ; i++)
		{
			m_matrix[i].link( & m_pPointer[i * Rows]);

			for (size_t j = 0 ; j < Rows ; j++)
			{
				if (i == j)
				{
					Templates::Policy<T>::assign( m_matrix[i][j], p_value);
				}
				else
				{
					Templates::Policy<T>::init( m_matrix[i][j]);
				}
			}
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: SetRow( size_t p_uiRow, T const * p_row)
	{
		if (Columns >= 1 && p_uiRow < Rows)
		{
			for (size_t i = 0 ; i < Columns ; i++)
			{
				Templates::Policy<T>::assign( m_matrix[i][p_uiRow], p_row[i]);
			}
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: SetRow( size_t p_uiRow, Point<T, Columns> const & p_row)
	{
		if (Columns >= 1 && p_uiRow < Rows)
		{
			for (size_t i = 0 ; i < Columns ; i++)
			{
				Templates::Policy<T>::assign( m_matrix[i][p_uiRow], p_row[i]);
			}
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Point<T, Columns> Matrix<T, Rows, Columns> :: GetRow( size_t p_uiRow)const
	{
		CASTOR_ASSERT( p_uiRow < Rows);
		Point<T, Columns> l_mReturn;

		for (size_t i = 0 ; i < Columns ; i++)
		{
			Templates::Policy<T>::assign( l_mReturn[i], m_matrix[i][p_uiRow]);
		}

		return l_mReturn;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: GetRow( Point<T, Columns> & p_mResult, size_t p_uiRow)const
	{
		CASTOR_ASSERT( p_uiRow < Rows);

		for (size_t i = 0 ; i < Columns ; i++)
		{
			Templates::Policy<T>::assign( p_mResult[i], m_matrix[i][p_uiRow]);
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: SetColumn( size_t p_uiColumn, T const * p_col)
	{
		if (Rows >= 1 && p_uiColumn < Columns)
		{
			m_matrix[p_uiColumn] = p_col;
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: SetColumn( size_t p_uiColumn, Point<T, Rows> const & p_col)
	{
		if (Rows >= 1 && p_uiColumn < Columns)
		{
			m_matrix[p_uiColumn] = p_col;
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Point<T, Rows> Matrix<T, Rows, Columns> :: GetColumn( size_t p_uiColumn)const
	{
		CASTOR_ASSERT( p_uiColumn < Columns);
		Point<T, Rows> l_mReturn = m_matrix[p_uiColumn];
		return l_mReturn;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: GetColumn( Point<T, Rows> & p_mResult, size_t p_uiColumn)const
	{
		CASTOR_ASSERT( p_uiColumn < Columns);
		p_mResult = m_matrix[p_uiColumn];
	}
	template <typename T, size_t Rows, size_t Columns>
	inline T Matrix<T, Rows, Columns> :: GetValue( size_t p_uiRow, size_t p_uiColumn)
	{
		CASTOR_ASSERT( p_uiRow < Rows);
		CASTOR_ASSERT( p_uiColumn < Columns);

		return m_matrix[p_uiColumn][p_uiRow];
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Columns, Rows> Matrix<T, Rows, Columns> :: GetTransposed()const
	{
		Matrix<T, Columns, Rows> l_mReturn;

		for (size_t i = 0 ; i < Columns ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				Templates::Policy<T>::assign( l_mReturn[j][i], m_matrix[i][j]);
			}
		}

		return l_mReturn;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: GetTransposed( Matrix<T, Columns, Rows> & p_mResult)const
	{
		for (size_t i = 0 ; i < Columns ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				Templates::Policy<T>::assign( p_mResult[j][i], m_matrix[i][j]);
			}
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> Matrix<T, Rows, Columns> :: GetTriangle()const
	{
		Matrix<T, Rows, Columns> l_mReturn( * this);
		l_mReturn.SetTriangle();
		return l_mReturn;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: SetTriangle()
	{
		size_t l_uiMinDim = std::min<size_t>( Rows, Columns);
		bool l_bContinue = true;
		T l_tCoef;
		size_t l_uiMaxIndex;

		for (size_t i = 0 ; i < l_uiMinDim && l_bContinue ; i++)
		{
			l_uiMaxIndex = 0;

			for (size_t j = 1 ; j < Rows ; j++)
			{
				if (std::abs( m_matrix[i][j]) > std::abs( m_matrix[i][l_uiMaxIndex]))
				{
					l_uiMaxIndex = j;
				}
			}

			if ( ! Templates::Policy<T>::is_null( m_matrix[i][l_uiMaxIndex]))
			{
				for (size_t k = i ; k < Columns ; k++)
				{
					std::swap( m_matrix[k][i], m_matrix[k][l_uiMaxIndex]);
				}

				for (size_t j = i + 1 ; j < Rows ; j++)
				{
					Templates::Policy<T>::assign( l_tCoef, Templates::Policy<T>::divide( m_matrix[i][j], m_matrix[i][i]));

					for (size_t k = Columns - 1 ; k >= i && k < Columns ; k--)
					{
						Templates::Policy<T>::ass_substract( m_matrix[k][j], l_tCoef * m_matrix[k][i]);
					}
				}
			}
			else
			{
				l_bContinue = false;
			}
		}

		if (l_bContinue)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					if (Templates::Policy<T>::is_null( m_matrix[j][i]))
					{
						Templates::Policy<T>::init( m_matrix[j][i]);
					}
				}
			}
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> Matrix<T, Rows, Columns> :: GetMinor( size_t x, size_t y, size_t p_uiRows, size_t p_uiCols)const
	{
		Matrix<T, Rows, Columns> l_mReturn;
		l_mReturn.Identity();
		size_t l_i = 0, l_j = 0;

		for (size_t i = 0 ; i < p_uiRows ; i++)
		{
			if (i != x)
			{
				l_j = 0;

				for (size_t j = 0 ; j < p_uiCols ; j++)
				{
					if (j != y)
					{
						Templates::Policy<T>::assign( l_mReturn[l_j++][l_i], m_matrix[j][i]);
					}
				}

				l_i++;
			}
		}

		return l_mReturn;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline T Matrix<T, Rows, Columns> :: GetTrace()const
	{
		T l_tSum;
		Templates::Policy<T>::init( l_tSum);

		for (int i = 0 ; i < Rows ; i++)
		{
			if ( ! Templates::Policy<T>::is_null( m_matrix[i][i]))
			{
				Templates::Policy<T>::ass_add( l_tSum, m_matrix[i][i]);
			}
		}

		return l_tSum;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: SetJordan( T p_tLambda)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				if (i == j)
				{
					Templates::Policy<T>::assign( m_matrix[i][i], p_tLambda);
				}
				else
				{
					Templates::Policy<T>::init( m_matrix[j][i]);
				}
			}
		}

		Templates::Policy<T>assign( m_matrix[0 * Columns + 0], p_tLambda);

		for (size_t i = 0 ; i < Rows - 1 ; i++)
		{
			Templates::Policy<T>::assign( m_matrix[i + 1][i], Templates::Policy<T>::unit());
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> Matrix<T, Rows, Columns> :: GetJordan( T p_tLambda)const
	{
		Matrix<T, Rows, Columns> l_mReturn( * this);
		l_mReturn.SetJordan();
		return l_mReturn;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: SetIdentity()
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				if (i == j)
				{
					Templates::Policy<T>::assign( m_matrix[i][i], Templates::Policy<T>::unit());
				}
				else
				{
					Templates::Policy<T>::init( m_matrix[j][i]);
				}
			}
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> Matrix<T, Rows, Columns> :: GetIdentity()const
	{
		Matrix<T, Rows, Columns> l_mReturn( * this);
		l_mReturn.SetIdentity();
		return l_mReturn;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> & Matrix<T, Rows, Columns> :: operator = ( Matrix<T, Rows, Columns> const & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				m_matrix[j][i] = p_matrix[j][i];
			}
		}

		return * this;
	}
	template <typename T, size_t Rows, size_t Columns>
	template <typename Type>
	inline Matrix<T, Rows, Columns> & Matrix<T, Rows, Columns> :: operator = ( Matrix<Type, Rows, Columns> const & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				Templates::Policy<T>::assign( m_matrix[j][i], p_matrix[j][i]);
			}
		}

		return * this;
	}
	template <typename T, size_t Rows, size_t Columns>
	template <typename Type>
	inline Matrix<T, Rows, Columns> & Matrix<T, Rows, Columns> :: operator += ( Matrix<Type, Rows, Columns> const & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				if ( ! Templates::Policy<T>::is_null( p_matrix[j][i]))
				{
					Templates::Policy<T>::ass_add( m_matrix[j][i], p_matrix[j][i]);
				}
			}
		}

		return * this;
	}
	template <typename T, size_t Rows, size_t Columns>
	template <typename Type>
	inline Matrix<T, Rows, Columns> & Matrix<T, Rows, Columns> :: operator -= ( Matrix<Type, Rows, Columns> const & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				if ( ! Templates::Policy<T>::is_null( p_matrix[j][i]))
				{
					Templates::Policy<T>::ass_substract( m_matrix[j][i], p_matrix[j][i]);
				}
			}
		}

		return * this;
	}
	template <typename T, size_t Rows, size_t Columns>
	template <typename Type>
	inline Matrix<T, Rows, Columns> & Matrix<T, Rows, Columns> :: operator = ( Type const * p_pMatrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				Templates::Policy<T>::assign( m_matrix[j][i], p_pMatrix[j * Rows + i]);
			}
		}

		return * this;
	}
	template <typename T, size_t Rows, size_t Columns>
	template <typename Type>
	inline Matrix<T, Rows, Columns> & Matrix<T, Rows, Columns> :: operator += ( Type const * p_pMatrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				if ( ! Templates::Policy<T>::is_null( p_pMatrix[j * Rows + i]))
				{
					Templates::Policy<T>::ass_add( m_matrix[j][i], p_pMatrix[j * Rows + i]);
				}
			}
		}

		return * this;
	}
	template <typename T, size_t Rows, size_t Columns>
	template <typename Type>
	inline Matrix<T, Rows, Columns> & Matrix<T, Rows, Columns> :: operator -= ( Type const * p_pMatrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				if ( ! Templates::Policy<T>::is_null( p_pMatrix[j * Rows + i]))
				{
					Templates::Policy<T>::ass_substract( m_matrix[j][i], p_pMatrix[j * Rows + i]);
				}
			}
		}

		return * this;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> & Matrix<T, Rows, Columns> :: operator += ( T const & p_tValue)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				Templates::Policy<T>::ass_add( m_matrix[j][i], p_tValue);

				if (Templates::Policy<T>::is_null( m_matrix[j][i]))
				{
					Templates::Policy<T>::init( m_matrix[j][i]);
				}
			}
		}

		return * this;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> & Matrix<T, Rows, Columns> :: operator -= ( T const & p_tValue)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				Templates::Policy<T>::ass_substract( m_matrix[j][i], p_tValue);

				if (Templates::Policy<T>::is_null( m_matrix[j][i]))
				{
					Templates::Policy<T>::init( m_matrix[j][i]);
				}
			}
		}

		return * this;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> & Matrix<T, Rows, Columns> :: operator *= ( T const & p_tValue)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				Templates::Policy<T>::ass_multiply( m_matrix[j][i], p_tValue);

				if (Templates::Policy<T>::is_null( m_matrix[j][i]))
				{
					Templates::Policy<T>::init( m_matrix[j][i]);
				}
			}
		}

		return * this;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix<T, Rows, Columns> & Matrix<T, Rows, Columns> :: operator /= ( T const & p_tValue)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				if (Templates::Policy<T>::is_null( p_tValue))
				{
					Templates::Policy<T>::init( m_matrix[j][i]);
				}
				else
				{
					Templates::Policy<T>::ass_divide( m_matrix[j][i], p_tValue);

					if (Templates::Policy<T>::is_null( m_matrix[j][i]))
					{
						Templates::Policy<T>::init( m_matrix[j][i]);
					}
				}
			}
		}

		return * this;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Point<T, Rows> const & Matrix<T, Rows, Columns> :: operator []( size_t i)const
	{
		CASTOR_ASSERT( i < Rows * Columns);
		return m_matrix[i];
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Point<T, Rows> & Matrix<T, Rows, Columns> :: operator []( size_t i)
	{
		CASTOR_ASSERT( i < Rows * Columns);
		return m_matrix[i];
	}
	template <typename T, size_t Rows, size_t Columns>
	inline T * Matrix<T, Rows, Columns> :: ptr()
	{
		CASTOR_ASSERT( Rows > 0 && Columns > 0);
		return & m_matrix[0][0];
	}
	template <typename T, size_t Rows, size_t Columns>
	inline T const * Matrix<T, Rows, Columns> :: const_ptr()const
	{
		CASTOR_ASSERT( Rows > 0 && Columns > 0);
		return & m_matrix[0][0];
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: LinkCoords( void * p_pCoords)
	{
		if (m_bOwnCoords)
		{
			delete [] m_pPointer;
			m_pPointer = nullptr;
		}

		m_pPointer = (T *)p_pCoords;
		m_bOwnCoords = false;

		for (size_t i = 0 ; i < Columns ; i++)
		{
			m_matrix[i].link( & m_pPointer[i * Rows]);
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	inline void Matrix<T, Rows, Columns> :: _recheck()
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				Templates::Policy<T>::stick( m_matrix[j][i]);
			}
		}
	}
	template <typename T, size_t Rows, size_t Columns>
	template <typename Type>
	inline void Matrix<T, Rows, Columns> :: _assign( Matrix<Type, Rows, Columns> const & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				Templates::Policy<T>::assign( m_matrix[i][j], Templates::Policy<T>::convert( p_matrix[i][j]));
			}
		}
	}

//*************************************************************************************************

	template <typename T, size_t Rows, size_t Columns>
	inline bool operator ==( Matrix<T, Rows, Columns> const & p_mtxA, Matrix<T, Rows, Columns> const & p_mtxB)
	{
		bool l_bReturn = true;

		for (size_t i = 0 ; i < Rows && l_bReturn ; i++)
		{
			for (size_t j = 0 ; j < Columns && l_bReturn ; j++)
			{
				l_bReturn = Templates::Policy<T>::equals( p_mtxA[j][i], p_mtxB[j][i]);
			}
		}

		return l_bReturn;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline bool operator !=( Matrix<T, Rows, Columns> const & p_mtxA, Matrix<T, Rows, Columns> const & p_mtxB)
	{
		return ! operator ==( p_mtxA, p_mtxB);
	}
	template <typename T, size_t Rows, size_t Columns>
	inline std::ostream & operator << ( std::ostream & l_streamOut, Matrix<T, Rows, Columns> const & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				l_streamOut << "\t" << p_matrix[j][i];
			}

			l_streamOut << std::endl;
		}

		return l_streamOut;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline std::istream & operator >> ( std::istream & l_streamIn, Matrix <T, Rows, Columns> & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				l_streamIn >> p_matrix[j][i];
			}
		}

		return l_streamIn;
	}
	template <typename T, size_t Rows, size_t Columns, typename U>
	Matrix <T, Rows, Columns> operator + ( Matrix<T, Rows, Columns> const & p_mtxA, Matrix<U, Rows, Columns> const & p_mtxB)
	{
		Matrix<T, Rows, Columns> l_mtx( p_mtxA);
		l_mtx += p_mtxB;
		return l_mtx;
	}
	template <typename T, size_t Rows, size_t Columns, typename U>
	Matrix <T, Rows, Columns> operator - ( Matrix<T, Rows, Columns> const & p_mtxA, Matrix<U, Rows, Columns> const & p_mtxB)
	{
		Matrix<T, Rows, Columns> l_mtx( p_mtxA);
		l_mtx -= p_mtxB;
		return l_mtx;
	}
	template <typename T, size_t Rows, size_t Columns, typename U, size_t _Columns>
	Matrix<T, Rows, _Columns>	operator * ( Matrix<T, Rows, Columns> const & p_mtxA, Matrix<U, Columns, _Columns> const & p_mtxB)
	{
		Matrix<T, Rows, _Columns> l_mtxReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < _Columns ; j++)
			{
				Templates::Policy<T>::init( l_mtxReturn[j][i]);

				for (size_t k = 0 ; k < Columns ; k++)
				{
					Templates::Policy<T>::ass_add( l_mtxReturn[j][i], Templates::Policy<T>::multiply( p_mtxA[k][i], Templates::Policy<T>::convert( p_mtxB[j][k])));
				}
			}
		}

		return l_mtxReturn;
	}
	template <typename T, size_t Rows, size_t Columns, typename U>
	inline Point <T, Rows> operator * ( Matrix<T, Rows, Columns> const & p_matrix, Point<U, Columns> const & p_ptVector)
	{
		Point<T, Rows> l_ptReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			Templates::Policy<T>::init( l_ptReturn[i]);

			for (size_t j = 0 ; j < Columns ; j++)
			{
				Templates::Policy<T>::ass_add( l_ptReturn[i], Templates::Policy<T>::multiply( p_matrix[j][i], p_ptVector[j]));
			}
		}

		return l_ptReturn;
	}
	template <typename T, size_t Rows, size_t Columns, typename U>
	Matrix <T, Rows, Columns> operator + ( Matrix<T, Rows, Columns> const & p_mtxA, U const * p_mtxB)
	{
		Matrix<T, Rows, Columns> l_mtx( p_mtxA);
		l_mtx += p_mtxB;
		return l_mtx;
	}
	template <typename T, size_t Rows, size_t Columns, typename U>
	Matrix <T, Rows, Columns> operator - ( Matrix<T, Rows, Columns> const & p_mtxA, U const * p_mtxB)
	{
		Matrix<T, Rows, Columns> l_mtx( p_mtxA);
		l_mtx -= p_mtxB;
		return l_mtx;
	}
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator + ( Matrix<T, Rows, Columns> const & p_matrix, T const & p_uValue)
	{
		Matrix<T, Rows, Columns> l_mtx( p_matrix);
		l_mtx += p_uValue;
		return l_mtx;
	}
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator - ( Matrix<T, Rows, Columns> const & p_matrix, T const & p_uValue)
	{
		Matrix<T, Rows, Columns> l_mtx( p_matrix);
		l_mtx -= p_uValue;
		return l_mtx;
	}
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator * ( Matrix<T, Rows, Columns> const & p_matrix, T const & p_uValue)
	{
		Matrix<T, Rows, Columns> l_mtx( p_matrix);
		l_mtx *= p_uValue;
		return l_mtx;
	}
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator / ( Matrix<T, Rows, Columns> const & p_matrix, T const & p_uValue)
	{
		Matrix<T, Rows, Columns> l_mtx( p_matrix);
		l_mtx /= p_uValue;
		return l_mtx;
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix <T, Rows, Columns> operator + ( T const & p_scalar, Matrix<T, Rows, Columns> const & p_matrix)
	{
		return operator +( p_matrix, p_scalar);
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix <T, Rows, Columns> operator - ( T const & p_scalar, Matrix<T, Rows, Columns> const & p_matrix)
	{
		return operator -( p_matrix, p_scalar);
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix <T, Rows, Columns> operator * ( T const & p_scalar, Matrix<T, Rows, Columns> const & p_matrix)
	{
		return operator *( p_matrix, p_scalar);
	}
	template <typename T, size_t Rows, size_t Columns>
	inline Matrix <T, Rows, Columns> operator - ( Matrix<T, Rows, Columns> const & p_matrix)
	{
		Matrix<T, Rows, Columns> l_mReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				Templates::Policy<T>::assign( l_mReturn[j][i], Templates::Policy<T>::negate( p_matrix[j][i]));
			}
		}

		return l_mReturn;
	}

//*************************************************************************************************
}
}
