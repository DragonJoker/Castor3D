/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VisibilityResolvePass_H___
#define ___C3D_VisibilityResolvePass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Render/RenderTechniquePass.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <RenderGraph/RunnablePass.hpp>

namespace castor3d
{
	class VisibilityResolvePass
		: public castor::Named
		, public RenderTechniquePass
		, public crg::RunnablePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	parent				The parent technique.
		 *\param[in]	pass				The parent frame pass.
		 *\param[in]	context				The rendering context.
		 *\param[in]	graph				The runnable graph.
		 *\param[in]	device				The GPU device.
		 *\param[in]	category			The pass category name.
		 *\param[in]	name				The pass name.
		 *\param[in]	nodesPass			The visibility nodes pass.
		 *\param[in]	pipelinesIds		The pipelines IDs buffer.
		 *\param[in]	renderPassDesc		The scene render pass construction data.
		 *\param[in]	techniquePassDesc	The technique render pass construction data.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent				La technique parente.
		 *\param[in]	pass				La frame pass parente.
		 *\param[in]	context				Le contexte de rendu.
		 *\param[in]	graph				Le runnable graph.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	category			Le nom de la catégorie de la passe.
		 *\param[in]	name				Le nom de la passe.
		 *\param[in]	nodesPass			La passe de visibilité des noeuds.
		 *\param[in]	pipelinesIds		Le buffer de pipeline IDs.
		 *\param[in]	renderPassDesc		Les données de construction de passe de rendu de scène.
		 *\param[in]	techniquePassDesc	Les données de construction de passe de rendu de technique.
		 */
		C3D_API VisibilityResolvePass( RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::String const & category
			, castor::String const & name
			, RenderNodesPass const & nodesPass
			, ShaderBuffer * pipelinesIds
			, RenderNodesPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniquePassDesc );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::update
		 */
		C3D_API void update( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::createPipelineFlags
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
			, uint32_t passLayerIndex
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets )const override;
		/**
		 *\copydoc	RenderNodesPass::areValidPassFlags
		 */
		C3D_API bool areValidPassFlags( PassComponentCombine const & passFlags )const override;
		/**
		 *\copydoc		castor3d::RenderNodesPass::getShaderFlags
		 */
		C3D_API ShaderFlags getShaderFlags()const override;
		/**
		 *\copydoc	RenderNodesPass::isPassEnabled
		 */
		C3D_API bool isPassEnabled()const override;
		/**
		 *\copydoc		castor3d::RenderNodesPass::getComponentsMask
		 */
		C3D_API static ComponentModeFlags getComponentsMask();

		static constexpr bool useCompute{ false };

	private:
		struct Pipeline
		{
			struct ShaderStages
			{
				ShaderModule shader;
				ashes::PipelinePtr pipeline{};
			};
			ashes::DescriptorSetLayoutPtr descriptorLayout{};
			ashes::PipelineLayoutPtr pipelineLayout{};
			ashes::DescriptorSetPoolPtr descriptorPool{};
			std::array< ShaderStages, 2u > shaders;
			std::unordered_map< size_t, ashes::DescriptorSetPtr > descriptorSets{};
		};
		struct PipelineNodesDescriptors
		{
			uint32_t pipelineId{};
			ashes::DescriptorSet const * descriptorSet{};
		};
		using PipelinePtr = std::unique_ptr< Pipeline >;
		using SubmeshPipelinesNodesDescriptors = std::map< ashes::DescriptorSet const *, uint32_t >;
		using SubmeshPipelinesMap = std::map< Pipeline const *, SubmeshPipelinesNodesDescriptors >;
		using BillboardPipelinesNodesDescriptors = std::map< uint32_t, PipelineNodesDescriptors >;
		using BillboardPipelinesMap = std::map< Pipeline const *, BillboardPipelinesNodesDescriptors >;
		using PipelineContainer = std::map< PipelineBaseHash, PipelinePtr >;

	private:
		bool doIsEnabled()const;
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		void doRecordCompute( crg::RecordContext & context
			, VkCommandBuffer commandBuffer );
		void doRecordGraphics( crg::RecordContext & context
			, VkCommandBuffer commandBuffer );
		Pipeline & doCreatePipeline( PipelineBaseHash const & hash
			, PipelineFlags const & flags );
		Pipeline & doCreatePipeline( PipelineBaseHash const & hash
			, PipelineFlags const & flags
			, uint32_t stride );
		Pipeline & doCreatePipeline( PipelineBaseHash const & hash
			, PipelineFlags const & flags
			, uint32_t stride
			, PipelineContainer & pipelines );

	private:
		RenderDevice const & m_device;
		RenderNodesPass const & m_nodesPass;
		ShaderBuffer * m_pipelinesIds;
		CameraUbo const & m_cameraUbo;
		SceneUbo const & m_sceneUbo;
		PassSortNodesSignalConnection m_onNodesPassSort;
		ashes::DescriptorSetLayoutPtr m_inOutsDescriptorLayout{};
		ashes::DescriptorSetPoolPtr m_inOutsDescriptorPool{};
		ashes::DescriptorSetPtr m_inOutsDescriptorSet{};
		bool m_commandsChanged{};
		ShaderModule m_vertexShader;
		ashes::RenderPassPtr m_firstRenderPass;
		ashes::FrameBufferPtr m_firstFramebuffer;
		ashes::RenderPassPtr m_blendRenderPass;
		ashes::FrameBufferPtr m_blendFramebuffer;
		PipelineContainer m_pipelines;
		PipelineContainer m_billboardPipelines;
		SubmeshPipelinesMap m_activePipelines;
		BillboardPipelinesMap m_activeBillboardPipelines;
	};
}

#endif
