/*
See LICENSE file in root folder
*/
#ifndef ___GC_CubeBoxManager_HPP___
#define ___GC_CubeBoxManager_HPP___
#pragma once

#include <Castor3D/Castor3DPrerequisites.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

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
		void displayObject( castor3d::Geometry const & object
			, castor3d::Submesh const & submesh );
		void hideObject( castor3d::Geometry const & object );

	private:
		castor3d::GeometryRPtr doAddBB( castor3d::MeshResPtr bbMesh
			, castor3d::MaterialObs material
			, castor::String const & name
			, castor3d::SceneNode & parent
			, castor::BoundingBox const & bb
			, bool handleSkeleton = true );
		void doRemoveBB( castor3d::GeometryRPtr bbObject );
		void onSceneUpdate( castor3d::Scene const & scene );

	private:
		castor3d::Scene & m_scene;
		castor3d::MaterialObs m_obbMeshMaterial{};
		castor3d::MaterialObs m_obbSubmeshMaterial{};
		castor3d::MaterialObs m_obbSelectedSubmeshMaterial{};
		castor3d::MaterialObs m_obbBoneMaterial{};
		castor3d::MaterialObs m_aabbMeshMaterial{};
		castor3d::MeshResPtr m_obbMesh{};
		castor3d::MeshResPtr m_obbSubmesh{};
		castor3d::MeshResPtr m_obbSelectedSubmesh{};
		castor3d::MeshResPtr m_obbBone{};
		castor3d::MeshResPtr m_aabbMesh{};
		castor3d::Geometry const * m_object{};
		castor3d::Submesh const * m_submesh{};
		castor3d::GeometryRPtr m_obbObject{};
		std::vector< castor3d::GeometryRPtr > m_obbSubmeshObjects;
		castor3d::GeometryRPtr m_obbSelectedSubmeshObject{};
		std::vector< castor3d::GeometryRPtr > m_obbBoneObjects;
		castor3d::GeometryRPtr m_aabbObject{};
		castor3d::OnSceneUpdateConnection m_sceneConnection;
	};
}

#endif
