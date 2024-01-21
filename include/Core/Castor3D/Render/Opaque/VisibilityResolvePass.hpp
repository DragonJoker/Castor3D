/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VisibilityResolvePass_H___
#define ___C3D_VisibilityResolvePass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Render/RenderTechniquePass.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <ashespp/Pipeline/Pipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

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
		 *\param[in]	targetImage			The image this pass renders to.
		 *\param[in]	targetDepth			The depth image this pass renders to.
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
		 *\param[in]	targetImage			L'image dans laquelle cette passe fait son rendu.
		 *\param[in]	targetDepth			L'image de profondeur dans laquelle cette passe fait son rendu.
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
			, crg::ImageViewIdArray targetImage
			, crg::ImageViewIdArray targetDepth
			, RenderNodesPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniquePassDesc );
		/**
		 *\copydoc	castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\copydoc	castor3d::RenderTechniquePass::update
		 */
		C3D_API void update( CpuUpdater & updater )override;
		/**
		 *\copydoc	castor3d::RenderTechniquePass::createPipelineFlags
		 */
		C3D_API PipelineFlags createPipelineFlags( PassComponentCombine passComponents
			, SubmeshComponentCombine submeshComponents
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
			, SubmeshRenderData * submeshData )const override;
		/**
		 *\copydoc	castor3d::RenderNodesPass::areValidPassFlags
		 */
		C3D_API bool areValidPassFlags( PassComponentCombine const & passFlags )const noexcept override;
		/**
		 *\copydoc	castor3d::RenderNodesPass::getShaderFlags
		 */
		C3D_API ShaderFlags getShaderFlags()const noexcept override;
		/**
		 *\copydoc	castor3d::RenderNodesPass::countNodes
		 */
		C3D_API void countNodes( RenderInfo & info )const noexcept override;
		/**
		 *\copydoc	castor3d::RenderNodesPass::isPassEnabled
		 */
		C3D_API bool isPassEnabled()const noexcept override;
		/**
		 *\copydoc	castor3d::RenderNodesPass::getDeferredLightingFilter
		 */
		DeferredLightingFilter getDeferredLightingFilter()const noexcept
		{
			return m_deferredLightingFilter;
		}
		/**
		 *\copydoc	castor3d::RenderNodesPass::getParallaxOcclusionFilter
		 */
		ParallaxOcclusionFilter getParallaxOcclusionFilter()const noexcept
		{
			return m_parallaxOcclusionFilter;
		}
		/**
		 *\copydoc	castor3d::RenderNodesPass::getComponentsMask
		 */
		C3D_API static ComponentModeFlags getComponentsMask()noexcept;

		C3D_API static bool useCompute()noexcept;

		bool hasSsao()const noexcept override
		{
			return m_ssao && m_ssaoConfig && m_ssaoConfig->enabled;
		}

		ClustersConfig const * getClustersConfig()const noexcept override
		{
			return m_clustersConfig;
		}

	private:
		struct Pipeline
		{
			explicit Pipeline( PipelineFlags pflags )
				: flags{ std::move( pflags ) }
			{
			}

			PipelineFlags flags;
			ashes::DescriptorSetLayoutPtr vtxDescriptorLayout{};
			ashes::DescriptorSetLayoutPtr ioDescriptorLayout{};
			ashes::PipelineLayoutPtr pipelineLayout{};
			ashes::DescriptorSetPoolPtr vtxDescriptorPool{};
			ashes::DescriptorSetPoolPtr ioDescriptorPool{};
			ProgramModule shader;
			ashes::PipelinePtr pipeline{};
			std::unordered_map< size_t, ashes::DescriptorSetPtr > vtxDescriptorSets{};
			ashes::DescriptorSetPtr ioDescriptorSet{};
		};
		struct PipelineNodesDescriptors
		{
			uint32_t pipelineId{};
			ashes::DescriptorSet const * vtxDescriptorSet{};
		};
		using PipelinePtr = std::unique_ptr< Pipeline >;
		using SubmeshPipelinesNodesDescriptors = std::map< ashes::DescriptorSet const *, UInt32Array >;
		using SubmeshPipelinesMap = std::map< Pipeline const *, SubmeshPipelinesNodesDescriptors >;
		using BillboardPipelinesNodesDescriptors = std::map< uint32_t, PipelineNodesDescriptors >;
		using BillboardPipelinesMap = std::map< Pipeline const *, BillboardPipelinesNodesDescriptors >;
		using PipelineContainer = std::vector< PipelinePtr >;

	private:
		void doAccept( castor3d::RenderTechniqueVisitor & visitor )override;
		bool doIsEnabled()const noexcept;
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		void doRecordCompute( crg::RecordContext const & context
			, VkCommandBuffer commandBuffer );
		void doRecordGraphics( crg::RecordContext & context
			, VkCommandBuffer commandBuffer );
		Pipeline & doCreatePipeline( PipelineFlags const & flags );
		Pipeline & doCreatePipeline( PipelineFlags const & flags
			, uint32_t stride );
		Pipeline & doCreatePipeline( PipelineFlags const & flags
			, uint32_t stride
			, PipelineContainer & pipelines );

	private:
		RenderDevice const & m_device;
		RenderNodesPass const & m_nodesPass;
		CameraUbo const & m_cameraUbo;
		SceneUbo const & m_sceneUbo;
		crg::ImageViewIdArray m_targetImage;
		crg::ImageViewIdArray m_targetDepth;
		SsaoConfig const * m_ssaoConfig{};
		Texture const * m_ssao{};
		DeferredLightingFilter m_deferredLightingFilter;
		ParallaxOcclusionFilter m_parallaxOcclusionFilter;
		RenderNodesPassChangeSignalConnection m_onNodesPassSort;
		bool m_commandsChanged{};
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_framebuffer;
		PipelineContainer m_pipelines;
		PipelineContainer m_billboardPipelines;
		SubmeshPipelinesMap m_activePipelines;
		BillboardPipelinesMap m_activeBillboardPipelines;
		ClustersConfig const * m_clustersConfig{};
		castor::ChangeTracked< uint32_t > m_maxPipelineId{};
	};
}

#endif
