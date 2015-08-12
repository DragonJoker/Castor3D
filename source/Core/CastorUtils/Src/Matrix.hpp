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
#ifndef ___CASTOR_MATRIX_H___
#define ___CASTOR_MATRIX_H___

#include "CastorUtils.hpp"
#include "Assertion.hpp"
#include "Point.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1.0.0
	\date		09/02/2010
	\~english
	\brief		Templated column major matrix representation
	\remark		Can hold any type which has a defined Policy
	\~french
	\brief		Représentation d'une matrice column major, le type des éléments et les dimensions de la matrice sont en template
	\remark		Peut contenir n'importe quel élément qui a une Castor::Policy
	*/
	template< typename T, uint32_t Rows, uint32_t Columns >
	class Matrix
	{
	protected:
		typedef T __value_type;
		typedef Matrix<__value_type, Rows, Columns> __type;
		typedef Matrix<__value_type, Columns, Rows> __transpose;
		typedef Point<__value_type, Columns> __row;
		typedef Coords<__value_type, Rows> __column;
		typedef Castor::Policy<__value_type> __policy;
		static const std::size_t size = sizeof( T ) * Rows * Columns;

	public:
		//!\~english Typedef on the data type	\~french Typedef sur le type de données
		typedef __value_type value_type;
		//!\~english Typedef on the column type	\~french Typedef sur le type de colonne
		typedef __column col_type;
		//!\~english Typedef on the line type	\~french Typedef sur le type de ligne
		typedef __row row_type;
		//!\~english Typedef on the transposed matrix type	\~french Typedef sur le type de matrice transposée
		typedef __transpose transpose_type;
		//!\~english Typedef on the policy	\~french Typedef sur la politique
		typedef __policy policy;
		//!\~english Typedef on this matrix type	\~french Typedef sur le type de cette matrice
		typedef Matrix< value_type, Rows, Columns > my_type;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Matrix();
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_matrix	The Matrix object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_matrix	L'objet Matrix à copier
		 */
		Matrix( Matrix< T, Rows, Columns > const & p_matrix );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_matrix	The Matrix object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_matrix	L'objet Matrix à déplacer
		 */
		Matrix( Matrix< T, Rows, Columns > && p_matrix );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_tValue	The matrix is initialised as Jordan with that value
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_tValue	La matrice est initialisée comme une Jordan avec cette valeur
		 */
		Matrix( T const & p_tValue );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pMatrix	Data buffer to copy in the matrix
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pMatrix	Buffer de données à copier dans la matrice
		 */
		template< typename Type > Matrix( Type const * p_pMatrix );
		/**
		 *\~english
		 *\brief		Conversion Copy Constructor
		 *\param[in]	p_matrix	The Matrix object to copy
		 *\~french
		 *\brief		Constructeur par copie convertie
		 *\param[in]	p_matrix	L'objet Matrix à copier
		 */
		template< typename Type > Matrix( Matrix< Type, Rows, Columns > const & p_matrix );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Matrix();
		/**
		 *\~english
		 *\brief		Sets the values for the given row
		 *\param[in]	p_uiRow	The row to affect
		 *\param[in]	p_row	The values
		 *\~french
		 *\brief		Définit les valeurs d'une ligne donnée
		 *\param[in]	p_uiRow	La ligne à affecter
		 *\param[in]	p_row	Les valeurs
		 */
		void set_row( uint32_t p_uiRow, value_type const * p_row );
		/**
		 *\~english
		 *\brief		Sets the values for the given row
		 *\param[in]	p_uiRow	The row to affect
		 *\param[in]	p_row	The values
		 *\~french
		 *\brief		Définit les valeurs d'une ligne donnée
		 *\param[in]	p_uiRow	La ligne à affecter
		 *\param[in]	p_row	Les valeurs
		 */
		void set_row( uint32_t p_uiRow, Point< value_type, Columns > const & p_row );
		/**
		 *\~english
		 *\brief		Retrieves a line
		 *\param[in]	p_uiRow	The line index
		 *\return		The line
		 *\~french
		 *\brief		Récupère une ligne
		 *\param[in]	p_uiRow	L'index de la ligne
		 *\return		La ligne
		 */
		row_type get_row( uint32_t p_uiRow )const;
		/**
		 *\~english
		 *\brief		Retrieves a line
		 *\param[in]	p_uiRow		The line index
		 *\param[out]	p_mResult	Receives the line values
		 *\~french
		 *\brief		Récupère une ligne
		 *\param[in]	p_uiRow		L'index de la ligne
		 *\param[out]	p_mResult	Reçoit les valeurs de la ligne
		 */
		void get_row( uint32_t p_uiRow, row_type & p_mResult )const;
		/**
		 *\~english
		 *\brief		Sets the values for the given column
		 *\param[in]	p_uiColumn	The column to affect
		 *\param[in]	p_col		The values
		 *\~french
		 *\brief		Définit les valeurs d'une colonne donnée
		 *\param[in]	p_uiColumn	La colonne à affecter
		 *\param[in]	p_col		Les valeurs
		 */
		void set_column( uint32_t p_uiColumn, value_type const * p_col );
		/**
		 *\~english
		 *\brief		Sets the values for the given column
		 *\param[in]	p_uiColumn	The column to affect
		 *\param[in]	p_col		The values
		 *\~french
		 *\brief		Définit les valeurs d'une colonne donnée
		 *\param[in]	p_uiColumn	La colonne à affecter
		 *\param[in]	p_col		Les valeurs
		 */
		void set_column( uint32_t p_uiColumn, Point< value_type, Rows > const & p_col );
		/**
		 *\~english
		 *\brief		Retrieves a column
		 *\param[in]	p_uiColumn	The column index
		 *\return		The column
		 *\~french
		 *\brief		Récupère une colonne
		 *\param[in]	p_uiColumn	L'index de la colonne
		 *\return		La colonne
		 */
		Point< value_type, Rows > get_column( uint32_t p_uiColumn )const;
		/**
		 *\~english
		 *\brief		Retrieves a column
		 *\param[in]	p_uiColumn	The column index
		 *\param[out]	p_mResult	Receives the column values
		 *\~french
		 *\brief		Récupère une colonne
		 *\param[in]	p_uiColumn	L'index de la colonne
		 *\param[out]	p_mResult	Reçoit les valeurs de la colonne
		 */
		void get_column( uint32_t p_uiColumn, Point< value_type, Rows > & p_mResult )const;
		/**
		 *\~english
		 *\brief		Retrieves a column
		 *\param[in]	p_uiColumn	The column index
		 *\return		The column
		 *\~french
		 *\brief		Récupère une colonne
		 *\param[in]	p_uiColumn	L'index de la colonne
		 *\return		La colonne
		 */
		col_type get_column( uint32_t p_uiColumn );
		/**
		 *\~english
		 *\brief		Retrieves the value at the given position
		 *\param[in]	p_uiRow, p_uiColumn	The position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur à la position donnée
		 *\param[in]	p_uiRow, p_uiColumn	La position
		 *\return		La valeur
		 */
		value_type value_at( uint32_t p_uiRow, uint32_t p_uiColumn );
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\param[in]	i	The index
		 *\return		A constant pointer on the column's first element
		 *\~french
		 *\brief		Récupère la colonne à l'index donné
		 *\param[in]	i	L'index
		 *\return		Un pointeur constant sur le premier élément de la colonne
		 */
		value_type const * operator []( uint32_t i )const;
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\param[in]	i	The index
		 *\return		A pointer on the column's first element
		 *\~french
		 *\brief		Récupère la colonne à l'index donné
		 *\param[in]	i	L'index
		 *\return		Un pointeur sur le premier élément de la colonne
		 */
		value_type * operator []( uint32_t i );
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Les données
		 */
		inline value_type * ptr();
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données constantes
		 *\return		Les données
		 */
		inline value_type const * const_ptr()const;
		/**
		 *\~english
		 *\brief		Links the data pointer to the one given in parameter
		 *\remark		The matrix loses ownership of it's data
		 *\~french
		 *\brief		Lie les données de cette matrice à celles données en paramètre
		 *\remark		La matrice perd la maîtrise de ses données
		 */
		void link( T * p_pCoords );
		/**
		 *\~english
		 *\brief		Initialises the matrix as a Jordan one, with the given value
		 *\param[in]	p_value	The value
		 *\~french
		 *\brief		Initialise la matrice comme une matrice de Jordan, avec la valeur donnée
		 *\param[in]	p_value	La valeur
		 */
		void initialise( value_type const & p_value = policy::zero() );
		/**
		 *\~english
		 *\brief		Computes and returns the transposed of this matrix
		 *\return		The transposed
		 *\~french
		 *\brief		Calcule et renvoie la transposée de cette matrice
		 *\return		La transposée
		 */
		transpose_type get_transposed()const;
		/**
		 *\~english
		 *\brief		Computes the transposed of this matrix
		 *\param[in]	p_mResult	Receives the transposed
		 *\~french
		 *\brief		Calcule la transposée de cette matrice
		 *\param[in]	p_mResult	Reçoit la transposée
		 */
		void get_transposed( transpose_type & p_mResult )const;
		/**
		 *\~english
		 *\brief		Computes and returns the triangular of this matrix
		 *\return		The triangular
		 *\~french
		 *\brief		Calcule et renvoie la triangulaire de cette matrice
		 *\return		La triangulaire
		 */
		my_type get_triangle()const;
		/**
		 *\~english
		 *\brief		Computes and returns a Jordan matrix
		 *\param[in]	p_tLambda	Ther Jordan value
		 *\return		The Jordan matrix
		 *\~french
		 *\brief		Calcule et renvoie une matrice de Jordan
		 *\param[in]	p_tLambda	La valeur de Jordan
		 *\return		La matrice de Jordan
		 */
		my_type get_jordan( value_type p_tLambda )const;
		/**
		 *\~english
		 *\brief		Computes and returns an identity matrix
		 *\return		The identity matrix
		 *\~french
		 *\brief		Calcule et renvoie une matrice identité
		 *\return		La matrice identité
		 */
		my_type get_identity()const;
		/**
		 *\~english
		 *\brief		Retrieves thr trace or this matrix
		 *\return		The trace
		 *\~french
		 *\brief		Récupère la trace de cette matrice
		 *\return		La trace
		 */
		value_type get_trace()const;
		/**
		 *\~english
		 *\brief		Sets this matrix to its triangular
		 *\~french
		 *\brief		Définit cette matrice comme sa triangulaire
		 */
		void set_triangle();
		/**
		 *\~english
		 *\brief		Sets this matrix to a Jordan one
		 *\param[in]	p_tLambda	The Jordan value
		 *\~french
		 *\brief		Définit cette matrice à une matrice de Jordan
		 *\param[in]	p_tLambda	La valeur de Jordan
		 */
		void set_jordan( value_type p_tLambda );
		/**
		 *\~english
		 *\brief		Sets this matrix to identity
		 *\~french
		 *\brief		Définit cette matrice à l'identité
		 */
		void set_identity();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_matrix	The Matrix object to copy
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_matrix	L'objet Matrix à copier
		 *\return		Une référence sur cet objet Matrix
		 */
		Matrix< T, Rows, Columns > & operator =( Matrix< T, Rows, Columns > const & p_matrix );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_matrix	The Matrix object to move
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_matrix	L'objet Matrix à déplacer
		 *\return		Une référence sur cet objet Matrix
		 */
		Matrix< T, Rows, Columns > & operator =( Matrix< T, Rows, Columns > && p_matrix );
		/**
		 *\~english
		 *\brief		Conversion Copy assignment operator
		 *\param[in]	p_matrix	The Matrix object to copy
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation par copie convertie
		 *\param[in]	p_matrix	L'objet Matrix à copier
		 *\return		Une référence sur cet objet Matrix
		 */
		template< typename Type > Matrix< T, Rows, Columns > & operator =( Matrix< Type, Rows, Columns > const & p_matrix );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_matrix	The Matrix object to add
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_matrix	L'objet Matrix à ajouter
		 *\return		Une référence sur cet objet Matrix
		 */
		template< typename Type > Matrix< T, Rows, Columns > & operator +=( Matrix< Type, Rows, Columns > const & p_matrix );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_matrix	The Matrix object to substract
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_matrix	L'objet Matrix à soustraire
		 *\return		Une référence sur cet objet Matrix
		 */
		template< typename Type > Matrix< T, Rows, Columns > & operator -=( Matrix< Type, Rows, Columns > const & p_matrix );
		/**
		 *\~english
		 *\brief		Conversion Copy assignment operator
		 *\param[in]	p_pMatrix	The data pointer to copy
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation par copie convertie
		 *\param[in]	p_pMatrix	Le pointeur sur les données à copier
		 *\return		Une référence sur cet objet Matrix
		 */
		template< typename Type > Matrix< T, Rows, Columns > & operator =( Type const * p_pMatrix );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_pMatrix	The data pointer to add
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_pMatrix	Le pointeur sur les données à ajouter
		 *\return		Une référence sur cet objet Matrix
		 */
		template< typename Type > Matrix< T, Rows, Columns > & operator +=( Type const * p_pMatrix );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pMatrix	The data pointer to substract
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_pMatrix	Le pointeur sur les données à soustraire
		 *\return		Une référence sur cet objet Matrix
		 */
		template< typename Type > Matrix< T, Rows, Columns > & operator -=( Type const * p_pMatrix );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_tValue	The value to add
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_tValue	La valeur à additionner
		 *\return		Une référence sur cet objet Matrix
		 */
		Matrix< T, Rows, Columns > & operator +=( T const & p_tValue );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_tValue	The value to substract
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_tValue	La valeur à soustraire
		 *\return		Une référence sur cet objet Matrix
		 */
		Matrix< T, Rows, Columns > & operator -=( T const & p_tValue );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_tValue	The value to multiply
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_tValue	La valeur à multiplier
		 *\return		Une référence sur cet objet Matrix
		 */
		Matrix< T, Rows, Columns > & operator *=( T const & p_tValue );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_tValue	The value to divide
		 *\return		A reference to this Matrix object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_tValue	La valeur à diviser
		 *\return		Une référence sur cet objet Matrix
		 */
		Matrix< T, Rows, Columns > & operator /=( T const & p_tValue );
		/**
		 *\~english
		 *\brief		Indexing operator
		 *\param[in]	p_row	The row index
		 *\param[in]	p_col	The column index
		 *\return		A reference to the indexed value
		 *\~french
		 *\brief		Opérateur d'indexation
		 *\param[in]	p_row	L'index de la ligne
		 *\param[in]	p_col	L'index de la colonne
		 *\return		Une référence sur la valeur indexée
		 */
		T & operator()( uint32_t p_row, uint32_t p_col );
		/**
		 *\~english
		 *\brief		Indexing operator
		 *\param[in]	p_row	The row index
		 *\param[in]	p_col	The column index
		 *\return		A reference to the indexed value
		 *\~french
		 *\brief		Opérateur d'indexation
		 *\param[in]	p_row	L'index de la ligne
		 *\param[in]	p_col	L'index de la colonne
		 *\return		Une référence sur la valeur indexée
		 */
		T const & operator()( uint32_t p_row, uint32_t p_col )const;

	protected:
		my_type rec_get_minor( uint32_t x, uint32_t y, uint32_t p_uiRows, uint32_t p_uiCols )const;
		
#if !defined( NDEBUG )
		void do_update_debug()const;
#endif

	protected:
		bool m_bOwnCoords;
		value_type * m_pPointer;
#if !defined( NDEBUG )
		mutable value_type m_debugData[Rows * Columns];
#endif
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
	template< typename T, uint32_t Rows, uint32_t Columns > bool operator ==( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< T, Rows, Columns > const & p_mtxB );
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
	template< typename T, uint32_t Rows, uint32_t Columns > bool operator !=( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< T, Rows, Columns > const & p_mtxB );
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
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Matrix< T, Rows, Columns > operator +( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< U, Rows, Columns > const & p_mtxB );
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
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Matrix< T, Rows, Columns > operator -( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< U, Rows, Columns > const & p_mtxB );
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
	template< typename T, uint32_t Rows, uint32_t Columns, typename U, uint32_t _Columns > Matrix< T, Rows, _Columns > operator * ( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< U, Columns, _Columns > const & p_mtxB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_mtxA		The matrix to multiply
	 *\param[in]	p_ptVector	The vector
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_mtxA		La matrice à multiplier
	 *\param[in]	p_ptVector	Le vecteur
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Point< T, Rows > operator *( Matrix< T, Rows, Columns > const  & p_mtxA, Point< U, Columns > const &	p_ptVector );
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
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Matrix< T, Rows, Columns > operator +( Matrix< T, Rows, Columns > const & p_mtxA, U const	* p_mtxB );
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
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Matrix< T, Rows, Columns > operator -( Matrix< T, Rows, Columns > const & p_mtxA, U const	* p_mtxB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_mtxA		The matrix
	 *\param[in]	p_tValue	The value to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur de addition
	 *\param[in]	p_mtxA		La matrice
	 *\param[in]	p_tValue	La valeur à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Matrix< T, Rows, Columns > operator +( Matrix< T, Rows, Columns > const & p_mtxA, T const & p_tValue );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_mtxA		The matrix
	 *\param[in]	p_tValue	The value to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_mtxA		La matrice
	 *\param[in]	p_tValue	La valeur à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Matrix< T, Rows, Columns > operator -( Matrix< T, Rows, Columns > const & p_mtxA, T const & p_tValue );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_mtxA		The matrix to multiply
	 *\param[in]	p_tValue	The value
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_mtxA		La matrice à multiplier
	 *\param[in]	p_tValue	La valeur
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Matrix< T, Rows, Columns > operator *( Matrix< T, Rows, Columns > const & p_mtxA, T const & p_tValue );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_mtxA		The matrix to divide
	 *\param[in]	p_tValue	The value
	 *\return		The division result
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_mtxA		La matrice à diviser
	 *\param[in]	p_tValue	La valeur
	 *\return		Le résultat de la division
	 */
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Matrix< T, Rows, Columns > operator /( Matrix< T, Rows, Columns > const & p_mtxA, T const & p_tValue );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_matrix	The matrix
	 *\param[in]	p_tValue	The value to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur de addition
	 *\param[in]	p_matrix	La matrice
	 *\param[in]	p_tValue	La valeur à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Matrix< T, Rows, Columns > operator + ( T const & p_tValue, Matrix< U, Rows, Columns > const & p_matrix );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_matrix	The matrix
	 *\param[in]	p_tValue	The value to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_matrix	La matrice
	 *\param[in]	p_tValue	La valeur à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Matrix< T, Rows, Columns > operator - ( T const & p_tValue, Matrix< U, Rows, Columns > const & p_matrix );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_matrix	The matrix to multiply
	 *\param[in]	p_tValue	The value
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_matrix	La matrice à multiplier
	 *\param[in]	p_tValue	La valeur
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Rows, uint32_t Columns, typename U > Matrix< T, Rows, Columns > operator * ( T const & p_tValue, Matrix< U, Rows, Columns > const & p_matrix );
	/*
	 *\~english
	 *\brief		Negation operator
	 *\param[in]	p_matrix	The matrix
	 *\return		The negation result
	 *\~french
	 *\brief		Opérateur de négation
	 *\param[in]	p_matrix	La matrice
	 *\return		Le résultat de la négation
	 */
	template< typename T, uint32_t Rows, uint32_t Columns > Matrix< T, Rows, Columns > operator - ( Matrix< T, Rows, Columns > const & p_matrix );
}
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamOut	The stream receiving matrix's data
 *\param[in]		p_matrix	The input matrix
 *\return			A reference to the stream
 *\~french
 *\brief			Opérateur de flux
 *\param[in,out]	p_streamOut	Le flux qui reçoit les données de la matrice
 *\param[in]		p_matrix	La matrice entré
 *\return			Une référence sur le flux
 */
template< typename T, uint32_t Rows, uint32_t Columns > Castor::String & operator <<( Castor::String & p_streamOut, Castor::Matrix< T, Rows, Columns > const & p_matrix );
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamIn	The stream holding matrix's data
 *\param[in,out]	p_matrix	The output matrix
 *\return			A reference to the stream
 *\~french
 *\brief			Opérateur de flux
 *\param[in,out]	p_streamIn	Le flux qui contient les données de la matrice
 *\param[in,out]	p_matrix	La matrice sortie
 *\return			Une référence sur le flux
 */
template< typename T, uint32_t Rows, uint32_t Columns	> Castor::String & operator >>( Castor::String & p_streamIn, Castor::Matrix< T, Rows, Columns > & p_matrix );
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamOut	The stream receiving matrix's data
 *\param[in]		p_matrix	The input matrix
 *\return			A reference to the stream
 *\~french
 *\brief			Opérateur de flux
 *\param[in,out]	p_streamOut	Le flux qui reçoit les données de la matrice
 *\param[in]		p_matrix	La matrice entré
 *\return			Une référence sur le flux
 */
template< typename T, uint32_t Rows, uint32_t Columns	> std::ostream & operator <<( std::ostream & p_streamOut, Castor::Matrix< T, Rows, Columns > const & p_matrix );
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamIn	The stream holding matrix's data
 *\param[in,out]	p_matrix	The output matrix
 *\return			A reference to the stream
 *\~french
 *\brief			Opérateur de flux
 *\param[in,out]	p_streamIn	Le flux qui contient les données de la matrice
 *\param[in,out]	p_matrix	La matrice sortie
 *\return			Une référence sur le flux
 */
template< typename T, uint32_t Rows, uint32_t Columns	> std::istream & operator >>( std::istream & p_streamIn, Castor::Matrix< T, Rows, Columns > & p_matrix );
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamOut	The stream receiving matrix's data
 *\param[in]		p_matrix	The input matrix
 *\return			A reference to the stream
 *\~french
 *\brief			Opérateur de flux
 *\param[in,out]	p_streamOut	Le flux qui reçoit les données de la matrice
 *\param[in]		p_matrix	La matrice entré
 *\return			Une référence sur le flux
 */
template< typename T, uint32_t Rows, uint32_t Columns	> std::wostream & operator <<( std::wostream & p_streamOut, Castor::Matrix< T, Rows, Columns > const & p_matrix );
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamIn	The stream holding matrix's data
 *\param[in,out]	p_matrix	The output matrix
 *\return			A reference to the stream
 *\~french
 *\brief			Opérateur de flux
 *\param[in,out]	p_streamIn	Le flux qui contient les données de la matrice
 *\param[in,out]	p_matrix	La matrice sortie
 *\return			Une référence sur le flux
 */
template< typename T, uint32_t Rows, uint32_t Columns	> std::wistream & operator >>( std::wistream & p_streamIn, Castor::Matrix< T, Rows, Columns	> & p_matrix );

#include "Matrix.inl"

#endif
