/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubsurfaceScatteringPass_H___
#define ___C3D_SubsurfaceScatteringPass_H___

#include "Render/Viewport.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Texture/TextureUnit.hpp"
#include "Technique/Opaque/LightPass.hpp"
#include "Technique/RenderTechniqueVisitor.hpp"

#include <Command/CommandBuffer.hpp>
#include <Sync/Semaphore.hpp>

#include <GlslShader.hpp>

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		26/07/2017
	\~english
	\brief		Gaussian blur pass.
	\~french
	\brief		Passe flou gaussien.
	*/
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
		renderer::Semaphore const & render( renderer::Semaphore const & toWait )const;
		/**
		 *\~english
		 *\brief		Dumps the results on the screen.
		 *\param[in]	size	The dump dimensions.
		 *\~french
		 *\brief		Dumpe les résultats sur l'écran.
		 *\param[in]	size	Les dimensions d'affichage.
		 */
		C3D_API void debugDisplay( castor::Size const & size )const;
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
			, castor::Point2r const & direction )const;
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
				, renderer::UniformBuffer< BlurConfiguration > const & blurUbo
				, GeometryPassResult const & gp
				, TextureUnit const & source
				, TextureUnit const & destination
				, bool isVertic
				, renderer::ShaderStageStateArray const & shaderStages );
			Blur( Blur && rhs );
			void prepareFrame( renderer::CommandBuffer & commandBuffer )const;

		private:
			void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
				, renderer::DescriptorSet & descriptorSet )override;

		private:
			RenderSystem & m_renderSystem;
			GeometryPassResult const & m_geometryBufferResult;
			GpInfoUbo & m_gpInfoUbo;
			SceneUbo & m_sceneUbo;
			renderer::UniformBuffer< BlurConfiguration > const & m_blurUbo;
			renderer::RenderPassPtr m_renderPass;
			renderer::FrameBufferPtr m_frameBuffer;
		};

		class Combine
			: private RenderQuad
		{
		public:
			explicit Combine( RenderSystem & renderSystem
				, castor::Size const & size
				, renderer::UniformBuffer< BlurWeights > const & blurUbo
				, GeometryPassResult const & gp
				, TextureUnit const & source
				, std::array< TextureUnit, 3u > const & blurResults
				, TextureUnit const & destination
				, renderer::ShaderStageStateArray const & shaderStages );
			Combine( Combine && rhs );
			void prepareFrame( renderer::CommandBuffer & commandBuffer )const;

		private:
			void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
				, renderer::DescriptorSet & descriptorSet )override;

		private:
			RenderSystem & m_renderSystem;
			renderer::UniformBuffer< BlurWeights > const & m_blurUbo;
			GeometryPassResult const & m_geometryBufferResult;
			TextureUnit const & m_source;
			std::array< TextureUnit, 3u > const & m_blurResults;
			renderer::RenderPassPtr m_renderPass;
			renderer::FrameBufferPtr m_frameBuffer;
		};

	private:
		castor::Size m_size;
		renderer::UniformBufferPtr< BlurConfiguration > m_blurConfigUbo;
		renderer::UniformBufferPtr< BlurWeights > m_blurWeightsUbo;
		TextureUnit m_intermediate;
		std::array< TextureUnit, 3u > m_blurResults;
		TextureUnit m_result;
		glsl::Shader m_blurHorizVertexShader;
		glsl::Shader m_blurHorizPixelShader;
		renderer::ShaderStageStateArray m_blurHorizProgram;
		Blur m_blurX[3];
		glsl::Shader m_blurVerticVertexShader;
		glsl::Shader m_blurVerticPixelShader;
		renderer::ShaderStageStateArray m_blurVerticProgram;
		Blur m_blurY[3];
		glsl::Shader m_combineVertexShader;
		glsl::Shader m_combinePixelShader;
		renderer::ShaderStageStateArray m_combineProgram;
		Combine m_combine;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::SemaphorePtr m_finished;
		renderer::FencePtr m_fence;
		RenderPassTimerSPtr m_timer;
	};
}

#endif
