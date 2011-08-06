/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#include "CastorUtils.hpp"
#include "Assertion.hpp"
#include "Point.hpp"

namespace Castor
{	namespace Math
{
	//! Templated matrix representation
	/*!
	Can hold any type which has a defined Policy
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
		typedef Templates::Policy<value_type>		policy;

	private:
		bool m_bOwnCoords;

	public:
		typedef my_column col_type;
		typedef my_row row_type;
		typedef my_transpose transpose_type;

		col_type m_matrix[Columns];
		value_type * m_pPointer;

	private:

	public:
		/**@name Constructors */
		//@{
		Matrix();
		Matrix( Matrix<T, Columns, Rows> const & p_matrix);
		Matrix( T const & p_tValue);
		template <typename Type> Matrix( Type const * p_pMatrix);
		template <typename Type> Matrix( Matrix<Type, Columns, Rows> const & p_matrix);
		virtual ~Matrix();
		//@}

		/**@name Accessors */
		//@{
		void						SetRow		( size_t p_uiRow, value_type const * p_row);
		void						SetRow		( size_t p_uiRow, row_type const & p_row);
		row_type					GetRow		( size_t p_uiRow)const;
		void 						GetRow		( row_type & p_mResult, size_t p_uiRow)const;
		void 						SetColumn	( size_t p_uiColumn, value_type const * p_col);
		void 						SetColumn	( size_t p_uiColumn, col_type const & p_col);
		col_type					GetColumn	( size_t p_uiColumn)const;
		void						GetColumn	( col_type & p_mResult, size_t p_uiColumn)const;
		value_type					GetValue	( size_t p_uiRow, size_t p_uiColumn);
		col_type const &			operator []	( size_t i)const;
		col_type &					operator []	( size_t i);
		inline value_type *			ptr			();
		inline value_type const *	const_ptr	()const;
		void						LinkCoords	( void * p_pCoords);
		//@}
		/**@name Matrix functions */
		//@{
		void			Init			( value_type const & p_value=policy::zero());
		transpose_type	GetTransposed	()const;
		void			GetTransposed	( transpose_type & p_mResult)const;
		my_type			GetTriangle		()const;
		my_type			GetJordan		( value_type p_tLambda)const;
		my_type			GetIdentity		()const;
		my_type			GetMinor		( size_t x, size_t y, size_t p_uiRows, size_t p_uiCols)const;
		value_type		GetTrace		()const;
		void			SetTriangle		();
		void			SetJordan		( value_type p_tLambda);
		void			SetIdentity		();
		//@}
		/**@name Operators */
		//@{
									Matrix <T, Rows, Columns> &	operator = ( Matrix<T, Rows, Columns>		const	& p_matrix);
		template <typename Type>	Matrix <T, Rows, Columns> &	operator = ( Matrix<Type, Rows, Columns>	const	& p_matrix);
		template <typename Type>	Matrix <T, Rows, Columns> &	operator +=( Matrix<Type, Rows, Columns>	const	& p_matrix);
		template <typename Type>	Matrix <T, Rows, Columns> &	operator -=( Matrix<Type, Rows, Columns>	const	& p_matrix);
		template <typename Type>	Matrix <T, Rows, Columns> &	operator = ( Type							const	* p_pMatrix);
		template <typename Type>	Matrix <T, Rows, Columns> &	operator +=( Type							const	* p_pMatrix);
		template <typename Type>	Matrix <T, Rows, Columns> &	operator -=( Type							const	* p_pMatrix);
									Matrix <T, Rows, Columns> &	operator +=( T								const	& p_tValue);
									Matrix <T, Rows, Columns> &	operator -=( T								const	& p_tValue);
									Matrix <T, Rows, Columns> &	operator *=( T								const	& p_tValue);
									Matrix <T, Rows, Columns> &	operator /=( T								const	& p_tValue);
		//@}

	protected:
		void _recheck();
		template <typename Type> void _assign( Matrix<Type, Rows, Columns> const & p_matrix);
	};

	template <typename T, size_t Rows, size_t Columns								> bool 							operator ==( Matrix<T, Rows, Columns>	const	&	p_mtxA,			Matrix<T, Rows, Columns>		const	&	p_mtxB);
	template <typename T, size_t Rows, size_t Columns								> bool 							operator !=( Matrix<T, Rows, Columns>	const	&	p_mtxA,			Matrix<T, Rows, Columns>		const	&	p_mtxB);
	template <typename T, size_t Rows, size_t Columns								> std::ostream &				operator <<( std::ostream						&	l_streamOut,	Matrix<T, Rows, Columns>		const	&	p_matrix);
	template <typename T, size_t Rows, size_t Columns								> std::istream &				operator >>( std::istream						&	l_streamIn,		Matrix<T, Rows, Columns>				&	p_matrix);
	template <typename T, size_t Rows, size_t Columns, typename U					> Matrix <T, Rows, Columns>		operator + ( Matrix<T, Rows, Columns>	const	&	p_mtxA,			Matrix<U, Rows, Columns>		const	&	p_mtxB);
	template <typename T, size_t Rows, size_t Columns, typename U					> Matrix <T, Rows, Columns>		operator - ( Matrix<T, Rows, Columns>	const	&	p_mtxA,			Matrix<U, Rows, Columns>		const	&	p_mtxB);
	template <typename T, size_t Rows, size_t Columns, typename U, size_t _Columns	> Matrix <T, Rows, _Columns>	operator * ( Matrix<T, Rows, Columns>	const	&	p_mtxA,			Matrix<U, Columns, _Columns>	const	&	p_mtxB);
	template <typename T, size_t Rows, size_t Columns, typename U					> Point <T, Rows>				operator * ( Matrix<T, Rows, Columns>	const	&	p_matrix,		Point<U, Columns>				const	&	p_ptVector);
	template <typename T, size_t Rows, size_t Columns, typename U					> Matrix <T, Rows, Columns>		operator + ( Matrix<T, Rows, Columns>	const	&	p_mtxA,			U								const	*	p_matrix);
	template <typename T, size_t Rows, size_t Columns, typename U					> Matrix <T, Rows, Columns>		operator - ( Matrix<T, Rows, Columns>	const	&	p_mtxA,			U								const	*	p_matrix);
	template <typename T, size_t Rows, size_t Columns, typename U					> Matrix <T, Rows, Columns>		operator + ( Matrix<T, Rows, Columns>	const	&	p_matrix,		T								const	&	p_tValue);
	template <typename T, size_t Rows, size_t Columns, typename U					> Matrix <T, Rows, Columns>		operator - ( Matrix<T, Rows, Columns>	const	&	p_matrix,		T								const	&	p_tValue);
	template <typename T, size_t Rows, size_t Columns, typename U					> Matrix <T, Rows, Columns>		operator * ( Matrix<T, Rows, Columns>	const	&	p_matrix,		T								const	&	p_tValue);
	template <typename T, size_t Rows, size_t Columns, typename U					> Matrix <T, Rows, Columns>		operator / ( Matrix<T, Rows, Columns>	const	&	p_matrix,		T								const	&	p_tValue);
	template <typename T, size_t Rows, size_t Columns, typename U					> Matrix <T, Rows, Columns>		operator + ( T							const	&	p_tValue,		Matrix<U, Rows, Columns>		const	&	p_matrix);
	template <typename T, size_t Rows, size_t Columns, typename U					> Matrix <T, Rows, Columns>		operator - ( T							const	&	p_tValue,		Matrix<U, Rows, Columns>		const	&	p_matrix);
	template <typename T, size_t Rows, size_t Columns, typename U					> Matrix <T, Rows, Columns>		operator * ( T							const	&	p_tValue,		Matrix<U, Rows, Columns>		const	&	p_matrix);
	template <typename T, size_t Rows, size_t Columns								> Matrix <T, Rows, Columns>		operator - ( Matrix<T, Rows, Columns>	const	&	p_matrix);

	//! Templated square matrix representation
	/*!
	Can hold any type which has a defined Policy
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <typename T, size_t Rows>
	class SquareMatrix : public Matrix<T, Rows, Rows>
	{
	protected:
		typedef T									value_type;
		typedef SquareMatrix<value_type, Rows>		my_square_type;
		typedef SquareMatrix<value_type, Rows>		my_transpose;
		typedef Point<value_type, Rows>				my_row;
		typedef Point<value_type, Rows>				my_column;
		typedef Templates::Policy<value_type>		policy;

	public:
		typedef my_column 				col_type;
		typedef my_row 					row_type;
		typedef my_transpose 			transpose_type;
		typedef my_square_type			square_matrix_type;
		typedef Matrix<T, Rows, Rows>	matrix_type;

	public:
		/**@name Constructors */
		//@{
		SquareMatrix();
		SquareMatrix( T const & p_tValue);
		SquareMatrix( SquareMatrix<T, Rows> const & p_matrix);
		template <typename Type> SquareMatrix( SquareMatrix<Type, Rows> const & p_matrix);
		template <typename Type> SquareMatrix( Matrix<Type, Rows, Rows> const & p_matrix);
		template <typename Type> SquareMatrix( Type const * p_pMatrix);
		virtual ~SquareMatrix();
		//@}
		/**@name Square Matrix operations */
		//@{
		SquareMatrix<T, Rows-1>											GetMinor			( size_t x, size_t y)const;
		value_type														GetCofactor			( size_t p_uiRow, size_t p_uiColumn)const;
		value_type														GetDeterminant		()const;
		bool															IsOrthogonal		()const;
		void															Transpose			();
		bool															IsSymmetrical		()const;
		bool															IsAntiSymmetrical	()const;
		my_square_type													GetInverse			()const;
		virtual void													Invert				();
		my_square_type													Multiply			( my_square_type const & p_matrix)const;
		template <typename Type> my_square_type							Multiply			( SquareMatrix<Type, Rows> const & p_matrix)const;
		template <size_t _Columns> Matrix <value_type, Rows, _Columns>	Multiply			( Matrix <value_type, Rows, _Columns> const & p_matrix)const;
		//@}
		/**@name Operators */
		//@{
		template <typename Type>	SquareMatrix <T, Rows> &	operator = ( Matrix <Type, Rows, Rows>	const	&	p_matrix);
									SquareMatrix <T, Rows> &	operator = ( SquareMatrix<T, Rows>		const	&	p_matrix);
		template <typename Type>	SquareMatrix <T, Rows> &	operator = ( SquareMatrix<Type, Rows>	const	&	p_matrix);
		template <typename Type>	SquareMatrix <T, Rows> &	operator +=( SquareMatrix<Type, Rows>	const	&	p_matrix);
		template <typename Type>	SquareMatrix <T, Rows> &	operator -=( SquareMatrix<Type, Rows>	const	&	p_matrix);
		template <typename Type>	SquareMatrix <T, Rows> &	operator *=( SquareMatrix<Type, Rows>	const	&	p_matrix);
		template <typename Type>	SquareMatrix <T, Rows> &	operator = ( Type						const	*	p_pMatrix);
		template <typename Type>	SquareMatrix <T, Rows> &	operator +=( Type						const	*	p_pMatrix);
		template <typename Type>	SquareMatrix <T, Rows> &	operator -=( Type						const	*	p_pMatrix);
		template <typename Type>	SquareMatrix <T, Rows> &	operator *=( Type						const	*	p_pMatrix);
									SquareMatrix <T, Rows> &	operator +=( T							const	&	p_tValue);
									SquareMatrix <T, Rows> &	operator -=( T							const	&	p_tValue);
									SquareMatrix <T, Rows> &	operator *=( T							const	&	p_tValue);
									SquareMatrix <T, Rows> &	operator /=( T							const	&	p_tValue);
		//@}
	};

	template <typename T, size_t Rows				> std::ostream				&	operator <<( std::ostream					&	l_streamOut,	SquareMatrix <T, Rows>	const	&	p_matrix);
	template <typename T, size_t Rows				> std::istream				&	operator >>( std::istream					&	l_streamIn,		SquareMatrix <T, Rows>			&	p_matrix);
	template <typename T, size_t Rows				> bool 							operator ==( SquareMatrix<T, Rows>	const	&	p_mtxA,			SquareMatrix <T, Rows>	const	&	p_mtxB);
	template <typename T, size_t Rows				> bool 							operator !=( SquareMatrix<T, Rows>	const	&	p_mtxA,			SquareMatrix <T, Rows>	const	&	p_mtxB);
	template <typename T, size_t Rows, typename U	> SquareMatrix <T, Rows>		operator + ( SquareMatrix <T, Rows>	const	&	p_mtxA,			SquareMatrix <U, Rows>	const	&	p_mtxB);
	template <typename T, size_t Rows, typename U	> SquareMatrix <T, Rows>		operator - ( SquareMatrix <T, Rows>	const	&	p_mtxA,			SquareMatrix <U, Rows>	const	&	p_mtxB);
	template <typename T, size_t Rows, typename U	> SquareMatrix <T, Rows>		operator * ( SquareMatrix <T, Rows>	const	&	p_mtxA,			SquareMatrix <U, Rows>	const	&	p_mtxB);
	template <typename T, size_t Rows, typename U	> SquareMatrix <T, Rows>		operator + ( SquareMatrix <T, Rows>	const	&	p_matrix,		U						const	*	p_pMatrix);
	template <typename T, size_t Rows, typename U	> SquareMatrix <T, Rows>		operator - ( SquareMatrix <T, Rows>	const	&	p_matrix,		U						const	*	p_pMatrix);
	template <typename T, size_t Rows, typename U	> SquareMatrix <T, Rows>		operator * ( SquareMatrix <T, Rows>	const	&	p_matrix,		U						const	*	p_pMatrix);
	template <typename T, size_t Rows, typename U	> SquareMatrix <T, Rows>		operator + ( SquareMatrix <T, Rows>	const	&	p_matrix,		T						const	&	p_tValue);
	template <typename T, size_t Rows, typename U	> SquareMatrix <T, Rows>		operator - ( SquareMatrix <T, Rows>	const	&	p_matrix,		T						const	&	p_tValue);
	template <typename T, size_t Rows, typename U	> SquareMatrix <T, Rows>		operator * ( SquareMatrix <T, Rows>	const	&	p_matrix,		T						const	&	p_tValue);
	template <typename T, size_t Rows, typename U	> SquareMatrix <T, Rows> 		operator / ( SquareMatrix <T, Rows>	const	&	p_matrix,		T						const	&	p_tValue);
	template <typename T, size_t Rows, typename U	> Point<T, Rows>				operator * ( SquareMatrix <T, Rows>	const	&	p_matrix,		Point<U, Rows>			const	&	p_ptVector);
	template <typename T, size_t Rows				> SquareMatrix <T, Rows>		operator + ( T						const	&	p_tValue,		SquareMatrix <T, Rows>	const	&	p_matrix);
	template <typename T, size_t Rows				> SquareMatrix <T, Rows>		operator - ( T						const	&	p_tValue,		SquareMatrix <T, Rows>	const	&	p_matrix);
	template <typename T, size_t Rows				> SquareMatrix <T, Rows>		operator * ( T						const	&	p_tValue,		SquareMatrix <T, Rows>	const	&	p_matrix);
	template <typename T, size_t Rows				> SquareMatrix <T, Rows>		operator - ( SquareMatrix <T, Rows>	const	&	p_matrix);

	template <typename T, size_t Rows> struct CoFactorComputer;
}
}

#include "Matrix.inl"
#include "SquareMatrix.inl"

#endif
