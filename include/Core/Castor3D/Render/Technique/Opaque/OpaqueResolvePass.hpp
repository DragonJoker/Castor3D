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
		using EnvMapArray = std::vector< std::reference_wrapper< EnvironmentMap > >;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine					The engine.
		 *\param[in]	device					The GPU device.
		 *\param[in]	scene					The rendered scene.
		 *\param[in]	gp						The geometry pass result.
		 *\param[in]	ssao					The SSAO image.
		 *\param[in]	subsurfaceScattering	The subsurface scattering result.
		 *\param[in]	lightDiffuse			The diffuse result of the lighting pass.
		 *\param[in]	lightSpecular			The specular result of the lighting pass.
		 *\param[in]	lightIndirectDiffuse	The indirect diffuse result of the lighting pass.
		 *\param[in]	lightIndirectSpecular	The indirect specular result of the lighting pass.
		 *\param[in]	result					The texture receiving the result.
		 *\param[in]	sceneUbo				The scene UBO.
		 *\param[in]	gpInfoUbo				The geometry pass UBO.
		 *\param[in]	hdrConfigUbo			The HDR UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine					Le moteur.
		 *\param[in]	device					Le device GPU.
		 *\param[in]	scene					La scène rendue.
		 *\param[in]	gp						Le résultat de la passe géométrique.
		 *\param[in]	ssao					L'image SSAO.
		 *\param[in]	subsurfaceScattering	Le résultat du subsurface scattering.
		 *\param[in]	lightDiffuse			Le résultat diffus de la passe d'éclairage.
		 *\param[in]	lightSpecular			Le résultat spéculaire de la passe d'éclairage.
		 *\param[in]	lightIndirectDiffuse	Le résultat indirect diffus de la passe d'éclairage.
		 *\param[in]	lightIndirectSpecular	Le résultat indirect spéculaire de la passe d'éclairage.
		 *\param[in]	result					La texture recevant le résultat.
		 *\param[in]	sceneUbo				L'UBO de la scène.
		 *\param[in]	gpInfoUbo				L'UBO de la passe géométrique.
		 *\param[in]	hdrConfigUbo			L'UBO HDR.
		 */
		C3D_API OpaqueResolvePass( crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, Scene & scene
			, OpaquePassResult const & gp
			, SsaoPass const & ssao
			, Texture const & subsurfaceScattering
			, Texture const & lightDiffuse
			, Texture const & lightSpecular
			, Texture const & lightIndirectDiffuse
			, Texture const & lightIndirectSpecular
			, Texture const & result
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo
			, HdrConfigUbo const & hdrConfigUbo );
		C3D_API ~OpaqueResolvePass() = default;
		C3D_API void initialise();
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
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
				, ashes::RenderPass const & renderPass
				, crg::ImageViewId const * ssao
				, crg::ImageViewId const * subsurfaceScattering
				, crg::ImageViewId const & lightDiffuse
				, crg::ImageViewId const & lightSpecular
				, crg::ImageViewId const * lightIndirectDiffuse
				, crg::ImageViewId const * lightIndirectSpecular
				, SamplerSPtr const & sampler
				, VkExtent2D const & size
				, FogType fogType
				, MaterialType matType
				, Scene const & scene );
			void updateCommandBuffer( ashes::VertexBufferBase & vbo
				, ashes::DescriptorSet const & uboSet
				, ashes::FrameBuffer const & frameBuffer
				, RenderPassTimer & timer );
			void accept( PipelineVisitorBase & visitor );

			Engine & engine;
			OpaquePassResult const & opaquePassResult;
			ashes::RenderPass const * renderPass;
			castor3d::ShaderModule vertexShader;
			castor3d::ShaderModule pixelShader;
			ashes::PipelineShaderStageCreateInfoArray program;
			ashes::DescriptorSetLayoutPtr texDescriptorLayout;
			ashes::DescriptorSetPoolPtr texDescriptorPool;
			ashes::DescriptorSetPtr texDescriptorSet;
			ashes::PipelineLayoutPtr pipelineLayout;
			ashes::GraphicsPipelinePtr pipeline;
			ashes::CommandBufferPtr commandBuffer;
		};
		using ProgramPipelinePtr = std::unique_ptr< ProgramPipeline >;
		static size_t constexpr SsaoCount = 2u;
		static size_t constexpr SsssCount = 2u;
		static size_t constexpr DiffuseGiCount = 2u;
		static size_t constexpr SpecularGiCount = 2u;
		static size_t constexpr AllButFogCount = SsaoCount * SsssCount * DiffuseGiCount * SpecularGiCount;
		static size_t constexpr MaxProgramsCount = size_t( FogType::eCount ) * AllButFogCount;
		using ReflectionPrograms = std::array< ProgramPipelinePtr, MaxProgramsCount >;

		ProgramPipeline & getProgram();

	private:
		RenderDevice const & m_device;
		Scene const & m_scene;
		Texture const & m_result;
		SceneUbo const & m_sceneUbo;
		GpInfoUbo const & m_gpInfoUbo;
		HdrConfigUbo const & m_hdrConfigUbo;
		SsaoPass const & m_ssao;
		VkExtent2D m_size;
		Viewport m_viewport;
		SamplerSPtr m_sampler;
		OpaquePassResult const & m_opaquePassResult;
		Texture const & m_subsurfaceScattering;
		Texture const & m_lightDiffuse;
		Texture const & m_lightSpecular;
		Texture const & m_lightIndirectDiffuse;
		Texture const & m_lightIndirectSpecular;
		ashes::VertexBufferBasePtr m_vertexBuffer;
		ashes::DescriptorSetLayoutPtr m_uboDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_uboDescriptorPool;
		ashes::DescriptorSetPtr m_uboDescriptorSet;
		ashes::WriteDescriptorSetArray m_texDescriptorWrites;
		ashes::SemaphorePtr m_finished;
		RenderPassTimerSPtr m_timer;
		ReflectionPrograms m_programs;
		ProgramPipeline * m_currentProgram;
		bool m_voxelConeTracing;
	};
}

#endif
