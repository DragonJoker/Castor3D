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
#ifndef ___CASTOR_SIZE_H___
#define ___CASTOR_SIZE_H___

#include "Math/Coords.hpp"

namespace Castor
{
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
	class Size
		: public Coords< uint32_t, 2 >
	{
	private:
		typedef Coords< uint32_t, 2 > BaseType;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_width, p_height	The dimensions
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_width, p_height	Les dimensions
		 */
		CU_API Size( uint32_t p_width = 0, uint32_t p_height = 0 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_obj	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_obj	L'objet à copier
		 */
		CU_API Size( Size const & p_obj );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_obj	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_obj	L'objet à déplacer
		 */
		CU_API Size( Size && p_obj );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~Size();
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
		CU_API Size & operator =( Size const & p_obj );
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
		CU_API Size & operator =( Size && p_obj );
		/**
		 *\~english
		 *\brief		Sets the size values
		 *\param[in]	p_width, p_height	The dimensions
		 *\~french
		 *\brief		Définit la taille
		 *\param[in]	p_width, p_height	Les dimensions
		 */
		CU_API void set( uint32_t p_width, uint32_t p_height );
		/**
		 *\~english
		 *\brief		Retrieves the width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur
		 *\return		La largeur
		 */
		inline uint32_t width()const
		{
			return m_data.size.cx;
		}
		/**
		 *\~english
		 *\brief		Retrieves the width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur
		 *\return		La largeur
		 */
		inline uint32_t & width()
		{
			return m_data.size.cx;
		}
		/**
		 *\~english
		 *\brief		Retrieves the height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur
		 *\return		La hauteur
		 */
		inline uint32_t height()const
		{
			return m_data.size.cy;
		}
		/**
		 *\~english
		 *\brief		Retrieves the height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur
		 *\return		La hauteur
		 */
		inline uint32_t & height()
		{
			return m_data.size.cy;
		}
		/**
		 *\~english
		 *\brief		Modifies the size
		 *\remarks		If width+cx < 0 (or height+cy < 0) then width=0 (respectively height=0)
		 *\param[in]	p_cx, p_cy	The size modifications
		 *\~french
		 *\brief		Modifie la taille
		 *\remarks		Si width+cx < 0 (ou height+cy < 0) alors width=0 (respectivement height=0)
		 *\param[in]	p_cx, p_cy	Les valeurs de modification
		 */
		CU_API void grow( int32_t p_cx, int32_t p_cy );

		using BaseType::ptr;
		using BaseType::const_ptr;

	private:
		union
		{
			struct
			{
				uint32_t cx;
				uint32_t cy;
			}			size;
			uint32_t	buffer[2];
		}	m_data;
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
	CU_API bool operator ==( Size const & p_a, Size const & p_b );
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
	CU_API bool operator !=( Size const & p_a, Size const & p_b );
}

#endif
