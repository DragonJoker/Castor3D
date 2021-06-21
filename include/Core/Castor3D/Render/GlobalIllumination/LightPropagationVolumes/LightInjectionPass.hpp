/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightInjectionPass_HPP___
#define ___C3D_LightInjectionPass_HPP___

#include "LightPropagationVolumesModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderAST/Shader.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

#include <RenderGraph/RunnablePasses/RenderPass.hpp>
#include <RenderGraph/RunnablePasses/PipelineHolder.hpp>

namespace castor3d
{
	class LightInjectionPass
		: public castor::Named
		, public crg::RenderPass
	{
	public:
		enum Idx : uint32_t
		{
			LightsIdx,
			RsmNormalsIdx,
			RsmPositionIdx,
			RsmFluxIdx,
			LpvGridUboIdx,
			LpvLightUboIdx,
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine				The engine.
		 *\param[in]	device				The GPU device.
		 *\param[in]	prefix				The pass name's prefix.
		 *\param[in]	lightCache			The lights cache.
		 *\param[in]	lightType			The light source type.
		 *\param[in]	smResult			The shadow map.
		 *\param[in]	lpvGridConfigUbo	The LPV grid configuration UBO.
		 *\param[in]	lpvLightConfigUbo	The LPV light configuration UBO.
		 *\param[in]	result				The result.
		 *\param[in]	gridSize			The grid dimensions.
		 *\param[in]	layerIndex			The layer index.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine				Le moteur.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	prefix				Le préfixe du nom de la passe.
		 *\param[in]	lightCache			Le cache de sources lumineuses.
		 *\param[in]	lightType			Le type de source lumineuse.
		 *\param[in]	smResult			La shadow map.
		 *\param[in]	lpvGridConfigUbo	L'UBO de configuration de la grille de LPV.
		 *\param[in]	lpvLightConfigUbo	L'UBO de configuration de la source lumineuse LPV.
		 *\param[in]	result				Le résultat.
		 *\param[in]	gridSize			Les dimensions de la grille.
		 *\param[in]	layerIndex			L'indice de la layer.
		 */
		C3D_API LightInjectionPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, LightType lightType
			, uint32_t gridSize
			, uint32_t layerIndex
			, uint32_t rsmSize );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );

	protected:
		C3D_API void doSubInitialise()override;
		C3D_API void doSubRecordInto( VkCommandBuffer commandBuffer
			, uint32_t index )override;

	private:
		class PipelineHolder
		{
		public:
			PipelineHolder( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, crg::pp::Config config
				, uint32_t lpvSize );

			void initialise( VkRenderPass renderPass );
			void recordInto( VkCommandBuffer commandBuffer
				, uint32_t index );

		protected:
			void doCreatePipeline( uint32_t index );

		private:
			uint32_t m_lpvSize;
			VkRenderPass m_renderPass;
			crg::PipelineHolder m_holder;
		};

	private:
		RenderDevice const & m_device;
		uint32_t m_rsmSize;
		ashes::VertexBufferPtr< NonTexturedQuad::Vertex > m_vertexBuffer;
		ShaderModule m_vertexShader;
		ShaderModule m_geometryShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		PipelineHolder m_holder;
	};
}

#endif
