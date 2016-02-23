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
#ifndef ___C3D_RENDER_NODE_H___
#define ___C3D_RENDER_NODE_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/**@name Render */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used link a pass and a shader program.
	\~french
	\brief		Structure d'aide utilisée pour lier une passe et un programme shader.
	*/
	struct RenderNode
	{
		//!\~english The pass.	\~french La passe.
		Pass & m_pass;
		//!\~english The shader program.	\~french Le programme shader.
		ShaderProgram & m_program;
		//!\~english The matrix UBO.	\~french L'UBO de matrices.
		FrameVariableBuffer & m_matrixUbo;
		//!\~english The pass UBO.	\~french L'UBO de passe.
		FrameVariableBuffer & m_passUbo;
		//!\~english The pass ambient colour.	\~french La couleur ambiante de passe.
		Point4rFrameVariable & m_ambient;
		//!\~english The pass diffuse colour.	\~french La couleur diffuse de passe.
		Point4rFrameVariable & m_diffuse;
		//!\~english The pass specular colour.	\~french La couleur spéculaire de passe.
		Point4rFrameVariable & m_specular;
		//!\~english The pass emissive colour.	\~french La couleur émissive de passe.
		Point4rFrameVariable & m_emissive;
		//!\~english The pass shininess.	\~french L'exposante de passe.
		OneFloatFrameVariable & m_shininess;
		//!\~english The pass opacity.	\~french L'opacité de passe.
		OneFloatFrameVariable & m_opacity;
		//!\~english The pass textures.	\~french Les textures de la passe.
		std::map< uint32_t, std::reference_wrapper< OneIntFrameVariable > > m_textures;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render submeshes.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des sous-maillages
	*/
	struct SceneRenderNode
	{
		//!\~english The scene UBO.	\~french L'UBO de scène.
		FrameVariableBuffer & m_sceneUbo;
		//!\~english The pass opacity.	\~french L'opacité de passe.
		Point3rFrameVariable & m_cameraPos;
		//!\~english The base render node.	\~french Le noeud de rendu.
		RenderNode m_node;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render submeshes.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des sous-maillages
	*/
	struct GeometryRenderNode
	{
		//!\~english The geometry instanciating the submesh.	\~french La géométrie instanciant le submesh.
		Geometry & m_geometry;
		//!\~english The submesh.	\~french Le sous-maillage.
		Submesh & m_submesh;
		//!\~english The parent scene node.	\~french Le scene node parent.
		SceneNode & m_sceneNode;
		//!\~english The base render node.	\~french Le noeud de rendu.
		SceneRenderNode m_scene;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render billboards.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des billboards.
	*/
	struct BillboardRenderNode
	{
		//!\~english The billboard.	\~french Le billboard.
		BillboardList & m_billboard;
		//!\~english The parent scene node.	\~french Le scene node parent.
		SceneNode & m_sceneNode;
		//!\~english The base render node.	\~french Le noeud de rendu.
		SceneRenderNode m_scene;
	};

	//!\~english stRENDER_NODE array.	\~french Tableau de stRENDER_NODE.
	DECLARE_VECTOR( GeometryRenderNode, GeometryRenderNode );
	//!\~english Submesh sorted RenderNodeArray. 	\~french Map RenderNodeArray, triés par sous-maillage.
	DECLARE_MAP( SubmeshSPtr, GeometryRenderNodeArray, SubmeshRenderNodes );
	//!\~english Pass sorted SubmeshRenderNodesMap map.	\~french Map de SubmeshRenderNodesMap, triés par passe.
	struct SubmeshRenderNodesByPassMap
	{
		inline decltype( auto ) begin()const
		{
			return m_map.begin();
		}

		inline decltype( auto ) begin()
		{
			return m_map.begin();
		}

		inline decltype( auto ) end()const
		{
			return m_map.end();
		}

		inline decltype( auto ) end()
		{
			return m_map.end();
		}

		inline decltype( auto ) find( PassSPtr p_pass )const
		{
			return m_map.find( p_pass );
		}

		inline decltype( auto ) insert( std::pair< PassSPtr, SubmeshRenderNodesMap > p_pair )
		{
			return m_map.insert( p_pair );
		}

	private:
		std::map< PassSPtr, SubmeshRenderNodesMap > m_map;
	};
	//!\~english Shader program sorted SubmeshRenderNodesByMaterialMap map	\~french Map de SubmeshRenderNodesByMaterialMap, triés par programme shader.
	DECLARE_MAP( ShaderProgramSPtr, SubmeshRenderNodesByPassMap, SubmeshRenderNodesByProgram );

	//@}
}

#endif
