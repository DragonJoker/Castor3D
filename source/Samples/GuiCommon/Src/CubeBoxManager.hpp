/*
See LICENSE file in root folder
*/
#ifndef ___GC_CubeBoxManager_HPP___
#define ___GC_CubeBoxManager_HPP___
#pragma once

#include <Castor3DPrerequisites.hpp>
#include <Math/Angle.hpp>
#include <Math/Point.hpp>
#include <Math/Quaternion.hpp>
#include <Math/RangedValue.hpp>

namespace GuiCommon
{
	/**
	*\brief
	*	Used to display the cube box of objects.
	*/
	class CubeBoxManager
	{
	public:
		explicit CubeBoxManager( castor3d::Scene & scene );
		~CubeBoxManager();
		void displayObject( castor3d::Geometry const & object );
		void hideObject( castor3d::Geometry const & object );

	private:
		castor3d::SceneNodeSPtr doAddBB( castor3d::MeshSPtr bbMesh
			, castor::String const & name
			, castor3d::SceneNodeSPtr parent
			, castor::BoundingBox const & bb );
		void doRemoveBB( castor::String const & name
			, castor3d::SceneNodeSPtr bbNode );
		void onSceneUpdate( castor3d::Scene const & scene );

	private:
		castor3d::Scene & m_scene;
		castor3d::MeshSPtr m_obbMesh;
		castor3d::MeshSPtr m_obbSubmesh;
		castor3d::MeshSPtr m_aabbMesh;
		castor::String m_objectName;
		castor3d::MeshSPtr m_objectMesh;
		castor3d::SceneNodeSPtr m_obbNode;
		castor3d::SceneNodePtrArray m_obbSubmeshNodes;
		castor3d::SceneNodeSPtr m_aabbNode;
		castor3d::OnSceneUpdateConnection m_sceneConnection;
	};
}

#endif
