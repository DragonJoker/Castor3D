/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPropagationPass_HPP___
#define ___C3D_LightPropagationPass_HPP___

#include "LightPropagationVolumesModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Opaque/OpaqueModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderAST/Shader.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

#include <RenderGraph/RunnablePasses/RenderPass.hpp>
#include <RenderGraph/RunnablePasses/PipelineHolder.hpp>

namespace castor3d
{
	class LightPropagationPass
		: public castor::Named
		, public crg::RenderPass
	{
	public:
		enum InIdx
		{
			LpvGridUboIdx,
			RLpvGridIdx,
			GLpvGridIdx,
			BLpvGridIdx,
			GpGridIdx,
		};

		enum OutIdx
		{
			RLpvAccumulatorIdx,
			GLpvAccumulatorIdx,
			BLpvAccumulatorIdx,
			RLpvNextStepIdx,
			GLpvNextStepIdx,
			BLpvNextStepIdx,
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	occlusion	Enable occlusion or not.
		 *\param[in]	gridSize	The grid dimensions.
		 *\param[in]	blendMode	The blend mode.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	occlusion	Active l'occlusion ou pas.
		 *\param[in]	gridSize	Les dimensions de la grille.
		 *\param[in]	blendMode	Le mode de mélange.
		 */
		C3D_API LightPropagationPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, bool occlusion
			, uint32_t gridSize
			, BlendMode blendMode );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( ConfigurationVisitorBase & visitor );

	protected:
		C3D_API void doSubInitialise( uint32_t index );
		C3D_API void doSubRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );

	private:
		class PipelineHolder
		{
		public:
			PipelineHolder( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, crg::pp::Config config
				, uint32_t gridSize
				, BlendMode blendMode );

			void initialise( VkRenderPass renderPass
				, uint32_t index );
			void recordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index );

		protected:
			void doCreatePipeline( uint32_t index );

		private:
			crg::PipelineHolder m_holder;
			uint32_t m_gridSize;
			BlendMode m_blendMode;
			VkRenderPass m_renderPass;
		};

	private:
		uint32_t m_gridSize;
		GpuBufferOffsetT< castor::Point3f > m_vertexBuffer;
		ShaderModule m_vertexShader;
		ShaderModule m_geometryShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		PipelineHolder m_holder;
	};
}

#endif
