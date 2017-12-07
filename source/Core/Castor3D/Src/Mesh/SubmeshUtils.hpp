/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshUtils_H___
#define ___C3D_SubmeshUtils_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
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
		 *\param[in]		submesh	The submesh.
		 *\param[in,out]	triFace	The component that will receive the computed triangles.
		 *\~french
		 *\brief			Crée les faces à partir des points.
		 *\remarks			Cette fonction suppose que les points sont tirés à la manière triangles fan.
		 *\param[in]		submesh	Le sous-maillage.
		 *\param[in,out]	triFace	Le composant qui va recevoir les faces calculées.
		 */
		C3D_API static void computeFacesFromPolygonVertex( Submesh & submesh
			, TriFaceMapping & triFace );
		/**
		 *\~english
		 *\brief			Generates normals and tangents.
		 *\param[in]		submesh		The submesh.
		 *\param[in]		reverted	Tells if the normals must be inverted.
		 *\param[in,out]	triFace		The component that will receive the computed triangles.
		 *\~french
		 *\brief			Génère les normales et les tangentes.
		 *\param[in]		submesh		Le sous-maillage.
		 *\param[in]		reverted	Dit si les normales doivent être inversées.
		 *\param[in,out]	triFace		Le composant qui va recevoir les faces calculées.
		 */
		C3D_API static void computeNormals( Submesh & submesh
			, TriFaceMapping & triFace
			, bool reverted = false );
		/**
		 *\~english
		 *\brief		Computes normal and tangent for each vertex of the given face.
		 *\param[in]	submesh	The submesh.
		 *\param[in]	face	The face.
		 *\~french
		 *\brief		Calcule la normale et la tangente pour chaque vertex de la face donnée.
		 *\param[in]	submesh	Le sous-maillage.
		 *\param[in]	face	La face.
		 */
		C3D_API static void computeNormals( Submesh & submesh
			, Face const & face );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the given face.
		 *\param[in]	submesh	The submesh.
		 *\param[in]	face	The face.
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex de la face donnée.
		 *\param[in]	submesh	Le sous-maillage.
		 *\param[in]	face	La face.
		 */
		C3D_API static void computeTangents( Submesh & submesh
			, Face const & face );
		/**
		 *\~english
		 *\brief			Computes tangent for each vertex of the submesh.
		 *\remarks			This function supposes the normals are defined.
		 *\param[in]		submesh	The submesh.
		 *\param[in,out]	triFace	The component that will receive the computed triangles.
		 *\~french
		 *\brief			Calcule la tangente pour chaque vertex du sous-maillage.
		 *\remarks			Cette fonction suppose que les normales sont définies.
		 *\param[in]		submesh	Le sous-maillage.
		 *\param[in,out]	triFace	Le composant qui va recevoir les faces calculées.
		 */
		C3D_API static void computeTangentsFromNormals( Submesh & submesh
			, TriFaceMapping & triFace );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the submesh.
		 *\remarks		This function supposes bitangents and normals are defined.
		 *\param[in]	submesh	The submesh.
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex du sous-maillage.
		 *\remarks		Cette fonction suppose que les bitangentes et les normales sont définies.
		 *\param[in]	submesh	Le sous-maillage.
		 */
		C3D_API static void computeTangentsFromBitangents( Submesh & submesh );
		/**
		 *\~english
		 *\brief		Computes bitangent for each vertex of the submesh.
		 *\remarks		This function supposes the tangents and normals are defined.
		 *\param[in]	submesh	The submesh.
		 *\~french
		 *\brief		Calcule la bitangente pour chaque vertex du sous-maillage.
		 *\remarks		Cette fonction suppose que les tangentes et les normales sont définies.
		 *\param[in]	submesh	Le sous-maillage.
		 */
		C3D_API static void computeBitangents( Submesh & submesh );
	};
}

#endif
