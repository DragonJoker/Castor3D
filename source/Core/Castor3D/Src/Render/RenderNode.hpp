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
	struct PassRenderNode
	{
		//!\~english	The pass.
		//!\~french		La passe.
		Pass & m_pass;
		//!\~english	The rendering pipeline.
		//!\~french		Le pipeline de rendu.
		Pipeline & m_pipeline;
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
		SceneRenderNode( FrameVariableBuffer & p_sceneUbo, Point3rFrameVariable & p_cameraPos );
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
		ObjectRenderNodeBase( SceneRenderNode const & p_scene
							  , PassRenderNode const & p_pass
							  , GeometryBuffers & p_buffers
							  , SceneNode & p_sceneNode );
		/**
		 *\~english
		 *\brief		Render function.
		 *\~french
		 *\brief		Fonction de rendu.
		 */
		C3D_API virtual void Render( DepthMapArray const & p_depthMaps ) = 0;
		/**
		 *\~english
		 *\brief		Binds the given pass to the render node.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée pour le noeud de rendu.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		C3D_API virtual void BindPass( DepthMapArray const & p_depthMaps, uint64_t p_excludedMtxFlags ) = 0;
		/**
		 *\~english
		 *\brief		Unbinds the render node's pass.
		 *\~french
		 *\brief		Désctive la passe du noeud de rendu.
		 */
		C3D_API virtual void UnbindPass( DepthMapArray const & p_depthMaps )const = 0;

		//!\~english	The scene render node.
		//!\~french		Le noeud de rendu de scène.
		SceneRenderNode m_scene;
		//!\~english	The pass render node.
		//!\~french		Le noeud de rendu de passe.
		PassRenderNode m_pass;
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
						  , PassRenderNode && p_pass
						  , GeometryBuffers & p_buffers
						  , SceneNode & p_sceneNode
						  , DataType & p_data )
			: ObjectRenderNodeBase{ std::move( p_scene ), std::move( p_pass ), p_buffers, p_sceneNode }
			, m_data{ p_data }
		{
		}
		/**
		 *\~english
		 *\brief		Render function.
		 *\~french
		 *\brief		Fonction de rendu.
		 */
		C3D_API void Render( DepthMapArray const & p_depthMaps );
		/**
		 *\~english
		 *\brief		Unbinds the render node's pass.
		 *\~french
		 *\brief		Désctive la passe du noeud de rendu.
		 */
		C3D_API void UnbindPass( DepthMapArray const & p_depthMaps )const;

		//!\~english	The object's data.
		//!\~french		Les données de l'objet.
		DataType & m_data;
	};
	using SubmeshRenderNode = ObjectRenderNode< Submesh >;
	using BillboardListGSRenderNode = ObjectRenderNode< BillboardGSBase >;
	using BillboardListInstRenderNode = ObjectRenderNode< BillboardInstBase >;
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render objects.
	\~french
	\brief		Structure d'aide utilisée pour le dessin d'objets.
	*/
	struct GeometryRenderNode
		: public SubmeshRenderNode
	{
		GeometryRenderNode( SceneRenderNode && p_scene
							, PassRenderNode && p_pass
							, GeometryBuffers & p_buffers
							, SceneNode & p_sceneNode
							, Submesh & p_data
							, Geometry & p_geometry )
			: SubmeshRenderNode{ std::move( p_scene ), std::move( p_pass ), p_buffers, p_sceneNode, p_data }
			, m_geometry{ p_geometry }
		{
		}
		/**
		 *\~english
		 *\brief		Render function.
		 *\~french
		 *\brief		Fonction de rendu.
		 */
		C3D_API void Render( DepthMapArray const & p_depthMaps );
		/**
		 *\~english
		 *\brief		Unbinds the render node's pass.
		 *\~french
		 *\brief		Désctive la passe du noeud de rendu.
		 */
		C3D_API void UnbindPass( DepthMapArray const & p_depthMaps )const;

		//!\~english	The geometry instanciating the submesh.
		//!\~french		La géométrie instanciant le submesh.
		Geometry & m_geometry;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render static submeshes.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des sous-maillages non animés.
	*/
	struct StaticGeometryRenderNode
		: public GeometryRenderNode
	{
		StaticGeometryRenderNode( SceneRenderNode && p_scene
								  , PassRenderNode && p_pass
								  , GeometryBuffers & p_buffers
								  , SceneNode & p_sceneNode
								  , Submesh & p_data
								  , Geometry & p_geometry )
			: GeometryRenderNode{ std::move( p_scene ), std::move( p_pass ), p_buffers, p_sceneNode, p_data, p_geometry }
		{
		}
		/**
		 *\~english
		 *\brief		Binds the given pass to the render node.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée pour le noeud de rendu.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		C3D_API void BindPass( DepthMapArray const & p_depthMaps, uint64_t p_excludedMtxFlags );
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
		: public GeometryRenderNode
	{
		AnimatedGeometryRenderNode( SceneRenderNode && p_scene
									, PassRenderNode && p_pass
									, GeometryBuffers & p_buffers
									, SceneNode & p_sceneNode
									, Submesh & p_data
									, Geometry & p_geometry
									, AnimatedSkeleton * p_skeleton
									, AnimatedMesh * p_mesh
									, FrameVariableBuffer & p_animationUbo )
			: GeometryRenderNode{ std::move( p_scene ), std::move( p_pass ), p_buffers, p_sceneNode, p_data, p_geometry }
			, m_skeleton{ p_skeleton }
			, m_mesh{ p_mesh }
			, m_animationUbo{ p_animationUbo }
		{
		}
		/**
		 *\~english
		 *\brief		Binds the given pass to the render node.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée pour le noeud de rendu.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		C3D_API void BindPass( DepthMapArray const & p_depthMaps, uint64_t p_excludedMtxFlags );

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
	struct BillboardGSRenderNode
		: public BillboardListGSRenderNode
	{
		BillboardGSRenderNode( SceneRenderNode && p_scene
							   , PassRenderNode && p_pass
							   , GeometryBuffers & p_buffers
							   , SceneNode & p_sceneNode
							   , BillboardGSBase & p_data
							   , FrameVariableBuffer & p_billboardUbo
							   , Point2iFrameVariable & p_dimensions )
			: BillboardListGSRenderNode{ std::move( p_scene ), std::move( p_pass ), p_buffers, p_sceneNode, p_data }
			, m_billboardUbo{ p_billboardUbo }
			, m_dimensions{ p_dimensions }
		{
		}
		/**
		 *\~english
		 *\brief		Binds the given pass to the render node.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée pour le noeud de rendu.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		C3D_API void BindPass( DepthMapArray const & p_depthMaps, uint64_t p_excludedMtxFlags );

		//!\~english	The billboard UBO.
		//!\~french		L'UBO de billboard.
		FrameVariableBuffer & m_billboardUbo;
		//!\~english	The dimensions uniform variable.
		//!\~french		La variable uniforme des dimensions.
		Point2iFrameVariable & m_dimensions;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render billboards.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des billboards.
	*/
	struct BillboardInstRenderNode
		: public BillboardListInstRenderNode
	{
		BillboardInstRenderNode( SceneRenderNode && p_scene
								 , PassRenderNode && p_pass
								 , GeometryBuffers & p_buffers
								 , SceneNode & p_sceneNode
								 , BillboardInstBase & p_data
								 , FrameVariableBuffer & p_billboardUbo
								 , Point2iFrameVariable & p_dimensions )
			: BillboardListInstRenderNode{ std::move( p_scene ), std::move( p_pass ), p_buffers, p_sceneNode, p_data }
			, m_billboardUbo{ p_billboardUbo }
			, m_dimensions{ p_dimensions }
		{
		}
		/**
		 *\~english
		 *\brief		Binds the given pass to the render node.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée pour le noeud de rendu.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		C3D_API void BindPass( DepthMapArray const & p_depthMaps, uint64_t p_excludedMtxFlags );

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
	//!\~english	BillboardGSRenderNode array.
	//!\~french		Tableau de BillboardGSRenderNode.
	DECLARE_VECTOR( BillboardGSRenderNode, BillboardGSRenderNode );
	//!\~english	BillboardInstRenderNode array.
	//!\~french		Tableau de BillboardInstRenderNode.
	DECLARE_VECTOR( BillboardInstRenderNode, BillboardInstRenderNode );
	//!\~english	Shader program sorted StaticGeometryRenderNodeArray map.
	//!\~french		Map de StaticGeometryRenderNodeArray, triés par programme shader.
	DECLARE_MAP( PipelineRPtr, StaticGeometryRenderNodeArray, StaticGeometryRenderNodesByPipeline );
	//!\~english	Shader program sorted AnimatedGeometryRenderNodeArray map.
	//!\~french		Map de AnimatedGeometryRenderNodeArray, triés par programme shader.
	DECLARE_MAP( PipelineRPtr, AnimatedGeometryRenderNodeArray, AnimatedGeometryRenderNodesByPipeline );
	//!\~english	Shader program sorted BillboardGSRenderNodeArray map.
	//!\~french		Map de BillboardGSRenderNodeArray, triés par programme shader.
	DECLARE_MAP( PipelineRPtr, BillboardGSRenderNodeArray, BillboardGSRenderNodesByPipeline );
	//!\~english	Shader program sorted BillboardInstRenderNodeArray map.
	//!\~french		Map de BillboardInstRenderNodeArray, triés par programme shader.
	DECLARE_MAP( PipelineRPtr, BillboardInstRenderNodeArray, BillboardInstRenderNodesByPipeline );

	//!\~english	Submesh sorted StaticGeometryRenderNodeArray (for instantiation).
	//!\~french		Map StaticGeometryRenderNodeArray, triés par sous-maillage (pour l'instanciation).
	DECLARE_MAP( SubmeshRPtr, StaticGeometryRenderNodeArray, SubmeshStaticRenderNodes );
	//!\~english	Pass sorted SubmeshRenderNodesMap map.
	//!\~french		Map de SubmeshRenderNodesMap, triés par passe.
	template< typename T >
	struct TypeRenderNodesByPassMap
	{
	public:
		using pass_ptr = PassRPtr;
		using key_type = typename std::map< pass_ptr, T >::key_type;
		using mapped_type = typename std::map< pass_ptr, T >::mapped_type;
		using value_type = typename std::map< pass_ptr, T >::value_type;

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

		inline auto size()const
		{
			return m_map.size();
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
	DECLARE_MAP( PipelineRPtr, SubmeshStaticRenderNodesByPassMap, SubmeshStaticRenderNodesByPipeline );

	//@}
}

#endif
