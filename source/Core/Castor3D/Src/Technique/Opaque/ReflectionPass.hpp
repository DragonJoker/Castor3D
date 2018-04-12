/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredReflectionPass_H___
#define ___C3D_DeferredReflectionPass_H___

#include "LightPass.hpp"
#include "SsaoPass.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/RenderInfo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Shader/Ubos/GpInfoUbo.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/FrameBuffer.hpp>

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
		 *\param[in]	viewport		The viewport holding depth bounds.
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
		 *\param[in]	viewport		Le viewport contenant les bornes profondeur.
		 */
		ReflectionPass( Engine & engine
			, Scene & scene
			, GeometryPassResult const & gp
			, renderer::TextureView const & lightDiffuse
			, renderer::TextureView const & lightSpecular
			, renderer::TextureView const & result
			, SceneUbo & sceneUbo
			, GpInfoUbo & gpInfoUbo
			, SsaoConfig const & config
			, Viewport const & viewport );
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
		void render( renderer::Semaphore const & toWait )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline TextureLayout const & getSsao()const
		{
			return *m_ssao.getResult().getTexture();
		}

		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_finished );
			return *m_finished;
		}
		/**@}*/

	private:
		struct ProgramPipeline
		{
			ProgramPipeline( ProgramPipeline const & ) = delete;
			ProgramPipeline( ProgramPipeline && ) = default;
			ProgramPipeline & operator=( ProgramPipeline const & ) = delete;
			ProgramPipeline & operator=( ProgramPipeline && ) = default;
			ProgramPipeline( Engine & engine
				, Scene & scene
				, renderer::VertexBufferBase & vbo
				, renderer::DescriptorSetLayout const & uboLayout
				, renderer::DescriptorSet const & uboSet
				, renderer::RenderPass const & renderPass
				, renderer::FrameBuffer const & frameBuffer
				, GeometryPassResult const & gp
				, renderer::TextureView const & lightDiffuse
				, renderer::TextureView const & lightSpecular
				, renderer::TextureView const * ssao
				, renderer::Extent2D const & size
				, FogType fogType
				, MaterialType matType
				, SamplerSPtr sampler );

			renderer::ShaderStageStateArray m_program;
			renderer::DescriptorSetLayoutPtr m_texDescriptorLayout;
			renderer::DescriptorSetPoolPtr m_texDescriptorPool;
			renderer::DescriptorSetPtr m_texDescriptorSet;
			renderer::WriteDescriptorSetArray m_texDescriptorWrites;
			renderer::PipelineLayoutPtr m_pipelineLayout;
			renderer::PipelinePtr m_pipeline;
			renderer::CommandBufferPtr m_commandBuffer;
		};
		using ReflectionPrograms = std::array< ProgramPipeline, size_t( FogType::eCount ) >;

	private:
		renderer::Device const & m_device;
		Scene const & m_scene;
		GpInfoUbo & m_gpInfoUbo;
		renderer::Extent2D m_size;
		Viewport m_viewport;
		SamplerSPtr m_sampler;
		renderer::VertexBufferBasePtr m_vertexBuffer;
		renderer::DescriptorSetLayoutPtr m_uboDescriptorLayout;
		renderer::DescriptorSetPoolPtr m_uboDescriptorPool;
		renderer::DescriptorSetPtr m_uboDescriptorSet;
		renderer::RenderPassPtr m_renderPass;
		renderer::FrameBufferPtr m_frameBuffer;
		renderer::SemaphorePtr m_finished;
		SsaoPass m_ssao;
		ReflectionPrograms m_programs;
		RenderPassTimerSPtr m_timer;
		bool m_ssaoEnabled{ false };
	};
}

#endif
