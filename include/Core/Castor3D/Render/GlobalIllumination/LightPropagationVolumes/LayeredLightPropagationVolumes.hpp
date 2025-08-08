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

namespace c3d
{
	class LayeredLightPropagationVolumesBase
		: public Named
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
		C3D_API void cleanup()noexcept;
		C3D_API void registerLight( LightInstance * light );
		C3D_API void update( CpuUpdater & updater );
		C3D_API SemaphoreWaitArray render( SemaphoreWaitArray const & toWait
			, ashes::Queue const & queue );
		C3D_API void accept( ConfigurationVisitorBase & visitor );

	private:
		void doCreateClearPass();
		void doCreateDownsamplePass();
		crg::FramePass & doCreatePropagationPass( String const & name
			, LightVolumePassResult const & injection
			, LightVolumePassResult & lpvResult
			, LightVolumePassResult & propagation
			, uint32_t cascade
			, uint32_t index );
		void doCreatePropagationPasses();

	private:
		Scene const & m_scene;
		RenderDevice const & m_device;
		ShadowMapResult const & m_sourceSmResult;
		ShadowMapResult m_downsampledSmResult;
		LightVolumePassResultArray const & m_lpvResult;
		LayeredLpvGridConfigUbo & m_lpvGridConfigUbo;
		Array< Grid const *, LpvMaxCascadesCount > m_grids;
		Array < Point4f, LpvMaxCascadesCount > m_gridsSizes;
		LpvGridConfigUboArray m_lpvGridConfigUbos;
		bool m_geometryVolumes{ false };
		crg::FrameGraph m_graph;
		bool m_initialised{ false };
		LightType m_lightType;
		Vector< LightVolumePassResult > m_injection;
		TextureArray m_geometry;
		Vector< Array< LightVolumePassResult, 2u > > m_propagate;
		Vector< crg::FramePass * > m_lightPropagationFirstPasses{};
		struct LightLpv
		{
			LightLpv( crg::FramePassGroup & graph
				, RenderDevice const & device
				, String const & name
				, LightCache const & lightCache
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUboArray const & lpvGridConfigUbos
				, Vector< LightVolumePassResult > & injection
				, TextureArray * geometry );
			bool update( CpuUpdater & updater
				, Vector< float > const & lpvCellSizes );

			LightCache const & lightCache;
			LpvLightConfigUboArray lpvLightConfigUbos;
			Vector< LightInjectionPass * > lightInjectionPasses{};
			Vector< GeometryInjectionPass * > geometryInjectionPasses{};

		private:
			void doCreateInjectionPass( crg::FramePassGroup & graph
				, RenderDevice const & device
				, String const & name
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUboArray const & lpvGridConfigUbos
				, Vector< LightVolumePassResult > & injection
				, uint32_t cascade );
			void doCreateGeometryPass( crg::FramePassGroup & graph
				, RenderDevice const & device
				, String const & name
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUboArray const & lpvGridConfigUbos
				, TextureArray & geometry
				, uint32_t cascade );
		};
		using LightLpvPtr = RawUniquePtr< LightLpv >;

		HashMap< LightInstance *, LightLpvPtr > m_lightLpvs;
		Vector< LightPropagationPass * > m_lightPropagationPasses;

		BoundingBox m_aabb;
		Point3f m_cameraPos;
		Point3f m_cameraDir;
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
