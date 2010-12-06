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
#include "Point.h"

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
		typedef Point<value_type, Columns>			my_row;
		typedef Point<value_type, Rows>				my_column;
		typedef Templates::Value<value_type>		value;

	private:
		bool m_bOwnCoords;

	public:
		typedef my_column col_type;
		typedef my_row row_type;
		typedef my_transpose transpose_type;

		col_type m_matrix[Columns];
		value_type * m_pPointer;

	public:
		Matrix( const value_type & p_value=value::zero());
		Matrix( const my_type & p_matrix);
		explicit Matrix( const value_type * p_pMatrix);
		template <typename Type>
		explicit Matrix( const Type * p_pMatrix)
			:	m_pPointer( new T[Rows * Columns]),
				m_bOwnCoords( true)
		{
			Init();
			operator =( p_pMatrix);
		}
		template <typename Type>
		explicit Matrix( const Matrix<Type, Columns, Rows> & p_matrix)
			:	m_pPointer( new T[Rows * Columns]),
				m_bOwnCoords( true)
		{
			Init();
			operator =( p_matrix);
		}
		virtual ~Matrix();

		void Init( const value_type & p_value=value::zero());
		/**
		 * Accessors
		 */
		void						SetRow( size_t p_uiRow, const value_type * p_row);
		void						SetRow( size_t p_uiRow, const row_type & p_row);
		row_type					GetRow( size_t p_uiRow)const;
		void 						GetRow( row_type & p_mResult, size_t p_uiRow)const;
		void 						SetColumn( size_t p_uiColumn, const value_type * p_col);
		void 						SetColumn( size_t p_uiColumn, const col_type & p_col);
		col_type					GetColumn( size_t p_uiColumn)const;
		void						GetColumn( col_type & p_mResult, size_t p_uiColumn)const;
		value_type					GetValue( size_t p_uiRow, size_t p_uiColumn);
		const col_type &			operator []( size_t i)const;
		col_type &					operator []( size_t i);
		inline value_type *			ptr();
		inline const value_type *	const_ptr()const;
		void						LinkCoords( void * p_pCoords);
		/**
		 * Operations
		 */
		transpose_type	GetTransposed()const;
		void			GetTransposed( transpose_type & p_mResult)const;
		my_type			GetTriangle()const;
		my_type			GetIdentity()const;
		my_type			GetJordan( value_type p_tLambda)const;
		my_type			GetMinor( size_t x, size_t y, size_t p_uiRows, size_t p_uiCols)const;
		value_type		GetTrace()const;
		void			Triangle();
		void			Identity();
		void			Jordan( value_type p_tLambda);
		/**
		 *	operators
		 */
		bool 			operator ==( const my_type & p_matrix)const;
		bool 			operator !=( const my_type & p_matrix)const;
		my_type &		operator = ( const my_type & p_matrix);
		my_type &		operator = ( const value_type * p_matrix);
		my_type			operator + ( const my_type & p_matrix)const;
		my_type			operator + ( value_type p_value)const;
		my_type &		operator +=( const my_type & p_matrix);
		my_type &		operator +=( value_type p_value);
		my_type			operator - ( const my_type & p_matrix)const;
		my_type			operator - ( value_type p_value)const;
		my_type &		operator -=( const my_type & p_matrix);
		my_type &		operator -=( value_type p_value);
		my_type			operator * ( value_type p_value)const;
		col_type		operator * ( const row_type & p_ptVector)const;
		my_type &		operator *=( value_type p_scalar);
		my_type			operator / ( value_type p_scalar)const;
		my_type &		operator /=( value_type p_scalar);
		std::ostream & 	operator <<( std::ostream & l_streamOut)const;
		std::istream & 	operator >>( std::istream & l_streamIn);
		template <size_t _Columns>
		Matrix<value_type, Rows, _Columns>	operator * ( const Matrix<value_type, Columns, _Columns> & p_matrix)const
		{
			Matrix<value_type, Rows, _Columns> l_mReturn;

			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < _Columns ; j++)
				{
					value::init( l_mReturn[j][i]);

					for (size_t k = 0 ; k < Columns ; k++)
					{
						value::ass_add( l_mReturn[j][i], value::multiply( m_matrix[k][i], p_matrix[j][k]));
					}
				}
			}

			return l_mReturn;
		}
		template <typename Type>
		my_type & operator = ( const Matrix<Type, Columns, Rows> & p_matrix)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					Templates::Value<T>::assign( m_matrix[j][i], value_type( p_matrix[j][i]));
				}
			}

			return * this;
		}
		template <typename Type>
		my_type & operator = ( const Type * p_pMatrix)
		{
			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Columns ; j++)
				{
					Templates::Value<T>::assign( m_matrix[j][i], value_type( p_pMatrix[j * Rows + i]));
				}
			}

			return * this;
		}
		template <typename Type>
		col_type operator * ( const Point<Type, Columns> & p_ptVector)const
		{
			col_type l_ptReturn;

			for (size_t i = 0 ; i < Rows ; i++)
			{
				Templates::Value<T>::init( l_ptReturn[i]);

				for (size_t j = 0 ; j < Columns ; j++)
				{
					Templates::Value<T>::ass_add( l_ptReturn[i], Templates::Value<T>::multiply( m_matrix[j][i], value_type( p_ptVector[j])));
				}
			}

			return l_ptReturn;
		}

	protected:
		void _recheck();
	};

	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator + ( int p_tScalar, const Matrix <T, Rows, Columns> & p_matrix);
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator + ( real p_tScalar, const Matrix <T, Rows, Columns> & p_matrix);
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator - ( int p_tScalar, const Matrix <T, Rows, Columns> & p_matrix);
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator - ( real p_tScalar, const Matrix <T, Rows, Columns> & p_matrix);
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator * ( int p_tScalar, const Matrix <T, Rows, Columns> & p_matrix);
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator * ( real p_tScalar, const Matrix <T, Rows, Columns> & p_matrix);
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator / ( int p_tScalar, const Matrix <T, Rows, Columns> & p_matrix);
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator / ( real p_tScalar, const Matrix <T, Rows, Columns> & p_matrix);
	template <typename T, size_t Rows, size_t Columns>
	Point <T, Rows> operator * ( const Matrix <T, Rows, Columns> & p_matrix, const Point<T, Columns> & p_ptVector);
	template <typename T, size_t Rows, size_t Columns>
	Matrix <T, Rows, Columns> operator - ( const Matrix <T, Rows, Columns> & p_matrix);
	template <typename T, size_t Rows, size_t Columns>
	std::ostream & operator << ( std::ostream & l_streamOut, const Matrix <T, Rows, Columns> & p_matrix);
	template <typename T, size_t Rows, size_t Columns>
	std::istream & operator >> ( std::istream & l_streamIn, Matrix <T, Rows, Columns> & p_matrix);

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
		typedef my_square_type	square_matrix_type;
		typedef my_type			matrix_type;

	public:
		SquareMatrix();
		SquareMatrix( const my_square_type & p_matrix);
		SquareMatrix( const my_type & p_matrix);
		SquareMatrix( const value_type & p_value);
		explicit SquareMatrix( const value_type * p_matrix);
		template <typename Type>
		SquareMatrix( const SquareMatrix<Type, Rows> & p_matrix)
			:	my_type( p_matrix)
		{
		}
		template <typename Type>
		SquareMatrix( const Matrix<Type, Rows, Rows> & p_matrix)
			:	my_type( p_matrix)
		{
		}
		template <typename Type>
		SquareMatrix( const Type * p_matrix)
			:	my_type( p_matrix)
		{
		}
		virtual ~SquareMatrix();
		/**
		 * Operations
		 */
		SquareMatrix<T, Rows-1>	GetMinor( size_t x, size_t y)const;
		value_type		GetCofactor( size_t p_uiRow, size_t p_uiColumn)const;
		value_type		GetDeterminant()const;
		bool			IsOrthogonal()const;
		void			Transpose();
		bool			IsSymmetrical()const;
		bool			IsAntiSymmetrical()const;
		my_square_type	GetInverse()const;
		virtual void	Invert();
		my_square_type	Multiply( const my_square_type & p_matrix)const;
		template <typename Type>
		my_square_type	Multiply( const SquareMatrix<Type, Rows> & p_matrix)const
		{
			SquareMatrix<T, Rows> l_mResult;

			for (size_t i = 0 ; i < Rows ; i++)
			{
				for (size_t j = 0 ; j < Rows ; j++)
				{
					l_mResult[i] += m_matrix[j] * value_type( p_matrix[i][j]);
				}
			}

			return l_mResult;
		}
		/**
		 *	Operators
		 */
		bool 				operator ==( const my_square_type & p_matrix)const;
		bool 				operator !=( const my_square_type & p_matrix)const;
		my_square_type &	operator = ( const my_square_type & p_matrix);
		my_square_type &	operator = ( const my_type & p_matrix);
		my_square_type &	operator = ( const value_type * p_matrix);
		my_square_type		operator + ( const my_square_type & p_matrix)const;
		my_square_type &	operator +=( const my_square_type & p_matrix);
		my_square_type		operator - ( const my_square_type & p_matrix)const;
		my_square_type &	operator -=( const my_square_type & p_matrix);
		my_square_type		operator * ( value_type p_scalar)const;
		my_square_type		operator * ( const my_square_type & p_matrix)const;
		col_type			operator * ( const col_type & p_ptVector)const;
		my_square_type &	operator *=( value_type p_scalar);
		my_square_type &	operator *=( const my_square_type & p_matrix);
		my_square_type &	operator /=( value_type p_scalar);
		my_square_type 		operator / ( value_type p_scalar)const;
		std::ostream & 		operator <<( std::ostream & l_streamOut)const;
		std::istream & 		operator >>( std::istream & l_streamIn);


		template <size_t _Columns>
		Matrix <value_type, Rows, _Columns> Multiply( const Matrix <value_type, Rows, _Columns> & p_matrix)const
		{
			Matrix <value_type, Rows, _Columns> l_mReturn;

			for (size_t i = 0 ; i < _Columns ; i++)
			{
				for (size_t j = 0 ; j < Rows ; j++)
				{
					l_mReturn[i] += m_matrix[j] * p_matrix[i][j];
				}
			}

			return l_mReturn;
		}
		template <typename Type>
		my_square_type &	operator = ( const SquareMatrix <Type, Rows> & p_matrix)
		{
			my_type::operator =( p_matrix);
			return * this;
		}
		template <typename Type>
		my_square_type &	operator = ( const Matrix <Type, Rows, Rows> & p_matrix)
		{
			my_type::operator =( p_matrix);
			return * this;
		}
		template <typename Type>
		my_square_type &	operator = ( const Type * p_matrix)
		{
			my_type::operator =( p_matrix);
			return * this;
		}
	};

	template <typename T, size_t Rows>
	SquareMatrix <T, Rows> operator * ( T p_tScalar, const SquareMatrix <T, Rows> & p_matrix);
	template <typename T, size_t Rows>
	SquareMatrix <T, Rows> operator - ( const SquareMatrix <T, Rows> & p_matrix);
	template <typename T, size_t Rows>
	std::ostream & operator << ( std::ostream & l_streamOut, const SquareMatrix <T, Rows> & p_matrix);
	template <typename T, size_t Rows>
	std::istream & operator >> ( std::istream & l_streamIn, SquareMatrix <T, Rows> & p_matrix);

	template <typename T, size_t Rows> struct CoFactorComputer;
}
}

#include "Matrix.inl"
#include "SquareMatrix.inl"

#endif