/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubsurfaceScatteringPass_H___
#define ___C3D_SubsurfaceScatteringPass_H___

#include "LightingModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"

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
		 *\param[in]	engine		The engine.
		 *\param[in]	device		The GPU device.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	textureSize	The render area dimensions.
		 *\param[in]	gpResult	The geometry pass result.
		 *\param[in]	lpResult	The light pass result.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	gpInfoUbo	L'UBO de la passe géométrique.
		 *\param[in]	sceneUbo	L'UBO de la scène.
		 *\param[in]	textureSize	Les dimensions de la zone de rendu.
		 *\param[in]	gpResult	Le résultat de la geometry pass.
		 *\param[in]	lpResult	Le résultat de la light pass.
		 */
		C3D_API SubsurfaceScatteringPass( Engine & engine
			, RenderDevice const & device
			, GpInfoUbo const & gpInfoUbo
			, SceneUbo & sceneUbo
			, castor::Size const & textureSize
			, OpaquePassResult const & gpResult
			, LightPassResult const & lpResult );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SubsurfaceScatteringPass() = default;
		/**
		 *\~english
		 *\brief		Initialises the GPU elements.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise les éléments GPU.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleans up the GPU elements.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Nettoie les éléments GPU.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Renders the subsurfaces scattering.
		 *\param[in]	device	The GPU device.
		 *\param[in]	toWait	The semaphore to wait.
		 *\~french
		 *\brief		Dessine le subsurfaces scattering.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	toWait	Le sémaphore à attendre.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );

		inline TextureUnit const & getResult()const
		{
			return m_result;
		}

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
				, RenderDevice const & device
				, castor::Size const & size
				, GpInfoUbo const & gpInfoUbo
				, SceneUbo & sceneUbo
				, OpaquePassResult const & gpResult
				, TextureUnit const & source
				, TextureUnit const & destination
				, bool isVertic
				, ashes::PipelineShaderStageCreateInfoArray const & shaderStages );
			Blur( Blur && rhs )noexcept;
			void prepareFrame( ashes::CommandBuffer & commandBuffer )const;

		private:
			OpaquePassResult const & m_geometryBufferResult;
			GpInfoUbo const & m_gpInfoUbo;
			SceneUbo & m_sceneUbo;
			UniformBufferOffsetT< BlurConfiguration > m_blurUbo;
			ashes::RenderPassPtr m_renderPass;
			ashes::FrameBufferPtr m_frameBuffer;
		};
		static constexpr uint32_t PassCount = 3u;
		using BlurResult = std::array< TextureUnit, PassCount >;
		using BlurArray = std::array< std::unique_ptr< Blur >, PassCount >;

		class Combine
			: private RenderQuad
		{
		public:
			explicit Combine( RenderSystem & renderSystem
				, RenderDevice const & device
				, castor::Size const & size
				, OpaquePassResult const & gpResult
				, TextureUnit const & source
				, BlurResult const & blurResults
				, TextureUnit const & destination
				, ashes::PipelineShaderStageCreateInfoArray const & shaderStages );
			Combine( Combine && rhs )noexcept;
			void prepareFrame( ashes::CommandBuffer & commandBuffer )const;

		private:
			UniformBufferOffsetT< BlurWeights > m_blurUbo;
			OpaquePassResult const & m_geometryBufferResult;
			TextureUnit const & m_source;
			BlurResult const & m_blurResults;
			ashes::RenderPassPtr m_renderPass;
			ashes::FrameBufferPtr m_frameBuffer;
		};

	private:
		GpInfoUbo const & m_gpInfoUbo;
		SceneUbo & m_sceneUbo;
		OpaquePassResult const & m_gpResult;
		LightPassResult const & m_lpResult;
		castor::Size m_size;
		TextureUnit m_intermediate;
		BlurResult m_blurResults;
		TextureUnit m_result;
		ShaderModule m_blurHorizVertexShader;
		ShaderModule m_blurHorizPixelShader;
		BlurArray m_blurX;
		ShaderModule m_blurVerticVertexShader;
		ShaderModule m_blurVerticPixelShader;
		BlurArray m_blurY;
		ShaderModule m_combineVertexShader;
		ShaderModule m_combinePixelShader;
		std::unique_ptr< Combine > m_combine;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
		RenderPassTimerSPtr m_timer;
		bool m_initialised{ false };
	};
}

#endif
