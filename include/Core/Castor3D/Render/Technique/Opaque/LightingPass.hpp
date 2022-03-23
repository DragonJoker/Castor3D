/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingPass_H___
#define ___C3D_LightingPass_H___

#include "OpaqueModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueResolvePass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <CastorUtils/Design/DelayedInitialiser.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <RenderGraph/RunnablePasses/PipelineHolder.hpp>
#include <RenderGraph/RunnablePasses/RenderPass.hpp>

#include <unordered_set>

namespace castor3d
{
	struct LightPipelineConfig
	{
		LightPipelineConfig( PassTypeID passType
			, SceneFlags const & sceneFlags
			, Light const & light );

		size_t makeHash()const;

		PassTypeID passType;
		SceneFlags const & sceneFlags;
		LightType lightType;
		ShadowType shadowType;
		bool shadows;
	};

	struct LightRenderPass
	{
		ashes::RenderPassPtr renderPass;
		ashes::FrameBufferPtr framebuffer;
		ashes::VkClearValueArray clearValues;
		struct Entry
		{
			crg::ImageViewId view;
			VkImageLayout input;
			VkImageLayout output;
		};
		std::vector< Entry > attaches;
	};

	struct LightDescriptors
	{
		explicit LightDescriptors( Light const & light
			, RenderDevice const & device );

		Light const & light;
		MatrixUbo matrixUbo;
		UniformBufferOffsetT< ModelBufferConfiguration > modelMatrixUbo;
		ashes::DescriptorSetPtr descriptorSet;
	};

	class LightPipeline
	{
	public:
		LightPipeline( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, LightPipelineConfig const & config
			, std::vector< LightRenderPass > const & renderPasses
			, ashes::PipelineShaderStageCreateInfoArray stages
			, VkDescriptorSetLayout descriptorSetLayout );

		VkPipeline getPipeline( uint32_t index )
		{
			return m_holder.getPipeline( index );
		}

		VkPipelineLayout getPipelineLayout()
		{
			return m_holder.getPipelineLayout();
		}

		VkDescriptorSet getDescriptorSet()
		{
			return m_holder.getDescriptorSet( 0u );
		}

	protected:
		void doCreatePipeline();

	private:
		ashes::PipelineVertexInputStateCreateInfo doCreateVertexLayout();
		ashes::PipelineViewportStateCreateInfo doCreateViewportState( ashes::FrameBuffer const & framebuffer );
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( bool blend );
		VkCullModeFlags doGetCullMode()const;

	private:
		crg::PipelineHolder m_holder;
		LightPipelineConfig const & m_config;
		std::vector< LightRenderPass > const & m_renderPasses;
	};

	struct LightsPipeline
	{
		LightsPipeline( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, LightPipelineConfig const & config
			, LightPassResult const & lpResult
			, ShadowMapResult const & smResult
			, std::vector< LightRenderPass > const & renderPasses );

		void clear();
		void addLight( Camera const & camera
			, Light const & light );
		void removeLight( Camera const & camera
			, Light const & light );
		void recordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t & index );

	private:
		ashes::DescriptorSetLayoutPtr doCreateDescriptorLayout();
		LightDescriptors & doCreateLightEntry( Light const & light );
		castor::Matrix4x4f doComputeModelMatrix( castor3d::Light const & light
			, Camera const & camera )const;
		GpuBufferOffsetT< float > doCreateVertexBuffer();

	private:
		crg::GraphContext & m_context;
		ShadowMapResult const & m_smResult;
		RenderDevice const & m_device;
		std::vector< LightRenderPass > const & m_renderPasses;
		LightPipelineConfig m_config;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		LightPipeline m_pipeline;
		uint32_t m_count{};
		GpuBufferOffsetT< float > m_vertexBuffer;
		std::map< size_t, std::unique_ptr< LightDescriptors > > m_lightDescriptors;
		std::vector< LightDescriptors const * > m_enabledLights;
		Viewport m_viewport;
	};

	using LightsPipelinePtr = std::unique_ptr< LightsPipeline >;

	class RunnableLightingPass
		: public crg::RunnablePass
	{
	public:
		RunnableLightingPass( LightingPass const & lightingPass
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, Scene const & scene
			, LightPassResult const & lpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult );

		void clear();
		void enableLight( Camera const & camera
			, Light const & light );
		void disableLight( Camera const & camera
			, Light const & light );
		void resetCommandBuffer();
		bool hasEnabledLights()const;

	protected:
		void doInitialise();
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );

	private:
		LightRenderPass doCreateRenderPass( bool blend
			, LightPassResult const & result );
		LightsPipeline & doFindPipeline( Light const & light );

	private:
		RenderDevice const & m_device;
		Scene const & m_scene;
		LightPassResult const & m_lpResult;
		ShadowMapResult const & m_smDirectionalResult;
		ShadowMapResult const & m_smPointResult;
		ShadowMapResult const & m_smSpotResult;
		std::vector< LightRenderPass > m_renderPasses;
		std::map< size_t, LightsPipelinePtr > m_pipelines;
	};

	class LightingPass
	{
	public:
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\param[in]	graph				The render graph.
		 *\param[in]	previousPass		The previous frame pass.
		 *\param[in]	device				The GPU device.
		 *\param[in]	progress			The progress bar.
		 *\param[in]	size				The render area dimensions.
		 *\param[in]	scene				The scene.
		 *\param[in]	depth				The depth texture.
		 *\param[in]	gpResult			The geometry pass buffers.
		 *\param[in]	smDirectionalResult	The directional lights shadow map.
		 *\param[in]	smPointResult		The point lights shadow map.
		 *\param[in]	smSpotResult		The spot lights shadow map.
		 *\param[in]	lpResult			The lighting pass result.
		 *\param[in]	sceneUbo			The scene UBO.
		 *\param[in]	gpInfoUbo			The GBuffer configuration UBO.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	graph				Le render graph.
		 *\param[in]	previousPass		La frame pass précédente.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	progress			La barre de progression.
		 *\param[in]	size				Les dimensions de la zone de rendu.
		 *\param[in]	scene				La scène.
		 *\param[in]	depth				La texxture de profondeur.
		 *\param[in]	gpResult			Les textures de la passe de géométries.
		 *\param[in]	smDirectionalResult	La shadow map des source lumineuses directionnelles.
		 *\param[in]	smPointResult		La shadow map des source lumineuses omnidirectionnelles.
		 *\param[in]	smSpotResult		La shadow map des source lumineuses projecteurs.
		 *\param[in]	lpResult			Le résulta de la lighting pass.
		 *\param[in]	sceneUbo			L'UBO de scène.
		 *\param[in]	gpInfoUbo			L'UBO de configuration du GBuffer.
		 */
		LightingPass( crg::FramePassGroup & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::Size const & size
			, Scene & scene
			, Texture const & depth
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, LightPassResult const & lpResult
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		void update( CpuUpdater & updater );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( PipelineVisitorBase & visitor );

		bool isEnabled()const
		{
			return m_lightPass
				&& m_lightPass->hasEnabledLights();
		}

	private:
		crg::FramePass const & doCreateDepthBlitPass( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, ProgressBar * progress );
		crg::FramePass const & doCreateLightingPass( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, Scene const & scene
			, ProgressBar * progress );
		void doUpdateLightPasses( CpuUpdater & updater
			, LightType lightType );

	private:
		RenderDevice const & m_device;
		Texture const & m_depth;
		OpaquePassResult const & m_gpResult;
		ShadowMapResult const & m_smDirectionalResult;
		ShadowMapResult const & m_smPointResult;
		ShadowMapResult const & m_smSpotResult;
		LightPassResult const & m_lpResult;
		SceneUbo const & m_sceneUbo;
		GpInfoUbo const & m_gpInfoUbo;
		crg::FramePassGroup & m_group;
		castor::Size const m_size;
		RunnableLightingPass * m_lightPass{};
	};
}

#endif
