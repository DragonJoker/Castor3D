/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_POINT_H___
#define ___CASTOR_POINT_H___

#include "Data/TextLoader.hpp"
#include "Data/TextWriter.hpp"
#include "Align/Aligned.hpp"

#include <cmath>
#include <iostream>

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Templated static dimensions point representation
	\remark		Can hold any type which has a defined Policy
	\~french
	\brief		Classe de points à dimensions statiques
	\remark		Peut recevoir les types de données qui ont une Policy créée
	*/
	template< typename T, uint32_t TCount >
	class Point
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		\~english
		\brief Coords< T, TCount > loader
		\~french
		\brief Loader de Coords< T, TCount >
		*/
		class TextLoader
			: public castor::TextLoader< Point >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			inline TextLoader();
			/**
			 *\~english
			 *\brief		Loads an object from a text file
			 *\param[out]	object	The object to read
			 *\param[in]	file	The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Charge un objet à partir d'un fichier texte
			 *\param[out]	object	L'objet à lire
			 *\param[in]	file	Le fichier
			 *\return		\p true si ok
			 */
			inline bool operator()( Point & object, TextFile & file )override;
		};
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		\~english
		\brief Coords< T, TCount > Writer
		\~french
		\brief Writer de Coords< T, TCount >
		*/
		class TextWriter
			: public castor::TextWriter< Point >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			inline explicit TextWriter( String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a Point2f object into a text file
			 *\param[in]	object	The Point2f object to write
			 *\param[out]	file	The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Ecrit un objet Point2f dans un fichier texte
			 *\param[in]	object	L'objet Point2f
			 *\param[out]	file	Le fichier
			 *\return		\p true si ok
			 */
			inline bool operator()( Point const & object, TextFile & file )override;
		};

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
		 *name Construction / Destruction.
		 *\~french
		 *name Construction / Destruction.
		 **/
		/**@{*/
		Point();
		Point( Point const & rhs );
		Point( Point && rhs );
		explicit Point( T const * rhs );
		template< typename U, uint32_t UCount >
		explicit Point( Point< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		explicit Point( Coords< U, UCount > const & rhs );
		template< typename U >
		explicit Point( U const * rhs );
		template< typename ValueA, typename ValueB, typename ... Values >
		explicit Point( ValueA a, ValueB b, Values ... values );
		/**@}*/
		/**
		 *\~english
		 *name Assignment operators.
		 *\~french
		 *name Opérateurs d'affectation.
		 **/
		/**@{*/
		Point & operator=( Point const & rhs );
		Point & operator=( Point && rhs );
		/**@}*/
		/**
		 *\~english
		 *name Arithmetic operators.
		 *\~french
		 *name Opérateurs arithmétiques.
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
		void swap( Point & rhs );
		/**
		 *\~english
		 *\brief		Inverts data in the point
		 *\remarks		The first becomes last and so on
		 *\~french
		 *\brief		Inverse les données dans ce point
		 *\remarks		Les premiers deviennent les derniers
		 */
		void flip();
		/**
		 *\~english
		 *\brief			Retrieves data from the point and put it into an array
		 *\param[in,out]	result	Receives the point data, needs to be allocated by the caller
		 *\~french
		 *\brief			Récupère les données et les place dans un tableau
		 *\param[in,out]	result	Reçoit les données, doit être alloué par l'appelant
		 */
		void toValues( T * result )const;
		/**
		 *\~english
		 *\brief		Retrieves the number of coordinates
		 *\return		The number of coordinates
		 *\~french
		 *\brief		Récupère le nombre de coordonnées
		 *\return		Le nombre de coordonnées
		 */
		inline uint32_t count()const
		{
			return TCount;
		}
		/**
		 *\~english
		 *\brief		Retrieves the coordinate type size
		 *\return		The data type size
		 *\~french
		 *\brief		Récupère la taille du type données
		 *\return		La taille du type de données
		 */
		inline std::size_t elemSize()const
		{
			return sizeof( T );
		}
		/**
		 *\~english
		 *\brief		Retrieves the total size of the point
		 *\return		count() * elemSize()
		 *\~french
		 *\brief		Récupère la taille totale du point
		 *\return		count() * elemSize()
		 */
		inline	std::size_t	size()const
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
		inline T * ptr()
		{
			return m_coords.data();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur constant sur les données
		 *\return		Le pointeur
		 */
		inline T const * constPtr()const
		{
			return m_coords.data();
		}
		/**
		 *\~english
		 *name Array access.
		 *\~french
		 *name Accesseurs de tableau.
		**/
		/**@{*/
		inline T const & operator[]( uint32_t index )const
		{
			return m_coords[index];
		}

		inline T & operator[]( uint32_t index )
		{
			return m_coords[index];
		}

		inline iterator begin()
		{
			return &m_coords[0];
		}

		inline const_iterator begin()const
		{
			return &m_coords[0];
		}

		inline const_iterator end()const
		{
			return &m_coords[0] + TCount;
		}

		inline iterator end()
		{
			return &m_coords[0] + TCount;
		}

		T const & at( uint32_t index )const;
		T & at( uint32_t index );
		/**@}*/

	private:
		std::array< T, TCount > m_coords;
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
	Point< typename std::remove_cv< T >::type, TCount > operator-( Point< T, TCount > const & rhs );

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator+( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator-( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator*( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator/( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs );

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator+( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator-( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator*( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator/( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs );

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator+( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator-( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator*( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs );
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, TCount > operator/( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs );

	template< typename T, uint32_t TCount, typename U >
	Point< typename std::remove_cv< T >::type, TCount > operator+( Point< T, TCount > const & lhs, U const * rhs );
	template< typename T, uint32_t TCount, typename U >
	Point< typename std::remove_cv< T >::type, TCount > operator-( Point< T, TCount > const & lhs, U const * rhs );
	template< typename T, uint32_t TCount, typename U >
	Point< typename std::remove_cv< T >::type, TCount > operator*( Point< T, TCount > const & lhs, U const * rhs );
	template< typename T, uint32_t TCount, typename U >
	Point< typename std::remove_cv< T >::type, TCount > operator/( Point< T, TCount > const & lhs, U const * rhs );

	template< typename T, uint32_t TCount >
	Point< typename std::remove_cv< T >::type, TCount > operator+( Point< T, TCount > const & lhs, T const & rhs );
	template< typename T, uint32_t TCount >
	Point< typename std::remove_cv< T >::type, TCount > operator-( Point< T, TCount > const & lhs, T const & rhs );
	template< typename T, uint32_t TCount >
	Point< typename std::remove_cv< T >::type, TCount > operator*( Point< T, TCount > const & lhs, T const & rhs );
	template< typename T, uint32_t TCount >
	Point< typename std::remove_cv< T >::type, TCount > operator/( Point< T, TCount > const & lhs, T const & rhs );
	/**@}*/
	/*!
	\author 	Sylvain DOREMUS
	\date 		10/07/2012
	\version	0.7.0
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
		template< typename T, typename U >
		static Point3< T > cross( Point3< T > const & lhs, Point3< T > const & rhs );
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
}
/**
 *\~english
 *name Stream operators.
 *\~french
 *name Opérateurs de flux.
**/
/**@{*/
template< typename T, uint32_t TCount >
castor::String & operator<<( castor::String & out, castor::Point< T, TCount > const & in );
template< typename T, uint32_t TCount >
castor::String & operator>>( castor::String & in, castor::Point< T, TCount > & out );
template< typename T, uint32_t TCount, typename CharType >
std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & out, castor::Point< T, TCount > const & in );
template< typename T, uint32_t TCount, typename CharType >
std::basic_istream< CharType > & operator>>( std::basic_istream< CharType > & in, castor::Point< T, TCount > & out );

#include "Point.inl"

#endif
