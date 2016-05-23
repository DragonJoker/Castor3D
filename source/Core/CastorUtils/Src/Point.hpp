/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CASTOR_POINT_H___
#define ___CASTOR_POINT_H___

#include "File.hpp"
#include "Aligned.hpp"

#include <cmath>
#include <iostream>

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		15/01/2016
	\~english
	\brief		Holds the point coords, and can be specialised to customise the behaviour.
	\~french
	\brief		Contient les coordonnées du point, peut être spécialisée, afin de personnaliser le comportement.
	*/
	template< typename T, uint32_t Count >
	class PointDataHolder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		PointDataHolder()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~PointDataHolder()
		{
		}

	protected:
		//!\~english The point data.	\~french Les données du point.
		T m_coords[Count];
	};

#if CASTOR_USE_SSE2

	/*!
	\author		Sylvain DOREMUS
	\date		15/01/2016
	\~english
	\brief		Specialisation for 4 floats, allocates an aligned buffer.
	\~french
	\brief		Spécialisation pour 4 floats, alloue un tampon aligné.
	*/
	template<>
	class PointDataHolder< float, 4 >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		PointDataHolder()
			: m_coords( reinterpret_cast< float * >( AlignedAlloc( 16, 16 ) ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~PointDataHolder()
		{
			AlignedFree( m_coords );
		}

	protected:
		//!\~english The point data.	\~french Les données du point.
		float * m_coords;
	};

#endif

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
	class Point
		: public PointDataHolder< T, Count >
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		\~english
		\brief Coords< T, Count > loader
		\~french
		\brief Loader de Coords< T, Count >
		*/
		class TextLoader
			: public Castor::TextLoader< Point< T, Count > >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( String const & p_tabs, File::eENCODING_MODE p_encodingMode = File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Loads a Coords< T, Count > object from a text file
			 *\param[out]	p_object	The Coords< T, Count > object to read
			 *\param[in]	p_file		The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Charge un objet Coords< T, Count > à partir d'un fichier texte
			 *\param[out]	p_object	L'objet Coords< T, Count >
			 *\param[in]	p_file		Le fichier
			 *\return		\p true si ok
			 */
			virtual bool operator()( Point< T, Count > & p_object, TextFile & p_file );
			/**
			 *\~english
			 *\brief		Writes a Point2f object into a text file
			 *\param[in]	p_object	The Point2f object to write
			 *\param[out]	p_file		The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Ecrit un objet Point2f dans un fichier texte
			 *\param[in]	p_object	L'objet Point2f
			 *\param[out]	p_file		Le fichier
			 *\return		\p true si ok
			 */
			virtual bool operator()( Point< T, Count > const & p_object, TextFile & p_file );
		};

	private:
		template< typename U, uint32_t _Count > friend class Point;

		typedef T value_type;
		typedef value_type & reference;
		typedef value_type * pointer;
		typedef value_type const & const_reference;
		typedef value_type const * const_pointer;
		typedef Point< value_type, Count > & point_reference;
		typedef Point< value_type, Count > * point_pointer;
		typedef Point< value_type, Count > const & const_point_reference;
		typedef Point< value_type, Count > const * const_point_pointer;
		static const std::size_t binary_size = sizeof( T ) * Count;

	public:
		//!\~english Typedef over a pointer on data	\~french Typedef d'un pointeur sur les données
		typedef value_type * iterator;
		//!\~english Typedef over a constant pointer on data	\~french Typedef d'un pointeur constant sur les données
		typedef value_type const * const_iterator;

	public:
		/**
		 *\~english
		 *\brief		Default Constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		Point();
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_ptPoint	The Point object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_ptPoint	L'objet Point à copier
		 */
		template< typename U >
		explicit Point( Point< U, Count > const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_ptPoint	The Point object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_ptPoint	L'objet Point à copier
		 */
		Point( Point< T, Count > const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_ptPoint	The Point object to move
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_ptPoint	L'objet Point à déplacer
		 */
		Point( Point< T, Count > && p_ptPoint );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_ptPoint	The Point object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_ptPoint	L'objet Point à copier
		 */
		Point( Coords< T, Count > const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pValues	The data buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pValues	Le buffer de données
		 */
		explicit Point( T const * p_pValues );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pValues	The data buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pValues	Le buffer de données
		 */
		template< typename U >
		explicit Point( U const * p_pValues );
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		template< typename ValueA, typename ValueB, typename ... Values >
		explicit Point( ValueA p_valueA, ValueB p_valueB, Values ... p_values );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Point();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pt	The Point object to copy
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_pt	L'objet Point à copier
		 *\return		Une référence sur cet objet Point
		 */
		Point< T, Count > & operator=( Point< T, Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pt	The Point object to move
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_pt	L'objet Point à déplacer
		 *\return		Une référence sur cet objet Point
		 */
		Point< T, Count > & operator=( Point< T, Count > && p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_pt	The Point object to add
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Point à ajouter
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator+=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Point object to substract
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Point à soustraire
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator-=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Point object to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Point à multiplier
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator*=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Point object to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_pt	L'objet Point à diviser
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator/=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_pt	The Point object to add
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Point à ajouter
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator+=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Point object to substract
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Point à soustraire
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator-=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Point object to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Point à multiplier
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator*=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Point object to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_pt	L'objet Point à diviser
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator/=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_coords	The values to add
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_coords	Les valeurs à additionner
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator+=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_coords	The values to substract
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_coords	Les valeurs à soustraire
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator-=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coords	The values to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_coords	Les valeurs à multiplier
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator*=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coords	The values to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_coords	Les valeurs à diviser
		 *\return		Une référence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator/=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coord	The value to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_coord	La valeur à multiplier
		 *\return		Une référence sur cet objet Point
		 */
		Point< T, Count > & operator*=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coord	The value to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_coord	La valeur à diviser
		 *\return		Une référence sur cet objet Point
		 */
		Point< T, Count > & operator/=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Swaps this pont data with the parameter ones
		 *\param[in]	p_pt	The point to swap
		 *\~french
		 *\brief		Echange les données de c epoint avec celles du point donné
		 *\param[in]	p_pt	Le point à échanger
		 */
		void swap( Point< T, Count > & p_pt );
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
		 *\param[in,out]	p_pResult	Receives the point data, needs to be allocated by the caller
		 *\~french
		 *\brief			Récupère les données et les place dans un tableau
		 *\param[in,out]	p_pResult	Reçoit les données, doit être alloué par l'appelant
		 */
		void to_values( T * p_pResult )const;
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
		inline std::size_t elem_size()const
		{
			return sizeof( T );
		}
		/**
		 *\~english
		 *\brief		Retrieves the total size of the point
		 *\return		count() * elem_size()
		 *\~french
		 *\brief		Récupère la taille totale du point
		 *\return		count() * elem_size()
		 */
		inline	std::size_t	size()const
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
		inline T const & operator[]( uint32_t p_pos )const
		{
			return this->m_coords[p_pos];
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
		inline T & operator[]( uint32_t p_pos )
		{
			return this->m_coords[p_pos];
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
		T const & at( uint32_t p_pos )const;
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
		T & at( uint32_t p_pos );
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
			return &this->m_coords[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur constant sur les données
		 *\return		Le pointeur
		 */
		inline T const * const_ptr()const
		{
			return &this->m_coords[0];
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
			return &this->m_coords[0];
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
			return &this->m_coords[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le dernier élément
		 *\return		L'itérateur
		 */
		inline const_iterator end()const
		{
			return &this->m_coords[0] + Count;
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
			return &this->m_coords[0] + Count;
		}
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have same dimensions and same values
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont les mêmes dimensions et les mêmes valeurs
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator==( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have different dimensions or at least one different value
	 *\~french
	 *\brief		Opérateur d'inégalité
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont des dimensions différentes ou au moins une valeur différente
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator!=( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_ptA, p_ptB	The points to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_ptA, p_ptB	Les points à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator+( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_ptA, p_ptB	The points to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_ptA, p_ptB	Les points à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator-( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator*( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_ptA, p_ptB	The points to divide
	 *\return		The division result
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_ptA, p_ptB	Les points à diviser
	 *\return		Le résultat de la division
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator/( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Vectorial product operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The vectorial product
	 *\~french
	 *\brief		Opérateur de produit vectoriel
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le résultat du produit vectoriel
	 */
	template< typename T, typename U > Point< T, 3 > operator^( Point< T, 3 > const & p_ptA,  Point< U, 3 > const & p_ptB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator+( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator-( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs à mulitplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator*( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pt		The point to divide
	 *\param[in]	p_coords	The values
	 *\return		The division result
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_pt		Le point à diviser
	 *\param[in]	p_coords	Les valeurs
	 *\return		Le résultat de la division
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator/( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pt	The point to multiply
	 *\param[in]	p_coord	The value
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_pt	Le point à multiplier
	 *\param[in]	p_coord	La valeur
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator*( Point< T, Count > const & p_pt, U const & p_coord );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pt	The point to divide
	 *\param[in]	p_coord	The value
	 *\return		The division result
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_pt	Le point à diviser
	 *\param[in]	p_coord	La valeur
	 *\return		Le résultat de la division
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator/( Point< T, Count > const & p_pt, U const & p_coord );
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have same dimensions and same values
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont les mêmes dimensions et les mêmes valeurs
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator==( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have different dimensions or at least one different value
	 *\~french
	 *\brief		Opérateur d'inégalité
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont des dimensions différentes ou au moins une valeur différente
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator!=( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_ptA, p_ptB	The points to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_ptA, p_ptB	Les points à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator+( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_ptA, p_ptB	The points to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_ptA, p_ptB	Les points à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator-( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator*( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_ptA, p_ptB	The points to divide
	 *\return		The division result
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_ptA, p_ptB	Les points à diviser
	 *\return		Le résultat de la division
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator/( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Vectorial product operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The vectorial product
	 *\~french
	 *\brief		Opérateur de produit vectoriel
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le résultat du produit vectoriel
	 */
	template< typename T, typename U > Point< T, 3 > operator^( Point< T, 3 > const & p_ptA, Coords< U, 3 > const & p_ptB );
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have same dimensions and same values
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont les mêmes dimensions et les mêmes valeurs
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator==( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have different dimensions or at least one different value
	 *\~french
	 *\brief		Opérateur d'inégalité
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont des dimensions différentes ou au moins une valeur différente
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator!=( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_ptA, p_ptB	The points to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_ptA, p_ptB	Les points à additionner
	 *\return		Le résultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator+( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_ptA, p_ptB	The points to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_ptA, p_ptB	Les points à soustraire
	 *\return		Le résultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator-( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le résultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator* ( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_ptA, p_ptB	The points to divide
	 *\return		The division result
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_ptA, p_ptB	Les points à diviser
	 *\return		Le résultat de la division
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator/( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Vectorial product operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The vectorial product
	 *\~french
	 *\brief		Opérateur de produit vectoriel
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le résultat du produit vectoriel
	 */
	template< typename T, typename U > Point< T, 3 > operator^( Coords< T, 3 > const & p_ptA, Point< U, 3 > const & p_ptB );
	/**
	 *\~english
	 *\brief		Negation operator
	 *\param[in]	p_pt	The point to negate
	 *\~french
	 *\brief		Opérateur de négation
	 *\param[in]	p_pt	Le point à négativer
	 */
	template< typename T, uint32_t Count > Point< T, Count > operator-( Point< T, Count > const & p_pt );
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
		 *\param[in]	p_ptPoint	The point
		 *\~french
		 *\brief		Négative chaque donnée du point donné
		 *\param[in]	p_ptPoint	Le point
		 */
		template< typename T, uint32_t Count > static void negate( Point< T, Count > & p_ptPoint );
		/**
		 *\~english
		 *\brief		Normalises the point
		 *\remarks		A point is normalised when it's length equals 1.0
						<br />So each data is divided by the point's length
		 *\param[in]	p_ptPoint	The point
		 *\~french
		 *\brief		Normalise le point
		 *\remarks		Un point est normalisé quand sa longueur vaut 1.0
						<br />Chaque donnée est donc divisée par la longueur du point
		 *\param[in]	p_ptPoint	Le point
						*/
		template< typename T, uint32_t Count > static void normalise( Point< T, Count > & p_ptPoint );
		/**
		 *\~english
		 *\brief		Returns the normalised form of a point
		 *\param[in]	p_ptPoint	The point
		 *\~french
		 *\brief		Retourne la forme normalisée d'un point
		 *\param[in]	p_ptPoint	Le point
		 */
		template< typename T, uint32_t Count > static Point< T, Count > get_normalised( Point< T, Count > const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Computes the scalar product of 2 points
		 *\param[in]	p_ptA, p_ptB	The points
		 *\return		The scalar product
		 *\~french
		 *\brief		Calcule le produit scalaire entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le produit scalaire
		 */
		template< typename T, typename U, uint32_t Count > static T dot( Point< T, Count > const & p_ptA, Point< T, Count > const & p_ptB );
		/**
		 *\~english
		 *\brief		Computes the trigonometric cosine of the angle between 2 points
		 *\param[in]	p_ptA, p_ptB	The points
		 *\return		The cosine
		 *\~french
		 *\brief		Calcule le cosinus trigonométrique de l'angle entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le cosinus
		 */
		template< typename T, uint32_t Count > static double cos_theta( Point< T, Count > const & p_ptA, Point< T, Count > const & p_ptB );
		/**
		 *\~english
		 *\brief		Computes the squared Euclidian distance of the point
		 *\param[in]	p_ptPoint	The point
		 *\return		The squared Euclidian distance
		 *\~french
		 *\brief		Calcule le carré de la norme Euclidienne du point
		 *\param[in]	p_ptPoint	Le point
		 *\return		Le carré de la norme Euclidienne
		 */
		template< typename T, uint32_t Count > static double distance_squared( Point< T, Count > const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Computes the Euclidian distance of the point
		 *\param[in]	p_ptPoint	The point
		 *\return		The Euclidian distance
		 *\~french
		 *\brief		Calcule la norme Euclidienne du point
		 *\param[in]	p_ptPoint	Le point
		 *\return		La norme Euclidienne
		 */
		template< typename T, uint32_t Count > static double distance( Point< T, Count > const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Computes the Manhattan distance of the point
		 *\param[in]	p_ptPoint	The point
		 *\return		The Manhattan distance
		 *\~french
		 *\brief		Calcule la distance de Manhattan du point
		 *\param[in]	p_ptPoint	Le point
		 *\return		La distance de Manhattan
		 */
		template< typename T, uint32_t Count > static double distance_manhattan( Point< T, Count > const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Computes the Minkowski distance of the point
		 *\param[in]	p_ptPoint	The point
		 *\param[in]	p_dOrder	The Minkowski order
		 *\return		The Minkowski distance
		 *\~french
		 *\brief		Calcule la distance de Minkowski du point
		 *\param[in]	p_ptPoint	Le point
		 *\param[in]	p_dOrder	L'ordre Minkownski
		 *\return		La distance de Minkowski
		 */
		template< typename T, uint32_t Count > static double distance_minkowski( Point< T, Count > const & p_ptPoint, double p_dOrder );
		/**
		 *\~english
		 *\brief		Computes the Chebychev distance of the point
		 *\param[in]	p_ptPoint	The point
		 *\return		The Chebychev distance
		 *\~french
		 *\brief		Calcule la distance de Chebychev du point
		 *\param[in]	p_ptPoint	Le point
		 *\return		La distance de Chebychev
		 */
		template< typename T, uint32_t Count > static double distance_chebychev( Point< T, Count > const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Computes the scalar product of 2 points
		 *\param[in]	p_ptA, p_ptB	The points
		 *\return		The scalar product
		 *\~french
		 *\brief		Calcule le produit scalaire entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le produit scalaire
		 */
		template< typename T, typename U, uint32_t Count > static T dot( Coords< T, Count > const & p_ptA, Point< T, Count > const & p_ptB );
		/**
		 *\~english
		 *\brief		Computes the trigonometric cosine of the angle between 2 points
		 *\param[in]	p_ptA, p_ptB	The points
		 *\return		The cosine
		 *\~french
		 *\brief		Calcule le cosinus trigonométrique de l'angle entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le cosinus
		 */
		template< typename T, uint32_t Count > static double cos_theta( Coords< T, Count > const & p_ptA, Point< T, Count > const & p_ptB );
		/**
		 *\~english
		 *\brief		Computes the scalar product of 2 points
		 *\param[in]	p_ptA, p_ptB	The points
		 *\return		The scalar product
		 *\~french
		 *\brief		Calcule le produit scalaire entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le produit scalaire
		 */
		template< typename T, typename U, uint32_t Count > static T dot( Point< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB );
		/**
		 *\~english
		 *\brief		Computes the trigonometric cosine of the angle between 2 points
		 *\param[in]	p_ptA, p_ptB	The points
		 *\return		The cosine
		 *\~french
		 *\brief		Calcule le cosinus trigonométrique de l'angle entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le cosinus
		 */
		template< typename T, uint32_t Count > static double cos_theta( Point< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB );
	}
}
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamOut	The stream receiving point's data
 *\param[in]		p_pt		The input point
 *\return			A reference to the stream
 *\~french
 *\brief			Opérateur de flux
 *\param[in,out]	p_streamOut	Le flux qui reçoit les données du point
 *\param[in]		p_pt		Le point entré
 *\return			Une référence sur le flux
 */
template< typename T, uint32_t Count > Castor::String & operator<<( Castor::String & p_streamOut, Castor::Point< T, Count > const & p_pt );
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamIn	The stream holding point's data
 *\param[in,out]	p_pt		The output point
 *\return			A reference to the stream
 *\~french
 *\brief			Opérateur de flux
 *\param[in,out]	p_streamIn	Le flux qui contient les données du point
 *\param[in,out]	p_pt		Le point sortie
 *\return			Une référence sur le flux
 */
template< typename T, uint32_t Count > Castor::String & operator>>( Castor::String & p_streamIn, Castor::Point< T, Count > & p_pt );
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamOut	The stream receiving point's data
 *\param[in]		p_pt		The input point
 *\return			A reference to the stream
 *\~french
 *\brief			Opérateur de flux
 *\param[in,out]	p_streamOut	Le flux qui reçoit les données du point
 *\param[in]		p_pt		Le point entré
 *\return			Une référence sur le flux
 */
template< typename T, uint32_t Count, typename CharType > std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & p_streamOut, Castor::Point< T, Count > const & p_pt );
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamIn	The stream holding point's data
 *\param[in,out]	p_pt		The output point
 *\return			A reference to the stream
 *\~french
 *\brief			Opérateur de flux
 *\param[in,out]	p_streamIn	Le flux qui contient les données du point
 *\param[in,out]	p_pt		Le point sortie
 *\return			Une référence sur le flux
 */
template< typename T, uint32_t Count, typename CharType > std::basic_istream< CharType > & operator>>( std::basic_istream< CharType > & p_streamIn, Castor::Point< T, Count > & p_pt );

#include "Point.inl"

#endif
