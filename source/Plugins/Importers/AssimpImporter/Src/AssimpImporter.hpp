/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3DAssimp___
#define ___C3DAssimp___

#include <Castor3DPrerequisites.hpp>

#include <FileParser/FileParser.hpp>

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

		static Castor3D::ImporterUPtr Create( Castor3D::Engine & p_engine );

	private:
		/**
		 *\copydoc		Castor3D::Importer::DoImportScene
		 */
		bool DoImportScene( Castor3D::Scene & p_scene )override;
		/**
		 *\copydoc		Castor3D::Importer::DoImportMesh
		 */
		bool DoImportMesh( Castor3D::Mesh & p_mesh )override;

		bool DoProcessMesh( Castor3D::Scene & p_scene, Castor3D::Mesh & p_mesh, Castor3D::Skeleton & p_skeleton, aiMesh const & p_aiMesh, aiScene const & p_aiScene, Castor3D::Submesh & p_submesh );
		Castor3D::MaterialSPtr DoProcessMaterial( Castor3D::Scene & p_scene, aiMaterial const & p_aiMaterial );
		void DoProcessBones( Castor3D::Skeleton & p_pSkeleton, aiBone const * const * p_aiBones, uint32_t p_count, std::vector< Castor3D::VertexBoneData > & p_arrayVertices );
		void DoProcessAnimation( Castor::String const & p_name, Castor3D::Skeleton & p_skeleton, aiNode const & p_aiNode, aiAnimation const & p_aiAnimation );
		void DoProcessAnimationNodes( Castor3D::SkeletonAnimation & p_animation, int64_t p_ticksPerMilliSecond, Castor3D::Skeleton & p_skeleton, aiNode const & p_aiNode, aiAnimation const & p_aiAnimation, Castor3D::SkeletonAnimationObjectSPtr p_object );
		void DoProcessAnimationMeshes( Castor3D::Mesh & p_mesh, Castor3D::Submesh & p_submesh, aiMesh const & p_aiMesh, aiMeshAnim const & p_aiMeshAnim );

	public:
		static Castor::String const Name;

	private:
		int m_anonymous;
		std::map< Castor::String, uint32_t > m_mapBoneByID;
		std::vector< Castor3D::BoneSPtr > m_arrayBones;
	};
}

#endif
