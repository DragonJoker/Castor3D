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
#ifndef ___C3D_FRUSTUM_H___
#define ___C3D_FRUSTUM_H___

#include "Castor3DPrerequisites.hpp"

#include <Math/PlaneEquation.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Implements a frustum and the checks related to frustum culling.
	\~french
	\brief		Implémente un frustum et les vérifications relatives au frustum culling.
	*/
	class Frustum
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_viewport	The viewport.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_viewport	Le viewport.
		 */
		C3D_API Frustum( Viewport & p_viewport );
		/**
		 *\~english
		 *\brief		Updates the frustum planes.
		 *\param[in]	p_position	The view position.
		 *\param[in]	p_x			The X vector.
		 *\param[in]	p_y			The Y vector.
		 *\param[in]	p_z			The Z vector.
		 *\~french
		 *\brief		Met à jour les plans du frustum.
		 *\param[in]	p_position	La position de la vue.
		 *\param[in]	p_x			Le vecteur X.
		 *\param[in]	p_y			Le vecteur Y.
		 *\param[in]	p_z			Le vecteur Z.
		 */
		C3D_API void update( castor::Point3r const & p_position, castor::Point3r const & p_x, castor::Point3r const & p_y, castor::Point3r const & p_z );
		/**
		 *\~english
		 *\brief		Checks if given CubeBox is in the view frustum.
		 *\param[in]	p_box				The CubeBox.
		 *\param[in]	p_transformations	The CubeBox transformations matrix.
		 *\return		\p false if the CubeBox is completely out of the view frustum.
		 *\~french
		 *\brief
		 *\brief		Vérifie si la CubeBox donnée est dans le frustum de vue.
		 *\param[in]	p_box				La CubeBox.
		 *\param[in]	p_transformations	La matrice de transformations de la CubeBox.
		 *\return		\p false si la CubeBox est complètement en dehors du frustum de vue.
		 */
		C3D_API bool isVisible( castor::CubeBox const & p_box, castor::Matrix4x4r const & p_transformations )const;
		/**
		 *\~english
		 *\brief		Checks if given SphereBox is in the view frustum.
		 *\param[in]	p_box				The SphereBox.
		 *\param[in]	p_transformations	The SphereBox transformations matrix.
		 *\return		\p false if the SphereBox is completely out of the view frustum.
		 *\~french
		 *\brief
		 *\brief		Vérifie si la SphereBox donnée est dans le frustum de vue.
		 *\param[in]	p_box				La SphereBox.
		 *\param[in]	p_transformations	La SphereBox de transformations de la CubeBox.
		 *\return		\p false si la SphereBox est complètement en dehors du frustum de vue.
		 */
		C3D_API bool isVisible( castor::SphereBox const & p_box, castor::Matrix4x4r const & p_transformations )const;
		/**
		 *\~english
		 *\brief		Checks if given point is in the view frustum.
		 *\param[in]	p_point	The point.
		 *\return		\p false if the point out of the view frustum.
		 *\~french
		 *\brief		Vérifie si le point donné est dans le frustum de vue.
		 *\param[in]	p_point	Le point.
		 *\return		\p false si le point en dehors du frustum de vue.
		 */
		C3D_API bool isVisible( castor::Point3r const & p_point )const;

	private:
		//!\~english	The viewport.
		//!\~french		Le viewport.
		Viewport & m_viewport;
		//!\~english	The view frustum's planes.
		//!\~french		Les plans du frustum de vue.
		std::array< castor::PlaneEquation< real >, size_t( FrustumPlane::eCount ) > m_planes;
	};
}

#endif
