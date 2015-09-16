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
#ifndef ___CASTOR_SQUARE_MATRIX_H___
#define ___CASTOR_SQUARE_MATRIX_H___

#include "Matrix.hpp"

namespace Castor
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
		typedef SquareMatrix< __value_type, Count > __transpose;

	public:
		//!\~english Typedef on the policy	\~french Typedef sur la politique
		typedef typename Matrix< __value_type, Count, Count >::policy policy;
		//!\~english Typedef on the data type	\~french Typedef sur le type de données
		typedef typename Matrix< __value_type, Count, Count >::value_type value_type;
		//!\~english Typedef on the column type	\~french Typedef sur le type de colonne
		typedef typename Matrix< __value_type, Count, Count >::col_type col_type;
		//!\~english Typedef on the line type	\~french Typedef sur le type de ligne
		typedef typename Matrix< __value_type, Count, Count >::row_type row_type;
		//!\~english Typedef on the transposed matrix type	\~french Typedef sur le type de matrice transposée
		typedef __square_type transpose_type;
		//!\~english Typedef on this matrix type	\~french Typedef sur le type de cette matrice
		typedef __square_type square_matrix_type;
		//!\~english Typedef on this matrix type	\~french Typedef sur le type de cette matrice
		typedef Matrix< T, Count, Count > my_matrix_type;
		//!\~english Typedef on this matrix type	\~french Typedef sur le type de cette matrice
		typedef __square_type my_square_type;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		SquareMatrix();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_value	The matrix is initialised as Jordan with that value
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_value	La matrice est initialisée comme une Jordan avec cette valeur
		 */
		SquareMatrix( T const & p_value );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_matrix	The Matrix object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_matrix	L'objet Matrix à copier
		 */
		SquareMatrix( SquareMatrix< T, Count > const & p_matrix );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_matrix	The Matrix object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_matrix	L'objet Matrix à déplacer
		 */
		SquareMatrix( SquareMatrix< T, Count > && p_matrix );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_matrix	The Matrix object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_matrix	L'objet Matrix à déplacer
		 */
		template< uint32_t _Rows >
		SquareMatrix( SquareMatrix< T, _Rows > const & p_matrix );
		/**
		 *\~english
		 *\brief		Conversion Copy Constructor
		 *\param[in]	p_matrix	The Matrix object to copy
		 *\~french
		 *\brief		Constructeur par copie convertie
		 *\param[in]	p_matrix	L'objet Matrix à copier
		 */
		template< typename Type > SquareMatrix( SquareMatrix< Type, Count > const & p_matrix );
		/**
		 *\~english
		 *\brief		Conversion Copy Constructor
		 *\param[in]	p_matrix	The Matrix object to copy
		 *\~french
		 *\brief		Constructeur par copie convertie
		 *\param[in]	p_matrix	L'objet Matrix à copier
		 */
		template< typename Type > SquareMatrix( Matrix< Type, Count, Count > const & p_matrix );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_matrix	Data buffer to copy in the matrix
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_matrix	Buffer de données à copier dans la matrice
		 */
		template< typename Type > SquareMatrix( Type const * p_matrix );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~SquareMatrix();
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
		SquareMatrix < T, Count - 1 > get_minor( uint32_t x, uint32_t y )const;
		/**
		 *\~english
		 *\brief		Computes the cofactor value of given row and column
		 *\param[in]	p_row, p_column	Row and column
		 *\return		The cofactor
		 *\~french
		 *\brief		Calcule le cofacteur à la ligne et la colonne données
		 *\param[in]	p_row, p_column	La ligne et la colonne
		 *\return		Le cofacteur
		 */
		__value_type get_cofactor( uint32_t p_column, uint32_t p_row )const;
		/**
		 *\~english
		 *\brief		Computes the determinant of this matrix
		 *\return		The determinant
		 *\~french
		 *\brief		Calcule le déterminant de cette matrice
		 *\return		Le déterminant
		 */
		__value_type get_determinant()const;
		/**
		 *\~english
		 *\brief		Tells if the matrix is orthogonal
		 *\return		\p true if it is orthogonal
		 *\~french
		 *\brief		Dit si la matrice est orthogonale
		 *\return		\p true si elle est orthogonale
		 */
		bool is_orthogonal()const;
		/**
		 *\~english
		 *\brief		Transposes this matrix
		 *\return		A reference to this matrix
		 *\~french
		 *\brief		Transpose cette matrice
		 *\return		Une référence sur cette matrice
		 */
		SquareMatrix< T, Count > & transpose();
		/**
		 *\~english
		 *\brief		Tells if this matrix is symmetrical
		 *\return		\p true if it is symmetrical
		 *\~french
		 *\brief		Dit si la matrice est symétrique
		 *\return		\p true si la matrice est symétrique
		 */
		bool is_symmetrical()const;
		/**
		 *\~english
		 *\brief		Tells if this matrix is anti symmetrical
		 *\return		\p true if it is anti symmetrical
		 *\~french
		 *\brief		Dit si la matrice est anti-symétrique
		 *\return		\p true si la matrice est anti-symétrique
		 */
		bool is_anti_symmetrical()const;
		/**
		 *\~english
		 *\brief		Retrieves the inverse of this matrix
		 *\return		The inverse
		 *\~french
		 *\brief		Récupère la matrice inverse de cette matrice
		 *\return		La matrice inverse
		 */
		SquareMatrix< T, Count > get_inverse()const;
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
		 *\brief		Conversion Copy assignment operator
		 *\param[in]	p_matrix	The Matrix object to copy
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par copie convertie
		 *\param[in]	p_matrix	L'objet Matrix à copier
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		template< typename Type > SquareMatrix< T, Count > & operator=( Matrix< Type, Count, Count > const & p_matrix );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_matrix	The SquareMatrix object to copy
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_matrix	L'objet SquareMatrix à copier
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		SquareMatrix< T, Count > & operator=( SquareMatrix< T, Count > const & p_matrix );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_matrix	The SquareMatrix object to move
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_matrix	L'objet SquareMatrix à déplacer
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		SquareMatrix< T, Count > & operator=( SquareMatrix< T, Count > && p_matrix );
		/**
		 *\~english
		 *\brief		Conversion Copy assignment operator
		 *\param[in]	p_matrix	The SquareMatrix object to copy
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par copie convertie
		 *\param[in]	p_matrix	L'objet SquareMatrix à copier
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		template< typename Type > SquareMatrix< T, Count > & operator=( SquareMatrix< Type, Count > const & p_matrix );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_matrix	The SquareMatrix object to add
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_matrix	L'objet SquareMatrix à ajouter
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		template< typename Type > SquareMatrix< T, Count > & operator+=( SquareMatrix< Type, Count > const & p_matrix );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_matrix	The SquareMatrix object to substract
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_matrix	L'objet SquareMatrix à soustraire
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		template< typename Type > SquareMatrix< T, Count > & operator-=( SquareMatrix< Type, Count > const & p_matrix );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_matrix	The SquareMatrix object to multiply
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_matrix	L'objet SquareMatrix à multiplier
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		template< typename Type > SquareMatrix< T, Count > & operator*=( SquareMatrix< Type, Count > const & p_matrix );
		/**
		 *\~english
		 *\brief		Conversion Copy assignment operator
		 *\param[in]	p_matrix	The data buffer to copy
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par copie convertie
		 *\param[in]	p_matrix	Le buffer de données à copier
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		template< typename Type > SquareMatrix< T, Count > & operator=( Type const * p_matrix );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_matrix	The data buffer to add
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_matrix	Le buffer de données à ajouter
		 *\return		Une référence sur cet SquareMatrix Point
		 */
		template< typename Type > SquareMatrix< T, Count > & operator+=( Type const * p_matrix );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_matrix	The data buffer to substract
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_matrix	Le buffer de données à soustraire
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		template< typename Type > SquareMatrix< T, Count > & operator-=( Type const * p_matrix );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_matrix	The data buffer to multiply
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_matrix	Le buffer de données à multiplier
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		template< typename Type > SquareMatrix< T, Count > & operator*=( Type const * p_matrix );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_value	The value to add
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_value	La valeur à additionner
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		SquareMatrix< T, Count > & operator+=( T const & p_value );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_value	The value to substract
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_value	La valeur à soustraire
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		SquareMatrix< T, Count > & operator-=( T const & p_value );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_value	The value to multiply
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_value	La valeur à multiplier
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		SquareMatrix< T, Count > & operator*=( T const & p_value );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_value	The value to divide
		 *\return		A reference to this SquareMatrix object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_value	La valeur à diviser
		 *\return		Une référence sur cet objet SquareMatrix
		 */
		SquareMatrix< T, Count > & operator/=( T const & p_value );
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to compare
	 *\return		\p true if matrices have same dimensions and same values
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à comparer
	 *\return		\p true si les matrices ont les mêmes dimensions et les mêmes valeurs
	 */
	template< typename T, uint32_t Count > bool operator==( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< T, Count > const & p_mtxB );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to compare
	 *\return		\p true if matrices have different dimensions or at least one different value
	 *\~french
	 *\brief		Opérateur d'inégalité
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à comparer
	 *\return		\p true si les matrices ont des dimensions différentes ou au moins une valeur différente
	 */
	template< typename T, uint32_t Count > bool operator!=( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< T, Count > const & p_mtxB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U > SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< U, Count > const & p_mtxB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U > SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< U, Count > const & p_mtxB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U > SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< U, Count > const & p_mtxB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, size_t Columns > SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & p_mtxA, Matrix< U, Columns, Count > const & p_mtxB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U > SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & p_mtxA, U const * p_mtxB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U > SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & p_mtxA, U const * p_mtxB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U > SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & p_mtxA, U const * p_mtxB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_mtxA		The matrix
	 *\param[in]	p_value	The value to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur de addition
	 *\param[in]	p_mtxA		La matrice
	 *\param[in]	p_value	La valeur à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U > SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & p_mtxA, T const & p_value );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_mtxA		The matrix
	 *\param[in]	p_value	The value to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_mtxA		La matrice
	 *\param[in]	p_value	La valeur à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U > SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & p_mtxA, T const &	p_value );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_mtxA		The matrix to multiply
	 *\param[in]	p_value	The value
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_mtxA		La matrice à multiplier
	 *\param[in]	p_value	La valeur
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U > SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & p_mtxA, T const & p_value );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_mtxA		The matrix to divide
	 *\param[in]	p_value	The value
	 *\return		The division result
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_mtxA		La matrice à diviser
	 *\param[in]	p_value	La valeur
	 *\return		Le résultat de la division
	 */
	template< typename T, uint32_t Count, typename U > SquareMatrix< T, Count > operator/( SquareMatrix< T, Count > const & p_mtxA, T const & p_value );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, size_t Rows > Matrix< T, Count, Rows > operator*( Matrix< U, Count, Rows > const & p_mtxA, SquareMatrix< T, Count > const & p_mtxB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_matrix	The matrix
	 *\param[in]	p_value	The value to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur de addition
	 *\param[in]	p_matrix	La matrice
	 *\param[in]	p_value	La valeur à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Count > SquareMatrix< T, Count > operator+( T const & p_value, SquareMatrix< T, Count > const & p_matrix );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_matrix	The matrix
	 *\param[in]	p_value	The value to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_matrix	La matrice
	 *\param[in]	p_value	La valeur à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Count > SquareMatrix< T, Count > operator-( T const & p_value, SquareMatrix< T, Count > const & p_matrix );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_matrix	The matrix to multiply
	 *\param[in]	p_value	The value
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_matrix	La matrice à multiplier
	 *\param[in]	p_value	La valeur
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Count > SquareMatrix< T, Count > operator*( T const & p_value, SquareMatrix< T, Count > const & p_matrix );
	/**
	 *\~english
	 *\brief		Negation operator
	 *\param[in]	p_matrix	The matrix
	 *\return		The negation result
	 *\~french
	 *\brief		Opérateur de négation
	 *\param[in]	p_matrix	La matrice
	 *\return		Le résultat de la négation
	 */
	template< typename T, uint32_t Count > SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & p_matrix );
}

#include "SquareMatrix.inl"

#endif
