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
		CubeBoxManager( castor3d::Scene & scene );
		~CubeBoxManager();
		void displayObject( castor3d::Geometry const & object );
		void hideObject( castor3d::Geometry const & object );

	private:
		castor3d::Scene & m_scene;
		castor3d::MeshSPtr m_mesh;
	};
}

#endif
