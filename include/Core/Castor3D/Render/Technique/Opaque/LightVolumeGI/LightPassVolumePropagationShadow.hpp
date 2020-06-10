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
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightInjectionPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightPropagationPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightVolumeGIPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightVolumePassResult.hpp"
#include "Castor3D/Shader/Ubos/LightInjectionUbo.hpp"

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
			, m_lpvUbo{ engine }
			, m_lightInjectionPass{ engine
				, lightCache
				, LtType
				, smResult
				, gpInfoUbo
				, m_lpvUbo
				, GridSize }
			, m_lightPropagationPass{ engine
				, m_lightInjectionPass.getResult()
				, m_lpvUbo }
			, m_lightVolumeGIPass{ engine
				, LtType
				, gpInfoUbo
				, m_lpvUbo
				, gpResult
				, m_lightPropagationPass.getResult()
				, lpResult[LpTexture::eDiffuse] }
			, m_aabb{ lightCache.getScene()->getBoundingBox() }
			, m_grid{ GridSize, 2.5f, m_aabb.getMax(), m_aabb.getMin(), 1.0f, 0 }
		{
		}

		ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait )override
		{
			auto result = &toWait;
			result = &my_pass_type::render( index, *result );
			result = &m_lightInjectionPass.compute( *result );
			result = &m_lightPropagationPass.compute( *result );
			result = &m_lightVolumeGIPass.compute( *result );

			return *result;
		}

		void accept( PipelineVisitorBase & visitor )override
		{
			LightPassShadow< LtType >::accept( visitor );
			m_lightInjectionPass.accept( visitor );
			m_lightPropagationPass.accept( visitor );
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
			Point3f camDir{ 0, 0, 1 };
			camera.getParent()->getDerivedOrientation().transform( camDir, camDir );

			if ( m_aabb != aabb
				|| m_lightIndex != light.getShadowMapIndex()
				|| m_cameraPos != camPos
				|| m_cameraDir != camDir )
			{
				m_lightIndex = light.getShadowMapIndex();
				m_aabb = aabb;
				m_cameraPos = camPos;
				m_cameraDir = camDir;
				auto cellSize = std::max( std::max( m_aabb.getDimensions()->x
					, m_aabb.getDimensions()->y )
					, m_aabb.getDimensions()->z ) / GridSize;
				m_grid = Grid{ GridSize, cellSize, m_aabb.getMax(), m_aabb.getMin(), 1.0f, 0 };
				m_grid.transform( m_cameraPos, m_cameraDir );

				m_lpvUbo.update( m_grid.getMin()
					, m_grid.getDimensions()
					, m_lightIndex
					, m_grid.getCellSize() );
			}
		}

	private:
		LightInjectionUbo m_lpvUbo;
		LightInjectionPass m_lightInjectionPass;
		LightPropagationPass m_lightPropagationPass;
		LightVolumeGIPass m_lightVolumeGIPass;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
		uint32_t m_lightIndex{};
		castor::Grid m_grid;
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
