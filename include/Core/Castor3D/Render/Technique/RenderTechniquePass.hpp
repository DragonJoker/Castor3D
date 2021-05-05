/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniquePass_H___
#define ___C3D_RenderTechniquePass_H___

#include "TechniqueModule.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/RenderPass.hpp"

#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	struct RenderTechniquePassDesc
	{
		RenderTechniquePassDesc( bool environment
			, SsaoConfig const & ssaoConfig
			, LpvGridConfigUbo const * lpvConfigUbo
			, LayeredLpvGridConfigUbo const * llpvConfigUbo = nullptr
			, VoxelizerUbo const * vctConfigUbo = nullptr
			, LightVolumePassResult const * lpvResult = nullptr
			, TextureUnit const * vctFirstBounce = nullptr
			, TextureUnit const * vctSecondaryBounce = nullptr )
			: environment{ environment }
			, ssaoConfig{ ssaoConfig }
			, lpvConfigUbo{ lpvConfigUbo }
			, llpvConfigUbo{ llpvConfigUbo }
			, vctConfigUbo{ vctConfigUbo }
			, lpvResult{ lpvResult }
			, vctFirstBounce{ vctFirstBounce }
			, vctSecondaryBounce{ vctSecondaryBounce }
		{
		}
		/**
		 *\~english
		 *\brief		Constructor for opaque passes.
		 *\param[in]	environment		Pass used for an environment map rendering.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	lpvConfigUbo	The LPV configuration, if needed.
		 *\param[in]	llpvConfigUbo	The Layered LPV configuration, if needed.
		 *\param[in]	vctConfigUbo	The VCT configuration, if needed.
		 *\~french
		 *\brief		Constructeur pour les passes opaques.
		 *\param[in]	environment		Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	lpvConfigUbo	La configuration des LPV, si nécessaire.
		 *\param[in]	llpvConfigUbo	La configuration des Layered LPV, si nécessaire.
		 *\param[in]	vctConfigUbo	La configuration du VCT, si nécessaire.
		 */
		RenderTechniquePassDesc( bool environment
			, SsaoConfig const & ssaoConfig
			, LpvGridConfigUbo const * lpvConfigUbo = nullptr
			, LayeredLpvGridConfigUbo const * llpvConfigUbo = nullptr
			, VoxelizerUbo const * vctConfigUbo = nullptr )
			: RenderTechniquePassDesc{ environment
				, ssaoConfig
				, lpvConfigUbo
				, llpvConfigUbo
				, vctConfigUbo
				, nullptr
				, nullptr
				, nullptr }
		{
		}

		bool environment;
		SsaoConfig const & ssaoConfig;
		LpvGridConfigUbo const * lpvConfigUbo;
		LayeredLpvGridConfigUbo const * llpvConfigUbo;
		VoxelizerUbo const * vctConfigUbo;
		LightVolumePassResult const * lpvResult;
		TextureUnit const * vctFirstBounce;
		TextureUnit const * vctSecondaryBounce;
	};

	class RenderTechniquePass
		: public SceneRenderPass
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device				The render device.
		 *\param[in]	category			The pass category.
		 *\param[in]	name				The pass name.
		 *\param[in]	renderPassDesc		The scene render pass construction data.
		 *\param[in]	techniquePassDesc	The technique render pass construction data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device				Le render device.
		 *\param[in]	category			La catégorie de la passe.
		 *\param[in]	name				Le nom de la passe.
		 *\param[in]	renderPassDesc		Les données de construction de passe de rendu de scène.
		 *\param[in]	techniquePassDesc	Les données de construction de passe de rendu de technique.
		 */
		C3D_API RenderTechniquePass( RenderDevice const & device
			, castor::String const & category
			, castor::String const & name
			, SceneRenderPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniquePassDesc
			, ashes::RenderPassPtr renderPass = nullptr );

	public:
		/**
		 *\~english
		 *\brief		Visitor acceptance function.
		 *\param		visitor	The ... visitor.
		 *\~french
		 *\brief		Fonction d'acceptation de visiteur.
		 *\param		visitor	Le ... visiteur.
		 */
		C3D_API virtual void accept( RenderTechniqueVisitor & visitor );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_finished );
			return *m_finished;
		}

		C3D_API ShaderFlags getShaderFlags()const override
		{
			return ShaderFlag::eWorldSpace
				| ShaderFlag::eTangentSpace
				| ShaderFlag::eVelocity
				| ShaderFlag::eViewSpace;
		}
		/**@}*/

	public:
		using SceneRenderPass::update;

	private:
		C3D_API virtual void doUpdateNodes( SceneCulledRenderNodes & nodes
			, castor::Point2f const & jitter
			, RenderInfo & info );
		C3D_API void doUpdateUbos( CpuUpdater & updater )override;
		C3D_API void doUpdateFlags( PipelineFlags & flags )const override;
		C3D_API ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		C3D_API void doFillUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, BillboardListRenderNode & nodes )override;
		C3D_API void doFillUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, SubmeshRenderNode & nodes )override;
		C3D_API void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		C3D_API void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		C3D_API void doUpdatePipeline( RenderPipeline & pipeline )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;

	protected:
		/**
		 *\copydoc		castor3d::SceneRenderPass::doCreateUboBindings
		 */
		ashes::VkDescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::SceneRenderPass::doCreateTextureBindings
		 */
		ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;

	protected:
		Scene const & m_scene;
		Camera * m_camera{ nullptr };
		SceneRenderNode m_sceneNode;
		bool m_environment{ false };
		SsaoConfig m_ssaoConfig;
		LpvGridConfigUbo const * m_lpvConfigUbo;
		LayeredLpvGridConfigUbo const * m_llpvConfigUbo;
		VoxelizerUbo const * m_vctConfigUbo;
		LightVolumePassResult const * m_lpvResult;
		TextureUnit const * m_vctFirstBounce;
		TextureUnit const * m_vctSecondaryBounce;
		ashes::SemaphorePtr m_finished;
	};
}

#endif
