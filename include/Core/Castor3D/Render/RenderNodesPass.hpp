/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPass_H___
#define ___C3D_RenderPass_H___

#include "RenderModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/Clustered/ClusteredModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <RenderGraph/RunnablePasses/RenderPass.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <optional>
#include <unordered_map>

namespace castor3d
{
	struct RenderNodesPassDesc
	{
	private:
		RenderNodesPassDesc( VkExtent3D size
			, CameraUbo const & cameraUbo
			, SceneUbo const * sceneUbo
			, SceneCuller & culler
			, RenderFilters filters
			, bool oit
			, bool forceTwoSided )
			: m_size{ castor::move( size ) }
			, m_cameraUbo{ cameraUbo }
			, m_sceneUbo{ sceneUbo }
			, m_culler{ culler }
			, m_filters{ filters }
			, m_oit{ oit }
			, m_forceTwoSided{ forceTwoSided }
		{
		}

	public:
		RenderNodesPassDesc( VkExtent3D size
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, SceneCuller & culler
			, RenderFilters filters
			, bool oit
			, bool forceTwoSided )
			: RenderNodesPassDesc{ castor::move( size )
				, cameraUbo
				, & sceneUbo
				, culler
				, filters
				, oit
				, forceTwoSided }
		{
		}
		/**
		 *\~english
		 *\brief		Constructor for shadow passes.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	cameraUbo	The scene matrices UBO.
		 *\param[in]	culler		The scene culler for this pass.
		 *\~french
		 *\brief		Constructeur pour les passes d'ombres.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	cameraUbo	L'UBO des matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 */
		RenderNodesPassDesc( VkExtent3D size
			, CameraUbo const & cameraUbo
			, SceneCuller & culler )
			: RenderNodesPassDesc{ castor::move( size )
				, cameraUbo
				, nullptr
				, culler
				, RenderFilter::eNone
				, true
				, false }
		{
		}
		/**
		 *\~english
		 *\brief		Constructor for opaque passes.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	cameraUbo	The scene matrices UBO.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	culler		The scene culler for this pass.
		 *\~french
		 *\brief		Constructeur pour les passes opaques.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	cameraUbo	L'UBO des matrices de la scène.
		 *\param[in]	sceneUbo	L'UBO de scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 */
		RenderNodesPassDesc( VkExtent3D size
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, SceneCuller & culler )
			: RenderNodesPassDesc{ castor::move( size )
				, cameraUbo
				, sceneUbo
				, culler
				, RenderFilter::eAlphaBlend
				, true
				, false }
		{
		}
		/**
		 *\~english
		 *\brief		Constructor for transparent passes.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	cameraUbo	The scene matrices UBO.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	culler		The scene culler for this pass.
		 *\param[in]	oit			The order independant status.
		 *\~french
		 *\brief		Constructeur pour les passes transparents.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	cameraUbo	L'UBO des matrices de la scène.
		 *\param[in]	sceneUbo	L'UBO de scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	oit			Le statut de rendu indépendant de l'ordre des objets.
		 */
		RenderNodesPassDesc( VkExtent3D size
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, SceneCuller & culler
			, bool oit )
			: RenderNodesPassDesc{ castor::move( size )
				, cameraUbo
				, sceneUbo
				, culler
				, RenderFilter::eOpaque
				, oit
				, false }
		{
		}
		/**
		 *\~english
		 *\param[in]	value	The scene node to ignore during rendering.
		 *\~french
		 *\param[in]	value	Le scene node à ignorer pendant le rendu.
		 */
		RenderNodesPassDesc & ignored( SceneNode const & value )
		{
			m_ignored = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The scene node to ignore during rendering.
		 *\~french
		 *\param[in]	value	Le scene node à ignorer pendant le rendu.
		 */
		RenderNodesPassDesc & safeBand( bool value )
		{
			m_safeBand = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The max frame pass variants count.
		 *\~french
		 *\param[in]	value	Le nombre maximum de variantes de la frame pass.
		 */
		RenderNodesPassDesc & passCount( uint32_t value )
		{
			m_ruConfig.maxPassCount = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	\p true if the pass is for static nodes.
		 *\~french
		 *\param[in]	value	\p true si la passe est pour les noeuds statiques.
		 */
		RenderNodesPassDesc & isStatic( bool value )
		{
			m_handleStatic = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The frame pass resettable status.
		 *\~french
		 *\param[in]	value	Le statut resettable de la frame pass.
		 */
		RenderNodesPassDesc & resettable( bool value )
		{
			m_ruConfig.resettable = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	view	The image view which the action is applied to.
		 *\param[in]	action	The action.
		 *\~french
		 *\param[in]	view	L'image view sur laquelle est appliquée l'action.
		 *\param[in]	action	L'action.
		 */
		RenderNodesPassDesc & implicitAction( crg::ImageViewId view
			, crg::RecordContext::ImplicitAction action )
		{
			m_ruConfig.implicitAction( view, castor::move( action ) );
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The use of mesh shaders status.
		 *\~french
		 *\param[in]	value	Le statut d'utilisation des mesh shaders.
		 */
		RenderNodesPassDesc & meshShading( bool value )
		{
			m_meshShading = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The nodes pass components flags.
		 *\~french
		 *\param[in]	value	Les indicateurs de composants de la passe de noeuds.
		 */
		RenderNodesPassDesc & componentModeFlags( ComponentModeFlags value )
		{
			m_componentModeFlags = castor::move( value );
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	\p true if the pass supports clustered lighting.
		 *\~french
		 *\param[in]	value	\p true si la passe supporte l'éclairage par cluster.
		 */
		RenderNodesPassDesc & allowClusteredLighting( bool value = true )
		{
			m_allowClusteredLighting = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The deferred lighting filtering mode.
		 *\~french
		 *\param[in]	value	Le mode de filtrage de l'éclairage différé.
		 */
		RenderNodesPassDesc & deferredLightingFilter( DeferredLightingFilter value )
		{
			m_deferredLightingFilter = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The parallax occlusion filtering mode.
		 *\~french
		 *\param[in]	value	Le mode de filtrage du parallax occlusion.
		 */
		RenderNodesPassDesc & parallaxOcclusionFilter( ParallaxOcclusionFilter value )
		{
			m_parallaxOcclusionFilter = value;
			return *this;
		}

		VkExtent3D m_size;
		CameraUbo const & m_cameraUbo;
		SceneUbo const * m_sceneUbo{};
		SceneCuller & m_culler;
		RenderFilters m_filters;
		bool m_oit;
		bool m_forceTwoSided;
		bool m_safeBand{};
		bool m_meshShading{};
		SceneNode const * m_ignored{};
		uint32_t m_index{ 0u };
		std::optional< bool > m_handleStatic{ std::nullopt };
		crg::ru::Config m_ruConfig{ 1u, true };
		ComponentModeFlags m_componentModeFlags{ ComponentModeFlag::eAll };
		bool m_allowClusteredLighting{};
		DeferredLightingFilter m_deferredLightingFilter{ DeferredLightingFilter::eIgnore };
		ParallaxOcclusionFilter m_parallaxOcclusionFilter{ ParallaxOcclusionFilter::eIgnore };
	};

	using SceneCullerHolder = castor::DataHolderT< SceneCuller * >;
	using RenderQueueHolder = castor::DataHolderT< RenderQueueUPtr >;

	class RenderNodesPass
		: public castor::OwnedBy< Engine >
		, public castor::Named
		, private SceneCullerHolder
		, private RenderQueueHolder
		, public crg::RenderPass
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	typeName	The pass type name.
		 *\param[in]	targetImage	The image this pass renders to.
		 *\param[in]	targetDepth	The depth image this pass renders to.
		 *\param[in]	desc		The construction data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	typeName	Le nom du type de la passe.
		 *\param[in]	targetImage	L'image dans laquelle cette passe fait son rendu.
		 *\param[in]	targetDepth	L'image de profondeur dans laquelle cette passe fait son rendu.
		 *\param[in]	desc		Les données de construction.
		 */
		C3D_API RenderNodesPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::String const & typeName
			, crg::ImageViewIdArray targetImage
			, crg::ImageViewIdArray targetDepth
			, RenderNodesPassDesc const & desc );

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderNodesPass()noexcept override;
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API virtual void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Retrieves the geometry shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\param[in]	builder	Receives the source.
		 *\~french
		 *\brief		Récupère le source du geometry shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 *\param[in]	builder	Reçoit le source.
		 */
		C3D_API void getSubmeshShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\param[in]	builder	Receives the source.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 *\param[in]	builder	Reçoit le source.
		 */
		C3D_API void getPixelShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const;
		/**
		 *\~english
		 *\brief		Adjusts given flags to match the render pass requirements.
		 *\param[in]	flags	The flags.
		 *\~french
		 *\brief		Ajuste les flags donnés pour qu'ils correspondent aux pré-requis de la passe.
		 *\param[in]	flags	Les flags.
		 */
		C3D_API void forceAdjustFlags( PipelineFlags & flags )const;
		/**
		 *\~english
		 *\brief		Adjusts given flags to match the render pass requirements.
		 *\param[in]	submeshCombine	The flags.
		 *\~french
		 *\brief		Ajuste les flags donnés pour qu'ils correspondent aux pré-requis de la passe.
		 *\param[in]	submeshCombine	Les flags.
		 */
		C3D_API SubmeshComponentCombine adjustFlags( SubmeshComponentCombine const & submeshCombine )const;
		/**
		 *\~english
		 *\brief		Adjusts given flags to match the render pass requirements.
		 *\param[in]	passCombine	The flags.
		 *\~french
		 *\brief		Ajuste les flags donnés pour qu'ils correspondent aux pré-requis de la passe.
		 *\param[in]	passCombine	Les flags.
		 */
		C3D_API PassComponentCombine adjustFlags( PassComponentCombine const & passCombine )const;
		/**
		 *\~english
		 *\brief		Adjusts given flags to match the render pass requirements.
		 *\param[in]	flags	The flags.
		 *\~french
		 *\brief		Ajuste les flags donnés pour qu'ils correspondent aux pré-requis de la passe.
		 *\param[in]	flags	Les flags.
		 */
		C3D_API ProgramFlags adjustFlags( ProgramFlags flags )const;
		/**
		 *\~english
		 *\brief		Adjusts given flags to match the render pass requirements.
		 *\param[in]	flags	The flags.
		 *\~french
		 *\brief		Ajuste les flags donnés pour qu'ils correspondent aux pré-requis de la passe.
		 *\param[in]	flags	Les flags.
		 */
		C3D_API SceneFlags adjustFlags( SceneFlags flags )const;
		/**
		 *\~english
		 *\brief		Filters the given textures flags using this pass needed textures.
		 *\param[in]	textureCombine	The textures flags.
		 *\return		The filtered flags.
		 *\~french
		 *\brief		Filtre les indicateurs de textures donnés en utilisant ceux voulus par cette passe.
		 *\param[in]	textureCombine	Les indicateurs de textures.
		 *\return		Les indicateurs filtrés.
		 */
		C3D_API TextureCombine adjustFlags( TextureCombine const & textureCombine )const;
		/**
		 *\~english
		 *\brief		Creates the pipeline flags for given configuration.
		 *\param[in]	passComponents		The pass components combination.
		 *\param[in]	submeshComponents	The submesh components combination.
		 *\param[in]	colourBlendMode		The colour blending mode.
		 *\param[in]	alphaBlendMode		The alpha blending mode.
		 *\param[in]	renderPassTypeId	The render pass type ID.
		 *\param[in]	lightingModelId		The lighting model ID.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\param[in]	alphaFunc			The alpha comparison function (for opaque nodes).
		 *\param[in]	blendAlphaFunc		The alpha comparison function (for transparent nodes).
		 *\param[in]	textures			The textures configuration.
		 *\param[in]	programFlags		A combination of ProgramFlag.
		 *\param[in]	sceneFlags			Scene related flags.
		 *\param[in]	topology			The render topology.
		 *\param[in]	isFrontCulled		\p true for front face culling, \p false for back face culling.
		 *\param[in]	passLayerIndex		The material pass layer index.
		 *\param[in]	morphTargets		The morph targets buffer.
		 *\param[in]	submeshData			The submesh render data, if any.
		 *\~french
		 *\brief		Crée les indicateurs de pipeline pour la configuration donnée.
		 *\param[in]	passComponents		La combinaison de composants de passe.
		 *\param[in]	submeshComponents	La combinaison de composants de submesh.
		 *\param[in]	colourBlendMode		Le mode de mélange de couleurs.
		 *\param[in]	alphaBlendMode		Le mode de mélange de l'alpha
		 *\param[in]	renderPassTypeId	L'ID du type de render pass.
		 *\param[in]	lightingModelId		L'ID du modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 *\param[in]	alphaFunc			La fonction de comparaison de l'alpha (pour les noeuds opaques).
		 *\param[in]	blendAlphaFunc		La fonction de comparaison de l'alpha (pour les noeuds transparents).
		 *\param[in]	textures			La configuration des textures.
		 *\param[in]	programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags			Les indicateurs relatifs à la scène.
		 *\param[in]	topology			La topologie de rendu.
		 *\param[in]	isFrontCulled		\p true pour front face culling, \p false pour back face culling.
		 *\param[in]	passLayerIndex		L'indice de la couche de la passe de matériau.
		 *\param[in]	morphTargets		Le buffer de morph targets.
		 *\param[in]	submeshData			Les données de rendu du submesh, s'il en a.
		 */
		C3D_API PipelineFlags createPipelineFlags( PassComponentCombine const & passComponents
			, SubmeshComponentCombine const & submeshComponents
			, BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, RenderPassTypeID renderPassTypeId
			, LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId
			, VkCompareOp alphaFunc
			, VkCompareOp blendAlphaFunc
			, TextureCombine const & textures
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, bool isFrontCulled
			, uint32_t passLayerIndex
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets
			, SubmeshRenderData const * submeshData )const noexcept;
		/**
		 *\~english
		 *\brief		Creates the pipeline flags for given configuration.
		 *\param[in]	pass				The pass for which the pipeline is created.
		 *\param[in]	textures			The textures configuration.
		 *\param[in]	submeshComponents	The submesh components combination.
		 *\param[in]	programFlags		A combination of ProgramFlag.
		 *\param[in]	sceneFlags			Scene related flags.
		 *\param[in]	topology			The render topology.
		 *\param[in]	isFrontCulled		\p true for front face culling, \p false for back face culling.
		 *\param[in]	morphTargets		The morph targets buffer.
		 *\param[in]	submeshData			The submesh render data, if any.
		 *\~french
		 *\brief		Crée les indicateurs de pipeline pour la configuration donnée.
		 *\param[in]	pass				La passe pour laquelle le pipeline est créé.
		 *\param[in]	textures			La configuration des textures.
		 *\param[in]	submeshComponents	La combinaison de composants de submesh.
		 *\param[in]	programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags			Les indicateurs relatifs à la scène.
		 *\param[in]	topology			La topologie de rendu.
		 *\param[in]	isFrontCulled		\p true pour front face culling, \p false pour back face culling.
		 *\param[in]	morphTargets		Le buffer de morph targets.
		 *\param[in]	submeshData			Les données de rendu du submesh, s'il en a.
		 */
		C3D_API PipelineFlags createPipelineFlags( Pass const & pass
			, TextureCombine const & textures
			, SubmeshComponentCombine const & submeshComponents
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, bool isFrontCulled
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets
			, SubmeshRenderData const * submeshData )const noexcept;
		/**
		 *\~english
		 *\brief			Prepares the pipeline matching the given flags, for back face culling nodes.
		 *\param[in]		pipelineFlags			The pipeline flags.
		 *\param[in]		vertexLayouts			The vertex buffers layouts.
		 *\param[in]		meshletDescriptorLayout	The optional meshlets descriptor layout.
		 *\~french
		 *\brief			Prépare le pipeline qui correspond aux indicateurs donnés, pour les noeuds en back face culling.
		 *\param[in]		pipelineFlags			Les indicateurs de pipeline.
		 *\param[in]		vertexLayouts			Les layouts des tampons de sommets.
		 *\param[in]		meshletDescriptorLayout	Les layouts optionnels de descripteurs de meshlets.
		 */
		C3D_API PipelineAndID prepareBackPipeline( PipelineFlags const & pipelineFlags
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
			, ashes::DescriptorSetLayout const * meshletDescriptorLayout );
		/**
		 *\~english
		 *\brief			Prepares the pipeline matching the given flags, for front face culling nodes.
		 *\param[in]		pipelineFlags			The pipeline flags.
		 *\param[in]		vertexLayouts			The vertex buffers layouts.
		 *\param[in]		meshletDescriptorLayout	The optional meshlets descriptor layout.
		 *\~french
		 *\brief			Prépare le pipeline qui correspond aux indicateurs donnés, pour les noeuds en front face culling.
		 *\param[in]		pipelineFlags			Les indicateurs de pipeline.
		 *\param[in]		vertexLayouts			Les layouts des tampons de sommets.
		 *\param[in]		meshletDescriptorLayout	Les layouts optionnels de descripteurs de meshlets.
		 */
		C3D_API PipelineAndID prepareFrontPipeline( PipelineFlags const & pipelineFlags
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
			, ashes::DescriptorSetLayout const * meshletDescriptorLayout );
		/**
		 *\~english
		 *\brief		Cleans up all pipelines from the lists.
		 *\~french
		 *\brief		Nettoie tous les pipelines des listes.
		 */
		C3D_API void cleanupPipelines();
		/**
		 *\~english
		 *\brief		Initialises the additional descriptor set.
		 *\param[in]	pipeline		The render pipeline.
		 *\param[in]	shadowMaps		The shadow maps.
		 *\param[in]	shadowBuffer	The buffer holding the shadow configuration data.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs additionnels.
		 *\param[in]	pipeline		Le render pipeline.
		 *\param[in]	shadowMaps		Les shadow maps.
		 *\param[in]	shadowBuffer	Le buffer contenant les données de configuration des ombres.
		 */
		C3D_API void initialiseAdditionalDescriptor( RenderPipeline & pipeline
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer );
		/**
		 *\~english
		 *\brief		Sets the node ignored node.
		 *\remarks		All objects attached to this node will be ignored during rendering.
		 *\param[in]	node	The node.
		 *\~french
		 *\brief		Définit le noeud ignoré.
		 *\remarks		Tous les objets liés à ce noued seront ignorés lors du rendu.
		 *\param[in]	node	Le noeud.
		 */
		C3D_API void setIgnoredNode( SceneNode const & node );
		/**
		 *\~english
		 *\brief			Registers nodes counts to given RenderInfo.
		 *\param[in,out]	info	Receives the counts.
		 *\~french
		 *\brief			Enregistre les comptes de noeuds dans le RenderInfo donné.
		 *\param[in,out]	info	Reçoit les comptes.
		 */
		C3D_API void countNodes( RenderInfo & info )const noexcept;
		/**
		 *\~english
		 *\param[in]	colourBlendMode	The colour blend mode.
		 *\param[in]	alphaBlendMode	The alpha blend mode.
		 *\param[in]	attachesCount	The wanted blend attaches count.
		 *\return		A blend state matching given blend modes.
		 *\~french
		 *\param[in]	colourBlendMode	Le mode de mélange couleurs.
		 *\param[in]	alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]	attachesCount	Le nombre d'attaches de mélange voulues.
		 *\return		Un état de mélange correspondant aux modes de mélange donnés.
		 */
		C3D_API static ashes::PipelineColorBlendStateCreateInfo createBlendState( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, uint32_t attachesCount );
		/**
		 *\~english
		 *\brief			Adds shadow maps descriptor layout bindings to given list.
		 *\param[in]		sceneFlags		Used to define what shadow maps need to be bound.
		 *\param[in,out]	bindings		Receives the bindings.
		 *\param[in]		shaderStages	The impacted shader stages.
		 *\param[in,out]	index			The current binding index.
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout des shadow maps à la liste donnée.
		 *\param[in]		sceneFlags		Le mode de mélange couleurs.
		 *\param[in,out]	bindings		Reçoit les bindings.
		 *\param[in]		shaderStages	Les shader stages impactés.
		 *\param[in,out]	index			L'index de binding actuel.
		 */
		C3D_API static void addShadowBindings( SceneFlags const & sceneFlags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds shadow maps descriptor layout bindings to given list.
		 *\param[in,out]	bindings		Receives the bindings.
		 *\param[in]		shaderStages	The impacted shader stages.
		 *\param[in,out]	index			The current binding index.
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout des shadow maps à la liste donnée.
		 *\param[in,out]	bindings		Reçoit les bindings.
		 *\param[in]		shaderStages	Les shader stages impactés.
		 *\param[in,out]	index			L'index de binding actuel.
		 */
		C3D_API static void addShadowBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds background descriptor layout bindings to given list.
		 *\param[in]		background		The background.
		 *\param[in,out]	bindings		Receives the bindings.
		 *\param[in]		shaderStages	The impacted shader stages.
		 *\param[in,out]	index			The current binding index.
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout du background à la liste donnée.
		 *\param[in]		background		Le fond.
		 *\param[in,out]	bindings		Reçoit les bindings.
		 *\param[in]		shaderStages	Les shader stages impactés.
		 *\param[in,out]	index			L'index de binding actuel.
		 */
		C3D_API static void addBackgroundBindings( SceneBackground const & background
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds indirect lighting descriptor layout bindings to given list.
		 *\param[in]		flags				The scene flags.
		 *\param[in]		indirectLighting	The indirect lighting data.
		 *\param[in,out]	bindings			Receives the bindings.
		 *\param[in]		shaderStages		The impacted shader stages.
		 *\param[in,out]	index				The current binding index.
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout de l'éclairage indirect à la liste donnée.
		 *\param[in]		flags				Les indicateurs de scène.
		 *\param[in]		indirectLighting	Les données d'indirect lighting.
		 *\param[in,out]	bindings			Reçoit les bindings.
		 *\param[in]		shaderStages		Les shader stages impactés.
		 *\param[in,out]	index				L'index de binding actuel.
		 */
		C3D_API static void addGIBindings( SceneFlags flags
			, IndirectLightingData const & indirectLighting
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds clusters descriptor layout bindings to given list.
		 *\param[in]		frustumClusters	The clusters.
		 *\param[in,out]	bindings		Receives the bindings.
		 *\param[in]		shaderStages	The impacted shader stages.
		 *\param[in,out]	index			The current binding index.
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout des clusters à la liste donnée.
		 *\param[in]		frustumClusters	Les clusters.
		 *\param[in,out]	bindings		Reçoit les bindings.
		 *\param[in]		shaderStages	Les shader stages impactés.
		 *\param[in,out]	index			L'index de binding actuel.
		 */
		C3D_API static void addClusteredLightingBindings( FrustumClusters const & frustumClusters
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index );
		C3D_API static void addShadowDescriptor( RenderSystem const & renderSystem
			, crg::RunnableGraph & graph
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const & shadowBuffer
			, uint32_t & index );
		C3D_API static void addShadowDescriptor( RenderSystem const & renderSystem
			, crg::RunnableGraph & graph
			, SceneFlags const & sceneFlags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const & shadowBuffer
			, uint32_t & index );
		C3D_API static void addBackgroundDescriptor( SceneBackground const & background
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index );
		C3D_API static void addGIDescriptor( SceneFlags sceneFlags
			, IndirectLightingData const & indirectLighting
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index );
		C3D_API static void addClusteredLightingDescriptor( FrustumClusters const & frustumClusters
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API bool areValidPassFlags( PassComponentCombine const & passFlags )const noexcept;
		C3D_API virtual bool isPassEnabled()const noexcept;
		C3D_API virtual ShaderFlags getShaderFlags()const noexcept;
		C3D_API virtual bool areDebugTargetsEnabled()const noexcept;
		C3D_API bool isValidPass( Pass const & pass )const noexcept;
		C3D_API bool isValidRenderable( RenderedObject const & object )const noexcept;
		C3D_API bool isValidNode( SceneNode const & node )const noexcept;
		C3D_API bool allowClusteredLighting( ClustersConfig const & config )const noexcept;
		C3D_API bool hasNodes()const noexcept;
		C3D_API Scene & getScene()const noexcept;
		C3D_API SceneNode const * getIgnoredNode()const noexcept;
		C3D_API bool isMeshShading()const noexcept;
		C3D_API PipelinesNodesT< SubmeshRenderNode > const & getSubmeshNodes()const;
		C3D_API InstantiatedPipelinesNodesT< SubmeshRenderNode > const & getInstancedSubmeshNodes()const;
		C3D_API PipelinesNodesT< BillboardRenderNode > const & getBillboardNodes()const;
		C3D_API uint32_t getMaxPipelineId()const;
		C3D_API PipelineBufferArray const & getPassPipelineNodes()const;
		C3D_API uint32_t getPipelineNodesIndex( PipelineBaseHash const & hash
			, ashes::BufferBase const & buffer )const;
		C3D_API uint32_t getDrawCallsCount()const;
		C3D_API RenderCounts const & getVisibleCounts()const;


		ComponentModeFlags getComponentsMask()const noexcept
		{
			return m_componentsMask;
		}

		bool isOrderIndependent()const noexcept
		{
			return m_oit;
		}

		SceneCuller & getCuller()const noexcept
		{
			return *SceneCullerHolder::getData();
		}

		CameraUbo const & getMatrixUbo()const noexcept
		{
			return m_cameraUbo;
		}

		uint32_t getPipelinesCount()const noexcept
		{
			return uint32_t( m_backPipelines.size()
				+ m_frontPipelines.size() );
		}

		bool isDirty()const noexcept
		{
			return m_isDirty;
		}

		bool forceTwoSided()const noexcept
		{
			return m_forceTwoSided;
		}

		RenderFilters getRenderFilters()const noexcept
		{
			return m_filters;
		}

		castor::String const & getTypeName()const noexcept
		{
			return m_typeName;
		}

		RenderPassTypeID getTypeID()const noexcept
		{
			return m_typeID;
		}

		bool filtersStatic()const noexcept
		{
			return handleStatic()
				&& !*m_handleStatic;
		}

		bool filtersNonStatic()const noexcept
		{
			return handleStatic()
				&& *m_handleStatic;
		}

		bool handleStatic()const noexcept
		{
			return m_handleStatic != std::nullopt;
		}

		DeferredLightingFilter getDeferredLightingFilter()const noexcept
		{
			return m_deferredLightingFilter;
		}

		ParallaxOcclusionFilter getParallaxOcclusionFilter()const noexcept
		{
			return m_parallaxOcclusionFilter;
		}
		/**@}*/

		mutable RenderNodesPassChangeSignal onSortNodes;

	private:
		void doSubInitialise()const;
		void doSubRecordInto( VkCommandBuffer commandBuffer )const;

	protected:
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
		 *\brief			Fills the render pass specific descriptor layout bindings.
		 *\param[in]		flags		The pipeline flags.
		 *\param[in,out]	bindings	Receives the additional bindings.
		 *\~french
		 *\brief			Remplit les attaches de layout de descripteurs spécifiques à une passe de rendu.
		 *\param[in]		flags		Les indicateurs de pipeline.
		 *\param[in,out]	bindings	Reçoit les attaches additionnelles.
		 */
		C3D_API virtual void doFillAdditionalBindings( PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const = 0;
		/**
		 *\~english
		 *\param[in]	pass	The material pass.
		 *\return		\p true if the pass is rendered through this nodes pass.
		 *\~french
		 *\param[in]	pass	La passe de matériau.
		 *\return		\p true si la passe est rendue via cette passe de noeuds.
		 */
		C3D_API virtual bool doIsValidPass( Pass const & pass )const noexcept;
		/**
		 *\~english
		 *\param[in]	object	The rendered object.
		 *\return		\p true if the object is rendered through this nodes pass.
		 *\~french
		 *\param[in]	object	L'objet rendu.
		 *\return		\p true si l'objet est rendu via cette passe de noeuds.
		 */
		C3D_API virtual bool doIsValidRenderable( RenderedObject const & object )const noexcept;
		C3D_API virtual SubmeshComponentCombine doAdjustSubmeshComponents( SubmeshComponentCombine submeshCombine )const;
		C3D_API virtual ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const;
		C3D_API virtual SceneFlags doAdjustSceneFlags( SceneFlags flags )const;
		C3D_API ShaderProgramRPtr doGetProgram( PipelineFlags const & flags );
		/**
		 *\copydoc	castor3d::RenderTechniquePass::doAccept
		 */
		C3D_API void doAccept( castor3d::RenderTechniqueVisitor & visitor );
		C3D_API void doUpdateFlags( PipelineFlags & flags )const;
		C3D_API void doAddShadowBindings( Scene const & scene
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddBackgroundBindings( Scene const & scene
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddClusteredLightingBindings( RenderTarget const & target
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddShadowDescriptor( Scene const & scene
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, uint32_t & index )const;
		C3D_API void doAddBackgroundDescriptor( Scene const & scene
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const;
		C3D_API void doAddClusteredLightingDescriptor( RenderTarget const & target
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index )const;

		RenderQueue & getRenderQueue()const noexcept
		{
			return *RenderQueueHolder::getData();
		}

	private:
		ashes::VkDescriptorSetLayoutBindingArray doCreateAdditionalBindings( PipelineFlags const & flags )const;
		castor::Vector< RenderPipelineUPtr > & doGetFrontPipelines();
		castor::Vector< RenderPipelineUPtr > & doGetBackPipelines();
		castor::Vector< RenderPipelineUPtr > const & doGetFrontPipelines()const;
		castor::Vector< RenderPipelineUPtr > const & doGetBackPipelines()const;
		PipelineAndID doPreparePipeline( ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
			, ashes::DescriptorSetLayout const * meshletDescriptorLayout
			, PipelineFlags const & flags
			, VkCullModeFlags cullMode );
		/**
		 *\~english
		 *\brief		Creates the rasterization state.
		 *\param[in]	flags		The pipeline flags.
		 *\param[in]	cullMode	The cull mode.
		 *\~french
		 *\brief		Crée l'état de rastérisation.
		 *\param[in]	flags		Les indicateurs de pipeline.
		 *\param[in]	cullMode	Le mode de culling.
		 */
		C3D_API virtual ashes::PipelineRasterizationStateCreateInfo doCreateRasterizationState( PipelineFlags const & flags
			, VkCullModeFlags cullMode )const;
		/**
		 *\~english
		 *\brief		Creates the depth stencil state.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Crée l'état de profondeur et stencil.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Creates the colour blend state.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Crée l'état de mélange des couleurs.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Creates the multisample state.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Crée l'état de multisample.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ashes::PipelineMultisampleStateCreateInfo doCreateMultisampleState( PipelineFlags const & flags )const
		{
			return ashes::PipelineMultisampleStateCreateInfo{};
		}
		/**
		 *\~english
		 *\brief			Initialises the additional descriptor set.
		 *\param[in]		flags				The pipeline flags.
		 *\param[in,out]	descriptorWrites	Receives the descriptor writes.
		 *\param[in]		shadowMaps			The shadow maps.
		 *\param[in]		shadowBuffer		The buffer holding the shadows configuration data.
		 *\~french
		 *\brief			Initialise l'ensemble de descripteurs additionnels.
		 *\param[in]		flags				Les indicateurs de pipeline.
		 *\param[in,out]	descriptorWrites	Reçoit les descriptor writes.
		 *\param[in]		shadowMaps			Les shadow maps.
		 *\param[in]		shadowBuffer		Le buffer contenant les données de configuration des ombres.
		 */
		C3D_API virtual void doFillAdditionalDescriptor( PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer ) = 0;
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	flags	The pipeline flags.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual void doAdjustFlags( PipelineFlags & flags )const;
		/**
		 *\~english
		 *\brief		Retrieves the task shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\param[in]	builder	Records the shader.
		 *\~french
		 *\brief		Récupère le source du task shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 *\param[in]	builder	Enregistree le shader.
		 */
		C3D_API virtual void doGetSubmeshShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\param[in]	builder	Records the shader.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 *\param[in]	builder	Enregistree le shader.
		 */
		C3D_API virtual void doGetPixelShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const = 0;

	protected:
		RenderDevice const & m_device;
		RenderSystem & m_renderSystem;
		CameraUbo const & m_cameraUbo;
		crg::ImageViewIdArray m_targetImage;
		crg::ImageViewIdArray m_targetDepth;
		castor::String m_typeName;
		RenderPassTypeID m_typeID{};
		RenderFilters m_filters{ RenderFilter::eNone };
		castor::String m_category;
		castor::Size m_size;
		bool m_oit{ false };
		bool m_forceTwoSided{ false };
		bool m_safeBand{ false };
		bool m_isDirty{ true };
		bool m_meshShading{};
		SceneUbo const * m_sceneUbo{};
		uint32_t m_index{ 0u };
		std::optional< bool > m_handleStatic{ std::nullopt };
		ComponentModeFlags m_componentsMask{};
		bool m_allowClusteredLighting{};
		DeferredLightingFilter m_deferredLightingFilter{};
		ParallaxOcclusionFilter m_parallaxOcclusionFilter{};

	private:
		struct PassDescriptors
		{
			PassDescriptors() = default;

			ashes::DescriptorSetPoolPtr pool{};
			ashes::DescriptorSetLayoutPtr layout{};
			ashes::DescriptorSetPtr set{};
		};
		using PassDescriptorsMap = castor::UnorderedMap< size_t, PassDescriptors >;

		PassDescriptorsMap m_additionalDescriptors;
		castor::Vector< RenderPipelineUPtr > m_frontPipelines;
		castor::Vector< RenderPipelineUPtr > m_backPipelines;
	};

	struct IsRenderPassEnabled
	{
		explicit IsRenderPassEnabled( RenderNodesPass const & pass )
			: m_pass{ &pass }
		{
		}

		IsRenderPassEnabled()
			: m_pass{}
		{
		}

		void setPass( RenderNodesPass const & pass )noexcept
		{
			m_pass = &pass;
		}

		bool operator()()const noexcept
		{
			return m_pass
				&& m_pass->isPassEnabled();
		}

		RenderNodesPass const * m_pass;
	};
	CU_DeclareSmartPtr( castor3d, IsRenderPassEnabled, C3D_API );
}

#endif
