/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RAY_H___
#define ___C3D_RAY_H___

#include "MiscellaneousModule.hpp"

#include <CastorUtils/Math/Point.hpp>

namespace castor3d
{
	class Ray
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor from mouse coordinates and a camera.
		 *\param[in]	point	The mouse coordinates.
		 *\param[in]	camera	The camera from which to retrieve the ray.
		 *\~french
		 *\brief		Constructeur depuis les coordonnées de la souris et une caméra.
		 *\param[in]	point	Les coordonnées de la souris.
		 *\param[in]	camera	La caméra depuis laquelle le rayon est récupéré.
		 */
		C3D_API Ray( castor::Position const & point
			, Camera const & camera );
		/**
		 *\~english
		 *\brief		Constructor from mouse coordinates and a camera.
		 *\param[in]	x, y	The mouse coordinates.
		 *\param[in]	camera	The camera from which to retrieve the ray.
		 *\~french
		 *\brief		Constructeur depuis les coordonnées de la souris et une caméra.
		 *\param[in]	x, y	Les coordonnées de la souris.
		 *\param[in]	camera	La caméra depuis laquelle le rayon est récupéré.
		 */
		C3D_API Ray( int x, int y
			, Camera const & camera );
		/**
		 *\~english
		 *\brief		Constructor from origin and direction.
		 *\param[in]	origin		The origin of the ray.
		 *\param[in]	direction	The direction of the ray.
		 *\~french
		 *\brief		Constructeur depuis une origine et une direction.
		 *\param[in]	origin		L'origine du rayon.
		 *\param[in]	direction	La direction du rayon.
		 */
		C3D_API Ray( castor::Point3f const & origin
			, castor::Point3f const & direction );
		/**
		 *\~english
		 *\brief		Tells if the ray intersects the given triangle of vertices.
		 *\param[in]	pt1			The first triangle vertex.
		 *\param[in]	pt2			The second triangle vertex.
		 *\param[in]	pt3			The third triangle vertex.
		 *\param[out]	distance	Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~french
		 *\brief		Dit si le rayon croise un triangle donné.
		 *\param[in]	pt1			Le premier sommet du triangle.
		 *\param[in]	pt2			Le second sommet du triangle.
		 *\param[in]	pt3			Le troisième sommet du triangle.
		 *\param[out]	distance	Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( castor::Point3f const & pt1
			, castor::Point3f const & pt2
			, castor::Point3f const & pt3
			, float & distance )const;
		/**
		 *\~english
		 *\brief		Tells if the ray intersects the given face.
		 *\param[in]	face		The face to test.
		 *\param[in]	transform	The face's vertex transformation matrix.
		 *\param[in]	submesh		The submesh holding the face.
		 *\param[out]	distance	Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~french
		 *\brief		Dit si le rayon croise la face donnée.
		 *\param[in]	face		La face donnée.
		 *\param[in]	transform	La matrice de transformation des sommets de la face.
		 *\param[in]	submesh		Le sous-maillage contenant la face.
		 *\param[out]	distance	Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( Face const & face
			, castor::Matrix4x4f const & transform
			, Submesh const & submesh
			, float & distance )const;
		/**
		 *\~english
		 *\brief		Tells if the vertex is on the ray.
		 *\param[in]	point		The vertex to test.
		 *\param[out]	distance	Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~french
		 *\brief		Dit si le point donné se trouve sur le rayon.
		 *\param[in]	point		Le point à tester.
		 *\param[out]	distance	Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( castor::Point3f const & point
			, float & distance )const;
		/**
		 *\~english
		 *\brief		Tells if the ray intersects the given Combo box.
		 *\param[in]	box			The box to test.
		 *\param[out]	distance	Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~french
		 *\brief		Dit si le rayon croise la Combo box donnée.
		 *\param[in]	box			La boîte à tester.
		 *\param[out]	distance	Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( castor::BoundingBox const & box
			, float & distance )const;
		/**
		 *\~english
		 *\brief		Tells if the ray intersects the given Sphere.
		 *\param[in]	sphere		The sphere to test.
		 *\param[out]	distance	Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~french
		 *\brief		Dit si le rayon croise la sphère donnée.
		 *\param[in]	sphere		La sphère à tester.
		 *\param[out]	distance	Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( castor::BoundingSphere const & sphere
			, float & distance )const;
		/**
		 *\~english
		 *\brief		Tells if the ray intersects the given Geometry.
		 *\param[in]	geometry		The geometry to test.
		 *\param[out]	nearestFace		Receives the intersected face.
		 *\param[out]	nearestSubmesh	Receives the intersected submesh.
		 *\param[out]	distance		Receives the distance.
		 *\return		\p castor::Intersection::eIn or \p castor::Intersection::eOut.
		 *\~french
		 *\brief		Dit si le rayon croise la géométrie donnée.
		 *\param[in]	geometry		La géométrie à tester.
		 *\param[out]	nearestFace		Reçoit la face croisée.
		 *\param[out]	nearestSubmesh	Reçoit le sous-maillage croisé.
		 *\param[out]	distance		Reçoit la distance.
		 *\return		\p castor::Intersection::eIn ou \p castor::Intersection::eOut.
		 */
		C3D_API castor::Intersection intersects( GeometrySPtr geometry
			, Face & nearestFace
			, SubmeshSPtr & nearestSubmesh
			, float & distance )const;
		/**
		 *\~english
		 *\brief		Projects the given vertex on the ray.
		 *\param[in]	point	The vertex we want to project.
		 *\param[out]	result	The projecion result.
		 *\return		\p true if the vertex can be projected on the ray, false if not.
		 */
		C3D_API bool projectVertex( castor::Point3f const & point
			, castor::Point3f & result )const;

	public:
		//!\~english	The ray origin.
		//!\~french		L'origine du rayon.
		castor::Point3f m_origin;
		//!\~english	The ray direction.
		//!\~french		La direction du rayon.
		castor::Point3f m_direction;
	};
}

#endif
