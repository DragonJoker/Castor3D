#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/GeometryInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

namespace castor3d
{
	//*********************************************************************************************

	LightPropagationVolumesBase::LightLpv::LightLpv( RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, LightVolumePassResult const & injection
		, uint32_t cascadeIndex
		, float lpvCellSize
		, TextureUnit * geometry )
		: lpvLightConfigUbo{ device }
		, lpvCellSize{ lpvCellSize }
	{
		lightInjectionPass = castor::makeUnique< LightInjectionPass >( *device.renderSystem.getEngine()
			, device
			, name
			, lightCache
			, lightType
			, smResult
			, lpvGridConfigUbo
			, lpvLightConfigUbo
			, injection
			, device.renderSystem.getEngine()->getLpvGridSize()
			, cascadeIndex );

		if ( geometry )
		{
			geometryInjectionPass = castor::makeUnique< GeometryInjectionPass >( *device.renderSystem.getEngine()
				, device
				, name
				, lightCache
				, lightType
				, smResult
				, lpvGridConfigUbo
				, lpvLightConfigUbo
				, *geometry
				, device.renderSystem.getEngine()->getLpvGridSize()
				, cascadeIndex );
		}
	}

	bool LightPropagationVolumesBase::LightLpv::update( CpuUpdater & updater )
	{
		auto & light = *updater.light;
		auto changed = light.hasChanged()
			|| light.getLpvConfig().indirectAttenuation.isDirty()
			|| light.getLpvConfig().texelAreaModifier.isDirty();

		if ( changed )
		{
			lpvLightConfigUbo.cpuUpdate( light, lpvCellSize );
		}

		return changed;
	}

	//*********************************************************************************************

	LightPropagationVolumesBase::LightPropagationVolumesBase( Scene const & scene
		, LightType lightType
		, RenderDevice const & device
		, ShadowMapResult const & smResult
		, LightVolumePassResult const & lpvResult
		, LpvGridConfigUbo & lpvGridConfigUbo
		, bool geometryVolumes )
		: castor::Named{ "LightPropagationVolumes" + ( geometryVolumes ? castor::String( "G" ) : castor::String( "" ) ) }
		, m_scene{ scene }
		, m_engine{ *m_scene.getEngine() }
		, m_device{ device }
		, m_smResult{ smResult }
		, m_lpvResult{ lpvResult }
		, m_lightType{ lightType }
		, m_lpvGridConfigUbo{ lpvGridConfigUbo }
		, m_geometryVolumes{ geometryVolumes }
		, m_injection{ m_engine
			, m_device
			, this->getName() + "Injection"
			, m_engine.getLpvGridSize() }
		, m_geometry{ ( geometryVolumes
			? GeometryInjectionPass::createResult( m_engine
				, m_device
				, this->getName()
				, 0u
				, m_engine.getLpvGridSize() )
			: TextureUnit{ m_engine } ) }
		, m_propagate{ LightVolumePassResult{ m_engine
				, m_device
				, this->getName() + "Propagate0"
				, m_engine.getLpvGridSize() }
			, LightVolumePassResult{ m_engine
				, m_device
				, this->getName() + "Propagate1"
				, m_engine.getLpvGridSize() } }
	{
	}

	void LightPropagationVolumesBase::initialise()
	{
		if ( !m_initialised
			&& m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLpvG
					: GlobalIlluminationType::eLpv ) ) )
		{
			auto const cascadeIndex = shader::DirectionalMaxCascadesCount - 2u;
			auto & lightCache = m_scene.getLightCache();
			m_aabb = m_scene.getBoundingBox();
			m_injection.initialise( m_device );
			m_lightPropagationPasses = { castor::makeUnique< LightPropagationPass >( m_device
					, this->getName()
					, cuT( "NoOccNoBlend" )
					, false
					, m_engine.getLpvGridSize()
					, BlendMode::eNoBlend )
				, castor::makeUnique< LightPropagationPass >( m_device
					, this->getName()
					, ( m_geometryVolumes
						? castor::String{ cuT( "OccBlend" ) }
						: castor::String{ cuT( "NoOccBlend" ) } )
					, m_geometryVolumes
					, m_engine.getLpvGridSize()
					, BlendMode::eAdditive ) };
			TextureUnit * geometry{ nullptr };

			if ( m_geometryVolumes )
			{
				geometry = &m_geometry;
				geometry->initialise( m_device );
			}

			for ( auto & propagate : m_propagate )
			{
				propagate.initialise( m_device );
			}

			LightPropagationPass & passNoOcc = *m_lightPropagationPasses[0u];
			auto & passOcc = *m_lightPropagationPasses[1u];
			uint32_t propIndex = 0u;
			passNoOcc.registerPassIO( nullptr
				, m_injection
				, m_lpvGridConfigUbo
				, m_lpvResult
				, m_propagate[propIndex] );

			for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
			{
				passOcc.registerPassIO( geometry
					, m_propagate[propIndex]
					, m_lpvGridConfigUbo
					, m_lpvResult
					, m_propagate[1u - propIndex] );
				propIndex = 1u - propIndex;
			}

			passOcc.initialisePasses();
			passNoOcc.initialisePasses();

			m_clearCommands = CommandsSemaphore{ m_device, this->getName() + cuT( "Clear" ) };
			ashes::CommandBuffer & cmd = *m_clearCommands.commandBuffer;
			auto clearTex = [&cmd]( LightVolumePassResult const & result
				, LpvTexture tex )
			{
				cmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, VK_PIPELINE_STAGE_TRANSFER_BIT
					, result[tex].getTexture()->getDefaultView().getSampledView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
				cmd.clear( result[tex].getTexture()->getDefaultView().getSampledView(), getClearValue( tex ).color );
				cmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, result[tex].getTexture()->getDefaultView().getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
			};
			cmd.begin();
			cmd.beginDebugBlock(
				{
					"Lighting - " + this->getName() + " - Clear",
					castor3d::makeFloatArray( m_engine.getNextRainbowColour() ),
				} );
			clearTex( m_injection, LpvTexture::eR );
			clearTex( m_injection, LpvTexture::eG );
			clearTex( m_injection, LpvTexture::eB );
			clearTex( m_propagate[0u], LpvTexture::eR );
			clearTex( m_propagate[0u], LpvTexture::eG );
			clearTex( m_propagate[0u], LpvTexture::eB );
			clearTex( m_propagate[1u], LpvTexture::eR );
			clearTex( m_propagate[1u], LpvTexture::eG );
			clearTex( m_propagate[1u], LpvTexture::eB );
			cmd.endDebugBlock();
			cmd.end();
			m_initialised = true;
		}
	}

	void LightPropagationVolumesBase::cleanup()
	{
		m_initialised = false;
		m_clearCommands = {};
		m_lightLpvs.clear();
		m_lightPropagationPasses = {};
	}

	void LightPropagationVolumesBase::registerLight( LightSPtr light )
	{
		auto const cascadeIndex = shader::DirectionalMaxCascadesCount - 2u;
		auto it = m_lightLpvs.find( light );

		if ( it == m_lightLpvs.end() )
		{
			m_lightLpvs.emplace( light
				, LightLpv{ m_device
					, this->getName() + light->getName()
					, light->getScene()->getLightCache()
					, m_lightType
					, m_smResult
					, m_lpvGridConfigUbo
					, m_injection
					, cascadeIndex
					, m_lpvGridConfigUbo.getUbo().getData().minVolumeCorner->w
					, ( m_geometryVolumes
						? &m_geometry
						: nullptr ) } );
		}
	}

	void LightPropagationVolumesBase::update( CpuUpdater & updater )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLpvG
					: GlobalIlluminationType::eLpv ) ) )
		{
			return;
		}

		auto & camera = *updater.camera;
		auto aabb = m_scene.getBoundingBox();
		auto camPos = camera.getParent()->getDerivedPosition();
		castor::Point3f camDir{ 0, 0, 1 };
		camera.getParent()->getDerivedOrientation().transform( camDir, camDir );
		auto changed = m_aabb != aabb
			|| m_cameraPos != camPos
			|| m_cameraDir != camDir;

		for ( auto & lightLpv : m_lightLpvs )
		{
			updater.light = lightLpv.first;
			changed = lightLpv.second.update( updater )
				|| changed;
		}

		if ( changed )
		{
			m_aabb = aabb;
			m_cameraPos = camPos;
			m_cameraDir = camDir;
			m_lpvGridConfigUbo.cpuUpdate( m_aabb
				, m_cameraPos
				, m_cameraDir
				, m_engine.getLpvGridSize()
				, m_scene.getLpvIndirectAttenuation() );
		}
	}

	ashes::Semaphore const & LightPropagationVolumesBase::render( ashes::Semaphore const & toWait )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLpvG
					: GlobalIlluminationType::eLpv ) )
			|| m_lightLpvs.empty() )
		{
			return toWait;
		}

		auto result = &toWait;
		result = &m_clearCommands.submit( *m_device.graphicsQueue, *result );

		for ( auto & lightLpv : m_lightLpvs )
		{
			result = &lightLpv.second.lightInjectionPass->compute( *result );

			if ( m_geometryVolumes )
			{
				result = &lightLpv.second.geometryInjectionPass->compute( *result );
			}
		}

		result = &m_lightPropagationPasses[0]->compute( *result, 0u );

		for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
		{
			result = &m_lightPropagationPasses[1]->compute( *result, i - 1u );
		}

		return *result;
	}

	void LightPropagationVolumesBase::accept( PipelineVisitorBase & visitor )
	{
		if ( m_initialised )
		{
			for ( auto & lightLpv : m_lightLpvs )
			{
				lightLpv.second.lightInjectionPass->accept( visitor );

				if ( m_geometryVolumes )
				{
					lightLpv.second.geometryInjectionPass->accept( visitor );
				}
			}

			for ( auto & pass : m_lightPropagationPasses )
			{
				pass->accept( visitor );
			}
		}
	}

	//*********************************************************************************************
}