/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_CloudsVolumePlugin_H___
#define ___C3DAS_CloudsVolumePlugin_H___

#include "AtmosphereModel.hpp"
#include "CloudsModel.hpp"
#include "ScatteringModel.hpp"

#include "AtmosphereMultiScatteringPass.hpp"
#include "CloudsPerlinPass.hpp"
#include "AtmosphereSkyViewPass.hpp"
#include "AtmosphereTransmittancePass.hpp"
#include "AtmosphereVolumePass.hpp"
#include "CloudsCurlPass.hpp"
#include "CloudsWeatherPass.hpp"
#include "CloudsWorleyPass.hpp"

#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>
#include <Castor3D/Render/Volumetric/VolumeComponentPlugin.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

namespace atmosphere_scattering
{
	struct VolumeData;

	struct VolumeCameraData
	{
		c3d::Texture skyView;
		c3d::Texture volume;
		CameraUbo cameraUbo;
		c3d::Texture const * depthObj;

		VolumeCameraData( c3d::RenderDevice const & device
			, crg::ResourcesCache & resources
			, c3d::Texture const * depthObj
			, c3d::Point2ui const & skyViewResolution
			, uint32_t volumeResolution );
		~VolumeCameraData();

		void accept( c3d::ConfigurationVisitorBase & visitor )const;

		void cpuUpdate( c3d::CpuUpdater & updater
			, bool atmosphereChanged
			, c3d::Point3f const & sunDirection
			, c3d::Vector3f const & planetPosition );
		void registerPasses( crg::FramePassGroup & graph
			, VolumeData const & volumeData );

	private:
		c3d::RenderDevice const & m_device;
		bool m_camAtmoChanged{ true };

		c3d::RawUniquePtr< AtmosphereSkyViewPass > m_skyViewPass;
		c3d::RawUniquePtr< AtmosphereVolumePass > m_volumePass;

		c3d::GroupChangeTracked< c3d::Point3f > m_position;
		c3d::GroupChangeTracked< c3d::Quaternion > m_orientation;
	};
	using VolumeCameraDataPtr = c3d::RawUniquePtr< VolumeCameraData >;

	struct VolumeData
	{
		AtmosphereScatteringUbo atmosphereUbo;
		CloudsUbo cloudsUbo;
		WeatherUbo weatherUbo;
		c3d::Texture transmittance;
		c3d::Texture multiScatter;
		c3d::Texture worley;
		c3d::Texture perlinWorley;
		c3d::Texture curl;
		c3d::Texture weather;
		c3d::Map< c3d::Camera const *, VolumeCameraDataPtr > cameraData;

		VolumeData( c3d::Scene & scene );
		~VolumeData();

		void accept( c3d::ConfigurationVisitorBase & visitor );

		void initialise( c3d::SceneNode const * sunNode
			, c3d::SceneNode const * planetNode );
		void cpuUpdate( c3d::CpuUpdater & updater
			, c3d::SceneNode const * sunNode
			, c3d::SceneNode const * planetNode );
		void registerPasses( crg::FramePassGroup & graph );
		VolumeCameraData * registerCamera( c3d::Camera const & camera
			, c3d::Texture const * depthObj );
		void unregisterCamera( c3d::Camera const & camera )noexcept;
		void loadWorley( uint32_t dimension );
		void loadPerlinWorley( uint32_t dimension );
		void loadCurl( uint32_t dimension );
		void loadWeather( uint32_t dimension );
		void loadTransmittance( c3d::Point2ui const & dimensions );
		void loadMultiScatter( uint32_t dimension );
		void loadAtmosphereVolume( uint32_t dimension );
		void loadSkyView( c3d::Point2ui const & dimensions );

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

	public:
		c3d::Scene * m_scene;
		c3d::PreciseTimer m_timer;
		float m_time{};
		AtmosphereScatteringConfig m_atmosphereCfg;
		WeatherConfig m_weatherCfg;
		CloudsConfig m_cloudsCfg;

		uint32_t m_volumeResolution{ 32u };
		c3d::Point2ui m_skyViewResolution{ 192u, 108u };
		uint32_t m_worleyResolution{ 32u };
		uint32_t m_perlinWorleyResolution{ 32u };
		uint32_t m_curlResolution{ 32u };
		uint32_t m_weatherResolution{ 32u };

		bool m_first{ true };
		bool m_generateWorley{ true };
		bool m_generatePerlinWorley{ true };
		bool m_generateCurl{ true };
		bool m_cloudsChanged{ true };
		bool m_weatherChanged{ true };
		bool m_atmosphereChanged{ true };

		c3d::RawUniquePtr< CloudsWorleyPass > m_worleyPass;
		c3d::RawUniquePtr< CloudsPerlinPass > m_perlinWorleyPass;
		c3d::RawUniquePtr< CloudsCurlPass > m_curlPass;
		c3d::RawUniquePtr< CloudsWeatherPass > m_weatherPass;
		c3d::RawUniquePtr< AtmosphereTransmittancePass > m_transmittancePass;
		c3d::RawUniquePtr< AtmosphereMultiScatteringPass > m_multiScatteringPass;

		CheckedAtmosphereScatteringConfig m_atmosphereCheckedConfig;
		CheckedWeatherConfig m_weatherCheckedConfig;
		CheckedCloudsConfig m_cloudsCheckedConfig;
		c3d::GroupChangeTracked< c3d::Point3f > m_sunDirection;
		c3d::GroupChangeTracked< c3d::Point3f > m_planetPosition;
		c3d::GroupChangeTracked< c3d::Point3f > m_mieAbsorption;
	};
	using VolumeDataPtr = c3d::RawUniquePtr< VolumeData >;

	class AtmosphereVolumeTraversal
	{
	public:
		AtmosphereVolumeTraversal( sdw::ShaderWriter & writer
			, c3d::shader::VolumeShaders const & volumeShaders
			, AtmosphereModel & atmosphere );

		void fillType( sdw::type::BaseStruct & type )const;
		void fillInit( sdw::type::BaseStruct & type
			, sdw::expr::ExprList & inits )const;
		void registerVolumes( c3d::shader::Volumes & volumes );
		void initialise( sdw::Vec2 const & pixelCoord, c3d::shader::Ray & ray
			, c3d::shader::Volumes & volumes, c3d::shader::VolumesTraversalResult & result
			, sdw::Vec4 const & depthObj );
		void finalise( c3d::shader::Ray const & ray, c3d::shader::VolumesTraversalResult & result );

		c3d::shader::VolumeTraversalFunc const & getTraversalFunc();
		c3d::shader::VolumeStepFunc const & getStepFunc();
		sdw::RetFloat getSamplesCount( sdw::Float const & range )const;
		sdw::RetFloat listVolumes( c3d::shader::Ray const & ray
			, sdw::Float const & objectId
			, sdw::Float const & linearDepth
			, sdw::Vec2 const & pixPos
			, c3d::shader::Volumes & volumes );

		uint32_t getVolumeType()const noexcept
		{
			return m_volumeType;
		}

	private:
		sdw::ShaderWriter & m_writer;
		c3d::shader::VolumeShaders const & m_volumeShaders;
		AtmosphereModel & m_atmosphere;
		uint32_t m_volumeType{};
		c3d::shader::VolumeTraversalFunc m_traverse;
		c3d::shader::VolumeStepFunc m_step;
		sdw::Function< sdw::Float
			, c3d::shader::InOutRay, sdw::InFloat, sdw::InFloat, sdw::InVec2
			, sdw::InOutParam< sdw::Array< c3d::shader::Volume > >, sdw::InOutUInt > m_listVolumes;
	};

	class CloudsVolumeTraversal
	{
	public:
		CloudsVolumeTraversal( sdw::ShaderWriter & writer
			, c3d::shader::VolumeShaders const & volumeShaders
			, CloudsModel & clouds );

		void fillType( sdw::type::BaseStruct & type )const;
		void fillInit( sdw::type::BaseStruct & type
			, sdw::expr::ExprList & inits )const;
		void registerVolumes( c3d::shader::Volumes & volumes );
		void initialise( sdw::Vec2 const & pixelCoord, c3d::shader::Ray & ray
			, c3d::shader::Volumes & volumes, c3d::shader::VolumesTraversalResult & result
			, sdw::Vec4 const & depthObj );
		void finalise( c3d::shader::Ray const & ray, c3d::shader::VolumesTraversalResult & result );

		c3d::shader::VolumeTraversalFunc const & getTraversalFunc();
		c3d::shader::VolumeStepFunc const & getStepFunc();
		sdw::RetFloat getSamplesCount( sdw::Float const & range )const;
		void listVolumes( c3d::shader::Ray const & ray
			, sdw::Float const & objectId
			, sdw::Float const & linearDepth
			, sdw::Vec2 const & pixCoord
			, c3d::shader::Volumes & volumes );

		uint32_t getVolumeType()const noexcept
		{
			return m_volumeType;
		}

	private:
		sdw::ShaderWriter & m_writer;
		c3d::shader::VolumeShaders const & m_volumeShaders;
		CloudsModel & m_clouds;
		uint32_t m_volumeType{};
		c3d::shader::VolumeTraversalFunc m_traverse;
		c3d::shader::VolumeStepFunc m_step;
		sdw::Function< sdw::Void
			, c3d::shader::InOutRay, sdw::InFloat, sdw::InFloat, sdw::InVec2
			, sdw::InOutParam< sdw::Array< c3d::shader::Volume > >, sdw::InOutUInt > m_listVolumes;
	};

	class CloudsVolumePlugin
		: public c3d::VolumeComponentPlugin
	{
	private:
		class Shader
			: public c3d::shader::VolumeComponentShader
		{
		public:
			Shader( sdw::ShaderWriter & writer
				, c3d::shader::VolumeShaders const & volumeShaders
				, c3d::Extent2D targetExtent
				, bool hasDepth
				, uint32_t & binding );

			void fillType( sdw::type::BaseStruct & type )const override;
			void fillInit( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
			void registerVolumes( c3d::shader::Volumes & volumes )override;
			void initialise( sdw::Vec2 const & pixelCoord, c3d::shader::Ray & ray
				, c3d::shader::Volumes & volumes, c3d::shader::VolumesTraversalResult & result )override;
			void finalise( c3d::shader::Ray const & ray, c3d::shader::VolumesTraversalResult & result )override;

		private:
			sdw::CombinedImage2DRgba32 m_depthMap;
			AtmosphereData m_atmosphereData;
			CloudsData m_cloudsData;
			CameraData m_cameraData;
			AtmosphereModel m_atmosphere;
			ScatteringModel m_scattering;
			c3d::shader::Utils m_utils;
			CloudsModel m_clouds;
			AtmosphereVolumeTraversal m_atmosphereTraversal;
			CloudsVolumeTraversal m_cloudsTraversal;
		};

	public:
		using VolumeComponentPlugin::VolumeComponentPlugin;

		void registerScenePasses( crg::ResourcesCache & resources
			, crg::FramePassGroup & graph
			, c3d::Scene const & scene )const override;
		void registerCameraPasses( crg::ResourcesCache & resources
			, crg::FramePassGroup & graph
			, c3d::Camera const & camera )const override;
		void registerBindings( crg::FramePass & pass
			, c3d::Camera const & camera
			, uint32_t & bindingId )const override;
		VolumeData * registerScene( c3d::Scene & scene );
		void unregisterScene( c3d::Scene & scene )noexcept;
		void registerCamera( c3d::Camera const & camera
			, c3d::Texture const * depthObj )override;

		c3d::shader::VolumeComponentShaderPtr createComponentsShader( sdw::ShaderWriter & writer
			, c3d::shader::VolumeShaders const & volumeShaders
			, c3d::Extent2D const & targetExtent
			, bool hasDepth
			, uint32_t & bindingId )const override
		{
			return c3d::makeRawUnique< Shader >( writer, volumeShaders, targetExtent, hasDepth, bindingId );
		}

		static c3d::VolumeComponentPluginUPtr createPlugin( c3d::VolumeComponentRegister const & volumeComponents )
		{
			return c3d::makeUniqueDerived< c3d::VolumeComponentPlugin, CloudsVolumePlugin >( volumeComponents );
		}


	public:
		static c3d::String const TypeName;

	private:
		c3d::Map< c3d::Scene const *, VolumeDataPtr > m_sceneData;
	};
}

#endif
