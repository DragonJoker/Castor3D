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
	\brief		Helper structure used to render objects.
	\~french
	\brief		Structure d'aide utilisée pour le dessin d'objets.
	*/
	struct ObjectRenderNodeBase
	{
		ObjectRenderNodeBase( SceneRenderNode && p_scene, GeometryBuffers & p_buffers, SceneNode & p_sceneNode );
		/**
		 *\~english
		 *\brief		Render function.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 */
		C3D_API virtual void Render( Scene const & p_scene, Pipeline & p_pipeline ) = 0;
		/**
		 *\~english
		 *\brief		Binds the given pass to the render node.
		 *\param[in]	p_scene				The rendered scene.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée pour le noeud de rendu.
		 *\param[in]	p_scene				La scène rendue.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		C3D_API virtual void BindPass( Scene const & p_scene, Pipeline & p_pipeline, uint64_t p_excludedMtxFlags ) = 0;
		/**
		 *\~english
		 *\brief		Unbinds the render node's pass.
		 *\param[in]	p_scene			The scene.
		 *\~french
		 *\brief		Désctive la passe du noeud de rendu.
		 *\param[in]	p_scene			La scène.
		 */
		C3D_API virtual void UnbindPass( Scene const & p_scene )const = 0;

		//!\~english	The base render node.
		//!\~french		Le noeud de rendu.
		SceneRenderNode m_scene;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de la géométrie.
		GeometryBuffers & m_buffers;
		//!\~english	The parent scene node.
		//!\~french		Le scene node parent.
		SceneNode & m_sceneNode;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render objects.
	\~french
	\brief		Structure d'aide utilisée pour le dessin d'objets.
	*/
	template< typename DataType >
	struct ObjectRenderNode
		: public ObjectRenderNodeBase
	{
		ObjectRenderNode( SceneRenderNode && p_scene
						  , GeometryBuffers & p_buffers
						  , SceneNode & p_sceneNode
						  , DataType & p_data )
			: ObjectRenderNodeBase{ std::move( p_scene ), p_buffers, p_sceneNode }
			, m_data{ p_data }
		{
		}
		/**
		 *\~english
		 *\brief		Render function.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 */
		C3D_API void Render( Scene const & p_scene, Pipeline & p_pipeline )override;
		/**
		 *\~english
		 *\brief		Unbinds the render node's pass.
		 *\param[in]	p_scene			The scene.
		 *\~french
		 *\brief		Désctive la passe du noeud de rendu.
		 *\param[in]	p_scene			La scène.
		 */
		C3D_API void UnbindPass( Scene const & p_scene )const override;

		//!\~english	The object's data.
		//!\~french		Les données de l'objet.
		DataType & m_data;
	};
	using SubmeshRenderNode = ObjectRenderNode< Submesh >;
	using BillboardListRenderNode = ObjectRenderNode< BillboardList >;
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render static submeshes.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des sous-maillages non animés.
	*/
	struct StaticGeometryRenderNode
		: public SubmeshRenderNode
	{
		StaticGeometryRenderNode( SceneRenderNode && p_scene
								  , GeometryBuffers & p_buffers
								  , SceneNode & p_sceneNode
								  , Submesh & p_data
								  , Geometry & p_geometry )
			: SubmeshRenderNode{ std::move( p_scene ), p_buffers, p_sceneNode, p_data }
			, m_geometry{ p_geometry }
		{
		}
		/**
		 *\~english
		 *\brief		Binds the given pass to the render node.
		 *\param[in]	p_scene				The rendered scene.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée pour le noeud de rendu.
		 *\param[in]	p_scene				La scène rendue.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		C3D_API void BindPass( Scene const & p_scene, Pipeline & p_pipeline, uint64_t p_excludedMtxFlags )override;

		//!\~english	The geometry instanciating the submesh.
		//!\~french		La géométrie instanciant le submesh.
		Geometry & m_geometry;

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
		: public SubmeshRenderNode
	{
		AnimatedGeometryRenderNode( SceneRenderNode && p_scene
									, GeometryBuffers & p_buffers
									, SceneNode & p_sceneNode
									, Submesh & p_data
									, Geometry & p_geometry
									, AnimatedSkeleton * p_skeleton
									, AnimatedMesh * p_mesh
									, FrameVariableBuffer & p_animationUbo )
			: SubmeshRenderNode{ std::move( p_scene ), p_buffers, p_sceneNode, p_data }
			, m_geometry{ p_geometry }
			, m_skeleton{ p_skeleton }
			, m_mesh{ p_mesh }
			, m_animationUbo{ p_animationUbo }
		{
		}
		/**
		 *\~english
		 *\brief		Binds the given pass to the render node.
		 *\param[in]	p_scene				The rendered scene.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée pour le noeud de rendu.
		 *\param[in]	p_scene				La scène rendue.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		C3D_API void BindPass( Scene const & p_scene, Pipeline & p_pipeline, uint64_t p_excludedMtxFlags )override;

		//!\~english	The geometry instanciating the submesh.
		//!\~french		La géométrie instanciant le submesh.
		Geometry & m_geometry;
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
		: public BillboardListRenderNode
	{
		BillboardRenderNode ( SceneRenderNode && p_scene
							 , GeometryBuffers & p_buffers
							 , SceneNode & p_sceneNode
							 , BillboardList & p_data
							 , FrameVariableBuffer & p_billboardUbo
							 , Point2iFrameVariable & p_dimensions )
			: BillboardListRenderNode{ std::move( p_scene ), p_buffers, p_sceneNode, p_data }
			, m_billboardUbo{ p_billboardUbo }
			, m_dimensions{ p_dimensions }
		{
		}
		/**
		 *\~english
		 *\brief		Binds the given pass to the render node.
		 *\param[in]	p_scene				The rendered scene.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée pour le noeud de rendu.
		 *\param[in]	p_scene				La scène rendue.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		C3D_API void BindPass( Scene const & p_scene, Pipeline & p_pipeline, uint64_t p_excludedMtxFlags )override;

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
	//!\~english	BillboardRenderNode array.
	//!\~french		Tableau de BillboardRenderNode.
	DECLARE_VECTOR( BillboardRenderNode, BillboardRenderNode );
	//!\~english	Shader program sorted SubmeshRenderNodesMap map.
	//!\~french		Map de SubmeshRenderNodesMap, triés par programme shader.
	DECLARE_MAP( ShaderProgramSPtr, StaticGeometryRenderNodeArray, StaticGeometryRenderNodesByProgram );
	//!\~english	Shader program sorted SubmeshRenderNodesMap map.
	//!\~french		Map de SubmeshRenderNodesMap, triés par programme shader.
	DECLARE_MAP( ShaderProgramSPtr, AnimatedGeometryRenderNodeArray, AnimatedGeometryRenderNodesByProgram );
	//!\~english	Shader program sorted BillboardRenderNodesMap map.
	//!\~french		Map de BillboardRenderNodesMap, triés par programme shader.
	DECLARE_MAP( ShaderProgramSPtr, BillboardRenderNodeArray, BillboardRenderNodesByProgram );

	//!\~english	Submesh sorted StaticGeometryRenderNodeArray (for instantiation).
	//!\~french		Map StaticGeometryRenderNodeArray, triés par sous-maillage (pour l'instanciation).
	DECLARE_MAP( SubmeshSPtr, StaticGeometryRenderNodeArray, SubmeshStaticRenderNodes );
	//!\~english	Pass sorted SubmeshRenderNodesMap map.
	//!\~french		Map de SubmeshRenderNodesMap, triés par passe.
	template< typename T >
	struct TypeRenderNodesByPassMap
	{
	public:
		using key_type = typename std::map< PassSPtr, T >::key_type;
		using mapped_type = typename std::map< PassSPtr, T >::mapped_type;
		using value_type = typename std::map< PassSPtr, T >::value_type;

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

		inline auto find( key_type p_pass )const
		{
			return m_map.find( p_pass );
		}

		inline auto insert( std::pair< key_type, mapped_type > p_pair )
		{
			return m_map.insert( p_pair );
		}

	private:
		std::map< key_type, mapped_type > m_map;
	};
	using SubmeshStaticRenderNodesByPassMap = TypeRenderNodesByPassMap< SubmeshStaticRenderNodesMap >;

	//!\~english	Shader program sorted SubmeshRenderNodesMap map.
	//!\~french		Map de SubmeshRenderNodesMap, triés par programme shader.
	DECLARE_MAP( ShaderProgramSPtr, SubmeshStaticRenderNodesByPassMap, SubmeshStaticRenderNodesByProgram );

	//@}
}

#endif
