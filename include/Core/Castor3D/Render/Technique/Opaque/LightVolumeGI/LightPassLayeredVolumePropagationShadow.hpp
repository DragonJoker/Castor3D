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
				LightVolumePassResult{ engine, "LightPropagate02", GridSize },
				LightVolumePassResult{ engine, "LightPropagate03", GridSize },
				LightVolumePassResult{ engine, "LightPropagate04", GridSize },
				LightVolumePassResult{ engine, "LightPropagate05", GridSize },
				LightVolumePassResult{ engine, "LightPropagate06", GridSize },
				LightVolumePassResult{ engine, "LightPropagate07", GridSize },
				LightVolumePassResult{ engine, "LightPropagate10", GridSize },
				LightVolumePassResult{ engine, "LightPropagate11", GridSize },
				LightVolumePassResult{ engine, "LightPropagate12", GridSize },
				LightVolumePassResult{ engine, "LightPropagate13", GridSize },
				LightVolumePassResult{ engine, "LightPropagate14", GridSize },
				LightVolumePassResult{ engine, "LightPropagate15", GridSize },
				LightVolumePassResult{ engine, "LightPropagate16", GridSize },
				LightVolumePassResult{ engine, "LightPropagate17", GridSize },
				LightVolumePassResult{ engine, "LightPropagate20", GridSize },
				LightVolumePassResult{ engine, "LightPropagate21", GridSize },
				LightVolumePassResult{ engine, "LightPropagate22", GridSize },
				LightVolumePassResult{ engine, "LightPropagate23", GridSize },
				LightVolumePassResult{ engine, "LightPropagate24", GridSize },
				LightVolumePassResult{ engine, "LightPropagate25", GridSize },
				LightVolumePassResult{ engine, "LightPropagate26", GridSize },
				LightVolumePassResult{ engine, "LightPropagate27", GridSize },
				LightVolumePassResult{ engine, "LightPropagate30", GridSize },
				LightVolumePassResult{ engine, "LightPropagate31", GridSize },
				LightVolumePassResult{ engine, "LightPropagate32", GridSize },
				LightVolumePassResult{ engine, "LightPropagate33", GridSize },
				LightVolumePassResult{ engine, "LightPropagate34", GridSize },
				LightVolumePassResult{ engine, "LightPropagate35", GridSize },
				LightVolumePassResult{ engine, "LightPropagate36", GridSize },
				LightVolumePassResult{ engine, "LightPropagate37", GridSize },
			}
			, m_lpvConfigUbos
			{
				LpvConfigUbo{ engine },
				LpvConfigUbo{ engine },
				LpvConfigUbo{ engine },
				LpvConfigUbo{ engine },
			}
			, m_lightInjectionPasses
			{
				LightInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[0u], m_injection[0], GridSize, 0u },
				LightInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[1u], m_injection[1], GridSize, 1u },
				LightInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[2u], m_injection[2], GridSize, 2u },
				LightInjectionPass{ engine, lightCache, LightType::eDirectional, smResult, gpInfoUbo, m_lpvConfigUbos[3u], m_injection[3], GridSize, 3u },
			}
			, m_lightPropagationPasses
			{
				LightPropagationPass{ engine, GridSize, m_injection[0], m_accumulation[0], m_propagate[0], m_lpvConfigUbos[0] },
				LightPropagationPass{ engine, GridSize, m_propagate[0], m_accumulation[0], m_propagate[1], m_lpvConfigUbos[0] },
				LightPropagationPass{ engine, GridSize, m_propagate[1], m_accumulation[0], m_propagate[2], m_lpvConfigUbos[0] },
				LightPropagationPass{ engine, GridSize, m_propagate[2], m_accumulation[0], m_propagate[3], m_lpvConfigUbos[0] },
				LightPropagationPass{ engine, GridSize, m_propagate[3], m_accumulation[0], m_propagate[4], m_lpvConfigUbos[0] },
				LightPropagationPass{ engine, GridSize, m_propagate[4], m_accumulation[0], m_propagate[5], m_lpvConfigUbos[0] },
				LightPropagationPass{ engine, GridSize, m_propagate[5], m_accumulation[0], m_propagate[6], m_lpvConfigUbos[0] },
				LightPropagationPass{ engine, GridSize, m_propagate[6], m_accumulation[0], m_propagate[7], m_lpvConfigUbos[0] },

				LightPropagationPass{ engine, GridSize, m_injection[1], m_accumulation[1], m_propagate[8], m_lpvConfigUbos[1] },
				LightPropagationPass{ engine, GridSize, m_propagate[8], m_accumulation[1], m_propagate[9], m_lpvConfigUbos[1] },
				LightPropagationPass{ engine, GridSize, m_propagate[9], m_accumulation[1], m_propagate[10], m_lpvConfigUbos[1] },
				LightPropagationPass{ engine, GridSize, m_propagate[10], m_accumulation[1], m_propagate[11], m_lpvConfigUbos[1] },
				LightPropagationPass{ engine, GridSize, m_propagate[11], m_accumulation[1], m_propagate[12], m_lpvConfigUbos[1] },
				LightPropagationPass{ engine, GridSize, m_propagate[12], m_accumulation[1], m_propagate[13], m_lpvConfigUbos[1] },
				LightPropagationPass{ engine, GridSize, m_propagate[13], m_accumulation[1], m_propagate[14], m_lpvConfigUbos[1] },
				LightPropagationPass{ engine, GridSize, m_propagate[14], m_accumulation[1], m_propagate[15], m_lpvConfigUbos[1] },

				LightPropagationPass{ engine, GridSize, m_injection[2], m_accumulation[2], m_propagate[16], m_lpvConfigUbos[2] },
				LightPropagationPass{ engine, GridSize, m_propagate[16], m_accumulation[2], m_propagate[17], m_lpvConfigUbos[2] },
				LightPropagationPass{ engine, GridSize, m_propagate[17], m_accumulation[2], m_propagate[18], m_lpvConfigUbos[2] },
				LightPropagationPass{ engine, GridSize, m_propagate[18], m_accumulation[2], m_propagate[19], m_lpvConfigUbos[2] },
				LightPropagationPass{ engine, GridSize, m_propagate[19], m_accumulation[2], m_propagate[20], m_lpvConfigUbos[2] },
				LightPropagationPass{ engine, GridSize, m_propagate[20], m_accumulation[2], m_propagate[21], m_lpvConfigUbos[2] },
				LightPropagationPass{ engine, GridSize, m_propagate[21], m_accumulation[2], m_propagate[22], m_lpvConfigUbos[2] },
				LightPropagationPass{ engine, GridSize, m_propagate[22], m_accumulation[2], m_propagate[23], m_lpvConfigUbos[2] },

				LightPropagationPass{ engine, GridSize, m_injection[3], m_accumulation[3], m_propagate[24], m_lpvConfigUbos[3] },
				LightPropagationPass{ engine, GridSize, m_propagate[24], m_accumulation[3], m_propagate[25], m_lpvConfigUbos[3] },
				LightPropagationPass{ engine, GridSize, m_propagate[25], m_accumulation[3], m_propagate[26], m_lpvConfigUbos[3] },
				LightPropagationPass{ engine, GridSize, m_propagate[26], m_accumulation[3], m_propagate[27], m_lpvConfigUbos[3] },
				LightPropagationPass{ engine, GridSize, m_propagate[27], m_accumulation[3], m_propagate[28], m_lpvConfigUbos[3] },
				LightPropagationPass{ engine, GridSize, m_propagate[28], m_accumulation[3], m_propagate[29], m_lpvConfigUbos[3] },
				LightPropagationPass{ engine, GridSize, m_propagate[29], m_accumulation[3], m_propagate[30], m_lpvConfigUbos[3] },
				LightPropagationPass{ engine, GridSize, m_propagate[30], m_accumulation[3], m_propagate[31], m_lpvConfigUbos[3] },
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
				|| m_lightIndex != light.getShadowMapIndex()
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

					m_lpvConfigUbos[i].update( levelGrid.getMin()
						, levelGrid.getDimensions()
						, m_lightIndex
						, levelGrid.getCellSize() );
					grids[i] = levelGrid;
				}

				m_lpvConfigUbo.update( grids
					, m_lightIndex );
			}
		}

	private:
		LayeredLpvConfigUbo m_lpvConfigUbo;
		std::array< LightVolumePassResult, shader::DirectionalMaxCascadesCount > m_injection;
		std::array< LightVolumePassResult, shader::DirectionalMaxCascadesCount > m_accumulation;
		std::array< LightVolumePassResult, MaxPropagationSteps * shader::DirectionalMaxCascadesCount > m_propagate;
		std::array< LpvConfigUbo, shader::DirectionalMaxCascadesCount > m_lpvConfigUbos;
		std::array< LightInjectionPass, shader::DirectionalMaxCascadesCount > m_lightInjectionPasses;
		std::array< LightPropagationPass, MaxPropagationSteps * shader::DirectionalMaxCascadesCount > m_lightPropagationPasses;
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
