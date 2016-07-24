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
#ifndef ___CASTOR_CUBE_BOX_H___
#define ___CASTOR_CUBE_BOX_H___

#include "ContainerBox.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Combo box container class
	\remark		A combo box is a box container
	\~french
	\brief		Classe de combo box
	\remark		Une combo box est un container boîte parallélépipédique
	*/
	class CubeBox
		: public ContainerBox3D
	{
	protected:
		//!\~english The min extent of the combo box	\~french Le point minimal de la combo box
		Point3r m_min;
		//!\~english The max extent of the combo box	\~french Le point maximal de la combo box
		Point3r m_max;

	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		CU_API CubeBox();
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_cube	The cube box to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_cube	La cube box à copier
		 */
		CU_API CubeBox( CubeBox const & p_cube );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_cube	The cube box to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_cube	La cube box à déplacer
		 */
		CU_API CubeBox( CubeBox && p_cube );
		/**
		 *\~english
		 *\brief		Constructor from min and max extents
		 *\param[in]	p_min	The min extent
		 *\param[in]	p_max	The max extent
		 *\~french
		 *\brief		Constructeur à partir des points min et max
		 *\param[in]	p_min	Le point minimal
		 *\param[in]	p_max	Le point maximal
		 */
		CU_API CubeBox( Point3r const & p_min, Point3r const & p_max );
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
		CU_API CubeBox & operator =( CubeBox const & p_container );
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
		CU_API CubeBox & operator =( CubeBox && p_container );
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
		CU_API virtual bool IsWithin( Point3r const & p_v );
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
		CU_API virtual bool IsOnLimits( Point3r const & p_v );
		/**
		 *\~english
		 *\brief		Reinitialises the combo box to the given limits
		 *\param[in]	p_ptMin	The min extent
		 *\param[in]	p_ptMax	The max extent
		 *\~french
		 *\brief		Réinitialise la boîte aux limites données
		 *\param[in]	p_ptMin	Le point minimal
		 *\param[in]	p_ptMax	Le point maximal
		 */
		CU_API void Load( Point3r const & p_ptMin, Point3r const & p_ptMax );
		/**
		 *\~english
		 *\brief		Retrieves the min extent
		 *\return		A constant reference on the min extent
		 *\~french
		 *\brief		Récupère le point minimal
		 *\return		Une référence constante sur le point minimal
		 */
		inline Point3r const & GetMin()const
		{
			return m_min;
		}
		/**
		 *\~english
		 *\brief		Retrieves the max extent
		 *\return		A constant reference on the max extent
		 *\~french
		 *\brief		Récupère le point maximal
		 *\return		Une référence constante sur le point maximal
		 */
		inline Point3r const & GetMax()const
		{
			return m_max;
		}
	};
}

#endif
