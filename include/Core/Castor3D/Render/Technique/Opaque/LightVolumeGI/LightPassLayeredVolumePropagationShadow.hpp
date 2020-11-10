/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPassLayeredVolumePropagationShadow_H___
#define ___C3D_LightPassLayeredVolumePropagationShadow_H___

#include "LightVolumeGIModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"

#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/Passes/DownscalePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/GeometryInjectionPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightInjectionPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightPropagationPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LayeredLightVolumeGIPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightVolumePassResult.hpp"
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
	class LightPassLayeredVolumePropagationShadow< LightType::eDirectional, GeometryVolumesT >
		: public LightPassShadow< LightType::eDirectional >
	{
	public:
		using my_traits = LightPassShadowTraits< LightType::eDirectional >;
		using my_light_type = typename my_traits::light_type;
		using my_pass_type = typename my_traits::light_pass_type;
		using my_shadow_pass_type = typename my_traits::shadow_pass_type;

		static constexpr uint32_t MaxPropagationSteps = 8u;
		static constexpr LightType LtType = LightType::eDirectional;

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
		LightPassLayeredVolumePropagationShadow( Engine & engine
			, RenderDevice const & device
			, LightCache const & lightCache
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smResult
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo )
			: LightPassShadow< LtType >{ engine
				, device
				, "LayeredLPVShadow"
				, lpResult
				, gpInfoUbo
				, true }
			, m_gpResult{ gpResult }
			, m_smResult{ smResult }
			, m_lpResult{ lpResult }
			, m_gpInfoUbo{ gpInfoUbo }
			, m_lpvConfigUbo{ device }
			, m_accumulation{ engine, device, this->getName() + "Accumulation", engine.getLpvGridSize() }
			, m_propagate
			{
				LightVolumePassResult{ engine, device, this->getName() + "Propagate0", engine.getLpvGridSize() },
				LightVolumePassResult{ engine, device, this->getName() + "Propagate1", engine.getLpvGridSize() },
			}
			, m_aabb{ lightCache.getScene()->getBoundingBox() }
		{
			for ( uint32_t cascade = 0u; cascade < shader::DirectionalMaxCascadesCount; ++cascade )
			{
				m_injection.emplace_back( engine, device, this->getName() + "Injection" + castor::string::toString( cascade ), engine.getLpvGridSize() );
				m_geometry.emplace_back( GeometryVolumesT
					? GeometryInjectionPass::createResult( engine, device, this->getName(), cascade, engine.getLpvGridSize() )
					: TextureUnit{ engine } );
				m_lpvConfigUbos.emplace_back( device, cascade );
			}
		}

		void initialise( Scene const & scene
			, OpaquePassResult const & gp
			, SceneUbo & sceneUbo
			, RenderPassTimer & timer )override
		{
			auto & lightCache = scene.getLightCache();
			m_lightVolumeGIPasses = { std::make_unique< LayeredLightVolumeGIPass >( this->m_engine
					, this->m_device
					, this->getName()
					, m_gpInfoUbo
					, m_lpvConfigUbo
					, m_gpResult
					, m_accumulation
					, m_lpResult[LpTexture::eIndirectDiffuse]
					, BlendMode::eNoBlend )
				, std::make_unique< LayeredLightVolumeGIPass >( this->m_engine
					, this->m_device
					, this->getName()
					, m_gpInfoUbo
					, m_lpvConfigUbo
					, m_gpResult
					, m_accumulation
					, m_lpResult[LpTexture::eIndirectDiffuse]
					, BlendMode::eAdditive ) };
			m_lightPropagationPasses = { std::make_unique< LightPropagationPass >( this->m_device
					, this->getName()
					, "NoOcc"
					, false
					, this->m_engine.getLpvGridSize()
					, BlendMode::eAdditive )
				, std::make_unique< LightPropagationPass >( this->m_device
					, this->getName()
					, ( GeometryVolumesT
						? castor::String{ cuT( "OccBlend" ) }
						: castor::String{ cuT( "NoOccBlend" ) } )
					, GeometryVolumesT
					, this->m_engine.getLpvGridSize()
					, BlendMode::eAdditive ) };

			auto & passNoOcc = *m_lightPropagationPasses[0u];
			auto & passOcc = *m_lightPropagationPasses[1u];
			auto clearTex = []( ashes::CommandBuffer const & cmd
				, LightVolumePassResult & result
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

			m_clearCommand = CommandsSemaphore{ this->m_device
				, this->getName() + cuT( "ClearAccumulation" ) };
			ashes::CommandBuffer & cmd = *m_clearCommand.commandBuffer;
			cmd.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
			cmd.beginDebugBlock(
				{
					"Lighting - " + this->getName() + " - Clear Accumulation",
					castor3d::makeFloatArray( this->m_engine.getNextRainbowColour() ),
				} );
			clearTex( cmd, m_accumulation, LpvTexture::eR );
			clearTex( cmd, m_accumulation, LpvTexture::eG );
			clearTex( cmd, m_accumulation, LpvTexture::eB );

			cmd.endDebugBlock();
			cmd.end();

			for ( uint32_t cascade = 0u; cascade < shader::DirectionalMaxCascadesCount; ++cascade )
			{
				m_lightInjectionPasses.emplace_back( this->m_engine
					, this->m_device
					, this->getName()
					, lightCache
					, LtType
					, m_smResult
					, m_gpInfoUbo
					, m_lpvConfigUbos[cascade]
					, m_injection[cascade]
					, this->m_engine.getLpvGridSize()
					, cascade );
				TextureUnit * geometry{ nullptr };

				if constexpr ( GeometryVolumesT )
				{
					m_geometryInjectionPasses.emplace_back( this->m_engine
						, this->m_device
						, this->getName()
						, lightCache
						, LtType
						, m_smResult
						, m_gpInfoUbo
						, m_lpvConfigUbos[cascade]
						, m_geometry[cascade]
						, this->m_engine.getLpvGridSize()
						, cascade );
					geometry = &m_geometry[cascade];
				}

				uint32_t propIndex = 0u;
				passNoOcc.registerPassIO( nullptr
					, m_injection[cascade]
					, m_lpvConfigUbos[cascade]
					, m_accumulation
					, m_propagate[propIndex] );

				for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
				{
					passOcc.registerPassIO( geometry
						, m_propagate[propIndex]
						, m_lpvConfigUbos[cascade]
						, m_accumulation
						, m_propagate[1u - propIndex] );
					propIndex = 1u - propIndex;
				}

				m_clearCommands.emplace_back( this->m_device
					, this->getName() + cuT( "Clear" ) + castor::string::toString( cascade ) );
				auto & clearCommands = m_clearCommands[cascade];
				ashes::CommandBuffer & cmd = *clearCommands.commandBuffer;
				cmd.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
				cmd.beginDebugBlock(
					{
						"Lighting - " + this->getName() + " - Clear " + castor::string::toString( cascade ),
						castor3d::makeFloatArray( this->m_engine.getNextRainbowColour() ),
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
			LightPassShadow< LtType >::initialise( scene, gp, sceneUbo, timer );
		}

		void cleanup()override
		{
			m_clearCommands.clear();
			m_clearCommand = {};
			LightPassShadow< LtType >::cleanup();
			m_lightVolumeGIPasses = {};
			m_lightPropagationPasses = {};
			m_geometryInjectionPasses.clear();
			m_lightInjectionPasses.clear();
		}

		ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait )override
		{
			auto result = &toWait;
			result = &my_pass_type::render( index, *result );

			for ( auto & pass : m_lightInjectionPasses )
			{
				result = &pass.compute( *result );
			}

			for ( auto & pass : m_geometryInjectionPasses )
			{
				result = &pass.compute( *result );
			}

			result = &this->m_clearCommand.submit( *this->m_device.graphicsQueue, *result );
			auto & passNoOcc = *m_lightPropagationPasses[0u];
			auto & passOcc = *m_lightPropagationPasses[1u];
			uint32_t occPassIndex = 0u;

			for ( uint32_t cascade = 0u; cascade < shader::DirectionalMaxCascadesCount; ++cascade )
			{
				result = &this->m_clearCommands[cascade].submit( *this->m_device.graphicsQueue, *result );
				result = &passNoOcc.compute( *result, cascade );

				for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i, ++occPassIndex )
				{
					result = &passOcc.compute( *result, occPassIndex );
				}
			}

			result = &m_lightVolumeGIPasses[index == 0 ? 0 : 1]->compute( *result );

			return *result;
		}

		void accept( PipelineVisitorBase & visitor )override
		{
			LightPassShadow< LtType >::accept( visitor );
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

	protected:
		void doUpdate( bool first
			, castor::Size const & size
			, Light const & light
			, Camera const & camera
			, ShadowMap const * shadowMap
			, uint32_t shadowMapIndex )override
		{
			my_pass_type::doUpdate( first
				, size
				, light
				, camera
				, shadowMap
				, shadowMapIndex );

			auto & scene = *light.getScene();
			auto aabb = scene.getBoundingBox();
			auto camPos = camera.getParent()->getDerivedPosition();
			castor::Point3f camDir{ 0, 0, 1 };
			camera.getParent()->getDerivedOrientation().transform( camDir, camDir );

			if ( m_aabb != aabb
				|| m_cameraPos != camPos
				|| m_cameraDir != camDir
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
					, m_aabb.getDimensions()->z ) / this->m_engine.getLpvGridSize();
				std::array< castor::Grid, shader::DirectionalMaxCascadesCount > grids;
				castor::Grid grid{ this->m_engine.getLpvGridSize(), cellSize, m_aabb.getMax(), m_aabb.getMin(), 1.0f, 0 };

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

	private:
		VkClearValue doGetIndirectClearColor()const override
		{
			return opaqueBlackClearColor;
		}

	private:
		OpaquePassResult const & m_gpResult;
		ShadowMapResult const & m_smResult;
		LightPassResult const & m_lpResult;
		GpInfoUbo const & m_gpInfoUbo;
		LayeredLpvConfigUbo m_lpvConfigUbo;
		std::vector< LpvConfigUbo > m_lpvConfigUbos;
		std::vector< LightVolumePassResult > m_injection;
		std::vector< TextureUnit > m_geometry;
		LightVolumePassResult m_accumulation;
		std::array< LightVolumePassResult, 2u > m_propagate;
		CommandsSemaphore m_clearCommand;
		std::vector< CommandsSemaphore > m_clearCommands;
		LightInjectionPassArray m_lightInjectionPasses;
		GeometryInjectionPassArray m_geometryInjectionPasses;
		LightPropagationPassArray m_lightPropagationPasses;
		LayeredLightVolumeGIPassArray m_lightVolumeGIPasses;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
		uint32_t m_lightIndex{};
	};
}

#endif
