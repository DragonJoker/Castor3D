/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniquePass_H___
#define ___C3D_RenderTechniquePass_H___

#include "TechniqueModule.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"

#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	struct RenderTechniquePassDesc
	{
		RenderTechniquePassDesc( bool environment
			, SsaoConfig const & ssaoConfig )
			: m_environment{ environment }
			, m_ssaoConfig{ ssaoConfig }
		{
		}
		/**
		 *\~english
		 *\param[in]	value	The LPV configuration.
		 *\~french
		 *\param[in]	value	La configuration des LPV.
		 */
		RenderTechniquePassDesc & lpvConfigUbo( LpvGridConfigUbo const & value )
		{
			m_lpvConfigUbo = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The Layered LPV configuration.
		 *\~french
		 *\param[in]	value	La configuration des Layered LPV.
		 */
		RenderTechniquePassDesc & llpvConfigUbo( LayeredLpvGridConfigUbo const & value )
		{
			m_llpvConfigUbo = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The VCT configuration.
		 *\~french
		 *\param[in]	value	La configuration du VCT.
		 */
		RenderTechniquePassDesc & vctConfigUbo( VoxelizerUbo const & value )
		{
			m_vctConfigUbo = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The LPV result.
		 *\~french
		 *\param[in]	value	Le résultat du LPV.
		 */
		RenderTechniquePassDesc & ssao( Texture const & value )
		{
			m_ssao = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The LPV result.
		 *\~french
		 *\param[in]	value	Le résultat du LPV.
		 */
		RenderTechniquePassDesc & lpvResult( LightVolumePassResult const & value )
		{
			m_lpvResult = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The LPV result.
		 *\~french
		 *\param[in]	value	Le résultat du LPV.
		 */
		RenderTechniquePassDesc & llpvResult( LightVolumePassResultArray const & value )
		{
			m_llpvResult = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The VCT first bounce result.
		 *\~french
		 *\param[in]	value	Le résultat du premier rebond de VCT.
		 */
		RenderTechniquePassDesc & vctFirstBounce( Texture const & value )
		{
			m_vctFirstBounce = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The VCT second bounce result.
		 *\~french
		 *\param[in]	value	Le résultat du second rebond de VCT.
		 */
		RenderTechniquePassDesc & vctSecondaryBounce( Texture const & value )
		{
			m_vctSecondaryBounce = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	\p true if the pass writes to velocity texture.
		 *\~french
		 *\param[in]	value	\p true si la passe écrit dans la texture de vélocité.
		 */
		RenderTechniquePassDesc & hasVelocity( bool value )
		{
			m_hasVelocity = value;
			return *this;
		}

		bool m_environment;
		SsaoConfig const & m_ssaoConfig;
		Texture const * m_ssao{};
		LpvGridConfigUbo const * m_lpvConfigUbo{};
		LayeredLpvGridConfigUbo const * m_llpvConfigUbo{};
		VoxelizerUbo const * m_vctConfigUbo{};
		LightVolumePassResult const * m_lpvResult{};
		LightVolumePassResultArray const * m_llpvResult{};
		Texture const * m_vctFirstBounce{};
		Texture const * m_vctSecondaryBounce{};
		bool m_hasVelocity{};
	};

	class RenderTechniquePass
		: public RenderNodesPass
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	parent				The parent technique.
		 *\param[in]	pass				The parent frame pass.
		 *\param[in]	context				The rendering context.
		 *\param[in]	graph				The runnable graph.
		 *\param[in]	device				The GPU device.
		 *\param[in]	typeName			The pass type name.
		 *\param[in]	category			The pass category.
		 *\param[in]	name				The pass name.
		 *\param[in]	renderPassDesc		The scene render pass construction data.
		 *\param[in]	techniquePassDesc	The technique render pass construction data.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent				La technique parente.
		 *\param[in]	pass				La frame pass parente.
		 *\param[in]	context				Le contexte de rendu.
		 *\param[in]	graph				Le runnable graph.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	typeName			Le nom du type de la passe.
		 *\param[in]	category			La catégorie de la passe.
		 *\param[in]	name				Le nom de la passe.
		 *\param[in]	renderPassDesc		Les données de construction de passe de rendu de scène.
		 *\param[in]	techniquePassDesc	Les données de construction de passe de rendu de technique.
		 */
		C3D_API RenderTechniquePass( RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::String const & typeName
			, castor::String const & category
			, castor::String const & name
			, RenderNodesPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniquePassDesc );

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
		C3D_API void update( GpuUpdater & updater )override;
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
				| ( m_hasVelocity ? ShaderFlag::eVelocity : ShaderFlag::eNone )
				| ShaderFlag::eViewSpace;
		}

		Scene const & getScene()
		{
			return m_scene;
		}
		/**@}*/

	public:
		using RenderNodesPass::update;

	protected:
		C3D_API void doUpdateNodes( QueueRenderNodes & nodes
			, castor::Point2f const & jitter
			, RenderInfo & info );
		C3D_API void doUpdateUbos( CpuUpdater & updater )override;
		C3D_API ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const override;
		C3D_API SceneFlags doAdjustSceneFlags( SceneFlags flags )const override;
		C3D_API void doAddShadowBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddEnvBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddGIBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddShadowDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, uint32_t & index )const;
		C3D_API void doAddEnvDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, uint32_t & index )const;
		C3D_API void doAddGIDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, uint32_t & index )const;

	private:
		ShaderPtr doGetHullShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetDomainShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		void doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		void doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;

	protected:
		RenderTechnique * m_parent{};
		Scene const & m_scene;
		Camera * m_camera{ nullptr };
		bool m_environment{ false };
		bool m_hasVelocity{ false };
		SsaoConfig m_ssaoConfig;
		Texture const * m_ssao;
		LpvGridConfigUbo const * m_lpvConfigUbo;
		LayeredLpvGridConfigUbo const * m_llpvConfigUbo;
		VoxelizerUbo const * m_vctConfigUbo;
		LightVolumePassResult const * m_lpvResult;
		LightVolumePassResultArray const * m_llpvResult;
		Texture const * m_vctFirstBounce;
		Texture const * m_vctSecondaryBounce;
		ashes::SemaphorePtr m_finished;
	};
}

#endif
