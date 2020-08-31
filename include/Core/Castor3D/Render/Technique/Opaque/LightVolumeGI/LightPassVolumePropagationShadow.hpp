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
			, LightCache const & lightCache
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smResult
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo )
			: LightPassShadow< LtType >{ engine
				, lpResult
				, gpInfoUbo }
			, m_gpResult{ gpResult }
			, m_smResult{ smResult }
			, m_lpResult{ lpResult }
			, m_gpInfoUbo{ gpInfoUbo }
			, m_lpvConfigUbo{ engine }
			, m_injection{ engine, "LightInjection", GridSize }
			, m_geometry{ GeometryInjectionPass::createResult( engine, 0u , GridSize ) }
			, m_accumulation{ engine, "LightAccumulation", GridSize }
			, m_propagate
			{
				LightVolumePassResult{ m_engine, "LightPropagate0", GridSize },
				LightVolumePassResult{ m_engine, "LightPropagate1", GridSize },
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
			m_geometryInjectionPass = std::make_unique< GeometryInjectionPass >( m_engine
				, lightCache
				, LtType
				, m_smResult
				, m_gpInfoUbo
				, m_lpvConfigUbo
				, m_geometry
				, GridSize );
			m_lightInjectionPass = std::make_unique< LightInjectionPass >( m_engine
				, lightCache
				, LtType
				, m_smResult
				, m_gpInfoUbo
				, m_lpvConfigUbo
				, m_injection
				, GridSize );
			m_lightVolumeGIPass = std::make_unique< LightVolumeGIPass >( m_engine
				, LtType
				, m_gpInfoUbo
				, m_lpvConfigUbo
				, m_gpResult
				, m_accumulation
				, m_lpResult[LpTexture::eDiffuse] );
			uint32_t propIndex = 0u;
			m_lightPropagationPasses.emplace_back( m_engine
				, "0"
				, GridSize
				, m_injection
				, m_accumulation
				, m_propagate[propIndex]
				, m_lpvConfigUbo );

			for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
			{
				m_lightPropagationPasses.emplace_back( m_engine
					, castor::string::toString( i )
					, GridSize
					, m_geometry
					, m_propagate[propIndex]
					, m_accumulation
					, m_propagate[1u - propIndex]
					, m_lpvConfigUbo );
				propIndex = 1u - propIndex;
			}

			LightPassShadow< LtType >::initialise( scene, gp, sceneUbo, timer );
		}

		void cleanup()override
		{
			LightPassShadow< LtType >::cleanup();
			m_lightVolumeGIPass.reset();
			m_lightInjectionPass.reset();
			m_geometryInjectionPass.reset();
			m_lightPropagationPasses.clear();
		}

		ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait )override
		{
			auto result = &toWait;
			result = &my_pass_type::render( index, *result );
			result = &m_lightInjectionPass->compute( *result );
			result = &m_geometryInjectionPass->compute( *result );

			for ( auto & pass : m_lightPropagationPasses )
			{
				result = &pass.compute( *result );
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
				pass.accept( visitor );
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
					m_lpvConfigUbo.update( grid, light, 3u );
				}
				else
				{
					m_lpvConfigUbo.update( grid, light );
				}
			}
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
		std::unique_ptr< GeometryInjectionPass > m_geometryInjectionPass;
		std::unique_ptr< LightInjectionPass > m_lightInjectionPass;
		std::vector< LightPropagationPass >  m_lightPropagationPasses;
		std::unique_ptr< LightVolumeGIPass > m_lightVolumeGIPass;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
		uint32_t m_lightIndex{};
	};
	//!\~english	The directional lights light pass with shadows.
	//!\~french		La passe d'éclairage avec ombres pour les lumières directionnelles.
	using DirectionalLightPassVolumePropagationShadow = LightPassVolumePropagationShadow< LightType::eDirectional >;
	//!\~english	The point lights light pass with shadows.
	//!\~french		La passe d'éclairage avec ombres pour les lumières omnidirectionnelles.
	using PointLightPassVolumePropagationShadow = LightPassVolumePropagationShadow< LightType::ePoint >;
	//!\~english	The spot lights light pass with shadows.
	//!\~french		La passe d'éclairage avec ombres pour les lumières projecteurs.
	using SpotLightPassVolumePropagationShadow = LightPassVolumePropagationShadow< LightType::eSpot >;
}

#endif
