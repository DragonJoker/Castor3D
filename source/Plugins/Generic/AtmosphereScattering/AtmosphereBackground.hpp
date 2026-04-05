/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereBackground_H___
#define ___C3DAS_AtmosphereBackground_H___

#include "AtmosphereCameraUbo.hpp"
#include "CloudsResolvePass.hpp"

#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <Castor3D/Scene/Background/Background.hpp>

namespace atmosphere_scattering
{
	struct VolumeData;
	struct VolumeCameraData;
	class CloudsVolumePlugin;

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
			, c3d::Scene & scene
			, CloudsVolumePlugin & plugin );
		~AtmosphereBackground()noexcept override;
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
		void createBackgroundPass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::ProgressBar * progress
			, c3d::Extent2D const & size
			, c3d::Camera const & camera
			, c3d::Texture & colour
			, c3d::Texture const * scattering
			, c3d::Texture const * transmittance
			, c3d::Texture * depth
			, c3d::Texture const * depthObj
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

		c3d::Texture const & getTransmittance()const noexcept;
		c3d::Texture const & getMultiScatter()const noexcept;
		void setAtmosphereCfg( AtmosphereScatteringConfig config )noexcept;
		void setWeatherCfg( WeatherConfig config )noexcept;
		void setCloudsCfg( CloudsConfig config )noexcept;

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

		auto & getVolumeData()const noexcept
		{
			return *m_volumeData;
		}

	private:
		bool doInitialise( c3d::RenderDevice const & device )override;
		void doCleanup()override;
		void doCpuUpdate( c3d::CpuUpdater & updater )override;
		void doGpuUpdate( c3d::GpuUpdater & updater )const override;
		void doUpload( c3d::UploadData & uploader )override;
		/**
		*\copydoc	c3d::SceneBackground::doAddPassBindings
		*/
		void doAddPassBindings( crg::FramePass & pass
			, c3d::Texture * targetImage
			, uint32_t & index )const override;
		/**
		*\copydoc	c3d::SceneBackground::doAddLayoutBindings
		*/
		void doAddLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index )const override;
		/**
		*\copydoc	c3d::SceneBackground::doAddDescriptorWrites
		*/
		void doAddDescriptorWrites( ashes::WriteDescriptorSetArray & descriptorWrites
			, c3d::Texture * targetImage
			, uint32_t & index )const override;

		auto findCameraPass( c3d::Texture const * image )const
		{
			if ( auto it = m_cameraPasses.find( image );
				it != m_cameraPasses.end() )
			{
				return it;
			}

			return m_cameraPasses.end();
		}

	private:
		struct CameraPasses
		{
			CameraPasses( crg::FramePassGroup & graph
				, c3d::RenderDevice const & device
				, AtmosphereBackground & background
				, c3d::Camera const & camera
				, VolumeData & volumeData
				, c3d::Texture const * scattering
				, c3d::Texture const * transmittance
				, c3d::Texture const * depthObj
				, c3d::RenderUbo const & renderUbo
				, c3d::SceneUbo const & sceneUbo
				, c3d::CameraUbo const & mainCameraUbo
				, c3d::Extent2D const & size
				, uint32_t index
				, bool forceVisible
				, c3d::BackgroundPassBase *& backgroundPass );
			~CameraPasses();

			void accept( c3d::ConfigurationVisitorBase & visitor )const;

			void update( c3d::CpuUpdater const & updater
				, c3d::Point3f const & sunDirection
				, c3d::Vector3f const & planetPosition );
			void update( c3d::Size const & renderSize
				, c3d::Camera const & camera
				, bool safeBanded
				, c3d::Point3f const & sunDirection
				, c3d::Vector3f const & planetPosition );

			VolumeCameraData * cameraData;
			c3d::Texture cloudsResult;
			crg::FramePass * lastPass;
			c3d::RawUniquePtr< CloudsResolvePass > cloudsResolvePass;
		};

	private:
		CloudsVolumePlugin * m_plugin;
		VolumeData * m_volumeData;
		c3d::SceneNode const * m_sunNode{};
		c3d::SceneNode const * m_planetNode{};
		bool m_passesCreated{};

		c3d::Map< c3d::Texture const *, c3d::RawUniquePtr< CameraPasses > > m_cameraPasses;
	};
}

#endif
