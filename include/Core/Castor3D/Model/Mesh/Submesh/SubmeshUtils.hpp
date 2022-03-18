/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshUtils_H___
#define ___C3D_SubmeshUtils_H___

#include "Component/ComponentModule.hpp"

namespace castor3d
{
	/**
	\~english
	\brief		Submesh utility functions.
	\~french
	\brief		Fonctions utilitaires pour les sous-maillages.
	*/
	class SubmeshUtils
	{
	public:
		/**
		 *\~english
		 *\brief			Creates faces from the points.
		 *\remarks			This function assumes the points are sorted like triangles fan.
		 *\param[in,out]	points	The points.
		 *\param[in,out]	triFace	The component that will receive the computed triangles.
		 *\~french
		 *\brief			Crée les faces à partir des points.
		 *\remarks			Cette fonction suppose que les points sont tirés à la manière triangles fan.
		 *\param[in,out]	points	Les points.
		 *\param[in,out]	triFace	Le composant qui va recevoir les faces calculées.
		 */
		C3D_API static void computeFacesFromPolygonVertex( InterleavedVertexArray & points
			, TriFaceMapping & triFace );
		/**
		 *\~english
		 *\brief			Generates normals and tangents.
		 *\param[in,out]	points		The points.
		 *\param[in]		reverted	Tells if the normals must be inverted.
		 *\param[in]		triFace		The component that will receive the computed triangles.
		 *\~french
		 *\brief			Génère les normales et les tangentes.
		 *\param[in,out]	points		Les points.
		 *\param[in]		reverted	Dit si les normales doivent être inversées.
		 *\param[in]		triFace		Le composant qui va recevoir les faces calculées.
		 */
		C3D_API static void computeNormals( InterleavedVertexArray & points
			, TriFaceMapping const & triFace
			, bool reverted = false );
		/**
		 *\~english
		 *\brief			Computes tangent for each vertex of the submesh.
		 *\remarks			This function supposes the normals are defined.
		 *\param[in,out]	points	The points.
		 *\param[in]		triFace	The component that will receive the computed triangles.
		 *\~french
		 *\brief			Calcule la tangente pour chaque vertex du sous-maillage.
		 *\remarks			Cette fonction suppose que les normales sont définies.
		 *\param[in,out]	points	Les points.
		 *\param[in]		triFace	Le composant qui va recevoir les faces calculées.
		 */
		C3D_API static void computeTangentsFromNormals( InterleavedVertexArray & points
			, TriFaceMapping const & triFace );
		/**
		 *\~english
		 *\brief			Computes normal and tangent for each vertex of the given face.
		 *\param[in,out]	points	The points.
		 *\param[in]		face	The face.
		 *\~french
		 *\brief			Calcule la normale et la tangente pour chaque vertex de la face donnée.
		 *\param[in,out]	points	Les points.
		 *\param[in]		face	La face.
		 */
		C3D_API static void computeNormals( InterleavedVertexArray & points
			, Face const & face );
		/**
		 *\~english
		 *\brief			Computes tangent for each vertex of the given face.
		 *\param[in,out]	points	The points.
		 *\param[in]		face	The face.
		 *\~french
		 *\brief			Calcule la tangente pour chaque vertex de la face donnée.
		 *\param[in,out]	points	Les points.
		 *\param[in]		face	La face.
		 */
		C3D_API static void computeTangents( InterleavedVertexArray & points
			, Face const & face );
	};
}

#endif
