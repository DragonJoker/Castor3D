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
			: m_size{ std::move( size ) }
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
			: RenderNodesPassDesc{ std::move( size )
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
			: RenderNodesPassDesc{ std::move( size )
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
			: RenderNodesPassDesc{ std::move( size )
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
			: RenderNodesPassDesc{ std::move( size )
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
			m_ruConfig.implicitAction( view, action );
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
			m_componentModeFlags = std::move( value );
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
	};

	class RenderNodesPass
		: public castor::OwnedBy< Engine >
		, public crg::RenderPass
		, public castor::Named
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
		 *\brief		Retrieves the task shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du task shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API ShaderPtr getTaskShaderSource( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Retrieves the mesh shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du mesh shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API ShaderPtr getMeshShaderSource( PipelineFlags const & flags )const;
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
		C3D_API SubmeshFlags adjustFlags( SubmeshFlags flags )const;
		/**
		 *\~english
		 *\brief		Adjusts given flags to match the render pass requirements.
		 *\param[in]	flags	The flags.
		 *\~french
		 *\brief		Ajuste les flags donnés pour qu'ils correspondent aux pré-requis de la passe.
		 *\param[in]	flags	Les flags.
		 */
		C3D_API PassComponentCombine adjustFlags( PassComponentCombine flags )const;
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
		 *\param[in]	texturesFlags	The textures flags.
		 *\return		The filtered flags.
		 *\~french
		 *\brief		Filtre les indicateurs de textures donnés en utilisant ceux voulus par cette passe.
		 *\param[in]	texturesFlags	Les indicateurs de textures.
		 *\return		Les indicateurs filtrés.
		 */
		C3D_API TextureCombine adjustFlags( TextureCombine texturesFlags )const;
		/**
		 *\~english
		 *\brief		Creates the pipeline flags for given configuration.
		 *\param[in]	components			The components combination.
		 *\param[in]	colourBlendMode		The colour blending mode.
		 *\param[in]	alphaBlendMode		The alpha blending mode.
		 *\param[in]	renderPassTypeId	The render pass type ID.
		 *\param[in]	lightingModelId		The lighting model ID.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\param[in]	alphaFunc			The alpha comparison function (for opaque nodes).
		 *\param[in]	blendAlphaFunc		The alpha comparison function (for transparent nodes).
		 *\param[in]	textures			The textures configuration.
		 *\param[in]	submeshFlags		A combination of SubmeshFlag.
		 *\param[in]	programFlags		A combination of ProgramFlag.
		 *\param[in]	sceneFlags			Scene related flags.
		 *\param[in]	topology			The render topology.
		 *\param[in]	isFrontCulled		\p true for front face culling, \p false for back face culling.
		 *\param[in]	invertNormals		\p true to invert normals.
		 *\param[in]	passLayerIndex		The material pass layer index.
		 *\param[in]	morphTargets		The morph targets buffer.
		 *\~french
		 *\brief		Crée les indicateurs de pipeline pour la configuration donnée.
		 *\param[in]	components			La combinaison de composants.
		 *\param[in]	colourBlendMode		Le mode de mélange de couleurs.
		 *\param[in]	alphaBlendMode		Le mode de mélange de l'alpha
		 *\param[in]	renderPassTypeId	L'ID du type de render pass.
		 *\param[in]	lightingModelId		L'ID du modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 *\param[in]	alphaFunc			La fonction de comparaison de l'alpha (pour les noeuds opaques).
		 *\param[in]	blendAlphaFunc		La fonction de comparaison de l'alpha (pour les noeuds transparents).
		 *\param[in]	textures			La configuration des textures.
		 *\param[in]	submeshFlags		Une combinaison de SubmeshFlag.
		 *\param[in]	programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags			Les indicateurs relatifs à la scène.
		 *\param[in]	topology			La topologie de rendu.
		 *\param[in]	isFrontCulled		\p true pour front face culling, \p false pour back face culling.
		 *\param[in]	invertNormals		\p true pour inverser les normales.
		 *\param[in]	passLayerIndex		L'indice de la couche de la passe de matériau.
		 *\param[in]	morphTargets		Le buffer de morph targets.
		 */
		C3D_API PipelineFlags createPipelineFlags( PassComponentCombine components
			, BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, RenderPassTypeID renderPassTypeId
			, LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId
			, VkCompareOp alphaFunc
			, VkCompareOp blendAlphaFunc
			, TextureCombine const & textures
			, SubmeshFlags const & submeshFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, bool isFrontCulled
			, bool invertNormals
			, uint32_t passLayerIndex
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets )const;
		/**
		 *\~english
		 *\brief		Creates the pipeline flags for given configuration.
		 *\param[in]	pass			The pass for which the pipeline is created.
		 *\param[in]	textures		The textures configuration.
		 *\param[in]	submeshFlags	A combination of SubmeshFlag.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	topology		The render topology.
		 *\param[in]	isFrontCulled	\p true for front face culling, \p false for back face culling.
		 *\param[in]	morphTargets	The morph targets buffer.
		 *\~french
		 *\brief		Crée les indicateurs de pipeline pour la configuration donnée.
		 *\param[in]	pass			La passe pour laquelle le pipeline est créé.
		 *\param[in]	textures		La configuration des textures.
		 *\param[in]	submeshFlags	Une combinaison de SubmeshFlag.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	topology		La topologie de rendu.
		 *\param[in]	isFrontCulled	\p true pour front face culling, \p false pour back face culling.
		 *\param[in]	morphTargets	Le buffer de morph targets.
		 */
		C3D_API PipelineFlags createPipelineFlags( Pass const & pass
			, TextureCombine const & textures
			, SubmeshFlags const & submeshFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, bool isFrontCulled
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets )const;
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
		C3D_API RenderPipeline & prepareBackPipeline( PipelineFlags pipelineFlags
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
		C3D_API RenderPipeline & prepareFrontPipeline( PipelineFlags pipelineFlags
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
			, ashes::DescriptorSetLayout const * meshletDescriptorLayout );
		/**
		 *\~english
		 *\brief		Initialises the additional descriptor set.
		 *\param[in]	pipeline		The render pipeline.
		 *\param[in]	shadowMaps		The shadow maps.
		 *\param[in]	morphTargets	The morph targets buffer.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs additionnels.
		 *\param[in]	pipeline		Le render pipeline.
		 *\param[in]	shadowMaps		Les shadow maps.
		 *\param[in]	morphTargets	Le buffer de morph targets.
		 */
		C3D_API void initialiseAdditionalDescriptor( RenderPipeline & pipeline
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets );
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
		C3D_API void countNodes( RenderInfo & info )const;
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
		 *\brief			Adds shadow maps descriptor layout bindings to given list.
		 *\param[in]		sceneFlags	Used to define what shadow maps need to be bound.
		 *\param[in,out]	bindings	Receives the bindings.
		 *\param[in,out]	index		The current binding index.
		 *\return
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout des shadow maps à la liste donnée.
		 *\param[in]		sceneFlags	Le mode de mélange couleurs.
		 *\param[in,out]	bindings	Reçoit les bindings.
		 *\param[in,out]	index		L'index de binding actuel.
		 *\return
		 */
		C3D_API static void addShadowBindings( SceneFlags const & sceneFlags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds shadow maps descriptor layout bindings to given list.
		 *\param[in,out]	bindings	Receives the bindings.
		 *\param[in,out]	index		The current binding index.
		 *\return
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout des shadow maps à la liste donnée.
		 *\param[in,out]	bindings	Reçoit les bindings.
		 *\param[in,out]	index		L'index de binding actuel.
		 *\return
		 */
		C3D_API static void addShadowBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds background descriptor layout bindings to given list.
		 *\param[in]		background	The background.
		 *\param[in,out]	bindings	Receives the bindings.
		 *\param[in,out]	index		The current binding index.
		 *\return
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout du background à la liste donnée.
		 *\param[in]		background	Le fond.
		 *\param[in,out]	bindings	Reçoit les bindings.
		 *\param[in,out]	index		L'index de binding actuel.
		 *\return
		 */
		C3D_API static void addBackgroundBindings( SceneBackground const & background
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds indirect lighting descriptor layout bindings to given list.
		 *\param[in]		llpvResult	The Layered LPV result.
		 *\param[in,out]	bindings	Receives the bindings.
		 *\param[in,out]	index		The current binding index.
		 *\return
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout de l'éclairage indirect à la liste donnée.
		 *\param[in]		llpvResult	Le résultat du Layered LPV.
		 *\param[in,out]	bindings	Reçoit les bindings.
		 *\param[in,out]	index		L'index de binding actuel.
		 *\return
		 */
		C3D_API static void addGIBindings( IndirectLightingData const & indirectLighting
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds indirect lighting descriptor layout bindings to given list.
		 *\param[in]		sceneFlags	The scene flags.
		 *\param[in]		llpvResult	The Layered LPV result.
		 *\param[in,out]	bindings	Receives the bindings.
		 *\param[in,out]	index		The current binding index.
		 *\return
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout de l'éclairage indirect à la liste donnée.
		 *\param[in]		sceneFlags	Les indicateurs de scène.
		 *\param[in]		llpvResult	Le résultat du Layered LPV.
		 *\param[in,out]	bindings	Reçoit les bindings.
		 *\param[in,out]	index		L'index de binding actuel.
		 *\return
		 */
		C3D_API static void addGIBindings( SceneFlags flags
			, IndirectLightingData const & indirectLighting
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds clusters descriptor layout bindings to given list.
		 *\param[in]		frustumClusters	The clusters.
		 *\param[in,out]	bindings		Receives the bindings.
		 *\param[in,out]	index			The current binding index.
		 *\return
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout des clusters à la liste donnée.
		 *\param[in]		frustumClusters	Les clusters.
		 *\param[in,out]	bindings		Reçoit les bindings.
		 *\param[in,out]	index			L'index de binding actuel.
		 *\return
		 */
		C3D_API static void addClusteredLightingBindings( FrustumClusters const & frustumClusters
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
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
		C3D_API static void addGIDescriptor( IndirectLightingData const & indirectLighting
			, ashes::WriteDescriptorSetArray & descriptorWrites
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
		C3D_API bool areValidPassFlags( PassComponentCombine const & passFlags )const;
		C3D_API virtual bool isPassEnabled()const;
		C3D_API virtual ShaderFlags getShaderFlags()const;
		C3D_API bool isValidPass( Pass const & pass )const;
		C3D_API bool isValidRenderable( RenderedObject const & object )const;
		C3D_API bool isValidNode( SceneNode const & node )const;
		C3D_API bool hasNodes()const;
		C3D_API Scene & getScene()const;
		C3D_API SceneNode const * getIgnoredNode()const;
		C3D_API bool isMeshShading()const;
		C3D_API NodePtrByPipelineMapT< SubmeshRenderNode > const & getSubmeshNodes()const;
		C3D_API ObjectNodesPtrByPipelineMapT< SubmeshRenderNode > const & getInstancedSubmeshNodes()const;
		C3D_API NodePtrByPipelineMapT< BillboardRenderNode > const & getBillboardNodes()const;
		C3D_API std::pair< uint32_t, uint32_t > fillPipelinesIds( castor::ArrayView< uint32_t > nodesPipelinesIds )const;
		C3D_API PipelineBufferArray const & getPassPipelineNodes()const;
		C3D_API uint32_t getPipelineNodesIndex( PipelineBaseHash const & hash
			, ashes::BufferBase const & buffer )const;
		C3D_API uint32_t getDrawCallsCount()const;

		ComponentModeFlags getComponentsMask()const
		{
			return m_componentsMask;
		}

		bool isOrderIndependent()const
		{
			return m_oit;
		}

		SceneCuller & getCuller()const
		{
			return m_culler;
		}

		CameraUbo const & getMatrixUbo()const
		{
			return m_cameraUbo;
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

		RenderFilters getRenderFilters()const
		{
			return m_filters;
		}

		castor::String const & getTypeName()const
		{
			return m_typeName;
		}

		RenderPassTypeID getTypeID()const
		{
			return m_typeID;
		}

		bool filtersStatic()const
		{
			return handleStatic()
				&& !*m_handleStatic;
		}

		bool filtersNonStatic()const
		{
			return handleStatic()
				&& *m_handleStatic;
		}

		bool handleStatic()const
		{
			return m_handleStatic != std::nullopt;
		}
		/**@}*/

		mutable PassSortNodesSignal onSortNodes;

	private:
		void doSubInitialise();
		void doSubRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );

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
		C3D_API virtual bool doIsValidPass( Pass const & pass )const;
		/**
		 *\~english
		 *\param[in]	object	The rendered object.
		 *\return		\p true if the object is rendered through this nodes pass.
		 *\~french
		 *\param[in]	object	L'objet rendu.
		 *\return		\p true si l'objet est rendu via cette passe de noeuds.
		 */
		C3D_API virtual bool doIsValidRenderable( RenderedObject const & object )const;
		C3D_API virtual SubmeshFlags doAdjustSubmeshFlags( SubmeshFlags flags )const;
		C3D_API virtual ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const;
		C3D_API virtual SceneFlags doAdjustSceneFlags( SceneFlags flags )const;
		C3D_API ShaderProgramRPtr doGetProgram( PipelineFlags const & flags
			, VkCullModeFlags cullMode = VK_CULL_MODE_NONE );
		C3D_API void doUpdateFlags( PipelineFlags & flags )const;
		C3D_API void doAddShadowBindings( Scene const & scene
			, PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddBackgroundBindings( Scene const & scene
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddClusteredLightingBindings( RenderTarget const & target
			, PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddShadowDescriptor( Scene const & scene
			, PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, uint32_t & index )const;
		C3D_API void doAddBackgroundDescriptor( Scene const & scene
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const;
		C3D_API void doAddClusteredLightingDescriptor( RenderTarget const & target
			, PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index )const;

	private:
		ashes::VkDescriptorSetLayoutBindingArray doCreateAdditionalBindings( PipelineFlags const & flags )const;
		std::vector< RenderPipelineUPtr > & doGetFrontPipelines();
		std::vector< RenderPipelineUPtr > & doGetBackPipelines();
		std::vector< RenderPipelineUPtr > const & doGetFrontPipelines()const;
		std::vector< RenderPipelineUPtr > const & doGetBackPipelines()const;
		RenderPipeline & doPreparePipeline( ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
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
		 *\~french
		 *\brief			Initialise l'ensemble de descripteurs additionnels.
		 *\param[in]		flags				Les indicateurs de pipeline.
		 *\param[in,out]	descriptorWrites	Reçoit les descriptor writes.
		 *\param[in]		shadowMaps			Les shadow maps.
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
		 *\~french
		 *\brief		Récupère le source du task shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetTaskShaderSource( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Retrieves the mesh shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du mesh shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetMeshShaderSource( PipelineFlags const & flags )const;
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
		C3D_API virtual ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Retrieves the hull shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du hull shader correspondant aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetHullShaderSource( PipelineFlags const & flags )const;
		/**
		 *\~english
		 *\brief		Retrieves the domain shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du domain shader correspondant aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		C3D_API virtual ShaderPtr doGetDomainShaderSource( PipelineFlags const & flags )const;
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
		CameraUbo const & m_cameraUbo;
		SceneCuller & m_culler;
		crg::ImageViewIdArray m_targetImage;
		crg::ImageViewIdArray m_targetDepth;
		castor::String m_typeName;
		RenderPassTypeID m_typeID{};
		RenderFilters m_filters{ RenderFilter::eNone };
		RenderQueueUPtr m_renderQueue;
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
		ComponentModeFlags m_componentsMask;
		bool m_allowClusteredLighting;

	private:
		struct PassDescriptors
		{
			ashes::DescriptorSetPoolPtr pool;
			ashes::DescriptorSetLayoutPtr layout;
			ashes::DescriptorSetPtr set;
		};
		using PassDescriptorsMap = std::map< size_t, PassDescriptors >;

		PassDescriptorsMap m_additionalDescriptors;
		std::vector< RenderPipelineUPtr > m_frontPipelines;
		std::vector< RenderPipelineUPtr > m_backPipelines;
	};

	struct IsRenderPassEnabled
	{
		explicit IsRenderPassEnabled( RenderNodesPass const & pass )
			: m_pass{ &pass }
		{}

		IsRenderPassEnabled()
			: m_pass{}
		{}

		void setPass( RenderNodesPass const & pass )
		{
			m_pass = &pass;
		}

		bool operator()()const
		{
			return m_pass
				&& m_pass->isPassEnabled();
		}

		RenderNodesPass const * m_pass;
	};
	CU_DeclareSmartPtr( castor3d, IsRenderPassEnabled, C3D_API );
}

#endif
