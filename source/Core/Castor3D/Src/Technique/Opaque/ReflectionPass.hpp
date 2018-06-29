/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredReflectionPass_H___
#define ___C3D_DeferredReflectionPass_H___

#include "LightPass.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/RenderInfo.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"
#include "Technique/RenderTechniqueVisitor.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/FrameBuffer.hpp>

#include <GlslShader.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		06/04/2017
	\~english
	\brief		The post lighting reflection pass.
	\~french
	\brief		La passe de réflexion post éclairage.
	*/
	class ReflectionPass
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
		 *\param[in]	config			The SSAO configuration.
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
		 *\param[in]	config			La configuration du SSAO.
		 */
		ReflectionPass( Engine & engine
			, Scene & scene
			, GeometryPassResult const & gp
			, renderer::TextureView const & lightDiffuse
			, renderer::TextureView const & lightSpecular
			, renderer::TextureView const & result
			, SceneUbo & sceneUbo
			, GpInfoUbo & gpInfoUbo
			, renderer::TextureView const * ssao );
		/**
		 *\~english
		 *\brief		Updates the configuration UBO.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Met à jour l'UBO de configuration.
		 *\param[in]	camera	La caméra de rendu.
		 */
		void update( Camera const & camera );
		/**
		 *\~english
		 *\brief		Renders the reflection mapping.
		 *\param[in]	toWait	The semaphore to wait.
		 *\~french
		 *\brief		Dessine le mapping de réflexion.
		 *\param[in]	toWait	Le sémaphore à attendre.
		 */
		renderer::Semaphore const & render( renderer::Semaphore const & toWait )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

	private:
		struct ProgramPipeline
		{
			ProgramPipeline( ProgramPipeline const & ) = delete;
			ProgramPipeline( ProgramPipeline && ) = default;
			ProgramPipeline & operator=( ProgramPipeline const & ) = delete;
			ProgramPipeline & operator=( ProgramPipeline && ) = default;
			ProgramPipeline( Engine & engine
				, renderer::DescriptorSetLayout const & uboLayout
				, renderer::DescriptorSetLayout const & texLayout
				, renderer::RenderPass const & renderPass
				, renderer::TextureView const * ssao
				, renderer::Extent2D const & size
				, FogType fogType
				, MaterialType matType );
			void updateCommandBuffer( renderer::VertexBufferBase & vbo
				, renderer::DescriptorSet const & uboSet
				, renderer::DescriptorSet const & texSet
				, renderer::FrameBuffer const & frameBuffer
				, RenderPassTimer & timer );
			void accept( RenderTechniqueVisitor & visitor );

			renderer::RenderPass const * m_renderPass;
			glsl::Shader m_vertexShader;
			glsl::Shader m_pixelShader;
			renderer::ShaderStageStateArray m_program;
			renderer::PipelineLayoutPtr m_pipelineLayout;
			renderer::PipelinePtr m_pipeline;
			renderer::CommandBufferPtr m_commandBuffer;
		};
		using ReflectionPrograms = std::array< ProgramPipeline, size_t( FogType::eCount ) >;

	private:
		renderer::Device const & m_device;
		Scene const & m_scene;
		GpInfoUbo & m_gpInfoUbo;
		renderer::TextureView const * m_ssaoResult;
		renderer::Extent2D m_size;
		Viewport m_viewport;
		SamplerSPtr m_sampler;
		GeometryPassResult const & m_geometryPassResult;
		renderer::TextureView const & m_lightDiffuse;
		renderer::TextureView const & m_lightSpecular;
		renderer::VertexBufferBasePtr m_vertexBuffer;
		renderer::DescriptorSetLayoutPtr m_uboDescriptorLayout;
		renderer::DescriptorSetPoolPtr m_uboDescriptorPool;
		renderer::DescriptorSetPtr m_uboDescriptorSet;
		renderer::DescriptorSetLayoutPtr m_texDescriptorLayout;
		renderer::DescriptorSetPoolPtr m_texDescriptorPool;
		renderer::DescriptorSetPtr m_texDescriptorSet;
		renderer::WriteDescriptorSetArray m_texDescriptorWrites;
		renderer::RenderPassPtr m_renderPass;
		renderer::FrameBufferPtr m_frameBuffer;
		renderer::SemaphorePtr m_finished;
		renderer::FencePtr m_fence;
		RenderPassTimerSPtr m_timer;
		ReflectionPrograms m_programs;
		bool m_ssaoEnabled{ false };
	};
}

#endif
