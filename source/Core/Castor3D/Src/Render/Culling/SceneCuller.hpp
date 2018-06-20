/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneCuller_H___
#define ___C3D_SceneCuller_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/**
	*\~english
	*\brief
	*	Base class to cull nodes, before adding them to the render queue.
	*\~french
	*\brief
	*	Classe de base pour éliminer les noeuds, avant de les ajouter à la file de rendu.
	*/
	class SceneCuller
	{
	public:
		struct CulledSubmesh
		{
			Geometry & instance;
			Submesh & data;
			MaterialSPtr material;
			SceneNode & sceneNode;
		};
		
		struct CulledBillboard
		{
			BillboardBase & instance;
			BillboardBase & data;
			MaterialSPtr material;
			SceneNode & sceneNode;
		};

	public:
		C3D_API SceneCuller( Scene const & scene
			, Camera * camera );
		C3D_API virtual ~SceneCuller() = default;
		C3D_API virtual void compute() = 0;

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
			REQUIRE( hasCamera() );
			return *m_camera;
		}

		inline Camera & getCamera()
		{
			REQUIRE( hasCamera() );
			return *m_camera;
		}

		inline bool isChanged()const
		{
			return m_changed;
		}

		inline std::vector< CulledSubmesh > const & getSubmeshes( bool opaque )const
		{
			return opaque
				? m_opaqueSubmeshes
				: m_transparentSubmeshes;
		}

		inline std::vector< CulledBillboard > const & getBillboards( bool opaque )const
		{
			return opaque
				? m_opaqueBillboards
				: m_transparentBillboards;
		}

	protected:
		void doClear();

	private:
		Scene const & m_scene;
		Camera * m_camera;

	protected:
		bool m_changed{ true };
		std::vector< CulledSubmesh > m_opaqueSubmeshes;
		std::vector< CulledSubmesh > m_transparentSubmeshes;
		std::vector< CulledBillboard > m_opaqueBillboards;
		std::vector< CulledBillboard > m_transparentBillboards;
	};
}

#endif
