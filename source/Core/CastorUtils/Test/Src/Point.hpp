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
	\brief		Classe de points à dimensions statiques
	\remark		Peut recevoir les types de donnàes qui ont une Policy crààe
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
		//!\~english Typedef over a pointer on data			\~french Typedef d'un pointeur sur les donnàes
		typedef value_type 			*			iterator;
		//!\~english Typedef over a constant pointer on data	\~french Typedef d'un pointeur constant sur les donnàes
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
		 *\param[in]	p_point	L'objet Coords à copier
		 */
		Coords( Coords< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_point	The Coords object to move
		 *\~french
		 *\brief		Constructeur par dàplacement
		 *\param[in]	p_point	L'objet Coords à dàplacer
		 */
		Coords( Coords< T, Count > && p_point );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pValues	The data buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pValues	Le buffer de donnàes
		 */
		Coords( T * p_pValues );
		/**
		 *\~english
		 *\brief		Destructor
		 *\remark		Not virtual but it should be ok to derive Coords because destructor is dummy
		 *\~french
		 *\brief		Destructeur
		 *\remark		Non virtuel mais il devrait àtre possible de dàriver Coords car le destructeur ne fait rien
		 */
		~Coords();
		/**
		 *\~english
		 *\brief		Assignment operator
		 *\param[in]	p_pValues	The values to affect
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation
		 *\param[in]	p_pValues	Les valeurs à affecter
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		Coords< T, Count > & operator =( T * p_pValues );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pt	The Coords object to copy
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par copie
		 *\param[in]	p_pt	L'objet Coords à copier
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		Coords< T, Count > & operator =( Coords< T, Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pt	The Coords object to move
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par dàplacement
		 *\param[in]	p_pt	L'objet Coords à dàplacer
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		Coords< T, Count > & operator =( Coords< T, Count > && p_pt );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pt	The Point object to copy
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par copie
		 *\param[in]	p_pt	L'objet Point à copier
		 *\return		Une ràfàrence sur cet objet Point
		 */
		Coords< T, Count > & operator =( Point< T, Count > const & p_pt );
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	p_pt	The Coords object to add
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Coords à ajouter
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator +=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Coords object to subtract
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Coords à soustraire
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator -=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Coords object to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Coords à multiplier
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator *=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Coords object to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par division
		 *\param[in]	p_pt	L'objet Coords à diviser
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator /=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	p_pt	The Coords object to add
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Coords à ajouter
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator +=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Coords object to subtract
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Coords à soustraire
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator -=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Coords object to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Coords à multiplier
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator *=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Coords object to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par division
		 *\param[in]	p_pt	L'objet Coords à diviser
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U, uint32_t _Count > Coords< T, Count > & operator /=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	p_coords	The values to add
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par addition
		 *\param[in]	p_coords	Les valeurs à additionner
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator +=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_coords	The values to subtract
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par soustraction
		 *\param[in]	p_coords	Les valeurs à soustraire
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator -=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coords	The values to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par multiplication
		 *\param[in]	p_coords	Les valeurs à multiplier
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator *=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coords	The values to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par division
		 *\param[in]	p_coords	Les valeurs à diviser
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		template< typename U >	Coords< T, Count > & operator /=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coord	The value to multiply
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par multiplication
		 *\param[in]	p_coord	La valeur à multiplier
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		Coords< T, Count > & operator *=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coord	The value to divide
		 *\return		A reference to this Coords object
		 *\~french
		 *\brief		Opàrateur d'affectation par division
		 *\param[in]	p_coord	La valeur à diviser
		 *\return		Une ràfàrence sur cet objet Coords
		 */
		Coords< T, Count > & operator /=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Swaps this pont data with the parameter ones
		 *\param[in]	p_pt	The point to swap
		 *\~french
		 *\brief		Echange les donnàes de c epoint avec celles du point donné
		 *\param[in]	p_pt	Le point à àchanger
		 */
		void swap( Coords< T, Count > & p_pt );
		/**
		 *\~english
		 *\brief		Inverts data in the point
		 *\remark		The first becomes last and so on
		 *\~french
		 *\brief		Inverse les donnàes dans ce point
		 *\remark		Les premiers deviennent les derniers
		 */
		void flip();
		/**
		 *\~english
		 *\brief		Retrieves the total size of the point
		 *\return		count() * elemSize()
		 *\~french
		 *\brief		Ràcupàre la taille totale du point
		 *\return		count() * elemSize()
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
		 *\brief			Ràcupàre les donnàes et les place dans un tableau
		 *\param[in,out]	p_pResult	Reàoit les donnàes, doit àtre allouà par l'appelant
		 */
		void toValues( T * p_pResult )const;
		/**
		 *\~english
		 *\brief		Retrieves the number of coordinates
		 *\return		The number of coordinates
		 *\~french
		 *\brief		Ràcupàre le nombre de coordonnàes
		 *\return		Le nombre de coordonnàes
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
		 *\brief		Ràcupàre la taille du type donnàes
		 *\return		La taille du type de donnàes
		 */
		inline std::size_t elemSize()const
		{
			return sizeof( T );
		}
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\return		A constant reference on data at wanted index
		 *\~french
		 *\brief		Ràcupàre la donnàe à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une ràfàrence constante sur la donnàe à l'index voulu
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
		 *\brief		Ràcupàre la donnàe à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une ràfàrence sur la donnàe à l'index voulu
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
		 *\brief		Ràcupàre la donnàe à l'index donné
		 *\remark		Cette fonction vàrifie l'index et lance une exception s'il est hors bornes
		 *\return		Une ràfàrence constante sur la donnàe à l'index voulu
		 */
		T const & at( uint32_t p_pos )const;
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		This fonction checks the index and throws an exception if it is out of bounds
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		Ràcupàre la donnàe à l'index donné
		 *\remark		Cette fonction vàrifie l'index et lance une exception s'il est hors bornes
		 *\return		Une ràfàrence sur la donnàe à l'index voulu
		 */
		T & at( uint32_t p_pos );
		/**
		 *\~english
		 *\brief		Retrieves a non constant pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Ràcupàre un pointeur non constant sur les donnàes
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
		 *\brief		Ràcupàre un pointeur constant sur les donnàes
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
		 *\brief		Ràcupàre un itàrateur sur le premier àlàment
		 *\return		L'itàrateur
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
		 *\brief		Ràcupàre un itàrateur constant sur le premier àlàment
		 *\return		L'itàrateur
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
		 *\brief		Ràcupàre un itàrateur sur le dernier àlàment
		 *\return		L'itàrateur
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
		 *\brief		Ràcupàre un itàrateur constant sur le dernier àlàment
		 *\return		L'itàrateur
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
	 *\brief		Opàrateur d'àgalità
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont les màmes dimensions et les màmes valeurs
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator ==( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have different dimensions or at least one different value
	 *\~french
	 *\brief		Opàrateur d'inàgalità
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont des dimensions diffàrentes ou au moins une valeur diffàrente
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator !=( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	p_ptA, p_ptB	The points to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opàrateur d'addition
	 *\param[in]	p_ptA, p_ptB	Les points à additionner
	 *\return		Le ràsultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator +( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_ptA, p_ptB	The points to subtract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opàrateur de soustraction
	 *\param[in]	p_ptA, p_ptB	Les points à soustraire
	 *\return		Le ràsultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator -( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opàrateur de multiplication
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le ràsultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator *( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_ptA, p_ptB	The points to divide
	 *\return		The division result
	 *\~french
	 *\brief		Opàrateur de division
	 *\param[in]	p_ptA, p_ptB	Les points à diviser
	 *\return		Le ràsultat de la division
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator /( Coords< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Vectorial product operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The vectorial product
	 *\~french
	 *\brief		Opàrateur de produit vectoriel
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le ràsultat du produit vectoriel
	 */
	template< typename T, typename U > Point< T, 3 > operator ^( Coords< T, 3 > const & p_ptA, Coords< U, 3 > const & p_ptB );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opàrateur d'addition
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs à additionner
	 *\return		Le ràsultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator +( Coords< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to subtract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opàrateur de soustraction
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs à soustraire
	 *\return		Le ràsultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator -( Coords< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opàrateur de multiplication
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs à mulitplier
	 *\return		Le ràsultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator *( Coords< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pt		The point to divide
	 *\param[in]	p_coords	The values
	 *\return		The division result
	 *\~french
	 *\brief		Opàrateur de division
	 *\param[in]	p_pt		Le point à diviser
	 *\param[in]	p_coords	Les valeurs
	 *\return		Le ràsultat de la division
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator /( Coords< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pt	The point to multiply
	 *\param[in]	p_coord	The value
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opàrateur de multiplication
	 *\param[in]	p_pt	Le point à multiplier
	 *\param[in]	p_coord	La valeur
	 *\return		Le ràsultat de la multiplication
	 */
	template< typename T, uint32_t Count > Point< T, Count > operator *( Coords< T, Count > const & p_pt, T const & p_coord );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pt	The point to divide
	 *\param[in]	p_coord	The value
	 *\return		The division result
	 *\~french
	 *\brief		Opàrateur de division
	 *\param[in]	p_pt	Le point à diviser
	 *\param[in]	p_coord	La valeur
	 *\return		Le ràsultat de la division
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
	\remark		Peut recevoir les types de donnàes qui ont une Policy crààe
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
		//!\~english Typedef over a pointer on data			\~french Typedef d'un pointeur sur les donnàes
		typedef value_type 			*			iterator;
		//!\~english Typedef over a constant pointer on data	\~french Typedef d'un pointeur constant sur les donnàes
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
		 *\param[in]	p_point	L'objet Point à copier
		 */
		Point( Point< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_point	The Point object to move
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_point	L'objet Point à dàplacer
		 */
		Point( Point< T, Count > && p_point );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_point	The Point object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_point	L'objet Point à copier
		 */
		Point( Coords< T, Count > const & p_point );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pValues	The data buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pValues	Le buffer de donnàes
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
		 *\param[in]	p_pValues	Le buffer de donnàes
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
		 *\brief		Opàrateur d'affectation par copie
		 *\param[in]	p_pt	L'objet Point à copier
		 *\return		Une ràfàrence sur cet objet Point
		 */
		Point< T, Count > & operator =( Point< T, Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pt	The Point object to move
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par dàplacement
		 *\param[in]	p_pt	L'objet Point à dàplacer
		 *\return		Une ràfàrence sur cet objet Point
		 */
		Point< T, Count > & operator =( Point< T, Count > && p_pt );
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	p_pt	The Point object to add
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Point à ajouter
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator +=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Point object to subtract
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Point à soustraire
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator -=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Point object to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Point à multiplier
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator *=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Point object to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par division
		 *\param[in]	p_pt	L'objet Point à diviser
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator /=( Point< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	p_pt	The Point object to add
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par addition
		 *\param[in]	p_pt	L'objet Point à ajouter
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator +=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_pt	The Point object to subtract
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par soustraction
		 *\param[in]	p_pt	L'objet Point à soustraire
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator -=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_pt	The Point object to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par multiplication
		 *\param[in]	p_pt	L'objet Point à multiplier
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator *=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_pt	The Point object to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par division
		 *\param[in]	p_pt	L'objet Point à diviser
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U, uint32_t _Count > Point< T, Count > & operator /=( Coords< U, _Count > const & p_pt );
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	p_coords	The values to add
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par addition
		 *\param[in]	p_coords	Les valeurs à additionner
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator +=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_coords	The values to subtract
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par soustraction
		 *\param[in]	p_coords	Les valeurs à soustraire
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator -=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coords	The values to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par multiplication
		 *\param[in]	p_coords	Les valeurs à multiplier
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator *=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coords	The values to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par division
		 *\param[in]	p_coords	Les valeurs à diviser
		 *\return		Une ràfàrence sur cet objet Point
		 */
		template< typename U > Point< T, Count > & operator /=( U const * p_coords );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_coord	The value to multiply
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par multiplication
		 *\param[in]	p_coord	La valeur à multiplier
		 *\return		Une ràfàrence sur cet objet Point
		 */
		Point< T, Count > & operator *=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_coord	The value to divide
		 *\return		A reference to this Point object
		 *\~french
		 *\brief		Opàrateur d'affectation par division
		 *\param[in]	p_coord	La valeur à diviser
		 *\return		Une ràfàrence sur cet objet Point
		 */
		Point< T, Count > & operator /=( T const & p_coord );
		/**
		 *\~english
		 *\brief		Swaps this pont data with the parameter ones
		 *\param[in]	p_pt	The point to swap
		 *\~french
		 *\brief		Echange les donnàes de c epoint avec celles du point donné
		 *\param[in]	p_pt	Le point à àchanger
		 */
		void swap( Point< T, Count > & p_pt );
		/**
		 *\~english
		 *\brief		Inverts data in the point
		 *\remark		The first becomes last and so on
		 *\~french
		 *\brief		Inverse les donnàes dans ce point
		 *\remark		Les premiers deviennent les derniers
		 */
		void flip();
		/**
		 *\~english
		 *\brief			Retrieves data from the point and put it into an array
		 *\param[in,out]	p_pResult	Receives the point data, needs to be allocated by the caller
		 *\~french
		 *\brief			Ràcupàre les donnàes et les place dans un tableau
		 *\param[in,out]	p_pResult	Reàoit les donnàes, doit àtre allouà par l'appelant
		 */
		void toValues( T * p_pResult )const;
		/**
		 *\~english
		 *\brief		Retrieves the number of coordinates
		 *\return		The number of coordinates
		 *\~french
		 *\brief		Ràcupàre le nombre de coordonnàes
		 *\return		Le nombre de coordonnàes
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
		 *\brief		Ràcupàre la taille du type donnàes
		 *\return		La taille du type de donnàes
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
		 *\brief		Ràcupàre la taille totale du point
		 *\return		count() * elemSize()
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
		 *\brief		Ràcupàre la donnàe à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une ràfàrence constante sur la donnàe à l'index voulu
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
		 *\brief		Ràcupàre la donnàe à l'index donné
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\return		Une ràfàrence sur la donnàe à l'index voulu
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
		 *\brief		Ràcupàre la donnàe à l'index donné
		 *\remark		Cette fonction vàrifie l'index et lance une exception s'il est hors bornes
		 *\return		Une ràfàrence constante sur la donnàe à l'index voulu
		 */
		T const & at( uint32_t p_pos )const;
		/**
		 *\~english
		 *\brief		Retrieves the data at given index
		 *\remark		This fonction checks the index and throws an exception if it is out of bounds
		 *\return		A reference on data at wanted index
		 *\~french
		 *\brief		Ràcupàre la donnàe à l'index donné
		 *\remark		Cette fonction vàrifie l'index et lance une exception s'il est hors bornes
		 *\return		Une ràfàrence sur la donnàe à l'index voulu
		 */
		T & at( uint32_t p_pos );
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Ràcupàre le pointeur sur les donnàes
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
		 *\brief		Ràcupàre un pointeur constant sur les donnàes
		 *\return		Le pointeur
		 */
		inline T const * constPtr()const
		{
			return &m_coords[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		Ràcupàre un itàrateur sur le premier àlàment
		 *\return		L'itàrateur
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
		 *\brief		Ràcupàre un itàrateur constant sur le premier àlàment
		 *\return		L'itàrateur
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
		 *\brief		Ràcupàre un itàrateur sur le dernier àlàment
		 *\return		L'itàrateur
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
		 *\brief		Ràcupàre un itàrateur sur le dernier àlàment
		 *\return		L'itàrateur
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
	 *\brief		Opàrateur d'àgalità
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont les màmes dimensions et les màmes valeurs
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator ==( Point< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have different dimensions or at least one different value
	 *\~french
	 *\brief		Opàrateur d'inàgalità
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont des dimensions diffàrentes ou au moins une valeur diffàrente
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> bool operator !=( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	p_ptA, p_ptB	The points to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opàrateur d'addition
	 *\param[in]	p_ptA, p_ptB	Les points à additionner
	 *\return		Le ràsultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator +( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_ptA, p_ptB	The points to subtract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opàrateur de soustraction
	 *\param[in]	p_ptA, p_ptB	Les points à soustraire
	 *\return		Le ràsultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator -( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opàrateur de multiplication
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le ràsultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator *( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_ptA, p_ptB	The points to divide
	 *\return		The division result
	 *\~french
	 *\brief		Opàrateur de division
	 *\param[in]	p_ptA, p_ptB	Les points à diviser
	 *\return		Le ràsultat de la division
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator /( Point< T, Count > const & p_ptA, Point< U, _Count >	const & p_ptB );
	/**
	 *\~english
	 *\brief		Vectorial product operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The vectorial product
	 *\~french
	 *\brief		Opàrateur de produit vectoriel
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le ràsultat du produit vectoriel
	 */
	template< typename T, typename U > Point< T, 3 > operator ^( Point< T, 3 > const & p_ptA,  Point< U, 3 > const & p_ptB );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opàrateur d'addition
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs à additionner
	 *\return		Le ràsultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator +( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to subtract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opàrateur de soustraction
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs à soustraire
	 *\return		Le ràsultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator -( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pt		The point
	 *\param[in]	p_coords	The values to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opàrateur de multiplication
	 *\param[in]	p_pt		Le point
	 *\param[in]	p_coords	Les valeurs à mulitplier
	 *\return		Le ràsultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator *( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pt		The point to divide
	 *\param[in]	p_coords	The values
	 *\return		The division result
	 *\~french
	 *\brief		Opàrateur de division
	 *\param[in]	p_pt		Le point à diviser
	 *\param[in]	p_coords	Les valeurs
	 *\return		Le ràsultat de la division
	 */
	template< typename T, uint32_t Count, typename U > Point< T, Count > operator /( Point< T, Count > const & p_pt, U const * p_coords );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_pt	The point to multiply
	 *\param[in]	p_coord	The value
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opàrateur de multiplication
	 *\param[in]	p_pt	Le point à multiplier
	 *\param[in]	p_coord	La valeur
	 *\return		Le ràsultat de la multiplication
	 */
	template< typename T, uint32_t Count > Point< T, Count > operator *( Point< T, Count > const & p_pt, T const & p_coord );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_pt	The point to divide
	 *\param[in]	p_coord	The value
	 *\return		The division result
	 *\~french
	 *\brief		Opàrateur de division
	 *\param[in]	p_pt	Le point à diviser
	 *\param[in]	p_coord	La valeur
	 *\return		Le ràsultat de la division
	 */
	template< typename T, uint32_t Count > Point< T, Count > operator /( Point< T, Count > const & p_pt, T const & p_coord );
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have same dimensions and same values
	 *\~french
	 *\brief		Opàrateur d'àgalità
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont les màmes dimensions et les màmes valeurs
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator ==( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have different dimensions or at least one different value
	 *\~french
	 *\brief		Opàrateur d'inàgalità
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont des dimensions diffàrentes ou au moins une valeur diffàrente
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator !=( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	p_ptA, p_ptB	The points to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opàrateur d'addition
	 *\param[in]	p_ptA, p_ptB	Les points à additionner
	 *\return		Le ràsultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator +( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_ptA, p_ptB	The points to subtract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opàrateur de soustraction
	 *\param[in]	p_ptA, p_ptB	Les points à soustraire
	 *\return		Le ràsultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator -( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opàrateur de multiplication
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le ràsultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator *( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_ptA, p_ptB	The points to divide
	 *\return		The division result
	 *\~french
	 *\brief		Opàrateur de division
	 *\param[in]	p_ptA, p_ptB	Les points à diviser
	 *\return		Le ràsultat de la division
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator /( Point< T, Count > const & p_ptA, Coords< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Vectorial product operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The vectorial product
	 *\~french
	 *\brief		Opàrateur de produit vectoriel
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le ràsultat du produit vectoriel
	 */
	template< typename T, typename U > Point< T, 3 > operator ^( Point< T, 3 > const & p_ptA, Coords< U, 3 > const & p_ptB );
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have same dimensions and same values
	 *\~french
	 *\brief		Opàrateur d'àgalità
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont les màmes dimensions et les màmes valeurs
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator ==( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_ptA, p_ptB	The points to compare
	 *\return		\p true if points have different dimensions or at least one different value
	 *\~french
	 *\brief		Opàrateur d'inàgalità
	 *\param[in]	p_ptA, p_ptB	Les points à comparer
	 *\return		\p true si les points ont des dimensions diffàrentes ou au moins une valeur diffàrente
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > bool operator !=( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	p_ptA, p_ptB	The points to add
	 *\return		The addition result
	 *\~french
	 *\brief		Opàrateur d'addition
	 *\param[in]	p_ptA, p_ptB	Les points à additionner
	 *\return		Le ràsultat de l'addition
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator +( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_ptA, p_ptB	The points to subtract
	 *\return		The substraction result
	 *\~french
	 *\brief		Opàrateur de soustraction
	 *\param[in]	p_ptA, p_ptB	Les points à soustraire
	 *\return		Le ràsultat de la soustraction
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator -( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The multiplication result
	 *\~french
	 *\brief		Opàrateur de multiplication
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le ràsultat de la multiplication
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count > Point< T, Count > operator * ( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_ptA, p_ptB	The points to divide
	 *\return		The division result
	 *\~french
	 *\brief		Opàrateur de division
	 *\param[in]	p_ptA, p_ptB	Les points à diviser
	 *\return		Le ràsultat de la division
	 */
	template< typename T, uint32_t Count, typename U, uint32_t _Count	> Point< T, Count > operator /( Coords< T, Count > const & p_ptA, Point< U, _Count > const & p_ptB );
	/**
	 *\~english
	 *\brief		Vectorial product operator
	 *\param[in]	p_ptA, p_ptB	The points to multiply
	 *\return		The vectorial product
	 *\~french
	 *\brief		Opàrateur de produit vectoriel
	 *\param[in]	p_ptA, p_ptB	Les points à multiplier
	 *\return		Le ràsultat du produit vectoriel
	 */
	template< typename T, typename U > Point< T, 3 > operator ^( Coords< T, 3 > const & p_ptA, Point< U, 3 > const & p_ptB );
	/**
	 *\~english
	 *\brief		Negation operator
	 *\param[in]	p_pt	The point to negate
	 *\~french
	 *\brief		Opàrateur de nàgation
	 *\param[in]	p_pt	Le point à nàgativer
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
		 *\brief		Nàgative chaque donnàe du point donné
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
		 *\remark		Un point est normalisà quand sa longueur vaut 1.0
						<br />Chaque donnàe est donc divisàe par la longueur du point
		 *\param[in]	p_point	Le point
						*/
		template< typename T, uint32_t Count > static void normalise( Point< T, Count > & p_point );
		/**
		 *\~english
		 *\brief		Returns the normalised form of a point
		 *\param[in]	p_point	The point
		 *\~french
		 *\brief		Retourne la forme normalisàe d'un point
		 *\param[in]	p_point	Le point
		 */
		template< typename T, uint32_t Count > static Point< T, Count > getNormalised( Point< T, Count > const & p_point );
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
		 *\brief		Calcule le cosinus trigonomàtrique de l'angle entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le cosinus
		 */
		template< typename T, uint32_t Count > static double cosTheta( Point< T, Count > const & p_ptA, Point< T, Count > const & p_ptB );
		/**
		 *\~english
		 *\brief		Computes the squared Euclidian distance of the point
		 *\param[in]	p_point	The point
		 *\return		The squared Euclidian distance
		 *\~french
		 *\brief		Calcule le carrà de la norme Euclidienne du point
		 *\param[in]	p_point	Le point
		 *\return		Le carrà de la norme Euclidienne
		 */
		template< typename T, uint32_t Count > static double distanceSquared( Point< T, Count > const & p_point );
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
		template< typename T, uint32_t Count > static double distanceManhattan( Point< T, Count > const & p_point );
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
		template< typename T, uint32_t Count > static double distanceMinkowski( Point< T, Count > const & p_point, double p_dOrder );
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
		template< typename T, uint32_t Count > static double distanceChebychev( Point< T, Count > const & p_point );
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
		 *\brief		Calcule le cosinus trigonomàtrique de l'angle entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le cosinus
		 */
		template< typename T, uint32_t Count > static double cosTheta( Coords< T, Count > const & p_ptA, Point< T, Count > const & p_ptB );
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
		 *\brief		Calcule le cosinus trigonomàtrique de l'angle entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le cosinus
		 */
		template< typename T, uint32_t Count > static double cosTheta( Point< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB );
		/**
		 *\~english
		 *\brief		Negates every value of a point's data
		 *\param[in]	p_point	The point
		 *\~french
		 *\brief		Nàgative chaque donnàe du point donné
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
		 *\remark		Un point est normalisà quand sa longueur vaut 1.0
						<br />Chaque donnàe est donc divisàe par la longueur du point
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
		 *\brief		Calcule le cosinus trigonomàtrique de l'angle entre 2 points
		 *\param[in]	p_ptA, p_ptB	Les points
		 *\return		Le cosinus
		 */
		template< typename T, uint32_t Count > static double cosTheta( Coords< T, Count > const & p_ptA, Coords< T, Count > const & p_ptB );
		/**
		 *\~english
		 *\brief		Computes the squared Euclidian distance of the point
		 *\param[in]	p_point	The point
		 *\return		The squared Euclidian distance
		 *\~french
		 *\brief		Calcule le carrà de la norme Euclidienne du point
		 *\param[in]	p_point	Le point
		 *\return		Le carrà de la norme Euclidienne
		 */
		template< typename T, uint32_t Count > static double distanceSquared( Coords< T, Count > const & p_point );
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
		template< typename T, uint32_t Count > static double distanceManhattan( Coords< T, Count > const & p_point );
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
		template< typename T, uint32_t Count > static double distanceMinkowski( Coords< T, Count > const & p_point, double p_dOrder );
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
		template< typename T, uint32_t Count > static double distanceChebychev( Coords< T, Count > const & p_point );
	};
}

#include "Point.inl"

#endif

#endif
