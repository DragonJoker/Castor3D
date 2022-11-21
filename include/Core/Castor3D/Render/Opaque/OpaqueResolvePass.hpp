/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredOpaqueResolvePass_H___
#define ___C3D_DeferredOpaqueResolvePass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include <ShaderAST/Shader.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

namespace castor3d
{
	class OpaqueResolvePass
		: castor::OwnedBy< Engine >
	{
	public:
		struct Program
		{
			Program( ShaderModule vtxShader
				, ShaderModule pxlShader )
				: vertexShader{ std::move( vtxShader ) }
				, pixelShader{ std::move( pxlShader ) }
			{
			}

			ShaderModule vertexShader;
			ShaderModule pixelShader;
			ashes::PipelineShaderStageCreateInfoArray stages;
		};
		using ProgramPtr = std::unique_ptr< Program >;

	public:
		using EnvMapArray = std::vector< std::reference_wrapper< EnvironmentMap > >;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph					The render graph.
		 *\param[in]	previousPasses			The passes this pass depends on.
		 *\param[in]	device					The GPU device.
		 *\param[in]	progress				The progress bar.
		 *\param[in]	scene					The rendered scene.
		 *\param[in]	depthObj				The depths and objects texture.
		 *\param[in]	gp						The geometry pass result.
		 *\param[in]	ssao					The SSAO configuration.
		 *\param[in]	ssaoResult				The SSAO image.
		 *\param[in]	subsurfaceScattering	The subsurface scattering result.
		 *\param[in]	lighting				The lighting pass result.
		 *\param[in]	result					The texture receiving the result.
		 *\param[in]	sceneUbo				The scene UBO.
		 *\param[in]	gpInfoUbo				The geometry pass UBO.
		 *\param[in]	hdrConfigUbo			The HDR UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph					Le render graph.
		 *\param[in]	previousPasses			Les passes dont celle-ci dépend.
		 *\param[in]	device					Le device GPU.
		 *\param[in]	progress				La barre de progression.
		 *\param[in]	scene					La scène rendue.
		 *\param[in]	depthObj				La texture de profondeurs et d'objets.
		 *\param[in]	gp						Le résultat de la passe géométrique.
		 *\param[in]	ssao					La configuration du SSAO.
		 *\param[in]	ssaoResult				L'image SSAO.
		 *\param[in]	subsurfaceScattering	Le résultat du subsurface scattering.
		 *\param[in]	lighting				Le résultat de la passe d'éclairage.
		 *\param[in]	result					La texture recevant le résultat.
		 *\param[in]	sceneUbo				L'UBO de la scène.
		 *\param[in]	gpInfoUbo				L'UBO de la passe géométrique.
		 *\param[in]	hdrConfigUbo			L'UBO HDR.
		 */
		C3D_API OpaqueResolvePass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, RenderDevice const & device
			, ProgressBar * progress
			, Scene & scene
			, Texture const & depthObj
			, OpaquePassResult const & gp
			, SsaoConfig const & ssao
			, Texture const & ssaoResult
			, Texture const & subsurfaceScattering
			, LightPassResult const & lighting
			, Texture const & result
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo
			, HdrConfigUbo const & hdrConfigUbo );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		crg::FramePass const & doCreatePass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, ProgressBar * progress );
		Program & doCreateProgram( uint32_t index );

	private:
		RenderDevice const & m_device;
		Scene const & m_scene;
		Texture const & m_result;
		SceneUbo const & m_sceneUbo;
		GpInfoUbo const & m_gpInfoUbo;
		HdrConfigUbo const & m_hdrConfigUbo;
		SsaoConfig const & m_ssao;
		Texture const & m_depthObj;
		OpaquePassResult const & m_opaquePassResult;
		Texture const & m_ssaoResult;
		Texture const & m_subsurfaceScattering;
		LightPassResult const & m_lighting;
		std::vector< ProgramPtr > m_programs;
		uint32_t m_programIndex{};
		crg::FramePass const * m_lastPass{};
	};
}

#endif
