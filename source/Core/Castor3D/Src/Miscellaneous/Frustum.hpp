/*
See LICENSE file in root folder
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
		 *\param[in]	viewport	The viewport.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	viewport	Le viewport.
		 */
		C3D_API explicit Frustum( Viewport & viewport );
		/**
		 *\~english
		 *\brief		Updates the frustum planes.
		 *\param[in]	projection	The projection matrix.
		 *\param[in]	view		The view matrix.
		 *\~french
		 *\brief		Met à jour les plans du frustum.
		 *\param[in]	projection	La matrice de projection.
		 *\param[in]	view		Le matrice de vue.
		 */
		C3D_API void update( castor::Matrix4x4r const & projection
			, castor::Matrix4x4r const & view );
		/**
		 *\~english
		 *\brief		Updates the frustum planes.
		 *\param[in]	position	The view position.
		 *\param[in]	right		The X vector.
		 *\param[in]	up			The Y vector.
		 *\param[in]	front		The Z vector.
		 *\~french
		 *\brief		Met à jour les plans du frustum.
		 *\param[in]	position	La position de la vue.
		 *\param[in]	right		Le vecteur X.
		 *\param[in]	up			Le vecteur Y.
		 *\param[in]	front		Le vecteur Z.
		 */
		C3D_API void update( castor::Point3r const & position
			, castor::Point3r const & right
			, castor::Point3r const & up
			, castor::Point3r const & front );
		/**
		 *\~english
		 *\brief		Updates the frustum planes.
		 *\param[in]	eye		The view position.
		 *\param[in]	target	The view target vector.
		 *\param[in]	up		The direction of the up vector according to the orientation of the view.
		 *\~french
		 *\brief		Met à jour les plans du frustum.
		 *\param[in]	eye		La position de la vue.
		 *\param[in]	target	La position de la cible de la vue.
		 *\param[in]	up		La direction du vecteur haut, selon l'orientation de la vue.
		 */
		C3D_API void update( castor::Point3r const & eye
			, castor::Point3r const & target
			, castor::Point3r const & up );
		/**
		 *\~english
		 *\brief		Checks if given BoundingBox is in the view frustum.
		 *\param[in]	box				The BoundingBox.
		 *\param[in]	transformations	The BoundingBox transformations matrix.
		 *\return		\p false if the BoundingBox is completely out of the view frustum.
		 *\~french
		 *\brief
		 *\brief		Vérifie si la BoundingBox donnée est dans le frustum de vue.
		 *\param[in]	box				La BoundingBox.
		 *\param[in]	transformations	La matrice de transformations de la BoundingBox.
		 *\return		\p false si la BoundingBox est complètement en dehors du frustum de vue.
		 */
		C3D_API bool isVisible( castor::BoundingBox const & box
			, castor::Matrix4x4r const & transformations )const;
		/**
		 *\~english
		 *\brief		Checks if given BoundingSphere is in the view frustum.
		 *\param[in]	sphere		The BoundingSphere.
		 *\param[in]	position	The position for the BoundingSphere.
		 *\param[in]	scale		The scale for the BoundingSphere.
		 *\return		\p false if the BoundingSphere is completely out of the view frustum.
		 *\~french
		 *\brief
		 *\brief		Vérifie si la SphereBox donnée est dans le frustum de vue.
		 *\param[in]	sphere		La BoundingSphere.
		 *\param[in]	position	La position de la BoundingSphere.
		 *\param[in]	scale		L'échelle de la SphereBox.
		 *\return		\p false si la BoundingSphere est complètement en dehors du frustum de vue.
		 */
		C3D_API bool isVisible( castor::BoundingSphere const & sphere
			, castor::Point3r const & position
			, castor::Point3r const & scale )const;
		/**
		 *\~english
		 *\brief		Checks if given point is in the view frustum.
		 *\param[in]	point	The point.
		 *\return		\p false if the point out of the view frustum.
		 *\~french
		 *\brief		Vérifie si le point donné est dans le frustum de vue.
		 *\param[in]	point	Le point.
		 *\return		\p false si le point en dehors du frustum de vue.
		 */
		C3D_API bool isVisible( castor::Point3r const & point )const;

	private:
		//!\~english	The viewport.
		//!\~french		Le viewport.
		Viewport & m_viewport;
		//!\~english	The view frustum's corners.
		//!\~french		Les coins du frustum de vue.
		std::array< castor::Point3r, size_t( FrustumCorner::eCount ) > m_corners;
		//!\~english	The view frustum's planes.
		//!\~french		Les plans du frustum de vue.
		std::array< castor::PlaneEquation, size_t( FrustumPlane::eCount ) > m_planes;
	};
}

#endif
