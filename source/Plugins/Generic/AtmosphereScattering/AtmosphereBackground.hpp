/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereBackground_H___
#define ___C3DAS_AtmosphereBackground_H___

#include "AtmosphereMultiScatteringPass.hpp"
#include "AtmosphereTransmittancePass.hpp"
#include "AtmosphereVolumePass.hpp"

#include <Castor3D/Scene/Background/Background.hpp>

namespace atmosphere_scattering
{
	class AtmosphereBackground
		: public castor3d::SceneBackground
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] engine
		*	The engine.
		*\param[in] scene
		*	The parent scene.
		*\param parameters
		*	The background parameters.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] engine
		*	Le moteur.
		*\param[in] scene
		*	La scène parente.
		*\param parameters
		*	Les paramètres du fond.
		*/
		AtmosphereBackground( castor3d::Engine & engine
			, castor3d::Scene & scene );
		/**
		*\copydoc	castor3d::SceneBackground::accept
		*/
		void accept( castor3d::BackgroundVisitor & visitor )override;
		/**
		*\copydoc	castor3d::SceneBackground::createBackgroundPass
		*/
		crg::FramePass & createBackgroundPass( crg::FramePassGroup & graph
			, castor3d::RenderDevice const & device
			, castor3d::ProgressBar * progress
			, VkExtent2D const & size
			, bool usesDepth
			, castor3d::MatrixUbo const & matrixUbo
			, castor3d::SceneUbo const & sceneUbo
			, castor3d::BackgroundPassBase *& backgroundPass )override;
		/**
		*\copydoc	castor3d::SceneBackground::write
		*/
		bool write( castor::String const & tabs
			, castor::Path const & folder
			, castor::StringStream & stream )const override;

		void loadTransmittance( castor::Point2ui const & dimensions );
		void loadMultiScatter( uint32_t dimension );
		void loadAtmosphereVolume( uint32_t dimension );

		void setConfiguration( AtmosphereScatteringConfig config )
		{
			m_config = std::move( config );
		}

		auto const & getTransmittance()const
		{
			return m_transmittance;
		}

		auto const & getMultiScatter()const
		{
			return m_multiScatter;
		}

		auto const & getAtmosphereVolume()const
		{
			return m_atmosphereVolume;
		}

		AtmosphereScatteringUbo const & getAtmosphereUbo()const
		{
			return *m_atmosphereUbo;
		}

	private:
		bool doInitialise( castor3d::RenderDevice const & device )override;
		void doCleanup()override;
		void doCpuUpdate( castor3d::CpuUpdater & updater )const override;
		void doGpuUpdate( castor3d::GpuUpdater & updater )const override;

	private:
		AtmosphereScatteringConfig m_config;
		castor3d::Texture m_transmittance;
		castor3d::Texture m_multiScatter;
		castor3d::Texture m_atmosphereVolume;
		struct BackgroundPasses
		{
			std::unique_ptr< AtmosphereTransmittancePass > transmittance;
			std::unique_ptr< AtmosphereMultiScatteringPass > multiScattering;
			std::unique_ptr< AtmosphereVolumePass > skyView;
			crg::FramePass * lastPass;
		};
		std::map< castor3d::MatrixUbo const *, BackgroundPasses > m_atmospherePasses;
		std::unique_ptr< AtmosphereScatteringUbo > m_atmosphereUbo;
		float m_ratio;
	};
}

#endif
