/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereBackground_H___
#define ___C3DAS_AtmosphereBackground_H___

#include "AtmosphereMultiScatteringPass.hpp"
#include "AtmosphereSkyViewPass.hpp"
#include "AtmosphereTransmittancePass.hpp"
#include "AtmosphereVolumePass.hpp"

#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <Castor3D/Scene/Background/Background.hpp>
#include <Castor3D/Shader/Shaders/GlslBackground.hpp>

namespace atmosphere_scattering
{
	class AtmosphereBackgroundModel
		: public castor3d::shader::BackgroundModel
	{
	public:
		AtmosphereBackgroundModel( sdw::ShaderWriter & writer
			, castor3d::shader::Utils & utils
			, uint32_t & binding
			, uint32_t set );

		static castor3d::shader::BackgroundModelPtr create( sdw::ShaderWriter & writer
			, castor3d::shader::Utils & utils
			, uint32_t & binding
			, uint32_t set );

		sdw::Vec3 computeReflections( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, castor3d::shader::LightMaterial const & material
			, sdw::CombinedImage2DRg32 const & brdf )override;
		sdw::Vec3 computeRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, castor3d::shader::LightMaterial const & material )override;
		sdw::Void mergeReflRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, castor3d::shader::LightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )override;

	public:
		static castor::String const Name;
	};

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
		~AtmosphereBackground()override;
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
			, crg::ImageViewId const & colour
			, crg::ImageViewId const * depth
			, castor3d::UniformBufferOffsetT< castor3d::ModelBufferConfiguration > const & modelUbo
			, castor3d::MatrixUbo const & matrixUbo
			, castor3d::HdrConfigUbo const & hdrConfigUbo
			, castor3d::SceneUbo const & sceneUbo
			, bool clearColour
			, castor3d::BackgroundPassBase *& backgroundPass )override;
		/**
		*\copydoc	castor3d::SceneBackground::write
		*/
		bool write( castor::String const & tabs
			, castor::Path const & folder
			, castor::StringStream & stream )const override;
		/**
		*\copydoc	castor3d::SceneBackground::write
		*/
		castor::String const & getModelName()const override;

		void loadTransmittance( castor::Point2ui const & dimensions );
		void loadMultiScatter( uint32_t dimension );
		void loadAtmosphereVolume( uint32_t dimension );
		void loadSkyView( castor::Point2ui const & dimensions );

		bool isDepthSampled()const override
		{
			return true;
		}

		void setConfiguration( AtmosphereScatteringConfig config )
		{
			m_config = std::move( config );
		}

		auto & getConfiguration()const
		{
			return m_config;
		}

		auto const & getTransmittance()const
		{
			return m_transmittance;
		}

		auto const & getMultiScatter()const
		{
			return m_multiScatter;
		}

		auto const & getSkyView()const
		{
			return m_skyView;
		}

		auto const & getVolume()const
		{
			return m_volume;
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
		/**
		*\copydoc	castor3d::SceneBackground::write
		*/
		void doAddPassBindings( crg::FramePass & pass
			, uint32_t & index )const override;
		/**
		*\copydoc	castor3d::SceneBackground::write
		*/
		void doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const override;
		/**
		*\copydoc	castor3d::SceneBackground::write
		*/
		void doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index )const override;

	private:
		struct VolumePass
		{
			mutable castor3d::UniformBufferOffsetT< CameraConfig > cameraUbo;
			castor3d::Texture atmosphereVolume;
			std::unique_ptr< AtmosphereVolumePass > atmosphereVolumePass;
			crg::FramePass * lastPass;
		};

	private:
		AtmosphereScatteringConfig m_config;
		mutable castor3d::UniformBufferOffsetT< CameraConfig > m_cameraUbo;
		crg::ImageViewId m_depthView;
		castor3d::Texture m_transmittance;
		castor3d::Texture m_multiScatter;
		castor3d::Texture m_skyView;
		castor3d::Texture m_volume;
		std::unique_ptr< AtmosphereTransmittancePass > m_transmittancePass;
		std::unique_ptr< AtmosphereMultiScatteringPass > m_multiScatteringPass;
		std::unique_ptr< AtmosphereSkyViewPass > m_skyViewPass;
		std::unique_ptr< AtmosphereVolumePass > m_volumePass;
		std::unique_ptr< AtmosphereScatteringUbo > m_atmosphereUbo;
	};
}

#endif
