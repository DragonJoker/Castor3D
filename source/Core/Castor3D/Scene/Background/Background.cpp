#include "Castor3D/Scene/Background/Background.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Cache/Cache.hpp"
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
	{
	}

	SceneBackground::~SceneBackground()
	{
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
				m_ibl->update( *device.graphicsData() );
			}

			onChanged( *this );
		}

		return m_initialised;
	}

	void SceneBackground::cleanup( RenderDevice const & device )
	{
		doCleanup();

		if ( m_texture )
		{
			m_texture->cleanup();
			m_texture.reset();
		}

		getEngine()->getSamplerCache().remove( cuT( "Skybox" ) );
		m_ibl.reset();
	}

	void SceneBackground::update( CpuUpdater & updater )const
	{
		if ( m_initialised )
		{
			static castor::Point3f const Scale{ 1, -1, 1 };
			static castor::Matrix3x3f const Identity{ 1.0f };

			auto & camera = *updater.camera;
			auto node = camera.getParent();

			castor::matrix::setTranslate( updater.bgMtxModl, node->getDerivedPosition() );
			castor::matrix::scale( updater.bgMtxModl, Scale );
			doCpuUpdate( updater, updater.bgMtxView, updater.bgMtxProj );
		}
	}

	void SceneBackground::update( GpuUpdater & updater )const
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
}
