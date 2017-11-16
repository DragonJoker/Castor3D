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
		void doAddBB( castor3d::MeshSPtr bbMesh
			, castor3d::SceneNodeSPtr parent );
		void doRemoveBB( castor3d::MeshSPtr bbMesh );
		void onNodeChanged( castor3d::SceneNode const & node );

	private:
		castor3d::Scene & m_scene;
		castor3d::MeshSPtr m_OBBMesh;
		castor3d::MeshSPtr m_AABBMesh;
		castor::String m_objectName;
		castor3d::MeshSPtr m_objectMesh;
		castor3d::OnSceneNodeChangedConnection m_connection;
	};
}

#endif
