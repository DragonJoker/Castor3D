/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_COORDS_H___
#define ___CASTOR_COORDS_H___

#include "Data/TextLoader.hpp"
#include "Data/TextWriter.hpp"

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
	template< typename T, uint32_t Count >
	class Coords
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		\~english
		\brief Coords loader
		\~french
		\brief Loader de Coords
		*/
		class TextLoader
			: public castor::TextLoader< Coords >
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
			inline bool operator()( Coords & object, TextFile & file )override;
		};
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		\~english
		\brief Coords Writer
		\~french
		\brief Writer de Coords
		*/
		class TextWriter
			: public castor::TextWriter< Coords >
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
			 *\brief		Writes an object into a text file
			 *\param[in]	object	The object to write
			 *\param[out]	file	The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Ecrit un objet dans un fichier texte
			 *\param[in]	object	L'objet à écrire
			 *\param[out]	file	Le fichier
			 *\return		\p true si ok
			 */
			inline bool operator()( Coords const & object, TextFile & file )override;
		};

	private:
		template< typename U, uint32_t UCount > friend class Coords;

		using value_type = T;
		using reference = value_type &;
		using pointer = value_type * ;
		using const_reference = value_type const &;
		using const_pointer = value_type const *;
		using point_reference = Coords< value_type, Count > &;
		using point_pointer = Coords< value_type, Count > *;
		using const_point_reference = Coords< value_type, Count > const &;
		using const_point_pointer = Coords< value_type, Count > const *;
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
		Coords();
		Coords( Coords const & rhs );
		Coords( Coords && rhs );
		explicit Coords( T * rhs );
		~Coords();
		/**@}*/
		/**
		 *\~english
		 *name Assignment operators.
		 *\~french
		 *name Opérateurs d'affectation.
		 **/
		/**@{*/
		Coords & operator=( T * rhs );
		Coords & operator=( Coords const & rhs );
		Coords & operator=( Coords && rhs );
		Coords & operator=( Point< T, Count > const & rhs );
		/**@}*/
		/**
		 *\~english
		 *name Arithmetic operators.
		 *\~french
		 *name Opérateurs arithmétiques.
		**/
		/**@{*/
		template< typename U, uint32_t UCount >
		Coords & operator+=( Coords< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Coords & operator-=( Coords< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Coords & operator*=( Coords< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Coords & operator/=( Coords< U, UCount > const & rhs );

		template< typename U, uint32_t UCount >
		Coords & operator+=( Point< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Coords & operator-=( Point< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Coords & operator*=( Point< U, UCount > const & rhs );
		template< typename U, uint32_t UCount >
		Coords & operator/=( Point< U, UCount > const & rhs );

		template< typename U >
		Coords & operator+=( U const * p_coords );
		template< typename U >
		Coords & operator-=( U const * p_coords );
		template< typename U >
		Coords & operator*=( U const * p_coords );
		template< typename U >
		Coords & operator/=( U const * p_coords );

		Coords & operator+=( T const & p_coord );
		Coords & operator-=( T const & p_coord );
		Coords & operator*=( T const & p_coord );
		Coords & operator/=( T const & p_coord );
		/**@}*/
		/**
		 *\~english
		 *\brief		Swaps this pont data with the parameter ones
		 *\param[in]	rhs	The point to swap
		 *\~french
		 *\brief		Echange les données de c epoint avec celles du point donné
		 *\param[in]	rhs	Le point à échanger
		 */
		void swap( Coords & rhs );
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
		 *\return		The point total size: count() * elemSize().
		 *\~french
		 *\return		La taille totale du point (count() * elemSize()).
		 */
		inline uint32_t size()const
		{
			return binary_size;
		}
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
			return Count;
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
		 *\brief		Retrieves the data at given index
		 *\remarks		No check is made, if you make an index error, expect a crash
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remarks		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une référence constante sur la donnée à l'index voulu
		 */
		inline T const & operator[]( uint32_t index )const
		{
			return m_coords[index];
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
		inline T & operator[]( uint32_t index )
		{
			return m_coords[index];
		}
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remarks		This fonction checks the index and throws an exception if it is out of bounds
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remarks		Cette fonction vérifie l'index et lance une exception s'il est hors bornes
		 *\return		Une référence constante sur la donnée à l'index voulu
		 */
		T const & at( uint32_t index )const;
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remarks		This fonction checks the index and throws an exception if it is out of bounds
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remarks		Cette fonction vérifie l'index et lance une exception s'il est hors bornes
		 *\return		Une référence sur la donnée à l'index voulu
		 */
		T & at( uint32_t index );
		/**
		 *\~english
		 *\brief		Retrieves a non constant pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur non constant sur les données
		 *\return		Le pointeur
		 */
		inline T * ptr()
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
		inline T const * constPtr()const
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
		inline iterator begin()
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
		inline const_iterator begin()const
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
		inline iterator end()
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
		inline const_iterator end()const
		{
			return m_coords + Count;
		}

	private:
		T * m_coords;
	};
	/**@}*/
	/**
	 *\~english
	 *name Comparison operators.
	 *\~french
	 *name Opérateurs de comparaison.
	**/
	/**@{*/
	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	bool operator==( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs );
	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	bool operator!=( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs );
	/**@}*/
	/**
	 *\~english
	 *name Arithmetic operators.
	 *\~french
	 *name Opérateurs arithmétiques.
	**/
	/**@{*/
	template< typename T, uint32_t Count >
	Point< typename std::remove_cv< T >::type, Count > operator-( Coords< T, Count > const & rhs );

	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, Count > operator+( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs );
	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, Count > operator-( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs );
	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, Count > operator*( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs );
	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	Point< typename std::remove_cv< T >::type, Count > operator/( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs );

	template< typename T, uint32_t Count, typename U >
	Point< typename std::remove_cv< T >::type, Count > operator+( Coords< T, Count > const & lhs, U const * rhs );
	template< typename T, uint32_t Count, typename U >
	Point< typename std::remove_cv< T >::type, Count > operator-( Coords< T, Count > const & lhs, U const * rhs );
	template< typename T, uint32_t Count, typename U >
	Point< typename std::remove_cv< T >::type, Count > operator*( Coords< T, Count > const & lhs, U const * rhs );
	template< typename T, uint32_t Count, typename U >
	Point< typename std::remove_cv< T >::type, Count > operator/( Coords< T, Count > const & lhs, U const * rhs );

	template< typename T, uint32_t Count, typename U >
	Point< typename std::remove_cv< T >::type, Count > operator+( Coords< T, Count > const & lhs, T const & rhs );
	template< typename T, uint32_t Count, typename U >
	Point< typename std::remove_cv< T >::type, Count > operator-( Coords< T, Count > const & lhs, T const & rhs );
	template< typename T, uint32_t Count >
	Point< typename std::remove_cv< T >::type, Count > operator*( Coords< T, Count > const & lhs, T const & rhs );
	template< typename T, uint32_t Count >
	Point< typename std::remove_cv< T >::type, Count > operator/( Coords< T, Count > const & lhs, T const & rhs );
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
		template< typename T, uint32_t Count >
		inline void negate( Coords< T, Count > & point );
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
		inline void normalise( Coords< T, Count > & point );
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
		inline T dot( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs );
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
		inline Point< T, 3 > cross( Coords3< T > const & lhs, Coords< U, 3 > const & rhs );
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
		inline double cosTheta( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs );
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
		inline double lengthSquared( Coords< T, Count > const & point );
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
		inline double length( Coords< T, Count > const & point );
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
		inline double lengthManhattan( Coords< T, Count > const & point );
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
		inline double lengthMinkowski( Coords< T, Count > const & point, double order );
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
		inline double lengthChebychev( Coords< T, Count > const & point );
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
		inline double distanceSquared( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs );
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
		inline double distance( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs );
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
		inline double distanceManhattan( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs );
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
		inline double distanceMinkowski( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs, double order );
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
		inline double distanceChebychev( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs );
	}
}
/**
 *\~english
 *name Stream operators.
 *\~french
 *name Opérateurs de flux.
**/
/**@{*/
template< typename T, uint32_t Count >
castor::String & operator<<( castor::String & out, castor::Coords< T, Count > const & in );
template< typename T, uint32_t Count >
castor::String & operator>>( castor::String & in, castor::Coords< T, Count > & out );
template< typename T, uint32_t Count, typename CharType >
std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & out, castor::Coords< T, Count > const & in );
template< typename T, uint32_t Count, typename CharType >
std::basic_istream< CharType > & operator>>( std::basic_istream< CharType > & in, castor::Coords< T, Count > & out );
/**@}*/

#include "Coords.inl"

#endif
