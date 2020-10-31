/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPassVolumePropagationShadow_H___
#define ___C3D_LightPassVolumePropagationShadow_H___

#include "LightVolumeGIModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/Passes/DownscalePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/GeometryInjectionPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightInjectionPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightPropagationPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightVolumeGIPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightVolumePassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Ubos/LpvConfigUbo.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

namespace castor3d
{
	template< LightType LtType, bool GeometryVolumesT >
	class LightPassVolumePropagationShadow
		: public LightPassShadow< LtType >
	{
	public:
		using my_traits = LightPassShadowTraits< LtType >;
		using my_light_type = typename my_traits::light_type;
		using my_pass_type = typename my_traits::light_pass_type;
		using my_shadow_pass_type = typename my_traits::shadow_pass_type;

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
		LightPassVolumePropagationShadow( Engine & engine
			, RenderDevice const & device
			, LightCache const & lightCache
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smResult
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo )
			: LightPassShadow< LtType >{ engine
				, device
				, "LPVShadow"
				, lpResult
				, gpInfoUbo
				, true }
			, m_gpResult{ gpResult }
			, m_smResult{ smResult }
			, m_lpResult{ lpResult }
			, m_gpInfoUbo{ gpInfoUbo }
			, m_lpvConfigUbo{ device }
			, m_injection{ engine, device, this->getName() + "Injection", engine.getLpvGridSize() }
			, m_geometry{ ( GeometryVolumesT
				? GeometryInjectionPass::createResult( engine, device, this->getName(), 0u , engine.getLpvGridSize() )
				: TextureUnit{ engine } ) }
			, m_accumulation{ engine, device, this->getName() + "Accumulation", engine.getLpvGridSize() }
			, m_propagate
			{
				LightVolumePassResult{ engine, device, this->getName() + "Propagate0", engine.getLpvGridSize() },
				LightVolumePassResult{ engine, device, this->getName() + "Propagate1", engine.getLpvGridSize() },
			}
			, m_aabb{ lightCache.getScene()->getBoundingBox() }
		{
		}

		void initialise( Scene const & scene
			, OpaquePassResult const & gp
			, SceneUbo & sceneUbo
			, RenderPassTimer & timer )override
		{
			auto const cascadeIndex = shader::DirectionalMaxCascadesCount - 2u;
			auto & lightCache = scene.getLightCache();
			m_lightInjectionPass = std::make_unique< LightInjectionPass >( this->m_engine
				, this->m_device
				, this->getName()
				, lightCache
				, LtType
				, m_smResult
				, m_gpInfoUbo
				, m_lpvConfigUbo
				, m_injection
				, this->m_engine.getLpvGridSize()
				, cascadeIndex );
			m_lightVolumeGIPasses = { std::make_unique< LightVolumeGIPass >( this->m_engine
					, this->m_device
					, this->getName()
					, LtType
					, m_gpInfoUbo
					, m_lpvConfigUbo
					, m_gpResult
					, m_accumulation
					, m_lpResult[LpTexture::eIndirectDiffuse]
					, BlendMode::eNoBlend )
				, std::make_unique< LightVolumeGIPass >( this->m_engine
					, this->m_device
					, this->getName()
					, LtType
					, m_gpInfoUbo
					, m_lpvConfigUbo
					, m_gpResult
					, m_accumulation
					, m_lpResult[LpTexture::eIndirectDiffuse]
					, BlendMode::eAdditive ) };
			m_lightPropagationPasses = { std::make_unique< LightPropagationPass >( this->m_device
					, this->getName()
					, cuT( "NoOcc" )
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
			TextureUnit * geometry{ nullptr };

			if constexpr ( GeometryVolumesT )
			{
				m_geometryInjectionPass = std::make_unique< GeometryInjectionPass >( this->m_engine
					, this->m_device
					, this->getName()
					, lightCache
					, LtType
					, m_smResult
					, m_gpInfoUbo
					, m_lpvConfigUbo
					, m_geometry
					, this->m_engine.getLpvGridSize()
					, cascadeIndex );
				geometry = &m_geometry;
			}

			LightPropagationPass & passNoOcc = *m_lightPropagationPasses[0u];
			auto & passOcc = *m_lightPropagationPasses[1u];
			uint32_t propIndex = 0u;
			passNoOcc.registerPassIO( nullptr
				, m_injection
				, m_lpvConfigUbo
				, m_accumulation
				, m_propagate[propIndex] );


			for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
			{
				passOcc.registerPassIO( geometry
					, m_propagate[propIndex]
					, m_lpvConfigUbo
					, m_accumulation
					, m_propagate[1u - propIndex] );
				propIndex = 1u - propIndex;
			}

			passOcc.initialisePasses();
			passNoOcc.initialisePasses();
			LightPassShadow< LtType >::initialise( scene, gp, sceneUbo, timer );

			m_clearCommands = CommandsSemaphore{ this->m_device, this->getName() + cuT( "Clear" ) };
			ashes::CommandBuffer & cmd = *m_clearCommands.commandBuffer;
			auto clearTex = [&cmd]( LightVolumePassResult & result, LpvTexture tex )
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
					"Lighting - " + getName() + " - Clear",
					castor3d::makeFloatArray( m_engine.getNextRainbowColour() ),
				} );
			clearTex( m_propagate[0u], LpvTexture::eR );
			clearTex( m_propagate[0u], LpvTexture::eG );
			clearTex( m_propagate[0u], LpvTexture::eB );
			clearTex( m_propagate[1u], LpvTexture::eR );
			clearTex( m_propagate[1u], LpvTexture::eG );
			clearTex( m_propagate[1u], LpvTexture::eB );
			clearTex( m_accumulation, LpvTexture::eR );
			clearTex( m_accumulation, LpvTexture::eG );
			clearTex( m_accumulation, LpvTexture::eB );
			cmd.endDebugBlock();
			cmd.end();
		}

		void cleanup()override
		{
			m_clearCommands = {};
			LightPassShadow< LtType >::cleanup();
			m_lightVolumeGIPasses = {};
			m_lightInjectionPass.reset();
			m_geometryInjectionPass.reset();
			m_lightPropagationPasses = {};
		}

		ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait )override
		{
			auto result = &toWait;
			result = &my_pass_type::render( index, *result );
			result = &this->m_clearCommands.submit( *this->m_device.graphicsQueue, *result );
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

			result = &m_lightVolumeGIPasses[index == 0 ? 0 : 1]->compute( *result );

			return *result;
		}

		void accept( PipelineVisitorBase & visitor )override
		{
			LightPassShadow< LtType >::accept( visitor );

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
				m_aabb = aabb;
				m_cameraPos = camPos;
				m_cameraDir = camDir;
				m_lpvConfigUbo.cpuUpdate( light
					, m_aabb
					, m_cameraPos
					, m_cameraDir
					, this->m_engine.getLpvGridSize() );
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
		LpvConfigUbo m_lpvConfigUbo;
		LightVolumePassResult m_injection;
		TextureUnit m_geometry;
		LightVolumePassResult m_accumulation;
		std::array< LightVolumePassResult, 2u > m_propagate;
		CommandsSemaphore m_clearCommands;
		GeometryInjectionPassUPtr m_geometryInjectionPass;
		LightInjectionPassUPtr m_lightInjectionPass;
		LightPropagationPassArray  m_lightPropagationPasses;
		LightVolumeGIPassArray m_lightVolumeGIPasses;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
	};
}

#endif
