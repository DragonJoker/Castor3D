/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
		//!\~english	The pass.
		//!\~french		La passe.
		Pass & m_pass;
		//!\~english	The shader program.
		//!\~french		Le programme shader.
		ShaderProgram & m_program;
		//!\~english	The matrix UBO.
		//!\~french		L'UBO de matrices.
		FrameVariableBuffer & m_matrixUbo;
		//!\~english	The pass UBO.
		//!\~french		L'UBO de passe.
		FrameVariableBuffer & m_passUbo;
		//!\~english	The pass ambient colour.
		//!\~french		La couleur ambiante de passe.
		Point4rFrameVariable & m_ambient;
		//!\~english	The pass diffuse colour.
		//!\~french		La couleur diffuse de passe.
		Point4rFrameVariable & m_diffuse;
		//!\~english	The pass specular colour.
		//!\~french		La couleur spéculaire de passe.
		Point4rFrameVariable & m_specular;
		//!\~english	The pass emissive colour.
		//!\~french		La couleur émissive de passe.
		Point4rFrameVariable & m_emissive;
		//!\~english	The pass shininess.
		//!\~french		L'exposante de passe.
		OneFloatFrameVariable & m_shininess;
		//!\~english	The pass opacity.
		//!\~french		L'opacité de passe.
		OneFloatFrameVariable & m_opacity;
		//!\~english	The pass textures.
		//!\~french		Les textures de la passe.
		std::map< uint32_t, std::reference_wrapper< OneIntFrameVariable > > m_textures;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render submeshes.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des sous-maillages.
	*/
	struct SceneRenderNode
	{
		//!\~english	The base render node.
		//!\~french		Le noeud de rendu.
		RenderNode m_node;
		//!\~english	The scene UBO.
		//!\~french		L'UBO de scène.
		FrameVariableBuffer & m_sceneUbo;
		//!\~english	The pass opacity.
		//!\~french		L'opacité de passe.
		Point3rFrameVariable & m_cameraPos;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render submeshes.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des sous-maillages.
	*/
	struct StaticGeometryRenderNode
	{
		//!\~english	The base render node.
		//!\~french		Le noeud de rendu.
		SceneRenderNode m_scene;
		//!\~english	The geometry instanciating the submesh.
		//!\~french		La géométrie instanciant le submesh.
		Geometry & m_geometry;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de la géométrie.
		GeometryBuffers & m_buffers;
		//!\~english	The submesh.
		//!\~french		Le sous-maillage.
		Submesh & m_submesh;
		//!\~english	The parent scene node.
		//!\~french		Le scene node parent.
		SceneNode & m_sceneNode;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date		02/06/2016
	\~english
	\brief		Helper structure used to render animated submeshes.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des sous-maillages animés.
	*/
	struct AnimatedGeometryRenderNode
	{
		//!\~english	The base render node.
		//!\~french		Le noeud de rendu.
		SceneRenderNode m_scene;
		//!\~english	The geometry instanciating the submesh.
		//!\~french		La géométrie instanciant le submesh.
		Geometry & m_geometry;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de la géométrie.
		GeometryBuffers & m_buffers;
		//!\~english	The submesh.
		//!\~french		Le sous-maillage.
		Submesh & m_submesh;
		//!\~english	The parent scene node.
		//!\~french		Le scene node parent.
		SceneNode & m_sceneNode;
		//!\~english	The animated skeleton.
		//!\~french		Le squelette animé.
		AnimatedSkeleton * m_skeleton;
		//!\~english	The animated mesh.
		//!\~french		Le maillage animé.
		AnimatedMesh * m_mesh;
		//!\~english	The animation UBO.
		//!\~french		L'UBO d'animation.
		FrameVariableBuffer & m_animationUbo;
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
		//!\~english	The base render node.
		//!\~french		Le noeud de rendu.
		SceneRenderNode m_scene;
		//!\~english	The billboard.
		//!\~french		Le billboard.
		BillboardList & m_billboard;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de la géométrie.
		GeometryBuffers & m_buffers;
		//!\~english	The parent scene node.
		//!\~french		Le scene node parent.
		SceneNode & m_sceneNode;
		//!\~english	The billboard UBO.
		//!\~french		L'UBO de billboard.
		FrameVariableBuffer & m_billboardUbo;
		//!\~english	The dimensions uniform variable.
		//!\~french		La variable uniforme des dimensions.
		Point2iFrameVariable & m_dimensions;
	};

	//!\~english	StaticGeometryRenderNode array.
	//!\~french		Tableau de StaticGeometryRenderNode.
	DECLARE_VECTOR( StaticGeometryRenderNode, StaticGeometryRenderNode );
	//!\~english	AnimatedGeometryRenderNode array.
	//!\~french		Tableau de AnimatedGeometryRenderNode.
	DECLARE_VECTOR( AnimatedGeometryRenderNode, AnimatedGeometryRenderNode );
	//!\~english	Submesh sorted StaticGeometryRenderNodeArray.
	//!\~french		Map StaticGeometryRenderNodeArray, triés par sous-maillage.
	DECLARE_MAP( SubmeshSPtr, StaticGeometryRenderNodeArray, SubmeshStaticRenderNodes );
	//!\~english	Submesh sorted AnimatedGeometryRenderNodeArray.
	//!\~french		Map AnimatedGeometryRenderNodeArray, triés par sous-maillage.
	DECLARE_MAP( SubmeshSPtr, AnimatedGeometryRenderNodeArray, SubmeshAnimatedRenderNodes );
	//!\~english	BillboardRenderNode array.
	//!\~french		Tableau de BillboardRenderNode.
	DECLARE_VECTOR( BillboardRenderNode, BillboardRenderNode );
	//!\~english	Billboard sorted BillboardRenderNodeArray.
	//!\~french		Map BillboardRenderNodeArray, triés par billboard.
	DECLARE_MAP( BillboardListSPtr, BillboardRenderNodeArray, BillboardRenderNodes );
	//!\~english	Pass sorted SubmeshRenderNodesMap map.
	//!\~french		Map de SubmeshRenderNodesMap, triés par passe.
	template< typename T >
	struct TypeRenderNodesByPassMap
	{
		inline auto begin()const
		{
			return m_map.begin();
		}

		inline auto begin()
		{
			return m_map.begin();
		}

		inline auto end()const
		{
			return m_map.end();
		}

		inline auto end()
		{
			return m_map.end();
		}

		inline auto find( PassSPtr p_pass )const
		{
			return m_map.find( p_pass );
		}

		inline auto insert( std::pair< PassSPtr, T > p_pair )
		{
			return m_map.insert( p_pair );
		}

	public:
		using mapped_type = typename std::map< PassSPtr, T >::mapped_type;
		using key_type = typename std::map< PassSPtr, T >::key_type;
		using value_type = typename std::map< PassSPtr, T >::value_type;

	private:
		std::map< PassSPtr, T > m_map;
	};
	using SubmeshStaticRenderNodesByPassMap = TypeRenderNodesByPassMap< SubmeshStaticRenderNodesMap >;
	using SubmeshAnimatedRenderNodesByPassMap = TypeRenderNodesByPassMap< SubmeshAnimatedRenderNodesMap >;
	using BillboardRenderNodesByPassMap = TypeRenderNodesByPassMap< BillboardRenderNodesMap >;

	//!\~english	Shader program sorted SubmeshRenderNodesMap map.
	//!\~french		Map de SubmeshRenderNodesMap, triés par programme shader.
	DECLARE_MAP( ShaderProgramSPtr, SubmeshStaticRenderNodesByPassMap, SubmeshStaticRenderNodesByProgram );
	//!\~english	Shader program sorted SubmeshRenderNodesMap map.
	//!\~french		Map de SubmeshRenderNodesMap, triés par programme shader.
	DECLARE_MAP( ShaderProgramSPtr, SubmeshAnimatedRenderNodesByPassMap, SubmeshAnimatedRenderNodesByProgram );
	//!\~english	Shader program sorted BillboardRenderNodesMap map.
	//!\~french		Map de BillboardRenderNodesMap, triés par programme shader.
	DECLARE_MAP( ShaderProgramSPtr, BillboardRenderNodesByPassMap, BillboardRenderNodesByProgram );

	//@}
}

#endif
