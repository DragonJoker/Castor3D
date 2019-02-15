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
		template< typename U, uint32_t C, uint32_t R >
		friend class Matrix;

	protected:
		using __value_type = T;
		using __type = Matrix< __value_type, Columns, Rows >;
		using __transpose = Matrix< __value_type, Rows, Columns >;
		using __row = Point< __value_type, Columns >;
		using __column = Point< __value_type, Rows >;
		static const std::size_t count = Rows * Columns;
		static const std::size_t size = sizeof( T ) * Rows * Columns;

	public:
		//!\~english	The data type.
		//!\~french		Le type de données.
		using value_type = __value_type;
		//!\~english	The column type
		//!\~french		Le type de colonne.
		using col_type = __column;
		//!\~english	The line type.
		//!\~french		Le type de ligne.
		using row_type = __row;
		//!\~english	The transposed matrix type.
		//!\~french		Le type de matrice transposée.
		using transpose_type = __transpose;
		//!\~english	This matrix type.
		//!\~french		Le type de cette matrice.
		using my_type = Matrix< value_type, Columns, Rows >;

	public:
		/**
		 *\~english
		 *name Construction / Destruction.
		 *\~french
		 *name Construction / Destruction.
		 **/
		/**@{*/
		Matrix();
		explicit Matrix( NoInit const & );
		Matrix( Matrix< T, Columns, Rows > const & matrix );
		Matrix( Matrix< T, Columns, Rows > && matrix );
		explicit Matrix( T const & value );
		explicit Matrix( T * matrix );
		template< typename Type >
		explicit Matrix( Type const * matrix );
		template< typename Type >
		explicit Matrix( Matrix< Type, Columns, Rows > const & matrix );
		~Matrix();
		/**@}*/
		/**
		 *\~english
		 *name Row/column access.
		 *\~french
		 *name Accès aux lignes/colonnes.
		 **/
		/**@{*/
		void setRow( uint32_t index, value_type const * row );
		void setRow( uint32_t index, Point< value_type, Columns > const & row );
		void setRow( uint32_t index, Coords< value_type, Columns > const & row );
		Point< value_type, Columns > getRow( uint32_t index )const;
		void getRow( uint32_t index, Point< value_type, Columns > & result )const;

		void setColumn( uint32_t index, value_type const * col );
		void setColumn( uint32_t index, Point< value_type, Rows > const & col );
		void setColumn( uint32_t index, Coords< value_type const, Rows > const & col );
		void setColumn( uint32_t index, Coords< value_type, Rows > const & col );
		Point< value_type, Rows > const & getColumn( uint32_t index )const;
		Point< value_type, Rows > & getColumn( uint32_t index );
		void getColumn( uint32_t index, Point< value_type, Rows > & result )const;
		void getColumn( uint32_t index, Coords< value_type, Rows > & result );
		/**@}*/
		/**
		 *\~english
		 *name Array access.
		 *\~french
		 *name Accesseurs de tableau.
		**/
		/**@{*/
		col_type const & operator[]( uint32_t index )const;
		col_type & operator[]( uint32_t index );
		value_type & operator()( uint32_t col, uint32_t row );
		value_type const & operator()( uint32_t col, uint32_t row )const;
		/**@}*/
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
		 *\brief		Initialises the matrix to 0.
		 *\~french
		 *\brief		Initialise la matrice à 0.
		 */
		void initialise();
		/**
		 *\~english
		 *\brief		Initialises the matrix as a Jordan one, with the given value.
		 *\param[in]	value	The value.
		 *\~french
		 *\brief		Initialise la matrice comme une matrice de Jordan, avec la valeur donnée.
		 *\param[in]	value	La valeur.
		 */
		void initialise( value_type const & value );
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
		 *\param[in]	result	Receives the transposed.
		 *\~french
		 *\brief		Calcule la transposée de cette matrice.
		 *\param[in]	result	Reçoit la transposée.
		 */
		void getTransposed( transpose_type & result )const;
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
		*name Assignment operators.
		*\~french
		*name Opérateurs d'affectation.
		**/
		/**@{*/
		Matrix< T, Columns, Rows > & operator=( Matrix< T, Columns, Rows > const & rhs );
		Matrix< T, Columns, Rows > & operator=( Matrix< T, Columns, Rows > && rhs );
		template< typename Type > Matrix< T, Columns, Rows > & operator=( Matrix< Type, Columns, Rows > const & rhs );
		template< typename Type > Matrix< T, Columns, Rows > & operator=( Type const * rhs );
		/**@}*/
		/**
		 *\~english
		 *name Arithmetic operators.
		 *\~french
		 *name Opérateurs arithmétiques.
		**/
		/**@{*/
		template< typename Type > Matrix< T, Columns, Rows > & operator+=( Matrix< Type, Columns, Rows > const & rhs );
		template< typename Type > Matrix< T, Columns, Rows > & operator-=( Matrix< Type, Columns, Rows > const & rhs );
		Matrix< T, Columns, Rows > & operator+=( T const & rhs );
		Matrix< T, Columns, Rows > & operator-=( T const & rhs );
		Matrix< T, Columns, Rows > & operator*=( T const & rhs );
		Matrix< T, Columns, Rows > & operator/=( T const & rhs );
		/**@}*/

	protected:
		my_type recGetMinor( uint32_t x, uint32_t y, uint32_t rows, uint32_t cols )const;
		
	protected:
		union
		{
			std::array< T, Columns * Rows > m_data;
			std::array< col_type, Columns > m_columns;
		};
	};
	/**
	 *\~english
	 *name Logic operators.
	 *\~french
	 *name Opérateurs logiques
	**/
	/**@{*/
	template< typename T, uint32_t Columns, uint32_t Rows >
	bool operator==( Matrix< T, Columns, Rows > const & lhs, Matrix< T, Columns, Rows > const & rhs );
	template< typename T, uint32_t Columns, uint32_t Rows >
	bool operator!=( Matrix< T, Columns, Rows > const & lhs, Matrix< T, Columns, Rows > const & rhs );
	/**@}*/
	/**
	 *\~english
	 *name Arithmetic operators.
	 *\~french
	 *name Opérateurs arithmétiques.
	**/
	/**@{*/
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator+( Matrix< T, Columns, Rows > const & lhs, Matrix< U, Columns, Rows > const & rhs );
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator-( Matrix< T, Columns, Rows > const & lhs, Matrix< U, Columns, Rows > const & rhs );
	template< typename T, uint32_t Columns, uint32_t Rows, typename U, uint32_t _Columns >
	Matrix< T, _Columns, Rows > operator*( Matrix< T, Columns, Rows > const & lhs, Matrix< U, _Columns, Columns > const & rhs );

	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Point< T, Rows > operator*( Matrix< T, Columns, Rows > const & lhs, Point< U, Columns > const & rhs );

	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Point< T, Columns > operator*( Point< T, Rows > const & lhs, Matrix< U, Columns, Rows > const & rhs );

	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator+( Matrix< T, Columns, Rows > const & lhs, U const * rhs );
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator-( Matrix< T, Columns, Rows > const & lhs, U const * rhs );

	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator+( Matrix< T, Columns, Rows > const & lhs, T const & rhs );
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator-( Matrix< T, Columns, Rows > const & lhs, T const & rhs );
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator*( Matrix< T, Columns, Rows > const & lhs, T const & rhs );
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator/( Matrix< T, Columns, Rows > const & lhs, T const & rhs );

	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator+( T const & lhs, Matrix< U, Columns, Rows > const & rhs );
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator-( T const & lhs, Matrix< U, Columns, Rows > const & rhs );
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator*( T const & lhs, Matrix< U, Columns, Rows > const & rhs );

	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix< T, Columns, Rows > operator-( Matrix< T, Columns, Rows > const & rhs );
	/**@}*/
	/**
	 *\~english
	 *name Stream operators.
	 *\~french
	 *name Opérateurs de flux.
	**/
	/**@{*/
	template< typename T, uint32_t Columns, uint32_t Rows >
	castor::String & operator<<( castor::String & stream, castor::Matrix< T, Columns, Rows > const & matrix );
	template< typename T, uint32_t Columns, uint32_t Rows >
	castor::String & operator>>( castor::String & stream, castor::Matrix< T, Columns, Rows > & matrix );
	template< typename CharT, typename T, uint32_t Columns, uint32_t Rows >
	std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & stream, castor::Matrix< T, Columns, Rows > const & matrix );
	template< typename CharT, typename T, uint32_t Columns, uint32_t Rows >
	std::basic_istream< CharT > & operator>>( std::basic_istream< CharT > & stream, castor::Matrix< T, Columns, Rows > & matrix );
	/**@}*/
}

#include "Matrix.inl"

#endif
