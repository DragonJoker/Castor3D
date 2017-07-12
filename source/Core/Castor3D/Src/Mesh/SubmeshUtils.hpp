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
#ifndef ___C3D_SubmeshUtils_H___
#define ___C3D_SubmeshUtils_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
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
		 *\brief		Creates faces from the points
		 *\remarks		This function assumes the points are sorted like triangles fan
		 *\~french
		 *\brief		Crée les faces à partir des points
		 *\remarks		Cette fonction suppose que les points sont tirés à la manière triangles fan
		 */
		C3D_API static void ComputeFacesFromPolygonVertex( Submesh & p_submesh );
		/**
		 *\~english
		 *\brief		Generates normals and tangents
		 *\~french
		 *\brief		Génère les normales et les tangentes
		 */
		C3D_API static void ComputeNormals( Submesh & p_submesh
			, bool p_reverted = false );
		/**
		 *\~english
		 *\brief		Computes normal and tangent for each vertex of the given face
		 *\param[in]	p_face	The face
		 *\~french
		 *\brief		Calcule la normale et la tangente pour chaque vertex de la face donnée
		 *\param[in]	p_face	La face
		 */
		C3D_API static void ComputeNormals( Submesh & p_submesh
			, Face const & p_face );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the given face
		 *\param[in]	p_face	The face
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex de la face donnée
		 *\param[in]	p_face	La face
		 */
		C3D_API static void ComputeTangents( Submesh & p_submesh
			, Face const & p_face );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the submesh
		 *\remarks		This function supposes the normals are defined
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex du sous-maillage
		 *\remarks		Cette fonction suppose que les normales sont définies
		 */
		C3D_API static void ComputeTangentsFromNormals( Submesh & p_submesh );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the submesh
		 *\remarks		This function supposes bitangents and normals are defined
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex du sous-maillage
		 *\remarks		Cette fonction suppose que les bitangentes et les normales sont définies
		 */
		C3D_API static void ComputeTangentsFromBitangents( Submesh & p_submesh );
		/**
		 *\~english
		 *\brief		Computes bitangent for each vertex of the submesh
		 *\remarks		This function supposes the tangents and normals are defined
		 *\~french
		 *\brief		Calcule la bitangente pour chaque vertex du sous-maillage
		 *\remarks		Cette fonction suppose que les tangentes et les normales sont définies
		 */
		C3D_API static void ComputeBitangents( Submesh & p_submesh );
	};
}

#endif
