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
#ifndef ___C3DAssimp___
#define ___C3DAssimp___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Castor3D/Prerequisites.hpp>

#include <Castor3D/Importer.hpp>
#include <Castor3D/Animation.hpp>
#include <CastorUtils/FileParser.hpp>

#include <Castor3D/RenderSystem.hpp>
#include <Castor3D/Buffer.hpp>
#include <Castor3D/SceneNode.hpp>
#include <Castor3D/Scene.hpp>
#include <Castor3D/Camera.hpp>
#include <Castor3D/Viewport.hpp>
#include <Castor3D/Material.hpp>
#include <Castor3D/Pass.hpp>
#include <Castor3D/TextureUnit.hpp>
#include <Castor3D/Version.hpp>
#include <Castor3D/Geometry.hpp>
#include <Castor3D/Mesh.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Submesh.hpp>
#include <Castor3D/Texture.hpp>
#include <Castor3D/Plugin.hpp>
#include <Castor3D/Face.hpp>
#include <Castor3D/Vertex.hpp>
#include <Castor3D/Skeleton.hpp>
#include <Castor3D/Parameter.hpp>
#include <Castor3D/Animation.hpp>
#include <Castor3D/KeyFrame.hpp>

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#pragma warning( pop )

#ifndef _WIN32
#	define C3D_Assimp_API
#else
#	ifdef ASSIMPImporter_EXPORTS
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
	class AssimpImporter : public Castor3D::Importer
	{
	private:
		int m_anonymous;
		Castor3D::MeshSPtr m_pMesh;
		std::map< Castor::String, uint32_t > m_mapBoneByID;
		std::vector< Castor3D::BoneSPtr > m_arrayBones;

	public:
		AssimpImporter( Castor3D::Engine * p_pEngine );
		~AssimpImporter();

	private:
		virtual Castor3D::SceneSPtr DoImportScene();
		virtual Castor3D::MeshSPtr DoImportMesh();

		bool DoProcessMesh( Castor3D::SkeletonSPtr p_pSkeleton, aiMesh const * p_pAiMesh, aiScene const * p_pAiScene, Castor3D::SubmeshSPtr p_pSubmesh );
		Castor3D::MaterialSPtr DoProcessMaterial( aiMaterial const * p_pAiMaterial );
		void DoProcessBones( Castor3D::SkeletonSPtr p_pSkeleton, aiBone ** p_pBones, uint32_t p_uiCount, std::vector< Castor3D::stVERTEX_BONE_DATA > & p_arrayVertices );
		Castor3D::AnimationSPtr DoProcessAnimation( Castor3D::SkeletonSPtr, aiNode * p_pNode, aiAnimation * p_pAnimation );
		void DoProcessAnimationNodes( Castor3D::AnimationSPtr p_pAnimation, Castor::real p_rTicksPerSecond, Castor3D::SkeletonSPtr, aiNode * p_pNode, aiAnimation * p_paiAnimation, Castor3D::MovingObjectBaseSPtr p_pObject );
		void DoAddTexture( Castor::String const & p_strValue, Castor3D::PassSPtr p_pPass, Castor3D::eTEXTURE_CHANNEL p_eChannel );
		void DoAddKeyframes( Castor3D::AnimationSPtr p_pAnimation, aiAnimation * p_paiAnimation );
	};
}

#endif
