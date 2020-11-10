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
	size_t hash( CulledSubmesh const & culled );
	size_t hash( CulledSubmesh const & culled
		, uint32_t instanceMult );
	bool isVisible( Camera const & camera
		, CulledSubmesh const & node );
	bool isVisible( Frustum const & frustum
		, CulledSubmesh const & node );

	struct CulledBillboard
	{
		BillboardBase & instance;
		BillboardBase & data;
		PassSPtr pass;
		SceneNode & sceneNode;
	};
	size_t hash( CulledBillboard const & culled );
	size_t hash( CulledBillboard const & culled
		, uint32_t instanceMult );
	bool isVisible( Camera const & camera
		, CulledBillboard const & node );
	bool isVisible( Frustum const & frustum
		, CulledBillboard const & node );

	class SceneCuller
	{
	public:
		template< typename CulledT, typename ArrayT >
		struct CulledInstancesArrayT
		{
			std::vector< CulledT > objects;
			std::vector< ArrayT > instances;
			uint32_t count;

			void clear()noexcept
			{
				objects.clear();
				instances.clear();
			}

			void push_back( CulledT object
				, ArrayT instance )
			{
				objects.push_back( std::move( object ) );
				instances.push_back( std::move( instance ) );
			}

			void copy( CulledInstancesArrayT< CulledT *, ArrayT * > & dst )
			{
				for ( auto & node : objects )
				{
					dst.objects.push_back( &node );
				}

				for ( auto & node : instances )
				{
					dst.instances.push_back( &node );
				}
			}
		};

		template< typename CulledT >
		using CulledInstancesT = CulledInstancesArrayT< CulledT, UInt32Array >;
		template< typename CulledT >
		using CulledInstancesPtrT = CulledInstancesArrayT< CulledT *, UInt32Array * >;

	public:
		C3D_API SceneCuller( Scene & scene
			, Camera * camera
			, uint32_t instancesCount );
		C3D_API virtual ~SceneCuller() = default;
		C3D_API void compute();

		inline float getMinCastersZ()
		{
			return m_minCullersZ;
		}

		inline Scene & getScene()const
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

		inline CulledInstancesT< CulledSubmesh > const & getAllSubmeshes( bool opaque )const
		{
			return opaque
				? m_allOpaqueSubmeshes
				: m_allTransparentSubmeshes;
		}

		inline CulledInstancesT< CulledBillboard > const & getAllBillboards( bool opaque )const
		{
			return opaque
				? m_allOpaqueBillboards
				: m_allTransparentBillboards;
		}

		inline CulledInstancesPtrT< CulledSubmesh > const & getCulledSubmeshes( bool opaque )const
		{
			return opaque
				? m_culledOpaqueSubmeshes
				: m_culledTransparentSubmeshes;
		}

		inline CulledInstancesPtrT< CulledBillboard > const & getCulledBillboards( bool opaque )const
		{
			return opaque
				? m_culledOpaqueBillboards
				: m_culledTransparentBillboards;
		}

	public:
		mutable SceneCullerSignal onCompute;

	protected:
		UInt32Array getInitialInstances()const;

	private:
		void onSceneChanged( Scene const & scene );
		void onCameraChanged( Camera const & camera );
		void doClearAll();
		void doClearCulled();
		void doListGeometries();
		void doListBillboards();
		void doListParticles();
		virtual void doCullGeometries() = 0;
		virtual void doCullBillboards() = 0;

	private:
		Scene & m_scene;
		Camera * m_camera;

	protected:
		uint32_t m_instancesCount;
		bool m_allChanged{ true };
		bool m_culledChanged{ true };
		bool m_sceneDirty{ true };
		bool m_cameraDirty{ true };
		float m_minCullersZ{ 0.0f };
		CulledInstancesT< CulledSubmesh > m_allOpaqueSubmeshes;
		CulledInstancesT< CulledSubmesh > m_allTransparentSubmeshes;
		CulledInstancesT< CulledBillboard > m_allOpaqueBillboards;
		CulledInstancesT< CulledBillboard > m_allTransparentBillboards;
		CulledInstancesPtrT< CulledSubmesh > m_culledOpaqueSubmeshes;
		CulledInstancesPtrT< CulledSubmesh > m_culledTransparentSubmeshes;
		CulledInstancesPtrT< CulledBillboard > m_culledOpaqueBillboards;
		CulledInstancesPtrT< CulledBillboard > m_culledTransparentBillboards;
		OnSceneChangedConnection m_sceneChanged;
		OnCameraChangedConnection m_cameraChanged;
	};
}

#endif
