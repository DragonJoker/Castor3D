/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubsurfaceScatteringPass_H___
#define ___C3D_SubsurfaceScatteringPass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/ToTexture/RenderQuad.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Technique/Opaque/LightPass.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class SubsurfaceScatteringPass
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	gpInfoUbo		The geometry pass UBO.
		 *\param[in]	sceneUbo		The scene UBO.
		 *\param[in]	textureSize		The render area dimensions.
		 *\param[in]	gp				The geometry pass result.
		 *\param[in]	lightDiffuse	The light pass diffuse result.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	gpInfoUbo		L'UBO de la passe géométrique.
		 *\param[in]	sceneUbo		L'UBO de la scène.
		 *\param[in]	textureSize		Les dimensions de la zone de rendu.
		 *\param[in]	gp				Le résultat de la geometry pass.
		 *\param[in]	lightDiffuse	Le résultat diffus de la light pass.
		 */
		C3D_API SubsurfaceScatteringPass( Engine & engine
			, GpInfoUbo & gpInfoUbo
			, SceneUbo & sceneUbo
			, castor::Size const & textureSize
			, GeometryPassResult const & gp
			, TextureUnit const & lightDiffuse );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SubsurfaceScatteringPass();
		/**
		*\~english
		*\brief		Prepares the command buffer.
		*\~french
		*\brief		Prépare le tampon de commandes.
		*/
		C3D_API void prepare();
		/**
		 *\~english
		 *\brief		Renders the subsurfaces scattering.
		 *\param[in]	toWait	The semaphore to wait.
		 *\~french
		 *\brief		Dessine le subsurfaces scattering.
		 *\param[in]	toWait	Le sémaphore à attendre.
		 */
		ashes::Semaphore const & render( ashes::Semaphore const & toWait )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

		inline TextureUnit const & getResult()const
		{
			return m_result;
		}

	private:
		void doBlur( GeometryPassResult const & gp
			, TextureUnit const & source
			, TextureUnit const & destination
			//, TextureAttachmentSPtr attach
			, castor::Point2f const & direction )const;
		void doCombine( GeometryPassResult const & gp
			, TextureUnit const & source )const;

	public:
		static castor::String const Config;
		static castor::String const Step;
		static castor::String const Correction;
		static castor::String const PixelSize;
		static castor::String const Weights;
		static castor::String const Offsets;

		struct BlurConfiguration
		{
			castor::Point2f blurPixelSize;
			float blurCorrection;
		};

		struct BlurWeights
		{
			castor::Point4f originalWeight;
			castor::Point4f blurWeights[3u];
			castor::Point4f blurVariance;
		};

	private:
		class Blur
			: private RenderQuad
		{
		public:
			Blur( RenderSystem & renderSystem
				, castor::Size const & size
				, GpInfoUbo & gpInfoUbo
				, SceneUbo & sceneUbo
				, UniformBuffer< BlurConfiguration > const & blurUbo
				, GeometryPassResult const & gp
				, TextureUnit const & source
				, TextureUnit const & destination
				, bool isVertic
				, ashes::PipelineShaderStageCreateInfoArray const & shaderStages );
			Blur( Blur && rhs )noexcept;
			void prepareFrame( ashes::CommandBuffer & commandBuffer )const;

		private:
			void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
				, ashes::DescriptorSet & descriptorSet )override;

		private:
			RenderSystem & m_renderSystem;
			GeometryPassResult const & m_geometryBufferResult;
			GpInfoUbo & m_gpInfoUbo;
			SceneUbo & m_sceneUbo;
			UniformBuffer< BlurConfiguration > const & m_blurUbo;
			ashes::RenderPassPtr m_renderPass;
			ashes::FrameBufferPtr m_frameBuffer;
		};

		class Combine
			: private RenderQuad
		{
		public:
			explicit Combine( RenderSystem & renderSystem
				, castor::Size const & size
				, UniformBuffer< BlurWeights > const & blurUbo
				, GeometryPassResult const & gp
				, TextureUnit const & source
				, std::array< TextureUnit, 3u > const & blurResults
				, TextureUnit const & destination
				, ashes::PipelineShaderStageCreateInfoArray const & shaderStages );
			Combine( Combine && rhs )noexcept;
			void prepareFrame( ashes::CommandBuffer & commandBuffer )const;

		private:
			void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
				, ashes::DescriptorSet & descriptorSet )override;

		private:
			RenderSystem & m_renderSystem;
			UniformBuffer< BlurWeights > const & m_blurUbo;
			GeometryPassResult const & m_geometryBufferResult;
			TextureUnit const & m_source;
			std::array< TextureUnit, 3u > const & m_blurResults;
			ashes::RenderPassPtr m_renderPass;
			ashes::FrameBufferPtr m_frameBuffer;
		};

	private:
		castor::Size m_size;
		UniformBufferUPtr< BlurConfiguration > m_blurConfigUbo;
		UniformBufferUPtr< BlurWeights > m_blurWeightsUbo;
		TextureUnit m_intermediate;
		std::array< TextureUnit, 3u > m_blurResults;
		TextureUnit m_result;
		ShaderModule m_blurHorizVertexShader;
		ShaderModule m_blurHorizPixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_blurHorizProgram;
		Blur m_blurX[3];
		ShaderModule m_blurVerticVertexShader;
		ShaderModule m_blurVerticPixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_blurVerticProgram;
		Blur m_blurY[3];
		ShaderModule m_combineVertexShader;
		ShaderModule m_combinePixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_combineProgram;
		Combine m_combine;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
		RenderPassTimerSPtr m_timer;
	};
}

#endif
