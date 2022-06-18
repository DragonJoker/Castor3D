/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereBackground_H___
#define ___C3DAS_AtmosphereBackground_H___

#include "AtmosphereCameraUbo.hpp"
#include "AtmosphereMultiScatteringPass.hpp"
#include "AtmosphereSkyViewPass.hpp"
#include "AtmosphereTransmittancePass.hpp"
#include "AtmosphereVolumePass.hpp"

#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <Castor3D/Scene/Background/Background.hpp>
#include <Castor3D/Shader/Shaders/GlslBackground.hpp>

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

		void setNode( castor3d::SceneNode const & node )
		{
			m_node = &node;
		}

		auto getNode()const
		{
			return m_node;
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

		auto const & getSkyViewResolution()const
		{
			return m_skyViewResolution;
		}

		auto const & getVolumeResolution()const
		{
			return m_volumeResolution;
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
			, crg::ImageData const & targetImage
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
			, crg::ImageData const & targetImage
			, uint32_t & index )const override;

	private:
		struct CameraPasses
		{
			CameraPasses( crg::FramePassGroup & graph
				, castor3d::RenderDevice const & device
				, AtmosphereBackground & background
				, crg::FramePass const & transmittancePass
				, crg::ImageViewId const & transmittance
				, AtmosphereScatteringUbo const & atmosphereUbo
				, VkExtent2D const & size
				, castor::Point2ui const & skyViewResolution
				, uint32_t volumeResolution
				, uint32_t index
				, castor3d::BackgroundPassBase *& backgroundPass );
			~CameraPasses();

			castor3d::Texture skyView;
			castor3d::Texture volume;
			mutable CameraUbo cameraUbo;
			std::unique_ptr< AtmosphereSkyViewPass > skyViewPass;
			std::unique_ptr< AtmosphereVolumePass > volumePass;
			crg::FramePass * lastPass;
		};

	private:
		AtmosphereScatteringConfig m_config;
		castor3d::SceneNode const * m_node{};
		crg::ImageViewId m_depthView;
		castor3d::Texture m_transmittance;
		castor3d::Texture m_multiScatter;
		uint32_t m_volumeResolution{ 32u };
		castor::Point2ui m_skyViewResolution{ 192u, 108u };
		std::unique_ptr< AtmosphereScatteringUbo > m_atmosphereUbo;
		std::unique_ptr< AtmosphereTransmittancePass > m_transmittancePass;
		std::unique_ptr< AtmosphereMultiScatteringPass > m_multiScatteringPass;
		std::map< crg::ImageData const *, std::unique_ptr< CameraPasses > > m_cameraPasses;
	};
}

#endif
