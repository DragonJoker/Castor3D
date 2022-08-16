/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VisibilityResolvePass_H___
#define ___C3D_VisibilityResolvePass_H___

#include "VisibilityModule.hpp"

#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

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
		 *\param[in]	parent	The parent technique.
		 *\param[in]	pass	The parent frame pass.
		 *\param[in]	context	The rendering context.
		 *\param[in]	graph	The runnable graph.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent	La technique parente.
		 *\param[in]	pass	La frame pass parente.
		 *\param[in]	context	Le contexte de rendu.
		 *\param[in]	graph	Le runnable graph.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API VisibilityResolvePass( RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::String const & category
			, castor::String const & name
			, RenderNodesPass const & depthPass
			, RenderNodesPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniquePassDesc );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void update( CpuUpdater & updater )override;

		static constexpr bool useCompute{ false };

	private:
		struct Pipeline
		{
			ShaderModule shader;
			ashes::PipelineShaderStageCreateInfoArray stages{};
			ashes::DescriptorSetLayoutPtr descriptorLayout{};
			ashes::PipelineLayoutPtr pipelineLayout{};
			ashes::PipelinePtr firstPipeline{};
			ashes::PipelinePtr blendPipeline{};
			ashes::DescriptorSetPoolPtr descriptorPool{};
			uint32_t passTypeIndex{};
			std::unordered_map< uint64_t, ashes::DescriptorSetPtr > descriptorSets{};
		};
		using PipelinePtr = std::unique_ptr< Pipeline >;

	private:
		void doInitialise();
		bool doIsEnabled()const;
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		void doRecordCompute( crg::RecordContext & context
			, VkCommandBuffer commandBuffer );
		void doRecordGraphics( crg::RecordContext & context
			, VkCommandBuffer commandBuffer );
		Pipeline & doCreatePipeline( PassTypeID passType
			, TextureFlags textureFlags
			, SubmeshFlags submeshFlags
			, PassFlags passFlags );
		Pipeline & doCreatePipeline( PassTypeID passType
			, TextureFlags textureFlags
			, PassFlags passFlags
			, uint32_t stride );
		Pipeline & doCreatePipeline( PassTypeID passType
			, TextureFlags textureFlags
			, SubmeshFlags submeshFlags
			, PassFlags passFlags
			, uint32_t stride
			, std::unordered_map< uint64_t, PipelinePtr > & pipelines );
		void doOnCullerCompute( SceneCuller const & culler );

	private:
		RenderDevice const & m_device;
		RenderNodesPass const & m_depthPass;
		MatrixUbo & m_matrixUbo;
		SceneUbo & m_sceneUbo;
		SceneCuller & m_culler;
		SceneCullerSignalConnection m_onCullerCompute;
		ashes::DescriptorSetLayoutPtr m_inOutsDescriptorLayout{};
		ashes::DescriptorSetPoolPtr m_inOutsDescriptorPool{};
		ashes::DescriptorSetPtr m_inOutsDescriptorSet{};
		bool m_commandsChanged{};
		ShaderModule m_vertexShader;
		ashes::RenderPassPtr m_firstRenderPass;
		ashes::FrameBufferPtr m_firstFramebuffer;
		ashes::RenderPassPtr m_blendRenderPass;
		ashes::FrameBufferPtr m_blendFramebuffer;
		std::unordered_map< uint64_t, PipelinePtr > m_pipelines;
		std::unordered_map< uint64_t, PipelinePtr > m_billboardPipelines;
		std::map< Pipeline const *, std::set< ashes::DescriptorSet const * > > m_activePipelines;
		std::map< Pipeline const *, std::map< uint32_t, ashes::DescriptorSet const * > > m_activeBillboardPipelines;
	};
}

#endif
