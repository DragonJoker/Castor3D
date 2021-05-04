/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPass_H___
#define ___C3D_RenderPass_H___

#include "RenderModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <unordered_map>

namespace castor3d
{
	class SceneRenderPass
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
		using DistanceSortedNodeMap = std::multimap< double, std::unique_ptr< DistanceRenderNodeBase > >;

	protected:
		C3D_API SceneRenderPass( RenderDevice const & device
			, castor::String const & category
			, castor::String const & name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, RenderMode mode
			, bool oit
			, bool forceTwoSided
			, SceneNode const * ignored
			, uint32_t instanceMult );

	protected:
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	category		The pass category.
		 *\param[in]	name			The pass name.
		 *\param[in]	engine			The engine.
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The scene culler for this pass.
		 *\param[in]	instanceMult	The object instance multiplier.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	category		La catégorie de la passe.
		 *\param[in]	name			Le nom de la passe.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	matrixUbo		L'UBO des matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	instanceMult	Le multiplicateur d'instances d'objets.
		 */
		C3D_API SceneRenderPass( RenderDevice const & device
			, castor::String const & category
			, castor::String const & name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, uint32_t instanceMult = 1u );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	category		The pass category.
		 *\param[in]	name			The pass name.
		 *\param[in]	engine			The engine.
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The scene culler for this pass.
		 *\param[in]	oit				The order independant status.
		 *\param[in]	instanceMult	The object instance multiplier.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	category		La catégorie de la passe.
		 *\param[in]	name			Le nom de la passe.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	matrixUbo		L'UBO des matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	oit				Le statut de rendu indépendant de l'ordre des objets.
		 *\param[in]	instanceMult	Le multiplicateur d'instances d'objets.
		 */
		C3D_API SceneRenderPass( RenderDevice const & device
			, castor::String const & category
			, castor::String const & name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, bool oit
			, uint32_t instanceMult = 1u );
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	category		The pass category.
		 *\param[in]	name			The pass name.
		 *\param[in]	engine			The engine.
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The scene culler for this pass.
		 *\param[in]	ignored			The geometries attached to this node will be ignored in the render.
		 *\param[in]	instanceMult	The object instance multiplier.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	category		La catégorie de la passe.
		 *\param[in]	name			Le nom de la passe.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	matrixUbo		L'UBO des matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	ignored			Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	instanceMult	Le multiplicateur d'instances d'objets.
		 */
		C3D_API SceneRenderPass( RenderDevice const & device
			, castor::String const & category
			, castor::String const & name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, SceneNode const * ignored
			, uint32_t instanceMult = 1u );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	category		The pass category.
		 *\param[in]	name			The pass name.
		 *\param[in]	engine			The engine.
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The scene culler for this pass.
		 *\param[in]	oit				The order independant status.
		 *\param[in]	ignored			The geometries attached to this node will be ignored in the render.
		 *\param[in]	instanceMult	The object instance multiplier.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	category		La catégorie de la passe.
		 *\param[in]	name			Le nom de la passe.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	matrixUbo		L'UBO des matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	oit				Le statut de rendu indépendant de l'ordre des objets.
		 *\param[in]	ignored			Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	instanceMult	Le multiplicateur d'instances d'objets.
		 */
		C3D_API SceneRenderPass( RenderDevice const & device
			, castor::String const & category
			, castor::String const & name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, bool oit
			, SceneNode const * ignored
			, uint32_t instanceMult = 1u );

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~SceneRenderPass() = default;
		/**
		 *\~english
		 *\brief		Initialises the pass.
		 *\param		device	The current device.
		 *\param		size	The pass needed dimensions.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param		device	Le device actuel.
		 *\param		size	Les dimensions voulues pour la passe.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( castor::Size const & size );
		/**
		 *\~english
		 *\brief		Initialises the pass.
		 *\param		device	The current device.
		 *\param		size	The pass needed dimensions.
		 *\param		timer	The parent timer.
		 *\param		index	The pass timer index, in its parent.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param		device	Le device actuel.
		 *\param		size	Les dimensions voulues pour la passe.
		 *\param		timer	Le timer parent.
		 *\param		index	L'indice de la passe, dans le parent.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( castor::Size const & size
			, RenderPassTimer & timer
			, uint32_t index );
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\param		device	The current device.
		 *\~french
		 *\brief		Nettoie la passe.
		 *\param		device	Le device actuel.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API ShaderPtr getVertexShaderSource( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API ShaderPtr getPixelShaderSource( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Retrieves the geometry shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du geometry shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API ShaderPtr getGeometryShaderSource( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief			Prepares the pipeline matching the given flags, for back face culling nodes.
		 *\param[in]		colourBlendMode	The colour blend mode.
		 *\param[in]		alphaBlendMode	The alpha blend mode.
		 *\param[in]		alphaFunc		The alpha test function.
		 *\param[in,out]	passFlags		A combination of PassFlag.
		 *\param[in,out]	textures		The textures configuration.
		 *\param[in,out]	heightMapIndex	The height map index.
		 *\param[in,out]	programFlags	A combination of ProgramFlag.
		 *\param[in,out]	sceneFlags		Scene related flags.
		 *\param[in]		topology		The render topology.
		 *\param[in]		layouts			The vertex buffers layouts.
		 *\~french
		 *\brief			Prépare le pipeline qui correspond aux indicateurs donnés, pour les noeuds en back face culling.
		 *\param[in]		colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]		alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]		alphaFunc		La fonction de test alpha.
		 *\param[in,out]	passFlags		Une combinaison de PassFlag.
		 *\param[in,out]	textures		La configuration des textures.
		 *\param[in,out]	heightMapIndex	L'indice de la texture contenant de hauteur.
		 *\param[in,out]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in,out]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]		topology		La topologie de rendu.
		 *\param[in]		layouts			Les layouts des tampons de sommets.
		 */
		C3D_API PipelineFlags prepareBackPipeline( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, VkCompareOp alphaFunc
			, VkCompareOp blendAlphaFunc
			, PassFlags const & passFlags
			, TextureFlagsArray const & textures
			, uint32_t heightMapIndex
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts );
		/**
		 *\~english
		 *\brief			Prepares the pipeline matching the given flags, for back face culling nodes.
		 *\param[in,out]	flags	The pipeline flags.
		 *\param[in]		layouts	The vertex buffers layouts.
		 *\~french
		 *\brief			Prépare le pipeline qui correspond aux indicateurs donnés, pour les noeuds en back face culling.
		 *\param[in,out]	flags	Les indicateurs de pipeline.
		 *\param[in]		layouts	Les layouts des tampons de sommets.
		 */
		C3D_API void prepareBackPipeline( PipelineFlags & flags
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts );
		/**
		 *\~english
		 *\brief			Prepares the pipeline matching the given flags, for front face culling nodes.
		 *\param[in]		colourBlendMode	The colour blend mode.
		 *\param[in]		alphaBlendMode	The alpha blend mode.
		 *\param[in]		alphaFunc		The alpha test function.
		 *\param[in,out]	passFlags		A combination of PassFlag.
		 *\param[in,out]	textures		The textures configuration.
		 *\param[in,out]	heightMapIndex	The height map index.
		 *\param[in,out]	programFlags	A combination of ProgramFlag.
		 *\param[in,out]	sceneFlags		Scene related flags.
		 *\param[in]		topology		The render topology.
		 *\param[in]		layouts			The vertex buffers layouts.
		 *\~french
		 *\brief			Prépare le pipeline qui correspond aux indicateurs donnés, pour les noeuds en front face culling.
		 *\param[in]		colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]		alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]		alphaFunc		La fonction de test alpha.
		 *\param[in,out]	passFlags		Une combinaison de PassFlag.
		 *\param[in,out]	textures		La configuration des textures.
		 *\param[in,out]	heightMapIndex	L'indice de la texture contenant de hauteur.
		 *\param[in,out]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in,out]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]		topology		La topologie de rendu.
		 *\param[in]		layouts			Les layouts des tampons de sommets.
		 */
		C3D_API PipelineFlags prepareFrontPipeline( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, VkCompareOp alphaFunc
			, VkCompareOp blendAlphaFunc
			, PassFlags const & passFlags
			, TextureFlagsArray const & textures
			, uint32_t heightMapIndex
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts );
		/**
		 *\~english
		 *\brief			Prepares the pipeline matching the given flags, for front face culling nodes.
		 *\param[in,out]	flags	The pipeline flags.
		 *\param[in]		layouts	The vertex buffers layouts.
		 *\~french
		 *\brief			Prépare le pipeline qui correspond aux indicateurs donnés, pour les noeuds en front face culling.
		 *\param[in,out]	flags	Les indicateurs de pipeline.
		 *\param[in]		layouts	Les layouts des tampons de sommets.
		 */
		C3D_API void prepareFrontPipeline( PipelineFlags & flags
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts );
		/**
		 *\~english
		 *\brief		Retrieves the pipeline matching the given flags, for front face culling.
		 *\param[in]	flags	The pipeline flags.
		 *\return		The ready to use pipeline, \p nullptr if none is available for wanted configuration.
		 *\~french
		 *\brief		Récupère le pipeline qui correspond aux indicateurs donnés, pour les faces avant supprimées.
		 *\param[in]	flags	Les indicateurs du pipeline.
		 *\return		Le pipeline prêt à l'utilisation, \p nullptr si aucun pipeline n'est disponible pour la configuration voulue.
		 */
		C3D_API RenderPipeline * getPipelineFront( PipelineFlags flags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pipeline matching the given flags, for back face culling.
		 *\param[in]	flags	The pipeline flags.
		 *\return		The ready to use pipeline, \p nullptr if none is available for wanted configuration.
		 *\~french
		 *\brief		Récupère le pipeline qui correspond aux indicateurs donnés, pour les faces arrière supprimées.
		 *\param[in]	flags	Les indicateurs du pipeline.
		 *\return		Le pipeline prêt à l'utilisation, \p nullptr si aucun pipeline n'est disponible pour la configuration voulue.
		 */
		C3D_API RenderPipeline * getPipelineBack( PipelineFlags flags )const;
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
		C3D_API void updatePipeline( RenderPipeline & pipeline );
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
		C3D_API void initialiseUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
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
		C3D_API void initialiseUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
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
		C3D_API void initialiseUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
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
		C3D_API void initialiseUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
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
		C3D_API void initialiseUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
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
		C3D_API void initialiseUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
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
		C3D_API void initialiseTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
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
		C3D_API void initialiseTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
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
		C3D_API void initialiseTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
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
		C3D_API void initialiseTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
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
		C3D_API void initialiseTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
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
		C3D_API void initialiseTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
			, SubmeshStaticRenderNodesByPassMap & nodes
			, ShadowMapLightTypeArray const & shadowMaps );
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	flags	The pipeline flags.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	flags	Les indicateurs de pipeline.
		 */
		C3D_API void updateFlags( PipelineFlags & flags )const;
		/**
		 *\~english
		 *\brief		Filters the given textures flags using this pass needed textures.
		 *\param[in]	textures	The textures flags.
		 *\return		The filtered flags.
		 *\~french
		 *\brief		Filtre les indicateurs de textures donnés en utilisant ceux voulus par cette passe.
		 *\param[in]	textures	Les indicateurs de textures.
		 *\return		Les indicateurs filtrés.
		 */
		C3D_API FilteredTextureFlags filterTexturesFlags( TextureFlagsArray const & textures )const;
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
		C3D_API static ashes::PipelineColorBlendStateCreateInfo createBlendState( BlendMode colourBlendMode
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
		C3D_API virtual TextureFlags getTexturesMask()const;

		C3D_API virtual ShaderFlags getShaderFlags()const
		{
			return ShaderFlag::eWorldSpace
				| ShaderFlag::eTangentSpace;
		}

		bool isOrderIndependent()const
		{
			return m_oit;
		}

		SceneCuller const & getCuller()const
		{
			return m_culler;
		}

		SceneCuller & getCuller()
		{
			return m_culler;
		}

		SceneUbo & getSceneUbo()
		{
			return m_sceneUbo;
		}

		SceneUbo const & getSceneUbo()const
		{
			return m_sceneUbo;
		}

		MatrixUbo & getMatrixUbo()const
		{
			return m_matrixUbo;
		}

		ashes::CommandBuffer const & getCommandBuffer()const
		{
			return m_renderQueue.getCommandBuffer();
		}

		bool hasNodes()const
		{
			return m_renderQueue.hasNodes();
		}

		ashes::RenderPass const & getRenderPass()const
		{
			return *m_renderPass;
		}

		RenderPassTimer const & getTimer()const
		{
			return *m_timer;
		}

		RenderPassTimer & getTimer()
		{
			return *m_timer;
		}

		uint32_t getPipelinesCount()const
		{
			return uint32_t( m_backPipelines.size()
				+ m_frontPipelines.size() );
		}

		uint32_t getInstanceMult()const
		{
			return m_instanceMult;
		}

		bool isDirty()const
		{
			return m_isDirty;
		}

		bool forceTwoSided()const
		{
			return m_forceTwoSided;
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
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le programme shader correspondant aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API ShaderProgramSPtr doGetProgram( PipelineFlags const & flags )const;
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
		C3D_API void doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes );
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
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Updates non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( StaticRenderNodesPtrByPipelineMap & nodes );
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
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Updates non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes );
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
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Updates instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes );
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
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Updates non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes );
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
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Updates billboards.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des billboards.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes );
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
			, RenderInfo & info );
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
		C3D_API virtual void doUpdate( RenderQueueArray & queues );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API virtual void doUpdateUbos( CpuUpdater & updater );
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
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts
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
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts
			, PipelineFlags const & flags );
		/**
		 *\~english
		 *\brief		Creates the common UBO descriptor layout bindings.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Crée les attaches de layout de descripteurs communs pour les UBO.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ashes::VkDescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Creates the common textures descriptor layout bindings.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Crée les attaches de layout de descripteurs communs pour les textures.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Creates the depth stencil state.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Crée l'attache de profondeur et stencil.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Creates the colour blend state.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Crée l'attache de mélange des couleurs.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		C3D_API virtual void doCleanup() = 0;
		/**
		 *\~english
		 *\brief		Initialises the pass.
		 *\param[in]	device	The GPU device.
		 *\param		size	The pass needed dimensions.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param[in]	device	Le device GPU.
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
		C3D_API virtual void doFillUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
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
		C3D_API virtual void doFillUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
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
		C3D_API virtual void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
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
		C3D_API virtual void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps ) = 0;

	private:
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	flags	The pipeline flags.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual void doUpdateFlags( PipelineFlags & flags )const = 0;
		/**
		 *\~english
		 *\brief		Updates the pipeline.
		 *\param[in]	pipeline	The render pipeline.
		 *\~french
		 *\brief		Met à jour le pipeline.
		 *\param[in]	pipeline	Le pipeline de rendu.
		 */
		C3D_API virtual void doUpdatePipeline( RenderPipeline & pipeline ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Retrieves the billboards' vertex shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du vertex shader de billboards qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetBillboardShaderSource( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Retrieves the geometry shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du geometry shader correspondant aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const = 0;

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
		RenderDevice const & m_device;
		RenderSystem & m_renderSystem;
		MatrixUbo & m_matrixUbo;
		SceneCuller & m_culler;
		RenderQueue m_renderQueue;
		castor::String m_category;
		RenderMode m_mode{ RenderMode::eBoth };
		bool m_oit{ false };
		bool m_forceTwoSided{ false };
		bool m_isDirty{ true };
		SceneUbo m_sceneUbo;
		ashes::RenderPassPtr m_renderPass;
		RenderPassTimer * m_timer{ nullptr };
		uint32_t m_index{ 0u };
		RenderPassTimerSPtr m_ownTimer;
		castor::Size m_size;
		uint32_t const m_instanceMult{ 1u };
		std::map< size_t, UniformBufferOffsetT< ModelInstancesUboConfiguration > > m_modelsInstances;

	private:
		std::map< PipelineFlags, RenderPipelineUPtr > m_frontPipelines;
		std::map< PipelineFlags, RenderPipelineUPtr > m_backPipelines;
	};
}

#endif
