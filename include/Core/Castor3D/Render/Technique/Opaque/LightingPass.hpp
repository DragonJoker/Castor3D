/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingPass_H___
#define ___C3D_LightingPass_H___

#include "OpaqueModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueResolvePass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmConfig.hpp"

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
		LightPipelineConfig( MaterialType materialType
			, SceneFlags const & sceneFlags
			, Light const & light );

		size_t makeHash()const;

		MaterialType materialType;
		SceneFlags const & sceneFlags;
		LightType lightType;
		ShadowType shadowType;
		bool shadows;
		bool rsm;
		bool voxels;
		bool generatesIndirect;
	};

	struct LightRenderPass
	{
		ashes::RenderPassPtr renderPass;
		ashes::FrameBufferPtr framebuffer;
		ashes::VkClearValueArray clearValues;
	};

	struct LightDescriptors
	{
		LightDescriptors( RenderDevice const & device );

		MatrixUbo matrixUbo;
		UniformBufferOffsetT< ModelUboConfiguration > modelMatrixUbo;
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
		void doCreatePipeline( uint32_t index );

	private:
		ashes::PipelineVertexInputStateCreateInfo doCreateVertexLayout();
		ashes::PipelineViewportStateCreateInfo doCreateViewportState( ashes::FrameBuffer const & framebuffer );
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( bool blend );

	private:
		crg::PipelineHolder m_holder;
		LightPipelineConfig const & m_config;
		std::vector< LightRenderPass > const & m_renderPasses;
		VkDescriptorSetLayout m_descriptorSetLayout;
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
		void recordInto( VkCommandBuffer commandBuffer
			, uint32_t & index );

	private:
		ashes::DescriptorSetLayoutPtr doCreateDescriptorLayout();
		LightDescriptors & doCreateLightEntry( Light const & light );
		castor::Matrix4x4f doComputeModelMatrix( castor3d::Light const & light
			, Camera const & camera )const;
		ashes::VertexBufferPtr< float > doCreateVertexBuffer();

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
		ashes::VertexBufferPtr< float > m_vertexBuffer;
		std::map< size_t, std::unique_ptr< LightDescriptors > > m_lightDescriptors;
		std::vector< LightDescriptors const * > m_enabledLights;
		Viewport m_viewport;
	};

	using LightsPipelinePtr = std::unique_ptr< LightsPipeline >;

	class RunnableLightingPass
		: public crg::RunnablePass
	{
	public:
		RunnableLightingPass( crg::FramePass const & pass
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
		void resetCommandBuffer();

	protected:
		void doInitialise( uint32_t index )override;
		void doRecordInto( VkCommandBuffer commandBuffer
			, uint32_t index )override;

		VkPipelineStageFlags doGetSemaphoreWaitFlags()const override
		{
			return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

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
		enum class Type
		{
			eNoShadow = 0u,
			eNoShadowVoxelConeTracingGI,
			eShadowNoGI,
			eShadowVoxelConeTracingGI,
			eShadowRsmGI,
			eShadowLpvGI,
			eShadowLpvGGI,
			eShadowLayeredLpvGI,
			eShadowLayeredLpvGGI,
			CU_ScopedEnumBounds( eNoShadow ),
		};
		static_assert( uint32_t( Type::eCount ) == uint32_t( GlobalIlluminationType::eCount ) + 2u );
		using TypeLightPasses = std::array< LightPassUPtr, size_t( LightType::eCount ) >;
		using LightPasses = std::array< TypeLightPasses, size_t( Type::eCount ) >;

	public:
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\param[in]	engine				The engine.
		 *\param[in]	device				The GPU device.
		 *\param[in]	size				The render area dimensions.
		 *\param[in]	scene				The scene.
		 *\param[in]	gpResult			The geometry pass buffers.
		 *\param[in]	smDirectionalResult	The directional lights shadow map.
		 *\param[in]	smPointResult		The point lights shadow map.
		 *\param[in]	smSpotResult		The spot lights shadow map.
		 *\param[in]	lpvResult			The LPV result.
		 *\param[in]	llpvResult			The Layered LPV result.
		 *\param[in]	vctFirstBounce		The VCT first bounce result.
		 *\param[in]	vctSecondaryBounce	The VCT secondary bounce result.
		 *\param[in]	depthView			The depth buffer attach.
		 *\param[in]	sceneUbo			The scene UBO.
		 *\param[in]	gpInfoUbo			The GBuffer configuration UBO.
		 *\param[in]	lpvConfigUbo		The LPV configuration UBO.
		 *\param[in]	llpvConfigUbo		The Layered LPV configuration UBO.
		 *\param[in]	vctConfigUbo		The VCT configuration UBO.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	engine				Le moteur.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	size				Les dimensions de la zone de rendu.
		 *\param[in]	scene				La scène.
		 *\param[in]	gpResult			Les textures de la passe de géométries.
		 *\param[in]	smDirectionalResult	La shadow map des source lumineuses directionnelles.
		 *\param[in]	smPointResult		La shadow map des source lumineuses omnidirectionnelles.
		 *\param[in]	smSpotResult		La shadow map des source lumineuses projecteurs.
		 *\param[in]	lpvResult			Le résultat du LPV.
		 *\param[in]	llpvResult			Le résultat du Layered LPV.
		 *\param[in]	vctFirstBounce		Le résultat du premier rebond de VCT.
		 *\param[in]	vctSecondaryBounce	Le résultat du second rebond de VCT.
		 *\param[in]	depthView			L'attache du tampon de profondeur.
		 *\param[in]	sceneUbo			L'UBO de scène.
		 *\param[in]	gpInfoUbo			L'UBO de configuration du GBuffer.
		 *\param[in]	lpvConfigUbo		L'UBO de configuration des LPV.
		 *\param[in]	llpvConfigUbo		L'UBO de configuration des Layered LPV.
		 *\param[in]	vctConfigUbo		L'UBO de configuration du VCT.
		 */
		LightingPass( crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, castor::Size const & size
			, Scene & scene
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, LightVolumePassResult const & lpvResult
			, LightVolumePassResultArray const & llpvResult
			, Texture const & vctFirstBounce
			, Texture const & vctSecondaryBounce
			, Texture const & depthView
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo
			, LpvGridConfigUbo const & lpvConfigUbo
			, LayeredLpvGridConfigUbo const & llpvConfigUbo
			, VoxelizerUbo const & vctConfigUbo );
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
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		void update( GpuUpdater & updater );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( PipelineVisitorBase & visitor );
		/**
		 *\~english
		 *\return		The light pass diffuse result.
		 *\~french
		 *\return		Le résultat diffus de la passe d'éclairage.
		 */
		inline LightPassResult const & getResult()const
		{
			return m_result;
		}

	private:
		crg::FramePass const & doCreateDepthBlitPass( crg::FrameGraph & graph
			, crg::FramePass const & previousPass );
		crg::FramePass const & doCreateLightingPass( crg::FrameGraph & graph
			, crg::FramePass const & previousPass
			, Scene const & scene );
		void doUpdateLightPasses( CpuUpdater & updater
			, LightType lightType );

	private:
		crg::FrameGraph & m_graph;
		crg::FramePass const & m_previousPass;
		RenderDevice const & m_device;
		OpaquePassResult const & m_gpResult;
		ShadowMapResult const & m_smDirectionalResult;
		ShadowMapResult const & m_smPointResult;
		ShadowMapResult const & m_smSpotResult;
		LightVolumePassResult const & m_lpvResult;
		LightVolumePassResultArray const & m_llpvResult;
		Texture const & m_vctFirstBounce;
		Texture const & m_vctSecondaryBounce;
		Texture const & m_depthView;
		SceneUbo const & m_sceneUbo;
		GpInfoUbo const & m_gpInfoUbo;
		LpvGridConfigUbo const & m_lpvConfigUbo;
		LayeredLpvGridConfigUbo const & m_llpvConfigUbo;
		VoxelizerUbo const & m_vctConfigUbo;
		Texture const & m_srcDepth;
		castor::Size const m_size;
		LightPassResult m_result;
		bool m_voxelConeTracing{ false };
		RunnableLightingPass * m_lightPass{};
	};
}

#endif
