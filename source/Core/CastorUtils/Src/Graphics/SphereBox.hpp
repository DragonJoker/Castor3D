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
#ifndef ___CASTOR_SPHERE_BOX_H___
#define ___CASTOR_SPHERE_BOX_H___

#include "ContainerBox.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Sphere container class
	\~french
	\brief		Classe de conteneur sphérique
	*/
	class SphereBox
		:	public ContainerBox3D
	{
	private:
		//!\~english The radius of the sphere	\~french Le rayon de la sphère
		real m_radius;

	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		CU_API SphereBox();
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_sphere	The sphere box to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_sphere	La sphere box à copier
		 */
		CU_API SphereBox( SphereBox const & p_sphere );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_sphere	The sphere box to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_sphere	La sphere box à déplacer
		 */
		CU_API SphereBox( SphereBox && p_sphere );
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	p_center	The center of the sphere
		 *\param[in]	p_radius	The radius of the sphere
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	p_center	Le centre de la sphère
		 *\param[in]	p_radius	Le rayon de la sphère
		 */
		CU_API SphereBox( Point3r const & p_center, real p_radius );
		/**
		 *\~english
		 *\brief		Constructor from a CubeBox
		 *\param[in]	p_box	The CubeBox
		 *\~french
		 *\brief		Constructeur à partir d'une CubeBox
		 *\param[in]	p_box	La CubeBox
		 */
		CU_API SphereBox( CubeBox const & p_box );
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
		CU_API SphereBox & operator =( SphereBox const & p_container );
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
		CU_API SphereBox & operator =( SphereBox && p_container );
		/**
		 *\~english
		 *\brief		Reinitialises the sphere box
		 *\param[in]	p_center	The center
		 *\param[in]	p_radius	The radius
		 *\~french
		 *\brief		Réinitialise la sphere box
		 *\param[in]	p_center	Le centre
		 *\param[in]	p_radius	Le rayon
		 */
		CU_API void load( Point3r const & p_center, real p_radius );
		/**
		 *\~english
		 *\brief		Reinitialises the sphere box from a CubeBox
		 *\param[in]	p_box	The CubeBox
		 *\~french
		 *\brief		Réinitialise à partir d'une CubeBox
		 *\param[in]	p_box	La CubeBox
		 */
		CU_API void load( CubeBox const & p_box );
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
		CU_API virtual bool isWithin( Point3r const & p_v );
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
		CU_API virtual bool isOnLimits( Point3r const & p_v );
		/**
		 *\~english
		 *\brief		Retrieves the radius
		 *\return		The radius
		 *\~french
		 *\brief		Récupère le rayon
		 *\return		Le rayon
		 */
		inline real getRadius()const
		{
			return m_radius;
		}
	};
}

#endif
