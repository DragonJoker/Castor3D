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
#ifndef ___Castor_Point___
#define ___Castor_Point___

#include "File.hpp"
#include "NonCopyable.hpp"

#include <cmath>

namespace Castor
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
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Coords< T, Count > loader
		\~french
		\brief		Loader de Coords< T, Count >
		*/
		class BinaryLoader : public Castor::Loader< Coords< T, Count >, eFILE_TYPE_BINARY, BinaryFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			BinaryLoader();
			/**
			 *\~english
			 *\brief			Loads a Coords< T, Count > from a binary file
			 *\param[in,out]	p_object	The Coords< T, Count > to load
			 *\param[in,out]	p_file		The file where to load the Coords< T, Count >
			 *\~french
			 *\brief			Charge un Coords< T, Count > à partir d'un fichier binaire
			 *\param[in,out]	p_object	Le Coords< T, Count > à charger
			 *\param[in,out]	p_file		Le fichier où charger le Coords< T, Count >
			 */
			bool operator()( Coords< T, Count > & p_object, BinaryFile & p_file );
			/**
			 *\~english
			 *\brief			Saves a Coords< T, Count > to a binary file
			 *\param[in]		p_object	The Coords< T, Count > to save
			 *\param[in,out]	p_file		The file where to save the Coords< T, Count >
			 *\~french
			 *\brief			Ecrit un Coords< T, Count > dans un fichier binaire
			 *\param[in,out]	p_object	Le Coords< T, Count > à écrire
			 *\param[in,out]	p_file		Le fichier où écrire le Coords< T, Count >
			 */
			bool operator()( Coords< T, Count > const & p_object, BinaryFile & p_file );
		};
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		\~english
		\brief Coords< T, Count > loader
		\~french
		\brief Loader de Coords< T, Count >
		*/
		class TextLoader : public Castor::Loader< Coords< T, Count >, eFILE_TYPE_TEXT, TextFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( File::eENCODING_MODE p_eEncodingMode=File::eENCODING_MODE_ASCII );
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
			virtual bool operator ()( Coords< T, Count > & p_object, TextFile & p_file);
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
			virtual bool operator ()( Coords< T, Count > const & p_object, TextFile & p_file);
		};

	private:
		template< typename U, uint32_t _Count > friend class Coords;

		typedef T									value_type;
		typedef value_type &						reference;
		typedef value_type *						pointer;
		typedef value_type const &					const_reference;
		typedef value_type const *					const_pointer;
		typedef Coords< value_type, Count > &		point_reference;
		typedef Coords< value_type, Count > *		point_pointer;
		typedef Coords< value_type, Count > const &	const_point_reference;
		typedef Coords< value_type, Count > const *	const_point_pointer;
		static const std::size_t					binary_size = sizeof( T ) * Count;

	public:
		//!\~english	Typedef over a pointer on data			\~french	Typedef d'un pointeur sur les données
		typedef value_type *						iterator;
		//!\~english	Typedef over a constant pointer on data	\~french	Typedef d'un pointeur constant sur les données
		typedef value_type const *					const_iterator;

	private:
		T * m_coords;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Coords();
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_ptPoint	The Coords object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_ptPoint	L'objet Coords à copier
		 */
		Coords( Coords< T, Count > const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_ptPoint	The Coords object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_ptPoint	L'objet Coords à déplacer
		 */
		Coords( Coords< T, Count > && p_ptPoint );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pValues	The data buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pValues	Le buffer de données
		 */
		explicit Coords( T * p_pValues );
		/**
		 *\~english
		 *\brief		Destructor
		 *\remark		Not virtual but it should be ok to derive Coords because destructor is dummy
		 *\~french
		 *\brief		Destructeur
		 *\remark		Non virtuel mais il devrait être possible de dériver Coords car le destructeur ne fait rien
		 */
		~Coords();
		/**
		 *\~english
		 *\brief		Assignment operator
		 *\param[in]	p_pValues	The values to affect
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation
		 *\param[in]	p_pValues	Les valeurs à affecter
		 *\return		Une référence sur cet objet Coords
		 */
		Coords< T, Count > & operator =( T * p_pValues );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pt	The Coords object to copy
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_pt	L'objet Coords à copier
		 *\return		Une référence sur cet objet Coords
		 */
		Coords< T, Count > & operator =( Coords< T, Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pt	The Coords object to move
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_pt	L'objet Coords à déplacer
		 *\return		Une référence sur cet objet Coords
		 */
		Coords< T, Count > & operator =( Coords< T, Count > && p_pt );
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
		Coords< T, Count > & operator =( Point< T, Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_pt	The Coords object to add
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Coords à ajouter
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator +=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Coords object to substract
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Coords à soustraire
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator -=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Coords object to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Coords à multiplier
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator *=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Coords object to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_pt	L'objet Coords à diviser
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator /=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_pt	The Coords object to add
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Coords à ajouter
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator +=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Coords object to substract
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Coords à soustraire
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator -=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Coords object to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Coords à multiplier
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator *=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Coords object to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_pt	L'objet Coords à diviser
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator /=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_coords	The values to add
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_coords	Les valeurs à additionner
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator +=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_coords	The values to substract
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_coords	Les valeurs à soustraire
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator -=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coords	The values to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_coords	Les valeurs à multiplier
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator *=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coords	The values to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_coords	Les valeurs à diviser
		 *\return		Une référence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator /=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coord	The value to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_coord	La valeur à multiplier
		 *\return		Une référence sur cet objet Coords
		 */
		Coords< T, Count > & operator *=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coord	The value to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_coord	La valeur à diviser
		 *\return		Une référence sur cet objet Coords
		 */
		Coords< T, Count > & operator /=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Swaps this pont data with the parameter ones
		 *\param[in]	p_pt	The point to swap
		 *\~french
		 *\brief		Echange les données de c epoint avec celles du point donné
		 *\param[in]	p_pt	Le point à échanger
		 */
		void swap( Coords< T, Count > & p_pt );
		/**
		 *\~english
		 *\brief		Inverts data in the point
		 *\remark		The first becomes last and so on
		 *\~french
		 *\brief		Inverse les données dans ce point
		 *\remark		Les premiers deviennent les derniers
		 */
		void flip();
		/**
		 *\~english
		 *\brief		Retrieves the total size of the point
		 *\return		count() * elem_size()
		 *\~french
		 *\brief		Récupère la taille totale du point
		 *\return		count() * elem_size()
		 */
		inline uint32_t size()const { return binary_size; }
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
		inline uint32_t count()const { return Count; }
		/**
		 *\~english
		 *\brief		Retrieves the coordinate type size
		 *\return		The data type size
		 *\~french
		 *\brief		Récupère la taille du type données
		 *\return		La taille du type de données
		 */
		inline std::size_t elem_size()const { return sizeof( T ); }
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une référence constante sur la donnée à l'index voulu
		 */
		inline T const & operator[]( uint32_t p_pos )const { return m_coords[p_pos]; }
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une référence sur la donnée à l'index voulu
		 */
		inline T & operator[]( uint32_t p_pos ) { return m_coords[p_pos]; }
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		This fonction checks the index and throws an exception if it is out of bounds
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remark		Cette fonction vérifie l'index et lance une exception s'il est hors bornes
		 *\return		Une référence constante sur la donnée à l'index voulu
		 */
		T const & at( uint32_t p_pos )const;
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		This fonction checks the index and throws an exception if it is out of bounds
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remark		Cette fonction vérifie l'index et lance une exception s'il est hors bornes
		 *\return		Une référence sur la donnée à l'index voulu
		 */
		T & at( uint32_t p_pos );
		/**
		 *\~english
		 *\brief		Retrieves a non constant pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur non constant sur les données
		 *\return		Le pointeur
		 */
		inline T * ptr() { return m_coords; }
		/**
		 *\~english
		 *\brief		Retrieves a constant pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur constant sur les données
		 *\return		Le pointeur
		 */
		inline T const * const_ptr()const { return m_coords; }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le premier élément
		 *\return		L'itérateur
		 */
		inline iterator begin() { return m_coords;	}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le premier élément
		 *\return		L'itérateur
		 */
		inline const_iterator begin()const { return m_coords;	}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le dernier élément
		 *\return		L'itérateur
		 */
		inline iterator end() { return m_coords + Count;	}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le dernier élément
		 *\return		L'itérateur
		 */
		inline const_iterator end()const { return m_coords + Count;	}
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator ==( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator !=( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator +( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator -( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator *( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator /( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, typename U > Point< T, 3 > operator ^( Coords< T, 3 > const & p_ptA, Coords< U, 3 > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator +( Coords< T, Count > const & p_pt, U const * p_coords );
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
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator -( Coords< T, Count > const & p_pt, U const * p_coords );
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
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator *( Coords< T, Count > const & p_pt, U const * p_coords );
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
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator /( Coords< T, Count > const & p_pt, U const * p_coords );
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
	template< typename T, uint32_t Count > Point< T, Count > operator *( Coords< T, Count > const & p_pt, T const & p_coord );
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
	template< typename T, uint32_t Count > Point< T, Count > operator /( Coords< T, Count > const & p_pt, T const & p_coord );
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
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Point< T, Count > loader
		\~french
		\brief		Loader de Point< T, Count >
		*/
		class BinaryLoader : public Castor::Loader< Point< T, Count >, eFILE_TYPE_BINARY, BinaryFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			BinaryLoader();
			/**
			 *\~english
			 *\brief			Loads a Point< T, Count > from a binary file
			 *\param[in,out]	p_object	The Point< T, Count > to load
			 *\param[in,out]	p_file		The file where to load the Point< T, Count >
			 *\~french
			 *\brief			Charge un Point< T, Count > à partir d'un fichier binaire
			 *\param[in,out]	p_object	Le Point< T, Count > à charger
			 *\param[in,out]	p_file		Le fichier où charger le Point< T, Count >
			 */
			bool operator()( Point< T, Count > & p_object, BinaryFile & p_file );
			/**
			 *\~english
			 *\brief			Saves a Point< T, Count > to a binary file
			 *\param[in]		p_object	The Point< T, Count > to save
			 *\param[in,out]	p_file		The file where to save the Point< T, Count >
			 *\~french
			 *\brief			Ecrit un Point< T, Count > dans un fichier binaire
			 *\param[in,out]	p_object	Le Point< T, Count > à écrire
			 *\param[in,out]	p_file		Le fichier où écrire le Point< T, Count >
			 */
			bool operator()( Point< T, Count > const & p_object, BinaryFile & p_file );
		};
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		\~english
		\brief Coords< T, Count > loader
		\~french
		\brief Loader de Coords< T, Count >
		*/
		class TextLoader : public Castor::Loader< Point< T, Count >, eFILE_TYPE_TEXT, TextFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( File::eENCODING_MODE p_eEncodingMode=File::eENCODING_MODE_ASCII );
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
			virtual bool operator ()( Point< T, Count > & p_object, TextFile & p_file);
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
			virtual bool operator ()( Point< T, Count > const & p_object, TextFile & p_file);
		};

	private:
		template< typename U, uint32_t _Count > friend class Point;

		typedef T									value_type;
		typedef value_type &						reference;
		typedef value_type *						pointer;
		typedef value_type const &					const_reference;
		typedef value_type const *					const_pointer;
		typedef Point< value_type, Count > &		point_reference;
		typedef Point< value_type, Count > *		point_pointer;
		typedef Point< value_type, Count > const &	const_point_reference;
		typedef Point< value_type, Count > const *	const_point_pointer;
		static const std::size_t					binary_size = sizeof( T ) * Count;

	public:
		//!\~english	Typedef over a pointer on data			\~french	Typedef d'un pointeur sur les données
		typedef value_type *						iterator;
		//!\~english	Typedef over a constant pointer on data	\~french	Typedef d'un pointeur constant sur les données
		typedef value_type const *					const_iterator;

	private:
		T m_coords[Count];

	public:
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
#if CASTOR_HAS_VARIADIC_TEMPLATES
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		template< typename ... Values >
		explicit Point( Values ... p_values );
#else
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Point();
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Point( T const & p_vA, T const & p_vB );
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Point( T const & p_vA, T const & p_vB, T const & p_vC );
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Point( T const & p_vA, T const & p_vB, T const & p_vC, T const & p_vD );
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		template< typename U, typename V > Point( U const & p_vA, V const & p_vB );
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		template< typename U, typename V, typename W > Point( U const & p_vA, V const & p_vB, W const & p_vC );
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		template< typename U, typename V, typename W, typename X > Point( U const & p_vA, V const & p_vB, W const & p_vC, X const & p_vD );
#endif
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
		Point< T, Count > & operator =( Point< T, Count > const & p_pt );
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
		Point< T, Count > & operator =( Point< T, Count > && p_pt );
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
		template< typename U, uint32_t _Count > Point< T, Count > & operator +=( Point< U, _Count > const & p_pt );
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
		template< typename U, uint32_t _Count > Point< T, Count > & operator -=( Point< U, _Count > const & p_pt );
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
		template< typename U, uint32_t _Count > Point< T, Count > & operator *=( Point< U, _Count > const & p_pt );
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
		template< typename U, uint32_t _Count > Point< T, Count > & operator /=( Point< U, _Count > const & p_pt );
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
		template< typename U, uint32_t _Count > Point< T, Count > & operator +=( Coords< U, _Count > const & p_pt );
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
		template< typename U, uint32_t _Count > Point< T, Count > & operator -=( Coords< U, _Count > const & p_pt );
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
		template< typename U, uint32_t _Count > Point< T, Count > & operator *=( Coords< U, _Count > const & p_pt );
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
		template< typename U, uint32_t _Count > Point< T, Count > & operator /=( Coords< U, _Count > const & p_pt );
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
		template< typename U > Point< T, Count > & operator +=( U const * p_coords );
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
		template< typename U > Point< T, Count > & operator -=( U const * p_coords );
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
		template< typename U > Point< T, Count > & operator *=( U const * p_coords );
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
		template< typename U > Point< T, Count > & operator /=( U const * p_coords );
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
		Point< T, Count > & operator *=( T const & p_coord );
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
		Point< T, Count > & operator /=( T const & p_coord );
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
		 *\remark		The first becomes last and so on
		 *\~french
		 *\brief		Inverse les données dans ce point
		 *\remark		Les premiers deviennent les derniers
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
		void to_values ( T * p_pResult )const;
		/**
		 *\~english
		 *\brief		Retrieves the number of coordinates
		 *\return		The number of coordinates
		 *\~french
		 *\brief		Récupère le nombre de coordonnées
		 *\return		Le nombre de coordonnées
		 */
		inline uint32_t count()const { return Count; }
		/**
		 *\~english
		 *\brief		Retrieves the coordinate type size
		 *\return		The data type size
		 *\~french
		 *\brief		Récupère la taille du type données
		 *\return		La taille du type de données
		 */
		inline std::size_t elem_size()const { return sizeof( T ); }
		/**
		 *\~english
		 *\brief		Retrieves the total size of the point
		 *\return		count() * elem_size()
		 *\~french
		 *\brief		Récupère la taille totale du point
		 *\return		count() * elem_size()
		 */
		inline	std::size_t	size()const { return binary_size; }
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une référence constante sur la donnée à l'index voulu
		 */
		inline T const & operator[]( uint32_t p_pos )const { return m_coords[p_pos]; }
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une référence sur la donnée à l'index voulu
		 */
		inline T & operator[]( uint32_t p_pos ) { return m_coords[p_pos]; }
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		This fonction checks the index and throws an exception if it is out of bounds
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remark		Cette fonction vérifie l'index et lance une exception s'il est hors bornes
		 *\return		Une référence constante sur la donnée à l'index voulu
		 */
		T const & at ( uint32_t p_pos )const;
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		This fonction checks the index and throws an exception if it is out of bounds
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		Récupère la donnée à l'index donné
		 *\remark		Cette fonction vérifie l'index et lance une exception s'il est hors bornes
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
		inline T * ptr() { return &m_coords[0]; }
		/**
		 *\~english
		 *\brief		Retrieves a constant pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur constant sur les données
		 *\return		Le pointeur
		 */
		inline T const * const_ptr()const { return &m_coords[0]; }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le premier élément
		 *\return		L'itérateur
		 */
		inline iterator begin() { return &m_coords[0]; }
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le premier élément
		 *\return		L'itérateur
		 */
		inline const_iterator begin()const { return &m_coords[0]; }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le dernier élément
		 *\return		L'itérateur
		 */
		inline const_iterator end()const { return &m_coords[0] + Count;	}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le dernier élément
		 *\return		L'itérateur
		 */
		inline iterator end() { return &m_coords[0] + Count; }
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator ==( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator !=( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator +( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator -( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator *( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator /( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
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
	template< typename T, typename U > Point< T, 3 > operator ^( Point< T, 3 > const & p_ptA,  Point< U, 3 > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator +( Point< T, Count > const & p_pt, U const * p_coords );
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
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator -( Point< T, Count > const & p_pt, U const * p_coords );
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
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator *( Point< T, Count > const & p_pt, U const * p_coords );
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
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator /( Point< T, Count > const & p_pt, U const * p_coords );
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
	template< typename T, uint32_t Count > Point< T, Count > operator *( Point< T, Count > const & p_pt, T const & p_coord );
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
	template< typename T, uint32_t Count > Point< T, Count > operator /( Point< T, Count > const & p_pt, T const & p_coord );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator ==( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator !=( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator +( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator -( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator *( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator /( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, typename U > Point< T, 3 > operator ^( Point< T, 3 > const & p_ptA, Coords< U, 3 > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator ==( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator !=( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator +( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator -( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator * ( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator /( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
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
	template< typename T, typename U > Point< T, 3 > operator ^( Coords< T, 3 > const & p_ptA, Point< U, 3 > const & p_ptB );
	/**
	 *\~english
	 *\brief		Negation operator
	 *\param[in]	p_pt	The point to negate
	 *\~french
	 *\brief		Opérateur de négation
	 *\param[in]	p_pt	Le point à négativer
	 */
	template< typename T, uint32_t Count > Point< T, Count > operator -( Point< T, Count > const & p_pt );
	/*!
	\author 	Sylvain DOREMUS
	\date 		10/07/2012
	\version	0.7.0
	\~english
	\brief		Point helper functions
	\~french
	\brief		Fonctions d'aide pour les Point
	*/
	class point
	{
	public:
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
		 *\remark		A point is normalised when it's length equals 1.0
						<br />So each data is divided by the point's length
		 *\param[in]	p_ptPoint	The point
		 *\~french
		 *\brief		Normalise le point
		 *\remark		Un point est normalisé quand sa longueur vaut 1.0
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
		template< typename T, uint32_t Count > static T dot( Point< T, Count > const & p_ptA, Point< T, Count > const & p_ptB );
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
		template< typename T, uint32_t Count > static T dot( Coords< T, Count > const & p_ptA, Point< T, Count > const & p_ptB );
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
		template< typename T, uint32_t Count > static T dot( Point< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB );
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
		/**
		 *\~english
		 *\brief		Negates every value of a point's data
		 *\param[in]	p_ptPoint	The point
		 *\~french
		 *\brief		Négative chaque donnée du point donné
		 *\param[in]	p_ptPoint	Le point
		 */
		template< typename T, uint32_t Count > static void negate( Coords< T, Count > & p_ptPoint );
		/**
		 *\~english
		 *\brief		Normalises the point
		 *\remark		A point is normalised when it's length equals 1.0
						<br />So each data is divided by the point's length
		 *\param[in]	p_ptPoint	The point
		 *\~french
		 *\brief		Normalise le point
		 *\remark		Un point est normalisé quand sa longueur vaut 1.0
						<br />Chaque donnée est donc divisée par la longueur du point
		 *\param[in]	p_ptPoint	Le point
						*/
		template< typename T, uint32_t Count > static void normalise( Coords< T, Count > & p_ptPoint );
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
		template< typename T, uint32_t Count > static T dot( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB );
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
		template< typename T, uint32_t Count > static double cos_theta( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB );
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
		template< typename T, uint32_t Count > static double distance_squared( Coords< T, Count > const & p_ptPoint );
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
		template< typename T, uint32_t Count > static double distance( Coords< T, Count > const & p_ptPoint );
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
		template< typename T, uint32_t Count > static double distance_manhattan( Coords< T, Count > const & p_ptPoint );
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
		template< typename T, uint32_t Count > static double distance_minkowski( Coords< T, Count > const & p_ptPoint, double p_dOrder );
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
		template< typename T, uint32_t Count > static double distance_chebychev( Coords< T, Count > const & p_ptPoint );
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/02/2013
	\version	0.7.0
	\~english
	\brief		Size class
	\remark		Kind of specialisation of Coords< 2, uint32_t >
	\~french
	\brief		Classe de taille
	\remark		Sorte de spécialisation de Coords< 2, uint32_t >
	*/
	class Size : private Coords< uint32_t, 2 >
	{
	private:
		typedef Coords< uint32_t, 2 > BaseType;
		union
		{
			struct
			{
				uint32_t cx;
				uint32_t cy;
			}			size;
			uint32_t	buffer[2];
		}	m_data;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_uiWidth, p_uiHeight	The dimensions
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_uiWidth, p_uiHeight	Les dimensions
		 */
		Size( uint32_t p_uiWidth=0, uint32_t p_uiHeight=0 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_obj	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_obj	L'objet à copier
		 */
		Size( Size const & p_obj );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_obj	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_obj	L'objet à déplacer
		 */
		Size( Size && p_obj );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Size();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_obj	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_obj	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		Size & operator =( Size const & p_obj );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_obj	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_obj	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		Size & operator =( Size && p_obj );
		/**
		 *\~english
		 *\brief		Sets the size values
		 *\param[in]	p_uiWidth, p_uiHeight	The dimensions
		 *\~french
		 *\brief		Définit la taille
		 *\param[in]	p_uiWidth, p_uiHeight	Les dimensions
		 */
		void set( uint32_t p_uiWidth, uint32_t p_uiHeight );
		/**
		 *\~english
		 *\brief		Retrieves the width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur
		 *\return		La largeur
		 */
		inline uint32_t width()const { return m_data.size.cx; }
		/**
		 *\~english
		 *\brief		Retrieves the height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur
		 *\return		La hauteur
		 */
		inline uint32_t height()const { return m_data.size.cy; }
		/**
		 *\~english
		 *\brief		Modifies the size
		 *\remark		If width+cx < 0 (or height+cy < 0) then width=0 (respectively height=0)
		 *\param[in]	p_cx, p_cy	The size modifications
		 *\~french
		 *\brief		Modifie la taille
		 *\remark		Si width+cx < 0 (ou height+cy < 0) alors width=0 (respectivement height=0)
		 *\param[in]	p_cx, p_cy	Les valeurs de modification
		 */
		void grow( int32_t p_cx, int32_t p_cy );

		using BaseType::ptr;
		using BaseType::const_ptr;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_a, p_b	The sizes to compare
	 *\return		\p true if sizes have same dimensions
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_a, p_b	Les tailles à comparer
	 *\return		\p true si les tailles ont les mêmes dimensions
	 */
	bool operator ==( Size const & p_a, Size const & p_b );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_a, p_b	The sizes to compare
	 *\return		\p false if sizes have same dimensions
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	p_a, p_b	Les tailles à comparer
	 *\return		\p false si les tailles ont les mêmes dimensions
	 */
	bool operator !=( Size const & p_a, Size const & p_b );
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/02/2013
	\version	0.7.0
	\~english
	\brief		Position class
	\remark		Kind of specialisation of Coords< 2, int32_t >
	\~french
	\brief		Classe de position
	\remark		Sorte de spécialisation de Coords< 2, int32_t >
	*/
	class Position : private Coords< int32_t, 2 >
	{
	private:
		typedef Coords< int32_t, 2 > BaseType;
		union
		{
			struct
			{
				int32_t x;
				int32_t y;
			}		position;
			int32_t	buffer[2];
		}	m_data;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_x, p_y	The position
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_x, p_y	La position
		 */
		Position( int32_t p_x=0, int32_t p_y=0 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_obj	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_obj	L'objet à copier
		 */
		Position( Position const & p_obj );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_obj	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_obj	L'objet à déplacer
		 */
		Position( Position && p_obj );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Position();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_obj	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_obj	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		Position & operator =( Position const & p_obj );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_obj	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_obj	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		Position & operator =( Position && p_obj );
		/**
		 *\~english
		 *\brief		Sets the position values
		 *\param[in]	p_x, p_y	The position
		 *\~french
		 *\brief		Définit la position
		 *\param[in]	p_x, p_y	La position
		 */
		void set( int32_t p_x, int32_t p_y );
		/**
		 *\~english
		 *\brief		Offsets the position values
		 *\param[in]	p_x, p_y	The position offsets
		 *\~french
		 *\brief		Déplace la position
		 *\param[in]	p_x, p_y	Les valeurs de déplacement
		 */
		void offset( int32_t p_x, int32_t p_y );
		/**
		 *\~english
		 *\brief		Retrieves the left coordinate
		 *\return		The rectangle's left coordinate
		 *\~french
		 *\brief		Récupère la coordonnée gauche
		 *\return		La coordonnée gauche du rectangle
		 */
		inline int32_t y()const { return m_data.position.x; }
		/**
		 *\~english
		 *\brief		Retrieves the right coordinate
		 *\return		The rectangle's right coordinate
		 *\~french
		 *\brief		Récupère la coordonnée droite
		 *\return		La coordonnée droite du rectangle
		 */
		inline int32_t x()const { return m_data.position.y; }

		using BaseType::ptr;
		using BaseType::const_ptr;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_a, p_b	The positions to compare
	 *\return		\p true if positions have same coordinates
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_a, p_b	Les positions à comparer
	 *\return		\p true si les positions ont les mêmes coordonnées
	 */
	bool operator ==( Position const & p_a, Position const & p_b );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_a, p_b	The positions to compare
	 *\return		\p false if positions have same coordinates
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	p_a, p_b	Les positions à comparer
	 *\return		\p false si les positions ont les mêmes coordonnées
	 */
	bool operator !=( Position const & p_a, Position const & p_b );
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
template< typename T, uint32_t Count > Castor::String & operator <<( Castor::String & p_streamOut, Castor::Point< T, Count > const & p_pt );
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
template< typename T, uint32_t Count > Castor::String & operator >>( Castor::String & p_streamIn, Castor::Point< T, Count > & p_pt );
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
template< typename T, uint32_t Count, typename CharType > std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & p_streamOut, Castor::Point< T, Count > const & p_pt );
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
template< typename T, uint32_t Count, typename CharType > std::basic_istream< CharType > & operator >>( std::basic_istream< CharType > & p_streamIn, Castor::Point< T, Count > & p_pt );
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
template< typename T, uint32_t Count > Castor::String & operator <<( Castor::String & p_streamOut, Castor::Coords< T, Count > const & p_pt );
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
template< typename T, uint32_t Count > Castor::String & operator >>( Castor::String & p_streamIn, Castor::Coords< T, Count > & p_pt );
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
template< typename T, uint32_t Count, typename CharType > std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & p_streamOut, Castor::Coords< T, Count > const & p_pt );
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
template< typename T, uint32_t Count, typename CharType > std::basic_istream< CharType > & operator >>( std::basic_istream< CharType > & p_streamIn, Castor::Coords< T, Count > & p_pt );

#include "Point.inl"

#endif
