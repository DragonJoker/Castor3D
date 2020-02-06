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
#ifndef ___C3D_MESH_GENERATOR_H___
#define ___C3D_MESH_GENERATOR_H___

#include "Castor3DPrerequisites.hpp"

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
	class MeshGenerator
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
		explicit MeshGenerator( MeshType p_eMeshType = MeshType::eCustom );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~MeshGenerator();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		Cylinder
		 *\~french
		 *\brief		Fonction de création, utilisée par la Factory
		 *\return		Un cylindre
		 */
		static MeshGeneratorSPtr Create();
		/**
		 *\~english
		 *\brief		Generates the mesh.
		 *\param[in]	p_mesh			The mesh.
		 *\param[in]	p_faces			The faces counts.
		 *\param[in]	p_dimensions	The mesh dimensions.
		 *\~french
		 *\brief		Génère le maillage.
		 *\param[in]	p_mesh			Le maillage.
		 *\param[in]	p_faces			Les nombres de faces.
		 *\param[in]	p_dimensions	Les dimensions du maillage.
		 */
		virtual void Generate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions );
		/**
		 *\~english
		 *\brief		Generates normals and tangents.
		 *\param[in]	p_mesh		The mesh.
		 *\param[in]	p_reverted	Tells if the generated tangent space must be inverted.
		 *\~french
		 *\brief		Génère les normales et les tangentes.
		 *\param[in]	p_mesh		Le maillage.
		 *\param[in]	p_reverted	Dit si l'espace tangent généré doit être inversé.
		 */
		virtual void ComputeNormals( Mesh & p_mesh, bool p_reverted = false );
		/**
		 *\~english
		 *\brief		Retrieves the mesh type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de maillage
		 *\return		La valeur
		 */
		inline MeshType GetMeshType()const
		{
			return m_eMeshType;
		}

	private:
		/**
		 *\~english
		 *\brief		Generates the mesh.
		 *\param[in]	p_mesh			The mesh.
		 *\param[in]	p_faces			The faces counts.
		 *\param[in]	p_dimensions	The mesh dimensions.
		 *\~french
		 *\brief		Génère le maillage.
		 *\param[in]	p_mesh			Le maillage.
		 *\param[in]	p_faces			Les nombres de faces.
		 *\param[in]	p_dimensions	Les dimensions du maillage.
		 */
		virtual void DoGenerate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions );

	private:
		//!\~english The mesh type	\~french Le type de mesh
		MeshType m_eMeshType;
	};
}

#endif
