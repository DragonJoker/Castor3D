/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPassVolumePropagationShadow_H___
#define ___C3D_LightPassVolumePropagationShadow_H___

#include "LightVolumeGIModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
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
#include "Castor3D/Shader/Ubos/LpvConfigUbo.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

namespace castor3d
{
	template< LightType LtType >
	class LightPassVolumePropagationShadow
		: public LightPassShadow< LtType >
	{
	public:
		using my_traits = LightPassShadowTraits< LtType >;
		using my_light_type = typename my_traits::light_type;
		using my_pass_type = typename my_traits::light_pass_type;
		using my_shadow_pass_type = typename my_traits::shadow_pass_type;

		static constexpr uint32_t GridSize = 32u;
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
				, gpInfoUbo }
			, m_gpResult{ gpResult }
			, m_smResult{ smResult }
			, m_lpResult{ lpResult }
			, m_gpInfoUbo{ gpInfoUbo }
			, m_lpvConfigUbo{ device }
			, m_injection{ engine, device, this->getName() + "Injection", GridSize }
			, m_geometry{ GeometryInjectionPass::createResult( engine, device, this->getName(), 0u , GridSize ) }
			, m_accumulation{ engine, device, this->getName() + "Accumulation", GridSize }
			, m_propagate
			{
				LightVolumePassResult{ engine, device, this->getName() + "Propagate0", GridSize },
				LightVolumePassResult{ engine, device, this->getName() + "Propagate1", GridSize },
			}
			, m_aabb{ lightCache.getScene()->getBoundingBox() }
		{
		}

		void initialise( Scene const & scene
			, OpaquePassResult const & gp
			, SceneUbo & sceneUbo
			, RenderPassTimer & timer )override
		{
			auto & lightCache = scene.getLightCache();
			m_geometryInjectionPass = std::make_unique< GeometryInjectionPass >( this->m_engine
				, this->m_device
				, this->getName()
				, lightCache
				, LtType
				, m_smResult
				, m_gpInfoUbo
				, m_lpvConfigUbo
				, m_geometry
				, GridSize );
			m_lightInjectionPass = std::make_unique< LightInjectionPass >( this->m_engine
				, this->m_device
				, this->getName()
				, lightCache
				, LtType
				, m_smResult
				, m_gpInfoUbo
				, m_lpvConfigUbo
				, m_injection
				, GridSize );
			m_lightVolumeGIPass = std::make_unique< LightVolumeGIPass >( this->m_engine
				, this->m_device
				, this->getName()
				, LtType
				, m_gpInfoUbo
				, m_lpvConfigUbo
				, m_gpResult
				, m_accumulation
				, m_lpResult[LpTexture::eIndirect] );
			m_lightPropagationPasses = { std::make_unique< LightPropagationPass >( this->m_device
					, this->getName()
					, "NoOcc"
					, false
					, GridSize )
				, std::make_unique< LightPropagationPass >( this->m_device
					, this->getName()
					, "Occ"
					, true
					, GridSize ) };
			auto & passNoOcc = *m_lightPropagationPasses[0u];
			uint32_t propIndex = 0u;
			passNoOcc.registerPassIO( nullptr
				, m_injection
				, m_lpvConfigUbo
				, m_accumulation
				, m_propagate[propIndex] );
			passNoOcc.initialisePasses();

			auto & passOcc = *m_lightPropagationPasses[1u];

			for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
			{
				passOcc.registerPassIO( &m_geometry
					, m_propagate[propIndex]
					, m_lpvConfigUbo
					, m_accumulation
					, m_propagate[1u - propIndex] );
				propIndex = 1u - propIndex;
			}

			passOcc.initialisePasses();
			LightPassShadow< LtType >::initialise( scene, gp, sceneUbo, timer );
		}

		void cleanup()override
		{
			LightPassShadow< LtType >::cleanup();
			m_lightVolumeGIPass.reset();
			m_lightInjectionPass.reset();
			m_geometryInjectionPass.reset();
			m_lightPropagationPasses = {};
		}

		ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait )override
		{
			auto result = &toWait;
			result = &my_pass_type::render( index, *result );
			result = &m_lightInjectionPass->compute( *result );
			result = &m_geometryInjectionPass->compute( *result );
			result = &m_lightPropagationPasses[0]->compute( *result, 0u );

			for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
			{
				result = &m_lightPropagationPasses[1]->compute( *result, i - 1u );
			}

			result = &m_lightVolumeGIPass->compute( *result );

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

			if ( m_lightVolumeGIPass )
			{
				m_lightVolumeGIPass->accept( visitor );
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
				|| light.hasChanged() )
			{
				m_aabb = aabb;
				m_cameraPos = camPos;
				m_cameraDir = camDir;
				auto cellSize = std::max( std::max( m_aabb.getDimensions()->x
					, m_aabb.getDimensions()->y )
					, m_aabb.getDimensions()->z ) / GridSize;
				castor::Grid grid{ GridSize, cellSize, m_aabb.getMax(), m_aabb.getMin(), 1.0f, 0 };
				grid.transform( m_cameraPos, m_cameraDir );

				if constexpr ( LtType == LightType::eDirectional )
				{
					m_lpvConfigUbo.cpuUpdate( grid, light, 3u );
				}
				else
				{
					m_lpvConfigUbo.cpuUpdate( grid, light );
				}
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
		GeometryInjectionPassUPtr m_geometryInjectionPass;
		LightInjectionPassUPtr m_lightInjectionPass;
		LightPropagationPassArray  m_lightPropagationPasses;
		LightVolumeGIPassUPtr m_lightVolumeGIPass;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
		uint32_t m_lightIndex{};
	};
}

#endif
