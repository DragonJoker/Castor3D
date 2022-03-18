/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPass_H___
#define ___C3D_RenderPass_H___

#include "RenderModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <RenderGraph/RunnablePasses/RenderPass.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <unordered_map>

namespace castor3d
{
	struct RenderNodesPassDesc
	{
		RenderNodesPassDesc( VkExtent3D size
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, RenderMode mode
			, bool oit
			, bool forceTwoSided )
			: m_size{ std::move( size ) }
			, m_matrixUbo{ matrixUbo }
			, m_culler{ culler }
			, m_mode{ mode }
			, m_oit{ oit }
			, m_forceTwoSided{ forceTwoSided }
		{
		}
		/**
		 *\~english
		 *\brief		Constructor for opaque passes.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The scene culler for this pass.
		 *\~french
		 *\brief		Constructeur pour les passes opaques.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	matrixUbo	L'UBO des matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 */
		RenderNodesPassDesc( VkExtent3D size
			, MatrixUbo & matrixUbo
			, SceneCuller & culler )
			: RenderNodesPassDesc{ std::move( size )
				, matrixUbo
				, culler
				, RenderMode::eOpaqueOnly
				, true
				, false }
		{
		}
		/**
		 *\~english
		 *\brief		Constructor for transparent passes.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The scene culler for this pass.
		 *\param[in]	oit			The order independant status.
		 *\~french
		 *\brief		Constructeur pour les passes transparents.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	matrixUbo	L'UBO des matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	oit			Le statut de rendu indépendant de l'ordre des objets.
		 */
		RenderNodesPassDesc( VkExtent3D size
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, bool oit )
			: RenderNodesPassDesc{ std::move( size )
				, matrixUbo
				, culler
				, RenderMode::eTransparentOnly
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
			m_ruConfig.implicitAction( view, action );
			return *this;
		}

		VkExtent3D m_size;
		MatrixUbo & m_matrixUbo;
		SceneCuller & m_culler;
		RenderMode m_mode;
		bool m_oit;
		bool m_forceTwoSided;
		bool m_safeBand{};
		SceneNode const * m_ignored{};
		uint32_t m_index{ 0u };
		crg::ru::Config m_ruConfig{ 1u, true };
	};

	class RenderNodesPass
		: public castor::OwnedBy< Engine >
		, public castor::Named
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
		 *\param[in]	category	The pass category.
		 *\param[in]	name		The pass name.
		 *\param[in]	desc		The construction data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	typeName	Le nom du type de la passe.
		 *\param[in]	category	La catégorie de la passe.
		 *\param[in]	name		Le nom de la passe.
		 *\param[in]	desc		Les données de construction.
		 */
		C3D_API RenderNodesPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::String const & typeName
			, castor::String const & category
			, castor::String const & name
			, RenderNodesPassDesc const & desc );

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderNodesPass()override;
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
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API virtual void update( GpuUpdater & updater );
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
		 *\brief		Retrieves the hull shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du hull shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API ShaderPtr getHullShaderSource( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Retrieves the domain shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du domain shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API ShaderPtr getDomainShaderSource( PipelineFlags const & flags )const;
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
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API ShaderPtr getPixelShaderSource( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Adjusts given flags to match the render pass requirements.
		 *\param[in]	flags	The flags.
		 *\~french
		 *\brief		Ajuste les flags donnés pour qu'ils correspondent aux pré-requis de la passe.
		 *\param[in]	flags	Les flags.
		 */
		C3D_API PassFlags adjustFlags( PassFlags flags )const;
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
		 *\brief		Creates the pipeline flags for given configuration.
		 *\param[in]	colourBlendMode		The colour blending mode.
		 *\param[in]	alphaBlendMode		The alpha blending mode.
		 *\param[in]	passFlags			The pass flags.
		 *\param[in]	renderPassTypeID	The render pass type ID.
		 *\param[in]	passTypeID			The material pass type ID.
		 *\param[in]	heightTextureIndex	The height map index (if any).
		 *\param[in]	alphaFunc			The alpha comparison function (for opaque nodes).
		 *\param[in]	blendAlphaFunc		The alpha comparison function (for transparent nodes).
		 *\param[in]	textures			The textures configuration.
		 *\param[in]	programFlags		A combination of ProgramFlag.
		 *\param[in]	sceneFlags			Scene related flags.
		 *\param[in]	topology			The render topology.
		 *\param[in]	isFrontCulled		\p true for front face culling, \p false for back face culling.
		 *\~french
		 *\brief		Crée les indicateurs de pipeline pour la configuration donnée.
		 *\param[in]	colourBlendMode		Le mode de mélange de couleurs.
		 *\param[in]	alphaBlendMode		Le mode de mélange de l'alpha
		 *\param[in]	passFlags			Les pass flags
		 *\param[in]	renderPassTypeID	L'ID du type de render pass.
		 *\param[in]	passTypeID			L'ID du type de passe de matériau.
		 *\param[in]	heightTextureIndex	L'index de la height map (s'il y en a une).
		 *\param[in]	alphaFunc			La fonction de comparaison de l'alpha (pour les noeuds opaques).
		 *\param[in]	blendAlphaFunc		La fonction de comparaison de l'alpha (pour les noeuds transparents).
		 *\param[in]	textures			La configuration des textures.
		 *\param[in]	programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags			Les indicateurs relatifs à la scène.
		 *\param[in]	topology			La topologie de rendu.
		 *\param[in]	isFrontCulled		\p true pour front face culling, \p false pour back face culling.
		 */
		C3D_API PipelineFlags createPipelineFlags( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, PassFlags passFlags
			, RenderPassTypeID renderPassTypeID
			, PassTypeID passTypeID
			, uint32_t heightTextureIndex
			, VkCompareOp alphaFunc
			, VkCompareOp blendAlphaFunc
			, TextureFlagsArray const & textures
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, bool isFrontCulled );
		/**
		 *\~english
		 *\brief		Creates the pipeline flags for given configuration.
		 *\param[in]	pass			The pass for whic the pipeline is created.
		 *\param[in]	textures		The textures configuration.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	topology		The render topology.
		 *\param[in]	isFrontCulled	\p true for front face culling, \p false for back face culling.
		 *\~french
		 *\brief		Crée les indicateurs de pipeline pour la configuration donnée.
		 *\param[in]	pass			La passe pour laquelle le pipeline est créé.
		 *\param[in]	textures		La configuration des textures.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	topology		La topologie de rendu.
		 *\param[in]	isFrontCulled	\p true pour front face culling, \p false pour back face culling.
		 */
		C3D_API PipelineFlags createPipelineFlags( Pass const & pass
			, TextureFlagsArray const & textures
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, bool isFrontCulled );
		/**
		 *\~english
		 *\brief			Prepares the pipeline matching the given flags, for back face culling nodes.
		 *\param[in]		pipelineFlags		The pipeline flags.
		 *\param[in]		vertexLayouts		The vertex buffers layouts.
		 *\~french
		 *\brief			Prépare le pipeline qui correspond aux indicateurs donnés, pour les noeuds en back face culling.
		 *\param[in]		pipelineFlags		Les indicateurs de pipeline.
		 *\param[in]		vertexLayouts		Les layouts des tampons de sommets.
		 */
		C3D_API RenderPipeline & prepareBackPipeline( PipelineFlags pipelineFlags
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts );
		/**
		 *\~english
		 *\brief			Prepares the pipeline matching the given flags, for front face culling nodes.
		 *\param[in]		pipelineFlags		The pipeline flags.
		 *\param[in]		vertexLayouts		The vertex buffers layouts.
		 *\~french
		 *\brief			Prépare le pipeline qui correspond aux indicateurs donnés, pour les noeuds en front face culling.
		 *\param[in]		pipelineFlags		Les indicateurs de pipeline.
		 *\param[in]		vertexLayouts		Les layouts des tampons de sommets.
		 */
		C3D_API RenderPipeline & prepareFrontPipeline( PipelineFlags pipelineFlags
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts );
		/**
		 *\~english
		 *\brief		Initialises the additional descriptor set of a billboard node.
		 *\param[in]	pipeline	The render pipeline.
		 *\param[in]	shadowMaps	The shadow maps.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs additionnels pour un noeud de billboard.
		 *\param[in]	pipeline	Le render pipeline.
		 *\param[in]	shadowMaps	Les shadow maps.
		 */
		C3D_API void initialiseAdditionalDescriptor( RenderPipeline & pipeline
			, ShadowMapLightTypeArray const & shadowMaps );
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
		C3D_API bool isValidPass( Pass const & pass )const;
		C3D_API bool isValidRenderable( RenderedObject const & object )const;
		C3D_API bool hasNodes()const;
		C3D_API bool isPassEnabled()const;
		C3D_API Scene & getScene()const;
		C3D_API SceneNode const * getIgnoredNode()const;

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

		uint32_t getPipelinesCount()const
		{
			return uint32_t( m_backPipelines.size()
				+ m_frontPipelines.size() );
		}

		bool isDirty()const
		{
			return m_isDirty;
		}

		bool forceTwoSided()const
		{
			return m_forceTwoSided;
		}

		RenderMode getRenderMode()const
		{
			return m_mode;
		}

		castor::String const & getTypeName()const
		{
			return m_typeName;
		}

		RenderPassTypeID getTypeID()const
		{
			return m_typeID;
		}
		/**@}*/

	private:
		void doSubInitialise();
		void doSubRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );

	protected:
		/**
		 *\~english
		 *\brief		Updates instantiated submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( SubmeshRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\~english
		 *\brief			Updates instantiated submeshes.
		 *\param[in]		nodes	The render nodes.
		 *\param[in, out]	info	Receives the render informations.
		 *\~french
		 *\brief			Met à jour des sous maillages instanciés.
		 *\param[in]		nodes	Les noeuds de rendu.
		 *\param[in,out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void doUpdate( SubmeshRenderNodesPtrByPipelineMap & nodes
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Updates non instantiated submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( SubmeshRenderNodePtrByPipelineMap & nodes );
		/**
		 *\~english
		 *\brief			Updates non instantiated submeshes.
		 *\param[in]		nodes	The render nodes.
		 *\param[in, out]	info	Receives the render informations.
		 *\~french
		 *\brief			Met à jour des sous maillages non instanciés.
		 *\param[in]		nodes	Les noeuds de rendu.
		 *\param[in,out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void doUpdate( SubmeshRenderNodePtrByPipelineMap & nodes
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Updates billboards.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Met à jour des billboards.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void doUpdate( BillboardRenderNodePtrByPipelineMap & nodes );
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
		C3D_API void doUpdate( BillboardRenderNodePtrByPipelineMap & nodes
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
		 *\brief			Fills the render pass specific descriptor layout bindings.
		 *\param[in,out]	bindings	Receives the additional bindings.
		 *\~french
		 *\brief			Remplit les attaches de layout de descripteurs spécifiques à une passe de rendu.
		 *\param[in,out]	bindings	Reçoit les attaches additionnelles.
		 */
		C3D_API virtual void doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const = 0;
		C3D_API virtual bool doAreValidPassFlags( PassFlags const & passFlags )const;
		C3D_API virtual bool doIsValidPass( Pass const & pass )const;
		C3D_API virtual bool doIsValidRenderable( RenderedObject const & object )const;
		C3D_API virtual PassFlags doAdjustPassFlags( PassFlags flags )const;
		C3D_API virtual ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const;
		C3D_API virtual SceneFlags doAdjustSceneFlags( SceneFlags flags )const;
		C3D_API ShaderProgramSPtr doGetProgram( PipelineFlags const & flags
			, VkCullModeFlags cullMode = VK_CULL_MODE_NONE );

	private:
		ashes::VkDescriptorSetLayoutBindingArray doCreateAdditionalBindings( PipelineFlags const & flags )const;
		std::vector< RenderPipelineUPtr > & doGetFrontPipelines();
		std::vector< RenderPipelineUPtr > & doGetBackPipelines();
		std::vector< RenderPipelineUPtr > const & doGetFrontPipelines()const;
		std::vector< RenderPipelineUPtr > const & doGetBackPipelines()const;
		RenderPipeline & doPreparePipeline( ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
			, PipelineFlags flags
			, VkCullModeFlags cullMode );
		void doUpdateFlags( PipelineFlags & flags )const;
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
		 *\brief			Initialises the additional descriptor set of a billboard node.
		 *\param[in,out]	descriptorWrites	Receives the descriptor writes.
		 *\param[in]		shadowMaps			The shadow maps.
		 *\~french
		 *\brief			Initialise l'ensemble de descripteurs additionnels pour un noeud de billboard.
		 *\param[in,out]	descriptorWrites	Reçoit les descriptor writes.
		 *\param[in]		shadowMaps			Les shadow maps.
		 */
		C3D_API virtual void doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps ) = 0;
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
		 *\brief		Retrieves the hull shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du hull shader correspondant aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetHullShaderSource( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the domain shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du domain shader correspondant aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetDomainShaderSource( PipelineFlags const & flags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const = 0;

	protected:
		RenderDevice const & m_device;
		RenderSystem & m_renderSystem;
		MatrixUbo & m_matrixUbo;
		SceneCuller & m_culler;
		castor::String m_typeName;
		RenderPassTypeID m_typeID{};
		RenderQueueUPtr m_renderQueue;
		castor::String m_category;
		castor::Size m_size;
		RenderMode m_mode{ RenderMode::eBoth };
		bool m_oit{ false };
		bool m_forceTwoSided{ false };
		bool m_safeBand{ false };
		bool m_isDirty{ true };
		SceneUbo m_sceneUbo;
		uint32_t m_index{ 0u };
		struct PassDescriptors
		{
			ashes::DescriptorSetPoolPtr pool;
			ashes::DescriptorSetLayoutPtr layout;
			ashes::DescriptorSetPtr set;
		};
		std::array< PassDescriptors, size_t( RenderNodeType::eCount ) > m_additionalDescriptors;

	private:
		std::vector< RenderPipelineUPtr > m_frontPipelines;
		std::vector< RenderPipelineUPtr > m_backPipelines;
	};
}

#endif
