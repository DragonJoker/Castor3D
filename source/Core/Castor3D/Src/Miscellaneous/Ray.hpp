/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RAY_H___
#define ___C3D_RAY_H___

#include "Castor3DPrerequisites.hpp"

#include <Math/Point.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Ray representation
	\remarks	A ray is an origin and a direction in 3D.
	\~french
	\brief		Représentation d'un rayon.
	\remarks	Un rayon est représentaté par une origine et une direction.
	*/
	class Ray
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor from mouse coordinates and a camera.
		 *\param[in]	p_point		The mouse coordinates.
		 *\param[in]	p_camera	The camera from which to retrieve the ray.
		 *\~french
		 *\brief		Constructeur depuis les coordonnées de la souris et une caméra.
		 *\param[in]	p_point		Les coordonnées de la souris.
		 *\param[in]	p_camera	La caméra depuis laquelle le rayon est récupéré.
		 */
		C3D_API Ray( castor::Point2i const & p_point, Camera const & p_camera );
		/**
		 *\brief		Constructor from mouse coordinates and a camera.
		 *\param[in]	p_x, p_y	The mouse coordinates.
		 *\param[in]	p_camera	The camera from which to retrieve the ray.
		 *\~french
		 *\brief		Constructeur depuis les coordonnées de la souris et une caméra.
		 *\param[in]	p_x, p_y	Les coordonnées de la souris.
		 *\param[in]	p_camera	La caméra depuis laquelle le rayon est récupéré.
		 */
		C3D_API Ray( int p_x, int p_y, Camera const & p_camera );
		/**
		 *\~english
		 *\brief		Constructor from origin and direction.
		 *\param[in]	p_origin	The origin of the ray.
		 *\param[in]	p_direction	The direction of the ray.
		 *\~french
		 *\brief		Constructeur depuis une origine et une direction.
		 *\param[in]	p_origin	L'origine du rayon.
		 *\param[in]	p_direction	La direction du rayon.
		 */
		C3D_API Ray( castor::Point3r const & p_origin, castor::Point3r const & p_direction );
		/**
		 *\~english
		 *\brief		Tells if the ray intersects the given triangle of vertices.
		 *\param[in]	p_pt1		The first triangle vertex.
		 *\param[in]	p_pt2		The second triangle vertex.
		 *\param[in]	p_pt3		The third triangle vertex.
		 *\param[out]	p_distance	Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~french
		 *\brief		Dit si le rayon croise un triangle donné.
		 *\param[in]	p_pt1		Le premier sommet du triangle.
		 *\param[in]	p_pt2		Le second sommet du triangle.
		 *\param[in]	p_pt3		Le troisième sommet du triangle.
		 *\param[out]	p_distance	Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( castor::Point3r const & p_pt1, castor::Point3r const & p_pt2, castor::Point3r const & p_pt3, real & p_distance )const;
		/**
		 *\~english
		 *\brief		Tells if the ray intersects the given face.
		 *\param[in]	p_face		The face to test.
		 *\param[in]	p_transform	The face's vertex transformation matrix.
		 *\param[in]	p_submesh	The submesh holding the face.
		 *\param[out]	p_distance	Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~french
		 *\brief		Dit si le rayon croise la face donnée.
		 *\param[in]	p_face		La face donnée.
		 *\param[in]	p_transform	La matrice de transformation des sommets de la face.
		 *\param[in]	p_submesh	Le sous-maillage contenant la face.
		 *\param[out]	p_distance	Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( Face const & p_face, castor::Matrix4x4r const & p_transform, Submesh const & p_submesh, real & p_distance )const;
		/**
		 *\~english
		 *\brief		Tells if the vertex is on the ray.
		 *\param[in]	p_point		The vertex to test.
		 *\param[out]	p_distance	Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~english
		 *\brief		Dit si le point donné se trouve sur le rayon.
		 *\param[in]	p_point		Le point à tester.
		 *\param[out]	p_distance	Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( castor::Point3r const & p_point, real & p_distance )const;
		/**
		 *\~english
		 *\brief		Tells if the ray intersects the given Combo box.
		 *\param[in]	p_box		The box to test.
		 *\param[out]	p_distance	Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~french
		 *\brief		Dit si le rayon croise la Combo box donnée.
		 *\param[in]	p_box		La boîte à tester.
		 *\param[out]	p_distance	Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( castor::BoundingBox const & p_box, real & p_distance )const;
		/**
		 *\~english
		 *\brief		Tells if the ray intersects the given Sphere.
		 *\param[in]	p_sphere	The sphere to test.
		 *\param[out]	p_distance	Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~french
		 *\brief		Dit si le rayon croise la sphère donnée.
		 *\param[in]	p_sphere	La sphère à tester.
		 *\param[out]	p_distance	Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( castor::BoundingSphere const & p_sphere, real & p_distance )const;
		/**
		 *\~english
		 *\brief		Tells if the ray intersects the given Geometry.
		 *\param[in]	p_geometry			The geometry to test.
		 *\param[out]	p_nearestFace		Receives the intersected face.
		 *\param[out]	p_nearestSubmesh	Receives the intersected submesh.
		 *\param[out]	p_distance			Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~french
		 *\brief		Dit si le rayon croise la géométrie donnée.
		 *\param[in]	p_geometry			La géométrie à tester.
		 *\param[out]	p_nearestFace		Reçoit la face croisée.
		 *\param[out]	p_nearestSubmesh	Reçoit le sous-maillage croisé.
		 *\param[out]	p_distance			Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( GeometrySPtr p_geometry, Face & p_nearestFace, SubmeshSPtr & p_nearestSubmesh, real & p_distance )const;
		/**
		 *\~english
		 *\brief		Projects the given vertex on the ray.
		 *\param[in]	p_point	The vertex we want to project.
		 *\param[out]	p_result	The projecion result.
		 *\return		\p true if the vertex can be projected on the ray, false if not.
		 */
		C3D_API bool projectVertex( castor::Point3r const & p_point, castor::Point3r & p_result )const;

	public:
		//!\~english	The ray origin.
		//!\~french		L'origine du rayon.
		castor::Point3r m_origin;
		//!\~english	The ray direction.
		//!\~french		La direction du rayon.
		castor::Point3r m_direction;
	};
}

#endif
