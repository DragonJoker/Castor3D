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
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

namespace castor3d
{
	class LightInjectionPass
		: public castor::Named
	{
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
		C3D_API LightInjectionPass( Engine & engine
			, RenderDevice const & device
			, castor::String const & prefix
			, LightCache const & lightCache
			, LightType lightType
			, ShadowMapResult const & smResult
			, LpvGridConfigUbo const & lpvGridConfigUbo
			, LpvLightConfigUbo const & lpvLightConfigUbo
			, LightVolumePassResult const & result
			, uint32_t gridSize
			, uint32_t layerIndex );
		/**
		 *\~english
		 *\brief		Renders the pass.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine la passe.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API ashes::Semaphore const & compute( ashes::Semaphore const & toWait )const;
		C3D_API CommandsSemaphore getCommands( RenderPassTimer const & timer
			, uint32_t index )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );

	private:
		Engine & m_engine;
		RenderDevice const & m_device;
		RenderPassTimerSPtr m_timer;
		uint32_t m_rsmSize;

		ashes::VertexBufferPtr< NonTexturedQuad::Vertex > m_vertexBuffer;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		ShaderModule m_vertexShader;
		ShaderModule m_geometryShader;
		ShaderModule m_pixelShader;
		ashes::RenderPassPtr m_renderPass;
		ashes::GraphicsPipelinePtr m_pipeline;
		ashes::FrameBufferPtr m_frameBuffer;
		CommandsSemaphore m_commands;
	};
}

#endif
