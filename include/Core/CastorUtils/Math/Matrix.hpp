/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_MATRIX_H___
#define ___CASTOR_MATRIX_H___

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Math/Point.hpp"

namespace c3d
{
	template< typename T, uint32_t Columns, uint32_t Rows >
	class Matrix
	{
		template< typename U, uint32_t C, uint32_t R >
		friend class Matrix;

	protected:
		static const std::size_t count = Rows * Columns;
		static const std::size_t size = sizeof( T ) * Rows * Columns;
		template< typename ... Types >
		static bool constexpr isRightSize = sizeof...( Types ) == count;

	public:
		//!\~english	The data type.
		//!\~french		Le type de données.
		using value_type = T;
		//!\~english	The column type
		//!\~french		Le type de colonne.
		using col_type = Point< value_type, Rows >;
		//!\~english	The line type.
		//!\~french		Le type de ligne.
		using row_type = Point< value_type, Columns >;
		//!\~english	The transposed matrix type.
		//!\~french		Le type de matrice transposée.
		using transpose_type = Matrix< value_type, Rows, Columns >;
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
		Matrix( Matrix const & matrix );
		Matrix( Matrix && matrix )noexcept;
		explicit Matrix( T const & value );
		explicit Matrix( T * matrix );
		template< typename Type >
		explicit Matrix( Type const * matrix );
		template< typename Type >
		explicit Matrix( Array< Type, Columns * Rows > const & rhs );
		template< typename Type >
		explicit Matrix( Matrix< Type, Columns, Rows > const & matrix );
		explicit Matrix( std::initializer_list< T > rhs );
		~Matrix()noexcept = default;

		template< typename ... Types >
		explicit Matrix( Types && ... inits ) requires isRightSize< Types... >
			: Matrix{ Array< T, Columns * Rows >{ std::forward< Types >( inits )... } }
		{
		}
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
		void setRow( uint32_t index, PointView< value_type, Columns > const & row );
		Point< value_type, Columns > getRow( uint32_t index )const;
		void getRow( uint32_t index, Point< value_type, Columns > & result )const;

		void setColumn( uint32_t index, value_type const * col );
		void setColumn( uint32_t index, Point< value_type, Rows > const & col );
		void setColumn( uint32_t index, PointView< value_type const, Rows > const & col );
		void setColumn( uint32_t index, PointView< value_type, Rows > const & col );
		Point< value_type, Rows > const & getColumn( uint32_t index )const;
		Point< value_type, Rows > & getColumn( uint32_t index );
		void getColumn( uint32_t index, Point< value_type, Rows > & result )const;
		void getColumn( uint32_t index, PointView< value_type, Rows > & result );
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
		value_type * ptr();
		/**
		 *\~english
		 *\return		The pointer on constant datas.
		 *\~french
		 *\return		Le pointeur sur les données constantes.
		 */
		value_type const * constPtr()const;
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
		static my_type getIdentity();
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
		Matrix & operator=( Matrix const & rhs );
		Matrix & operator=( Matrix && rhs )noexcept;
		template< typename Type > Matrix & operator=( Matrix< Type, Columns, Rows > const & rhs );
		template< typename Type > Matrix & operator=( Type const * rhs );
		/**@}*/
		/**
		 *\~english
		 *name Arithmetic operators.
		 *\~french
		 *name Opérateurs arithmétiques.
		**/
		/**@{*/
		template< typename Type > Matrix & operator+=( Matrix< Type, Columns, Rows > const & rhs );
		template< typename Type > Matrix & operator-=( Matrix< Type, Columns, Rows > const & rhs );
		Matrix & operator+=( T const & rhs );
		Matrix & operator-=( T const & rhs );
		Matrix & operator*=( T const & rhs );
		Matrix & operator/=( T const & rhs );
		/**@}*/

	protected:
		my_type recGetMinor( uint32_t x, uint32_t y, uint32_t rows, uint32_t cols )const;
		
	protected:
		union
		{
			Array< T, Columns * Rows > m_data;
			Array< col_type, Columns > m_columns;
		};
		/**
		 *\~english
		 *name Logic operators.
		 *\~french
		 *name Opérateurs logiques
		**/
		/**@{*/
		friend bool operator==( Matrix const & lhs, Matrix const & rhs )noexcept
		{
			bool result = true;

			uint32_t i = 0;
			while ( i < Columns && result )
			{
				uint32_t j = 0;
				while ( j < Rows && result )
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
		friend Matrix operator+( Matrix const & lhs, Matrix< U, Columns, Rows > const & rhs )
		{
			Matrix mtx( lhs );
			mtx += rhs;
			return mtx;
		}

		template< typename U >
		friend Matrix operator-( Matrix const & lhs, Matrix< U, Columns, Rows > const & rhs )
		{
			Matrix mtx( lhs );
			mtx -= rhs;
			return mtx;
		}

		template< typename U, uint32_t ColumnsU >
		friend Matrix< T, ColumnsU, Rows > operator*( Matrix const & lhs, Matrix< U, ColumnsU, Columns > const & rhs )
		{
			Matrix< T, ColumnsU, Rows > result;

			for ( uint32_t i = 0; i < ColumnsU; i++ )
			{
				for ( uint32_t j = 0; j < Columns; j++ )
				{
					for ( uint32_t k = 0; k < Rows; k++ )
					{
						result[i][k] += T( lhs[j][k] * rhs[i][j] );
					}
				}
			}

			return result;
		}

		template< typename U >
		friend Point< T, Rows > operator*( Matrix const & lhs, Point< U, Columns > const & rhs )
		{
			Point< T, Rows > result;

			for ( uint32_t i = 0; i < Columns; i++ )
			{
				for ( uint32_t j = 0; j < Rows; j++ )
				{
					result[j] += T( lhs[i][j] * rhs[i] );
				}
			}

			return result;
		}

		template< typename U >
		friend Point< T, Columns > operator*( Point< T, Rows > const & lhs, Matrix< U, Columns, Rows > const & rhs )
		{
			return rhs * lhs;
		}

		template< typename U >
		friend Matrix operator+( Matrix const & lhs, U const * rhs )
		{
			Matrix mtx( lhs );
			mtx += rhs;
			return mtx;
		}

		template< typename U >
		friend Matrix operator-( Matrix const & lhs, U const * rhs )
		{
			Matrix mtx( lhs );
			mtx -= rhs;
			return mtx;
		}

		friend Matrix operator+( Matrix const & lhs, T const & rhs )
		{
			Matrix mtx( lhs );
			mtx += rhs;
			return mtx;
		}

		friend Matrix operator-( Matrix const & lhs, T const & rhs )
		{
			Matrix mtx( lhs );
			mtx -= rhs;
			return mtx;
		}

		friend Matrix operator*( Matrix const & lhs, T const & rhs )
		{
			Matrix mtx( lhs );
			mtx *= rhs;
			return mtx;
		}

		friend Matrix operator/( Matrix const & lhs, T const & rhs )
		{
			Matrix mtx( lhs );
			mtx /= rhs;
			return mtx;
		}

		friend Matrix operator+( T const & lhs, Matrix const & rhs )
		{
			return rhs + lhs;
		}

		friend Matrix operator-( T const & lhs, Matrix const & rhs )
		{
			return rhs - lhs;
		}

		friend Matrix operator*( T const & lhs, Matrix const & rhs )
		{
			return rhs * lhs;
		}

		friend Matrix operator-( Matrix const & matrix )
		{
			Matrix result;

			for ( uint32_t i = 0; i < Columns; i++ )
			{
				for ( uint32_t j = 0; j < Rows; j++ )
				{
					result[i][j] = -matrix[i][j];
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
		friend String & operator<<( String & text, Matrix const & matrix )
		{
			StringStream stream{ makeStringStream() };
			stream.precision( 10 );
			stream << matrix;
			text += stream.str();
			return text;
		}

		friend String & operator>>( String & text, Matrix & matrix )
		{
			StringStream stream( text );
			stream >> matrix;
			text = stream.str();
			return text;
		}

		template< typename CharT >
		friend std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & stream, Matrix const & matrix )
		{
			auto precision = stream.precision( 10 );

			for ( uint32_t i = 0; i < Columns; i++ )
			{
				for ( uint32_t j = 0; j < Rows; j++ )
				{
					stream.width( 15 );
					stream << std::right << matrix[i][j];
				}

				stream << std::endl;
			}

			stream.precision( precision );
			return stream;
		}

		template< typename CharT >
		friend std::basic_istream< CharT > & operator>>( std::basic_istream< CharT > & stream, Matrix & matrix )
		{
			for ( uint32_t i = 0; i < Columns; i++ )
			{
				for ( uint32_t j = 0; j < Rows; j++ )
				{
					stream >> matrix[i][j];
				}

				stream.ignore();
			}

			return stream;
		}
		/**@}*/
	};
}

#include "Matrix.inl"

#endif
