/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Frustum_H___
#define ___C3D_Frustum_H___

#include "RenderModule.hpp"
#include "Castor3D/Model/VertexGroup.hpp"

#include <CastorUtils/Math/PlaneEquation.hpp>

namespace castor3d
{
	class Frustum
	{
	public:
		using Planes = std::array< castor::PlaneEquation, size_t( FrustumPlane::eCount ) >;

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
		C3D_API Planes update( castor::Matrix4x4f const & projection
			, castor::Matrix4x4f const & view );
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
		C3D_API Planes update( castor::Point3f const & position
			, castor::Point3f const & right
			, castor::Point3f const & up
			, castor::Point3f const & front );
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
		C3D_API Planes update( castor::Point3f const & eye
			, castor::Point3f const & target
			, castor::Point3f const & up );
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
			, castor::Matrix4x4f const & transformations )const;
		/**
		 *\~english
		 *\brief		Checks if given BoundingSphere is in the view frustum.
		 *\param[in]	sphere			The BoundingSphere.
		 *\param[in]	transformations	The BoundingSphere transformations matrix.
		 *\param[in]	scale			The scale for the BoundingSphere.
		 *\return		\p false if the BoundingSphere is completely out of the view frustum.
		 *\~french
		 *\brief
		 *\brief		Vérifie si la SphereBox donnée est dans le frustum de vue.
		 *\param[in]	sphere			La BoundingSphere.
		 *\param[in]	transformations	La matrice de transformations de la BoundingSphere.
		 *\param[in]	scale			L'échelle de la BoundingSphere.
		 *\return		\p false si la BoundingSphere est complètement en dehors du frustum de vue.
		 */
		C3D_API bool isVisible( castor::BoundingSphere const & sphere
			, castor::Matrix4x4f const & transformations
			, castor::Point3f const & scale )const;
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
		C3D_API bool isVisible( castor::Point3f const & point )const;

		std::array< InterleavedVertex, 6u * 4u > const & getPoints()const
		{
			return m_points;
		}

	private:
		Viewport & m_viewport;
		Planes m_planes;
		std::array< InterleavedVertex, 6u * 4u > m_points;
	};
}

#endif
