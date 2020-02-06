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
#ifndef ___CASTOR_COORDS_H___
#define ___CASTOR_COORDS_H___

#include "BinaryFile.hpp"
#include "TextFile.hpp"
#include "NonCopyable.hpp"
#include "BinaryLoader.hpp"
#include "TextLoader.hpp"

#include <cmath>
#include <iostream>

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Templated static dimensions point representation
	\remark		Can hold any type which has a defined Policy
	\~french
	\brief		Classe de points � dimensions statiques
	\remark		Peut recevoir les types de donn�es qui ont une Policy cr��e
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
		class BinaryLoader
			: public Castor::Loader< Coords< T, Count >, eFILE_TYPE_BINARY, BinaryFile >, public Castor::NonCopyable
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
			 *\brief			Charge un Coords< T, Count > � partir d'un fichier binaire
			 *\param[in,out]	p_object	Le Coords< T, Count > � charger
			 *\param[in,out]	p_file		Le fichier o� charger le Coords< T, Count >
			 */
			bool operator()( Coords< T, Count > & p_object, BinaryFile & p_file );
			/**
			 *\~english
			 *\brief			Saves a Coords< T, Count > to a binary file
			 *\param[in]		p_object	The Coords< T, Count > to save
			 *\param[in,out]	p_file		The file where to save the Coords< T, Count >
			 *\~french
			 *\brief			Ecrit un Coords< T, Count > dans un fichier binaire
			 *\param[in,out]	p_object	Le Coords< T, Count > � �crire
			 *\param[in,out]	p_file		Le fichier o� �crire le Coords< T, Count >
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
		class TextLoader
			: public Castor::Loader< Coords< T, Count >, eFILE_TYPE_TEXT, TextFile >, public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( File::eENCODING_MODE p_eEncodingMode = File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Loads a Coords< T, Count > object from a text file
			 *\param[out]	p_object	The Coords< T, Count > object to read
			 *\param[in]	p_file		The file
			 *\return		\p true if ok
			 *\~french
			 *\brief		Charge un objet Coords< T, Count > � partir d'un fichier texte
			 *\param[out]	p_object	L'objet Coords< T, Count >
			 *\param[in]	p_file		Le fichier
			 *\return		\p true si ok
			 */
			virtual bool operator()( Coords< T, Count > & p_object, TextFile & p_file );
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
			virtual bool operator()( Coords< T, Count > const & p_object, TextFile & p_file );
		};

	private:
		template< typename U, uint32_t _Count > friend class Coords;

		typedef T value_type;
		typedef value_type & reference;
		typedef value_type * pointer;
		typedef value_type const & const_reference;
		typedef value_type const * const_pointer;
		typedef Coords< value_type, Count > & point_reference;
		typedef Coords< value_type, Count > * point_pointer;
		typedef Coords< value_type, Count > const & const_point_reference;
		typedef Coords< value_type, Count > const * const_point_pointer;
		static const std::size_t binary_size = sizeof( T ) * Count;

	public:
		//!\~english Typedef over a pointer on data	\~french Typedef d'un pointeur sur les donn�es
		typedef value_type * iterator;
		//!\~english Typedef over a constant pointer on data	\~french Typedef d'un pointeur constant sur les donn�es
		typedef value_type const * const_iterator;

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
		 *\param[in]	p_ptPoint	L'objet Coords � copier
		 */
		Coords( Coords< T, Count > const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_ptPoint	The Coords object to move
		 *\~french
		 *\brief		Constructeur par d�placement
		 *\param[in]	p_ptPoint	L'objet Coords � d�placer
		 */
		Coords( Coords< T, Count > && p_ptPoint );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pValues	The data buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pValues	Le buffer de donn�es
		 */
		explicit Coords( T * p_pValues );
		/**
		 *\~english
		 *\brief		Destructor
		 *\remark		Not virtual but it should be ok to derive Coords because destructor is dummy
		 *\~french
		 *\brief		Destructeur
		 *\remark		Non virtuel mais il devrait �tre possible de d�river Coords car le destructeur ne fait rien
		 */
		~Coords();
		/**
		 *\~english
		 *\brief		Assignment operator
		 *\param[in]	p_pValues	The values to affect
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation
		 *\param[in]	p_pValues	Les valeurs � affecter
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		Coords< T, Count > & operator =( T * p_pValues );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pt	The Coords object to copy
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par copie
		 *\param[in]	p_pt	L'objet Coords � copier
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		Coords< T, Count > & operator =( Coords< T, Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pt	The Coords object to move
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par d�placement
		 *\param[in]	p_pt	L'objet Coords � d�placer
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		Coords< T, Count > & operator =( Coords< T, Count > && p_pt );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pt	The Point object to copy
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par copie
		 *\param[in]	p_pt	L'objet Point � copier
		 *\return		Une r�f�rence sur cet objet Point
		 */
		Coords< T, Count > & operator =( Point< T, Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_pt	The Coords object to add
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Coords � ajouter
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator +=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Coords object to substract
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Coords � soustraire
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator -=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Coords object to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Coords � multiplier
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator *=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Coords object to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par division
		 *\param[in]	p_pt	L'objet Coords � diviser
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator /=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_pt	The Coords object to add
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Coords � ajouter
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator +=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Coords object to substract
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Coords � soustraire
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator -=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Coords object to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Coords � multiplier
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator *=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Coords object to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par division
		 *\param[in]	p_pt	L'objet Coords � diviser
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator /=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_coords	The values to add
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par addition
		 *\param[in]	p_coords	Les valeurs � additionner
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator +=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_coords	The values to substract
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par soustraction
		 *\param[in]	p_coords	Les valeurs � soustraire
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator -=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coords	The values to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par multiplication
		 *\param[in]	p_coords	Les valeurs � multiplier
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator *=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coords	The values to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par division
		 *\param[in]	p_coords	Les valeurs � diviser
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator /=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coord	The value to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par multiplication
		 *\param[in]	p_coord	La valeur � multiplier
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		Coords< T, Count > & operator *=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coord	The value to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Op�rateur d'affectation par division
		 *\param[in]	p_coord	La valeur � diviser
		 *\return		Une r�f�rence sur cet objet Coords
		 */
		Coords< T, Count > & operator /=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Swaps this pont data with the parameter ones
		 *\param[in]	p_pt	The point to swap
		 *\~french
		 *\brief		Echange les donn�es de c epoint avec celles du point donn�
		 *\param[in]	p_pt	Le point � �changer
		 */
		void swap( Coords< T, Count > & p_pt );
		/**
		 *\~english
		 *\brief		Inverts data in the point
		 *\remark		The first becomes last and so on
		 *\~french
		 *\brief		Inverse les donn�es dans ce point
		 *\remark		Les premiers deviennent les derniers
		 */
		void flip();
		/**
		 *\~english
		 *\brief		Retrieves the total size of the point
		 *\return		count() * elem_size()
		 *\~french
		 *\brief		R�cup�re la taille totale du point
		 *\return		count() * elem_size()
		 */
		inline uint32_t size()const
		{
			return binary_size;
		}
		/**
		 *\~english
		 *\brief			Retrieves data from the point and put it into an array
		 *\param[in,out]	p_pResult	Receives the point data, needs to be allocated by the caller
		 *\~french
		 *\brief			R�cup�re les donn�es et les place dans un tableau
		 *\param[in,out]	p_pResult	Re�oit les donn�es, doit �tre allou� par l'appelant
		 */
		void to_values( T * p_pResult )const;
		/**
		 *\~english
		 *\brief		Retrieves the number of coordinates
		 *\return		The number of coordinates
		 *\~french
		 *\brief		R�cup�re le nombre de coordonn�es
		 *\return		Le nombre de coordonn�es
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
		 *\brief		R�cup�re la taille du type donn�es
		 *\return		La taille du type de donn�es
		 */
		inline std::size_t elem_size()const
		{
			return sizeof( T );
		}
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		R�cup�re la donn�e � l'index donn�
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous � un crash
		 *\return		Une r�f�rence constante sur la donn�e � l'index voulu
		 */
		inline T const & operator[]( uint32_t p_pos )const
		{
			return m_coords[p_pos];
		}
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		R�cup�re la donn�e � l'index donn�
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous � un crash
		 *\return		Une r�f�rence sur la donn�e � l'index voulu
		 */
		inline T & operator[]( uint32_t p_pos )
		{
			return m_coords[p_pos];
		}
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		This fonction checks the index and throws an exception if it is out of bounds
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		R�cup�re la donn�e � l'index donn�
		 *\remark		Cette fonction v�rifie l'index et lance une exception s'il est hors bornes
		 *\return		Une r�f�rence constante sur la donn�e � l'index voulu
		 */
		T const & at( uint32_t p_pos )const;
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		This fonction checks the index and throws an exception if it is out of bounds
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		R�cup�re la donn�e � l'index donn�
		 *\remark		Cette fonction v�rifie l'index et lance une exception s'il est hors bornes
		 *\return		Une r�f�rence sur la donn�e � l'index voulu
		 */
		T & at( uint32_t p_pos );
		/**
		 *\~english
		 *\brief		Retrieves a non constant pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		R�cup�re un pointeur non constant sur les donn�es
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
		 *\brief		R�cup�re un pointeur constant sur les donn�es
		 *\return		Le pointeur
		 */
		inline T const * const_ptr()const
		{
			return m_coords;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le premier �l�ment
		 *\return		L'it�rateur
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
		 *\brief		R�cup�re un it�rateur constant sur le premier �l�ment
		 *\return		L'it�rateur
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
		 *\brief		R�cup�re un it�rateur sur le dernier �l�ment
		 *\return		L'it�rateur
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
		 *\brief		R�cup�re un it�rateur constant sur le dernier �l�ment
		 *\return		L'it�rateur
		 */
		inline const_iterator end()const
		{
			return m_coords + Count;
		}

	private:
		T * m_coords;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have same dimensions and same values
	 *\~french
	 *\brief		Op�rateur d'�galit�
	 *\param[in]	p_ptA, p_ptB	Les points � comparer
	 *\return		\p true si les points ont les m�mes dimensions et les m�mes valeurs
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator ==( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have different dimensions or at least one different value
	 *\~french
	 *\brief		Op�rateur d'in�galit�
	 *\param[in]	p_ptA, p_ptB	Les points � comparer
	 *\return		\p true si les points ont des dimensions diff�rentes ou au moins une valeur diff�rente
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator !=( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
		 *\brief		N�gative chaque donn�e du point donn�
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
		 *\remark		Un point est normalis� quand sa longueur vaut 1.0
						<br />Chaque donn�e est donc divis�e par la longueur du point
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
		 *\brief		Calcule le cosinus trigonom�trique de l'angle entre 2 points
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
		 *\brief		Calcule le carr� de la norme Euclidienne du point
		 *\param[in]	p_ptPoint	Le point
		 *\return		Le carr� de la norme Euclidienne
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
	}
}
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamOut	The stream receiving point's data
 *\param[in]		p_pt		The input point
 *\return			A reference to the stream
 *\~french
 *\brief			Op�rateur de flux
 *\param[in,out]	p_streamOut	Le flux qui re�oit les donn�es du point
 *\param[in]		p_pt		Le point entr�
 *\return			Une r�f�rence sur le flux
 */
template< typename T, uint32_t Count > Castor::String & operator <<( Castor::String & p_streamOut, Castor::Coords< T, Count > const & p_pt );
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamIn	The stream holding point's data
 *\param[in,out]	p_pt		The output point
 *\return			A reference to the stream
 *\~french
 *\brief			Op�rateur de flux
 *\param[in,out]	p_streamIn	Le flux qui contient les donn�es du point
 *\param[in,out]	p_pt		Le point sortie
 *\return			Une r�f�rence sur le flux
 */
template< typename T, uint32_t Count > Castor::String & operator >>( Castor::String & p_streamIn, Castor::Coords< T, Count > & p_pt );
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamOut	The stream receiving point's data
 *\param[in]		p_pt		The input point
 *\return			A reference to the stream
 *\~french
 *\brief			Op�rateur de flux
 *\param[in,out]	p_streamOut	Le flux qui re�oit les donn�es du point
 *\param[in]		p_pt		Le point entr�
 *\return			Une r�f�rence sur le flux
 */
template< typename T, uint32_t Count, typename CharType > std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & p_streamOut, Castor::Coords< T, Count > const & p_pt );
/**
 *\~english
 *\brief			Stream operator
 *\param[in,out]	p_streamIn	The stream holding point's data
 *\param[in,out]	p_pt		The output point
 *\return			A reference to the stream
 *\~french
 *\brief			Op�rateur de flux
 *\param[in,out]	p_streamIn	Le flux qui contient les donn�es du point
 *\param[in,out]	p_pt		Le point sortie
 *\return			Une r�f�rence sur le flux
 */
template< typename T, uint32_t Count, typename CharType > std::basic_istream< CharType > & operator >>( std::basic_istream< CharType > & p_streamIn, Castor::Coords< T, Count > & p_pt );

#include "Coords.inl"

#endif
