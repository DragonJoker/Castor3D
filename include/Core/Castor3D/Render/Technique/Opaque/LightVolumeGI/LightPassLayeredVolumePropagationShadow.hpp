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
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/Ubos/LpvConfigUbo.hpp"
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
				, gpInfoUbo }
			, m_gpResult{ gpResult }
			, m_smResult{ smResult }
			, m_lpResult{ lpResult }
			, m_gpInfoUbo{ gpInfoUbo }
			, m_lpvConfigUbo{ device }
			, m_injection
			{
				LightVolumePassResult{ engine, device, this->getName() + "Injection0", GridSize },
				LightVolumePassResult{ engine, device, this->getName() + "Injection1", GridSize },
				LightVolumePassResult{ engine, device, this->getName() + "Injection2", GridSize },
				LightVolumePassResult{ engine, device, this->getName() + "Injection3", GridSize },
			}
			, m_geometry
			{
				GeometryInjectionPass::createResult( engine, device, this->getName(), 0u, GridSize ),
				GeometryInjectionPass::createResult( engine, device, this->getName(), 1u, GridSize ),
				GeometryInjectionPass::createResult( engine, device, this->getName(), 2u, GridSize ),
				GeometryInjectionPass::createResult( engine, device, this->getName(), 3u, GridSize ),
			}
			, m_accumulation
			{
				LightVolumePassResult{ engine, device, this->getName() + "Accumulation0", GridSize },
				LightVolumePassResult{ engine, device, this->getName() + "Accumulation1", GridSize },
				LightVolumePassResult{ engine, device, this->getName() + "Accumulation2", GridSize },
				LightVolumePassResult{ engine, device, this->getName() + "Accumulation3", GridSize },
			}
			, m_propagate
			{
				LightVolumePassResult{ engine, device, this->getName() + "Propagate0", GridSize },
				LightVolumePassResult{ engine, device, this->getName() + "Propagate1", GridSize },
			}
			, m_lpvConfigUbos
			{
				LpvConfigUbo{ device, 0u },
				LpvConfigUbo{ device, 1u },
				LpvConfigUbo{ device, 2u },
				LpvConfigUbo{ device, 3u },
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
			uint32_t propIndex = 0u;

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
					, GridSize
					, cascade );
				m_geometryInjectionPasses.emplace_back( this->m_engine
					, this->m_device
					, this->getName()
					, lightCache
					, LtType
					, m_smResult
					, m_gpInfoUbo
					, m_lpvConfigUbos[cascade]
					, m_geometry[cascade]
					, GridSize
					, cascade );
				m_lightPropagationPasses.emplace_back( this->m_engine
					, this->m_device
					, this->getName()
					, castor::string::toString( cascade ) + "x0"
					, GridSize
					, m_injection[cascade]
					, m_accumulation[cascade]
					, m_propagate[propIndex]
					, m_lpvConfigUbos[cascade] );

				for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
				{
					m_lightPropagationPasses.emplace_back( this->m_engine
						, this->m_device
						, this->getName()
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

			m_lightVolumeGIPass = std::make_unique< LayeredLightVolumeGIPass >( this->m_engine
				, this->m_device
				, this->getName()
				, m_gpInfoUbo
				, m_lpvConfigUbo
				, m_gpResult
				, m_accumulation[0u]
				, m_accumulation[1u]
				, m_accumulation[2u]
				, m_accumulation[3u]
				, m_lpResult[LpTexture::eIndirect] );
			LightPassShadow< LtType >::initialise( scene, gp, sceneUbo, timer );
		}

		void cleanup()override
		{
			LightPassShadow< LtType >::cleanup();
			m_lightVolumeGIPass.reset();
			m_lightPropagationPasses.clear();
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
					m_lpvConfigUbos[i].cpuUpdate( levelGrid, light, i );
					grids[i] = levelGrid;
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
		std::array< LpvConfigUbo, shader::DirectionalMaxCascadesCount > m_lpvConfigUbos;
		std::array< LightVolumePassResult, shader::DirectionalMaxCascadesCount > m_injection;
		std::array< TextureUnit, shader::DirectionalMaxCascadesCount > m_geometry;
		std::array< LightVolumePassResult, shader::DirectionalMaxCascadesCount > m_accumulation;
		std::array< LightVolumePassResult, 2u > m_propagate;
		LightInjectionPassArray m_lightInjectionPasses;
		GeometryInjectionPassArray m_geometryInjectionPasses;
		LightPropagationPassArray m_lightPropagationPasses;
		LayeredLightVolumeGIPassUPtr m_lightVolumeGIPass;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
		uint32_t m_lightIndex{};
	};
}

#endif
