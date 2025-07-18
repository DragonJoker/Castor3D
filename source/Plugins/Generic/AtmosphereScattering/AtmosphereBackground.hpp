/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereBackground_H___
#define ___C3DAS_AtmosphereBackground_H___

#include "AtmosphereCameraUbo.hpp"
#include "AtmosphereMultiScatteringPass.hpp"
#include "CloudsPerlinPass.hpp"
#include "AtmosphereSkyViewPass.hpp"
#include "AtmosphereTransmittancePass.hpp"
#include "AtmosphereVolumePass.hpp"
#include "CloudsCurlPass.hpp"
#include "CloudsResolvePass.hpp"
#include "CloudsVolumePass.hpp"
#include "CloudsWeatherPass.hpp"
#include "CloudsWorleyPass.hpp"

#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <Castor3D/Scene/Background/Background.hpp>
#include <Castor3D/Shader/Shaders/GlslBackground.hpp>

#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

namespace atmosphere_scattering
{
	class AtmosphereBackground
		: public c3d::SceneBackground
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
		AtmosphereBackground( c3d::Engine & engine
			, c3d::Scene & scene );
		~AtmosphereBackground()override;
		/**
		*\copydoc	c3d::SceneBackground::accept
		*/
		void accept( c3d::BackgroundVisitor & visitor )override;
		/**
		*\copydoc	c3d::SceneBackground::accept
		*/
		void accept( c3d::ConfigurationVisitorBase & visitor )override;
		/**
		*\copydoc	c3d::SceneBackground::createBackgroundPass
		*/
		crg::FramePass & createBackgroundPass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::ProgressBar * progress
			, c3d::Extent2D const & size
			, crg::ImageViewIdArray const & colour
			, crg::ImageViewIdArray const & depth
			, crg::ImageViewId const * depthObj
			, c3d::UniformBufferOffsetT< c3d::ModelBufferConfiguration > const & modelUbo
			, c3d::CameraUbo const & cameraUbo
			, c3d::RenderUbo const & renderUbo
			, c3d::SceneUbo const & sceneUbo
			, bool clearColour
			, bool clearDepth
			, bool forceVisible
			, c3d::BackgroundPassBase *& backgroundPass )override;
		/**
		*\copydoc	c3d::SceneBackground::write
		*/
		bool write( c3d::String const & tabs
			, c3d::Path const & folder
			, c3d::StringStream & stream )const override;
		/**
		*\copydoc	c3d::SceneBackground::write
		*/
		c3d::String const & getModelName()const override;

		void loadWorley( uint32_t dimension );
		void loadPerlinWorley( uint32_t dimension );
		void loadCurl( uint32_t dimension );
		void loadWeather( uint32_t dimension );
		void loadTransmittance( c3d::Point2ui const & dimensions );
		void loadMultiScatter( uint32_t dimension );
		void loadAtmosphereVolume( uint32_t dimension );
		void loadSkyView( c3d::Point2ui const & dimensions );

		bool hasScattering()const noexcept override
		{
			return true;
		}

		bool isDepthSampled()const noexcept override
		{
			return true;
		}

		void setSunNode( c3d::SceneNode const & node )noexcept
		{
			m_sunNode = &node;
		}

		void setPlanetNode( c3d::SceneNode const & node )noexcept
		{
			m_planetNode = &node;
		}

		auto getSunNode()const noexcept
		{
			return m_sunNode;
		}

		auto getPlanetNode()const noexcept
		{
			return m_planetNode;
		}

		void setAtmosphereCfg( AtmosphereScatteringConfig config )noexcept
		{
			m_atmosphereCfg = c3d::move( config );
		}

		auto & getAtmosphereCfg()const noexcept
		{
			return m_atmosphereCfg;
		}

		void setWeatherCfg( WeatherConfig config )noexcept
		{
			m_weatherCfg = c3d::move( config );
		}

		auto & getWeatherCfg()const noexcept
		{
			return m_weatherCfg;
		}

		void setCloudsCfg( CloudsConfig config )noexcept
		{
			m_cloudsCfg = c3d::move( config );
		}

		auto & getCloudsCfg()const noexcept
		{
			return m_cloudsCfg;
		}

		auto const & getTransmittance()const noexcept
		{
			return m_transmittance;
		}

		auto const & getMultiScatter()const noexcept
		{
			return m_multiScatter;
		}

		auto const & getSkyViewResolution()const noexcept
		{
			return m_skyViewResolution;
		}

		auto const & getVolumeResolution()const noexcept
		{
			return m_volumeResolution;
		}

		auto const & getWorleyResolution()const noexcept
		{
			return m_worleyResolution;
		}

		auto const & getPerlinWorleyResolution()const noexcept
		{
			return m_perlinWorleyResolution;
		}

		auto const & getCurlResolution()const noexcept
		{
			return m_curlResolution;
		}

		auto const & getWeatherResolution()const noexcept
		{
			return m_weatherResolution;
		}

	private:
		bool doInitialise( c3d::RenderDevice const & device )override;
		void doCleanup()override;
		void doCpuUpdate( c3d::CpuUpdater & updater )const override;
		void doGpuUpdate( c3d::GpuUpdater & updater )const override;
		void doUpload( c3d::UploadData & uploader )override;
		/**
		*\copydoc	c3d::SceneBackground::doAddPassBindings
		*/
		void doAddPassBindings( crg::FramePass & pass
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const override;
		/**
		*\copydoc	c3d::SceneBackground::doAddBindings
		*/
		void doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index )const override;
		/**
		*\copydoc	c3d::SceneBackground::doAddDescriptors
		*/
		void doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const override;

		auto findCameraPass( crg::ImageViewIdArray const & images )const
		{
			for ( auto image : images )
			{
				auto it = m_cameraPasses.find( image.data->image.data );

				if ( it != m_cameraPasses.end() )
				{
					return it;
				}
			}

			return m_cameraPasses.end();
		}

	private:
		struct CameraPasses
		{
			CameraPasses( crg::FramePassGroup & graph
				, c3d::RenderDevice const & device
				, AtmosphereBackground & background
				, crg::FramePass const & transmittancePass
				, crg::FramePass const & multiscatterPass
				, crg::FramePass const & weatherPass
				, crg::ImageViewId const & transmittance
				, crg::ImageViewId const & multiscatter
				, crg::ImageViewId const & worley
				, crg::ImageViewId const & perlinWorley
				, crg::ImageViewId const & curl
				, crg::ImageViewId const & weather
				, crg::ImageViewIdArray const & colour
				, crg::ImageViewId const * depthObj
				, c3d::RenderUbo const & renderUbo
				, c3d::SceneUbo const & sceneUbo
				, AtmosphereScatteringUbo const & atmosphereUbo
				, CloudsUbo const & cloudsUbo
				, c3d::Extent2D const & size
				, c3d::Point2ui const & skyViewResolution
				, uint32_t volumeResolution
				, uint32_t index
				, bool forceVisible
				, c3d::BackgroundPassBase *& backgroundPass );
			~CameraPasses();

			void accept( c3d::ConfigurationVisitorBase & visitor );

			void update( c3d::CpuUpdater & updater
				, c3d::Point3f const & sunDirection
				, c3d::Vector3f const & planetPosition )const;
			void update( c3d::Size const & renderSize
				, c3d::Camera const & camera
				, bool safeBanded
				, c3d::Point3f const & sunDirection
				, c3d::Vector3f const & planetPosition )const;

			c3d::Texture skyView;
			c3d::Texture volume;
			c3d::Texture skyColour;
			c3d::Texture sunColour;
			c3d::Texture cloudsColour;
			c3d::Texture cloudsResult;
			mutable bool camAtmoChanged{ true };
			mutable CameraUbo cameraUbo;
			c3d::RawUniquePtr< AtmosphereSkyViewPass > skyViewPass;
			c3d::RawUniquePtr< AtmosphereVolumePass > volumePass;
			c3d::RawUniquePtr< CloudsVolumePass > volumetricCloudsPass;
			c3d::RawUniquePtr< CloudsResolvePass > cloudsResolvePass;
			crg::FramePass * lastPass;
		};

	private:
		c3d::SceneNode const * m_sunNode{};
		c3d::SceneNode const * m_planetNode{};
		// Clouds
		WeatherConfig m_weatherCfg;
		CloudsConfig m_cloudsCfg;
		c3d::Texture m_worley;
		c3d::Texture m_perlinWorley;
		c3d::Texture m_curl;
		c3d::Texture m_weather;
		mutable bool m_first{ true };
		mutable bool m_generateWorley{ true };
		mutable bool m_generatePerlinWorley{ true };
		mutable bool m_generateCurl{ true };
		mutable bool m_cloudsChanged{ true };
		mutable bool m_weatherChanged{ true };
		c3d::RawUniquePtr< WeatherUbo > m_weatherUbo;
		c3d::RawUniquePtr< CloudsUbo > m_cloudsUbo;
		c3d::RawUniquePtr< CloudsWorleyPass > m_worleyPass;
		c3d::RawUniquePtr< CloudsPerlinPass > m_perlinWorleyPass;
		c3d::RawUniquePtr< CloudsCurlPass > m_curlPass;
		c3d::RawUniquePtr< CloudsWeatherPass > m_weatherPass;
		// Atmosphere
		AtmosphereScatteringConfig m_atmosphereCfg;
		c3d::Texture m_transmittance;
		c3d::Texture m_multiScatter;
		uint32_t m_volumeResolution{ 32u };
		c3d::Point2ui m_skyViewResolution{ 192u, 108u };
		uint32_t m_worleyResolution{ 32u };
		uint32_t m_perlinWorleyResolution{ 32u };
		uint32_t m_curlResolution{ 32u };
		uint32_t m_weatherResolution{ 32u };
		mutable bool m_atmosphereChanged{ true };
		c3d::RawUniquePtr< AtmosphereScatteringUbo > m_atmosphereUbo;
		c3d::RawUniquePtr< AtmosphereTransmittancePass > m_transmittancePass;
		c3d::RawUniquePtr< AtmosphereMultiScatteringPass > m_multiScatteringPass;
		c3d::Map< crg::ImageData const *, c3d::RawUniquePtr< CameraPasses > > m_cameraPasses;
		mutable c3d::PreciseTimer m_timer;
		mutable float m_time{};
	};
}

#endif
