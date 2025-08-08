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

namespace c3d
{
	class SubsurfaceScatteringPass
		: public OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph			The frame graph.
		 *\param[in]	device			The GPU device.
		 *\param[in]	progress		The optional progress bar.
		 *\param[in]	scene			The scene.
		 *\param[in]	cameraUbo		The camera UBO.
		 *\param[in]	depthObj		The depths and objects image.
		 *\param[in]	diffuse			The diffuse lighting texture.
		 *\param[in]	isEnabled		Callback to check if the pass is active.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph			Le frame graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	progress		La barre de progression optionnelle.
		 *\param[in]	scene			La scène.
		 *\param[in]	cameraUbo		L'UBO de la caméra.
		 *\param[in]	depthObj		La texture de profondeurs et d'objets.
		 *\param[in]	diffuse			La texture de diffuse lighting.
		 *\param[in]	isEnabled		Callback de détermination d'activation de la passe.
		 */
		C3D_API SubsurfaceScatteringPass( crg::FramePassGroup & graph
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
		C3D_API void update( CpuUpdater const & updater );
		/**
		 *\copydoc		RenderTechniquePass::accept
		 */
		C3D_API void accept( ConfigurationVisitorBase & visitor );

		Texture const & getResult()const noexcept
		{
			return m_result;
		}

	public:
		static MbString const Config;
		static MbString const Step;
		static MbString const Correction;
		static MbString const PixelSize;
		static MbString const Weights;
		static MbString const Offsets;

		struct BlurConfiguration
		{
			Point2f blurPixelSize;
			float blurCorrection;
		};

		struct BlurWeights
		{
			Point4f originalWeight;
			Array< Point4f, 3u > blurWeights;
			Point4f blurVariance;
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
		Size m_size;
		Texture m_intermediate;
		Array< Texture, 3u > m_blurImages;
		Texture m_result;
		UniformBufferOffsetT< BlurConfiguration > m_blurCfgUbo;
		UniformBufferOffsetT< BlurWeights > m_blurWgtUbo;
		ProgramModule m_blurHorizProgram;
		ashes::PipelineShaderStageCreateInfoArray m_blurXShader;
		ProgramModule m_blurVerticProgram;
		ashes::PipelineShaderStageCreateInfoArray m_blurYShader;
		ProgramModule m_combineProgram;
		ashes::PipelineShaderStageCreateInfoArray m_combineShader;
	};
}

#endif
