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
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	linearisedDepth	The linearised depth buffer.
		 *\param[in]	scene			The scene buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	linearisedDepth	Le tampon de profondeur linéarisé.
		 *\param[in]	scene			Le tampon de scène.
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
			, uint32_t size
			, uint32_t layerIndex );
		/**
		 *\~english
		 *\brief		Renders the SSGI pass.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine la passe SSGI.
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
