/*
See LICENSE file in root folder
*/
#ifndef ___C3D_IndirectLightingPass_H___
#define ___C3D_IndirectLightingPass_H___

#include "OpaqueModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"
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
			eGpInfo,
			eScene,
			eLpvGridConfig,
			eLayeredLpvGridConfig,
			eVoxelData,
			eDepth,
			eData1,
			eData2,
			eData3,
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
				, Config const & config );

			ShaderModule vertexShader;
			ShaderModule pixelShader;
			ashes::PipelineShaderStageCreateInfoArray stages;
		};

	public:
		IndirectLightingPass( RenderDevice const & device
			, ProgressBar * progress
			, Scene const & scene
			, crg::FramePassGroup & graph
			, crg::FramePass const *& previousPass
			, OpaquePassResult const & gpResult
			, LightPassResult const & lpResult
			, LightVolumePassResult const & lpvResult
			, LightVolumePassResultArray const & llpvResult
			, Texture const & vctFirstBounce
			, Texture const & vctSecondaryBounce
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo
			, LpvGridConfigUbo const & lpvConfigUbo
			, LayeredLpvGridConfigUbo const & llpvConfigUbo
			, VoxelizerUbo const & vctConfigUbo );
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

	private:
		crg::FramePass const & doCreateLightingPass( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, ProgressBar * progress );

	private:
		RenderDevice const & m_device;
		OpaquePassResult const & m_gpResult;
		LightPassResult const & m_lpResult;
		LightVolumePassResult const & m_lpvResult;
		LightVolumePassResultArray const & m_llpvResult;
		Texture const & m_vctFirstBounce;
		Texture const & m_vctSecondaryBounce;
		SceneUbo const & m_sceneUbo;
		GpInfoUbo const & m_gpInfoUbo;
		LpvGridConfigUbo const & m_lpvConfigUbo;
		LayeredLpvGridConfigUbo const & m_llpvConfigUbo;
		VoxelizerUbo const & m_vctConfigUbo;
		crg::FramePassGroup & m_group;
		std::vector< Program > m_programs;
		uint32_t m_programIndex{};
		bool m_enabled{};
	};
}

#endif
