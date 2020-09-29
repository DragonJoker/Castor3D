/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredOpaqueResolvePass_H___
#define ___C3D_DeferredOpaqueResolvePass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class OpaqueResolvePass
		: castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	scene			The rendered scene.
		 *\param[in]	gp				The geometry pass result.
		 *\param[in]	lightDiffuse	The diffuse result of the lighting pass.
		 *\param[in]	lightSpecular	The specular result of the lighting pass.
		 *\param[in]	result			The texture receiving the result.
		 *\param[in]	sceneUbo		The scene UBO.
		 *\param[in]	gpInfoUbo		The geometry pass UBO.
		 *\param[in]	hdrConfigUbo	The HDR UBO.
		 *\param[in]	ssao			The SSAO image.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	scene			La scène rendue.
		 *\param[in]	gp				Le résultat de la passe géométrique.
		 *\param[in]	lightDiffuse	Le résultat diffus de la passe d'éclairage.
		 *\param[in]	lightSpecular	Le résultat spéculaire de la passe d'éclairage.
		 *\param[in]	result			La texture recevant le résultat.
		 *\param[in]	sceneUbo		L'UBO de la scène.
		 *\param[in]	gpInfoUbo		L'UBO de la passe géométrique.
		 *\param[in]	hdrConfigUbo	L'UBO HDR.
		 *\param[in]	ssao			L'image SSAO.
		 */
		C3D_API OpaqueResolvePass( Engine & engine
			, RenderDevice const & device
			, Scene & scene
			, OpaquePassResult const & gp
			, TextureUnit const & lightDiffuse
			, TextureUnit const & lightSpecular
			, TextureUnit const & result
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo
			, HdrConfigUbo const & hdrConfigUbo
			, SsaoPass const * ssao );
		C3D_API ~OpaqueResolvePass() = default;
		C3D_API void initialise();
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the configuration UBO.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Met à jour l'UBO de configuration.
		 *\param[in]	camera	La caméra de rendu.
		 */
		C3D_API void update( Camera const & camera );
		/**
		 *\~english
		 *\brief		Renders the reflection mapping.
		 *\param[in]	toWait	The semaphore to wait.
		 *\~french
		 *\brief		Dessine le mapping de réflexion.
		 *\param[in]	toWait	Le sémaphore à attendre.
		 */
		C3D_API ashes::Semaphore const & render( ashes::Semaphore const & toWait )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );

	private:
		struct ProgramPipeline
		{
			ProgramPipeline( ProgramPipeline const & ) = delete;
			ProgramPipeline( ProgramPipeline && ) = default;
			ProgramPipeline & operator=( ProgramPipeline const & ) = delete;
			ProgramPipeline & operator=( ProgramPipeline && ) = delete;
			ProgramPipeline( Engine & engine
				, RenderDevice const & device
				, OpaquePassResult const & gp
				, ashes::DescriptorSetLayout const & uboLayout
				, ashes::DescriptorSetLayout const & texLayout
				, ashes::RenderPass const & renderPass
				, ashes::ImageView const * ssao
				, VkExtent2D const & size
				, FogType fogType
				, MaterialType matType );
			void updateCommandBuffer( ashes::VertexBufferBase & vbo
				, ashes::DescriptorSet const & uboSet
				, ashes::DescriptorSet const & texSet
				, ashes::FrameBuffer const & frameBuffer
				, RenderPassTimer & timer );
			void accept( PipelineVisitorBase & visitor );

			Engine & m_engine;
			OpaquePassResult const & m_opaquePassResult;
			ashes::RenderPass const * m_renderPass;
			castor3d::ShaderModule m_vertexShader;
			castor3d::ShaderModule m_pixelShader;
			ashes::PipelineShaderStageCreateInfoArray m_program;
			ashes::PipelineLayoutPtr m_pipelineLayout;
			ashes::GraphicsPipelinePtr m_pipeline;
			ashes::CommandBufferPtr m_commandBuffer;
		};
		using ReflectionPrograms = std::array< std::unique_ptr< ProgramPipeline >, size_t( FogType::eCount ) >;

	private:
		RenderDevice const & m_device;
		Scene const & m_scene;
		TextureUnit const & m_result;
		SceneUbo const & m_sceneUbo;
		GpInfoUbo const & m_gpInfoUbo;
		HdrConfigUbo const & m_hdrConfigUbo;
		SsaoPass const * m_ssao;
		VkExtent2D m_size;
		Viewport m_viewport;
		SamplerSPtr m_sampler;
		OpaquePassResult const & m_opaquePassResult;
		TextureUnit const & m_lightDiffuse;
		TextureUnit const & m_lightSpecular;
		ashes::VertexBufferBasePtr m_vertexBuffer;
		ashes::DescriptorSetLayoutPtr m_uboDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_uboDescriptorPool;
		ashes::DescriptorSetPtr m_uboDescriptorSet;
		ashes::DescriptorSetLayoutPtr m_texDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_texDescriptorPool;
		ashes::DescriptorSetPtr m_texDescriptorSet;
		ashes::WriteDescriptorSetArray m_texDescriptorWrites;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::SemaphorePtr m_finished;
		RenderPassTimerSPtr m_timer;
		ReflectionPrograms m_programs;
		bool m_ssaoEnabled{ false };
	};
}

#endif
