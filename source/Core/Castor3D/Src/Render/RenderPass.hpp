/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_PASS_H___
#define ___C3D_RENDER_PASS_H___

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

#include "Mesh/SubmeshComponent/InstantiationComponent.hpp"
#include "Render/RenderInfo.hpp"
#include "Render/RenderQueue.hpp"
#include "Shader/Ubos/BillboardUbo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "Shader/Ubos/ModelUbo.hpp"
#include "Shader/Ubos/MorphingUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Shader/Ubos/SkinningUbo.hpp"

#include <Ashes/Command/CommandBuffer.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>

#include <unordered_map>

namespace castor3d
{
	inline bool operator<( PipelineFlags const & lhs, PipelineFlags const & rhs )
	{
		return lhs.alphaFunc < rhs.alphaFunc
			|| ( lhs.alphaFunc == rhs.alphaFunc
				&& ( lhs.topology < rhs.topology
					||	( lhs.topology == rhs.topology
						&&	( lhs.colourBlendMode < rhs.colourBlendMode
							||	( lhs.colourBlendMode == rhs.colourBlendMode
								&&	( lhs.alphaBlendMode < rhs.alphaBlendMode
									||	( lhs.alphaBlendMode == rhs.alphaBlendMode
										&&	( lhs.textureFlags < rhs.textureFlags
											||	( lhs.textureFlags == rhs.textureFlags
												&&	( lhs.programFlags < rhs.programFlags
													||	( lhs.programFlags == rhs.programFlags
														&&	( lhs.passFlags < rhs.passFlags
															|| ( lhs.passFlags == rhs.passFlags
																&& lhs.sceneFlags < rhs.sceneFlags )
															)
														)
													)
												)
											)
										)
									)
								)
							)
						)
					)
				);
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
		 *\param[in]	category	The pass category.
		 *\param[in]	name		The pass name.
		 *\param[in]	engine		The engine.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The scene culler for this pass.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	category	La catégorie de la passe.
		 *\param[in]	name		Le nom de la passe.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	matrixUbo	L'UBO des matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 */
		C3D_API RenderPass( castor::String const & category
			, castor::String const & name
			, Engine & engine
			, MatrixUbo const & matrixUbo
			, SceneCuller & culler );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	category	The pass category.
		 *\param[in]	name		The pass name.
		 *\param[in]	engine		The engine.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The scene culler for this pass.
		 *\param[in]	oit			The order independant status.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	category	La catégorie de la passe.
		 *\param[in]	name		Le nom de la passe.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	matrixUbo	L'UBO des matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	oit			Le statut de rendu indépendant de l'ordre des objets.
		 */
		C3D_API RenderPass( castor::String const & category
			, castor::String const & name
			, Engine & engine
			, MatrixUbo const & matrixUbo
			, SceneCuller & culler
			, bool oit );
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	category	The pass category.
		 *\param[in]	name		The pass name.
		 *\param[in]	engine		The engine.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The scene culler for this pass.
		 *\param[in]	ignored		The geometries attached to this node will be ignored in the render.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	category	La catégorie de la passe.
		 *\param[in]	name		Le nom de la passe.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	matrixUbo	L'UBO des matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 */
		C3D_API RenderPass( castor::String const & category
			, castor::String const & name
			, Engine & engine
			, MatrixUbo const & matrixUbo
			, SceneCuller & culler
			, SceneNode const * ignored );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	category	The pass category.
		 *\param[in]	name		The pass name.
		 *\param[in]	engine		The engine.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The scene culler for this pass.
		 *\param[in]	oit			The order independant status.
		 *\param[in]	ignored		The geometries attached to this node will be ignored in the render.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	category	La catégorie de la passe.
		 *\param[in]	name		Le nom de la passe.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	matrixUbo	L'UBO des matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	oit			Le statut de rendu indépendant de l'ordre des objets.
		 *\param[in]	ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 */
		C3D_API RenderPass( castor::String const & category
			, castor::String const & name
			, Engine & engine
			, MatrixUbo const & matrixUbo
			, SceneCuller & culler
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
		C3D_API ShaderPtr getVertexShaderSource( PassFlags const & passFlags
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
		C3D_API ShaderPtr getPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const;
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
		C3D_API ShaderPtr getGeometryShaderSource( PassFlags const & passFlags
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
		 *\param[in]	topology		The render topology.
		 *\param[in]	twoSided		Tells if the pass is two sided.
		 *\param[in]	layouts			The vertex buffers layouts.
		 *\~french
		 *\brief		Prépare le pipeline qui correspond aux indicateurs donnés.
		 *\param[in]	colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	topology		La topologie de rendu.
		 *\param[in]	twoSided		Dit si la passe est sur les deux faces.
		 *\param[in]	layouts			Les layouts des tampons de sommets.
		 */
		C3D_API void preparePipeline( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, ashes::CompareOp alphaFunc
			, PassFlags & passFlags
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags
			, ashes::PrimitiveTopology topology
			, bool twoSided
			, ashes::VertexLayoutCRefArray const & layouts );
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
		 *\param[in]	topology		The render topology.
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
		 *\param[in]	topology		La topologie de rendu.
		 *\return		Le pipeline prêt à l'utilisation, \p nullptr si aucun pipeline n'est disponible pour la configuration voulue.
		 */
		C3D_API RenderPipeline * getPipelineFront( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, ashes::CompareOp alphaFunc
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::PrimitiveTopology topology )const;
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
		 *\param[in]	topology		The render topology.
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
		 *\param[in]	topology		La topologie de rendu.
		 *\return		Le pipeline prêt à l'utilisation, \p nullptr si aucun pipeline n'est disponible pour la configuration voulue.
		 */
		C3D_API RenderPipeline * getPipelineBack( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, ashes::CompareOp alphaFunc
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::PrimitiveTopology topology )const;
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
		 *\brief		Initialises the UBO descriptor set of a billboard node.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	node			The node.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs d'UBO pour un noeud de billboard.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	node			Le noeud.
		 */
		C3D_API void initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, BillboardRenderNode & node );
		/**
		 *\~english
		 *\brief		Initialises the UBO descriptor set of a morphing node.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	node			The node.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs d'UBO pour un noeud de morphing.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	node			Le noeud.
		 */
		C3D_API void initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, MorphingRenderNode & node );
		/**
		 *\~english
		 *\brief		Initialises the UBO descriptor set of a skinning node.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	node			The node.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs d'UBO pour un noeud de skinning.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	node			Le noeud.
		 */
		C3D_API void initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, SkinningRenderNode & node );
		/**
		 *\~english
		 *\brief		Initialises the UBO descriptor set of a static node.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	node			The node.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs d'UBO pour un noeud statique.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	node			Le noeud.
		 */
		C3D_API void initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, StaticRenderNode & node );
		/**
		 *\~english
		 *\brief		Initialises the UBO descriptor set of skining nodes.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	nodes			The nodes.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs d'UBO pour des noeuds de skining.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	nodes			Les noeuds.
		 */
		C3D_API void initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, SubmeshSkinninRenderNodesByPassMap & nodes );
		/**
		 *\~english
		 *\brief		Initialises the UBO descriptor set of static nodes.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	nodes			The nodes.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs d'UBO pour des noeuds statiques.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	nodes			Les noeuds.
		 */
		C3D_API void initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, SubmeshStaticRenderNodesByPassMap & nodes );
		/**
		 *\~english
		 *\brief		Initialises the textures descriptor set of a billboard node.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	node			The node.
		 *\param[in]	shadowMaps		The shadow maps.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs de textures pour un noeud de billboard.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	node			Le noeud.
		 *\param[in]	shadowMaps		Les shadow maps.
		 */
		C3D_API void initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, BillboardRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps );
		/**
		 *\~english
		 *\brief		Initialises the textures descriptor set of a morphing node.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	node			The node.
		 *\param[in]	shadowMaps		The shadow maps.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs de textures pour un noeud de morphing.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	node			Le noeud.
		 *\param[in]	shadowMaps		Les shadow maps.
		 */
		C3D_API void initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, MorphingRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps );
		/**
		 *\~english
		 *\brief		Initialises the textures descriptor set of a skinning node.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	node			The node.
		 *\param[in]	shadowMaps		The shadow maps.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs de textures pour un noeud de skinning.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	node			Les noeud.
		 *\param[in]	shadowMaps		Les shadow maps.
		 */
		C3D_API void initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, SkinningRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps );
		/**
		 *\~english
		 *\brief		Initialises the textures descriptor set of a static node.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	node			The node.
		 *\param[in]	shadowMaps		The shadow maps.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs de textures pour un noeud statique.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	node			Le noeud.
		 *\param[in]	shadowMaps		Les shadow maps.
		 */
		C3D_API void initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, StaticRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps );
		/**
		 *\~english
		 *\brief		Initialises the textures descriptor set of skinning nodes.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	nodes			The nodes.
		 *\param[in]	shadowMaps		The shadow maps.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs de textures pour des noeuds de skinning.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	nodes			Les noeuds.
		 *\param[in]	shadowMaps		Les shadow maps.
		 */
		C3D_API void initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, SubmeshSkinninRenderNodesByPassMap & nodes
			, ShadowMapLightTypeArray const & shadowMaps );
		/**
		 *\~english
		 *\brief		Initialises the textures descriptor set of static nodes.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	nodes			The nodes.
		 *\param[in]	shadowMaps		The shadow maps.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs de textures pour des noeuds statiques.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	nodes			Les noeuds.
		 *\param[in]	shadowMaps		Les shadow maps.
		 */
		C3D_API void initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
			, SubmeshStaticRenderNodesByPassMap & nodes
			, ShadowMapLightTypeArray const & shadowMaps );
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
		 *\brief		Creates a blend state matching given blend modes.
		 *\param[in]	colourBlendMode	The colour blend mode.
		 *\param[in]	alphaBlendMode	The alpha blend mode.
		 *\param[in]	attachesCount	The wanted blend attaches count.
		 *\return		
		 *\~french
		 *\brief		Crée un état de mélange correspondant aux modes de mélange donnés.
		 *\param[in]	colourBlendMode	Le mode de mélange couleurs.
		 *\param[in]	alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]	attachesCount	Le nombre d'attaches de mélange voulues.
		 *\return		
		 */
		C3D_API static ashes::ColourBlendState createBlendState( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, uint32_t attachesCount );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline bool isOrderIndependent()const
		{
			return m_oit;
		}

		inline SceneCuller const & getCuller()const
		{
			return m_culler;
		}

		inline SceneCuller & getCuller()
		{
			return m_culler;
		}

		inline SceneUbo & getSceneUbo()
		{
			return m_sceneUbo;
		}

		inline SceneUbo const & getSceneUbo()const
		{
			return m_sceneUbo;
		}

		inline MatrixUbo const & getMatrixUbo()const
		{
			return m_matrixUbo;
		}

		inline ashes::CommandBuffer const & getCommandBuffer()const
		{
			return m_renderQueue.getCommandBuffer();
		}

		inline bool hasNodes()const
		{
			return m_renderQueue.hasNodes();
		}

		inline ashes::RenderPass const & getRenderPass()const
		{
			return *m_renderPass;
		}

		inline RenderPassTimer const & getTimer()const
		{
			return *m_timer;
		}

		inline RenderPassTimer & getTimer()
		{
			return *m_timer;
		}
		/**@}*/

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
		C3D_API ShaderProgramSPtr doGetProgram( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc
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
		C3D_API uint32_t doCopyNodesMatrices( StaticRenderNodePtrArray const & renderNodes
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
		C3D_API uint32_t doCopyNodesMatrices( StaticRenderNodePtrArray const & renderNodes
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
		C3D_API uint32_t doCopyNodesMatrices( SkinningRenderNodePtrArray const & renderNodes
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
		C3D_API uint32_t doCopyNodesMatrices( SkinningRenderNodePtrArray const & renderNodes
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
		C3D_API uint32_t doCopyNodesBones( SkinningRenderNodePtrArray const & renderNodes
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
		C3D_API uint32_t doCopyNodesBones( SkinningRenderNodePtrArray const & renderNodes
			, ShaderBuffer & bonesBuffer
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Updates instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief			Updates instanced submeshes.
		 *\param[in]		nodes	The render nodes.
		 *\param[in, out]	info	Receives the render informations.
		 *\~french
		 *\brief			Met à jour des sous maillages instanciés.
		 *\param[in]		nodes	Les noeuds de rendu.
		 *\param[in,out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Updates non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( StaticRenderNodesPtrByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief			Updates non instanced submeshes.
		 *\param[in]		nodes	The render nodes.
		 *\param[in, out]	info	Receives the render informations.
		 *\~french
		 *\brief			Met à jour des sous maillages non instanciés.
		 *\param[in]		nodes	Les noeuds de rendu.
		 *\param[in,out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void doUpdate( StaticRenderNodesPtrByPipelineMap & nodes
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Updates non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief			Updates non instanced submeshes.
		 *\param[in]		nodes	The render nodes.
		 *\param[in, out]	info	Receives the render informations.
		 *\~french
		 *\brief			Met à jour des sous maillages non instanciés.
		 *\param[in]		nodes	Les noeuds de rendu.
		 *\param[in,out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Updates instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief			Updates instanced submeshes.
		 *\param[in]		nodes	The render nodes.
		 *\param[in, out]	info	Receives the render informations.
		 *\~french
		 *\brief			Met à jour des sous maillages instanciés.
		 *\param[in]		nodes	Les noeuds de rendu.
		 *\param[in,out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Updates non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief			Updates non instanced submeshes.
		 *\param[in]		nodes	The render nodes.
		 *\param[in, out]	info	Receives the render informations.
		 *\~french
		 *\brief			Met à jour des sous maillages non instanciés.
		 *\param[in]		nodes	Les noeuds de rendu.
		 *\param[in,out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Updates billboards.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des billboards.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief			Updates billboards.
		 *\param[in]		nodes		The render nodes.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Met à jour des billboards.
		 *\param[in]		nodes	Les noeuds de rendu.
		 *\param[in,out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Uploads UBO pools to VRAM.
		 *\param[in]	camera	The camera.
		 *\param[in]	jitter	The jitteering data.
		 *\~french
		 *\brief		Met à jour les pools d'UBO en VRAM.
		 *\param[in]	camera	La caméra.
		 *\param[in]	jitter	Les données de jittering.
		 */
		void doUpdateUbos( Camera const & camera
			, castor::Point2r const & jitter );
		/**
		*\~english
		*\return
		*	The front culled pipelines list.
		*\~french
		*\return
		*	La liste des pipelines front culled.
		*/
		C3D_API std::map< PipelineFlags, RenderPipelineUPtr > & doGetFrontPipelines();
		/**
		*\~english
		*\return
		*	The back culled pipelines list.
		*\~french
		*\return
		*	La liste des pipelines back culled.
		*/
		C3D_API std::map< PipelineFlags, RenderPipelineUPtr > & doGetBackPipelines();
		/**
		*\~english
		*\return
		*	The front culled pipelines list.
		*\~french
		*\return
		*	La liste des pipelines front culled.
		*/
		C3D_API std::map< PipelineFlags, RenderPipelineUPtr > const & doGetFrontPipelines()const;
		/**
		*\~english
		*\return
		*	The back culled pipelines list.
		*\~french
		*\return
		*	La liste des pipelines back culled.
		*/
		C3D_API std::map< PipelineFlags, RenderPipelineUPtr > const & doGetBackPipelines()const;
		/**
		 *\~english
		 *\brief		Prepares the pipeline, culling front faces.
		 *\param[in]	program	The program.
		 *\param[in]	layouts	The vertex buffers layouts.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Prépare le pipeline de rendu, en supprimant les faces avant.
		 *\param[in]	program	Le programme.
		 *\param[in]	layouts	Les layouts des tampons de sommets.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual void doPrepareFrontPipeline( ShaderProgramSPtr program
			, ashes::VertexLayoutCRefArray const & layouts
			, PipelineFlags const & flags );
		/**
		 *\~english
		 *\brief		Prepares the pipeline, culling back faces.
		 *\param[in]	program	The program.
		 *\param[in]	layouts	The vertex buffers layouts.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Prépare le pipeline de rendu, en supprimant les faces arrière.
		 *\param[in]	program	Le programme.
		 *\param[in]	layouts	Les layouts des tampons de sommets.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual void doPrepareBackPipeline( ShaderProgramSPtr program
			, ashes::VertexLayoutCRefArray const & layouts
			, PipelineFlags const & flags );
		/**
		 *\~english
		 *\brief		Creates the common UBO descriptor layout bindings.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Crée les attaches de layout de descripteurs communs pour les UBO.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ashes::DescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Creates the common textures descriptor layout bindings.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Crée les attaches de layout de descripteurs communs pour les textures.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ashes::DescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Creates the depth stencil state.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Crée l'attache de profondeur et stencil.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ashes::DepthStencilState doCreateDepthStencilState( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Creates the colour blend state.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Crée l'attache de mélange des couleurs.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ashes::ColourBlendState doCreateBlendState( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		C3D_API virtual void doCleanup() = 0;

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
		 *\brief		Initialises the descriptor set of a billboard node.
		 *\param[in]	layout	The descriptors layout.
		 *\param[in]	node	The node.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs pour un noeud de billboard.
		 *\param[in]	layout	Le layout des descripteurs.
		 *\param[in]	node	Le noeud.
		 */
		C3D_API virtual void doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
			, BillboardListRenderNode & node ) = 0;
		/**
		 *\~english
		 *\brief		Initialises the descriptor set of a morphing node.
		 *\param[in]	layout	The descriptors layout.
		 *\param[in]	node	The node.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs pour un noeud de morphing.
		 *\param[in]	layout	Le layout des descripteurs.
		 *\param[in]	node	Le noeud.
		 */
		C3D_API virtual void doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
			, SubmeshRenderNode & node ) = 0;
		/**
		 *\~english
		 *\brief			Initialises the descriptor set of a billboard node.
		 *\param[in]		layout		The descriptors layout.
		 *\param[in,out]	index		The texture index, updated to the next available.
		 *\param[in]		node		The node.
		 *\param[in]		shadowMaps	The shadow maps.
		 *\~french
		 *\brief			Initialise l'ensemble de descripteurs pour un noeud de billboard.
		 *\param[in]		layout		Le layout des descripteurs.
		 *\param[in, out]	index		L'indice de la texture, mis à jour au prochain disponible.
		 *\param[in]		node		Le noeud.
		 *\param[in]		shadowMaps	Les shadow maps.
		 */
		C3D_API virtual void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps ) = 0;
		/**
		 *\~english
		 *\brief			Initialises the descriptor set of a morphing node.
		 *\param[in]		layout		The descriptors layout.
		 *\param[in,out]	index		The texture index, updated to the next available.
		 *\param[in]		node		The node.
		 *\param[in]		shadowMaps	The shadow maps.
		 *\~french
		 *\brief			Initialise l'ensemble de descripteurs pour un noeud de morphing.
		 *\param[in]		layout		Le layout des descripteurs.
		 *\param[in,out]	index		L'indice de la texture, mis à jour au prochain disponible.
		 *\param[in]		node		Le noeud.
		 *\param[in]		shadowMaps	Les shadow maps.
		 */
		C3D_API virtual void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps ) = 0;
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
		C3D_API virtual ShaderPtr doGetVertexShaderSource( PassFlags const & passFlags
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
		C3D_API virtual ShaderPtr doGetGeometryShaderSource( PassFlags const & passFlags
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
		C3D_API virtual ShaderPtr doGetLegacyPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const = 0;
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
		C3D_API virtual ShaderPtr doGetPbrMRPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const = 0;
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
		C3D_API virtual ShaderPtr doGetPbrSGPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const = 0;
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
		C3D_API virtual void doUpdatePipeline( RenderPipeline & pipeline )const = 0;

	public:
		struct VertexInputs
		{
			// Classic inputs
			static uint32_t constexpr PositionLocation = 0u;
			static uint32_t constexpr NormalLocation = 1u;
			static uint32_t constexpr TangentLocation = 2u;
			static uint32_t constexpr TextureLocation = 3u;
			// Skinning inputs
			static uint32_t constexpr BoneIds0Location = 4u;
			static uint32_t constexpr BoneIds1Location = 5u;
			static uint32_t constexpr Weights0Location = 6u;
			static uint32_t constexpr Weights1Location = 7u;
			// Morphing inputs
			static uint32_t constexpr Position2Location = 8u;
			static uint32_t constexpr Normal2Location = 9u;
			static uint32_t constexpr Tangent2Location = 10u;
			static uint32_t constexpr Texture2Location = 11u;
			// Instantiation inputs (overlaps morphing inputs)
			static uint32_t constexpr TransformLocation = 8u; // 4 components since it is a matrix
			static uint32_t constexpr MaterialLocation = 12u;
		};
		struct VertexOutputs
		{
			static uint32_t constexpr WorldPositionLocation = 0u;
			static uint32_t constexpr ViewPositionLocation = 1u;
			static uint32_t constexpr CurPositionLocation = 2u;
			static uint32_t constexpr PrvPositionLocation = 3u;
			static uint32_t constexpr TangentSpaceFragPositionLocation = 4u;
			static uint32_t constexpr TangentSpaceViewPositionLocation = 5u;
			static uint32_t constexpr NormalLocation = 6u;
			static uint32_t constexpr TangentLocation = 7u;
			static uint32_t constexpr BitangentLocation = 8u;
			static uint32_t constexpr TextureLocation = 9u;
			static uint32_t constexpr InstanceLocation = 10u;
			static uint32_t constexpr MaterialLocation = 11u;
		};

	protected:
		RenderSystem & m_renderSystem;
		MatrixUbo const & m_matrixUbo;
		SceneCuller & m_culler;
		castor::String m_category;
		RenderQueue m_renderQueue;
		bool m_opaque{ false };
		bool m_oit{ false };
		SceneUbo m_sceneUbo;
		ashes::RenderPassPtr m_renderPass;
		RenderPassTimerSPtr m_timer;
		castor::Size m_size;

	private:
		std::map< PipelineFlags, RenderPipelineUPtr > m_frontPipelines;
		std::map< PipelineFlags, RenderPipelineUPtr > m_backPipelines;
	};
}

#endif
