/*
See LICENSE file in root folder
*/
#ifndef ___C3D_IndirectLightingPass_H___
#define ___C3D_IndirectLightingPass_H___

#include "OpaqueModule.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightingModule.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

#include <array>

namespace castor3d
{
	class IndirectLightingPass
	{
	public:
		enum DescriptorIdx
			: uint32_t
		{
			eModels,
			eCamera,
			eMaterials,
			eLpvGridConfig,
			eLayeredLpvGridConfig,
			eVoxelData,
			eDepthObj,
			eNmlOcc,
			eColMtl,
			eSpcRgh,
			eBrdf,
			eVctFirstBounce,
			eVctSecondBounce,
			eLpvR,
			eLpvG,
			eLpvB,
			eLayeredLpv1R,
			eLayeredLpv1G,
			eLayeredLpv1B,
			eLayeredLpv2R,
			eLayeredLpv2G,
			eLayeredLpv2B,
			eLayeredLpv3R,
			eLayeredLpv3G,
			eLayeredLpv3B,
			eCount,
			eVctStart = eVctFirstBounce,
			eLpvStart = eLpvR,
			eLayeredLpvStart = eLayeredLpv1R,
		};

		enum class ProgramType
		{
			eNoGI,
			eVCT,
			eLPV,
			eLLPV,
			eLPVLLPV,
			eCount,
		};

		struct Config
		{
			explicit Config( SceneFlags const & sceneFlags );
			explicit Config( uint32_t index );

			SceneFlags sceneFlags{};
			bool voxels{};
			bool lpv{};
			bool llpv{};
			uint32_t index{};
		};

		struct Program
		{
			Program( RenderDevice const & device
				, Scene const & scene
				, DebugConfig & debugConfig
				, Config const & config );

			ShaderModule vertexShader;
			ShaderModule pixelShader;
			ashes::PipelineShaderStageCreateInfoArray stages;
		};

		using ProgramPtr = std::unique_ptr< Program >;

	public:
		IndirectLightingPass( RenderDevice const & device
			, ProgressBar * progress
			, Scene const & scene
			, DebugConfig & debugConfig
			, crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, Texture const & brdf
			, Texture const & depthObj
			, OpaquePassResult const & gpResult
			, LightPassResult const & lpResult
			, IndirectLightingData const & indirect
			, CameraUbo const & cameraUbo );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		void update( CpuUpdater & updater );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( PipelineVisitorBase & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		crg::FramePass const & doCreateLightingPass( crg::FramePassGroup & graph
			, Scene const & scene
			, crg::FramePass const & previousPass
			, ProgressBar * progress );
		Program & doCreateProgram( uint32_t index );

	private:
		RenderDevice const & m_device;
		Scene const & m_scene;
		DebugConfig & m_debugConfig;
		Texture const & m_brdf;
		Texture const & m_depthObj;
		OpaquePassResult const & m_gpResult;
		LightPassResult const & m_lpResult;
		IndirectLightingData const & m_indirect;
		CameraUbo const & m_cameraUbo;
		crg::FramePassGroup & m_group;
		std::vector< ProgramPtr > m_programs;
		uint32_t m_programIndex{};
		bool m_enabled{};
		crg::FramePass const * m_lastPass{};
	};
}

#endif
