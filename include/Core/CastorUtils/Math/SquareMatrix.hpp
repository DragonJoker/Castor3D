/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_SQUARE_MATRIX_H___
#define ___CASTOR_SQUARE_MATRIX_H___

#include "CastorUtils/Math/Matrix.hpp"

namespace c3d
{
	/**
	\~english
	\brief		Templated column major square matrix representation
	\~french
	\brief		Représentation d'une matrice carrée column major
	*/
	template< typename T, uint32_t Count >
	class SquareMatrix
		: public Matrix< T, Count, Count >
	{
	public:
		using /*value_type = */typename Matrix< T, Count, Count >::value_type;
		using /*col_type = */typename Matrix< T, Count, Count >::col_type;
		using /*row_type = */typename Matrix< T, Count, Count >::row_type;
		template< typename ... Types >
		static bool constexpr isRightSize = sizeof...( Types ) == Count * Count;

		//!\~english	This matrix type
		//!\~french		Le type de cette matrice
		using matrix_type = Matrix< T, Count, Count >;
		//!\~english	This matrix type
		//!\~french		Le type de cette matrice
		using square_type = SquareMatrix< T, Count >;
		//!\~english	The transposed matrix type
		//!\~french		Le type de matrice transposée
		using transpose_type = square_type;

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
		SquareMatrix( SquareMatrix const & rhs );
		SquareMatrix( SquareMatrix && rhs )noexcept;
		template< uint32_t CountU >
		explicit SquareMatrix( SquareMatrix< T, CountU > const & rhs );
		template< typename Type >
		explicit SquareMatrix( SquareMatrix< Type, Count > const & rhs );
		template< typename Type >
		explicit SquareMatrix( Matrix< Type, Count, Count > const & rhs );
		template< typename Type >
		explicit SquareMatrix( Array< Type, Count * Count > const & rhs );
		template< typename Type >
		explicit SquareMatrix( Type const * rhs );
		explicit SquareMatrix( std::initializer_list< T > rhs );

		template< typename ... Types >
		explicit SquareMatrix( Types && ... inits ) requires isRightSize< Types... >
			: matrix_type{ std::forward< Types >( inits )... }
		{
		}
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
		value_type getCofactor( uint32_t column, uint32_t row )const;
		/**
		 *\~english
		 *\brief		Computes the determinant of this matrix
		 *\return		The determinant
		 *\~french
		 *\brief		Calcule le déterminant de cette matrice
		 *\return		Le déterminant
		 */
		value_type getDeterminant()const;
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
		square_type & transpose();
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
		SquareMatrix getInverse()const;
		/**
		 *\~english
		 *\brief		Inverts this matrix
		 *\return		A reference to this matrix
		 *\~french
		 *\brief		Inverse cette matrice
		 *\return		Une référence sur cette matrice
		 */
		SquareMatrix & invert();
		/**
		 *\~english
		 *name Assignment operators.
		 *\~french
		 *name Opérateurs d'affectation.
		**/
		/**@{*/
		template< typename Type >
		SquareMatrix & operator=( Matrix< Type, Count, Count > const & rhs );
		SquareMatrix & operator=( SquareMatrix const & rhs );
		SquareMatrix & operator=( SquareMatrix && rhs )noexcept;
		template< typename Type >
		SquareMatrix & operator=( SquareMatrix< Type, Count > const & rhs );
		template< typename Type >
		SquareMatrix & operator=( Type const * rhs );
		/**@}*/
		/**
		 *\~english
		 *name Arithmetic operators.
		 *\~french
		 *name Opérateurs arithmétiques.
		**/
		/**@{*/
		template< typename Type >
		SquareMatrix & operator+=( SquareMatrix< Type, Count > const & rhs );
		template< typename Type >
		SquareMatrix & operator-=( SquareMatrix< Type, Count > const & rhs );
		template< typename Type >
		SquareMatrix & operator*=( SquareMatrix< Type, Count > const & rhs );
		template< typename Type >
		SquareMatrix & operator+=( Type const * rhs );
		template< typename Type >
		SquareMatrix & operator-=( Type const * rhs );
		template< typename Type >
		SquareMatrix & operator*=( Type const * rhs );
		SquareMatrix & operator+=( T const & rhs );
		SquareMatrix & operator-=( T const & rhs );
		SquareMatrix & operator*=( T const & rhs );
		SquareMatrix & operator/=( T const & rhs );
		/**@}*/

	private:
		/**
		 *\~english
		 *name Logic operators.
		 *\~french
		 *name Opérateurs logiques
		**/
		/**@{*/
		friend bool operator==( SquareMatrix const & lhs, SquareMatrix const & rhs )noexcept
		{
			bool result = true;

			uint32_t i = 0;
			while ( i < Count && result )
			{
				uint32_t j = 0;
				while ( j < Count && result )
				{
					result = lhs[i][j] == rhs[i][j];
					++j;
				}
				++i;
			}

			return result;
		}
		/**@}*/
		/**
		 *\~english
		 *name Arithmetic operators.
		 *\~french
		 *name Opérateurs arithmétiques.
		**/
		/**@{*/
		template< typename U >
		friend SquareMatrix operator+( SquareMatrix const & lhs, SquareMatrix< U, Count > const & rhs )
		{
			SquareMatrix result( lhs );
			result += rhs;
			return result;
		}

		template< typename U >
		friend SquareMatrix operator-( SquareMatrix const & lhs, SquareMatrix< U, Count > const & rhs )
		{
			SquareMatrix result( lhs );
			result -= rhs;
			return result;
		}

		template< typename U >
		friend SquareMatrix operator*( SquareMatrix const & lhs, SquareMatrix< U, Count > const & rhs )
		{
			SquareMatrix result( lhs );
			result *= rhs;
			return result;
		}

		template< typename U >
		friend SquareMatrix operator+( SquareMatrix const & lhs, U const * rhs )
		{
			SquareMatrix result( lhs );
			result += rhs;
			return result;
		}

		template< typename U >
		friend SquareMatrix operator-( SquareMatrix const & lhs, U const * rhs )
		{
			SquareMatrix result( lhs );
			result -= rhs;
			return result;
		}

		template< typename U >
		friend SquareMatrix operator*( SquareMatrix const & lhs, U const * rhs )
		{
			SquareMatrix result( lhs );
			result *= rhs;
			return result;
		}

		template< typename U >
		friend SquareMatrix operator+( SquareMatrix const & lhs, U const & rhs )
		{
			SquareMatrix result( lhs );
			result += rhs;
			return result;
		}

		template< typename U >
		friend SquareMatrix operator-( SquareMatrix const & lhs, U const & rhs )
		{
			SquareMatrix result( lhs );
			result -= rhs;
			return result;
		}

		template< typename U >
		friend SquareMatrix operator*( SquareMatrix const & lhs, U const & rhs )
		{
			SquareMatrix result( lhs );
			result *= rhs;
			return result;
		}

		template< typename U >
		friend SquareMatrix operator/( SquareMatrix const & lhs, U const & rhs )
		{
			SquareMatrix result( lhs );
			result /= rhs;
			return result;
		}

		friend SquareMatrix operator+( T lhs, SquareMatrix const & rhs )
		{
			return rhs + lhs;
		}

		friend SquareMatrix operator-( T lhs, SquareMatrix const & rhs )
		{
			return rhs - lhs;
		}

		friend SquareMatrix operator*( T lhs, SquareMatrix const & rhs )
		{
			return rhs * lhs;
		}

		friend SquareMatrix operator-( SquareMatrix const & lhs )
		{
			SquareMatrix result;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				for ( uint32_t j = 0; j < Count; j++ )
				{
					result[i][j] = -lhs[i][j];
				}
			}

			return result;
		}
		/**@}*/
		/**
		*\~english
		*name Stream operators.
		*\~french
		*name Opérateurs de flux.
		**/
		/**@{*/
		template< typename CharT >
		friend std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & stream, SquareMatrix const & rhs )
		{
			auto precision = stream.precision( 10 );

			for ( uint32_t i = 0; i < Count; i++ )
			{
				for ( uint32_t j = 0; j < Count; j++ )
				{
					stream.width( 15 );
					stream << std::right << rhs[i][j];
				}

				stream << std::endl;
			}

			stream.precision( precision );
			return stream;
		}

		template< typename CharT >
		friend std::basic_istream< CharT > & operator>>( std::basic_istream< CharT > & stream, SquareMatrix & rhs )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				for ( uint32_t j = 0; j < Count; j++ )
				{
					stream >> rhs[i][j];
				}

				stream.ignore();
			}

			return stream;
		}
	   /**@}*/
	};
}

#include "SquareMatrix.inl"

#endif
