namespace Castor
{	namespace Math
{
//*************************************************************************************************

	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> :: SquareMatrix()
		:	Matrix<T, Rows, Rows>( Templates::Policy<T>::zero())
	{
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> :: SquareMatrix( T const & p_tValue)
		:	Matrix<T, Rows, Rows>( p_tValue)
	{
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> :: SquareMatrix( SquareMatrix<T, Rows> const & p_matrix)
		:	Matrix<T, Rows, Rows>( p_matrix)
	{
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> :: SquareMatrix( SquareMatrix<Type, Rows> const & p_matrix)
		:	Matrix<T, Rows, Rows>( p_matrix)
	{
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> :: SquareMatrix( Matrix<Type, Rows, Rows> const & p_matrix)
		:	Matrix<T, Rows, Rows>( p_matrix)
	{
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> :: SquareMatrix( Type const * p_matrix)
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
		Templates::Policy<T>::init( l_tReturn);

		T l_tCofactor;
		T l_tValue;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			Templates::Policy<T>::assign( l_tCofactor, GetCofactor( i, 0));
			Templates::Policy<T>::assign( l_tValue, matrix_type::m_matrix[0][i]);
			Templates::Policy<T>::ass_add( l_tReturn, Templates::Policy<T>::multiply( l_tValue, l_tCofactor));
		}

		if (Templates::Policy<T>::is_null( l_tReturn))
		{
			Templates::Policy<T>::init( l_tReturn);
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
				Templates::Policy<T>::assign( matrix_type::m_matrix[i][j], l_mResult[j][i]);
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
				l_bReturn = Templates::Policy<T>::equals( matrix_type::m_matrix[i][j], matrix_type::m_matrix[j][i]);
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
				if ( ! Templates::Policy<T>::is_null( matrix_type::m_matrix[i][j] + matrix_type::m_matrix[j][i]))
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
		l_mReturn.SetIdentity();
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
						Templates::Policy<T>::assign( l_mReturn[l_j++][l_i], matrix_type::m_matrix[j][i]);
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

		if ( ! Templates::Policy<T>::is_null( l_tDeterminant))
		{
			SquareMatrix<T, Rows> l_mTmp;

			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Rows ; j++)
				{
					Templates::Policy<T>::assign( l_mTmp[j][i], Templates::Policy<T>::divide( GetCofactor( j, i), l_tDeterminant));
				}
			}

			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Rows ; j++)
				{
					if (Templates::Policy<T>::is_null( l_mTmp[j][i]))
					{
						Templates::Policy<T>::init( matrix_type::m_matrix[j][i]);
					}
					else
					{
						Templates::Policy<T>::assign( matrix_type::m_matrix[j][i], l_mTmp[j][i]);
					}
				}
			}
		}
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> SquareMatrix<T, Rows> :: Multiply( SquareMatrix<T, Rows> const & p_matrix)const
	{
		SquareMatrix<T, Rows> l_mResult;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				l_mResult[i] += matrix_type::m_matrix[j] * p_matrix[i][j];
			}
		}

		return l_mResult;
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> SquareMatrix<T, Rows> :: Multiply( SquareMatrix<Type, Rows> const & p_matrix)const
	{
		SquareMatrix<T, Rows> l_mResult;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				l_mResult[i] += matrix_type::m_matrix[j] * policy::convert( p_matrix[i][j]);
			}
		}

		return l_mResult;
	}
	template <typename T, size_t Rows>
	template <size_t _Columns>
	inline Matrix <T, Rows, _Columns> SquareMatrix<T, Rows> :: Multiply( Matrix <value_type, Rows, _Columns> const & p_matrix)const
	{
		Matrix <T, Rows, _Columns> l_mReturn;

		for (size_t i = 0 ; i < _Columns ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				l_mReturn[i] += matrix_type::m_matrix[j] * p_matrix[i][j];
			}
		}

		return l_mReturn;
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator =( Matrix<Type, Rows, Rows> const & p_matrix)
	{
		Matrix<T, Rows, Rows>::operator =( p_matrix);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator =( SquareMatrix<T, Rows> const & p_matrix)
	{
		Matrix<T, Rows, Rows>::operator =( p_matrix);
		return * this;
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator =( SquareMatrix<Type, Rows> const & p_matrix)
	{
		Matrix<T, Rows, Rows>::operator =( p_matrix);
		return * this;
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator += ( SquareMatrix<Type, Rows> const & p_matrix)
	{
		Matrix<T, Rows, Rows>::operator +=( p_matrix);
		return * this;
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator -= ( SquareMatrix<Type, Rows> const & p_matrix)
	{
		Matrix<T, Rows, Rows>::operator -=( p_matrix);
		return * this;
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator *= ( SquareMatrix<Type, Rows> const & p_matrix)
	{
		SquareMatrix<T, Rows> l_mResult;

//		std::cout << "this before : \n" << (* this) << "\n";
//		std::cout << "l_mResult before : \n" << l_mResult << "\n";

		for (size_t i = 0 ; i < Rows ; i++)
		{
//			std::cout << "l_mResult[" << i << "] before : " << l_mResult[i];

			for (size_t j = 0 ; j < Rows ; j++)
			{
//				std::cout << "p_matrix[" << i << "][" << j << "] : " << p_matrix[i][j] << "\tm_matrix[" << j << "] : " << m_matrix[j];
				l_mResult[i] += (this->m_matrix[j] * policy::convert( p_matrix[i][j]));
			}

//			std::cout << "l_mResult[" << i << "] after : " << l_mResult[i];
		}

//		std::cout << "l_mResult after : \n" << l_mResult << "\n";

		operator =( l_mResult);

//		std::cout << "this after : \n" << (* this) << "\n";
		return * this;
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator =( Type const * p_pMatrix)
	{
		Matrix<T, Rows, Rows>::operator =( p_pMatrix);
		return * this;
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator += ( Type const * p_pMatrix)
	{
		Matrix<T, Rows, Rows>::operator +=( p_pMatrix);
		return * this;
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator -= ( Type const * p_pMatrix)
	{
		Matrix<T, Rows, Rows>::operator -=( p_pMatrix);
		return * this;
	}
	template <typename T, size_t Rows>
	template <typename Type>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator *= ( Type const * p_pMatrix)
	{
		* this = Multiply( p_pMatrix);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator += ( T const & p_tValue)
	{
		Matrix<T, Rows, Rows>::operator +=( p_tValue);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator -= ( T const & p_tValue)
	{
		Matrix<T, Rows, Rows>::operator -=( p_tValue);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator *= ( T const & p_tValue)
	{
		Matrix<T, Rows, Rows>::operator *=( p_tValue);
		return * this;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix<T, Rows> & SquareMatrix<T, Rows> :: operator /= ( T const & p_tValue)
	{
		Matrix<T, Rows, Rows>::operator /=( p_tValue);
		return * this;
	}

//*************************************************************************************************

	template <typename T, size_t Rows>
	inline bool operator ==( SquareMatrix<T, Rows> const & p_mtxA, SquareMatrix<T, Rows> const & p_mtxB)
	{
		bool l_bReturn = true;

		for (size_t i = 0 ; i < Rows && l_bReturn ; i++)
		{
			for (size_t j = 0 ; j < Rows && l_bReturn ; j++)
			{
				l_bReturn = Templates::Policy<T>::equals( p_mtxA[j][i], p_mtxB[j][i]);
			}
		}

		return l_bReturn;
	}
	template <typename T, size_t Rows>
	inline bool operator !=( SquareMatrix<T, Rows> const & p_mtxA, SquareMatrix<T, Rows> const & p_mtxB)
	{
		return ! operator ==( p_mtxA, p_mtxB);
	}
	template <typename T, size_t Rows>
	inline std::ostream & operator << ( std::ostream & l_streamOut, SquareMatrix <T, Rows> const & p_matrix)
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
	template <typename T, size_t Rows, typename U>
	SquareMatrix <T, Rows> operator + ( SquareMatrix <T, Rows> const & p_mtxA, SquareMatrix <U, Rows> const & p_mtxB)
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_mtxA);
		l_mtxReturn += p_mtxB;
		return l_mtxReturn;
	}
	template <typename T, size_t Rows, typename U>
	SquareMatrix <T, Rows> operator - ( SquareMatrix <T, Rows> const & p_mtxA, SquareMatrix <U, Rows> const & p_mtxB)
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_mtxA);
		l_mtxReturn -= p_mtxB;
		return l_mtxReturn;
	}
	template <typename T, size_t Rows, typename U>
	SquareMatrix <T, Rows> operator * ( SquareMatrix <T, Rows> const & p_mtxA, SquareMatrix <U, Rows> const & p_mtxB)
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_mtxA);
		l_mtxReturn *= p_mtxB;
		return l_mtxReturn;
	}
	template <typename T, size_t Rows, typename U>
	SquareMatrix <T, Rows> operator + ( SquareMatrix <T, Rows> const & p_matrix, U const * p_pMatrix)
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix);
		l_mtxReturn += p_pMatrix;
		return l_mtxReturn;
	}
	template <typename T, size_t Rows, typename U>
	SquareMatrix <T, Rows> operator - ( SquareMatrix <T, Rows> const & p_matrix, U const * p_pMatrix)
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix);
		l_mtxReturn -= p_pMatrix;
		return l_mtxReturn;
	}
	template <typename T, size_t Rows, typename U>
	SquareMatrix <T, Rows> operator * ( SquareMatrix <T, Rows> const & p_matrix, U const * p_pMatrix)
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix);
		l_mtxReturn *= p_pMatrix;
		return l_mtxReturn;
	}
	template <typename T, size_t Rows, typename U>
	SquareMatrix <T, Rows> operator + ( SquareMatrix <T, Rows> const & p_matrix, T const & p_tValue)
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix);
		l_mtxReturn += p_tValue;
		return l_mtxReturn;
	}
	template <typename T, size_t Rows, typename U>
	SquareMatrix <T, Rows> operator - ( SquareMatrix <T, Rows> const & p_matrix, T const & p_tValue)
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix);
		l_mtxReturn -= p_tValue;
		return l_mtxReturn;
	}
	template <typename T, size_t Rows, typename U>
	SquareMatrix <T, Rows> operator * ( SquareMatrix <T, Rows> const & p_matrix, T const & p_tValue)
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix);
		l_mtxReturn *= p_tValue;
		return l_mtxReturn;
	}
	template <typename T, size_t Rows, typename U>
	SquareMatrix <T, Rows> operator / ( SquareMatrix <T, Rows> const & p_matrix, T const & p_tValue)
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix);
		l_mtxReturn /= p_tValue;
		return l_mtxReturn;
	}
	template <typename T, size_t Rows, typename U>
	inline Point <T, Rows> operator * ( SquareMatrix <T, Rows> const & p_matrix, const Point<U, Rows> & p_ptVector)
	{
		Point<T, Rows> l_ptReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			Templates::Policy<T>::init( l_ptReturn[i]);

			for (size_t j = 0 ; j < Rows ; j++)
			{
				Templates::Policy<T>::ass_add( l_ptReturn[i], Templates::Policy<T>::multiply( p_matrix[j][i], p_ptVector[j]));
			}
		}

		return l_ptReturn;
	}
	template <typename T, size_t Rows>
	inline SquareMatrix <T, Rows> operator + ( T p_tValue, SquareMatrix <T, Rows> const & p_matrix)
	{
		return operator +( p_matrix, p_tValue);
	}
	template <typename T, size_t Rows>
	inline SquareMatrix <T, Rows> operator - ( T p_tValue, SquareMatrix <T, Rows> const & p_matrix)
	{
		return operator -( p_matrix, p_tValue);
	}
	template <typename T, size_t Rows>
	inline SquareMatrix <T, Rows> operator * ( T p_tValue, SquareMatrix <T, Rows> const & p_matrix)
	{
		return operator *( p_matrix, p_tValue);
	}
	template <typename T, size_t Rows>
	inline SquareMatrix <T, Rows> operator - ( SquareMatrix <T, Rows> const & p_matrix)
	{
		Matrix<T, Rows, Rows> l_mReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				Templates::Policy<T>::assign( l_mReturn[i][j], Templates::Policy<T>::negate( p_matrix[i][j]));
			}
		}

		return l_mReturn;
	}

//*************************************************************************************************

	template <typename T> struct CoFactorComputer<T, 1>
	{
		static T Value( SquareMatrix<T, 1> const & p_matrix, size_t p_uiRow, size_t p_uiColumn)
		{
			return Templates::Policy<T>::unit();
		}
	};
	template <typename T, size_t Rows> struct CoFactorComputer//<T, Rows>
	{
		static T Value( SquareMatrix<T, Rows> const & p_matrix, size_t p_uiRow, size_t p_uiColumn)
		{
			T l_tReturn;
			Templates::Policy<T>::init( l_tReturn);
			SquareMatrix<T, Rows-1> l_mTmp = p_matrix.GetMinor( p_uiRow, p_uiColumn);

			if ((p_uiRow + p_uiColumn) % 2 == 0)
			{
				Templates::Policy<T>::assign( l_tReturn, l_mTmp.GetDeterminant());
			}
			else
			{
				Templates::Policy<T>::assign( l_tReturn, -l_mTmp.GetDeterminant());
			}

			return l_tReturn;
		}
	};

//*************************************************************************************************
}
}
