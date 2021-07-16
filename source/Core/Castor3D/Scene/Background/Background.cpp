#include "Castor3D/Scene/Background/Background.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBufferInheritanceInfo.hpp>
#include <ashespp/Core/Device.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	SceneBackground::SceneBackground( Engine & engine
		, Scene & scene
		, castor::String const & name
		, BackgroundType type )
		: castor::OwnedBy< Engine >{ engine }
		, castor::Named{ scene.getName() + name }
		, m_scene{ scene }
		, m_type{ type }
		, m_modelUbo{ getEngine()->getRenderSystem()->getMainRenderDevice()->uboPools->getBuffer< ModelUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
		, m_matrixUbo{ *getEngine()->getRenderSystem()->getMainRenderDevice() }
	{
	}

	SceneBackground::~SceneBackground()
	{
		getEngine()->getRenderSystem()->getMainRenderDevice()->uboPools->putBuffer( m_modelUbo );
	}

	bool SceneBackground::initialise( RenderDevice const & device )
	{
		if ( !m_initialised )
		{
			m_initialised = doInitialise( device );
			castor::String const name = cuT( "Skybox" );
			SamplerSPtr sampler;

			if ( getEngine()->getSamplerCache().has( name ) )
			{
				sampler = getEngine()->getSamplerCache().find( name );
			}
			else
			{
				sampler = getEngine()->getSamplerCache().add( name );
				sampler->setMinFilter( VK_FILTER_LINEAR );
				sampler->setMagFilter( VK_FILTER_LINEAR );
				sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setMinLod( 0.0f );
				sampler->setMaxLod( float( m_texture->getMipmapCount() - 1u ) );

				if ( m_texture->getMipmapCount() > 1u )
				{
					sampler->setMipFilter( VK_SAMPLER_MIPMAP_MODE_LINEAR );
				}
			}

			sampler->initialise( device );
			m_sampler = sampler;

			if ( m_initialised
				&& m_scene.getEngine()->getPassFactory().hasIBL( m_scene.getPassesType() )
				&& m_texture->getLayersCount() == 6u )
			{
				m_ibl = std::make_unique< IblTextures >( m_scene
					, device
					, m_textureId
					, sampler );
				m_ibl->update();
			}

			onChanged( *this );
		}

		return m_initialised;
	}

	void SceneBackground::cleanup( RenderDevice const & device )
	{
		doCleanup();

		device.uboPools->putBuffer( m_modelUbo );

		if ( m_texture )
		{
			m_texture->cleanup();
			m_texture.reset();
		}

		getEngine()->getSamplerCache().remove( cuT( "Skybox" ) );
		m_ibl.reset();
	}

	void SceneBackground::update( CpuUpdater & updater )
	{
		if ( m_initialised )
		{
			auto & camera = *updater.camera;
			static castor::Point3f const Scale{ 1, -1, 1 };
			static castor::Matrix3x3f const Identity{ 1.0f };
			auto node = camera.getParent();
			castor::matrix::setTranslate( m_mtxModel, node->getDerivedPosition() );
			castor::matrix::scale( m_mtxModel, Scale );
			auto & configuration = m_modelUbo.getData();
			configuration.prvModel = configuration.curModel;
			configuration.curModel = m_mtxModel;
			doCpuUpdate( updater );
		}
	}

	void SceneBackground::update( GpuUpdater & updater )
	{
		if ( m_initialised )
		{
			doGpuUpdate( updater );
		}
	}

	void SceneBackground::notifyChanged()
	{
		if ( m_initialised )
		{
			m_initialised = false;
			onChanged( *this );
		}
	}

	void SceneBackground::doGpuUpdate( GpuUpdater & updater )
	{
	}
}
