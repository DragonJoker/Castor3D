/*
See LICENSE file in root folder
*/
#ifndef ___CU_PointView_H___
#define ___CU_PointView_H___

#include "CastorUtils/Math/MathModule.hpp"

#include "CastorUtils/Data/TextLoader.hpp"
#include "CastorUtils/Data/TextWriter.hpp"
#include "CastorUtils/Math/Point.hpp"

#include <cmath>

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <iostream>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace c3d
{
	template< typename T, uint32_t Count >
	class PointView
	{
	private:
		template< typename U, uint32_t UCount > friend class PointView;

		using value_type = T;
		using reference = value_type &;
		using pointer = value_type * ;
		using const_reference = value_type const &;
		using const_pointer = value_type const *;
		using point_reference = PointView< value_type, Count > &;
		using point_pointer = PointView< value_type, Count > *;
		using const_point_reference = PointView< value_type, Count > const &;
		using const_point_pointer = PointView< value_type, Count > const *;
		static constexpr std::size_t binary_size = sizeof( T ) * Count;

	public:
		//!\~english	Typedef over a pointer on data.
		//!\~french		Typedef d'un pointeur sur les données.
		using iterator = value_type *;
		//!\~english	Typedef over a constant pointer on data.
		//!\~french		Typedef d'un pointeur constant sur les données.
		using const_iterator = value_type const *;

	public:
		/**
		 *\~english
		 *name Construction / Destruction.
		 *\~french
		 *name Construction / Destruction.
		 **/
		/**@{*/
		PointView() = default;
		explicit PointView( Point< T, Count > & rhs );
		explicit PointView( T * rhs );
		/**@}*/
		/**
		 *\~english
		 *name Assignment operators.
		 *\~french
		 *name Opérateurs d'affectation.
		 **/
		/**@{*/
		PointView & operator=( T * rhs );
		PointView & operator=( Point< T, Count > const & rhs );
		/**@}*/
		/**
		 *\~english
		 *name Arithmetic operators.
		 *\~french
		 *name Opérateurs arithmétiques.
		**/
		/**@{*/
		template< typename U >
		PointView & operator+=( PointView< U, Count > const & rhs );
		template< typename U >
		PointView & operator-=( PointView< U, Count > const & rhs );
		template< typename U >
		PointView & operator*=( PointView< U, Count > const & rhs );
		template< typename U >
		PointView & operator/=( PointView< U, Count > const & rhs );

		template< typename U >
		PointView & operator+=( Point< U, Count > const & rhs );
		template< typename U >
		PointView & operator-=( Point< U, Count > const & rhs );
		template< typename U >
		PointView & operator*=( Point< U, Count > const & rhs );
		template< typename U >
		PointView & operator/=( Point< U, Count > const & rhs );

		template< typename U >
		PointView & operator+=( U const * coords );
		template< typename U >
		PointView & operator-=( U const * coords );
		template< typename U >
		PointView & operator*=( U const * coords );
		template< typename U >
		PointView & operator/=( U const * coords );

		PointView & operator+=( T const & coord );
		PointView & operator-=( T const & coord );
		PointView & operator*=( T const & coord );
		PointView & operator/=( T const & coord );
		/**@}*/
		/**
		 *\~english
		 *\brief		Swaps this pont data with the parameter ones
		 *\param[in]	rhs	The point to swap
		 *\~french
		 *\brief		Echange les données de c epoint avec celles du point donné
		 *\param[in]	rhs	Le point à échanger
		 */
		void swap( PointView & rhs )noexcept;
		/**
		 *\~english
		 *\return		The point total size: count * elemSize.
		 *\~french
		 *\return		La taille totale du point (count * elemSize).
		 */
		uint32_t size()const
		{
			return binary_size;
		}
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remarks		No check is made, if you make an index error, expect a crash
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remarks		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une référence constante sur la donnée à l'index voulu
		 */
		T const & operator[]( uint32_t index )const
		{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
			return m_coords[index];
#pragma GCC diagnostic pop
		}
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remarks		No check is made, if you make an index error, expect a crash
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remarks		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une référence sur la donnée à l'index voulu
		 */
		T & operator[]( uint32_t index )
		{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
			return m_coords[index];
#pragma GCC diagnostic pop
		}
		/**
		 *\~english
		 *\brief		Retrieves a non constant pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur non constant sur les données
		 *\return		Le pointeur
		 */
		T * ptr()
		{
			return m_coords;
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur constant sur les données
		 *\return		Le pointeur
		 */
		T const * constPtr()const
		{
			return m_coords;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le premier élément
		 *\return		L'itérateur
		 */
		iterator begin()
		{
			return m_coords;
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le premier élément
		 *\return		L'itérateur
		 */
		const_iterator begin()const
		{
			return m_coords;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le dernier élément
		 *\return		L'itérateur
		 */
		iterator end()
		{
			return m_coords + Count;
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le dernier élément
		 *\return		L'itérateur
		 */
		const_iterator end()const
		{
			return m_coords + Count;
		}

	private:
		T * m_coords{};
		/**
		 *\~english
		 *name Comparison operators.
		 *\~french
		 *name Opérateurs de comparaison.
		**/
		/**@{*/
		template< typename U >
		friend bool operator==( PointView const & lhs, PointView< U, Count > const & rhs )noexcept
		{
			bool result = true;
			uint32_t i = 0;

			while ( i < Count && result )
			{
				result = lhs[i] == rhs[i];
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
		friend Point< std::remove_cv_t< T >, Count > operator-( PointView const & rhs )
		{
			Point< std::remove_cv_t< T >, Count > result;
			for ( uint32_t i = 0; i < Count; ++i )
				result[i] = -rhs[i];
			return result;
		}

		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator+( PointView const & lhs, PointView< U, Count > const & rhs )
		{
			return PtOperators< T, U, Count, Count >::add( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator-( PointView const & lhs, PointView< U, Count > const & rhs )
		{
			return PtOperators< T, U, Count, Count >::sub( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator*( PointView const & lhs, PointView< U, Count > const & rhs )
		{
			return PtOperators< T, U, Count, Count >::mul( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator/( PointView const & lhs, PointView< U, Count > const & rhs )
		{
			return PtOperators< T, U, Count, Count >::div( lhs, rhs );
		}

		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator+( PointView const & lhs, U const * rhs )
		{
			return PtOperators< T, U, Count, Count >::add( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator-( PointView const & lhs, U const * rhs )
		{
			return PtOperators< T, U, Count, Count >::sub( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator*( PointView const & lhs, U const * rhs )
		{
			return PtOperators< T, U, Count, Count >::mul( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator/( PointView const & lhs, U const * rhs )
		{
			return PtOperators< T, U, Count, Count >::div( lhs, rhs );
		}

		friend Point< std::remove_cv_t< T >, Count > operator+( PointView const & lhs, T const & rhs )
		{
			return PtOperators< T, T, Count, Count >::add( lhs, rhs );
		}
		friend Point< std::remove_cv_t< T >, Count > operator-( PointView const & lhs, T const & rhs )
		{
			return PtOperators< T, T, Count, Count >::sub( lhs, rhs );
		}
		friend Point< std::remove_cv_t< T >, Count > operator*( PointView const & lhs, T const & rhs )
		{
			return PtOperators< T, T, Count, Count >::mul( lhs, rhs );
		}
		friend Point< std::remove_cv_t< T >, Count > operator/( PointView const & lhs, T const & rhs )
		{
			return PtOperators< T, T, Count, Count >::div( lhs, rhs );
		}

		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator+( PointView const & lhs, Point< U, Count > const & rhs )
		{
			return PtOperators< T, T, Count, Count >::add( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator-( PointView const & lhs, Point< U, Count > const & rhs )
		{
			return PtOperators< T, T, Count, Count >::sub( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator*( PointView const & lhs, Point< U, Count > const & rhs )
		{
			return PtOperators< T, T, Count, Count >::mul( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< T >, Count > operator/( PointView const & lhs, Point< U, Count > const & rhs )
		{
			return PtOperators< T, T, Count, Count >::div( lhs, rhs );
		}

		template< typename U >
		friend Point< std::remove_cv_t< U >, Count > operator+( Point< U, Count > const & lhs, PointView const & rhs )
		{
			return PtOperators< U, T, Count, Count >::add( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< U >, Count > operator-( Point< U, Count > const & lhs, PointView const & rhs )
		{
			return PtOperators< U, T, Count, Count >::sub( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< U >, Count > operator*( Point< U, Count > const & lhs, PointView const & rhs )
		{
			return PtOperators< U, T, Count, Count >::mul( lhs, rhs );
		}
		template< typename U >
		friend Point< std::remove_cv_t< U >, Count > operator/( Point< U, Count > const & lhs, PointView const & rhs )
		{
			return PtOperators< U, T, Count, Count >::div( lhs, rhs );
		}
		/**@}*/
		/**
		 *\~english
		 *name Stream operators.
		 *\~french
		 *name Opérateurs de flux.
		**/
		/**@{*/
		friend String & operator<<( String & out, PointView const & in )
		{
			StringStream stream{ makeStringStream() };
			stream << in;
			out += stream.str();
			return out;
		}

		friend String & operator>>( String & in, PointView & out )
		{
			StringStream stream( in );
			stream >> out;
			in = stream.str();
			return in;
		}

		template< typename CharT >
		friend std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & out, PointView const & in )
		{
			if constexpr ( Count != 0 )
			{
				out << in[0];
				for ( uint32_t i = 1; i < Count; i++ )
					out << " " << in[i];
			}

			return out;
		}

		template< typename CharT >
		friend std::basic_istream< CharT > & operator>>( std::basic_istream< CharT > & in, PointView & out )
		{
			for ( uint32_t i = 0; i < Count; i++ )
				in >> out[i];
			return in;
		}
		/**@}*/
	};
	/**
	\~english
	\brief		Point helper functions
	\~french
	\brief		Fonctions d'aide pour les Point
	*/
	namespace point
	{
		/**
		 *\~english
		 *\brief		Negates every value of a point's data
		 *\param[in]	point	The point
		 *\~french
		 *\brief		Négative chaque donnée du point donné
		 *\param[in]	point	Le point
		 */
		template< typename T, uint32_t Count >
		inline void negate( PointView< T, Count > & point );
		/**
		 *\~english
		 *\brief		Normalises the point
		 *\remarks		A point is normalised when it's length equals 1.0
						<br />So each data is divided by the point's length
		 *\param[in]	point	The point
		 *\~french
		 *\brief		Normalise le point
		 *\remarks		Un point est normalisé quand sa longueur vaut 1.0
						<br />Chaque donnée est donc divisée par la longueur du point
		 *\param[in]	point	Le point
		 */
		template< typename T, uint32_t Count >
		inline void normalise( PointView< T, Count > & point );
		/**
		 *\~english
		 *\brief		Computes the scalar product of 2 points
		 *\param[in]	lhs, rhs	The points
		 *\return		The scalar product
		 *\~french
		 *\brief		Calcule le produit scalaire entre 2 points
		 *\param[in]	lhs, rhs	Les points
		 *\return		Le produit scalaire
		 */
		template< typename T, uint32_t Count >
		inline T dot( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs );
		/**
		 *\~english
		 *\brief		Cross product operator
		 *\param[in]	lhs, rhs	The points to multiply
		 *\return		The cross product
		 *\~french
		 *\brief		Opérateur de produit vectoriel
		 *\param[in]	lhs, rhs	Les points à multiplier
		 *\return		Le résultat du produit vectoriel
		 */
		template< typename T, typename U >
		inline Point< T, 3 > cross( PointView< T, 3 > const & lhs, PointView< U, 3 > const & rhs );
		/**
		 *\~english
		 *\brief		Computes the trigonometric cosine of the angle between 2 points
		 *\param[in]	lhs, rhs	The points
		 *\return		The cosine
		 *\~french
		 *\brief		Calcule le cosinus trigonométrique de l'angle entre 2 points
		 *\param[in]	lhs, rhs	Les points
		 *\return		Le cosinus
		 */
		template< typename T, uint32_t Count >
		inline double cosTheta( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs );
		/**
		 *\~english
		 *\brief		Computes the squared Euclidian length of the vector
		 *\param[in]	point	The point
		 *\return		The squared Euclidian distance
		 *\~french
		 *\brief		Calcule le carré de la longueur Euclidienne du vecteur
		 *\param[in]	point	Le point
		 *\return		Le carré de la norme Euclidienne
		 */
		template< typename T, uint32_t Count >
		inline double lengthSquared( PointView< T, Count > const & point );
		/**
		 *\~english
		 *\brief		Computes the Euclidian length of the vector
		 *\param[in]	point	The point
		 *\return		The Euclidian distance
		 *\~french
		 *\brief		Calcule la longueur Euclidienne du vecteur
		 *\param[in]	point	Le point
		 *\return		La norme Euclidienne
		 */
		template< typename T, uint32_t Count >
		inline double length( PointView< T, Count > const & point );
		/**
		 *\~english
		 *\brief		Computes the Manhattan length of the vector
		 *\param[in]	point	The point
		 *\return		The Manhattan distance
		 *\~french
		 *\brief		Calcule la longueur de Manhattan du vecteur
		 *\param[in]	point	Le point
		 *\return		La distance de Manhattan
		 */
		template< typename T, uint32_t Count >
		inline double lengthManhattan( PointView< T, Count > const & point );
		/**
		 *\~english
		 *\brief		Computes the Minkowski length of the vector
		 *\param[in]	point	The point
		 *\param[in]	order	The Minkowski order
		 *\return		The Minkowski distance
		 *\~french
		 *\brief		Calcule la longueur de Minkowski du vecteur
		 *\param[in]	point	Le point
		 *\param[in]	order	L'ordre Minkownski
		 *\return		La distance de Minkowski
		 */
		template< typename T, uint32_t Count >
		inline double lengthMinkowski( PointView< T, Count > const & point, double order );
		/**
		 *\~english
		 *\brief		Computes the Chebychev length of the vector
		 *\param[in]	point	The point
		 *\return		The Chebychev distance
		 *\~french
		 *\brief		Calcule la longueur de Chebychev du vecteur
		 *\param[in]	point	Le point
		 *\return		La distance de Chebychev
		 */
		template< typename T, uint32_t Count >
		inline double lengthChebychev( PointView< T, Count > const & point );
		/**
		 *\~english
		 *\brief		Computes the squared Euclidian distance between two points.
		 *\param[in]	lhs, rhs	The points.
		 *\return		The squared Euclidian distance.
		 *\~french
		 *\brief		Calcule le carré de la distance Euclidienne entre deux points.
		 *\param[in]	lhs, rhs	Les points.
		 *\return		Le carré de la norme Euclidienne.
		 */
		template< typename T, uint32_t Count >
		inline double distanceSquared( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs );
		/**
		 *\~english
		 *\brief		Computes the Euclidian distance between two points.
		 *\param[in]	lhs, rhs	The points.
		 *\return		The Euclidian distance.
		 *\~french
		 *\brief		Calcule la norme Euclidienne entre deux points.
		 *\param[in]	lhs, rhs	Les points.
		 *\return		La norme Euclidienne.
		 */
		template< typename T, uint32_t Count >
		inline double distance( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs );
		/**
		 *\~english
		 *\brief		Computes the Manhattan distance between two points.
		 *\param[in]	lhs, rhs	The points.
		 *\return		The Manhattan distance.
		 *\~french
		 *\brief		Calcule la distance de Manhattan entre deux points.
		 *\param[in]	lhs, rhs	Les points.
		 *\return		La distance de Manhattan.
		 */
		template< typename T, uint32_t Count >
		inline double distanceManhattan( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs );
		/**
		 *\~english
		 *\brief		Computes the Minkowski distance between two points.
		 *\param[in]	lhs, rhs	The points.
		 *\param[in]	order	The Minkowski order.
		 *\return		The Minkowski distance.
		 *\~french
		 *\brief		Calcule la distance de Minkowski entre deux points.
		 *\param[in]	lhs, rhs	Les points.
		 *\param[in]	order	L'ordre Minkownski.
		 *\return		La distance de Minkowski.
		 */
		template< typename T, uint32_t Count >
		inline double distanceMinkowski( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs, double order );
		/**
		 *\~english
		 *\brief		Computes the Chebychev distance between two points.
		 *\param[in]	lhs, rhs	The points.
		 *\return		The Chebychev distance.
		 *\~french
		 *\brief		Calcule la distance de Chebychev entre deux points.
		 *\param[in]	lhs, rhs	Les points.
		 *\return		La distance de Chebychev.
		 */
		template< typename T, uint32_t Count >
		inline double distanceChebychev( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs );
	}
}

#include "PointView.inl"

#endif
