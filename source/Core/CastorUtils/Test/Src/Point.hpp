/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___Testing_Point___
#define ___Testing_Point___

#if defined( CASTOR_USE_OCL )

#include <cmath>

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 120
#include "cl.hpp"

namespace Testing
{
	template< typename T, uint32_t Count > class Point;
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
	private:
		template< typename U, uint32_t _Count > friend class Coords;

		typedef T									value_type;
		typedef value_type 			&			reference;
		typedef value_type 			*			pointer;
		typedef value_type const 		&			const_reference;
		typedef value_type const 		*			const_pointer;
		typedef Coords< value_type, Count > 	&	point_reference;
		typedef Coords< value_type, Count > 	*	point_pointer;
		typedef Coords< value_type, Count > const &	const_point_reference;
		typedef Coords< value_type, Count > const *	const_point_pointer;
		static const std::size_t					binary_size = sizeof( T ) * Count;

	public:
		//!\~english Typedef over a pointer on data			\~french Typedef d'un pointeur sur les donn�es
		typedef value_type 			*			iterator;
		//!\~english Typedef over a constant pointer on data	\~french Typedef d'un pointeur constant sur les donn�es
		typedef value_type const 		*			const_iterator;

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
		 *\param[in]	p_point	The Coords object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_point	L'objet Coords � copier
		 */
		Coords( Coords< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_point	The Coords object to move
		 *\~french
		 *\brief		Constructeur par d�placement
		 *\param[in]	p_point	L'objet Coords � d�placer
		 */
		Coords( Coords< T, Count > && p_point );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pValues	The data buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pValues	Le buffer de donn�es
		 */
		Coords( T * p_pValues );
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
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_ptA, p_ptB	The points to add
	 *\return		The addition result
	 *\~french
	 *\brief		Op�rateur d'addition
	 *\param[in]	p_ptA, p_ptB	Les points � additionner
	 *\return		Le r�sultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator +( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_ptA, p_ptB	The points to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Op�rateur de soustraction
	 *\param[in]	p_ptA, p_ptB	Les points � soustraire
	 *\return		Le r�sultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator -( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Op�rateur de multiplication
	 *\param[in]	p_ptA, p_ptB	Les points � multiplier
	 *\return		Le r�sultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator *( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_ptA, p_ptB	The points to divide
	 *\return		The division result
	 *\~french
	 *\brief		Op�rateur de division
	 *\param[in]	p_ptA, p_ptB	Les points � diviser
	 *\return		Le r�sultat de la division
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator /( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Vectorial product operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The vectorial product
	 *\~french
	 *\brief		Op�rateur de produit vectoriel
	 *\param[in]	p_ptA, p_ptB	Les points � multiplier
	 *\return		Le r�sultat du produit vectoriel
	 */
	template< typename T, typename U > Point< T, 3 > operator ^( Coords< T, 3 > const & p_ptA, Coords< U, 3 > const & p_ptB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to add
	 *\return		The addition result
	 *\~french
	 *\brief		Op�rateur d'addition
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs � additionner
	 *\return		Le r�sultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator +( Coords< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Op�rateur de soustraction
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs � soustraire
	 *\return		Le r�sultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator -( Coords< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Op�rateur de multiplication
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs � mulitplier
	 *\return		Le r�sultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator *( Coords< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pt		The point to divide
	 *\param[in]	p_coords	The values
	 *\return		The division result
	 *\~french
	 *\brief		Op�rateur de division
	 *\param[in]	p_pt		Le point � diviser
	 *\param[in]	p_coords	Les valeurs
	 *\return		Le r�sultat de la division
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator /( Coords< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pt	The point to multiply
	 *\param[in]	p_coord	The value
	 *\return		The multiplication result
	 *\~french
	 *\brief		Op�rateur de multiplication
	 *\param[in]	p_pt	Le point � multiplier
	 *\param[in]	p_coord	La valeur
	 *\return		Le r�sultat de la multiplication
	 */
	template< typename T, uint32_t Count > Point< T, Count > operator *( Coords< T, Count > const & p_pt, T const & p_coord );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pt	The point to divide
	 *\param[in]	p_coord	The value
	 *\return		The division result
	 *\~french
	 *\brief		Op�rateur de division
	 *\param[in]	p_pt	Le point � diviser
	 *\param[in]	p_coord	La valeur
	 *\return		Le r�sultat de la division
	 */
	template< typename T, uint32_t Count > Point< T, Count > operator /( Coords< T, Count > const & p_pt, T const & p_coord );
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
	class Point
	{
	private:
		template< typename U, uint32_t _Count > friend class Point;

		typedef T									value_type;
		typedef value_type 			&			reference;
		typedef value_type 			*			pointer;
		typedef value_type const 		&			const_reference;
		typedef value_type const 		*			const_pointer;
		typedef Point< value_type, Count > 	&	point_reference;
		typedef Point< value_type, Count > 	*	point_pointer;
		typedef Point< value_type, Count > const &	const_point_reference;
		typedef Point< value_type, Count > const *	const_point_pointer;
		static const std::size_t					binary_size = sizeof( T ) * Count;

	public:
		//!\~english Typedef over a pointer on data			\~french Typedef d'un pointeur sur les donn�es
		typedef value_type 			*			iterator;
		//!\~english Typedef over a constant pointer on data	\~french Typedef d'un pointeur constant sur les donn�es
		typedef value_type const 		*			const_iterator;

	private:
		T m_coords[Count];

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Point();
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_point	The Point object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_point	L'objet Point � copier
		 */
		Point( Point< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_point	The Point object to move
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_point	L'objet Point � d�placer
		 */
		Point( Point< T, Count > && p_point );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_point	The Point object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_point	L'objet Point � copier
		 */
		Point( Coords< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pValues	The data buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pValues	Le buffer de donn�es
		 */
		Point( T const * p_pValues );
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
		 *\param[in]	p_pValues	The data buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pValues	Le buffer de donn�es
		 */
		template< typename U > Point( U const * p_pValues );
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
		 *\brief		Op�rateur d'affectation par copie
		 *\param[in]	p_pt	L'objet Point � copier
		 *\return		Une r�f�rence sur cet objet Point
		 */
		Point< T, Count > & operator =( Point< T, Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pt	The Point object to move
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par d�placement
		 *\param[in]	p_pt	L'objet Point � d�placer
		 *\return		Une r�f�rence sur cet objet Point
		 */
		Point< T, Count > & operator =( Point< T, Count > && p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_pt	The Point object to add
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Point � ajouter
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator +=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Point object to substract
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Point � soustraire
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator -=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Point object to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Point � multiplier
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator *=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Point object to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par division
		 *\param[in]	p_pt	L'objet Point � diviser
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator /=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_pt	The Point object to add
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Point � ajouter
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator +=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Point object to substract
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Point � soustraire
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator -=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Point object to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Point � multiplier
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator *=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Point object to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par division
		 *\param[in]	p_pt	L'objet Point � diviser
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator /=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_coords	The values to add
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par addition
		 *\param[in]	p_coords	Les valeurs � additionner
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator +=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_coords	The values to substract
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par soustraction
		 *\param[in]	p_coords	Les valeurs � soustraire
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator -=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coords	The values to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par multiplication
		 *\param[in]	p_coords	Les valeurs � multiplier
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator *=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coords	The values to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par division
		 *\param[in]	p_coords	Les valeurs � diviser
		 *\return		Une r�f�rence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator /=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coord	The value to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par multiplication
		 *\param[in]	p_coord	La valeur � multiplier
		 *\return		Une r�f�rence sur cet objet Point
		 */
		Point< T, Count > & operator *=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coord	The value to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Op�rateur d'affectation par division
		 *\param[in]	p_coord	La valeur � diviser
		 *\return		Une r�f�rence sur cet objet Point
		 */
		Point< T, Count > & operator /=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Swaps this pont data with the parameter ones
		 *\param[in]	p_pt	The point to swap
		 *\~french
		 *\brief		Echange les donn�es de c epoint avec celles du point donn�
		 *\param[in]	p_pt	Le point � �changer
		 */
		void swap( Point< T, Count > & p_pt );
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
		 *\brief		Retrieves the total size of the point
		 *\return		count() * elem_size()
		 *\~french
		 *\brief		R�cup�re la taille totale du point
		 *\return		count() * elem_size()
		 */
		inline	std::size_t	size()const
		{
			return binary_size;
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
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		R�cup�re le pointeur sur les donn�es
		 *\return		Le pointeur
		 */
		inline T * ptr()
		{
			return &m_coords[0];
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
			return &m_coords[0];
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
			return &m_coords[0];
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
			return &m_coords[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le dernier �l�ment
		 *\return		L'it�rateur
		 */
		inline const_iterator end()const
		{
			return &m_coords[0] + Count;
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
			return &m_coords[0] + Count;
		}
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator ==( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator !=( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_ptA, p_ptB	The points to add
	 *\return		The addition result
	 *\~french
	 *\brief		Op�rateur d'addition
	 *\param[in]	p_ptA, p_ptB	Les points � additionner
	 *\return		Le r�sultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator +( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_ptA, p_ptB	The points to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Op�rateur de soustraction
	 *\param[in]	p_ptA, p_ptB	Les points � soustraire
	 *\return		Le r�sultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator -( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Op�rateur de multiplication
	 *\param[in]	p_ptA, p_ptB	Les points � multiplier
	 *\return		Le r�sultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator *( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_ptA, p_ptB	The points to divide
	 *\return		The division result
	 *\~french
	 *\brief		Op�rateur de division
	 *\param[in]	p_ptA, p_ptB	Les points � diviser
	 *\return		Le r�sultat de la division
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator /( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Vectorial product operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The vectorial product
	 *\~french
	 *\brief		Op�rateur de produit vectoriel
	 *\param[in]	p_ptA, p_ptB	Les points � multiplier
	 *\return		Le r�sultat du produit vectoriel
	 */
	template< typename T, typename U > Point< T, 3 > operator ^( Point< T, 3 > const & p_ptA,  Point< U, 3 > const & p_ptB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to add
	 *\return		The addition result
	 *\~french
	 *\brief		Op�rateur d'addition
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs � additionner
	 *\return		Le r�sultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator +( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Op�rateur de soustraction
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs � soustraire
	 *\return		Le r�sultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator -( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Op�rateur de multiplication
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs � mulitplier
	 *\return		Le r�sultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator *( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pt		The point to divide
	 *\param[in]	p_coords	The values
	 *\return		The division result
	 *\~french
	 *\brief		Op�rateur de division
	 *\param[in]	p_pt		Le point � diviser
	 *\param[in]	p_coords	Les valeurs
	 *\return		Le r�sultat de la division
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator /( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pt	The point to multiply
	 *\param[in]	p_coord	The value
	 *\return		The multiplication result
	 *\~french
	 *\brief		Op�rateur de multiplication
	 *\param[in]	p_pt	Le point � multiplier
	 *\param[in]	p_coord	La valeur
	 *\return		Le r�sultat de la multiplication
	 */
	template< typename T, uint32_t Count > Point< T, Count > operator *( Point< T, Count > const & p_pt, T const & p_coord );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pt	The point to divide
	 *\param[in]	p_coord	The value
	 *\return		The division result
	 *\~french
	 *\brief		Op�rateur de division
	 *\param[in]	p_pt	Le point � diviser
	 *\param[in]	p_coord	La valeur
	 *\return		Le r�sultat de la division
	 */
	template< typename T, uint32_t Count > Point< T, Count > operator /( Point< T, Count > const & p_pt, T const & p_coord );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator ==( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator !=( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_ptA, p_ptB	The points to add
	 *\return		The addition result
	 *\~french
	 *\brief		Op�rateur d'addition
	 *\param[in]	p_ptA, p_ptB	Les points � additionner
	 *\return		Le r�sultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator +( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_ptA, p_ptB	The points to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Op�rateur de soustraction
	 *\param[in]	p_ptA, p_ptB	Les points � soustraire
	 *\return		Le r�sultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator -( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Op�rateur de multiplication
	 *\param[in]	p_ptA, p_ptB	Les points � multiplier
	 *\return		Le r�sultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator *( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_ptA, p_ptB	The points to divide
	 *\return		The division result
	 *\~french
	 *\brief		Op�rateur de division
	 *\param[in]	p_ptA, p_ptB	Les points � diviser
	 *\return		Le r�sultat de la division
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator /( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Vectorial product operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The vectorial product
	 *\~french
	 *\brief		Op�rateur de produit vectoriel
	 *\param[in]	p_ptA, p_ptB	Les points � multiplier
	 *\return		Le r�sultat du produit vectoriel
	 */
	template< typename T, typename U > Point< T, 3 > operator ^( Point< T, 3 > const & p_ptA, Coords< U, 3 > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator ==( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
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
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator !=( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_ptA, p_ptB	The points to add
	 *\return		The addition result
	 *\~french
	 *\brief		Op�rateur d'addition
	 *\param[in]	p_ptA, p_ptB	Les points � additionner
	 *\return		Le r�sultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator +( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_ptA, p_ptB	The points to substract
	 *\return		The substraction result
	 *\~french
	 *\brief		Op�rateur de soustraction
	 *\param[in]	p_ptA, p_ptB	Les points � soustraire
	 *\return		Le r�sultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator -( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Op�rateur de multiplication
	 *\param[in]	p_ptA, p_ptB	Les points � multiplier
	 *\return		Le r�sultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator * ( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_ptA, p_ptB	The points to divide
	 *\return		The division result
	 *\~french
	 *\brief		Op�rateur de division
	 *\param[in]	p_ptA, p_ptB	Les points � diviser
	 *\return		Le r�sultat de la division
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator /( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Vectorial product operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The vectorial product
	 *\~french
	 *\brief		Op�rateur de produit vectoriel
	 *\param[in]	p_ptA, p_ptB	Les points � multiplier
	 *\return		Le r�sultat du produit vectoriel
	 */
	template< typename T, typename U > Point< T, 3 > operator ^( Coords< T, 3 > const & p_ptA, Point< U, 3 > const & p_ptB );
	/**
	 *\~english
	 *\brief		Negation operator
	 *\param[in]	p_pt	The point to negate
	 *\~french
	 *\brief		Op�rateur de n�gation
	 *\param[in]	p_pt	Le point � n�gativer
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
		 *\param[in]	p_point	The point
		 *\~french
		 *\brief		N�gative chaque donn�e du point donn�
		 *\param[in]	p_point	Le point
		 */
		template< typename T, uint32_t Count > static void negate( Point< T, Count > & p_point );
		/**
		 *\~english
		 *\brief		Normalises the point
		 *\remark		A point is normalised when it's length equals 1.0
						<br />So each data is divided by the point's length
		 *\param[in]	p_point	The point
		 *\~french
		 *\brief		Normalise le point
		 *\remark		Un point est normalis� quand sa longueur vaut 1.0
						<br />Chaque donn�e est donc divis�e par la longueur du point
		 *\param[in]	p_point	Le point
						*/
		template< typename T, uint32_t Count > static void normalise( Point< T, Count > & p_point );
		/**
		 *\~english
		 *\brief		Returns the normalised form of a point
		 *\param[in]	p_point	The point
		 *\~french
		 *\brief		Retourne la forme normalis�e d'un point
		 *\param[in]	p_point	Le point
		 */
		template< typename T, uint32_t Count > static Point< T, Count > get_normalised( Point< T, Count > const & p_point );
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
		 *\brief		Calcule le cosinus trigonom�trique de l'angle entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le cosinus
		 */
		template< typename T, uint32_t Count > static double cos_theta( Point< T, Count > const & p_ptA, Point< T, Count > const & p_ptB );
		/**
		 *\~english
		 *\brief		Computes the squared Euclidian distance of the point
		 *\param[in]	p_point	The point
		 *\return		The squared Euclidian distance
		 *\~french
		 *\brief		Calcule le carr� de la norme Euclidienne du point
		 *\param[in]	p_point	Le point
		 *\return		Le carr� de la norme Euclidienne
		 */
		template< typename T, uint32_t Count > static double distance_squared( Point< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Computes the Euclidian distance of the point
		 *\param[in]	p_point	The point
		 *\return		The Euclidian distance
		 *\~french
		 *\brief		Calcule la norme Euclidienne du point
		 *\param[in]	p_point	Le point
		 *\return		La norme Euclidienne
		 */
		template< typename T, uint32_t Count > static double distance( Point< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Computes the Manhattan distance of the point
		 *\param[in]	p_point	The point
		 *\return		The Manhattan distance
		 *\~french
		 *\brief		Calcule la distance de Manhattan du point
		 *\param[in]	p_point	Le point
		 *\return		La distance de Manhattan
		 */
		template< typename T, uint32_t Count > static double distance_manhattan( Point< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Computes the Minkowski distance of the point
		 *\param[in]	p_point	The point
		 *\param[in]	p_dOrder	The Minkowski order
		 *\return		The Minkowski distance
		 *\~french
		 *\brief		Calcule la distance de Minkowski du point
		 *\param[in]	p_point	Le point
		 *\param[in]	p_dOrder	L'ordre Minkownski
		 *\return		La distance de Minkowski
		 */
		template< typename T, uint32_t Count > static double distance_minkowski( Point< T, Count > const & p_point, double p_dOrder );
		/**
		 *\~english
		 *\brief		Computes the Chebychev distance of the point
		 *\param[in]	p_point	The point
		 *\return		The Chebychev distance
		 *\~french
		 *\brief		Calcule la distance de Chebychev du point
		 *\param[in]	p_point	Le point
		 *\return		La distance de Chebychev
		 */
		template< typename T, uint32_t Count > static double distance_chebychev( Point< T, Count > const & p_point );
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
		 *\brief		Calcule le cosinus trigonom�trique de l'angle entre 2 points
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
		 *\brief		Calcule le cosinus trigonom�trique de l'angle entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le cosinus
		 */
		template< typename T, uint32_t Count > static double cos_theta( Point< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB );
		/**
		 *\~english
		 *\brief		Negates every value of a point's data
		 *\param[in]	p_point	The point
		 *\~french
		 *\brief		N�gative chaque donn�e du point donn�
		 *\param[in]	p_point	Le point
		 */
		template< typename T, uint32_t Count > static void negate( Coords< T, Count > & p_point );
		/**
		 *\~english
		 *\brief		Normalises the point
		 *\remark		A point is normalised when it's length equals 1.0
						<br />So each data is divided by the point's length
		 *\param[in]	p_point	The point
		 *\~french
		 *\brief		Normalise le point
		 *\remark		Un point est normalis� quand sa longueur vaut 1.0
						<br />Chaque donn�e est donc divis�e par la longueur du point
		 *\param[in]	p_point	Le point
						*/
		template< typename T, uint32_t Count > static void normalise( Coords< T, Count > & p_point );
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
		 *\param[in]	p_point	The point
		 *\return		The squared Euclidian distance
		 *\~french
		 *\brief		Calcule le carr� de la norme Euclidienne du point
		 *\param[in]	p_point	Le point
		 *\return		Le carr� de la norme Euclidienne
		 */
		template< typename T, uint32_t Count > static double distance_squared( Coords< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Computes the Euclidian distance of the point
		 *\param[in]	p_point	The point
		 *\return		The Euclidian distance
		 *\~french
		 *\brief		Calcule la norme Euclidienne du point
		 *\param[in]	p_point	Le point
		 *\return		La norme Euclidienne
		 */
		template< typename T, uint32_t Count > static double distance( Coords< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Computes the Manhattan distance of the point
		 *\param[in]	p_point	The point
		 *\return		The Manhattan distance
		 *\~french
		 *\brief		Calcule la distance de Manhattan du point
		 *\param[in]	p_point	Le point
		 *\return		La distance de Manhattan
		 */
		template< typename T, uint32_t Count > static double distance_manhattan( Coords< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Computes the Minkowski distance of the point
		 *\param[in]	p_point	The point
		 *\param[in]	p_dOrder	The Minkowski order
		 *\return		The Minkowski distance
		 *\~french
		 *\brief		Calcule la distance de Minkowski du point
		 *\param[in]	p_point	Le point
		 *\param[in]	p_dOrder	L'ordre Minkownski
		 *\return		La distance de Minkowski
		 */
		template< typename T, uint32_t Count > static double distance_minkowski( Coords< T, Count > const & p_point, double p_dOrder );
		/**
		 *\~english
		 *\brief		Computes the Chebychev distance of the point
		 *\param[in]	p_point	The point
		 *\return		The Chebychev distance
		 *\~french
		 *\brief		Calcule la distance de Chebychev du point
		 *\param[in]	p_point	Le point
		 *\return		La distance de Chebychev
		 */
		template< typename T, uint32_t Count > static double distance_chebychev( Coords< T, Count > const & p_point );
	};
}

#include "Point.inl"

#endif

#endif
