/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubsurfaceScatteringPass_H___
#define ___C3D_SubsurfaceScatteringPass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace castor3d
{
	class SubsurfaceScatteringPass
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph			The frame graph.
		 *\param[in]	previousPass	The previous frame pass.
		 *\param[in]	device			The GPU device.
		 *\param[in]	progress		The optional progress bar.
		 *\param[in]	scene			The scene.
		 *\param[in]	cameraUbo		The camera UBO.
		 *\param[in]	depthObj		The depths and objects image.
		 *\param[in]	diffuse			The diffuse lighting texture.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph			Le frame graph.
		 *\param[in]	previousPass	La frame pass précédente.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	progress		La barre de progression optionnelle.
		 *\param[in]	scene			La scène.
		 *\param[in]	cameraUbo		L'UBO de la caméra.
		 *\param[in]	depthObj		La texture de profondeurs et d'objets.
		 *\param[in]	diffuse			La texture de diffuse lighting.
		 */
		C3D_API SubsurfaceScatteringPass( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, RenderDevice const & device
			, ProgressBar * progress
			, Scene const & scene
			, CameraUbo const & cameraUbo
			, Texture const & depthObj
			, Texture const & diffuse
			, crg::RunnablePass::IsEnabledCallback const & isEnabled );
		C3D_API ~SubsurfaceScatteringPass()noexcept;
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
		C3D_API void accept( ConfigurationVisitorBase & visitor );

		Texture const & getResult()const noexcept
		{
			return m_result;
		}

		crg::FramePass const & getLastPass()const noexcept
		{
			return *m_lastPass;
		}

	public:
		static castor::MbString const Config;
		static castor::MbString const Step;
		static castor::MbString const Correction;
		static castor::MbString const PixelSize;
		static castor::MbString const Weights;
		static castor::MbString const Offsets;

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
		CameraUbo const & m_cameraUbo;
		Texture const & m_diffuse;
		Scene const & m_scene;
		crg::FramePassGroup & m_group;
		bool m_enabled;
		castor::Size m_size;
		Texture m_intermediate;
		castor::Array< Texture, 3u > m_blurImages;
		Texture m_result;
		UniformBufferOffsetT< BlurConfiguration > m_blurCfgUbo;
		UniformBufferOffsetT< BlurWeights > m_blurWgtUbo;
		ProgramModule m_blurHorizProgram;
		ashes::PipelineShaderStageCreateInfoArray m_blurXShader;
		ProgramModule m_blurVerticProgram;
		ashes::PipelineShaderStageCreateInfoArray m_blurYShader;
		ProgramModule m_combineProgram;
		ashes::PipelineShaderStageCreateInfoArray m_combineShader;
		crg::FramePass const * m_lastPass{};
	};
}

#endif
