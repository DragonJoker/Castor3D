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

#include <ShaderWriter/Shader.hpp>

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
			, ashes::TextureView const & lightDiffuse
			, ashes::TextureView const & lightSpecular
			, ashes::TextureView const & result
			, SceneUbo & sceneUbo
			, GpInfoUbo & gpInfoUbo
			, HdrConfigUbo & hdrConfigUbo
			, ashes::TextureView const * ssao );
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
		ashes::Semaphore const & render( ashes::Semaphore const & toWait )const;
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
				, GeometryPassResult const & gp
				, ashes::DescriptorSetLayout const & uboLayout
				, ashes::DescriptorSetLayout const & texLayout
				, ashes::RenderPass const & renderPass
				, ashes::TextureView const * ssao
				, ashes::Extent2D const & size
				, FogType fogType
				, MaterialType matType );
			void updateCommandBuffer( ashes::VertexBufferBase & vbo
				, ashes::DescriptorSet const & uboSet
				, ashes::DescriptorSet const & texSet
				, ashes::FrameBuffer const & frameBuffer
				, RenderPassTimer & timer );
			void accept( RenderTechniqueVisitor & visitor );

			GeometryPassResult const & m_geometryPassResult;
			ashes::RenderPass const * m_renderPass;
			castor3d::ShaderModule m_vertexShader;
			castor3d::ShaderModule m_pixelShader;
			ashes::ShaderStageStateArray m_program;
			ashes::PipelineLayoutPtr m_pipelineLayout;
			ashes::PipelinePtr m_pipeline;
			ashes::CommandBufferPtr m_commandBuffer;
		};
		using ReflectionPrograms = std::array< ProgramPipeline, size_t( FogType::eCount ) >;

	private:
		ashes::Device const & m_device;
		Scene const & m_scene;
		ashes::TextureView const * m_ssaoResult;
		ashes::Extent2D m_size;
		Viewport m_viewport;
		SamplerSPtr m_sampler;
		GeometryPassResult const & m_geometryPassResult;
		ashes::TextureView const & m_lightDiffuse;
		ashes::TextureView const & m_lightSpecular;
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
		ashes::FencePtr m_fence;
		RenderPassTimerSPtr m_timer;
		ReflectionPrograms m_programs;
		bool m_ssaoEnabled{ false };
	};
}

#endif
