/*
See LICENSE file in root folder
*/
#ifndef ___CU_Point_H___
#define ___CU_Point_H___

#include "CastorUtils/Data/TextLoader.hpp"
#include "CastorUtils/Data/TextWriter.hpp"
#include "CastorUtils/Align/Aligned.hpp"
#include "CastorUtils/Math/PointData.hpp"

#include <cmath>

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <iostream>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	template< typename T, uint32_t TCount >
	class Point
	{
	private:
		template< typename U, uint32_t UCount > friend class Point;

		using value_type = T;
		using reference = value_type &;
		using pointer = value_type *;
		using const_reference = value_type const &;
		using const_pointer = value_type const *;
		using point_reference = Point< value_type, TCount > &;
		using point_pointer = Point< value_type, TCount > *;
		using const_point_reference = Point< value_type, TCount > const &;
		using const_point_pointer = Point< value_type, TCount > const *;
		static constexpr std::size_t binary_size = sizeof( T ) * TCount;

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
		 *\name Construction / Destruction.
		 *\~french
		 *\name Construction / Destruction.
		 **/
		/**@{*/
		constexpr Point()noexcept;
		constexpr ~Point()noexcept = default;
		constexpr Point( Point const & rhs )noexcept;
		constexpr Point( Point && rhs )noexcept;
		explicit Point( T const * rhs )noexcept;
		template< typename U, uint32_t UCount >
		explicit Point( Point< U, UCount > const & rhs )noexcept;
		template< typename U, uint32_t UCount >
		explicit Point( Coords< U, UCount > const & rhs )noexcept;
		template< typename U >
		explicit Point( U const * rhs )noexcept;
		template< typename ValueA >
		explicit constexpr Point( ValueA a )noexcept;
		template< typename ValueA, typename ValueB >
		constexpr Point( ValueA a, ValueB b )noexcept;
		template< typename ValueA, typename ValueB, typename ValueC >
		constexpr Point( ValueA a, ValueB b, ValueC c )noexcept;
		template< typename ValueA, typename ValueB, typename ValueC, typename ValueD >
		constexpr Point( ValueA a, ValueB b, ValueC c, ValueD d )noexcept;
		/**@}*/
		/**
		 *\~english
		 *\name Assignment operators.
		 *\~french
		 *\name Opérateurs d'affectation.
		 **/
		/**@{*/
		constexpr Point & operator=( Point const & rhs )noexcept;
		constexpr Point & operator=( Point && rhs )noexcept;
		/**@}*/
		/**
		 *\~english
		 *\name Arithmetic operators.
		 *\~french
		 *\name Opérateurs arithmétiques.
		**/
		/**@{*/
		template< typename U, uint32_t UCount >
		Point & operator+=( Point< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Point & operator-=( Point< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Point & operator*=( Point< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Point & operator/=( Point< U, UCount > const & rhs );

		template< typename U, uint32_t UCount >
		Point & operator+=( Coords< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Point & operator-=( Coords< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Point & operator*=( Coords< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Point & operator/=( Coords< U, UCount > const & rhs );

		template< typename U >
		Point & operator+=( U const * rhs );
		template< typename U >
		Point & operator-=( U const * rhs );
		template< typename U >
		Point & operator*=( U const * rhs );
		template< typename U >
		Point & operator/=( U const * rhs );

		Point & operator+=( T const & rhs );
		Point & operator-=( T const & rhs );
		Point & operator*=( T const & rhs );
		Point & operator/=( T const & rhs );
		/**@}*/
		/**
		 *\~english
		 *\brief		Swaps this pont data with the parameter ones
		 *\param[in]	rhs	The point to swap
		 *\~french
		 *\brief		Echange les données de c epoint avec celles du point donné
		 *\param[in]	rhs	Le point à échanger
		 */
		void swap( Point & rhs )noexcept;
		/**
		 *\~english
		 *\brief		Retrieves the total size of the point
		 *\return		count * elemSize
		 *\~french
		 *\brief		Récupère la taille totale du point
		 *\return		count * elemSize
		 */
		std::size_t	size()const
		{
			return binary_size;
		}
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Le pointeur
		 */
		T * ptr()
		{
			return m_data.coords.data();
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
			return m_data.coords.data();
		}
		/**
		 *\~english
		 *name Array access.
		 *\~french
		 *name Accesseurs de tableau.
		**/
		/**@{*/
		T const & operator[]( uint32_t index )const
		{
			return m_data.coords[index];
		}

		T & operator[]( uint32_t index )
		{
			return m_data.coords[index];
		}

		iterator begin()
		{
			return &m_data.coords[0];
		}

		const_iterator begin()const
		{
			return &m_data.coords[0];
		}

		const_iterator end()const
		{
			return &m_data.coords[0] + TCount;
		}

		iterator end()
		{
			return &m_data.coords[0] + TCount;
		}

		PointData< T, TCount > * operator->()
		{
			return &m_data.data;
		}

		PointData< T, TCount > const * operator->()const
		{
			return &m_data.data;
		}
		/**@}*/

	private:
		union
		{
			Array< T, TCount > coords;
			PointData< T, TCount > data;
		} m_data{};
	};
	/**
	 *\~english
	 *name Logic operators.
	 *\~french
	 *name Opérateurs logiques
	**/
	/**@{*/
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	bool operator==( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	bool operator!=( Point< T, TCount > const & lhs, Point< U, UCount >	const & rhs );

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	bool operator==( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	bool operator!=( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs );

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	bool operator==( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	bool operator!=( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	/**@}*/
	/**
	 *\~english
	 *name Arithmetic operators.
	 *\~french
	 *name Opérateurs arithmétiques.
	**/
	/**@{*/
	template< typename T, uint32_t TCount >
	Point< std::remove_cv_t< T >, TCount > operator-( Point< T, TCount > const & rhs );

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator+( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator-( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator*( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator/( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs );

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator+( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator-( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator*( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator/( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs );

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator+( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator-( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator*( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< std::remove_cv_t< T >, TCount > operator/( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs );

	template< typename T, uint32_t TCount, typename U >
	Point< std::remove_cv_t< T >, TCount > operator+( Point< T, TCount > const & lhs, U const * rhs );
	template< typename T, uint32_t TCount, typename U >
	Point< std::remove_cv_t< T >, TCount > operator-( Point< T, TCount > const & lhs, U const * rhs );
	template< typename T, uint32_t TCount, typename U >
	Point< std::remove_cv_t< T >, TCount > operator*( Point< T, TCount > const & lhs, U const * rhs );
	template< typename T, uint32_t TCount, typename U >
	Point< std::remove_cv_t< T >, TCount > operator/( Point< T, TCount > const & lhs, U const * rhs );

	template< typename T, uint32_t TCount >
	Point< std::remove_cv_t< T >, TCount > operator+( Point< T, TCount > const & lhs, T const & rhs );
	template< typename T, uint32_t TCount >
	Point< std::remove_cv_t< T >, TCount > operator-( Point< T, TCount > const & lhs, T const & rhs );
	template< typename T, uint32_t TCount >
	Point< std::remove_cv_t< T >, TCount > operator*( Point< T, TCount > const & lhs, T const & rhs );
	template< typename T, uint32_t TCount >
	Point< std::remove_cv_t< T >, TCount > operator/( Point< T, TCount > const & lhs, T const & rhs );

	template< typename DstCompT, typename SrcT >
	PointTypeT< SrcT, DstCompT > pointCast( SrcT const & src );
	/**@}*/
	/**
	\~english
	\brief		Point helper functions
	\~french
	\brief		Fonctions d'aide pour les Point
	*/
	namespace point
	{
		template< typename CoordT, uint32_t CountT >
		Point< CoordT, CountT > const & getPoint( Point< CoordT, CountT > const & lhs );
		template< typename CoordT, uint32_t CountT >
		Point< CoordT, CountT > const & getPoint( LengthT< Point< CoordT, CountT > > const & lhs );
		template< typename CoordT, typename CoordU, uint32_t CountT >
		void setPoint( Point< CoordT, CountT > & lhs
			, Point< CoordU, CountT > const & rhs );
		template< typename CoordT, typename CoordU, uint32_t CountT >
		void setPoint( LengthT< Point< CoordT, CountT > > & lhs
			, Point< CoordU, CountT > const & rhs );
		template< typename CoordT, uint32_t CountT >
		void setCoord( Point< CoordT, CountT > & pt
			, uint32_t i
			, CoordT const & c );
		template< typename CoordT, uint32_t CountT >
		void setCoord( Coords< CoordT, CountT > & pt
			, uint32_t i
			, CoordT const & c );
		template< typename CoordT, uint32_t CountT >
		void setCoord( LengthT< Point< CoordT, CountT > > & pt
			, uint32_t i
			, CoordT const & c );
		template< typename CoordT, uint32_t CountT >
		CoordT const & getCoord( Point< CoordT, CountT > const & pt
			, uint32_t i );
		template< typename CoordT, uint32_t CountT >
		CoordT const & getCoord( Coords< CoordT, CountT > const & pt
			, uint32_t i );
		template< typename CoordT, uint32_t CountT >
		CoordT const & getCoord( LengthT< Point< CoordT, CountT > > const & pt
			, uint32_t i );
		/**
		 *\~english
		 *\brief		Floors every value of a point's data.
		 *\param[in]	point	The point.
		 *\~french
		 *\brief		Arrondit à l'inférieur chaque donnée du point donné.
		 *\param[in]	point	Le point.
		 */
		template< typename T, uint32_t TCount >
		static void floor( Point< T, TCount > & point );
		/**
		 *\~english
		 *\brief		Floors every value of a point's data.
		 *\param[in]	point	The point.
		 *\return		The rounded point.
		 *\~french
		 *\brief		Arrondit à l'inférieur chaque donnée du point donné.
		 *\param[in]	point	Le point.
		 *\return		Le point arrondi.
		 */
		template< typename T, uint32_t TCount >
		static Point< T, TCount > getFloored( Point< T, TCount > const & point );
		/**
		 *\~english
		 *\brief		Ceils every value of a point's data.
		 *\param[in]	point	The point.
		 *\~french
		 *\brief		Arrondit au supérieur chaque donnée du point donné.
		 *\param[in]	point	Le point.
		 */
		template< typename T, uint32_t TCount >
		static void ceil( Point< T, TCount > & point );
		/**
		 *\~english
		 *\brief		Ceils every value of a point's data.
		 *\param[in]	point	The point.
		 *\return		The rounded point.
		 *\~french
		 *\brief		Arrondit au supérieur chaque donnée du point donné.
		 *\param[in]	point	Le point.
		 *\return		Le point arrondi.
		 */
		template< typename T, uint32_t TCount >
		static Point< T, TCount > getCeiled( Point< T, TCount > const & point );
		/**
		 *\~english
		 *\brief		Rounds every value of a point's data.
		 *\param[in]	point	The point.
		 *\~french
		 *\brief		Arrondit chaque donnée du point donné.
		 *\param[in]	point	Le point.
		 */
		template< typename T, uint32_t TCount >
		static void round( Point< T, TCount > & point );
		/**
		 *\~english
		 *\brief		Rounds every value of a point's data.
		 *\param[in]	point	The point.
		 *\return		The rounded point.
		 *\~french
		 *\brief		Arrondit chaque donnée du point donné.
		 *\param[in]	point	Le point.
		 *\return		Le point arrondi.
		 */
		template< typename T, uint32_t TCount >
		static Point< T, TCount > getRounded( Point< T, TCount > const & point );
		/**
		 *\~english
		 *\brief		Negates every value of a point's data
		 *\param[in]	point	The point
		 *\~french
		 *\brief		Négative chaque donnée du point donné
		 *\param[in]	point	Le point
		 */
		template< typename T, uint32_t TCount >
		static void negate( Point< T, TCount > & point );
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
		template< typename T, uint32_t TCount >
		static void normalise( Point< T, TCount > & point );
		/**
		 *\~english
		 *\brief		Returns the normalised form of a point
		 *\param[in]	point	The point
		 *\~french
		 *\brief		Retourne la forme normalisée d'un point
		 *\param[in]	point	Le point
		 */
		template< typename T, uint32_t TCount >
		static Point< T, TCount > getNormalised( Point< T, TCount > const & point );
		/**
		 *\~english
		 *\brief		Computes the dot product of 2 points
		 *\param[in]	lhs, rhs	The points
		 *\return		The scalar product
		 *\~french
		 *\brief		Calcule le produit scalaire entre 2 points
		 *\param[in]	lhs, rhs	Les points
		 *\return		Le produit scalaire
		 */
		template< typename T, typename U, uint32_t TCount >
		static T dot( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs );
		/**
		 *\~english
		 *\brief		Computes the cross product of 2 points.
		 *\param[in]	lhs, rhs	The points.
		 *\return		The resulting vector.
		 *\~french
		 *\brief		Calcule le produit vectoriel entre 2 points.
		 *\param[in]	lhs, rhs	Les points.
		 *\return		Le vecteur résultant.
		 */
		template< Vector3T LhsT, Vector3T RhsT >
		static LhsT cross( LhsT const & lhs, RhsT const & rhs );
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
		template< typename T, uint32_t TCount >
		static double cosTheta( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs );
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
		template< typename T, uint32_t TCount >
		static double lengthSquared( Point< T, TCount > const & point );
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
		template< typename T, uint32_t TCount >
		static double length( Point< T, TCount > const & point );
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
		template< typename T, uint32_t TCount >
		static double lengthManhattan( Point< T, TCount > const & point );
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
		template< typename T, uint32_t TCount >
		static double lengthMinkowski( Point< T, TCount > const & point, double order );
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
		template< typename T, uint32_t TCount >
		static double lengthChebychev( Point< T, TCount > const & point );
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
		template< typename T, uint32_t TCount >
		static double distanceSquared( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs );
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
		template< typename T, uint32_t TCount >
		static double distance( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs );
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
		template< typename T, uint32_t TCount >
		static double distanceManhattan( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs );
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
		template< typename T, uint32_t TCount >
		static double distanceMinkowski( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs, double order );
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
		template< typename T, uint32_t TCount >
		static double distanceChebychev( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs );
		/**
		 *\~english
		 *\brief		Computes the dot product of 2 points
		 *\param[in]	lhs, rhs	The points
		 *\return		The scalar product
		 *\~french
		 *\brief		Calcule le produit scalaire entre 2 points
		 *\param[in]	lhs, rhs	Les points
		 *\return		Le produit scalaire
		 */
		template< typename T, typename U, uint32_t TCount >
		static T dot( Coords< T, TCount > const & lhs, Point< T, TCount > const & rhs );
		/**
		 *\~english
		 *\brief		Computes the cross product of 2 points.
		 *\param[in]	lhs, rhs	The points.
		 *\return		The resulting vector.
		 *\~french
		 *\brief		Calcule le produit vectoriel entre 2 points.
		 *\param[in]	lhs, rhs	Les points.
		 *\return		Le vecteur résultant.
		 */
		template< typename T, typename U >
		static Point3< T > cross( Coords3< T > const & lhs, Point3< T > const & rhs );
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
		template< typename T, uint32_t TCount >
		static double cosTheta( Coords< T, TCount > const & lhs, Point< T, TCount > const & rhs );
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
		template< typename T, typename U, uint32_t TCount >
		static T dot( Point< T, TCount > const & lhs, Coords< T, TCount > const & rhs );
		/**
		 *\~english
		 *\brief		Computes the cross product of 2 points.
		 *\param[in]	lhs, rhs	The points.
		 *\return		The resulting vector.
		 *\~french
		 *\brief		Calcule le produit vectoriel entre 2 points.
		 *\param[in]	lhs, rhs	Les points.
		 *\return		Le vecteur résultant.
		 */
		template< typename T, typename U >
		static Point3< T > cross( Point3< T > const & lhs, Coords3< T > const & rhs );
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
		template< typename T, uint32_t TCount >
		static double cosTheta( Point< T, TCount > const & lhs, Coords< T, TCount > const & rhs );
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
		template< typename T, typename U, uint32_t TCount >
		static T dot( Coords< T const, TCount > const & lhs, Point< T, TCount > const & rhs );
		/**
		 *\~english
		 *\brief		Computes the cross product of 2 points.
		 *\param[in]	lhs, rhs	The points.
		 *\return		The resulting vector.
		 *\~french
		 *\brief		Calcule le produit vectoriel entre 2 points.
		 *\param[in]	lhs, rhs	Les points.
		 *\return		Le vecteur résultant.
		 */
		template< typename T, typename U >
		static Point3< T > cross( Coords< T const, 3 > const & lhs, Point3< T > const & rhs );
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
		template< typename T, uint32_t TCount >
		static double cosTheta( Coords< T const, TCount > const & lhs, Point< T, TCount > const & rhs );
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
		template< typename T, typename U, uint32_t TCount >
		static T dot( Point< T, TCount > const & lhs, Coords< T const, TCount > const & rhs );
		/**
		 *\~english
		 *\brief		Computes the cross product of 2 points.
		 *\param[in]	lhs, rhs	The points.
		 *\return		The resulting vector.
		 *\~french
		 *\brief		Calcule le produit vectoriel entre 2 points.
		 *\param[in]	lhs, rhs	Les points.
		 *\return		Le vecteur résultant.
		 */
		template< typename T, typename U >
		static Point3< T > cross( Point3< T > const & lhs, Coords< T const, 3 > const & rhs );
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
		template< typename T, uint32_t TCount >
		static double cosTheta( Point< T, TCount > const & lhs, Coords< T const, TCount > const & rhs );
	}
	/**
	 *\~english
	 *name Stream operators.
	 *\~french
	 *name Opérateurs de flux.
	**/
	/**@{*/
	template< typename T, uint32_t TCount >
	String & operator<<( String & out, Point< T, TCount > const & in );
	template< typename T, uint32_t TCount >
	String & operator>>( String & in, Point< T, TCount > & out );
	template< typename T, uint32_t TCount, typename CharType >
	std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & out, Point< T, TCount > const & in );
	template< typename T, uint32_t TCount, typename CharType >
	std::basic_istream< CharType > & operator>>( std::basic_istream< CharType > & in, Point< T, TCount > & out );
	/**@}*/
}
#include "Point.inl"

#endif
