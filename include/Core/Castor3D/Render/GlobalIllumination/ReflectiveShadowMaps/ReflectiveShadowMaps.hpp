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

namespace c3d
{
	class ReflectiveShadowMaps
		: public Named
	{
	public:
		C3D_API ReflectiveShadowMaps( crg::ResourcesCache & resources
			, Scene const & scene
			, RenderDevice const & device
			, CameraUbo const & cameraUbo
			, ShadowBuffer const & shadowBuffer
			, Texture const & depthObj
			, Texture const & nmlOcc
			, ShadowMapResult const & directionalSmResult
			, ShadowMapResult const & pointSmResult
			, ShadowMapResult const & spotSmResult
			, Texture & result );
		C3D_API ~ReflectiveShadowMaps()noexcept;

	public:
		C3D_API void initialise();
		C3D_API void cleanup();
		C3D_API void registerLight( LightInstance * light );
		C3D_API void update( CpuUpdater & updater );
		C3D_API SemaphoreWaitArray render( SemaphoreWaitArray const & toWait
			, ashes::Queue const & queue );
		C3D_API void accept( ConfigurationVisitorBase & visitor );

	private:
		crg::FramePass & doCreateClearPass();

	private:
		Scene const & m_scene;
		RenderDevice const & m_device;
		CameraUbo const & m_cameraUbo;
		ShadowBuffer const & m_shadowBuffer;
		Texture const & m_depthObj;
		Texture const & m_nmlOcc;
		ShadowMapResult const & m_directionalSmResult;
		ShadowMapResult const & m_pointSmResult;
		ShadowMapResult const & m_spotSmResult;
		crg::FrameGraph m_graph;
		bool m_initialised{ false };
		TextureArray m_intermediate;
		Texture & m_result;
		struct LightRsm
		{
			LightRsm( crg::FrameGraph & graph
				, RenderDevice const & device
				, LightCache const & lightCache
				, LightType lightType
				, ShadowBuffer const & shadowBuffer
				, CameraUbo const & cameraUbo
				, Texture const & depthObj
				, Texture const & nmlOcc
				, ShadowMapResult const & smResult
				, TextureArray & intermediate
				, Texture & result );
			void update( CpuUpdater & updater );

			LightCache const & lightCache;
			RsmGIPassUPtr giPass;
			RsmInterpolatePassUPtr interpolatePass;
		};
		using LightRsmPtr = std::unique_ptr< LightRsm >;

		std::unordered_map< LightInstance *, LightRsmPtr > m_lightRsms;
		GpuFrameEvent * m_recordEvent{};
		crg::RunnableGraphPtr m_runnable;
	};
}

#endif
