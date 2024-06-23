/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPass_H___
#define ___C3D_RenderPass_H___

#include "Castor3D/Render/NodesPass.hpp"

#include <RenderGraph/RunnablePasses/RenderPass.hpp>

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
			: m_base{ castor::move( size ), cameraUbo, sceneUbo, filters }
			, m_culler{ culler }
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
		NodesPassDesc const & base()const noexcept
		{
			return m_base;
		}
		/**
		 *\~english
		 *\param[in]	value	The scene node to ignore during rendering.
		 *\~french
		 *\param[in]	value	Le scene node à ignorer pendant le rendu.
		 */
		RenderNodesPassDesc & safeBand( bool value )
		{
			m_base.safeBand( value );
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
			m_base.isStatic( value );
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
			m_base.componentModeFlags( value );
			return *this;
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
		 *\param[in]	action	The action to run before the pass recording.
		 *\~french
		 *\param[in]	action	L'action à lancer avant l'enregistrement de la passe.
		 */
		RenderNodesPassDesc & prePassAction( crg::RecordContext::ImplicitAction action )
		{
			m_ruConfig.prePassAction( std::move( action ) );
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	action	The action to run after the pass recording.
		 *\~french
		 *\param[in]	action	L'action à lancer après l'enregistrement de la passe.
		 */
		RenderNodesPassDesc & postPassAction( crg::RecordContext::ImplicitAction action )
		{
			m_ruConfig.postPassAction( std::move( action ) );
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

		NodesPassDesc m_base;
		SceneCuller & m_culler;
		bool m_oit;
		bool m_forceTwoSided;
		bool m_meshShading{};
		SceneNode const * m_ignored{};
		crg::ru::Config m_ruConfig{ 1u, true };
		bool m_allowClusteredLighting{};
		DeferredLightingFilter m_deferredLightingFilter{ DeferredLightingFilter::eIgnore };
		ParallaxOcclusionFilter m_parallaxOcclusionFilter{ ParallaxOcclusionFilter::eIgnore };
	};

	using SceneCullerHolder = castor::DataHolderT< SceneCuller * >;
	using RenderQueueHolder = castor::DataHolderT< RenderQueueUPtr >;

	class RenderNodesPass
		: public NodesPass
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
		 *\param[in]	flags	The flags.
		 *\~french
		 *\brief		Ajuste les flags donnés pour qu'ils correspondent aux pré-requis de la passe.
		 *\param[in]	flags	Les flags.
		 */
		C3D_API ProgramFlags adjustFlags( ProgramFlags flags )const;
		using NodesPass::adjustFlags;
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
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API bool isPassEnabled()const noexcept override;
		C3D_API virtual ShaderFlags getShaderFlags()const noexcept;
		C3D_API virtual bool areDebugTargetsEnabled()const noexcept;
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

		bool forceTwoSided()const noexcept
		{
			return m_forceTwoSided;
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
		C3D_API void doUpdate( CpuUpdater & updater )override;
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
		C3D_API virtual ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const;
		C3D_API ShaderProgramRPtr doGetProgram( PipelineFlags const & flags );
		/**
		 *\~english
		 *\param[in]	pass	The material pass.
		 *\return		\p true if the pass is rendered through this nodes pass.
		 *\~french
		 *\param[in]	pass	La passe de matériau.
		 *\return		\p true si la passe est rendue via cette passe de noeuds.
		 */
		C3D_API bool doIsValidPass( Pass const & pass )const noexcept override;
		/**
		 *\~english
		 *\param[in]	object	The rendered object.
		 *\return		\p true if the object is rendered through this nodes pass.
		 *\~french
		 *\param[in]	object	L'objet rendu.
		 *\return		\p true si l'objet est rendu via cette passe de noeuds.
		 */
		C3D_API bool doIsValidRenderable( RenderedObject const & object )const noexcept override;
		C3D_API SubmeshComponentCombine doAdjustSubmeshComponents( SubmeshComponentCombine submeshCombine )const override;
		C3D_API SceneFlags doAdjustSceneFlags( SceneFlags flags )const override;
		C3D_API bool doAreValidPassFlags( PassComponentCombine const & passFlags )const noexcept override;
		C3D_API bool doIsValidNode( SceneNode const & node )const noexcept override;
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
		bool m_oit{ false };
		bool m_forceTwoSided{ false };
		bool m_meshShading{};
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
}

#endif
