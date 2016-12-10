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
#ifndef ___C3D_RENDER_PASS_H___
#define ___C3D_RENDER_PASS_H___

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

#include "Render/RenderQueue.hpp"

#include <unordered_map>

namespace Castor3D
{
	inline bool operator<( PipelineFlags const & p_lhs, PipelineFlags const & p_rhs )
	{
		return p_lhs.m_colourBlendMode < p_rhs.m_colourBlendMode
			   || ( p_lhs.m_colourBlendMode == p_rhs.m_colourBlendMode
					&& ( p_lhs.m_alphaBlendMode < p_rhs.m_alphaBlendMode
						 || ( p_lhs.m_alphaBlendMode == p_rhs.m_alphaBlendMode
							  && ( p_lhs.m_textureFlags < p_rhs.m_textureFlags
								   || ( p_lhs.m_textureFlags == p_rhs.m_textureFlags
										&& ( p_lhs.m_programFlags < p_rhs.m_programFlags
											 || ( p_lhs.m_programFlags == p_rhs.m_programFlags
												  && p_lhs.m_sceneFlags < p_rhs.m_sceneFlags ) ) ) ) ) ) );
	}
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Render pass base class.
	\~french
	\brief		Classe de base d'une passe de rendu.
	*/
	class RenderPass
		: public Castor::OwnedBy< Engine >
		, public Castor::Named
	{
	public:
		using DistanceSortedNodeMap = std::multimap< double, std::reference_wrapper< ObjectRenderNodeBase > >;

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name			The pass name.
		 *\param[in]	p_renderTarget	The engine.
		 *\param[in]	p_opaque		Tells if this pass if for opaque nodes.
		 *\param[in]	p_multisampling	The multisampling status.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name			Le nom de la passe.
		 *\param[in]	p_renderTarget	Le moteur.
		 *\param[in]	p_opaque		Dit si cette passe de rendu est pour les noeuds opaques.
		 *\param[in]	p_multisampling	Le statut de multiéchantillonnage.
		 */
		C3D_API RenderPass( Castor::String const & p_name
			, Engine & p_engine
			, bool p_opaque
			, bool p_multisampling = false );

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~RenderPass();
		/**
		 *\~english
		 *\brief		Initialises the pass.
		 *\param		p_size	The pass needed dimensions.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param		p_size	Les dimensions voulues pour la passe.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Initialise( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Updates the scenes render nodes, if needed.
		 *\~french
		 *\brief		Met les noeuds de scènes à jour, si nécessaire.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags.
		 *\param[in]	p_textureFlags	Bitwise ORed TextureChannel.
		 *\param[in]	p_programFlags	Bitwise ORed ProgramFlag.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\param[in]	p_invertNormals	Tells if the normals must be inverted, in the program.
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux indicateurs donnés.
		 *\param[in]	p_textureFlags	Combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 *\param[in]	p_invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 */
		C3D_API Castor::String GetVertexShaderSource( TextureChannels const & p_textureFlags
			,ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags
			, bool p_invertNormals )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 */
		C3D_API Castor::String GetPixelShaderSource( TextureChannels const & p_textureFlags
			,ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the geometry shader source matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\~french
		 *\brief		Récupère le source du geometry shader qui correspond aux indicateurs donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 */
		C3D_API Castor::String GetGeometryShaderSource( TextureChannels const & p_textureFlags
			,ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags )const;
		/**
		 *\~english
		 *\brief		Prepares the pipeline matching the given flags.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\param[in]	p_colourBlendMode	The alpha blend mode.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags		Scene related flags.
		 *\param[in]	p_twoSided			Tells if the pass is two sided.
		 *\~french
		 *\brief		Prépare le pipeline qui correspond aux indicateurs donnés.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	p_colourBlendMode	Le mode de mélange alpha.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	p_twoSided			Dit si la passe est sur les deux faces.
		 */
		C3D_API void PreparePipeline( BlendMode p_colourBlendMode
			, BlendMode p_alphaBlendMode
			, TextureChannels & p_textureFlags
			, ProgramFlags & p_programFlags
			, uint8_t p_sceneFlags
			, bool p_twoSided );
		/**
		 *\~english
		 *\brief		Retrieves the pipeline matching the given flags, for front face culling.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\param[in]	p_alphaBlendMode	The alpha blend mode.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags		Scene related flags.
		 *\return		The ready to use pipeline, \p nullptr if none is available for wanted configuration.
		 *\~french
		 *\brief		Récupère le pipeline qui correspond aux indicateurs donnés, pour les faces avant supprimées.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	p_alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags		Les indicateurs relatifs à la scène.
		 *\return		Le pipeline prêt à l'utilisation, \p nullptr si aucun pipeline n'est disponible pour la configuration voulue.
		 */
		C3D_API RenderPipeline * GetPipelineFront( BlendMode p_colourBlendMode
			, BlendMode p_alphaBlendMode
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags );
		/**
		 *\~english
		 *\brief		Retrieves the pipeline matching the given flags, for back face culling.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\param[in]	p_alphaBlendMode	The alpha blend mode.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags		Scene related flags.
		 *\return		The ready to use pipeline, \p nullptr if none is available for wanted configuration.
		 *\~french
		 *\brief		Récupère le pipeline qui correspond aux indicateurs donnés, pour les faces arrière supprimées.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	p_alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags		Les indicateurs relatifs à la scène.
		 *\return		Le pipeline prêt à l'utilisation, \p nullptr si aucun pipeline n'est disponible pour la configuration voulue.
		 */
		C3D_API RenderPipeline * GetPipelineBack( BlendMode p_colourBlendMode
			, BlendMode p_alphaBlendMode
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags );
		/**
		 *\~english
		 *\brief		Creates an animated render node.
		 *\param[in]	p_pass		The pass.
		 *\param[in]	p_pipeline	The pipeline.
		 *\param[in]	p_submesh	The submesh.
		 *\param[in]	p_primitive	The geometry.
		 *\param[in]	p_skeleton	The animated skeleton.
		 *\param[in]	p_mesh		The animated mesh.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu animé.
		 *\param[in]	p_pass		La passe.
		 *\param[in]	p_pipeline	Le pipeline.
		 *\param[in]	p_submesh	Le sous-maillage.
		 *\param[in]	p_primitive	La géométrie.
		 *\param[in]	p_skeleton	Le squelette animé.
		 *\param[in]	p_mesh		Le maillage animé.
		 *\return		Le noeud de rendu.
		 */
		C3D_API AnimatedGeometryRenderNode CreateAnimatedNode( Pass & p_pass
			, RenderPipeline & p_pipeline
			, Submesh & p_submesh
			, Geometry & p_primitive
			, AnimatedSkeletonSPtr p_skeleton
			, AnimatedMeshSPtr p_mesh );
		/**
		 *\~english
		 *\brief		Creates a static render node.
		 *\param[in]	p_pass		The pass.
		 *\param[in]	p_pipeline	The pipeline.
		 *\param[in]	p_submesh	The submesh.
		 *\param[in]	p_primitive	The geometry.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu statique.
		 *\param[in]	p_pass		La passe.
		 *\param[in]	p_pipeline	Le pipeline.
		 *\param[in]	p_submesh	Le sous-maillage.
		 *\param[in]	p_primitive	La géométrie.
		 *\return		Le noeud de rendu.
		 */
		C3D_API StaticGeometryRenderNode CreateStaticNode( Pass & p_pass
			, RenderPipeline & p_pipeline
			, Submesh & p_submesh
			, Geometry & p_primitive );
		/**
		 *\~english
		 *\brief		Creates a static render node.
		 *\param[in]	p_pass		The pass.
		 *\param[in]	p_pipeline	The pipeline.
		 *\param[in]	p_billboard	The billboard.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu statique.
		 *\param[in]	p_pass		La passe.
		 *\param[in]	p_pipeline	Le pipeline.
		 *\param[in]	p_billboard	Le billboard.
		 *\return		Le noeud de rendu.
		 */
		C3D_API BillboardRenderNode CreateBillboardNode( Pass & p_pass
			, RenderPipeline & p_pipeline
			, BillboardBase & p_billboard );
		/**
		 *\~english
		 *\brief		Updates the opaque pipeline.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\~french
		 *\brief		Met à jour lee pipeline opaque.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 */
		C3D_API void UpdatePipeline( Camera const & p_camera
			, RenderPipeline & p_pipeline
			, DepthMapArray & p_depthMaps )const;
		/**
		 *\~english
		 *\return		The multsampling status.
		 *\~french
		 *\return		Le statut d'échantillonnage.
		 */
		inline bool IsMultisampling()const
		{
			return m_multisampling;
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates a render node.
		 *\param[in]	p_pass		The pass.
		 *\param[in]	p_pipeline	The pipeline.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu.
		 *\param[in]	p_pass		La passe.
		 *\param[in]	p_pipeline	Le pipeline.
		 *\return		Le noeud de rendu.
		 */
		C3D_API PassRenderNode DoCreatePassRenderNode( Pass & p_pass
			, RenderPipeline & p_pipeline );
		/**
		 *\~english
		 *\brief		Creates a scene render node.
		 *\param[in]	p_pass		The pass.
		 *\param[in]	p_pipeline	The pipeline.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu de scène.
		 *\param[in]	p_pass		La passe.
		 *\param[in]	p_pipeline	Le pipeline.
		 *\return		Le noeud de rendu.
		 */
		C3D_API SceneRenderNode DoCreateSceneRenderNode( Scene & p_scene
			, RenderPipeline & p_pipeline );
		/**
		 *\~english
		 *\brief		Retrieves the shader program matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\param[in]	p_invertNormals	Tells if the normals must be inverted, in the program.
		 *\~french
		 *\brief		Récupère le programme shader correspondant aux flags donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 *\param[in]	p_invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 */
		C3D_API ShaderProgramSPtr DoGetProgram( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags
			, bool p_invertNormals )const;
		/**
		 *\~english
		 *\brief		Copies the instanced nodes model matrices into given matrix buffer.
		 *\param[in]	p_renderNodes	The instanced nodes.
		 *\param[in]	p_matrixBuffer	The matrix buffer.
		 *\~french
		 *\brief		Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\param[in]	p_renderNodes	Les noeuds instanciés.
		 *\param[in]	p_matrixBuffer	Le tampon de matrices.
		 */
		C3D_API uint32_t DoCopyNodesMatrices( StaticGeometryRenderNodeArray const & p_renderNodes
			, VertexBuffer & p_matrixBuffer );
		/**
		 *\~english
		 *\brief			Copies the instanced nodes model matrices into given matrix buffer.
		 *\remarks			The nodes which are copied will be registered in the rendered nodes list.
		 *\param[in]		p_renderNodes	The instanced nodes.
		 *\param[in]		p_matrixBuffer	The matrix buffer.
		 *\param[in,out]	p_count			Receives the rendered nodes count.
		 *\~french
		 *\brief			Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\remarks			Les noeuds pour lesquels les matrices sont copiées seront enregistrés dans la liste des noeuds dessinés.
		 *\param[in]		p_renderNodes	Les noeuds instanciés.
		 *\param[in]		p_matrixBuffer	Le tampon de matrices.
		 *\param[in,out]	p_count			Reçoit le nombre de noeuds dessinés.
		 */
		C3D_API uint32_t DoCopyNodesMatrices( StaticGeometryRenderNodeArray const & p_renderNodes
			, VertexBuffer & p_matrixBuffer
			, uint32_t & p_count );
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	p_nodes		The render nodes.
		 *\param		p_camera	The viewing camera.
		 *\param[in]	p_depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 *\param		p_camera	La caméra regardant la scène.
		 *\param[in]	p_depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRenderInstancedSubmeshes( SubmeshStaticRenderNodesByPipelineMap & p_nodes
			, DepthMapArray & p_depthMaps );
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	p_nodes		The render nodes.
		 *\param		p_camera	The viewing camera.
		 *\param[in]	p_depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 *\param		p_camera	La caméra regardant la scène.
		 *\param[in]	p_depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRenderInstancedSubmeshes( SubmeshStaticRenderNodesByPipelineMap & p_nodes
			, Camera const & p_camera
			, DepthMapArray & p_depthMaps );
		/**
		 *\~english
		 *\brief			Renders instanced submeshes.
		 *\param[in]		p_nodes		The render nodes.
		 *\param[in]		p_depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	p_count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine des sous maillages instanciés.
		 *\param[in]		p_nodes		Les noeuds de rendu.
		 *\param[in]		p_depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in, out]	p_count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void DoRenderInstancedSubmeshes( SubmeshStaticRenderNodesByPipelineMap & p_nodes
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count );
		/**
		 *\~english
		 *\brief			Renders instanced submeshes.
		 *\param[in]		p_nodes		The render nodes.
		 *\param			p_camera	The viewing camera.
		 *\param[in]		p_depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	p_count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine des sous maillages instanciés.
		 *\param[in]		p_nodes		Les noeuds de rendu.
		 *\param			p_camera	La caméra regardant la scène.
		 *\param[in]		p_depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in, out]	p_count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void DoRenderInstancedSubmeshes( SubmeshStaticRenderNodesByPipelineMap & p_nodes
			, Camera const & p_camera
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count );
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	p_nodes		The render nodes.
		 *\param		p_camera	The viewing camera.
		 *\param[in]	p_depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 *\param		p_camera	La caméra regardant la scène.
		 *\param[in]	p_depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRenderStaticSubmeshes( StaticGeometryRenderNodesByPipelineMap & p_nodes
			, Camera const & p_camera
			, DepthMapArray & p_depthMaps );
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	p_nodes		The render nodes.
		 *\param[in]	p_depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 *\param[in]	p_depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRenderStaticSubmeshes( StaticGeometryRenderNodesByPipelineMap & p_nodes
			, DepthMapArray & p_depthMaps );
		/**
		 *\~english
		 *\brief			Renders non instanced submeshes.
		 *\param[in]		p_nodes		The render nodes.
		 *\param			p_camera	The viewing camera.
		 *\param[in]		p_depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	p_count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine des sous maillages non instanciés.
		 *\param[in]		p_nodes		Les noeuds de rendu.
		 *\param			p_camera	La caméra regardant la scène.
		 *\param[in]		p_depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in, out]	p_count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void DoRenderStaticSubmeshes( StaticGeometryRenderNodesByPipelineMap & p_nodes
			, Camera const & p_camera
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count );
		/**
		 *\~english
		 *\brief			Renders non instanced submeshes.
		 *\param[in]		p_nodes		The render nodes.
		 *\param[in]		p_depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	p_count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine des sous maillages non instanciés.
		 *\param[in]		p_nodes		Les noeuds de rendu.
		 *\param[in]		p_depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in, out]	p_count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void DoRenderStaticSubmeshes( StaticGeometryRenderNodesByPipelineMap & p_nodes
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count );
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	p_nodes		The render nodes.
		 *\param		p_camera	The viewing camera.
		 *\param[in]	p_depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 *\param		p_camera	La caméra regardant la scène.
		 *\param[in]	p_depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRenderAnimatedSubmeshes( AnimatedGeometryRenderNodesByPipelineMap & p_nodes
			, Camera const & p_camera
			, DepthMapArray & p_depthMaps );
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	p_nodes		The render nodes.
		 *\param[in]	p_depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 *\param[in]	p_depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRenderAnimatedSubmeshes( AnimatedGeometryRenderNodesByPipelineMap & p_nodes
			, DepthMapArray & p_depthMaps );
		/**
		 *\~english
		 *\brief			Renders non instanced submeshes.
		 *\param[in]		p_nodes		The render nodes.
		 *\param			p_camera	The viewing camera.
		 *\param[in]		p_depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	p_count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine des sous maillages non instanciés.
		 *\param[in]		p_nodes		Les noeuds de rendu.
		 *\param			p_camera	La caméra regardant la scène.
		 *\param[in]		p_depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in, out]	p_count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void DoRenderAnimatedSubmeshes( AnimatedGeometryRenderNodesByPipelineMap & p_nodes
			, Camera const & p_camera
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count );
		/**
		 *\~english
		 *\brief			Renders non instanced submeshes.
		 *\param[in]		p_nodes		The render nodes.
		 *\param[in]		p_depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	p_count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine des sous maillages non instanciés.
		 *\param[in]		p_nodes		Les noeuds de rendu.
		 *\param[in]		p_depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in, out]	p_count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void DoRenderAnimatedSubmeshes( AnimatedGeometryRenderNodesByPipelineMap & p_nodes
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count );
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	p_nodes		The render nodes.
		 *\param		p_camera	The viewing camera.
		 *\param[in]	p_depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 *\param		p_camera	La caméra regardant la scène.
		 *\param[in]	p_depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRenderBillboards( BillboardRenderNodesByPipelineMap & p_nodes
			, Camera const & p_camera
			, DepthMapArray & p_depthMaps );
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	p_nodes		The render nodes.
		 *\param[in]	p_depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 *\param[in]	p_depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRenderBillboards( BillboardRenderNodesByPipelineMap & p_nodes
			, DepthMapArray & p_depthMaps );
		/**
		 *\~english
		 *\brief			Renders billboards.
		 *\param[in]		p_nodes		The render nodes.
		 *\param			p_camera	The viewing camera.
		 *\param[in]		p_depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	p_count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine des billboards.
		 *\param[in]		p_nodes		Les noeuds de rendu.
		 *\param			p_camera	La caméra regardant la scène.
		 *\param[in]		p_depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in, out]	p_count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void DoRenderBillboards( BillboardRenderNodesByPipelineMap & p_nodes
			, Camera const & p_camera
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count );
		/**
		 *\~english
		 *\brief			Renders billboards.
		 *\param[in]		p_nodes		The render nodes.
		 *\param[in]		p_depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	p_count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine des billboards.
		 *\param[in]		p_nodes		Les noeuds de rendu.
		 *\param[in]		p_depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in, out]	p_count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void DoRenderBillboards( BillboardRenderNodesByPipelineMap & p_nodes
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count );

	private:
		/**
		 *\~english
		 *\brief		Initialises the pass.
		 *\param		p_size	The pass needed dimensions.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param		p_size	Les dimensions voulues pour la passe.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool DoInitialise( Castor::Size const & p_size ) = 0;
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		C3D_API virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Updates the specific data.
		 *\~french
		 *\brief		Met les données spécifiques.
		 */
		C3D_API virtual void DoUpdate() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux indicateurs donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 */
		C3D_API virtual Castor::String DoGetPixelShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags.
		 *\param[in]	p_textureFlags	Bitwise ORed TextureChannel.
		 *\param[in]	p_programFlags	Bitwise ORed ProgramFlag.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\param[in]	p_invertNormals	Tells if the normals must be inverted, in the program.
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux indicateurs donnés.
		 *\param[in]	p_textureFlags	Combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 *\param[in]	p_invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 */
		C3D_API virtual Castor::String DoGetVertexShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags
			, bool p_invertNormals )const;
		/**
		 *\~english
		 *\brief		Retrieves the geometry shader source matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\~french
		 *\brief		Récupère le source du geometry shader correspondant aux indicateurs donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 */
		C3D_API virtual Castor::String DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags )const = 0;
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	p_textureFlags	A combination of TextureChannel.
		 *\param[in,out]	p_programFlags	A combination of ProgramFlag.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in,out]	p_programFlags	Une combinaison de ProgramFlag.
		 */
		C3D_API virtual void DoUpdateFlags( TextureChannels & p_textureFlags
			, ProgramFlags & p_programFlags )const = 0;
		/**
		 *\~english
		 *\brief		Updates the pipeline.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\~french
		 *\brief		Met à jour lee pipeline.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 */
		C3D_API virtual void DoUpdatePipeline( RenderPipeline & p_pipeline
			, DepthMapArray & p_depthMaps )const = 0;
		/**
		 *\~english
		 *\brief		Prepares the pipeline, culling front faces.
		 *\~french
		 *\brief		Prépare le pipeline de rendu, en supprimant les faces avant.
		 */
		C3D_API virtual void DoPrepareFrontPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags ) = 0;
		/**
		 *\~english
		 *\brief		Prepares the pipeline, culling back faces.
		 *\~french
		 *\brief		Prépare le pipeline de rendu, en supprimant les faces arrière.
		 */
		C3D_API virtual void DoPrepareBackPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags ) = 0;

	protected:
		//!\~english	The render system.
		//!\~french		Le render system.
		RenderSystem & m_renderSystem;
		//!\~english	The render queue.
		//!\~french		La file de rendu.
		RenderQueue m_renderQueue;
		//!\~english	Tells if this pass is used for opaque render nodes.
		//!\~french		Dit si cette passe est utilisée pour les noeuds de rendu opaques.
		bool m_opaque{ false };
		//!\~english	The pipelines used to render nodes' back faces.
		//!\~french		Les pipelines de rendu utilisés pour dessiner les faces arrière des noeuds.
		std::map< PipelineFlags, RenderPipelineUPtr > m_frontPipelines;
		//!\~english	The pipelines used to render nodes' front faces.
		//!\~french		Les pipelines de rendu utilisés pour dessiner les faces avant noeuds.
		std::map< PipelineFlags, RenderPipelineUPtr > m_backPipelines;
		//!\~english	The geometries buffers.
		//!\~french		Les tampons de géométries.
		std::set< GeometryBuffersSPtr > m_geometryBuffers;
		//!\~english	Tells if the technique uses multisampling.
		//!\~french		Dit si la technique utilise le multiéchantillonnage.
		bool m_multisampling{ false };
	};
}

#endif
