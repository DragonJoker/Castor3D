#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"

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
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

namespace castor3d
{
	//*********************************************************************************************

	LayeredLightPropagationVolumesBase::LightLpv::LightLpv( RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUboArray const & lpvGridConfigUbos
		, std::vector< LightVolumePassResult > const & injection
		, crg::ImageIdArray const * geometry
		, std::vector< float > lpvCellSizes )
		: lpvCellSizes{ std::move( lpvCellSizes ) }
	{
		auto & engine = *device.renderSystem.getEngine();

		for ( uint32_t cascade = 0u; cascade < CascadeCount; ++cascade )
		{
			lpvLightConfigUbos.emplace_back( device );
			lightInjectionPasses.emplace_back( engine
				, device
				, name
				, lightCache
				, lightType
				, smResult
				, lpvGridConfigUbos[cascade]
				, lpvLightConfigUbos[cascade]
				, injection[cascade]
				, engine.getLpvGridSize()
				, CascadeCount - cascade );

			if ( geometry )
			{
				geometryInjectionPasses.emplace_back( engine
					, device
					, name
					, lightCache
					, lightType
					, smResult
					, lpvGridConfigUbos[cascade]
					, lpvLightConfigUbos[cascade]
					, ( *geometry )[cascade]
					, engine.getLpvGridSize()
					, CascadeCount - cascade );
			}
		}
	}

	bool LayeredLightPropagationVolumesBase::LightLpv::update( CpuUpdater & updater )
	{
		auto & light = *updater.light;
		auto changed = light.hasChanged()
			|| light.getLpvConfig().indirectAttenuation.isDirty()
			|| light.getLpvConfig().texelAreaModifier.isDirty();

		if ( changed )
		{
			uint32_t index = 0;

			for ( auto & lpvLightConfigUbo : lpvLightConfigUbos )
			{
				lpvLightConfigUbo.cpuUpdate( light, lpvCellSizes[index++] );
			}
		}

		return changed;
	}

	//*********************************************************************************************

	LayeredLightPropagationVolumesBase::LayeredLightPropagationVolumesBase( crg::FrameGraph & graph
		, Scene const & scene
		, LightType lightType
		, RenderDevice const & device
		, ShadowMapResult const & smResult
		, LightVolumePassResultArray const & lpvResult
		, LayeredLpvGridConfigUbo & lpvGridConfigUbo
		, bool geometryVolumes )
		: castor::Named{ "LayeredLightPropagationVolumes" + ( geometryVolumes ? castor::String( "G" ) : castor::String( "" ) ) }
		, m_scene{ scene }
		, m_engine{ *m_scene.getEngine() }
		, m_device{ device }
		, m_smResult{ smResult }
		, m_lpvResult{ lpvResult }
		, m_lightType{ lightType }
		, m_lpvGridConfigUbo{ lpvGridConfigUbo }
		, m_geometryVolumes{ geometryVolumes }
		, m_propagate
		{
			LightVolumePassResult{ m_engine.getGraphResourceHandler(), m_device, getName() + "Propagate0", m_engine.getLpvGridSize() },
			LightVolumePassResult{ m_engine.getGraphResourceHandler(), m_device, getName() + "Propagate1", m_engine.getLpvGridSize() },
		}
	{
		for ( uint32_t cascade = 0u; cascade < CascadeCount; ++cascade )
		{
			m_lpvGridConfigUbos.emplace_back( m_device );
			m_injection.emplace_back( m_engine.getGraphResourceHandler()
				, m_device
				, this->getName() + "Injection" + castor::string::toString( cascade )
				, m_engine.getLpvGridSize() );
			m_geometry.emplace_back( m_geometryVolumes
				? GeometryInjectionPass::createResult( m_engine.getGraphResourceHandler()
					, m_device
					, this->getName()
					, cascade
					, m_engine.getLpvGridSize() )
				: crg::ImageId{} );
		}
	}

	void LayeredLightPropagationVolumesBase::initialise()
	{
		if ( !m_initialised
			&& m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLayeredLpvG
					: GlobalIlluminationType::eLayeredLpv ) ) )
		{
			auto & lightCache = m_scene.getLightCache();
			m_aabb = m_scene.getBoundingBox();
			m_lightPropagationPasses = { castor::makeUnique< LightPropagationPass >( m_device
				, this->getName()
				, "NoOcc"
				, false
				, m_engine.getLpvGridSize()
				, BlendMode::eAdditive )
				, castor::makeUnique< LightPropagationPass >( m_device
					, this->getName()
					, ( m_geometryVolumes
						? castor::String{ cuT( "OccBlend" ) }
						: castor::String{ cuT( "NoOccBlend" ) } )
					, m_geometryVolumes
					, m_engine.getLpvGridSize()
					, BlendMode::eAdditive ) };

			auto & passNoOcc = *m_lightPropagationPasses[0u];
			auto & passOcc = *m_lightPropagationPasses[1u];
			auto clearTex = []( ashes::CommandBuffer const & cmd
				, LightVolumePassResult const & result
				, LpvTexture tex )
			{
				// TODO: CRG
				//cmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				//	, VK_PIPELINE_STAGE_TRANSFER_BIT
				//	, result[tex].getTexture()->getDefaultView().getSampledView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
				//cmd.clear( result[tex].getTexture()->getDefaultView().getSampledView(), getClearValue( tex ).color );
				//cmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				//	, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				//	, result[tex].getTexture()->getDefaultView().getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
			};

			m_clearCommand = CommandsSemaphore{ m_device
				, this->getName() + cuT( "ClearInjection" ) };
			ashes::CommandBuffer & cmd = *m_clearCommand.commandBuffer;
			cmd.begin();
			cmd.beginDebugBlock(
				{
					"Lighting - " + this->getName() + " - Clear Injection",
					castor3d::makeFloatArray( m_engine.getNextRainbowColour() ),
				} );

			for ( auto & injection : m_injection )
			{
				clearTex( cmd, injection, LpvTexture::eR );
				clearTex( cmd, injection, LpvTexture::eG );
				clearTex( cmd, injection, LpvTexture::eB );
			}

			cmd.endDebugBlock();
			cmd.end();

			for ( uint32_t cascade = 0u; cascade < CascadeCount; ++cascade )
			{
				crg::ImageId * geometry{ nullptr };

				if ( m_geometryVolumes )
				{
					geometry = &m_geometry[cascade];
				}

				uint32_t propIndex = 0u;
				passNoOcc.registerPassIO( nullptr
					, m_injection[cascade]
					, m_lpvGridConfigUbos[cascade]
					, *m_lpvResult[cascade]
					, m_propagate[propIndex] );

				for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
				{
					passOcc.registerPassIO( geometry
						, m_propagate[propIndex]
						, m_lpvGridConfigUbos[cascade]
						, *m_lpvResult[cascade]
						, m_propagate[1u - propIndex] );
					propIndex = 1u - propIndex;
				}

				m_clearCommands.emplace_back( m_device
					, this->getName() + cuT( "ClearPropagate" ) + castor::string::toString( cascade ) );
				auto & clearCommands = m_clearCommands[cascade];
				ashes::CommandBuffer & cmd = *clearCommands.commandBuffer;
				cmd.begin();
				cmd.beginDebugBlock(
					{
						"Lighting - " + this->getName() + " - Clear Propagate " + castor::string::toString( cascade ),
						castor3d::makeFloatArray( m_engine.getNextRainbowColour() ),
					} );

				for ( auto & tex : m_propagate )
				{
					clearTex( cmd, tex, LpvTexture::eR );
					clearTex( cmd, tex, LpvTexture::eG );
					clearTex( cmd, tex, LpvTexture::eB );
				}

				cmd.endDebugBlock();
				cmd.end();
			}

			passNoOcc.initialisePasses();
			passOcc.initialisePasses();
			m_initialised = true;
		}
	}

	void LayeredLightPropagationVolumesBase::cleanup()
	{
		m_initialised = false;
		m_clearCommands.clear();
		m_clearCommand = {};
		m_lightPropagationPasses = {};
		m_lightLpvs.clear();
	}

	void LayeredLightPropagationVolumesBase::registerLight( LightSPtr light )
	{
		auto it = m_lightLpvs.find( light );

		if ( it == m_lightLpvs.end() )
		{
			m_lightLpvs.emplace( light
				, LightLpv{ m_device
					, this->getName() + light->getName()
					, light->getScene()->getLightCache()
					, m_lightType
					, m_smResult
					, m_lpvGridConfigUbos
					, m_injection
					, ( m_geometryVolumes
						? &m_geometry
						: nullptr )
					, { m_lpvGridConfigUbo.getUbo().getData().allMinVolumeCorners[0]->w
						, m_lpvGridConfigUbo.getUbo().getData().allMinVolumeCorners[1]->w
						, m_lpvGridConfigUbo.getUbo().getData().allMinVolumeCorners[2]->w } } );
		}
	}

	void LayeredLightPropagationVolumesBase::update( CpuUpdater & updater )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLayeredLpvG
					: GlobalIlluminationType::eLayeredLpv ) ) )
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
			auto cellSize = std::max( std::max( m_aabb.getDimensions()->x
				, m_aabb.getDimensions()->y )
				, m_aabb.getDimensions()->z ) / m_engine.getLpvGridSize();
			std::array< castor::Grid, CascadeCount > grids;
			castor::Grid grid{ m_engine.getLpvGridSize(), cellSize, m_aabb.getMax(), m_aabb.getMin(), 1.0f, 0 };
			std::array< float, CascadeCount > const scales{ 1.0f, 0.65f, 0.4f };

			for ( auto i = 0u; i < CascadeCount; ++i )
			{
				grids[i] = m_lpvGridConfigUbos[i].cpuUpdate( i
					, scales[i]
					, grid
					, m_aabb
					, m_cameraPos
					, m_cameraDir
					, m_scene.getLpvIndirectAttenuation() );
			}

			m_lpvGridConfigUbo.cpuUpdate( grids
				, m_scene.getLpvIndirectAttenuation() );
		}
	}

	ashes::Semaphore const & LayeredLightPropagationVolumesBase::render( ashes::Semaphore const & toWait )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLayeredLpvG
					: GlobalIlluminationType::eLayeredLpv ) )
			|| m_lightLpvs.empty() )
		{
			return toWait;
		}

		auto result = &toWait;
		result = &m_clearCommand.submit( *m_device.graphicsQueue, *result );

		for ( auto & lightLpv : m_lightLpvs )
		{
			for ( auto & pass : lightLpv.second.lightInjectionPasses )
			{
				result = &pass.compute( *result );
			}

			for ( auto & pass : lightLpv.second.geometryInjectionPasses )
			{
				result = &pass.compute( *result );
			}
		}

		auto & passNoOcc = *m_lightPropagationPasses[0u];
		auto & passOcc = *m_lightPropagationPasses[1u];
		uint32_t occPassIndex = 0u;

		for ( uint32_t cascade = 0u; cascade < CascadeCount; ++cascade )
		{
			result = &m_clearCommands[cascade].submit( *m_device.graphicsQueue, *result );
			result = &passNoOcc.compute( *result, cascade );

			for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i, ++occPassIndex )
			{
				result = &passOcc.compute( *result, occPassIndex );
			}
		}

		return *result;
	}

	void LayeredLightPropagationVolumesBase::accept( PipelineVisitorBase & visitor )
	{
		if ( m_initialised )
		{
			for ( auto & lightLpv : m_lightLpvs )
			{
				lightLpv.second.lightInjectionPasses[0].accept( visitor );

				if ( m_geometryVolumes )
				{
					lightLpv.second.geometryInjectionPasses[0].accept( visitor );
				}
			}

			for ( auto & pass : m_lightPropagationPasses )
			{
				if ( pass )
				{
					pass->accept( visitor );
				}
			}
		}
	}
}