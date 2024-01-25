/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneCuller_H___
#define ___C3D_SceneCuller_H___

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include "Castor3D/Limits.hpp"

namespace castor3d
{
	AnimatedObjectRPtr findAnimatedObject( Scene const & scene
		, castor::String const & name );

	size_t hash( SubmeshRenderNode const & culled );
	bool isVisible( Camera const & camera
		, SubmeshRenderNode const & node );
	bool isVisible( Frustum const & frustum
		, SubmeshRenderNode const & node );

	size_t hash( BillboardRenderNode const & culled );
	bool isVisible( Camera const & camera
		, BillboardRenderNode const & node );
	bool isVisible( Frustum const & frustum
		, BillboardRenderNode const & node );

	class SceneCuller
	{
	public:
		C3D_API SceneCuller( Scene & scene
			, Camera * camera
			, std::optional< bool > isStatic = std::nullopt );
		C3D_API virtual ~SceneCuller()noexcept;

		C3D_API void update( CpuUpdater & updater );
		C3D_API void removeCulled( SubmeshRenderNode const & node );
		C3D_API void removeCulled( BillboardRenderNode const & node );
		C3D_API void resetCamera( Camera * camera );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Scene & getScene()const noexcept
		{
			return m_scene;
		}

		bool hasCamera()const noexcept
		{
			return m_camera != nullptr;
		}

		bool hasNodes()const noexcept
		{
			return !m_culledSubmeshes.empty()
				|| !m_culledBillboards.empty();
		}

		Camera const & getCamera()const noexcept
		{
			CU_Require( hasCamera() );
			return *m_camera;
		}

		Camera & getCamera()noexcept
		{
			CU_Require( hasCamera() );
			return *m_camera;
		}

		bool areAnyChanged()const noexcept
		{
			return m_anyChanged;
		}

		bool areCulledChanged()const noexcept
		{
			return m_culledChanged;
		}

		NodeArrayT< SubmeshRenderNode, CulledNodePtrT > const & getSubmeshes()const noexcept
		{
			return m_culledSubmeshes;
		}

		NodeArrayT< BillboardRenderNode, CulledNodePtrT > const & getBillboards()const noexcept
		{
			return m_culledBillboards;
		}

		RenderCounts const & getTotalCounts()const noexcept
		{
			return m_total;
		}
		/**@}*/

	public:
		mutable SceneCullerSignal onCompute;
		mutable SceneCullerSubmeshSignal onSubmeshChanged;
		mutable SceneCullerBillboardSignal onBillboardChanged;

	private:
		void doInitialiseCulled();
		void doUpdateChanged( CpuUpdater::DirtyObjects & sceneObjs );
		void doUpdateCulled( CpuUpdater::DirtyObjects const & sceneObjs );
		void doMarkDirty( CpuUpdater::DirtyObjects const & sceneObjs
			, castor::Vector< SubmeshRenderNode const * > & dirtySubmeshes
			, castor::Vector< BillboardRenderNode const * > & dirtyBillboards )const;
		void duUpdateCulledSubmeshes( castor::Vector< SubmeshRenderNode const * > const & dirtySubmeshes );
		void duUpdateCulledBillboards( castor::Vector< BillboardRenderNode const * > const & dirtyBillboards );
		void doMakeDirty( Geometry const & object
			, castor::Vector< SubmeshRenderNode const * > & dirtySubmeshes )const;
		void doMakeDirty( BillboardBase const & object
			, castor::Vector< BillboardRenderNode const * > & dirtyBillboards )const;
		virtual bool isSubmeshVisible( SubmeshRenderNode const & node )const = 0;
		virtual bool isBillboardVisible( BillboardRenderNode const & node )const = 0;

	private:
		Scene & m_scene;

	protected:
		Camera * m_camera;
		uint32_t m_index;
		bool m_first{ true };
		bool m_anyChanged{ true };
		bool m_culledChanged{ true };
		FramePassTimerUPtr m_timer;
		FramePassTimerUPtr m_timerDirty;
		FramePassTimerUPtr m_timerCompute;
		std::optional< bool > m_isStatic;
		RenderCounts m_total{};
		NodeArrayT< SubmeshRenderNode, CulledNodePtrT > m_culledSubmeshes;
		NodeArrayT< BillboardRenderNode, CulledNodePtrT > m_culledBillboards;
	};
}

#endif
