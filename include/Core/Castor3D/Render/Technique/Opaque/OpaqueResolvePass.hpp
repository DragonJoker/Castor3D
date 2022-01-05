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
#include "Castor3D/Render/Ssao/SsaoModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
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
			ShaderModule vertexShader;
			ShaderModule pixelShader;
			ashes::PipelineShaderStageCreateInfoArray stages;
		};

	public:
		using EnvMapArray = std::vector< std::reference_wrapper< EnvironmentMap > >;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine					The engine.
		 *\param[in]	device					The GPU device.
		 *\param[in]	scene					The rendered scene.
		 *\param[in]	gp						The geometry pass result.
		 *\param[in]	ssao					The SSAO image.
		 *\param[in]	subsurfaceScattering	The subsurface scattering result.
		 *\param[in]	lightDiffuse			The diffuse result of the lighting pass.
		 *\param[in]	lightSpecular			The specular result of the lighting pass.
		 *\param[in]	lightIndirectDiffuse	The indirect diffuse result of the lighting pass.
		 *\param[in]	lightIndirectSpecular	The indirect specular result of the lighting pass.
		 *\param[in]	result					The texture receiving the result.
		 *\param[in]	sceneUbo				The scene UBO.
		 *\param[in]	gpInfoUbo				The geometry pass UBO.
		 *\param[in]	hdrConfigUbo			The HDR UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine					Le moteur.
		 *\param[in]	device					Le device GPU.
		 *\param[in]	scene					La scène rendue.
		 *\param[in]	gp						Le résultat de la passe géométrique.
		 *\param[in]	ssao					L'image SSAO.
		 *\param[in]	subsurfaceScattering	Le résultat du subsurface scattering.
		 *\param[in]	lightDiffuse			Le résultat diffus de la passe d'éclairage.
		 *\param[in]	lightSpecular			Le résultat spéculaire de la passe d'éclairage.
		 *\param[in]	lightIndirectDiffuse	Le résultat indirect diffus de la passe d'éclairage.
		 *\param[in]	lightIndirectSpecular	Le résultat indirect spéculaire de la passe d'éclairage.
		 *\param[in]	result					La texture recevant le résultat.
		 *\param[in]	sceneUbo				L'UBO de la scène.
		 *\param[in]	gpInfoUbo				L'UBO de la passe géométrique.
		 *\param[in]	hdrConfigUbo			L'UBO HDR.
		 */
		C3D_API OpaqueResolvePass( crg::FramePassGroup & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, ProgressBar * progress
			, Scene & scene
			, OpaquePassResult const & gp
			, SsaoConfig const & ssao
			, Texture const & ssaoResult
			, Texture const & subsurfaceScattering
			, Texture const & lightDiffuse
			, Texture const & lightSpecular
			, Texture const & lightIndirectDiffuse
			, Texture const & lightIndirectSpecular
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

	private:
		crg::FramePass const & doCreatePass( crg::FramePassGroup & graph
			, crg::FramePass const & pass
			, ProgressBar * progress );
		std::vector< Program > doCreatePrograms();

	private:
		RenderDevice const & m_device;
		Scene const & m_scene;
		Texture const & m_result;
		SceneUbo const & m_sceneUbo;
		GpInfoUbo const & m_gpInfoUbo;
		HdrConfigUbo const & m_hdrConfigUbo;
		SsaoConfig const & m_ssao;
		OpaquePassResult const & m_opaquePassResult;
		Texture const & m_ssaoResult;
		Texture const & m_subsurfaceScattering;
		Texture const & m_lightSpecular;
		Texture const & m_lightIndirectDiffuse;
		Texture const & m_lightIndirectSpecular;
		std::vector< Program > m_programs;
		uint32_t m_programIndex{};
	};
}

#endif
