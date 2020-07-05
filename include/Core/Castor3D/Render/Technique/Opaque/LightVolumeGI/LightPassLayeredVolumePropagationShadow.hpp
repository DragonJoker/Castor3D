/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPassLayeredVolumePropagationShadow_H___
#define ___C3D_LightPassLayeredVolumePropagationShadow_H___

#include "LightVolumeGIModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Passes/DownscalePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightInjectionPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightPropagationPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LayeredLightVolumeGIPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightVolumePassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvConfigUbo.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <array>

namespace castor3d
{
	template<>
	class LightPassLayeredVolumePropagationShadow< LightType::eDirectional >
		: public LightPassShadow< LightType::eDirectional >
	{
	public:
		using my_traits = LightPassShadowTraits< LightType::eDirectional >;
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
		LightPassLayeredVolumePropagationShadow( Engine & engine
			, LightCache const & lightCache
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smResult
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo )
			: LightPassShadow< LightType::eDirectional >{ engine
				, lpResult
				, gpInfoUbo }
			, m_lpvConfigUbo{ engine }
			, m_injection
			{
				LightVolumePassResult{ engine, "LightInjection0", GridSize },
				LightVolumePassResult{ engine, "LightInjection1", GridSize },
				LightVolumePassResult{ engine, "LightInjection2", GridSize },
				LightVolumePassResult{ engine, "LightInjection3", GridSize },
			}
			, m_geometry
			{
				GeometryInjectionPass::createResult( engine, 0u, GridSize ),
				GeometryInjectionPass::createResult( engine, 1u, GridSize ),
				GeometryInjectionPass::createResult( engine, 2u, GridSize ),
				GeometryInjectionPass::createResult( engine, 3u, GridSize ),
			}
			, m_accumulation
			{
				LightVolumePassResult{ engine, "LightAccumulation0", GridSize },
				LightVolumePassResult{ engine, "LightAccumulation1", GridSize },
				LightVolumePassResult{ engine, "LightAccumulation2", GridSize },
				LightVolumePassResult{ engine, "LightAccumulation3", GridSize },
			}
			, m_propagate
			{
				LightVolumePassResult{ engine, "LightPropagate00", GridSize },
				LightVolumePassResult{ engine, "LightPropagate01", GridSize },
			}
			, m_lpvConfigUbos
			{
				LpvConfigUbo{ engine, 0u },
				LpvConfigUbo{ engine, 1u },
				LpvConfigUbo{ engine, 2u },
				LpvConfigUbo{ engine, 3u },
			}
			, m_lightInjectionPasses
			{
				LightInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[0u], m_injection[0], GridSize, 0u },
				LightInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[1u], m_injection[1], GridSize, 1u },
				LightInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[2u], m_injection[2], GridSize, 2u },
				LightInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[3u], m_injection[3], GridSize, 3u },
			}
			, m_geometryInjectionPasses
			{
				GeometryInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[0u], m_geometry[0], GridSize, 0u },
				GeometryInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[1u], m_geometry[1], GridSize, 1u },
				GeometryInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[2u], m_geometry[2], GridSize, 2u },
				GeometryInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[3u], m_geometry[3], GridSize, 3u },
			}
			, m_lightVolumeGIPass
			{
				engine,
				gpInfoUbo,
				m_lpvConfigUbo,
				gpResult,
				m_accumulation[0u],
				m_accumulation[1u],
				m_accumulation[2u],
				m_accumulation[3u],
				lpResult[LpTexture::eDiffuse],
			}
			, m_aabb{ lightCache.getScene()->getBoundingBox() }
		{
			uint32_t propIndex = 0u;

			for ( uint32_t cascade = 0; cascade < shader::DirectionalMaxCascadesCount; ++cascade )
			{
				m_lightPropagationPasses.emplace_back( engine
					, castor::string::toString( cascade ) + "x0"
					, GridSize
					, m_injection[cascade]
					, m_accumulation[cascade]
					, m_propagate[propIndex]
					, m_lpvConfigUbos[cascade] );

				for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
				{
					m_lightPropagationPasses.emplace_back( engine
						, castor::string::toString( cascade ) + "x" + castor::string::toString( i )
						, GridSize
						, m_geometry[cascade]
						, m_propagate[propIndex]
						, m_accumulation[cascade]
						, m_propagate[1u - propIndex]
						, m_lpvConfigUbos[cascade] );
					propIndex = 1u - propIndex;
				}
			}
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

			for ( auto & pass : m_lightPropagationPasses )
			{
				result = &pass.compute( *result );
			}

			result = &m_lightVolumeGIPass.compute( *result );

			return *result;
		}

		void accept( PipelineVisitorBase & visitor )override
		{
			LightPassShadow< LightType::eDirectional >::accept( visitor );

			for ( auto & pass : m_lightInjectionPasses )
			{
				pass.accept( visitor );
			}

			for ( auto & pass : m_geometryInjectionPasses )
			{
				pass.accept( visitor );
			}

			for ( auto & pass : m_lightPropagationPasses )
			{
				pass.accept( visitor );
			}

			m_lightVolumeGIPass.accept( visitor );
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
				|| light.hasChanged()
				|| m_cameraPos != camPos
				|| m_cameraDir != camDir )
			{
				auto & directional = *light.getDirectionalLight();
				m_lightIndex = light.getShadowMapIndex();
				m_aabb = aabb;
				m_cameraPos = camPos;
				m_cameraDir = camDir;
				auto cellSize = std::max( std::max( m_aabb.getDimensions()->x
					, m_aabb.getDimensions()->y )
					, m_aabb.getDimensions()->z ) / GridSize;
				std::array< castor::Grid, shader::DirectionalMaxCascadesCount > grids;
				castor::Grid grid{ GridSize, cellSize, m_aabb.getMax(), m_aabb.getMin(), 1.0f, 0 };

				for ( auto i = 0u; i < shader::DirectionalMaxCascadesCount; ++i )
				{
					castor::Grid levelGrid{ grid, directional.getSplitScale( i ), i };
					levelGrid.transform( m_cameraPos, m_cameraDir );
					m_lpvConfigUbos[i].update( levelGrid, light, i );
					grids[i] = levelGrid;
				}

				m_lpvConfigUbo.update( grids );
			}
		}

	private:
		LayeredLpvConfigUbo m_lpvConfigUbo;
		std::array< LpvConfigUbo, shader::DirectionalMaxCascadesCount > m_lpvConfigUbos;
		std::array< LightVolumePassResult, shader::DirectionalMaxCascadesCount > m_injection;
		std::array< TextureUnit, shader::DirectionalMaxCascadesCount > m_geometry;
		std::array< LightVolumePassResult, shader::DirectionalMaxCascadesCount > m_accumulation;
		std::array< LightVolumePassResult, 2u > m_propagate;
		std::array< LightInjectionPass, shader::DirectionalMaxCascadesCount > m_lightInjectionPasses;
		std::array< GeometryInjectionPass, shader::DirectionalMaxCascadesCount > m_geometryInjectionPasses;
		std::vector< LightPropagationPass > m_lightPropagationPasses;
		LayeredLightVolumeGIPass m_lightVolumeGIPass;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
		uint32_t m_lightIndex{};
	};
	//!\~english	The directional lights light pass with shadows.
	//!\~french		La passe d'éclairage avec ombres pour les lumières directionnelles.
	using DirectionalLightPassLayeredVolumePropagationShadow = LightPassLayeredVolumePropagationShadow< LightType::eDirectional >;
}

#endif
