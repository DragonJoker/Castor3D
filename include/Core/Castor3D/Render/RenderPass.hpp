/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPass_H___
#define ___C3D_RenderPass_H___

#include "RenderModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
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
	struct SceneRenderPassDesc
	{
		SceneRenderPassDesc( VkExtent3D size
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
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The scene culler for this pass.
		 *\param[in]	instanceMult	The object instance multiplier.
		 *\~french
		 *\brief		Constructeur pour les passes opaques.
		 *\param[in]	matrixUbo		L'UBO des matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	instanceMult	Le multiplicateur d'instances d'objets.
		 */
		SceneRenderPassDesc( VkExtent3D size
			, MatrixUbo & matrixUbo
			, SceneCuller & culler )
			: SceneRenderPassDesc{ std::move( size )
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
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The scene culler for this pass.
		 *\param[in]	oit				The order independant status.
		 *\param[in]	instanceMult	The object instance multiplier.
		 *\~french
		 *\brief		Constructeur pour les passes transparents.
		 *\param[in]	matrixUbo		L'UBO des matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	oit				Le statut de rendu indépendant de l'ordre des objets.
		 *\param[in]	instanceMult	Le multiplicateur d'instances d'objets.
		 */
		SceneRenderPassDesc( VkExtent3D size
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, bool oit )
			: SceneRenderPassDesc{ std::move( size )
				, matrixUbo
				, culler
				, RenderMode::eTransparentOnly
				, oit
				, false }
		{
		}
		/**
		 *\~english
		 *\param[in]	value	The object instance multiplier.
		 *\~french
		 *\param[in]	value	Le multiplicateur d'instances d'objets.
		 */
		SceneRenderPassDesc & instanceMult( uint32_t value )
		{
			m_instanceMult = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The scene node to ignore during rendering.
		 *\~french
		 *\param[in]	value	Le scene node à ignorer pendant le rendu.
		 */
		SceneRenderPassDesc & ignored( SceneNode const & value )
		{
			m_ignored = &value;
			return *this;
		}

		VkExtent3D m_size;
		MatrixUbo & m_matrixUbo;
		SceneCuller & m_culler;
		RenderMode m_mode;
		bool m_oit;
		bool m_forceTwoSided;
		SceneNode const * m_ignored{};
		uint32_t m_index{ 0u };
		uint32_t m_instanceMult{ 1u };
	};

	class SceneRenderPass
		: public castor::OwnedBy< Engine >
		, public castor::Named
		, public crg::RenderPass
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device		The render device.
		 *\param[in]	category	The pass category.
		 *\param[in]	name		The pass name.
		 *\param[in]	desc		The construction data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device		Le render device.
		 *\param[in]	category	La catégorie de la passe.
		 *\param[in]	name		Le nom de la passe.
		 *\param[in]	desc		Les données de construction.
		 */
		C3D_API SceneRenderPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::String const & category
			, castor::String const & name
			, SceneRenderPassDesc const & desc );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~SceneRenderPass();

	public:
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
		 *\param[in]		pass				The pass for whic the pipeline is created.
		 *\param[in,out]	textures			The textures configuration.
		 *\param[in,out]	programFlags		A combination of ProgramFlag.
		 *\param[in,out]	sceneFlags			Scene related flags.
		 *\param[in]		topology			The render topology.
		 *\param[in]		vertexLayouts		The vertex buffers layouts.
		 *\param[in]		descriptorLayouts	The render node descriptor set layouts.
		 *\~french
		 *\brief			Prépare le pipeline qui correspond aux indicateurs donnés, pour les noeuds en back face culling.
		 *\param[in]		pass				La passe pour laquelle le pipeline est créé.
		 *\param[in,out]	textures			La configuration des textures.
		 *\param[in,out]	programFlags		Une combinaison de ProgramFlag.
		 *\param[in,out]	sceneFlags			Les indicateurs relatifs à la scène.
		 *\param[in]		topology			La topologie de rendu.
		 *\param[in]		vertexLayouts		Les layouts des tampons de sommets.
		 *\param[in]		descriptorLayouts	Les layouts des descriptor sets de noeud de rendu.
		 */
		C3D_API RenderPipeline * prepareBackPipeline( Pass const & pass
			, TextureFlagsArray const & textures
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
			, ashes::DescriptorSetLayoutCRefArray descriptorLayouts );
		/**
		 *\~english
		 *\brief			Prepares the pipeline matching the given flags, for front face culling nodes.
		 *\param[in]		pass				The pass for whic the pipeline is created.
		 *\param[in,out]	textures			The textures configuration.
		 *\param[in,out]	programFlags		A combination of ProgramFlag.
		 *\param[in,out]	sceneFlags			Scene related flags.
		 *\param[in]		topology			The render topology.
		 *\param[in]		vertexLayouts		The vertex buffers layouts.
		 *\param[in]		descriptorLayouts	The render node descriptor set layouts.
		 *\~french
		 *\brief			Prépare le pipeline qui correspond aux indicateurs donnés, pour les noeuds en front face culling.
		 *\param[in]		pass				La passe pour laquelle le pipeline est créé.
		 *\param[in,out]	textures			La configuration des textures.
		 *\param[in,out]	programFlags		Une combinaison de ProgramFlag.
		 *\param[in,out]	sceneFlags			Les indicateurs relatifs à la scène.
		 *\param[in]		topology			La topologie de rendu.
		 *\param[in]		vertexLayouts		Les layouts des tampons de sommets.
		 *\param[in]		descriptorLayouts	Les layouts des descriptor sets de noeud de rendu.
		 */
		C3D_API RenderPipeline * prepareFrontPipeline( Pass const & pass
			, TextureFlagsArray const & textures
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
			, ashes::DescriptorSetLayoutCRefArray descriptorLayouts );
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
		C3D_API SubmeshRenderNode * createSkinningNode( Pass & pass
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
		C3D_API SubmeshRenderNode * createMorphingNode( Pass & pass
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
		C3D_API SubmeshRenderNode * createStaticNode( Pass & pass
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
		C3D_API BillboardRenderNode * createBillboardNode( Pass & pass
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
		 *\brief		Initialises the additional descriptor set of a billboard node.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	node			The node.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs additionnels pour un noeud de billboard.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	node			Le noeud.
		 */
		C3D_API void initialiseAdditionalDescriptor( RenderPipeline & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
			, BillboardRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps );
		/**
		 *\~english
		 *\brief		Initialises the additional descriptor set of a submesh node.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	node			The node.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs additionnels pour un noeud de submesh.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	node			Le noeud.
		 */
		C3D_API void initialiseAdditionalDescriptor( RenderPipeline & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
			, SubmeshRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps );
		/**
		 *\~english
		 *\brief		Initialises the additional descriptor set of instantiated submesh nodes.
		 *\param[in]	descriptorPool	The pool.
		 *\param[in]	nodes			The nodes.
		 *\~french
		 *\brief		Initialise l'ensemble de descripteurs additionnels pour des noeuds de submesh instanciés.
		 *\param[in]	descriptorPool	Le pool.
		 *\param[in]	nodes			Les noeuds.
		 */
		C3D_API void initialiseAdditionalDescriptor( RenderPipeline & pipeline
			, ashes::DescriptorSetPool const & descriptorPool
			, SubmeshRenderNodesByPassMap & nodes
			, ShadowMapLightTypeArray const & shadowMaps );
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	flags	The pipeline flags.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	flags	Les indicateurs de pipeline.
		 */
		C3D_API void updateFlags( PipelineFlags & flags
			, VkCullModeFlags cullMode )const;
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
		C3D_API bool isValidPass( Pass const & pass )const;

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

		RenderMode getRenderMode()const
		{
			return m_mode;
		}
		/**@}*/

	protected:
		C3D_API void doSubInitialise()override;
		C3D_API void doSubRecordInto( VkCommandBuffer commandBuffer
			, uint32_t index )override;
		C3D_API  VkSubpassContents doGetSubpassContents()const override
		{
			return VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
		}

	protected:
		/**
		 *\~english
		 *\brief		Copies the instantiated nodes model matrices into given matrix buffer.
		 *\param[in]	renderNodes		The instantiated nodes.
		 *\param[in]	matrixBuffer	The matrix buffer.
		 *\~french
		 *\brief		Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\param[in]	renderNodes		Les noeuds instanciés.
		 *\param[in]	matrixBuffer	Le tampon de matrices.
		 */
		C3D_API uint32_t doCopyNodesMatrices( SubmeshRenderNodePtrArray const & renderNodes
			, std::vector< InstantiationData > & matrixBuffer )const;
		/**
		 *\~english
		 *\brief			Copies the instantiated nodes model matrices into given matrix buffer.
		 *\remarks			The nodes which are copied will be registered in the rendered nodes list.
		 *\param[in]		renderNodes		The instantiated nodes.
		 *\param[in]		matrixBuffer	The matrix buffer.
		 *\param[in, out]	info			Receives the render informations.
		 *\~french
		 *\brief			Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\remarks			Les noeuds pour lesquels les matrices sont copiées seront enregistrés dans la liste des noeuds dessinés.
		 *\param[in]		renderNodes		Les noeuds instanciés.
		 *\param[in]		matrixBuffer	Le tampon de matrices.
		 *\param[in,out]	info			Reçoit les informations de rendu.
		 */
		C3D_API uint32_t doCopyNodesMatrices( SubmeshRenderNodePtrArray const & renderNodes
			, std::vector< InstantiationData > & matrixBuffer
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Copies the instantiated skinned nodes model matrices into given matrix buffer.
		 *\param[in]	renderNodes	The instantiated nodes.
		 *\param[in]	bonesBuffer	The bones matrix buffer.
		 *\~french
		 *\brief		Copie les matrices de noeuds skinnés instanciés dans le tampon de matrices donné.
		 *\param[in]	renderNodes	Les noeuds instanciés.
		 *\param[in]	bonesBuffer	Le tampon de matrices des os.
		 */
		C3D_API uint32_t doCopyNodesBones( SubmeshRenderNodePtrArray const & renderNodes
			, ShaderBuffer & bonesBuffer )const;
		/**
		 *\~english
		 *\brief			Copies the instantiated skinned nodes model matrices into given matrix buffer.
		 *\remarks			The nodes which are copied will be registered in the rendered nodes list.
		 *\param[in]		renderNodes	The instantiated nodes.
		 *\param[in]		bonesBuffer	The bones matrix buffer.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Copie les matrices de noeuds skinnés instanciés dans le tampon de matrices donné.
		 *\remarks			Les noeuds pour lesquels les matrices sont copiées seront enregistrés dans la liste des noeuds dessinés.
		 *\param[in]		renderNodes	Les noeuds instanciés.
		 *\param[in]		bonesBuffer	Le tampon de matrices des os.
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API uint32_t doCopyNodesBones( SubmeshRenderNodePtrArray const & renderNodes
			, ShaderBuffer & bonesBuffer
			, RenderInfo & info )const;
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
		 *\param[in]		flags		The pipeline flags.
		 *\param[in,out]	bindings	Receives the additional bindings.
		 *\~french
		 *\brief			Remplit les attaches de layout de descripteurs spécifiques à une passe de rendu.
		 *\param[in]		flags		Les indicateurs de pipeline.
		 *\param[in,out]	bindings	Reçoit les attaches additionnelles.
		 */
		C3D_API virtual void doFillAdditionalBindings( PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const = 0;
		C3D_API virtual bool doIsValidPass( PassFlags const & passFlags )const;
		C3D_API ShaderProgramSPtr doGetProgram( PipelineFlags & flags
			, VkCullModeFlags cullMode = VK_CULL_MODE_NONE );

	private:
		ashes::VkDescriptorSetLayoutBindingArray doCreateAdditionalBindings( PipelineFlags const & flags )const;
		std::vector< RenderPipelineUPtr > & doGetFrontPipelines();
		std::vector< RenderPipelineUPtr > & doGetBackPipelines();
		std::vector< RenderPipelineUPtr > const & doGetFrontPipelines()const;
		std::vector< RenderPipelineUPtr > const & doGetBackPipelines()const;
		RenderPipeline * doPreparePipeline( ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
			, ashes::DescriptorSetLayoutCRefArray descriptorLayouts
			, PipelineFlags & flags
			, VkCullModeFlags cullMode );
		SubmeshRenderNode * doCreateSubmeshNode( Pass & pass
			, RenderPipeline & pipeline
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedMesh * mesh
			, AnimatedSkeleton * skeleton );
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
		 *\param[in]		layout		The descriptors layout.
		 *\param[in,out]	index		The texture index, updated to the next available.
		 *\param[in]		node		The node.
		 *\param[in]		shadowMaps	The shadow maps.
		 *\~french
		 *\brief			Initialise l'ensemble de descripteurs additionnels pour un noeud de billboard.
		 *\param[in]		layout		Le layout des descripteurs.
		 *\param[in, out]	index		L'indice de la texture, mis à jour au prochain disponible.
		 *\param[in]		node		Le noeud.
		 *\param[in]		shadowMaps	Les shadow maps.
		 */
		C3D_API virtual void doFillAdditionalDescriptor( RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, BillboardRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps ) = 0;
		/**
		 *\~english
		 *\brief			Initialises the additional descriptor set of a submesh node.
		 *\param[in]		layout		The descriptors layout.
		 *\param[in,out]	index		The texture index, updated to the next available.
		 *\param[in]		node		The node.
		 *\param[in]		shadowMaps	The shadow maps.
		 *\~french
		 *\brief			Initialise l'ensemble de descripteurs additionnels pour un noeud de submesh.
		 *\param[in]		layout		Le layout des descripteurs.
		 *\param[in, out]	index		L'indice de la texture, mis à jour au prochain disponible.
		 *\param[in]		node		Le noeud.
		 *\param[in]		shadowMaps	Les shadow maps.
		 */
		C3D_API virtual void doFillAdditionalDescriptor( RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, SubmeshRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps ) = 0;
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
		castor::Size m_size;
		RenderMode m_mode{ RenderMode::eBoth };
		bool m_oit{ false };
		bool m_forceTwoSided{ false };
		bool m_isDirty{ true };
		SceneUbo m_sceneUbo;
		uint32_t m_index{ 0u };
		uint32_t const m_instanceMult{ 1u };
		std::map< size_t, UniformBufferOffsetT< ModelInstancesUboConfiguration > > m_modelsInstances;

	private:
		std::vector< RenderPipelineUPtr > m_frontPipelines;
		std::vector< RenderPipelineUPtr > m_backPipelines;
	};
}

#endif
