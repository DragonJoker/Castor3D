/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LayeredLightPropagationVolumes_H___
#define ___C3D_LayeredLightPropagationVolumes_H___

#include "LightPropagationVolumesModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"

#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/GeometryInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Passes/DownscalePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Ubos/LpvConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvConfigUbo.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <array>

namespace castor3d
{
	template< bool GeometryVolumesT >
	class LayeredLightPropagationVolumesT
		: public castor::Named
	{
	public:
		static constexpr uint32_t MaxPropagationSteps = 8u;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	lpResult	The light pass result.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	lpResult	Le résultat de la passe d'éclairage.
		 *\param[in]	gpInfoUbo	L'UBO de la geometry pass.
		 */
		LayeredLightPropagationVolumesT( Scene const & scene
			, LightType lightType
			, RenderDevice const & device
			, ShadowMapResult const & smResult
			, LightVolumePassResult const & lpvResult )
			: castor::Named{ "LayeredLightPropagationVolumes" + ( GeometryVolumesT ? castor::String( "G" ) : castor::String( "" ) ) }
			, m_scene{ scene }
			, m_engine{ *m_scene.getEngine() }
			, m_device{ device }
			, m_smResult{ smResult }
			, m_lpvResult{ lpvResult }
			, m_lightType{ lightType }
			, m_lpvConfigUbo{ m_device }
			, m_propagate
			{
				LightVolumePassResult{ m_engine, m_device, this->getName() + "Propagate0", m_engine.getLpvGridSize() },
				LightVolumePassResult{ m_engine, m_device, this->getName() + "Propagate1", m_engine.getLpvGridSize() },
			}
		{
			for ( uint32_t cascade = 0u; cascade < shader::DirectionalMaxCascadesCount; ++cascade )
			{
				m_injection.emplace_back( m_engine
					, m_device
					, this->getName() + "Injection" + castor::string::toString( cascade )
					, m_engine.getLpvGridSize() );
				m_geometry.emplace_back( GeometryVolumesT
					? GeometryInjectionPass::createResult( m_engine
						, m_device
						, this->getName()
						, cascade
						, m_engine.getLpvGridSize() )
					: TextureUnit{ m_engine } );
				m_lpvConfigUbos.emplace_back( m_device, cascade );
			}
		}

		void initialise()
		{
			if ( !m_initialised
				&& m_scene.needsGlobalIllumination( m_lightType
					, ( GeometryVolumesT
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
						, ( GeometryVolumesT
							? castor::String{ cuT( "OccBlend" ) }
							: castor::String{ cuT( "NoOccBlend" ) } )
						, GeometryVolumesT
						, m_engine.getLpvGridSize()
						, BlendMode::eAdditive ) };

				auto & passNoOcc = *m_lightPropagationPasses[0u];
				auto & passOcc = *m_lightPropagationPasses[1u];
				auto clearTex = []( ashes::CommandBuffer const & cmd
					, LightVolumePassResult const & result
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

				m_clearCommand = CommandsSemaphore{ m_device
					, this->getName() + cuT( "ClearAccumulation" ) };
				ashes::CommandBuffer & cmd = *m_clearCommand.commandBuffer;
				cmd.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
				cmd.beginDebugBlock(
					{
						"Lighting - " + this->getName() + " - Clear Accumulation",
						castor3d::makeFloatArray( m_engine.getNextRainbowColour() ),
					} );
				clearTex( cmd, m_lpvResult, LpvTexture::eR );
				clearTex( cmd, m_lpvResult, LpvTexture::eG );
				clearTex( cmd, m_lpvResult, LpvTexture::eB );

				cmd.endDebugBlock();
				cmd.end();

				for ( uint32_t cascade = 0u; cascade < shader::DirectionalMaxCascadesCount; ++cascade )
				{
					m_lightInjectionPasses.emplace_back( m_engine
						, m_device
						, this->getName()
						, lightCache
						, m_lightType
						, m_smResult
						, m_lpvConfigUbos[cascade]
						, m_injection[cascade]
						, m_engine.getLpvGridSize()
						, cascade );
					TextureUnit * geometry{ nullptr };

					if constexpr ( GeometryVolumesT )
					{
						m_geometryInjectionPasses.emplace_back( m_engine
							, m_device
							, this->getName()
							, lightCache
							, m_lightType
							, m_smResult
							, m_lpvConfigUbos[cascade]
							, m_geometry[cascade]
							, m_engine.getLpvGridSize()
							, cascade );
						geometry = &m_geometry[cascade];
					}

					uint32_t propIndex = 0u;
					passNoOcc.registerPassIO( nullptr
						, m_injection[cascade]
						, m_lpvConfigUbos[cascade]
						, m_lpvResult
						, m_propagate[propIndex] );

					for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
					{
						passOcc.registerPassIO( geometry
							, m_propagate[propIndex]
							, m_lpvConfigUbos[cascade]
							, m_lpvResult
							, m_propagate[1u - propIndex] );
						propIndex = 1u - propIndex;
					}

					m_clearCommands.emplace_back( m_device
						, this->getName() + cuT( "Clear" ) + castor::string::toString( cascade ) );
					auto & clearCommands = m_clearCommands[cascade];
					ashes::CommandBuffer & cmd = *clearCommands.commandBuffer;
					cmd.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
					cmd.beginDebugBlock(
						{
							"Lighting - " + this->getName() + " - Clear " + castor::string::toString( cascade ),
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

		void cleanup()
		{
			m_initialised = false;
			m_clearCommands.clear();
			m_clearCommand = {};
			m_lightPropagationPasses = {};
			m_geometryInjectionPasses.clear();
			m_lightInjectionPasses.clear();
		}

		void update( CpuUpdater & updater )
		{
			if ( !m_initialised
				|| !m_scene.needsGlobalIllumination( m_lightType
					, ( GeometryVolumesT
						? GlobalIlluminationType::eLayeredLpvG
						: GlobalIlluminationType::eLayeredLpv ) ) )
			{
				return;
			}

			auto & camera = *updater.camera;
			auto & light = *updater.light;
			auto aabb = m_scene.getBoundingBox();
			auto camPos = camera.getParent()->getDerivedPosition();
			castor::Point3f camDir{ 0, 0, 1 };
			camera.getParent()->getDerivedOrientation().transform( camDir, camDir );

			if ( m_aabb != aabb
				|| m_cameraPos != camPos
				|| m_cameraDir != camDir
				|| m_lightIndex != light.getShadowMapIndex()
				|| light.hasChanged()
				|| light.getLpvConfig().indirectAttenuation.isDirty()
				|| light.getLpvConfig().texelAreaModifier.isDirty() )
			{
				auto & directional = *light.getDirectionalLight();
				m_lightIndex = light.getShadowMapIndex();
				m_aabb = aabb;
				m_cameraPos = camPos;
				m_cameraDir = camDir;
				auto cellSize = std::max( std::max( m_aabb.getDimensions()->x
					, m_aabb.getDimensions()->y )
					, m_aabb.getDimensions()->z ) / m_engine.getLpvGridSize();
				std::array< castor::Grid, shader::DirectionalMaxCascadesCount > grids;
				castor::Grid grid{ m_engine.getLpvGridSize(), cellSize, m_aabb.getMax(), m_aabb.getMin(), 1.0f, 0 };

				for ( auto i = 0u; i < shader::DirectionalMaxCascadesCount; ++i )
				{
					grids[i] = m_lpvConfigUbos[i].cpuUpdate( directional
						, i
						, grid
						, m_aabb
						, m_cameraPos
						, m_cameraDir );
				}

				m_lpvConfigUbo.cpuUpdate( grids );
			}
		}

		ashes::Semaphore const & render( ashes::Semaphore const & toWait )
		{
			if ( !m_initialised
				|| !m_scene.needsGlobalIllumination( m_lightType
					, ( GeometryVolumesT
						? GlobalIlluminationType::eLayeredLpvG
						: GlobalIlluminationType::eLayeredLpv ) ) )
			{
				return toWait;
			}

			auto result = &toWait;

			for ( auto & pass : m_lightInjectionPasses )
			{
				result = &pass.compute( *result );
			}

			for ( auto & pass : m_geometryInjectionPasses )
			{
				result = &pass.compute( *result );
			}

			result = &m_clearCommand.submit( *m_device.graphicsQueue, *result );
			auto & passNoOcc = *m_lightPropagationPasses[0u];
			auto & passOcc = *m_lightPropagationPasses[1u];
			uint32_t occPassIndex = 0u;

			for ( uint32_t cascade = 0u; cascade < shader::DirectionalMaxCascadesCount; ++cascade )
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

		void accept( PipelineVisitorBase & visitor )
		{
			m_lightInjectionPasses[0].accept( visitor );

			if ( !m_geometryInjectionPasses.empty() )
			{
				m_geometryInjectionPasses[0].accept( visitor );
			}

			for ( auto & pass : m_lightPropagationPasses )
			{
				if ( pass )
				{
					pass->accept( visitor );
				}
			}

			if ( m_lightVolumeGIPasses[0] )
			{
				m_lightVolumeGIPasses[0]->accept( visitor );
			}
		}

		LayeredLpvConfigUbo const & getLpvConfigUbo()const
		{
			return m_lpvConfigUbo;
		}

		LpvConfigUboArray const & getLpvConfigUbos()const
		{
			return m_lpvConfigUbos;
		}

	private:
		Scene const & m_scene;
		Engine & m_engine;
		RenderDevice const & m_device;
		ShadowMapResult const & m_smResult;
		LightVolumePassResult const & m_lpvResult;
		LightType m_lightType;
		LayeredLpvConfigUbo m_lpvConfigUbo;
		LpvConfigUboArray m_lpvConfigUbos;
		std::vector< LightVolumePassResult > m_injection;
		std::vector< TextureUnit > m_geometry;;
		std::array< LightVolumePassResult, 2u > m_propagate;
		CommandsSemaphore m_clearCommand;
		std::vector< CommandsSemaphore > m_clearCommands;
		LightInjectionPassArray m_lightInjectionPasses;
		GeometryInjectionPassArray m_geometryInjectionPasses;
		LightPropagationPassArray m_lightPropagationPasses;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
		uint32_t m_lightIndex{};
		bool m_initialised{ false };
	};
}

#endif
