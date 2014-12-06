/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_MESH_CATEGORY_H___
#define ___C3D_MESH_CATEGORY_H___

#include "Castor3DPrerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\~english
	\brief		Mesh category is used to create a mesh's vertices, it is created by MeshFactory
	\remark		Custom mesh category doesn't generate vertices.
	\~french
	\brief		La catégorie de maillage est utilisée afin de créer les sommets du maillage, c'est créé via la MeshFactory
	\remark		La categorie de type Custom ne génère pas de points
	*/
	class MeshCategory
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eMeshType	The mesh type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eMeshType	Le type de maillage
		 */
		MeshCategory( eMESH_TYPE p_eMeshType = eMESH_TYPE_CUSTOM );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~MeshCategory();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		Cylinder
		 *\~french
		 *\brief		Fonction de création, utilisée par la Factory
		 *\return		Un cylindre
		 */
		static MeshCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Generates the mesh (vertices and faces)
		 *\~french
		 *\brief		Génère le maillage
		 */
		virtual void Generate();
		/**
		 *\~english
		 *\brief		Initialises the mesh
		 *\remark		Calls Generate
		 *\param[in]	p_arrayFaces		The faces counts
		 *\param[in]	p_arrayDimensions	The mesh dimensions
		 *\~french
		 *\brief		Initialise le maillage
		 *\remark		Appelle Generate
		 *\param[in]	p_arrayFaces		Les nombres de faces
		 *\param[in]	p_arrayDimensions	Les dimensions du maillage
		 */
		virtual void Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions );
		/**
		 *\~english
		 *\brief		Generates normals and tangents
		 *\~french
		 *\brief		Génère les normales et les tangentes
		 */
		virtual void ComputeNormals( bool p_bReverted = false );
		/**
		 *\~english
		 *\brief		Retrieves the mesh type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de maillage
		 *\return		La valeur
		 */
		inline eMESH_TYPE GetMeshType()const
		{
			return m_eMeshType;
		}
		/**
		 *\~english
		 *\brief		Retrieves the mesh
		 *\return		The value
		 *\~french
		 *\brief		Récupère le maillage
		 *\return		La valeur
		 */
		inline Mesh * GetMesh()const
		{
			return m_pMesh;
		}
		/**
		 *\~english
		 *\brief		Sets the mesh
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit le maillage
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetMesh( Mesh * val )
		{
			m_pMesh = val;
		}

	private:
		//!\~english The mesh type	\~french Le type de mesh
		eMESH_TYPE m_eMeshType;
		//!\~english The mesh	\~french Le mesh
		Mesh * m_pMesh;
	};
}

#pragma warning( pop )

#endif
