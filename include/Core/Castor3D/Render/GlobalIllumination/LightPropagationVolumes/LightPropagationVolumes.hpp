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

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

namespace castor3d
{
	class LightPropagationVolumesBase
		: public castor::Named
	{
	public:
		static constexpr uint32_t MaxPropagationSteps = 8u;

	protected:
		C3D_API LightPropagationVolumesBase( crg::ResourceHandler & handler
			, Scene const & scene
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
		C3D_API crg::SemaphoreWait render( crg::SemaphoreWait const & toWait );
		C3D_API void accept( PipelineVisitorBase & visitor );

	private:
		crg::FramePass & doCreateClearPass();
		crg::FramePass & doCreatePropagationPass( std::vector< crg::FramePass const * > previousPasses
			, std::string const & name
			, LightVolumePassResult const & injection
			, LightVolumePassResult const & lpvResult
			, LightVolumePassResult const & propagation
			, uint32_t index );
		std::vector< crg::FramePass * > doCreatePropagationPasses();

	private:
		Scene const & m_scene;
		RenderDevice const & m_device;
		ShadowMapResult const & m_smResult;
		LightVolumePassResult const & m_lpvResult;
		castor::Point4f m_gridsSize;
		LpvGridConfigUbo & m_lpvGridConfigUbo;
		bool m_geometryVolumes{ false };
		crg::FrameGraph m_graph;
		bool m_initialised{ false };
		LightType m_lightType;
		LightVolumePassResult m_injection;
		Texture m_geometry;
		std::array< LightVolumePassResult, 2u > m_propagate;
		struct LightLpv
		{
			LightLpv( crg::FrameGraph & graph
				, crg::FramePass const & previousPass
				, RenderDevice const & device
				, castor::String const & name
				, LightCache const & lightCache
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, LightVolumePassResult const & injection
				, Texture * geometry );
			bool update( CpuUpdater & updater
				, float lpvCellSize );

			std::vector< LpvLightConfigUbo > lpvLightConfigUbos;
			crg::FramePass const * lastPass{};
			std::vector< LightInjectionPass * > lightInjectionPasses;
			crg::FramePassArray lightInjectionPassDescs;
			std::vector< GeometryInjectionPass * > geometryInjectionPasses;
			crg::FramePassArray geometryInjectionPassDescs;

		private:
			crg::FramePass const & doCreateInjectionPass( crg::FrameGraph & graph
				, RenderDevice const & device
				, castor::String const & name
				, LightCache const & lightCache
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, LightVolumePassResult const & injection );
			crg::FramePass const & doCreateInjectionPass( crg::FrameGraph & graph
				, RenderDevice const & device
				, castor::String const & name
				, LightCache const & lightCache
				, std::vector< crg::ImageViewId > const & arrayViews
				, CubeMapFace face
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, LightVolumePassResult const & injection );
			crg::FramePassArray doCreateInjectionPasses( crg::FrameGraph & graph
				, RenderDevice const & device
				, castor::String const & name
				, LightCache const & lightCache
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, LightVolumePassResult const & injection );
			crg::FramePass const & doCreateGeometryPass( crg::FrameGraph & graph
				, RenderDevice const & device
				, castor::String const & name
				, LightCache const & lightCache
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, Texture & geometry );
			crg::FramePass const & doCreateGeometryPass( crg::FrameGraph & graph
				, RenderDevice const & device
				, castor::String const & name
				, LightCache const & lightCache
				, std::vector< crg::ImageViewId > const & arrayViews
				, CubeMapFace face
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, Texture & geometry );
			crg::FramePassArray doCreateGeometryPasses( crg::FrameGraph & graph
				, RenderDevice const & device
				, castor::String const & name
				, LightCache const & lightCache
				, LightType lightType
				, ShadowMapResult const & smResult
				, LpvGridConfigUbo const & lpvGridConfigUbo
				, Texture & geometry );
		};
		using LightLpvPtr = std::unique_ptr< LightLpv >;

		crg::FramePass & m_clearPass;
		std::unordered_map< LightSPtr, LightLpvPtr > m_lightLpvs;
		std::vector< crg::FramePass * > m_lightPropagationPassesDesc;
		std::vector< LightPropagationPass * > m_lightPropagationPasses;

		castor::BoundingBox m_aabb;
		castor::Point3f m_cameraPos;
		castor::Point3f m_cameraDir;
		crg::RunnableGraphPtr m_runnable;
	};

	template< bool GeometryVolumesT >
	class LightPropagationVolumesT
		: public LightPropagationVolumesBase
	{
	public:
		LightPropagationVolumesT( crg::ResourceHandler & handler
			, Scene const & scene
			, LightType lightType
			, RenderDevice const & device
			, ShadowMapResult const & smResult
			, LightVolumePassResult const & lpvResult
			, LpvGridConfigUbo & lpvGridConfigUbo )
			: LightPropagationVolumesBase{ handler
				, scene
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
