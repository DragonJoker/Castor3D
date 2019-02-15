/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_SQUARE_MATRIX_H___
#define ___CASTOR_SQUARE_MATRIX_H___

#include "Matrix.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1.0.0
	\date		09/02/2010
	\~english
	\brief		Templated column major square matrix representation
	\~french
	\brief		Représentation d'une matrice carrée column major
	*/
	template< typename T, uint32_t Count >
	class SquareMatrix
		: public Matrix< T, Count, Count >
	{
	private:
		typedef T __value_type;
		typedef SquareMatrix< __value_type, Count > __square_type;
		typedef __square_type __transpose;

	public:
		//!\~english	The data type
		//!\~french		Le type de données
		typedef typename Matrix< __value_type, Count, Count >::value_type value_type;
		//!\~english	The column type
		//!\~french		Le type de colonne
		typedef typename Matrix< __value_type, Count, Count >::col_type col_type;
		//!\~english	The line type
		//!\~french		Le type de ligne
		typedef typename Matrix< __value_type, Count, Count >::row_type row_type;
		//!\~english	The transposed matrix type
		//!\~french		Le type de matrice transposée
		typedef __transpose transpose_type;
		//!\~english	This matrix type
		//!\~french		Le type de cette matrice
		typedef __square_type square_matrix_type;
		//!\~english	This matrix type
		//!\~french		Le type de cette matrice
		typedef Matrix< T, Count, Count > my_matrix_type;
		//!\~english	This matrix type
		//!\~french		Le type de cette matrice
		typedef __square_type my_square_type;

	public:
		/**
		 *\~english
		 *name Construction / Destruction.
		 *\~french
		 *name Construction / Destruction.
		 **/
		/**@{*/
		SquareMatrix();
		explicit SquareMatrix( NoInit const & );
		explicit SquareMatrix( T const & rhs );
		SquareMatrix( SquareMatrix< T, Count > const & rhs );
		explicit SquareMatrix( SquareMatrix< T, Count > && rhs );
		template< uint32_t _Rows >
		explicit SquareMatrix( SquareMatrix< T, _Rows > const & rhs );
		template< typename Type >
		explicit SquareMatrix( SquareMatrix< Type, Count > const & rhs );
		template< typename Type >
		explicit SquareMatrix( Matrix< Type, Count, Count > const & rhs );
		template< typename Type >
		explicit SquareMatrix( Type const * rhs );
		/**@}*/
		/**
		 *\~english
		 *\brief		Retrieves a minor matrix, ignoring given line and column
		 *\param[in]	x, y	Line and column ignored
		 *\return		The minor matrix
		 *\~french
		 *\brief		Récupère une matrice mineure, en ignorant la ligne et la colonne données
		 *\param[in]	x, y	La ligne et la colonne ignorées
		 *\return		La matrice mineure
		 */
		SquareMatrix< T, Count - 1 > getMinor( uint32_t x, uint32_t y )const;
		/**
		 *\~english
		 *\brief		Computes the cofactor value of given row and column
		 *\param[in]	row, column	Row and column
		 *\return		The cofactor
		 *\~french
		 *\brief		Calcule le cofacteur à la ligne et la colonne données
		 *\param[in]	row, column	La ligne et la colonne
		 *\return		Le cofacteur
		 */
		__value_type getCofactor( uint32_t column, uint32_t row )const;
		/**
		 *\~english
		 *\brief		Computes the determinant of this matrix
		 *\return		The determinant
		 *\~french
		 *\brief		Calcule le déterminant de cette matrice
		 *\return		Le déterminant
		 */
		__value_type getDeterminant()const;
		/**
		 *\~english
		 *\brief		Computes and returns the transposed of this matrix
		 *\return		The transposed
		 *\~french
		 *\brief		Calcule et renvoie la transposée de cette matrice
		 *\return		La transposée
		 */
		transpose_type getTransposed()const;
		/**
		 *\~english
		 *\brief		Transposes this matrix
		 *\return		This matrix
		 *\~french
		 *\brief		Transpose cette matrice.
		 *\return		Cette matrice.
		 */
		my_square_type & transpose();
		/**
		 *\~english
		 *\brief		Tells if the matrix is orthogonal
		 *\return		\p true if it is orthogonal
		 *\~french
		 *\brief		Dit si la matrice est orthogonale
		 *\return		\p true si elle est orthogonale
		 */
		bool isOrthogonal()const;
		/**
		 *\~english
		 *\brief		Tells if this matrix is symmetrical
		 *\return		\p true if it is symmetrical
		 *\~french
		 *\brief		Dit si la matrice est symétrique
		 *\return		\p true si la matrice est symétrique
		 */
		bool isSymmetrical()const;
		/**
		 *\~english
		 *\brief		Tells if this matrix is anti symmetrical
		 *\return		\p true if it is anti symmetrical
		 *\~french
		 *\brief		Dit si la matrice est anti-symétrique
		 *\return		\p true si la matrice est anti-symétrique
		 */
		bool isAntiSymmetrical()const;
		/**
		 *\~english
		 *\brief		Retrieves the inverse of this matrix
		 *\return		The inverse
		 *\~french
		 *\brief		Récupère la matrice inverse de cette matrice
		 *\return		La matrice inverse
		 */
		SquareMatrix< T, Count > getInverse()const;
		/**
		 *\~english
		 *\brief		Inverts this matrix
		 *\return		A reference to this matrix
		 *\~french
		 *\brief		Inverse cette matrice
		 *\return		Une référence sur cette matrice
		 */
		SquareMatrix< T, Count > & invert();
		/**
		 *\~english
		 *name Assignment operators.
		 *\~french
		 *name Opérateurs d'affectation.
		**/
		/**@{*/
		template< typename Type >
		SquareMatrix< T, Count > & operator=( Matrix< Type, Count, Count > const & rhs );
		SquareMatrix< T, Count > & operator=( SquareMatrix< T, Count > const & rhs );
		SquareMatrix< T, Count > & operator=( SquareMatrix< T, Count > && rhs );
		template< typename Type >
		SquareMatrix< T, Count > & operator=( SquareMatrix< Type, Count > const & rhs );
		template< typename Type >
		SquareMatrix< T, Count > & operator=( Type const * rhs );
		/**@}*/
		/**
		 *\~english
		 *name Arithmetic operators.
		 *\~french
		 *name Opérateurs arithmétiques.
		**/
		/**@{*/
		template< typename Type >
		SquareMatrix< T, Count > & operator+=( SquareMatrix< Type, Count > const & rhs );
		template< typename Type >
		SquareMatrix< T, Count > & operator-=( SquareMatrix< Type, Count > const & rhs );
		template< typename Type >
		SquareMatrix< T, Count > & operator*=( SquareMatrix< Type, Count > const & rhs );
		template< typename Type >
		SquareMatrix< T, Count > & operator+=( Type const * rhs );
		template< typename Type >
		SquareMatrix< T, Count > & operator-=( Type const * rhs );
		template< typename Type >
		SquareMatrix< T, Count > & operator*=( Type const * rhs );
		SquareMatrix< T, Count > & operator+=( T const & rhs );
		SquareMatrix< T, Count > & operator-=( T const & rhs );
		SquareMatrix< T, Count > & operator*=( T const & rhs );
		SquareMatrix< T, Count > & operator/=( T const & rhs );
		/**@}*/
	};
	/**
	 *\~english
	 *name Logic operators.
	 *\~french
	 *name Opérateurs logiques
	**/
	/**@{*/
	template< typename T, uint32_t Count >
	bool operator==( SquareMatrix< T, Count > const & lhs, SquareMatrix< T, Count > const & rhs );
	template< typename T, uint32_t Count >
	bool operator!=( SquareMatrix< T, Count > const & lhs, SquareMatrix< T, Count > const & rhs );
	/**@}*/
	/**
	 *\~english
	 *name Arithmetic operators.
	 *\~french
	 *name Opérateurs arithmétiques.
	**/
	/**@{*/
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & lhs, SquareMatrix< U, Count > const & rhs );
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & lhs, SquareMatrix< U, Count > const & rhs );
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & lhs, SquareMatrix< U, Count > const & rhs );
	template< typename T, uint32_t Count, typename U, size_t Columns >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & lhs, Matrix< U, Columns, Count > const & rhs );
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & lhs, U const * rhs );
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & lhs, U const * rhs );
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & lhs, U const * rhs );
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & lhs, T const & rhs );
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & lhs, T const &	rhs );
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & lhs, T const & rhs );
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator/( SquareMatrix< T, Count > const & lhs, T const & rhs );
	template< typename T, uint32_t Count, typename U, size_t Rows >
	Matrix< T, Count, Rows > operator*( Matrix< U, Count, Rows > const & lhs, SquareMatrix< T, Count > const & rhs );
	template< typename T, uint32_t Count >
	SquareMatrix< T, Count > operator+( T const & lhs, SquareMatrix< T, Count > const & rhs );
	template< typename T, uint32_t Count >
	SquareMatrix< T, Count > operator-( T const & lhs, SquareMatrix< T, Count > const & rhs );
	template< typename T, uint32_t Count >
	SquareMatrix< T, Count > operator*( T const & lhs, SquareMatrix< T, Count > const & rhs );
	template< typename T, uint32_t Count >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & rhs );
	/**@}*/
	/**
	*\~english
	*name Stream operators.
	*\~french
	*name Opérateurs de flux.
	**/
	/**@{*/
	template< typename CharT, typename T, uint32_t Count > std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & streamOut, castor::SquareMatrix< T, Count > const & matrix );
	template< typename CharT, typename T, uint32_t Count > std::basic_istream< CharT > & operator>>( std::basic_istream< CharT > & streamIn, castor::SquareMatrix< T, Count > & matrix );
	/**@}*/
}

#include "SquareMatrix.inl"

#endif
