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
#ifndef ___CASTOR_CONTAINER_BOX_H___
#define ___CASTOR_CONTAINER_BOX_H___

#include "Math/Point.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Box container class
	\remark		A container will be a simple object which surrounds a graphic object (2D or 3D)
				<br />It can be a parallelepiped, a sphere or other.
	\~french
	\brief		Classe de conteneur boîte
	\remark		Un conteneur boîte est un simple objet encadrant un objet graphique (2D ou 3D)
				<br />Ce peut être un parallélépipède, une sphère ou autre.
	*/
	template< uint8_t Dimension >
	class ContainerBox
	{
	protected:
		typedef castor::Policy< real > policy;

	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		ContainerBox()
			:	m_ptCenter()
		{
		}
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_container	The container to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_container	Le conteneur à copier
		 */
		ContainerBox( ContainerBox const & p_container )
			:	m_ptCenter( p_container.m_ptCenter )
		{
		}
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_container	The container to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_container	Le conteneur à déplacer
		 */
		ContainerBox( ContainerBox && p_container )
			:	m_ptCenter( std::move( p_container.m_ptCenter ) )
		{
			p_container.m_ptCenter = Point< real, Dimension >();
		}
		/**
		 *\~english
		 *\brief		Constructor from center
		 *\param[in]	p_ptCenter	The center
		 *\~french
		 *\brief		Constructeur à partir du centre
		 *\param[in]	p_ptCenter	Le centre
		 */
		ContainerBox( Point< real, Dimension > const & p_ptCenter )
			:	m_ptCenter( p_ptCenter )
		{
		}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_container	The container to copy
		 *\return		A reference to this container
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_container	Le conteneur à copier
		 *\return		Une référence sur ce conteneur
		 */
		ContainerBox & operator=( ContainerBox const & p_container )
		{
			m_ptCenter = p_container.m_ptCenter;
			return * this;
		}
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_container	The container to copy
		 *\return		A reference to this container
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_container	Le conteneur à copier
		 *\return		Une référence sur ce conteneur
		 */
		ContainerBox & operator=( ContainerBox && p_container )
		{
			if ( this != &p_container )
			{
				m_ptCenter = std::move( p_container.m_ptCenter );
			}

			return * this;
		}
		/**
		 *\~english
		 *\brief		Tests if a vertex is within the container, id est inside it but not on it's limits
		 *\param[in]	p_v	The vertex to test
		 *\return		\p true if the vertex is within the container, false if not
		 *\~french
		 *\brief		Teste si un point est contenu dans le container (mais pas à la limite)
		 *\param[in]	p_v	Le point à tester
		 *\return		\p true si le point est dans le container
		 */
		CU_API virtual bool isWithin( Point< real, Dimension > const & p_v ) = 0;
		/**
		 *\~english
		 *\brief		Tests if a vertex is on the limits of this container, and not within
		 *\param[in]	p_v	The vertex to test
		 *\return		\p true if the vertex is on the limits of the container, false if not
		 *\~french
		 *\brief		Teste si un point est sur la limite du container, et pas dedans
		 *\param[in]	p_v	Le point à tester
		 *\return		\p true si le point est sur la limite
		 */
		CU_API virtual bool isOnLimits( Point< real, Dimension > const & p_v ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the center of this container
		 *\return		A constant reference to the center
		 *\~french
		 *\brief		Récupère le centre de ce conteneur
		 *\return		Une référence constante sur le centre
		 */
		inline Point< real, Dimension > const & getCenter()const
		{
			return m_ptCenter;
		}

	protected:
		//!\~english The center of the container	\~french Le centre de ce conteneur
		Point< real, Dimension > m_ptCenter;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/02/2010
	\~english
	\brief		Typedef on a 2 dimensions ContainerBox
	\~french
	\brief		Typedef sur une ContainerBox à 2 dimensions
	*/
	using ContainerBox2D = ContainerBox< 2 >;
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/02/2010
	\~english
	\brief		Typedef on a 3 dimensions ContainerBox
	\~french
	\brief		Typedef sur une ContainerBox à 3 dimensions
	*/
	using ContainerBox3D = ContainerBox< 3 >;
}

#endif
