/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPropagationVolumes_H___
#define ___C3D_LightPropagationVolumes_H___

#include "LightPropagationVolumesModule.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/GeometryInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"
#include "Castor3D/Render/Passes/DownscalePass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Ubos/LpvConfigUbo.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/BoundingBox.hpp>
#include <CastorUtils/Miscellaneous/StringUtils.hpp>

namespace castor3d
{
	template< bool GeometryVolumesT >
	class LightPropagationVolumesT
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
		LightPropagationVolumesT( Scene const & scene
			, LightType lightType
			, RenderDevice const & device
			, ShadowMapResult const & smResult
			, LightVolumePassResult const & lpvResult )
			: castor::Named{ "LightPropagationVolumes" + ( GeometryVolumesT ? castor::String( "G" ) : castor::String( "" ) ) }
			, m_scene{ scene }
			, m_engine{ *m_scene.getEngine() }
			, m_device{ device }
			, m_smResult{ smResult }
			, m_lpvResult{ lpvResult }
			, m_lightType{ lightType }
			, m_lpvConfigUbo{ m_device }
			, m_injection{ m_engine
				, m_device
				, this->getName() + "Injection"
				, m_engine.getLpvGridSize() }
			, m_geometry{ ( GeometryVolumesT
				? GeometryInjectionPass::createResult( m_engine
					, m_device
					, this->getName()
					, 0u
					, m_engine.getLpvGridSize() )
				: TextureUnit{ m_engine } ) }
			, m_propagate
			{
				LightVolumePassResult{ m_engine
					, m_device
					, this->getName() + "Propagate0"
					, m_engine.getLpvGridSize() },
				LightVolumePassResult{ m_engine
					, m_device
					, this->getName() + "Propagate1"
					, m_engine.getLpvGridSize() },
			}
		{
		}

		void initialise()
		{
			if ( !m_initialised
				&& m_scene.needsGlobalIllumination( m_lightType
					, ( GeometryVolumesT
						? GlobalIlluminationType::eLpvG
						: GlobalIlluminationType::eLpv ) ) )
			{
				auto const cascadeIndex = shader::DirectionalMaxCascadesCount - 2u;
				auto & lightCache = m_scene.getLightCache();
				m_aabb = m_scene.getBoundingBox();
				m_lightInjectionPass = castor::makeUnique< LightInjectionPass >( m_engine
					, m_device
					, this->getName()
					, lightCache
					, m_lightType
					, m_smResult
					, m_lpvConfigUbo
					, m_injection
					, m_engine.getLpvGridSize()
					, cascadeIndex );
				m_lightPropagationPasses = { castor::makeUnique< LightPropagationPass >( m_device
					, this->getName()
					, cuT( "NoOcc" )
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
				TextureUnit * geometry{ nullptr };

				if constexpr ( GeometryVolumesT )
				{
					m_geometryInjectionPass = castor::makeUnique< GeometryInjectionPass >( m_engine
						, m_device
						, this->getName()
						, lightCache
						, m_lightType
						, m_smResult
						, m_lpvConfigUbo
						, m_geometry
						, m_engine.getLpvGridSize()
						, cascadeIndex );
					geometry = &m_geometry;
				}

				LightPropagationPass & passNoOcc = *m_lightPropagationPasses[0u];
				auto & passOcc = *m_lightPropagationPasses[1u];
				uint32_t propIndex = 0u;
				passNoOcc.registerPassIO( nullptr
					, m_injection
					, m_lpvConfigUbo
					, m_lpvResult
					, m_propagate[propIndex] );


				for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
				{
					passOcc.registerPassIO( geometry
						, m_propagate[propIndex]
						, m_lpvConfigUbo
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
				cmd.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
				cmd.beginDebugBlock(
					{
						"Lighting - " + this->getName() + " - Clear",
						castor3d::makeFloatArray( m_engine.getNextRainbowColour() ),
					} );
				clearTex( m_propagate[0u], LpvTexture::eR );
				clearTex( m_propagate[0u], LpvTexture::eG );
				clearTex( m_propagate[0u], LpvTexture::eB );
				clearTex( m_propagate[1u], LpvTexture::eR );
				clearTex( m_propagate[1u], LpvTexture::eG );
				clearTex( m_propagate[1u], LpvTexture::eB );
				clearTex( m_lpvResult, LpvTexture::eR );
				clearTex( m_lpvResult, LpvTexture::eG );
				clearTex( m_lpvResult, LpvTexture::eB );
				cmd.endDebugBlock();
				cmd.end();
				m_initialised = true;
			}
		}

		void cleanup()
		{
			m_initialised = false;
			m_clearCommands = {};
			m_lightInjectionPass.reset();
			m_geometryInjectionPass.reset();
			m_lightPropagationPasses = {};
		}

		void update( CpuUpdater & updater )
		{
			if ( !m_initialised
				|| !m_scene.needsGlobalIllumination( m_lightType
					, ( GeometryVolumesT
						? GlobalIlluminationType::eLpvG
						: GlobalIlluminationType::eLpv ) ) )
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
				m_lightIndex = light.getShadowMapIndex();
				m_aabb = aabb;
				m_cameraPos = camPos;
				m_cameraDir = camDir;
				m_lpvConfigUbo.cpuUpdate( light
					, m_aabb
					, m_cameraPos
					, m_cameraDir
					, m_engine.getLpvGridSize() );
			}
		}

		ashes::Semaphore const & render( ashes::Semaphore const & toWait )
		{
			if ( !m_initialised
				|| !m_scene.needsGlobalIllumination( m_lightType
					, ( GeometryVolumesT
						? GlobalIlluminationType::eLpvG
						: GlobalIlluminationType::eLpv ) ) )
			{
				return toWait;
			}

			CU_Require( m_initialised );
			auto result = &toWait;
			result = &m_clearCommands.submit( *m_device.graphicsQueue, *result );
			result = &m_lightInjectionPass->compute( *result );

			if constexpr ( GeometryVolumesT )
			{
				result = &m_geometryInjectionPass->compute( *result );
			}

			result = &m_lightPropagationPasses[0]->compute( *result, 0u );

			for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
			{
				result = &m_lightPropagationPasses[1]->compute( *result, i - 1u );
			}

			return *result;
		}

		void accept( PipelineVisitorBase & visitor )
		{
			if ( m_initialised )
			{
				if ( m_lightInjectionPass )
				{
					m_lightInjectionPass->accept( visitor );
				}

				if ( m_geometryInjectionPass )
				{
					m_geometryInjectionPass->accept( visitor );
				}

				for ( auto & pass : m_lightPropagationPasses )
				{
					pass->accept( visitor );
				}
			}
		}

		LpvConfigUbo const & getLpvConfigUbo()const
		{
			return m_lpvConfigUbo;
		}

	private:
		Scene const & m_scene;
		Engine & m_engine;
		RenderDevice const & m_device;
		ShadowMapResult const & m_smResult;
		LightVolumePassResult const & m_lpvResult;
		LightType m_lightType;
		LpvConfigUbo m_lpvConfigUbo;
		LightVolumePassResult m_injection;
		TextureUnit m_geometry;
		std::array< LightVolumePassResult, 2u > m_propagate;
		CommandsSemaphore m_clearCommands;
		GeometryInjectionPassUPtr m_geometryInjectionPass;
		LightInjectionPassUPtr m_lightInjectionPass;
		LightPropagationPassArray  m_lightPropagationPasses;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
		uint32_t m_lightIndex{};
		bool m_initialised{ false };
	};
}

#endif
