/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereBackground_H___
#define ___C3DAS_AtmosphereBackground_H___

#include "AtmosphereScatteringUbo.hpp"

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
		std::unique_ptr< castor3d::BackgroundPassBase > createBackgroundPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, castor3d::RenderDevice const & device
			, VkExtent2D const & size
			, bool usesDepth )override;
		/**
		*\copydoc	castor3d::SceneBackground::write
		*/
		bool write( castor::String const & tabs
			, castor::Path const & folder
			, castor::StringStream & stream )const override;

		void loadTransmittance( castor::Path const & folder
			, castor::Path const & relative
			, castor::Point2ui const & dimensions
			, castor::PixelFormat format );
		void loadInscaterring( castor::Path const & folder
			, castor::Path const & relative
			, castor::Point3ui const & dimensions
			, castor::PixelFormat format );
		void loadIrradiance( castor::Path const & folder
			, castor::Path const & relative
			, castor::Point2ui const & dimensions
			, castor::PixelFormat format );

		auto const & getTransmittance()const
		{
			return m_transmittance;
		}

		auto const & getInscattering()const
		{
			return m_inscattering;
		}

		auto const & getIrradiance()const
		{
			return m_irradiance;
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
		AtmosphereScatteringUboConfiguration m_config;
		template< size_t CountT >
		struct ImageT
		{
			castor::Path path;
			castor::Point< uint32_t, CountT > dimensions;
			castor::PixelFormat format;
			castor3d::TextureLayoutUPtr texture;
		};
		ImageT< 2u > m_transmittance;
		ImageT< 3u > m_inscattering;
		ImageT< 2u > m_irradiance;
		std::unique_ptr< AtmosphereScatteringUbo > m_atmosphereUbo;
		float m_ratio;
	};
}

#endif
