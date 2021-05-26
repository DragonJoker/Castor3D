/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechnique_H___
#define ___C3D_RenderTechnique_H___

#include "TechniqueModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentModule.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelizeModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Design/DelayedInitialiser.hpp>
#include <CastorUtils/Design/Named.hpp>

#include <RenderGraph/Attachment.hpp>
#include <RenderGraph/FramePass.hpp>

namespace castor3d
{
	class RenderTechnique
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
		friend class RenderTechniquePass;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name			The technique name.
		 *\param[in]	renderTarget	The render target for this technique.
		 *\param[in]	device			The GPU device.
		 *\param[in]	parameters		The technique parameters.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name			Le nom de la technique.
		 *\param[in]	renderTarget	La render target pour cette technique.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	parameters		Les paramètres de la technique.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 */
		C3D_API RenderTechnique( castor::String const & name
			, RenderTarget & renderTarget
			, RenderDevice const & device
			, Parameters const & parameters
			, SsaoConfig const & ssaoConfig );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderTechnique();
		/**
		 *\~english
		 *\brief		Lists the intermediate view used by the whole technique.
		 *\param[out]	intermediates	Receives the intermediate views.
		 *\~french
		 *\brief		Liste les vues intermédiaires utilisées par toute la technique.
		 *\param[out]	intermediates	Reçoit les vues intermédiaires.
		 */
		C3D_API void listIntermediates( std::vector< IntermediateView > & intermediates );
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
		 *\brief		Writes the technique into a text file.
		 *\param[in]	file	The file.
		 *\~french
		 *\brief		Ecrit la technique dans un fichier texte.
		 *\param[in]	file	Le fichier.
		 */
		C3D_API bool writeInto( castor::TextFile & file );
		/**
		*\~english
		*\brief
		*	Visitor acceptance function.
		*\param visitor
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de visiteur.
		*\param visitor
		*	Le ... visiteur.
		*/
		C3D_API void accept( RenderTechniqueVisitor & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		castor::Size const & getSize()const
		{
			return m_size;
		}

		TextureLayout const & getResult()const
		{
			CU_Require( m_colourTexture.isTextured() );
			return *m_colourTexture.getTexture();
		}

		crg::ImageId const & getResultImg()const
		{
			return m_colourImage;
		}

		crg::ImageViewId const & getResultImgView()const
		{
			return m_colourView;
		}

		TextureLayout const & getDepth()const
		{
			return *m_depthBuffer.getTexture();
		}

		TextureLayoutSPtr getDepthPtr()const
		{
			return m_depthBuffer.getTexture();
		}

		MatrixUbo const & getMatrixUbo()const
		{
			return m_matrixUbo;
		}

		MatrixUbo & getMatrixUbo()
		{
			return m_matrixUbo;
		}

		crg::FramePass const & getOpaquePass()const
		{
			return *m_opaquePassDesc;
		}

		crg::FramePass const & getBackgroundPass()const
		{
			return *m_backgroundPassDesc;
		}

		crg::FramePass const & getTransparentPass()const
		{
			return *m_transparentPassDesc;
		}

		ShadowMapLightTypeArray const & getShadowMaps()const
		{
			return m_allShadowMaps;
		}

		ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_signalFinished );
			return *m_signalFinished;
		}

		RenderTarget const & getRenderTarget()const
		{
			return m_renderTarget;
		}

		bool isMultisampling()const
		{
			return false;
		}

		SsaoConfig const & getSsaoConfig()const
		{
			return m_ssaoConfig;
		}

		SsaoConfig & getSsaoConfig()
		{
			return m_ssaoConfig;
		}
		/**@}*/

	public:
		using ShadowMapArray = std::vector< ShadowMapUPtr >;

	private:
		crg::FramePass & doCreateDepthPass();
		crg::FramePass & doCreateBackgroundPass();
		crg::FramePass & doCreateOpaquePass();
		crg::FramePass & doCreateTransparentPass();
		void doInitialiseShadowMaps();
		void doInitialiseLpv();
		void doUpdateShadowMaps( CpuUpdater & updater );
		void doUpdateShadowMaps( GpuUpdater & updater );
		void doUpdateLpv( CpuUpdater & updater );
		void doUpdateLpv( GpuUpdater & updater );
		void doUpdateParticles( CpuUpdater & updater );
		void doUpdateParticles( GpuUpdater & updater );
		ashes::Semaphore const & doRenderShadowMaps( RenderDevice const & device
			, ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderLpv( RenderDevice const & device
			, ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderEnvironmentMaps( RenderDevice const & device
			, ashes::Semaphore const & semaphore );

	private:
		RenderTarget & m_renderTarget;
		RenderDevice const & m_device;
		castor::Size m_size;
		SsaoConfig m_ssaoConfig;
		crg::ImageId m_colourImage;
		crg::ImageViewId m_colourView;
		TextureUnit m_colourTexture;
		crg::ImageId m_depthImage;
		crg::ImageViewId m_depthView;
		TextureUnit m_depthBuffer;
		MatrixUbo m_matrixUbo;
		GpInfoUbo m_gpInfoUbo;
		LpvGridConfigUbo m_lpvConfigUbo;
		LayeredLpvGridConfigUbo m_llpvConfigUbo;
		VoxelizerUbo m_vctConfigUbo;
		crg::FramePass * m_depthPassDecl;
		DepthPass * m_depthPass{};
		VoxelizerUPtr m_voxelizer;
		LightVolumePassResultUPtr m_lpvResult;
		LightVolumePassResultArray m_llpvResult;
		crg::FramePass * m_backgroundPassDesc;
		BackgroundPass * m_backgroundPass;
		OpaquePassResultUPtr m_opaquePassResult;
		crg::FramePass * m_opaquePassDesc;
		RenderTechniquePass * m_opaquePass;
		DeferredRenderingUPtr m_deferredRendering;
		TransparentPassResultUPtr m_transparentPassResult;
		crg::FramePass * m_transparentPassDesc;
		RenderTechniquePass * m_transparentPass;
		WeightedBlendRenderingUPtr m_weightedBlendRendering;
		ashes::SemaphorePtr m_signalFinished;
		ShadowMapUPtr m_directionalShadowMap;
		ShadowMapUPtr m_pointShadowMap;
		ShadowMapUPtr m_spotShadowMap;
		CommandsSemaphore m_clearLpv;
		RenderPassTimerSPtr m_particleTimer;
		ShadowMapLightTypeArray m_allShadowMaps;
		ShadowMapLightTypeArray m_activeShadowMaps;
		LightPropagationVolumesLightType m_lightPropagationVolumes;
		LayeredLightPropagationVolumesLightType m_layeredLightPropagationVolumes;
		LightPropagationVolumesGLightType m_lightPropagationVolumesG;
		LayeredLightPropagationVolumesGLightType m_layeredLightPropagationVolumesG;
	};
}

#endif
