/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3DAssimp___
#define ___C3DAssimp___

#include <Castor3DPrerequisites.hpp>

#include <FileParser.hpp>

#include <Engine.hpp>

#include <Animation/Animation.hpp>
#include <Animation/KeyFrame.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Importer.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/Buffer.hpp>
#include <Mesh/Skeleton/Skeleton.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Miscellaneous/Version.hpp>
#include <Plugin/Plugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/SceneNode.hpp>
#include <Scene/Scene.hpp>
#include <Texture/TextureUnit.hpp>
#include <Texture/TextureLayout.hpp>

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#ifndef _WIN32
#	define C3D_Assimp_API
#else
#	ifdef AssimpImporter_EXPORTS
#		define C3D_Assimp_API __declspec(dllexport)
#	else
#		define C3D_Assimp_API __declspec(dllimport)
#	endif
#endif

namespace C3dAssimp
{
	/*!
	\author		Sylvain DOREMUS
	\date		25/08/2010
	\~english
	\brief		ASE file importer
	\~french
	\brief		Importeur de fichiers ASE
	*/
	class AssimpImporter
		: public Castor3D::Importer
	{
	public:
		AssimpImporter( Castor3D::Engine & p_engine );
		~AssimpImporter();

	private:
		virtual Castor3D::SceneSPtr DoImportScene();
		virtual Castor3D::MeshSPtr DoImportMesh( Castor3D::Scene & p_scene );

		bool DoProcessMesh( Castor3D::Scene & p_scene, Castor3D::SkeletonSPtr p_pSkeleton, aiMesh const * p_pAiMesh, aiScene const * p_pAiScene, Castor3D::SubmeshSPtr p_pSubmesh );
		void DoLoadTexture( aiString const & p_name, Castor3D::Pass & p_pass, Castor3D::TextureChannel p_channel );
		Castor3D::MaterialSPtr DoProcessMaterial( Castor3D::Scene & p_scene, aiMaterial const * p_pAiMaterial );
		void DoProcessBones( Castor3D::SkeletonSPtr p_pSkeleton, aiBone ** p_pBones, uint32_t p_count, std::vector< Castor3D::stVERTEX_BONE_DATA > & p_arrayVertices );
		Castor3D::SkeletonAnimationSPtr DoProcessAnimation( Castor::String const & p_name, Castor3D::SkeletonSPtr, aiNode * p_node, aiAnimation * p_pAnimation );
		void DoProcessAnimationNodes( Castor3D::SkeletonAnimationSPtr p_pAnimation, Castor::real p_rTicksPerSecond, Castor3D::SkeletonSPtr, aiNode * p_node, aiAnimation * p_paiAnimation, Castor3D::SkeletonAnimationObjectSPtr p_object );

	private:
		int m_anonymous;
		Castor3D::MeshSPtr m_mesh;
		std::map< Castor::String, uint32_t > m_mapBoneByID;
		std::vector< Castor3D::BoneSPtr > m_arrayBones;
	};
}

#endif
