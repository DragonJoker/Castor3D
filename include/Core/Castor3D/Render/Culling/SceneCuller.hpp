/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneCuller_H___
#define ___C3D_SceneCuller_H___

#include "CullingModule.hpp"

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

namespace castor3d
{
	struct CulledSubmesh
	{
		Geometry & instance;
		Submesh & data;
		PassSPtr pass;
		SceneNode & sceneNode;
	};

	struct CulledBillboard
	{
		BillboardBase & instance;
		BillboardBase & data;
		PassSPtr pass;
		SceneNode & sceneNode;
	};

	class SceneCuller
	{
	public:
	public:
		C3D_API SceneCuller( Scene const & scene
			, Camera * camera );
		C3D_API virtual ~SceneCuller() = default;
		C3D_API void compute();

		inline float getMinCastersZ()
		{
			return m_minCullersZ;
		}

		inline Scene const & getScene()const
		{
			return m_scene;
		}

		inline bool hasCamera()const
		{
			return m_camera != nullptr;
		}

		inline Camera const & getCamera()const
		{
			CU_Require( hasCamera() );
			return *m_camera;
		}

		inline Camera & getCamera()
		{
			CU_Require( hasCamera() );
			return *m_camera;
		}

		inline bool areAllChanged()const
		{
			return m_allChanged;
		}

		inline bool areCulledChanged()const
		{
			return m_culledChanged;
		}

		inline std::vector< CulledSubmesh > const & getAllSubmeshes( bool opaque )const
		{
			return opaque
				? m_allOpaqueSubmeshes
				: m_allTransparentSubmeshes;
		}

		inline std::vector< CulledBillboard > const & getAllBillboards( bool opaque )const
		{
			return opaque
				? m_allOpaqueBillboards
				: m_allTransparentBillboards;
		}

		inline std::vector< CulledSubmesh * > const & getCulledSubmeshes( bool opaque )const
		{
			return opaque
				? m_culledOpaqueSubmeshes
				: m_culledTransparentSubmeshes;
		}

		inline std::vector< CulledBillboard * > const & getCulledBillboards( bool opaque )const
		{
			return opaque
				? m_culledOpaqueBillboards
				: m_culledTransparentBillboards;
		}

	public:
		mutable SceneCullerSignal onCompute;

	private:
		void onSceneChanged( Scene const & scene );
		void onCameraChanged( Camera const & camera );
		void doListGeometries();
		void doListBillboards();
		void doListParticles();
		virtual void doCullGeometries() = 0;
		virtual void doCullBillboards() = 0;

	private:
		Scene const & m_scene;
		Camera * m_camera;

	protected:
		bool m_allChanged{ true };
		bool m_culledChanged{ true };
		bool m_sceneDirty{ true };
		bool m_cameraDirty{ true };
		float m_minCullersZ{ 0.0f };
		std::vector< CulledSubmesh > m_allOpaqueSubmeshes;
		std::vector< CulledSubmesh > m_allTransparentSubmeshes;
		std::vector< CulledBillboard > m_allOpaqueBillboards;
		std::vector< CulledBillboard > m_allTransparentBillboards;
		std::vector< CulledSubmesh * > m_culledOpaqueSubmeshes;
		std::vector< CulledSubmesh * > m_culledTransparentSubmeshes;
		std::vector< CulledBillboard * > m_culledOpaqueBillboards;
		std::vector< CulledBillboard * > m_culledTransparentBillboards;
		OnSceneChangedConnection m_sceneChanged;
		OnCameraChangedConnection m_cameraChanged;
	};
}

#endif
