/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_MATRIX_H___
#define ___CASTOR_MATRIX_H___

#include "CastorUtils.hpp"
#include "Exception/Assertion.hpp"
#include "Point.hpp"

namespace castor
{
	template< typename T, uint32_t Columns, uint32_t Rows >
	struct MatrixDataAllocator
	{
		static T * allocate( uint32_t p_count = 1 )
		{
			return new T[Columns * Rows * p_count];
		}

		static void deallocate( T * p_data )
		{
			delete[] p_data;
		}
	};

#if CASTOR_USE_SSE2

	template<>
	struct MatrixDataAllocator< float, 4, 4 >
	{
		static float * allocate( uint32_t p_count = 1 )
		{
			return alignedAlloc< float >( 16, 64 * p_count );
		}

		static void deallocate( float * p_data )
		{
			alignedFree( p_data );
		}
	};

#endif

	/*!
	\author		Sylvain DOREMUS
	\version	0.1.0.0
	\date		09/02/2010
	\~english
	\brief		Templated column major matrix representation.
	\remark		Can hold any type which has a defined Policy.
	\~french
	\brief		Représentation d'une matrice column major, le type des éléments et les dimensions de la matrice sont en template.
	\remark		Peut contenir n'importe quel élément qui a une castor::Policy.
	*/
	template< typename T, uint32_t Columns, uint32_t Rows >
	class Matrix
	{
	protected:
		typedef T __value_type;
		typedef Matrix< __value_type, Columns, Rows > __type;
		typedef Matrix< __value_type, Rows, Columns > __transpose;
		typedef Point< __value_type, Columns > __row;
		typedef Coords< __value_type, Rows > __column;
		typedef Coords< __value_type const, Rows > __const_column;
		static const std::size_t count = Rows * Columns;
		static const std::size_t size = sizeof( T ) * Rows * Columns;

	public:
		//!\~english	Typedef on the data type.
		//!\~french		Typedef sur le type de données.
		typedef __value_type value_type;
		//!\~english	Typedef on the column type
		//!\~french		Typedef sur le type de colonne.
		typedef __column col_type;
		//!\~english	Typedef on the column type
		//!\~french		Typedef sur le type de colonne.
		typedef __const_column const_col_type;
		//!\~english	Typedef on the line type.
		//!\~french		Typedef sur le type de ligne.
		typedef __row row_type;
		//!\~english	Typedef on the transposed matrix type.
		//!\~french		Typedef sur le type de matrice transposée.
		typedef __transpose transpose_type;
		//!\~english	Typedef on this matrix type.
		//!\~french		Typedef sur le type de cette matrice.
		typedef Matrix< value_type, Columns, Rows > my_type;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		Matrix();
		/**
		 *\~english
		 *\brief		Constructor, without data initialisation.
		 *\~french
		 *\brief		Constructeur, sans initialisation des données.
		 */
		Matrix( NoInit const & );
		/**
		 *\~english
		 *\brief		Copy Constructor.
		 *\param[in]	p_matrix	The Matrix object to copy.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	p_matrix	L'objet Matrix à copier.
		 */
		Matrix( Matrix< T, Columns, Rows > const & p_matrix );
		/**
		 *\~english
		 *\brief		Move Constructor.
		 *\param[in]	p_matrix	The Matrix object to move.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 *\param[in]	p_matrix	L'objet Matrix à déplacer.
		 */
		Matrix( Matrix< T, Columns, Rows > && p_matrix );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_value	The matrix is initialised as Jordan with that value.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_value	La matrice est initialisée comme une Jordan avec cette valeur.
		 */
		explicit Matrix( T const & p_value );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_matrix	Data buffer to copy in the matrix.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_matrix	Buffer de données à copier dans la matrice.
		 */
		explicit Matrix( T * p_matrix );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_matrix	Data buffer to copy in the matrix.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_matrix	Buffer de données à copier dans la matrice.
		 */
		template< typename Type >
		explicit Matrix( Type const * p_matrix );
		/**
		 *\~english
		 *\brief		Conversion Copy Constructor.
		 *\param[in]	p_matrix	The Matrix object to copy.
		 *\~french
		 *\brief		Constructeur par copie convertie.
		 *\param[in]	p_matrix	L'objet Matrix à copier.
		 */
		template< typename Type >
		explicit Matrix( Matrix< Type, Columns, Rows > const & p_matrix );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~Matrix();
		/**
		 *\~english
		 *\brief		sets the values for the given row.
		 *\param[in]	p_index	The row to affect.
		 *\param[in]	p_row	The values.
		 *\~french
		 *\brief		Définit les valeurs d'une ligne donnée.
		 *\param[in]	p_index	La ligne à affecter.
		 *\param[in]	p_row	Les valeurs.
		 */
		void setRow( uint32_t p_index, value_type const * p_row );
		/**
		 *\~english
		 *\brief		sets the values for the given row.
		 *\param[in]	p_index	The row to affect.
		 *\param[in]	p_row	The values.
		 *\~french
		 *\brief		Définit les valeurs d'une ligne donnée.
		 *\param[in]	p_index	La ligne à affecter.
		 *\param[in]	p_row	Les valeurs.
		 */
		void setRow( uint32_t p_index, Point< value_type, Columns > const & p_row );
		/**
		 *\~english
		 *\brief		sets the values for the given row.
		 *\param[in]	p_index	The row to affect.
		 *\param[in]	p_row	The values.
		 *\~french
		 *\brief		Définit les valeurs d'une ligne donnée.
		 *\param[in]	p_index	La ligne à affecter.
		 *\param[in]	p_row	Les valeurs.
		 */
		void setRow( uint32_t p_index, Coords< value_type, Columns > const & p_row );
		/**
		 *\~english
		 *\brief		Retrieves a row.
		 *\param[in]	p_index	The row index.
		 *\return		The row.
		 *\~french
		 *\brief		Récupère une ligne.
		 *\param[in]	p_index	L'index de la ligne.
		 *\return		La ligne.
		 */
		row_type getRow( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Retrieves a row.
		 *\param[in]	p_index		The row index.
		 *\param[out]	p_result	Receives the row values.
		 *\~french
		 *\brief		Récupère une ligne.
		 *\param[in]	p_index		L'index de la ligne.
		 *\param[out]	p_result	Reçoit les valeurs de la ligne.
		 */
		void getRow( uint32_t p_index, row_type & p_result )const;
		/**
		 *\~english
		 *\brief		sets the values for the given column.
		 *\param[in]	p_index	The column to affect.
		 *\param[in]	p_col	The values.
		 *\~french
		 *\brief		Définit les valeurs d'une colonne donnée.
		 *\param[in]	p_index	La colonne à affecter.
		 *\param[in]	p_col	Les valeurs.
		 */
		void setColumn( uint32_t p_index, value_type const * p_col );
		/**
		 *\~english
		 *\brief		sets the values for the given column.
		 *\param[in]	p_index	The column to affect.
		 *\param[in]	p_col	The values.
		 *\~french
		 *\brief		Définit les valeurs d'une colonne donnée.
		 *\param[in]	p_index	La colonne à affecter.
		 *\param[in]	p_col	Les valeurs.
		 */
		void setColumn( uint32_t p_index, Point< value_type, Rows > const & p_col );
		/**
		 *\~english
		 *\brief		sets the values for the given column.
		 *\param[in]	p_index	The column to affect.
		 *\param[in]	p_col	The values.
		 *\~french
		 *\brief		Définit les valeurs d'une colonne donnée.
		 *\param[in]	p_index	La colonne à affecter.
		 *\param[in]	p_col	Les valeurs.
		 */
		void setColumn( uint32_t p_index, Coords< value_type const, Rows > const & p_col );
		/**
		 *\~english
		 *\brief		sets the values for the given column.
		 *\param[in]	p_index	The column to affect.
		 *\param[in]	p_col	The values.
		 *\~french
		 *\brief		Définit les valeurs d'une colonne donnée.
		 *\param[in]	p_index	La colonne à affecter.
		 *\param[in]	p_col	Les valeurs.
		 */
		void setColumn( uint32_t p_index, Coords< value_type, Rows > const & p_col );
		/**
		 *\~english
		 *\brief		Retrieves a column.
		 *\param[in]	p_index	The column index.
		 *\return		The column.
		 *\~french
		 *\brief		Récupère une colonne.
		 *\param[in]	p_index	L'index de la colonne.
		 *\return		La colonne.
		 */
		const_col_type getColumn( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Retrieves a column.
		 *\param[in]	p_index	The column index.
		 *\return		The column.
		 *\~french
		 *\brief		Récupère une colonne.
		 *\param[in]	p_index	L'index de la colonne.
		 *\return		La colonne.
		 */
		col_type getColumn( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Retrieves a column.
		 *\param[in]	p_index		The column index.
		 *\param[out]	p_result	Receives the column values.
		 *\~french
		 *\brief		Récupère une colonne.
		 *\param[in]	p_index		L'index de la colonne.
		 *\param[out]	p_result	Reçoit les valeurs de la colonne.
		 */
		void getColumn( uint32_t p_index, const_col_type & p_result )const;
		/**
		 *\~english
		 *\brief		Retrieves a column.
		 *\param[in]	p_index		The column index.
		 *\param[out]	p_result	Receives the column values.
		 *\~french
		 *\brief		Récupère une colonne.
		 *\param[in]	p_index		L'index de la colonne.
		 *\param[out]	p_result	Reçoit les valeurs de la colonne.
		 */
		void getColumn( uint32_t p_index, col_type & p_result );
		/**
		 *\~english
		 *\brief		Retrieves the value at the given position.
		 *\param[in]	p_row, p_column	The position.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère la valeur à la position donnée.
		 *\param[in]	p_row, p_column	La position.
		 *\return		La valeur.
		 */
		value_type valueAt( uint32_t p_column, uint32_t p_row );
		/**
		 *\~english
		 *\brief		Retrieves the column at given index.
		 *\param[in]	p_index	The index.
		 *\return		A constant pointer on the column's first element.
		 *\~french
		 *\brief		Récupère la colonne à l'index donné.
		 *\param[in]	p_index	L'index.
		 *\return		Un pointeur constant sur le premier élément de la colonne.
		 */
		const_col_type operator[]( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Retrieves the column at given index.
		 *\param[in]	p_index	The index.
		 *\return		A pointer on the column's first element.
		 *\~french
		 *\brief		Récupère la colonne à l'index donné.
		 *\param[in]	p_index	L'index.
		 *\return		Un pointeur sur le premier élément de la colonne.
		 */
		col_type & operator[]( uint32_t p_index );
		/**
		 *\~english
		 *\return		The pointer on datas.
		 *\~french
		 *\return		Le pointeur sur les données.
		 */
		inline value_type * ptr();
		/**
		 *\~english
		 *\return		The pointer on constant datas.
		 *\~french
		 *\return		Le pointeur sur les données constantes.
		 */
		inline value_type const * constPtr()const;
		/**
		 *\~english
		 *\brief		Links the data pointer to the one given in parameter.
		 *\remarks		The matrix loses ownership of it's data.
		 *\~french
		 *\brief		Lie les données de cette matrice à celles données en paramètre.
		 *\remarks		La matrice perd la maîtrise de ses données.
		 */
		void link( T * p_coords );
		/**
		 *\~english
		 *\brief		Initialises the matrix as a Jordan one, with the given value.
		 *\param[in]	p_value	The value.
		 *\~french
		 *\brief		Initialise la matrice comme une matrice de Jordan, avec la valeur donnée.
		 *\param[in]	p_value	La valeur.
		 */
		void initialise( value_type const & p_value = value_type{} );
		/**
		 *\~english
		 *\return		The transposed of this matrix.
		 *\~french
		 *\return		La transposée de cette matrice.
		 */
		transpose_type getTransposed()const;
		/**
		 *\~english
		 *\brief		Computes the transposed of this matrix.
		 *\param[in]	p_result	Receives the transposed.
		 *\~french
		 *\brief		Calcule la transposée de cette matrice.
		 *\param[in]	p_result	Reçoit la transposée.
		 */
		void getTransposed( transpose_type & p_result )const;
		/**
		 *\~english
		 *\return		The identity matrix.
		 *\~french
		 *\return		La matrice identité.
		 */
		my_type getIdentity()const;
		/**
		 *\~english
		 *\return		The trace or this matrix.
		 *\~french
		 *\return		La trace de cette matrice.
		 */
		value_type getTrace()const;
		/**
		 *\~english
		 *\brief		sets this matrix to identity
		 *\~french
		 *\brief		Définit cette matrice à l'identité
		 */
		void setIdentity();
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\param[in]	p_matrix	The Matrix object to copy.
		 *\return		A reference to this Matrix object.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 *\param[in]	p_matrix	L'objet Matrix à copier.
		 *\return		Une référence sur cet objet Matrix.
		 */
		Matrix< T, Columns, Rows > & operator=( Matrix< T, Columns, Rows > const & p_matrix );
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\param[in]	p_matrix	The Matrix object to move.
		 *\return		A reference to this Matrix object.
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement.
		 *\param[in]	p_matrix	L'objet Matrix à déplacer.
		 *\return		Une référence sur cet objet Matrix.
		 */
		Matrix< T, Columns, Rows > & operator=( Matrix< T, Columns, Rows > && p_matrix );
		/**
		 *\~english
		 *\brief		Conversion Copy assignment operator.
		 *\param[in]	p_matrix	The Matrix object to copy.
		 *\return		A reference to this Matrix object.
		 *\~french
		 *\brief		Opérateur d'affectation par copie convertie.
		 *\param[in]	p_matrix	L'objet Matrix à copier.
		 *\return		Une référence sur cet objet Matrix.
		 */
		template< typename Type > Matrix< T, Columns, Rows > & operator=( Matrix< Type, Columns, Rows > const & p_matrix );
		/**
		 *\~english
		 *\brief		Conversion Copy assignment operator.
		 *\param[in]	p_matrix	The data pointer to copy.
		 *\return		A reference to this Matrix object.
		 *\~french
		 *\brief		Opérateur d'affectation par copie convertie.
		 *\param[in]	p_matrix	Le pointeur sur les données à copier.
		 *\return		Une référence sur cet objet Matrix.
		 */
		template< typename Type > Matrix< T, Columns, Rows > & operator=( Type const * p_matrix );
		/**
		 *\~english
		 *\brief		addition assignment operator.
		 *\param[in]	p_matrix	The Matrix object to add.
		 *\return		A reference to this Matrix object.
		 *\~french
		 *\brief		Opérateur d'affectation par addition.
		 *\param[in]	p_matrix	L'objet Matrix à ajouter.
		 *\return		Une référence sur cet objet Matrix.
		 */
		template< typename Type > Matrix< T, Columns, Rows > & operator+=( Matrix< Type, Columns, Rows > const & p_matrix );
		/**
		 *\~english
		 *\brief		Substraction assignment operator.
		 *\param[in]	p_matrix	The Matrix object to subtract.
		 *\return		A reference to this Matrix object.
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction.
		 *\param[in]	p_matrix	L'objet Matrix à soustraire.
		 *\return		Une référence sur cet objet Matrix.
		 */
		template< typename Type > Matrix< T, Columns, Rows > & operator-=( Matrix< Type, Columns, Rows > const & p_matrix );
		/**
		 *\~english
		 *\brief		addition assignment operator.
		 *\param[in]	p_value	The value to add.
		 *\return		A reference to this Matrix object.
		 *\~french
		 *\brief		Opérateur d'affectation par addition.
		 *\param[in]	p_value	La valeur à additionner.
		 *\return		Une référence sur cet objet Matrix.
		 */
		Matrix< T, Columns, Rows > & operator+=( T const & p_value );
		/**
		 *\~english
		 *\brief		Substraction assignment operator.
		 *\param[in]	p_value	The value to subtract.
		 *\return		A reference to this Matrix object.
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction.
		 *\param[in]	p_value	La valeur à soustraire.
		 *\return		Une référence sur cet objet Matrix.
		 */
		Matrix< T, Columns, Rows > & operator-=( T const & p_value );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator.
		 *\param[in]	p_value	The value to multiply.
		 *\return		A reference to this Matrix object.
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication.
		 *\param[in]	p_value	La valeur à multiplier.
		 *\return		Une référence sur cet objet Matrix.
		 */
		Matrix< T, Columns, Rows > & operator*=( T const & p_value );
		/**
		 *\~english
		 *\brief		Division assignment operator.
		 *\param[in]	p_value	The value to divide.
		 *\return		A reference to this Matrix object.
		 *\~french
		 *\brief		Opérateur d'affectation par division.
		 *\param[in]	p_value	La valeur à diviser.
		 *\return		Une référence sur cet objet Matrix.
		 */
		Matrix< T, Columns, Rows > & operator/=( T const & p_value );
		/**
		 *\~english
		 *\brief		Indexing operator.
		 *\param[in]	p_col	The column index.
		 *\param[in]	p_row	The row index.
		 *\return		A reference to the indexed value.
		 *\~french
		 *\brief		Opérateur d'indexation.
		 *\param[in]	p_col	L'index de la colonne.
		 *\param[in]	p_row	L'index de la ligne.
		 *\return		Une référence sur la valeur indexée.
		 */
		T & operator()( uint32_t p_col, uint32_t p_row );
		/**
		 *\~english
		 *\brief		Indexing operator.
		 *\param[in]	p_col	The column index.
		 *\param[in]	p_row	The row index.
		 *\return		A reference to the indexed value.
		 *\~french
		 *\brief		Opérateur d'indexation.
		 *\param[in]	p_col	L'index de la colonne.
		 *\param[in]	p_row	L'index de la ligne.
		 *\return		Une référence sur la valeur indexée.
		 */
		T const & operator()( uint32_t p_col, uint32_t p_row )const;

	protected:
		my_type recGetMinor( uint32_t x, uint32_t y, uint32_t p_rows, uint32_t p_cols )const;
		void doUpdateColumns()const;
		
	protected:
		//!\~english	The matrix data.
		//!\~french		Les données de la matrice.
		T * m_data;
		mutable col_type m_columns[Columns];
#if !defined( NDEBUG )
		mutable value_type * m_debugData[Columns][Rows];
#endif

	private:
		//!\~english The matrix owns its data.	\~french La matrice st responsable de ses données.
		bool m_ownCoords;
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
	template< typename T, uint32_t Columns, uint32_t Rows > bool operator==( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< T, Columns, Rows > const & p_mtxB );
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
	template< typename T, uint32_t Columns, uint32_t Rows > bool operator!=( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< T, Columns, Rows > const & p_mtxB );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Matrix< T, Columns, Rows > operator+( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< U, Columns, Rows > const & p_mtxB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to subtract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Matrix< T, Columns, Rows > operator-( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< U, Columns, Rows > const & p_mtxB );
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
	template< typename T, uint32_t Columns, uint32_t Rows, typename U, uint32_t _Columns > Matrix< T, _Columns, Rows > operator*( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< U, _Columns, Columns > const & p_mtxB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_mtxA		The matrix to multiply
	 *\param[in]	p_vector	The vector
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_mtxA		La matrice à multiplier
	 *\param[in]	p_vector	Le vecteur
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Point< T, Rows > operator*( Matrix< T, Columns, Rows > const & p_mtxA, Point< U, Columns > const & p_vector );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_vector	The vector
	 *\param[in]	p_mtxA		The matrix to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_vector	Le vecteur
	 *\param[in]	p_mtxA		La matrice à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Point< T, Columns > operator*( Point< T, Rows > const & p_vector, Matrix< U, Columns, Rows > const & p_mtxA );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Matrix< T, Columns, Rows > operator+( Matrix< T, Columns, Rows > const & p_mtxA, U const * p_mtxB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_mtxA, p_mtxB	The matrices to subtract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_mtxA, p_mtxB	Les matrices à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Matrix< T, Columns, Rows > operator-( Matrix< T, Columns, Rows > const & p_mtxA, U const * p_mtxB );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	p_mtxA		The matrix
	 *\param[in]	p_value	The value to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur de addition
	 *\param[in]	p_mtxA		La matrice
	 *\param[in]	p_value	La valeur à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Matrix< T, Columns, Rows > operator+( Matrix< T, Columns, Rows > const & p_mtxA, T const & p_value );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_mtxA		The matrix
	 *\param[in]	p_value	The value to subtract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_mtxA		La matrice
	 *\param[in]	p_value	La valeur à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Matrix< T, Columns, Rows > operator-( Matrix< T, Columns, Rows > const & p_mtxA, T const & p_value );
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
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Matrix< T, Columns, Rows > operator*( Matrix< T, Columns, Rows > const & p_mtxA, T const & p_value );
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
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Matrix< T, Columns, Rows > operator/( Matrix< T, Columns, Rows > const & p_mtxA, T const & p_value );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	p_matrix	The matrix
	 *\param[in]	p_value	The value to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur de addition
	 *\param[in]	p_matrix	La matrice
	 *\param[in]	p_value	La valeur à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Matrix< T, Columns, Rows > operator+( T const & p_value, Matrix< U, Columns, Rows > const & p_matrix );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_matrix	The matrix
	 *\param[in]	p_value	The value to subtract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_matrix	La matrice
	 *\param[in]	p_value	La valeur à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Matrix< T, Columns, Rows > operator-( T const & p_value, Matrix< U, Columns, Rows > const & p_matrix );
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
	template< typename T, uint32_t Columns, uint32_t Rows, typename U > Matrix< T, Columns, Rows > operator*( T const & p_value, Matrix< U, Columns, Rows > const & p_matrix );
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
	template< typename T, uint32_t Columns, uint32_t Rows > Matrix< T, Columns, Rows > operator-( Matrix< T, Columns, Rows > const & p_matrix );
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
	template< typename T, uint32_t Columns, uint32_t Rows > castor::String & operator<<( castor::String & p_streamOut, castor::Matrix< T, Columns, Rows > const & p_matrix );
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
	template< typename T, uint32_t Columns, uint32_t Rows > castor::String & operator>>( castor::String & p_streamIn, castor::Matrix< T, Columns, Rows > & p_matrix );
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
	template< typename CharT, typename T, uint32_t Columns, uint32_t Rows > std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & p_streamOut, castor::Matrix< T, Columns, Rows > const & p_matrix );
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
	template< typename CharT, typename T, uint32_t Columns, uint32_t Rows > std::basic_istream< CharT > & operator>>( std::basic_istream< CharT > & p_streamIn, castor::Matrix< T, Columns, Rows > & p_matrix );
}

#include "Matrix.inl"

#endif
