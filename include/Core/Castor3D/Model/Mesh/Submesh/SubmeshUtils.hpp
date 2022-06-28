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
		 *\param[in,out]	texcoords	The texture coordinates.
		 *\param[in,out]	triFace		The component that will receive the computed triangles.
		 *\~french
		 *\brief			Crée les faces à partir des points.
		 *\remarks			Cette fonction suppose que les points sont tirés à la manière triangles fan.
		 *\param[in,out]	texcoords	Les coordonnées de texture.
		 *\param[in,out]	triFace		Le composant qui va recevoir les faces calculées.
		 */
		C3D_API static void computeFacesFromPolygonVertex( castor::Point3fArray & texcoords
			, TriFaceMapping & triFace );
		/**
		 *\~english
		 *\brief		Generates normals and tangents.
		 *\param[in]	positions	The vertices positions.
		 *\param[in]	texcoords	The vertices texture coordinates.
		 *\param[out]	normals		The vertices normals.
		 *\param[out]	tangents	The vertices tangents.
		 *\param[in]	faces		The triangles.
		 *\param[in]	reverted	Tells if the normals must be inverted.
		 *\~french
		 *\brief		Génère les normales et les tangentes.
		 *\param[in]	positions	Les positions des sommets.
		 *\param[in]	texcoords	Les coordonnées de texture des sommets.
		 *\param[out]	normals		Les normales des sommets.
		 *\param[out]	tangents	Les tangentes des sommets.
		 *\param[in]	faces		Les triangles.
		 *\param[in]	reverted	Dit si les normales doivent être inversées.
		 */
		C3D_API static void computeNormals( castor::Point3fArray const & positions
			, castor::Point3fArray const & texcoords
			, castor::Point3fArray & normals
			, castor::Point3fArray & tangents
			, FaceArray const & triFace
			, bool reverted = false );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the submesh.
		 *\remarks		This function supposes the normals are defined.
		 *\param[in]	positions	The vertices positions.
		 *\param[in]	texcoords	The vertices texture coordinates.
		 *\param[in]	normals		The vertices normals.
		 *\param[out]	tangents	The vertices tangents.
		 *\param[in]	faces		The triangles.
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex du sous-maillage.
		 *\remarks		Cette fonction suppose que les normales sont définies.
		 *\param[in]	positions	Les positions des sommets.
		 *\param[in]	texcoords	Les coordonnées de texture des sommets.
		 *\param[in]	normals		Les normales des sommets.
		 *\param[out]	tangents	Les tangentes des sommets.
		 *\param[in]	faces		Les triangles.
		 */
		C3D_API static void computeTangentsFromNormals( castor::Point3fArray const & positions
			, castor::Point3fArray const & texcoords
			, castor::Point3fArray const & normals
			, castor::Point3fArray & tangents
			, FaceArray const & faces );
		/**
		 *\~english
		 *\brief		Computes normal and tangent for each vertex of the given face.
		 *\param[in]	positions	The vertices positions.
		 *\param[in]	texcoords	The vertices texture coordinates.
		 *\param[out]	normals		The vertices normals.
		 *\param[out]	tangents	The vertices tangents.
		 *\param[in]	face		The face.
		 *\~french
		 *\brief		Calcule la normale et la tangente pour chaque vertex de la face donnée.
		 *\param[in]	positions	Les positions des sommets.
		 *\param[in]	texcoords	Les coordonnées de texture des sommets.
		 *\param[out]	normals		Les normales des sommets.
		 *\param[out]	tangents	Les tangentes des sommets.
		 *\param[in]	face		La face.
		 */
		C3D_API static void computeNormals( castor::Point3fArray const & positions
			, castor::Point3fArray const & texcoords
			, castor::Point3fArray & normals
			, castor::Point3fArray & tangents
			, Face const & face );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the given face.
		 *\param[in]	positions	The vertices positions.
		 *\param[in]	texcoords	The vertices texture coordinates.
		 *\param[out]	tangents	The vertices tangents.
		 *\param[in]	face		The face.
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex de la face donnée.
		 *\param[in]	positions	Les positions des sommets.
		 *\param[in]	texcoords	Les coordonnées de texture des sommets.
		 *\param[out]	tangents	Les tangentes des sommets.
		 *\param[in]	face		La face.
		 */
		C3D_API static void computeTangents( castor::Point3fArray const & positions
			, castor::Point3fArray const & texcoords
			, castor::Point3fArray & tangents
			, Face const & face );
	};
}

#endif
