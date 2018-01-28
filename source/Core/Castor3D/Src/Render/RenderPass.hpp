/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_PASS_H___
#define ___C3D_RENDER_PASS_H___

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

#include "Mesh\SubmeshComponent\InstantiationComponent.hpp"
#include "Render/RenderInfo.hpp"
#include "Render/RenderQueue.hpp"
#include "Shader/Ubos/BillboardUbo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "Shader/Ubos/ModelUbo.hpp"
#include "Shader/Ubos/MorphingUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Shader/Ubos/SkinningUbo.hpp"

#include <unordered_map>

namespace castor3d
{
	inline bool operator<( PipelineFlags const & lhs, PipelineFlags const & rhs )
	{
		return lhs.m_colourBlendMode < rhs.m_colourBlendMode
			   || ( lhs.m_colourBlendMode == rhs.m_colourBlendMode
					&& ( lhs.m_alphaBlendMode < rhs.m_alphaBlendMode
						 || ( lhs.m_alphaBlendMode == rhs.m_alphaBlendMode
							  && ( lhs.m_textureFlags < rhs.m_textureFlags
								   || ( lhs.m_textureFlags == rhs.m_textureFlags
										&& ( lhs.m_programFlags < rhs.m_programFlags
											 || ( lhs.m_programFlags == rhs.m_programFlags
												  && ( lhs.m_passFlags < rhs.m_passFlags
													 || ( lhs.m_passFlags == rhs.m_passFlags
														  && lhs.m_sceneFlags < rhs.m_sceneFlags ) ) ) ) ) ) ) ) );
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
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
		using DistanceSortedNodeMap = std::multimap< double, std::unique_ptr< DistanceRenderNodeBase > >;

	protected:
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	name	The pass name.
		 *\param[in]	engine	The engine.
		 *\param[in]	ignored	The geometries attached to this node will be ignored in the render.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	name	Le nom de la passe.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	ignored	Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 */
		C3D_API RenderPass( castor::String const & name
			, Engine & engine
			, SceneNode const * ignored );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	name	The pass name.
		 *\param[in]	engine	The engine.
		 *\param[in]	oit		The order independant status.
		 *\param[in]	ignored	The geometries attached to this node will be ignored in the render.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	name	Le nom de la passe.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	oit		Le statut de rendu indépendant de l'ordre des objets.
		 *\param[in]	ignored	Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 */
		C3D_API RenderPass( castor::String const & name
			, Engine & engine
			, bool oit
			, SceneNode const * ignored );

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
		 *\param		size	The pass needed dimensions.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param		size	Les dimensions voulues pour la passe.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( castor::Size const & size );
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the render pass.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met à jour la passe de rendu.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void update( RenderQueueArray & queues );
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux indicateurs donnés.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 */
		C3D_API glsl::Shader getVertexShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 */
		C3D_API glsl::Shader getPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const;
		/**
		 *\~english
		 *\brief		Retrieves the geometry shader source matching the given flags.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Récupère le source du geometry shader qui correspond aux indicateurs donnés.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API glsl::Shader getGeometryShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const;
		/**
		 *\~english
		 *\brief		Prepares the pipeline matching the given flags.
		 *\param[in]	colourBlendMode	The colour blend mode.
		 *\param[in]	alphaBlendMode	The alpha blend mode.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	twoSided		Tells if the pass is two sided.
		 *\~french
		 *\brief		Prépare le pipeline qui correspond aux indicateurs donnés.
		 *\param[in]	colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	twoSided		Dit si la passe est sur les deux faces.
		 */
		C3D_API void preparePipeline( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, renderer::CompareOp alphaFunc
			, PassFlags & passFlags
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags
			, bool twoSided );
		/**
		 *\~english
		 *\brief		Retrieves the pipeline matching the given flags, for front face culling.
		 *\param[in]	colourBlendMode	The colour blend mode.
		 *\param[in]	alphaBlendMode	The alpha blend mode.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\return		The ready to use pipeline, \p nullptr if none is available for wanted configuration.
		 *\~french
		 *\brief		Récupère le pipeline qui correspond aux indicateurs donnés, pour les faces avant supprimées.
		 *\param[in]	colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\return		Le pipeline prêt à l'utilisation, \p nullptr si aucun pipeline n'est disponible pour la configuration voulue.
		 */
		C3D_API RenderPipeline * getPipelineFront( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, renderer::CompareOp alphaFunc
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pipeline matching the given flags, for back face culling.
		 *\param[in]	colourBlendMode	The colour blend mode.
		 *\param[in]	alphaBlendMode	The alpha blend mode.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\return		The ready to use pipeline, \p nullptr if none is available for wanted configuration.
		 *\~french
		 *\brief		Récupère le pipeline qui correspond aux indicateurs donnés, pour les faces arrière supprimées.
		 *\param[in]	colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\return		Le pipeline prêt à l'utilisation, \p nullptr si aucun pipeline n'est disponible pour la configuration voulue.
		 */
		C3D_API RenderPipeline * getPipelineBack( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, renderer::CompareOp alphaFunc
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const;
		/**
		 *\~english
		 *\brief		Creates an animated render node.
		 *\param[in]	pass		The pass.
		 *\param[in]	pipeline	The pipeline.
		 *\param[in]	submesh		The submesh.
		 *\param[in]	primitive	The geometry.
		 *\param[in]	skeleton	The animated skeleton.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu animé.
		 *\param[in]	pass		La passe.
		 *\param[in]	pipeline	Le pipeline.
		 *\param[in]	submesh		Le sous-maillage.
		 *\param[in]	primitive	La géométrie.
		 *\param[in]	skeleton	Le squelette animé.
		 *\return		Le noeud de rendu.
		 */
		C3D_API SkinningRenderNode createSkinningNode( Pass & pass
			, RenderPipeline & pipeline
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedSkeleton & skeleton );
		/**
		 *\~english
		 *\brief		Creates an animated render node.
		 *\param[in]	pass		The pass.
		 *\param[in]	pipeline	The pipeline.
		 *\param[in]	submesh		The submesh.
		 *\param[in]	primitive	The geometry.
		 *\param[in]	mesh		The animated mesh.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu animé.
		 *\param[in]	pass		La passe.
		 *\param[in]	pipeline	Le pipeline.
		 *\param[in]	submesh		Le sous-maillage.
		 *\param[in]	primitive	La géométrie.
		 *\param[in]	mesh		Le maillage animé.
		 *\return		Le noeud de rendu.
		 */
		C3D_API MorphingRenderNode createMorphingNode( Pass & pass
			, RenderPipeline & pipeline
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedMesh & mesh );
		/**
		 *\~english
		 *\brief		Creates a static render node.
		 *\param[in]	pass		The pass.
		 *\param[in]	pipeline	The pipeline.
		 *\param[in]	submesh		The submesh.
		 *\param[in]	primitive	The geometry.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu statique.
		 *\param[in]	pass		La passe.
		 *\param[in]	pipeline	Le pipeline.
		 *\param[in]	submesh		Le sous-maillage.
		 *\param[in]	primitive	La géométrie.
		 *\return		Le noeud de rendu.
		 */
		C3D_API StaticRenderNode createStaticNode( Pass & pass
			, RenderPipeline & pipeline
			, Submesh & submesh
			, Geometry & primitive );
		/**
		 *\~english
		 *\brief		Creates a static render node.
		 *\param[in]	pass		The pass.
		 *\param[in]	pipeline	The pipeline.
		 *\param[in]	billboard	The billboard.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu statique.
		 *\param[in]	pass		La passe.
		 *\param[in]	pipeline	Le pipeline.
		 *\param[in]	billboard	Le billboard.
		 *\return		Le noeud de rendu.
		 */
		C3D_API BillboardRenderNode createBillboardNode( Pass & pass
			, RenderPipeline & pipeline
			, BillboardBase & billboard );
		/**
		 *\~english
		 *\brief		Updates the opaque pipeline.
		 *\param[in]	pipeline	The render pipeline.
		 *\~french
		 *\brief		Met à jour lee pipeline opaque.
		 *\param[in]	pipeline	Le pipeline de rendu.
		 */
		C3D_API void updatePipeline( RenderPipeline & pipeline )const;
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	passFlags		A combination of PassFlag.
		 *\param[in,out]	textureFlags	A combination of TextureChannel.
		 *\param[in,out]	programFlags	A combination of ProgramFlag.
		 *\param[in,out]	sceneFlags		A combination of SceneFlag.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	passFlags		Une combinaison de PassFlag.
		 *\param[in,out]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in,out]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in,out]	sceneFlags		Une combinaison de SceneFlag.
		 */
		C3D_API void updateFlags( PassFlags & passFlags
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const;
		/**
		 *\~english
		 *\return		The multsampling status.
		 *\~french
		 *\return		Le statut d'échantillonnage.
		 */
		inline bool isOrderIndependent()const
		{
			return m_oit;
		}
		/**
		 *\~english
		 *\return		The Scene UBO.
		 *\~french
		 *\return		L'UBO de la scène.
		 */
		inline SceneUbo & getSceneUbo()
		{
			return m_sceneUbo;
		}
		/**
		 *\~english
		 *\return		The command buffer.
		 *\~english
		 *\return		Le tampon de commandes.
		 */
		inline renderer::CommandBuffer const & getCommandBuffer( uint32_t index = 0u )const
		{
			REQUIRE( m_commandBuffers.size() > index );
			REQUIRE( m_commandBuffers[index] );
			return *m_commandBuffers[index];
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates a render node.
		 *\param[in]	pass		The pass.
		 *\param[in]	pipeline	The pipeline.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu.
		 *\param[in]	pass		La passe.
		 *\param[in]	pipeline	Le pipeline.
		 *\return		Le noeud de rendu.
		 */
		C3D_API PassRenderNode doCreatePassRenderNode( Pass & pass
			, RenderPipeline & pipeline );
		/**
		 *\~english
		 *\brief		Creates a scene render node.
		 *\param[in]	scene		The scene.
		 *\param[in]	pipeline	The pipeline.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu de scène.
		 *\param[in]	scene		La scène.
		 *\param[in]	pipeline	Le pipeline.
		 *\return		Le noeud de rendu.
		 */
		C3D_API SceneRenderNode doCreateSceneRenderNode( Scene & scene
			, RenderPipeline & pipeline );
		/**
		 *\~english
		 *\brief		Retrieves the shader program matching the given flags.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\~french
		 *\brief		Récupère le programme shader correspondant aux flags donnés.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 */
		C3D_API renderer::ShaderProgramPtr doGetProgram( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc
			, bool invertNormals )const;
		/**
		 *\~english
		 *\brief		Copies the instanced nodes model matrices into given matrix buffer.
		 *\param[in]	renderNodes		The instanced nodes.
		 *\param[in]	matrixBuffer	The matrix buffer.
		 *\~french
		 *\brief		Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\param[in]	renderNodes		Les noeuds instanciés.
		 *\param[in]	matrixBuffer	Le tampon de matrices.
		 */
		C3D_API uint32_t doCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
			, std::vector< InstantiationData > & matrixBuffer )const;
		/**
		 *\~english
		 *\brief			Copies the instanced nodes model matrices into given matrix buffer.
		 *\remarks			The nodes which are copied will be registered in the rendered nodes list.
		 *\param[in]		renderNodes		The instanced nodes.
		 *\param[in]		matrixBuffer	The matrix buffer.
		 *\param[in, out]	info			Receives the render informations.
		 *\~french
		 *\brief			Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\remarks			Les noeuds pour lesquels les matrices sont copiées seront enregistrés dans la liste des noeuds dessinés.
		 *\param[in]		renderNodes		Les noeuds instanciés.
		 *\param[in]		matrixBuffer	Le tampon de matrices.
		 *\param[in,out]	info			Reçoit les informations de rendu.
		 */
		C3D_API uint32_t doCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
			, std::vector< InstantiationData > & matrixBuffer
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Copies the instanced nodes model matrices into given matrix buffer.
		 *\param[in]	renderNodes		The instanced nodes.
		 *\param[in]	matrixBuffer	The matrix buffer.
		 *\~french
		 *\brief		Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\param[in]	renderNodes		Les noeuds instanciés.
		 *\param[in]	matrixBuffer	Le tampon de matrices.
		 */
		C3D_API uint32_t doCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
			, std::vector< InstantiationData > & matrixBuffer )const;
		/**
		 *\~english
		 *\brief			Copies the instanced nodes model matrices into given matrix buffer.
		 *\remarks			The nodes which are copied will be registered in the rendered nodes list.
		 *\param[in]		renderNodes		The instanced nodes.
		 *\param[in]		matrixBuffer	The matrix buffer.
		 *\param[in, out]	info			Receives the render informations.
		 *\~french
		 *\brief			Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\remarks			Les noeuds pour lesquels les matrices sont copiées seront enregistrés dans la liste des noeuds dessinés.
		 *\param[in]		renderNodes		Les noeuds instanciés.
		 *\param[in]		matrixBuffer	Le tampon de matrices.
		 *\param[in,out]	info			Reçoit les informations de rendu.
		 */
		C3D_API uint32_t doCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
			, std::vector< InstantiationData > & matrixBuffer
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Copies the instanced nodes model matrices into given matrix buffer.
		 *\param[in]	renderNodes		The instanced nodes.
		 *\param[in]	camera			The viewing camera.
		 *\param[in]	matrixBuffer	The matrix buffer.
		 *\~french
		 *\brief		Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\param[in]	renderNodes		Les noeuds instanciés.
		 *\param[in]	camera			La caméra regardant la scène.
		 *\param[in]	matrixBuffer	Le tampon de matrices.
		 */
		C3D_API uint32_t doCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
			, Camera const & camera
			, std::vector< InstantiationData > & matrixBuffer )const;
		/**
		 *\~english
		 *\brief			Copies the instanced nodes model matrices into given matrix buffer.
		 *\remarks			The nodes which are copied will be registered in the rendered nodes list.
		 *\param[in]		renderNodes		The instanced nodes.
		 *\param[in]		camera			The viewing camera.
		 *\param[in]		matrixBuffer	The matrix buffer.
		 *\param[in, out]	info			Receives the render informations.
		 *\~french
		 *\brief			Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\remarks			Les noeuds pour lesquels les matrices sont copiées seront enregistrés dans la liste des noeuds dessinés.
		 *\param[in]		renderNodes		Les noeuds instanciés.
		 *\param[in]		camera			La caméra regardant la scène.
		 *\param[in]		matrixBuffer	Le tampon de matrices.
		 *\param[in,out]	info			Reçoit les informations de rendu.
		 */
		C3D_API uint32_t doCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
			, Camera const & camera
			, std::vector< InstantiationData > & matrixBuffer
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Copies the instanced nodes model matrices into given matrix buffer.
		 *\param[in]	renderNodes		The instanced nodes.
		 *\param[in]	camera			The viewing camera.
		 *\param[in]	matrixBuffer	The matrix buffer.
		 *\~french
		 *\brief		Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\param[in]	renderNodes		Les noeuds instanciés.
		 *\param[in]	camera			La caméra regardant la scène.
		 *\param[in]	matrixBuffer	Le tampon de matrices.
		 */
		C3D_API uint32_t doCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
			, Camera const & camera
			, std::vector< InstantiationData > & matrixBuffer )const;
		/**
		 *\~english
		 *\brief			Copies the instanced nodes model matrices into given matrix buffer.
		 *\remarks			The nodes which are copied will be registered in the rendered nodes list.
		 *\param[in]		renderNodes		The instanced nodes.
		 *\param[in]		camera			The viewing camera.
		 *\param[in]		matrixBuffer	The matrix buffer.
		 *\param[in, out]	info			Receives the render informations.
		 *\~french
		 *\brief			Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\remarks			Les noeuds pour lesquels les matrices sont copiées seront enregistrés dans la liste des noeuds dessinés.
		 *\param[in]		renderNodes		Les noeuds instanciés.
		 *\param[in]		camera			La caméra regardant la scène.
		 *\param[in]		matrixBuffer	Le tampon de matrices.
		 *\param[in,out]	info			Reçoit les informations de rendu.
		 */
		C3D_API uint32_t doCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
			, Camera const & camera
			, std::vector< InstantiationData > & matrixBuffer
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Copies the instanced skinned nodes model matrices into given matrix buffer.
		 *\param[in]	renderNodes	The instanced nodes.
		 *\param[in]	bonesBuffer	The bones matrix buffer.
		 *\~french
		 *\brief		Copie les matrices de noeuds skinnés instanciés dans le tampon de matrices donné.
		 *\param[in]	renderNodes	Les noeuds instanciés.
		 *\param[in]	bonesBuffer	Le tampon de matrices des os.
		 */
		C3D_API uint32_t doCopyNodesBones( SkinningRenderNodeArray const & renderNodes
			, ShaderBuffer & bonesBuffer )const;
		/**
		 *\~english
		 *\brief			Copies the instanced skinned nodes model matrices into given matrix buffer.
		 *\remarks			The nodes which are copied will be registered in the rendered nodes list.
		 *\param[in]		renderNodes	The instanced nodes.
		 *\param[in]		bonesBuffer	The bones matrix buffer.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Copie les matrices de noeuds skinnés instanciés dans le tampon de matrices donné.
		 *\remarks			Les noeuds pour lesquels les matrices sont copiées seront enregistrés dans la liste des noeuds dessinés.
		 *\param[in]		renderNodes	Les noeuds instanciés.
		 *\param[in]		bonesBuffer	Le tampon de matrices des os.
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API uint32_t doCopyNodesBones( SkinningRenderNodeArray const & renderNodes
			, ShaderBuffer & bonesBuffer
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doRender( SubmeshStaticRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( SubmeshStaticRenderNodesByPipelineMap & nodes
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param[in]	camera	La caméra regardant la scène.
		 */
		C3D_API void doRender( SubmeshStaticRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	camera		The viewing camera.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	camera		La caméra regardant la scène.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( SubmeshStaticRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief			Renders instanced submeshes.
		 *\param[in]		nodes		The render nodes.
		 *\param[in]		camera		The viewing camera.
		 *\param[in]		shadowMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des sous maillages instanciés.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param[in]		camera		La caméra regardant la scène.
		 *\param[in]		shadowMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void doRender( SubmeshStaticRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 */
		C3D_API void doRender( StaticRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( StaticRenderNodesByPipelineMap & nodes
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param[in]	camera	La caméra regardant la scène.
		 */
		C3D_API void doRender( StaticRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	camera		The viewing camera.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	camera		La caméra regardant la scène.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( StaticRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief			Renders non instanced submeshes.
		 *\param[in]		nodes		The render nodes.
		 *\param[in]		camera		The viewing camera.
		 *\param[in]		shadowMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des sous maillages non instanciés.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param[in]		camera		La caméra regardant la scène.
		 *\param[in]		shadowMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void doRender( StaticRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 */
		C3D_API void doRender( SkinningRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( SkinningRenderNodesByPipelineMap & nodes
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param[in]	camera	La caméra regardant la scène.
		 */
		C3D_API void doRender( SkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	camera		The viewing camera.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	camera		La caméra regardant la scène.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( SkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief			Renders non instanced submeshes.
		 *\param[in]		nodes		The render nodes.
		 *\param[in]		camera		The viewing camera.
		 *\param[in]		shadowMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des sous maillages non instanciés.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param[in]		camera		La caméra regardant la scène.
		 *\param[in]		shadowMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void doRender( SkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doRender( SubmeshSkinningRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param[in]	camera	La caméra regardant la scène.
		 */
		C3D_API void doRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	camera		The viewing camera.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	camera		La caméra regardant la scène.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief			Renders instanced submeshes.
		 *\param[in]		nodes		The render nodes.
		 *\param[in]		camera		The viewing camera.
		 *\param[in]		shadowMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des sous maillages instanciés.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param[in]		camera		La caméra regardant la scène.
		 *\param[in]		shadowMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void doRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 */
		C3D_API void doRender( MorphingRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( MorphingRenderNodesByPipelineMap & nodes
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param[in]	camera	La caméra regardant la scène.
		 */
		C3D_API void doRender( MorphingRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	camera		The viewing camera.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	camera		La caméra regardant la scène.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( MorphingRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief			Renders non instanced submeshes.
		 *\param[in]		nodes		The render nodes.
		 *\param[in]		camera		The viewing camera.
		 *\param[in]		shadowMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des sous maillages non instanciés.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param[in]		camera		La caméra regardant la scène.
		 *\param[in]		shadowMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void doRender( MorphingRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	nodes		Les noeuds de rendu.
		 */
		C3D_API void doRender( BillboardRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( BillboardRenderNodesByPipelineMap & nodes
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	nodes	The render nodes.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param[in]	camera	La caméra regardant la scène.
		 */
		C3D_API void doRender( BillboardRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	camera		The viewing camera.
		 *\param[in]	shadowMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	camera		La caméra regardant la scène.
		 *\param[in]	shadowMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void doRender( BillboardRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps )const;
		/**
		 *\~english
		 *\brief			Renders billboards.
		 *\param[in]		nodes		The render nodes.
		 *\param[in]		camera		The viewing camera.
		 *\param[in]		shadowMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des billboards.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param[in]		camera		La caméra regardant la scène.
		 *\param[in]		shadowMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void doRender( BillboardRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps
			, RenderInfo & info )const;

	private:
		/**
		 *\~english
		 *\brief		Initialises the pass.
		 *\param		size	The pass needed dimensions.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param		size	Les dimensions voulues pour la passe.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool doInitialise( castor::Size const & size ) = 0;
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		C3D_API virtual void doCleanup() = 0;
		/**
		 *\~english
		 *\brief		Updates the specific data.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met les données spécifiques.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API virtual void doUpdate( RenderQueueArray & queues ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux indicateurs donnés.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 */
		C3D_API virtual glsl::Shader doGetVertexShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const;
		/**
		 *\~english
		 *\brief		Retrieves the geometry shader source matching the given flags.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Récupère le source du geometry shader correspondant aux indicateurs donnés.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API virtual glsl::Shader doGetGeometryShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 */
		C3D_API virtual glsl::Shader doGetLegacyPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 */
		C3D_API virtual glsl::Shader doGetPbrMRPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	passFlags		A combination of PassFlag.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 */
		C3D_API virtual glsl::Shader doGetPbrSGPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const = 0;
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	passFlags		A combination of PassFlag.
		 *\param[in,out]	textureFlags	A combination of TextureChannel.
		 *\param[in,out]	programFlags	A combination of ProgramFlag.
		 *\param[in,out]	sceneFlags		A combination of SceneFlag.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	passFlags		Une combinaison de PassFlag.
		 *\param[in,out]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in,out]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in,out]	sceneFlags		Une combinaison de SceneFlag.
		 */
		C3D_API virtual void doUpdateFlags( PassFlags & passFlags
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const = 0;
		/**
		 *\~english
		 *\brief		Updates the pipeline.
		 *\param[in]	pipeline	The render pipeline.
		 *\~french
		 *\brief		Met à jour le pipeline.
		 *\param[in]	pipeline	Le pipeline de rendu.
		 */
		C3D_API virtual void doUpdatePipeline( RenderPipeline & pipeline  )const = 0;
		/**
		 *\~english
		 *\brief			Prepares the pipeline, culling front faces.
		 *\param[in,out]	program	The program, and updates it if needed.
		 *\param[in]		flags	The pipeline flags.
		 *\~french
		 *\brief			Prépare le pipeline de rendu, en supprimant les faces avant.
		 *\param[in,out]	program	Le programme, mis à jour si besoin est.
		 *\param[in]		flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual void doPrepareFrontPipeline( renderer::ShaderProgram & program
			, PipelineFlags const & flags ) = 0;
		/**
		 *\~english
		 *\brief			Prepares the pipeline, culling back faces.
		 *\param[in,out]	program	The program, and updates it if needed.
		 *\param[in]		flags	The pipeline flags.
		 *\~french
		 *\brief			Prépare le pipeline de rendu, en supprimant les faces arrière.
		 *\param[in,out]	program	Le programme, mis à jour si besoin est.
		 *\param[in]		flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual void doPrepareBackPipeline( renderer::ShaderProgram & program
			, PipelineFlags const & flags ) = 0;

	protected:
		RenderSystem & m_renderSystem;
		RenderQueue m_renderQueue;
		bool m_opaque{ false };
		std::map< PipelineFlags, RenderPipelineUPtr > m_frontPipelines;
		std::map< PipelineFlags, RenderPipelineUPtr > m_backPipelines;
		std::set< renderer::GeometryBuffersPtr > m_geometryBuffers;
		bool m_oit{ false };
		SceneUbo m_sceneUbo;
		MatrixUbo m_matrixUbo;
		ModelUbo m_modelUbo;
		ModelMatrixUbo m_modelMatrixUbo;
		BillboardUbo m_billboardUbo;
		SkinningUbo m_skinningUbo;
		MorphingUbo m_morphingUbo;
		RenderPassTimerSPtr m_timer;
		std::vector< renderer::CommandBufferPtr > m_commandBuffers;
	};
}

#endif
