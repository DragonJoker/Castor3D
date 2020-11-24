/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPropagationVolumes_H___
#define ___C3D_LightPropagationVolumes_H___

#include "LightPropagationVolumesModule.hpp"

#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Shader/Ubos/LpvLightConfigUbo.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/BoundingBox.hpp>

namespace castor3d
{
	class LightPropagationVolumesBase
		: public castor::Named
	{
	public:
		static constexpr uint32_t MaxPropagationSteps = 8u;

	protected:
		C3D_API LightPropagationVolumesBase( Scene const & scene
			, LightType lightType
			, RenderDevice const & device
			, ShadowMapResult const & smResult
			, LightVolumePassResult const & lpvResult
			, LpvGridConfigUbo & lpvGridConfigUbo
			, bool geometryVolumes );

	public:
		C3D_API void initialise();
		C3D_API void cleanup();
		C3D_API void registerLight( LightSPtr light );
		C3D_API void update( CpuUpdater & updater );
		C3D_API ashes::Semaphore const & render( ashes::Semaphore const & toWait );
		C3D_API void accept( PipelineVisitorBase & visitor );

	private:
		Scene const & m_scene;
		Engine & m_engine;
		RenderDevice const & m_device;
		ShadowMapResult const & m_smResult;
		LightVolumePassResult const & m_lpvResult;
		LpvGridConfigUbo & m_lpvGridConfigUbo;
		bool m_geometryVolumes{ false };
		bool m_initialised{ false };
		LightType m_lightType;
		LightVolumePassResult m_injection;
		TextureUnit m_geometry;
		std::array< LightVolumePassResult, 2u > m_propagate;
		CommandsSemaphore m_clearCommands;
		struct LightLpv
		{
			LightLpv( RenderDevice const & device
				, castor::String const & name
				, LightCache const & lightCache
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, LightVolumePassResult const & injection
				, uint32_t cascadeIndex
				, float lpvCellSize
				, TextureUnit * geometry );
			bool update( CpuUpdater & updater );
			LpvLightConfigUbo lpvLightConfigUbo;
			float lpvCellSize;
			LightInjectionPassUPtr lightInjectionPass;
			GeometryInjectionPassUPtr geometryInjectionPass;
		};
		std::unordered_map< LightSPtr, LightLpv > m_lightLpvs;
		LightPropagationPassArray  m_lightPropagationPasses;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
	};

	template< bool GeometryVolumesT >
	class LightPropagationVolumesT
		: public LightPropagationVolumesBase
	{
	public:
		LightPropagationVolumesT( Scene const & scene
			, LightType lightType
			, RenderDevice const & device
			, ShadowMapResult const & smResult
			, LightVolumePassResult const & lpvResult
			, LpvGridConfigUbo & lpvGridConfigUbo )
			: LightPropagationVolumesBase{ scene
				, lightType
				, device
				, smResult
				, lpvResult
				, lpvGridConfigUbo
				, GeometryVolumesT }
		{
		}
	};
}

#endif
