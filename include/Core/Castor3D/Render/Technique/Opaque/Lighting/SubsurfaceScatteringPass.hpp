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

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

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
		C3D_API SubsurfaceScatteringPass( crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, ProgressBar * progress
			, Scene const & scene
			, GpInfoUbo const & gpInfoUbo
			, SceneUbo const & sceneUbo
			, castor::Size const & textureSize
			, OpaquePassResult const & gpResult
			, LightPassResult const & lpResult );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );

		Texture const & getResult()const
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
		static constexpr uint32_t PassCount = 3u;

	private:
		RenderDevice const & m_device;
		GpInfoUbo const & m_gpInfoUbo;
		SceneUbo const & m_sceneUbo;
		OpaquePassResult const & m_gpResult;
		LightPassResult const & m_lpResult;
		Scene const & m_scene;
		bool m_enabled;
		castor::Size m_size;
		Texture m_intermediate;
		TextureArray m_blurImages;
		Texture m_result;
		UniformBufferOffsetT< BlurConfiguration > m_blurCfgUbo;
		UniformBufferOffsetT< BlurWeights > m_blurWgtUbo;
		ShaderModule m_blurHorizVertexShader;
		ShaderModule m_blurHorizPixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_blurXShader;
		ShaderModule m_blurVerticVertexShader;
		ShaderModule m_blurVerticPixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_blurYShader;
		ShaderModule m_combineVertexShader;
		ShaderModule m_combinePixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_combineShader;
	};
}

#endif
