/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPropagationVolumes_H___
#define ___C3D_LightPropagationVolumes_H___

#include "LightPropagationVolumesModule.hpp"
#include "Castor3D/Event/Frame/FrameEventModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Shader/Ubos/LpvLightConfigUbo.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/BoundingBox.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

namespace c3d
{
	class LightPropagationVolumesBase
		: public Named
	{
	protected:
		C3D_API LightPropagationVolumesBase( crg::ResourcesCache & resources
			, Scene const & scene
			, LightType lightType
			, RenderDevice const & device
			, ShadowMapResult const & smResult
			, LightVolumePassResult const & lpvResult
			, LpvGridConfigUbo & lpvGridConfigUbo
			, bool geometryVolumes );

	public:
		C3D_API void initialise();
		C3D_API void cleanup()noexcept;
		C3D_API void registerLight( LightInstance * light );
		C3D_API void update( CpuUpdater & updater );
		C3D_API SemaphoreWaitArray render( SemaphoreWaitArray const & toWait
			, ashes::Queue const & queue );
		C3D_API void accept( ConfigurationVisitorBase & visitor )const;

	private:
		crg::FramePass & doCreateClearPass();
		crg::FramePass & doCreateDownsamplePass();
		crg::FramePass & doCreatePropagationPass( crg::FramePassArray const & previousPasses
			, String const & name
			, LightVolumePassResult const & injection
			, LightVolumePassResult const & lpvResult
			, LightVolumePassResult const & propagation
			, uint32_t index );
		Vector< crg::FramePass * > doCreatePropagationPasses();

	private:
		Scene const & m_scene;
		RenderDevice const & m_device;
		ShadowMapResult const & m_sourceSmResult;
		RawUniquePtr< ShadowMapResult > m_downsampledSmResult;
		ShadowMapResult const * m_usedSmResult;
		LightVolumePassResult const & m_lpvResult;
		Point4f m_gridsSize;
		LpvGridConfigUbo & m_lpvGridConfigUbo;
		bool m_geometryVolumes{ false };
		crg::FrameGraph m_graph;
		bool m_initialised{ false };
		LightType m_lightType;
		LightVolumePassResult m_injection;
		Texture m_geometry;
		Array< LightVolumePassResult, 2u > m_propagate;
		struct LightLpv
		{
			LightLpv( crg::FramePassGroup & graph
				, crg::FramePassArray const & previousPasses
				, RenderDevice const & device
				, String const & name
				, LightCache const & lightCache
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, LightVolumePassResult const & injection
				, Texture const * geometry );
			bool update( CpuUpdater & updater
				, float lpvCellSize );

			LightCache const & lightCache;
			Vector< LpvLightConfigUbo > lpvLightConfigUbos;
			crg::FramePass const * lastPass{};
			crg::FramePassArray previousPasses{};
			Vector< LightInjectionPass * > lightInjectionPasses;
			crg::FramePassArray lightInjectionPassDescs;
			Vector< GeometryInjectionPass * > geometryInjectionPasses;
			crg::FramePassArray geometryInjectionPassDescs;

		private:
			crg::FramePass const & doCreateInjectionPass( crg::FramePassGroup & graph
				, RenderDevice const & device
				, String const & name
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, LightVolumePassResult const & injection );
			crg::FramePass const & doCreateInjectionPass( crg::FramePassGroup & graph
				, RenderDevice const & device
				, String const & name
				, Vector< crg::ImageViewId > const & arrayViews
				, CubeMapFace face
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, LightVolumePassResult const & injection );
			crg::FramePassArray doCreateInjectionPasses( crg::FramePassGroup & graph
				, RenderDevice const & device
				, String const & name
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, LightVolumePassResult const & injection );
			crg::FramePass const & doCreateGeometryPass( crg::FramePassGroup & graph
				, RenderDevice const & device
				, String const & name
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, Texture const & geometry );
			crg::FramePass const & doCreateGeometryPass( crg::FramePassGroup & graph
				, RenderDevice const & device
				, String const & name
				, Vector< crg::ImageViewId > const & arrayViews
				, CubeMapFace face
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, Texture const & geometry );
			crg::FramePassArray doCreateGeometryPasses( crg::FramePassGroup & graph
				, RenderDevice const & device
				, String const & name
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, Texture const & geometry );
		};
		using LightLpvPtr = RawUniquePtr< LightLpv >;

		crg::FramePass & m_clearPass;
		crg::FramePass * m_downsamplePass;
		HashMap< LightInstance *, LightLpvPtr > m_lightLpvs;
		Vector< crg::FramePass * > m_lightPropagationPassesDesc;
		Vector< LightPropagationPass * > m_lightPropagationPasses;

		BoundingBox m_aabb;
		Point3f m_cameraPos;
		Point3f m_cameraDir;
		GpuFrameEvent * m_recordEvent{};
		crg::RunnableGraphPtr m_runnable;
	};

	template< bool GeometryVolumesT >
	class LightPropagationVolumesT
		: public LightPropagationVolumesBase
	{
	public:
		LightPropagationVolumesT( crg::ResourcesCache & resources
			, Scene const & scene
			, LightType lightType
			, RenderDevice const & device
			, ShadowMapResult const & smResult
			, LightVolumePassResult const & lpvResult
			, LpvGridConfigUbo & lpvGridConfigUbo )
			: LightPropagationVolumesBase{ resources
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
			++result;// light injection

			if constexpr ( GeometryVolumesT )
			{
				++result;// geom injection
			}

			result += LpvMaxPropagationSteps;// propagation
			return result;
		}
	};
}

#endif
