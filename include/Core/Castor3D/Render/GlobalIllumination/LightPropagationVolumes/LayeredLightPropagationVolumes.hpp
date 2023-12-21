/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LayeredLightPropagationVolumes_H___
#define ___C3D_LayeredLightPropagationVolumes_H___

#include "LightPropagationVolumesModule.hpp"
#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/GeometryInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvLightConfigUbo.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/BoundingBox.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

#include <array>

namespace castor3d
{
	class LayeredLightPropagationVolumesBase
		: public castor::Named
	{
	protected:
		C3D_API LayeredLightPropagationVolumesBase( crg::ResourcesCache & resources
			, Scene const & scene
			, LightType lightType
			, RenderDevice const & device
			, ShadowMapResult const & smResult
			, LightVolumePassResultArray const & lpvResult
			, LayeredLpvGridConfigUbo & lpvGridConfigUbo
			, bool geometryVolumes );

	public:
		C3D_API void initialise();
		C3D_API void cleanup();
		C3D_API void registerLight( Light * light );
		C3D_API void update( CpuUpdater & updater );
		C3D_API crg::SemaphoreWaitArray render( crg::SemaphoreWaitArray const & toWait
			, ashes::Queue const & queue );
		C3D_API void accept( ConfigurationVisitorBase & visitor );

	private:
		crg::FramePass & doCreateClearInjectionPass();
		crg::FramePass & doCreateDownsamplePass();
		crg::FramePass & doCreatePropagationPass( std::vector< crg::FramePass const * > previousPasses
			, std::string const & name
			, LightVolumePassResult const & injection
			, LightVolumePassResult const & lpvResult
			, LightVolumePassResult const & propagation
			, uint32_t cascade
			, uint32_t index );
		std::vector< crg::FramePass * > doCreatePropagationPasses();

	private:
		Scene const & m_scene;
		RenderDevice const & m_device;
		ShadowMapResult const & m_sourceSmResult;
		ShadowMapResult m_downsampledSmResult;
		LightVolumePassResultArray const & m_lpvResult;
		LayeredLpvGridConfigUbo & m_lpvGridConfigUbo;
		std::array< castor::Grid const *, LpvMaxCascadesCount > m_grids;
		std::array < castor::Point4f, LpvMaxCascadesCount > m_gridsSizes;
		LpvGridConfigUboArray m_lpvGridConfigUbos;
		bool m_geometryVolumes{ false };
		crg::FrameGraph m_graph;
		bool m_initialised{ false };
		LightType m_lightType;
		std::vector< LightVolumePassResult > m_injection;
		TextureArray m_geometry;
		std::vector< std::array< LightVolumePassResult, 2u > > m_propagate;
		struct LightLpv
		{
			LightLpv( crg::FrameGraph & graph
				, crg::FramePass const & previousPass
				, RenderDevice const & device
				, castor::String const & name
				, LightCache const & lightCache
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUboArray const & lpvGridConfigUbos
				, std::vector< LightVolumePassResult > const & injection
				, TextureArray const * geometry );
			bool update( CpuUpdater & updater
				, std::vector< float > const & lpvCellSizes );

			LightCache const & lightCache;
			LpvLightConfigUboArray lpvLightConfigUbos;
			crg::FramePass const * lastLightPass{};
			crg::FramePass const * lastGeomPass{};
			std::vector< LightInjectionPass * > lightInjectionPasses{};
			std::vector< crg::FramePass * > lightInjectionPassesDesc;
			std::vector< GeometryInjectionPass * > geometryInjectionPasses{};
			std::vector< crg::FramePass * > geometryInjectionPassesDesc{};

		private:
			crg::FramePass & doCreateInjectionPass( crg::FrameGraph & graph
				, RenderDevice const & device
				, castor::String const & name
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUboArray const & lpvGridConfigUbos
				, std::vector< LightVolumePassResult > const & injection
				, uint32_t cascade );
			crg::FramePass & doCreateGeometryPass( crg::FrameGraph & graph
				, RenderDevice const & device
				, castor::String const & name
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUboArray const & lpvGridConfigUbos
				, TextureArray const & geometry
				, uint32_t cascade );
		};

		crg::FramePass & m_clearInjectionPass;
		crg::FramePass & m_downsamplePass;
		std::unordered_map< Light *, LightLpv > m_lightLpvs;
		std::vector< crg::FramePass * > m_lightPropagationPassesDesc;
		std::vector< LightPropagationPass * > m_lightPropagationPasses;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
		GpuFrameEvent * m_recordEvent{};
		crg::RunnableGraphPtr m_runnable;
	};

	template< bool GeometryVolumesT >
	class LayeredLightPropagationVolumesT
		: public LayeredLightPropagationVolumesBase
	{
	public:
		LayeredLightPropagationVolumesT( crg::ResourcesCache & resources
			, Scene const & scene
			, LightType lightType
			, RenderDevice const & device
			, ShadowMapResult const & smResult
			, LightVolumePassResultArray const & lpvResult
			, LayeredLpvGridConfigUbo & lpvGridConfigUbo )
			: LayeredLightPropagationVolumesBase{ resources
				, scene
				, lightType
				, device
				, smResult
				, lpvResult
				, lpvGridConfigUbo
				, GeometryVolumesT }
		{
		}

		static uint32_t countInitialisationSteps()noexcept
		{
			uint32_t result = 0u;
			++result;// light injection;

			if constexpr ( GeometryVolumesT )
			{
				++result;// geom injection;
			}

			result += LpvMaxPropagationSteps;// propagation;
			return result;
		}
	};
}

#endif
