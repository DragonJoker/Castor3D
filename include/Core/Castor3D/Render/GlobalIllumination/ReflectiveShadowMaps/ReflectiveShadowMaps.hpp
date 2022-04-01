/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ReflectiveShadowMaps_H___
#define ___C3D_ReflectiveShadowMaps_H___

#include "ReflectiveShadowMapsModule.hpp"

#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

namespace castor3d
{
	class ReflectiveShadowMaps
		: public castor::Named
	{
	public:
		C3D_API ReflectiveShadowMaps( crg::ResourceHandler & handler
			, Scene const & scene
			, RenderDevice const & device
			, CameraUbo const & cameraUbo
			, ShadowBuffer const & shadowBuffer
			, crg::ImageViewId const & depthObj
			, crg::ImageViewId const & nmlOcc
			, ShadowMapResult const & directionalSmResult
			, ShadowMapResult const & pointSmResult
			, ShadowMapResult const & spotSmResult
			, Texture const & result );
		C3D_API ~ReflectiveShadowMaps()noexcept;

	public:
		C3D_API void initialise();
		C3D_API void cleanup();
		C3D_API void registerLight( Light * light );
		C3D_API void update( CpuUpdater & updater );
		C3D_API crg::SemaphoreWaitArray render( crg::SemaphoreWaitArray const & toWait
			, ashes::Queue const & queue );
		C3D_API void accept( ConfigurationVisitorBase & visitor );

	private:
		crg::FramePass & doCreateClearPass();

	private:
		Scene const & m_scene;
		RenderDevice const & m_device;
		CameraUbo const & m_cameraUbo;
		ShadowBuffer const & m_shadowBuffer;
		crg::ImageViewId const & m_depthObj;
		crg::ImageViewId const & m_nmlOcc;
		ShadowMapResult const & m_directionalSmResult;
		ShadowMapResult const & m_pointSmResult;
		ShadowMapResult const & m_spotSmResult;
		crg::FrameGraph m_graph;
		bool m_initialised{ false };
		TextureArray m_intermediate;
		Texture const & m_result;
		struct LightRsm
		{
			LightRsm( crg::FrameGraph & graph
				, crg::FramePassArray previousPasses
				, RenderDevice const & device
				, LightCache const & lightCache
				, LightType lightType
				, ShadowBuffer const & shadowBuffer
				, CameraUbo const & cameraUbo
				, crg::ImageViewId const & depthObj
				, crg::ImageViewId const & nmlOcc
				, ShadowMapResult const & smResult
				, TextureArray const & intermediate
				, Texture const & result );
			void update( CpuUpdater & updater );

			LightCache const & lightCache;
			RsmGIPassUPtr giPass;
			RsmInterpolatePassUPtr interpolatePass;
			crg::FramePass const * lastPass{};
		};
		using LightRsmPtr = std::unique_ptr< LightRsm >;

		crg::FramePass const & m_clearPass;
		crg::FramePass const * m_lastPass;
		std::unordered_map< Light *, LightRsmPtr > m_lightRsms;
		crg::RunnableGraphPtr m_runnable;
	};
}

#endif
