/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Castor_Matrix___
#define ___Castor_Matrix___

#include "Module_Utils.h"
#include "Exception.h"
#include <list>
#include <cstdarg>

namespace Castor
{	namespace Math
{
	/*!
	Templated matrix representation
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <typename T, size_t Rows, size_t Columns>
	class Matrix
	{
	protected:
		typedef T									value_type;
		typedef Matrix<value_type, Rows, Columns>	my_type;
		typedef Matrix<value_type, Columns, Rows>	my_transpose;
		typedef Matrix<value_type, 1, Columns>		my_row;
		typedef Matrix<value_type, Rows, 1>			my_column;
		typedef Templates::Value<value_type>		value;

	public:
		typedef my_transpose Transpose;
		T m_matrix[Rows * Columns];

	public:
		Matrix()
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::init( m_matrix[i * Columns + j]);
				}
			}
		}
		Matrix( const my_type & matrix)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::assign( m_matrix[i * Columns + j], matrix[i * Columns + j]);
				}
			}
		}
		explicit Matrix( value_type * matrix)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::assign( m_matrix[i * Columns + j], matrix[i * Columns + j]);
				}
			}
		}
		virtual ~Matrix()
		{
		}
		void SetRow( size_t p_uiRow, ...)
		{
			if (Columns >= 1)
			{
				va_list l_list;
				va_start( l_list, p_uiRow);

				for (size_t i = 0 ; i < Columns ; i++)
				{
					value::assign( m_matrix[p_uiRow * Columns + i], va_arg( l_list, value_type));
				}

				va_end( l_list);
			}
		}
		my_row GetRow( size_t p_uiRow)
		{
			CASTOR_ASSERT( p_uiRow < Rows);
			my_row l_mReturn;

			for (size_t j = 0 ; j < Columns ; j++)
			{
				value::assign( l_mReturn[j], m_matrix[p_uiRow * Columns + j]);
			}

			return mReturn;
		}
		void GetRow( my_row & p_mResult, size_t p_uiRow)
		{
			CASTOR_ASSERT( p_uiRow < Rows);

			for (size_t j = 0 ; j < Columns ; j++)
			{
				value::assign( p_mResult[j], m_matrix[p_uiRow * Columns + j]);
			}
		}
		my_column GetColumn( size_t p_uiColumn)
		{
			CASTOR_ASSERT( p_uiColumn < Columns);
			my_row l_mReturn;

			for (size_t i = 0 ; i < Rows ; i++)
			{
				value::assign( l_mReturn[i], m_matrix[i * Columns + p_uiColumn]);
			}

			return mReturn;
		}
		void GetColumn( my_column & p_mResult, size_t p_uiColumn)
		{
			CASTOR_ASSERT( p_uiColumn < Columns);

			for (size_t i = 0 ; i < Rows ; i++)
			{
				value::assign( p_mResult[i], m_matrix[i * Columns + p_uiColumn]);
			}
		}
		value_type GetValue( size_t p_uiRow, size_t p_uiColumn)
		{
			CASTOR_ASSERT( p_uiRow < Rows);
			CASTOR_ASSERT( p_uiColumn < Columns);

			return m_matrix[p_uiRow * Columns + p_uiColumn];
		}
		my_transpose GetTransposed()
		{
			my_transpose l_mReturn;

			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::assign( l_mReturn[j * Columns + i], m_matrix[i * Columns + j]);
				}
			}

			return l_mReturn;
		}
		void GetTransposed( my_transpose & p_mResult)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::assign( p_mResult[j * Columns + i], m_matrix[i * Columns + j]);
				}
			}
		}
		void Triangle()
		{
			size_t l_uiMinDim = std::min<size_t>( Rows, Columns);
			bool l_bContinue = true;
			value_type l_tCoef;
			size_t l_uiMaxIndex;

			for (size_t i = 0 ; i < l_uiMinDim && l_bContinue ; i++)
			{
				l_uiMaxIndex = 0;

				for (size_t j = 1 ; j < Rows ; j++)
				{
					if (std::abs( m_matrix[j * Columns + i]) > std::abs( m_matrix[l_uiMaxIndex * Columns + i]))
					{
						l_uiMaxIndex = j;
					}
				}

				if ( ! value::is_null( m_matrix[l_uiMaxIndex * Columns + i]))
				{
					for (size_t k = i ; k < Columns ; k++)
					{
						std::swap( m_matrix[i * Columns + k], m_matrix[l_uiMaxIndex * Columns + k]);
					}

					for (size_t j = i + 1 ; j < Rows ; j++)
					{
						value::assign( l_tCoef, value::divide( m_matrix[j * Columns + i], m_matrix[i * Columns + i]));

						for (size_t k = Columns - 1 ; k >= i && k < Columns ; k--)
						{
							value::ass_substract( m_matrix[j * Columns + k], l_tCoef * m_matrix[i * Columns + k]);
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
						if (value::is_null( m_matrix[i * Columns + j]))
						{
							value::init( m_matrix[i * Columns + j]);
						}
					}
				}
			}
		}
		my_type GetMinor( size_t x, size_t y, size_t p_uiRows, size_t p_uiCols)
		{
			my_type l_mReturn;
			l_mReturn.ToIdentity();
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
							value::assign( l_mReturn[l_i * Columns + l_j++], m_matrix[i * Columns + j]);
						}
					}

					l_i++;
				}
			}

			return l_mReturn;
		}
		value_type GetCofactor( size_t p_uiRow, size_t p_uiColumn, size_t p_uiRows, size_t p_uiCols)
		{
			value_type l_tReturn;
			value::assign( l_tReturn, value::zero());

			if (p_uiRows > 1)
			{
				my_type l_mTmp = GetMinor( p_uiRow, p_uiColumn, p_uiRows, p_uiCols);

				if ((p_uiRow + p_uiColumn) % 2 == 0)
				{
					value::assign( l_tReturn, l_mTmp.GetDeterminant( p_uiRows - 1, p_uiCols - 1));
				}
				else
				{
					value::assign( l_tReturn, -l_mTmp.GetDeterminant( p_uiRows - 1, p_uiCols - 1));
				}

			}
			else if (p_uiRows == 1)
			{
				value::assign( l_tReturn, value::unit());
			}

			return l_tReturn;
		}
		value_type GetTrace()
		{
			value_type l_tSum;
			value::init( l_tSum);

			for (int i = 0 ; i < Rows ; i++)
			{
				if ( ! value::is_null( m_matrix[i * Columns + i]))
				{
					value::ass_add( l_tSum, m_matrix[i * Columns + i]);
				}
			}

			return l_tSum;
		}
		my_type GetInverse()
		{
			my_type l_mReturn( * this);
			l_mReturn.Invert();
			return l_mReturn;
		}
		void ToIdentity()
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					if (i == j)
					{
						value::assign( m_matrix[i * Columns + j], value::unit());
					}
					else
					{
						value::init( m_matrix[i * Columns + j]);
					}
				}
			}
		}
		void ToJordan( value_type p_tLambda)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					if (i == j)
					{
						value::assign( m_matrix[i * Columns + i], p_tLambda);
					}
					else
					{
						value::init( m_matrix[i * Columns + j]);
					}
				}
			}

			value::assign( m_matrix[0 * Columns + 0], p_tLambda);

			for (size_t i = 0 ; i < Rows - 1 ; i++)
			{
				value::assign( m_matrix[i * Columns + i + 1], value::unit());
			}
		}
		value_type GetDeterminant( size_t p_uiRows = Rows, size_t p_uiCols = Columns)
		{
			value_type l_tReturn;
			value::init( l_tReturn);

			if (Rows == Columns)
			{
				value_type l_tCofactor;
				value_type l_tValue;

				for (size_t i = 0 ; i < Rows ; i++)
				{
					value::assign( l_tCofactor, GetCofactor( i, 0, p_uiRows, p_uiCols));
					value::assign( l_tValue, m_matrix[i * Columns + 0]);
					value::ass_add( l_tReturn, value::multiply( l_tValue, l_tCofactor));
				}

				if (value::is_null( l_tReturn))
				{
					value::init( l_tReturn);
				}
			}
			else
			{
				CASTOR_EXCEPTION( "Can't compute the determinant of a non square matrix");
			}

			return l_tReturn;
		}
		inline bool operator ==( const my_type & p_matrix)
		{
			bool l_bReturn = true;

			for (size_t i = 0 ; i < Rows && l_bReturn ; i++)
			{
				for (size_t j = 0 ; j < Columns && l_bReturn ; j++)
				{
					l_bReturn = value::equals( m_matrix[i * Columns + j], p_matrix[i * Columns + j]);
				}
			}

			return l_bReturn;
		}
		inline bool operator !=( const my_type & p_matrix)
		{
			bool l_bReturn = false;

			for (size_t i = 0 ; i < 2 && ! l_bReturn ; i++)
			{
				for (size_t j = 0 ; j < 2 && ! l_bReturn ; j++)
				{
					l_bReturn = ! value::equals( m_matrix[i * Columns + j], p_matrix[i * Columns + j]);
				}
			}

			return l_bReturn;
		}
		inline my_type operator =( const my_type & p_matrix)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::assign( m_matrix[i * Columns + j], p_matrix[i * Columns + j]);
				}
			}

			return * this;
		}
		inline my_type operator =( const value_type * p_matrix)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::assign( m_matrix[i * Columns + j], p_matrix[i * Columns + j]);
				}
			}

			return * this;
		}
		/**
		 *		+ operators
		 */
		my_type operator + ( const my_type & p_matrix)
		{
			my_type l_mReturn;

			for(size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::assign( l_mReturn[i * Columns + j], value::add( m_matrix[i * Columns + j], p_matrix[i * Columns + j]));
				}
			}
		}
		void operator += ( const my_type & p_matrix)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					if ( ! value::is_null( p_matrix[i * Columns + j]))
					{
						value::ass_add( m_matrix[i * Columns + j], p_matrix[i * Columns + j]);
					}
				}
			}
		}
		/**
		 *		- operators
		 */
		my_type operator - ( const my_type & p_matrix)
		{
			my_type l_mReturn;

			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::assign( l_mReturn[i * Columns + j], value::substract( m_matrix[i * Columns + j], p_matrix[i * Columns + j]));
				}
			}
		}
		my_type operator -= ( const my_type & p_matrix)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					if ( ! value::is_null( p_matrix[i * Columns + j]))
					{
						value::ass_substract( m_matrix[i * Columns + j], p_matrix[i * Columns + j]);
					}
				}
			}
		}
		/**
		 *		* operators
		 */
		template <typename _Ty>
		void operator *= ( _Ty p_scalar)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::ass_multiply( m_matrix[i * Columns + j], p_scalar);

					if (value::is_null( m_matrix[i * Columns + j]))
					{
						value::init( m_matrix[i * Columns + j]);
					}
				}
			}
		}
		template <size_t _Columns>
		Matrix<value_type, Rows, _Columns> operator * ( const Matrix<value_type, Columns, _Columns> & p_matrix)
		{
			Matrix<value_type, Rows, _Columns> l_mReturn;
			value_type l_tSomme;

			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < _Columns ; j++)
				{
					value::init( l_tSomme);

					for (size_t k = 0 ; k < Columns ; k++)
					{
						value::ass_add( l_tSomme, value::multiply( m_matrix[i * Columns + k], p_matrix[k * _Columns + j]));
					}

					if (value::is_null(l_tSomme))
					{
						value::init( l_mReturn[i * _Columns + j]);
					}
					else
					{
						value::assign( l_mReturn[i * _Columns + j], l_tSomme);
					}
				}
			}

			return l_mReturn;
		}
		/**
		 *		/ operators
		 */
		template <typename _Ty>
		void operator /= ( _Ty p_scalar)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::ass_divide( m_matrix[i * Columns + j], p_scalar);

					if (value::is_null( m_matrix[i * Columns + j]))
					{
						value::init( m_matrix[i * Columns + j]);
					}
				}
			}
		}
		const value_type & operator []( size_t i)const
		{
			CASTOR_ASSERT( i < Rows * Columns);
			return m_matrix[i];
		}
		value_type & operator []( size_t i)
		{
			CASTOR_ASSERT( i < Rows * Columns);
			return m_matrix[i];
		}
		inline value_type * ptr()
		{
			return m_matrix;
		}
		inline const value_type * const_ptr()const
		{
			return m_matrix;
		}
		std::ostream & operator << ( std::ostream & l_streamOut)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					l_streamOut << "\t" << m_matrix[i * Columns + j];
				}

				l_streamOut << std::endl;
			}
			return l_streamOut;
		}
		std::istream & operator >> ( std::istream & l_streamIn)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					l_streamIn >> m_matrix[i * Columns + j];
				}
			}

			return l_streamIn;
		}

	protected:
		void _recheck()
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::stick( m_matrix[i * Columns + j]);
				}
			}
		}
	};

	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator * ( T p_tScalar, const Matrix <T, Rows, Columns> & p_matrix)
	{
		Matrix <T, Rows, Columns> l_mReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				Value<T>::assign( l_mReturn[i * Columns + j], Value<T>::multiply( p_tScalar, p_matrix[i * Columns + j]));

				if (Value<T>::is_null( l_mReturn[i * Columns + j]))
				{
					Value<T>::init( l_mReturn[i * Columns + j]);
				}
			}
		}

		return l_mReturn;
	}

	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator - ( const Matrix <T, Rows, Columns> & p_matrix)
	{
		my_type l_mReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				value::assign( l_mReturn[i * Columns + j], Value<T>::Negate( p_matrix[i * Columns + j]));
			}
		}

		return l_mReturn;
	}
	template <typename T, size_t Rows, size_t Columns>
	std::ostream & operator << ( std::ostream & l_streamOut, const Matrix <T, Rows, Columns> & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				l_streamOut << "\t" << p_matrix[i * Columns + j];
			}

			l_streamOut << std::endl;
		}

		return l_streamOut;
	}
	template <typename T, size_t Rows, size_t Columns>
	std::istream & operator >> ( std::istream & l_streamIn, Matrix <T, Rows, Columns> & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				l_streamIn >> p_matrix[i * Columns + j];
			}
		}

		return l_streamIn;
	}
	/*!
	Templated matrix representation
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <typename T, size_t Rows>
	class SquareMatrix : public Matrix<T, Rows, Rows>
	{
	protected:
		typedef SquareMatrix<value_type, Rows>		my_square_type;
		typedef Templates::Value<value_type>		value;

	public:
		SquareMatrix()
		{
		}
		SquareMatrix( const my_square_type & p_matrix)
			:	my_type( p_matrix)
		{
		}
		explicit SquareMatrix( value_type * p_matrix)
			:	my_type( p_matrix)
		{
		}
		virtual ~SquareMatrix()
		{
		}
		bool IsOrthogonal()
		{
			bool l_bReturn = false;

			my_type l_mTmp( * this);
			l_mTmp.ToTranspose();
			my_square_type l_mId1;
			l_mId1.Identity();
			my_square_type l_mId2;
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
		void ToTranspose()
		{
			my_type l_mResult( * this);

			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					value::assign( m_matrix[i * Columns + j], l_mResult[j * Columns + i]);
				}
			}
		}
		bool IsSymmetrical()
		{
			bool l_bReturn = true;

			for (size_t i = 0 ; i < Rows && l_bReturn ; i++)
			{
				for (size_t j = 0 ; j < Columns && l_bReturn ; j++)
				{
					l_bReturn = value::equals( m_matrix[i * Columns + j], m_matrix[j * Columns + i]);
				}
			}

			return l_bReturn;
		}
		bool IsAntiSymmetrical()
		{
			bool l_bReturn = true;

			for (size_t i = 0 ; i < Rows && l_bReturn ; i++)
			{
				for (size_t j = 0 ; j < Columns && l_bReturn ; j++)
				{
					if ( ! value::is_null( m_matrix[i * Columns + j] + m_matrix[j * Columns + i]))
					{
						l_bReturn = false;
					}
				}
			}

			return l_bReturn;
		}
		void Invert()
		{
			value_type l_tDeterminant = GetDeterminant();

			if ( ! value::is_null( l_tDeterminant))
			{
				my_square_type l_mTmp;

				for (size_t i = 0 ; i < Rows ; i++)
				{
					for (size_t j = 0 ; j < Rows ; j++)
					{
						value::assign( l_mTmp[i * Rows + j], value::divide( GetCofactor( j, i, Rows, Rows), l_tDeterminant));
					}
				}

				for (size_t i = 0 ; i < Rows ; i++)
				{
					for (size_t j = 0 ; j < Rows ; j++)
					{
						if (value::is_null( l_mTmp[i * Rows + j]))
						{
							value::init( m_matrix[i * Rows + j]);
						}
						else
						{
							value::assign( m_matrix[i * Rows + j], l_mTmp[i * Rows + j]);
						}
					}
				}
			}
		}
		inline bool operator ==( const my_square_type & p_matrix)
		{
			return my_type::operator ==( my_type( p_matrix));
		}
		inline bool operator !=( const my_square_type & p_matrix)
		{
			return my_type::operator !=( my_type( p_matrix));
		}
		inline my_square_type operator =( const my_square_type & p_matrix)
		{
			my_type::operator =( my_type( p_matrix));
			return * this;
		}
		inline my_square_type operator =( const value_type * p_matrix)
		{
			return my_type::operator =( p_matrix);
			return * this;
		}
		/**
		 *		+ operators
		 */
		my_square_type operator + ( const my_square_type & p_matrix)
		{
			my_type::operator +( p_matrix);
			return * this;
		}
		void operator += ( const my_square_type & p_matrix)
		{
			my_type::operator +=( p_matrix);
		}
		/**
		 *		- operators
		 */
		my_square_type operator - ( const my_square_type & p_matrix)
		{
			my_type::operator -( p_matrix);
			return * this;
		}
		void operator -= ( const my_square_type & p_matrix)
		{
			my_type::operator -=( p_matrix);
		}
		/**
		 *		* operators
		 */
		template <typename _Ty>
		void operator *= ( _Ty p_scalar)
		{
			my_type::operator *=( p_scalar);
		}
		void operator *= ( const my_square_type & p_matrix)
		{
			my_square_type l_mTmp( * this);
			value_type l_tSomme;

			for(size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Rows ; j++)
				{
					value::init( l_tSomme);

					for (size_t k = 0 ; k < Rows ; k++)
					{
						value::ass_add( l_tSomme, value::multiply( l_mTmp[i * Rows + k], p_matrix[k * Rows + j]));
					}

					if (value::is_null( l_tSomme))
					{
						value::init( m_matrix[i * Rows + j]);
					}
					else
					{
						value::assign( m_matrix[i * Rows + j], l_tSomme);
					}
				}
			}
		}
		/**
		 *		/ operators
		 */
		template <typename _Ty>
		void operator /= ( _Ty p_scalar)
		{
			my_type::operator /=( p_scalar);
		}
		std::ostream & operator << ( std::ostream & l_streamOut)
		{
			return my_type::operator <<( l_streamOut);
		}
		std::istream & operator >> ( std::istream & l_streamIn)
		{
			return my_type::operator >>( l_streamIn);
		}
	};

	template <typename T, size_t Rows>
	SquareMatrix <T, Rows> operator * ( T p_tScalar, const SquareMatrix <T, Rows> & p_matrix)
	{
		SquareMatrix <T, Rows> l_mReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Rows ; j++)
			{
				Value<T>::assign( l_mReturn[i * Rows + j], Value<T>::multiply( p_tScalar, p_matrix[i * Rows + j]));

				if (Value<T>::is_null( l_mReturn[i * Rows + j]))
				{
					Value<T>::init( l_mReturn[i * Rows + j]);
				}
			}
		}

		return l_mReturn;
	}

	template <typename T, size_t Rows>
	SquareMatrix <T, Rows> operator - ( const SquareMatrix <T, Rows> & p_matrix)
	{
		my_type l_mReturn;

		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				value::assign( l_mReturn[i * Rows + j], Value<T>::negate( p_matrix[i * Rows + j]));
			}
		}

		return l_mReturn;
	}
	template <typename T, size_t Rows>
	std::ostream & operator << ( std::ostream & l_streamOut, const SquareMatrix <T, Rows> & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				l_streamOut << "\t" << p_matrix[i * Columns + j];
			}

			l_streamOut << std::endl;
		}

		return l_streamOut;
	}
	template <typename T, size_t Rows>
	std::istream & operator >> ( std::istream & l_streamIn, SquareMatrix <T, Rows> & p_matrix)
	{
		for (size_t i = 0 ; i < Rows ; i++)
		{
			for (size_t j = 0 ; j < Columns ; j++)
			{
				l_streamIn >> p_matrix[i * Columns + j];
			}
		}

		return l_streamIn;
	}
}
}

#endif