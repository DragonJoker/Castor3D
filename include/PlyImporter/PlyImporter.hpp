/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_PlyImporter___
#define ___C3D_PlyImporter___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Castor3D/Prerequisites.hpp>

#include <Castor3D/RenderSystem.hpp>
#include <Castor3D/Buffer.hpp>
#include <Castor3D/SceneNode.hpp>
#include <Castor3D/Scene.hpp>
#include <Castor3D/Camera.hpp>
#include <Castor3D/Viewport.hpp>
#include <Castor3D/Material.hpp>
#include <Castor3D/Pass.hpp>
#include <Castor3D/TextureUnit.hpp>
#include <Castor3D/Geometry.hpp>
#include <Castor3D/Mesh.hpp>
#include <Castor3D/Submesh.hpp>
#include <Castor3D/Face.hpp>
#include <Castor3D/Version.hpp>
#include <Castor3D/Plugin.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Texture.hpp>
#include <Castor3D/Vertex.hpp>

#include <Castor3D/Importer.hpp>

#pragma warning( pop )

#ifndef _WIN32
#	define C3D_Ply_API
#else
#	ifdef PlyImporter_EXPORTS
#		define C3D_Ply_API __declspec(dllexport)
#	else
#		define C3D_Ply_API __declspec(dllimport)
#	endif
#endif

namespace C3dPly
{
	//! PLY file importer
	/*!
	Imports data from PLY files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class PlyImporter : public Castor3D::Importer
	{
	private:
		friend class Scene;

	public:
		/**
		 * Constructor
		 */
		PlyImporter( Castor3D::Engine * p_pEngine );

	private:
		virtual Castor3D::SceneSPtr DoImportScene();
		virtual Castor3D::MeshSPtr DoImportMesh();
	};
}

#endif
