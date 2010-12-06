namespace Castor
{	namespace Math
{
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> :: SquareMatrix()
		:	Matrix<T, Rows, Rows>()
	{
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> :: SquareMatrix( const SquareMatrix<T, Rows> & p_matrix)
		:	Matrix<T, Rows, Rows>( p_matrix)
	{
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> :: SquareMatrix( const typename SquareMatrix<T, Rows>::matrix_type & p_matrix)
		:	Matrix<T, Rows, Rows>( p_matrix)
	{
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> :: SquareMatrix( const T & p_value)
		:	Matrix<T, Rows, Rows>( p_value)
	{
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> :: SquareMatrix( const T * p_matrix)
		:	Matrix<T, Rows, Rows>( p_matrix)
	{
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> :: ~SquareMatrix()
	{
	}
	template <typename T, size_t Rows>
	inline T SquareMatrix<T, Rows> :: GetDeterminant()const
	{
		T l_tReturn;
		Templates::Value<T>::init( l_tReturn);

		T l_tCofactor;
		T l_tValue;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			Templates::Value<T>::assign( l_tCofactor, GetCofactor( i, 0));
			Templates::Value<T>::assign( l_tValue, m_matrix[0][i]);
			Templates::Value<T>::ass_add( l_tReturn, Templates::Value<T>::multiply( l_tValue, l_tCofactor));
		}

		if (Templates::Value<T>::is_null( l_tReturn))
		{
			Templates::Value<T>::init( l_tReturn);
		}

		return l_tReturn;
	}
	template <typename T, size_t Rows>
	inline bool SquareMatrix<T, Rows> :: IsOrthogonal()const
	{
		bool l_bReturn = false;

		Matrix<T, Rows, Rows> l_mTmp( * this);
		l_mTmp.Transpose();
		SquareMatrix<T, Rows> l_mId1;
		l_mId1.Identity();
		SquareMatrix<T, Rows> l_mId2;
		l_mId2.Identity();

		if ((l_mId1 != (* this) * l_mTmp) || (l_mId2 != l_mTmp * (* this)))
		{
			l_bReturn = false;
		}
		else
		{
			l_bReturn = true;
		}

		return l_bReturn;
	}
	template <typename T, size_t Rows>
	inline void SquareMatrix<T, Rows> :: Transpose()
	{
		Matrix<T, Rows, Rows> l_mResult( * this);

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				Templates::Value<T>::assign( m_matrix[i][j], l_mResult[j][i]);
			}
		}
	}
	template <typename T, size_t Rows>
	inline bool SquareMatrix<T, Rows> :: IsSymmetrical()const
	{
		bool l_bReturn = true;

		for (size_t i = 0 ; i < Rows && l_bReturn ; i++)
		{
			for (size_t j = 0 ; j < Rows && l_bReturn ; j++)
			{
				l_bReturn = Templates::Value<T>::equals( m_matrix[i][j], m_matrix[j][i]);
			}
		}

		return l_bReturn;
	}
	template <typename T, size_t Rows>
	inline bool SquareMatrix<T, Rows> :: IsAntiSymmetrical()const
	{
		bool l_bReturn = true;

		for (size_t i = 0 ; i < Rows && l_bReturn ; i++)
		{
			for (size_t j = 0 ; j < Rows && l_bReturn ; j++)
			{
				if ( ! Templates::Value<T>::is_null( m_matrix[i][j] + m_matrix[j][i]))
				{
					l_bReturn = false;
				}
			}
		}

		return l_bReturn;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> SquareMatrix<T, Rows> :: GetInverse()const
	{
		SquareMatrix<T, Rows> l_mReturn( * this);
		l_mReturn.Invert();
		return l_mReturn;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows-1> SquareMatrix<T, Rows> :: GetMinor( size_t x, size_t y)const
	{
		SquareMatrix<T, Rows-1> l_mReturn;
		l_mReturn.Identity();
		size_t l_i = 0, l_j = 0;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			if (i != x)
			{
				l_j = 0;

				for (size_t j = 0 ; j < Rows ; j++)
				{
					if (j != y)
					{
						Templates::Value<T>::assign( l_mReturn[l_j++][l_i], m_matrix[j][i]);
					}
				}

				l_i++;
			}
		}

		return l_mReturn;
	}
	template <typename T, size_t Rows>
	inline T SquareMatrix<T, Rows> :: GetCofactor( size_t p_uiRow, size_t p_uiColumn)const
	{
		return CoFactorComputer<T, Rows>::Value( *this, p_uiRow, p_uiColumn);
	}
	template <typename T, size_t Rows>
	inline void SquareMatrix<T, Rows> :: Invert()
	{
		T l_tDeterminant = GetDeterminant();

		if ( ! Templates::Value<T>::is_null( l_tDeterminant))
		{
			SquareMatrix<T, Rows> l_mTmp;

			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Rows ; j++)
				{
					Templates::Value<T>::assign( l_mTmp[j][i], Templates::Value<T>::divide( GetCofactor( j, i), l_tDeterminant));
				}
			}

			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Rows ; j++)
				{
					if (Templates::Value<T>::is_null( l_mTmp[j][i]))
					{
						Templates::Value<T>::init( m_matrix[j][i]);
					}
					else
					{
						Templates::Value<T>::assign( m_matrix[j][i], l_mTmp[j][i]);
					}
				}
			}
		}
	}
	template <typename T, size_t Rows>
	inline bool SquareMatrix<T, Rows> :: operator ==( const SquareMatrix<T, Rows> & p_matrix)const
	{
		return Matrix<T, Rows, Rows>::operator ==( Matrix<T, Rows, Rows>( p_matrix));
	}
	template <typename T, size_t Rows>
	inline bool SquareMatrix<T, Rows> :: operator !=( const SquareMatrix<T, Rows> & p_matrix)const
	{
		return ! operator ==( p_matrix);
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator =( const SquareMatrix<T, Rows> & p_matrix)
	{
		Matrix<T, Rows, Rows>::operator =( p_matrix);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator =( const typename SquareMatrix<T, Rows>::matrix_type & p_matrix)
	{
		Matrix<T, Rows, Rows>::operator =( p_matrix);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator =( const T * p_matrix)
	{
		Matrix<T, Rows, Rows>::operator =( p_matrix);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> SquareMatrix<T, Rows> :: operator + ( const SquareMatrix<T, Rows> & p_matrix)const
	{
		return Matrix<T, Rows, Rows>::operator +( p_matrix);
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator += ( const SquareMatrix<T, Rows> & p_matrix)
	{
		Matrix<T, Rows, Rows>::operator +=( p_matrix);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> SquareMatrix<T, Rows> :: operator - ( const SquareMatrix<T, Rows> & p_matrix)const
	{
		return Matrix<T, Rows, Rows>::operator -( p_matrix);
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator -= ( const SquareMatrix<T, Rows> & p_matrix)
	{
		Matrix<T, Rows, Rows>::operator -=( p_matrix);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> SquareMatrix<T, Rows> :: operator * ( T p_scalar)const
	{
		return SquareMatrix<T, Rows>( Matrix<T, Rows, Rows>::operator *( p_scalar));
	}
	template <typename T, size_t Rows>
	inline Point<T, Rows> SquareMatrix<T, Rows> :: operator * ( const Point<T, Rows> & p_ptVector)const
	{
		return Matrix<T, Rows, Rows>::operator *( p_ptVector);
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> SquareMatrix<T, Rows> :: operator * ( const SquareMatrix<T, Rows> & p_matrix)const
	{
		return Multiply( p_matrix);
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator *= ( T p_scalar)
	{
		Matrix<T, Rows, Rows>::operator *=( p_scalar);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator *= ( const SquareMatrix<T, Rows> & p_matrix)
	{
		* this = Multiply( p_matrix);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator /= ( T p_scalar)
	{
		Matrix<T, Rows, Rows>::operator /=( p_scalar);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> SquareMatrix<T, Rows> :: operator / ( T p_scalar)const
	{
		return SquareMatrix<T, Rows>( Matrix<T, Rows, Rows>::operator /( p_scalar));
	}
	template <typename T, size_t Rows>
	inline std::ostream & SquareMatrix<T, Rows> :: operator << ( std::ostream & l_streamOut)const
	{
		return Matrix<T, Rows, Rows>::operator <<( l_streamOut);
	}
	template <typename T, size_t Rows>
	inline std::istream & SquareMatrix<T, Rows> :: operator >> ( std::istream & l_streamIn)
	{
		return Matrix<T, Rows, Rows>::operator >>( l_streamIn);
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> SquareMatrix<T, Rows> :: Multiply( const SquareMatrix<T, Rows> & p_matrix)const
	{
		SquareMatrix<T, Rows> l_mResult;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				l_mResult[i] += m_matrix[j] * p_matrix[i][j];
			}
		}

		return l_mResult;
	}




	template <typename T, size_t Rows>
	inline SquareMatrix <T, Rows> operator * ( T p_tScalar, const SquareMatrix <T, Rows> & p_matrix)
	{
		SquareMatrix <T, Rows> l_mReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				Templates::Value<T>::assign( l_mReturn[i][j], Templates::Value<T>::multiply( p_tScalar, p_matrix[i][j]));

				if (Templates::Value<T>::is_null( l_mReturn[i][j]))
				{
					Templates::Value<T>::init( l_mReturn[i][j]);
				}
			}
		}

		return l_mReturn;
	}
	template <typename T, size_t Rows>
	inline Point <T, Rows> operator * ( const SquareMatrix <T, Rows> & p_matrix, const Point<T, Rows> & p_ptVector)
	{
		Point<T, Rows> l_ptReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			Templates::Value<T>::init( l_ptReturn[i]);

			for (size_t j = 0 ; j < Rows ; j++)
			{
				Templates::Value<T>::ass_add( l_ptReturn[i], Templates::Value<T>::multiply( p_matrix[j][i], p_ptVector[j]));
			}
		}

		return l_ptReturn;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix <T, Rows> operator - ( const SquareMatrix <T, Rows> & p_matrix)
	{
		Matrix<T, Rows, Rows> l_mReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				Templates::Value<T>::assign( l_mReturn[i][j], Templates::Value<T>::negate( p_matrix[i][j]));
			}
		}

		return l_mReturn;
	}
	template <typename T, size_t Rows>
	inline std::ostream & operator << ( std::ostream & l_streamOut, const SquareMatrix <T, Rows> & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				l_streamOut << "\t" << p_matrix[j][i];
			}

			l_streamOut << std::endl;
		}

		return l_streamOut;
	}
	template <typename T, size_t Rows>
	inline std::istream & operator >> ( std::istream & l_streamIn, SquareMatrix <T, Rows> & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				l_streamIn >> p_matrix[j][i];
			}
		}

		return l_streamIn;
	}
	template <typename T> struct CoFactorComputer<T, 1>
	{
		static T Value( const SquareMatrix<T, 1> & p_matrix, size_t p_uiRow, size_t p_uiColumn)
		{
			return Templates::Value<T>::unit();
		}
	};
	template <typename T, size_t Rows> struct CoFactorComputer<T, Rows>
	{
		static T Value( const SquareMatrix<T, Rows> & p_matrix, size_t p_uiRow, size_t p_uiColumn)
		{
			T l_tReturn;
			Templates::Value<T>::init( l_tReturn);
			SquareMatrix<T, Rows-1> l_mTmp = p_matrix.GetMinor( p_uiRow, p_uiColumn);

			if ((p_uiRow + p_uiColumn) % 2 == 0)
			{
				Templates::Value<T>::assign( l_tReturn, l_mTmp.GetDeterminant());
			}
			else
			{
				Templates::Value<T>::assign( l_tReturn, -l_mTmp.GetDeterminant());
			}

			return l_tReturn;
		}
	};
}
}