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
		: public castor3d::Importer
	{
	public:
		AssimpImporter( castor3d::Engine & engine );
		~AssimpImporter();

		static castor3d::ImporterUPtr create( castor3d::Engine & engine );

	private:
		/**
		 *\copydoc		castor3d::Importer::doImportScene
		 */
		bool doImportScene( castor3d::Scene & p_scene )override;
		/**
		 *\copydoc		castor3d::Importer::doImportMesh
		 */
		bool doImportMesh( castor3d::Mesh & p_mesh )override;

		bool doProcessMesh( castor3d::Scene & p_scene
			, castor3d::Mesh & p_mesh
			, castor3d::Skeleton & p_skeleton
			, aiMesh const & p_aiMesh
			, aiScene const & p_aiScene
			, castor3d::Submesh & p_submesh );
		castor3d::MaterialSPtr doProcessMaterial( castor3d::Scene & p_scene
			, aiMaterial const & p_aiMaterial );
		castor3d::BoneSPtr doAddBone( castor::String const & p_name
			, castor::Matrix4x4r const & p_offset
			, castor3d::Skeleton & p_skeleton
			, uint32_t & p_index );
		void doProcessBones( castor3d::Skeleton & p_pSkeleton
			, aiBone const * const * p_aiBones
			, uint32_t p_count
			, std::vector< castor3d::VertexBoneData > & p_arrayVertices );
		void doProcessAnimation( castor3d::Mesh & p_mesh
			, castor::String const & p_name
			, castor3d::Skeleton & p_skeleton
			, aiNode const & p_aiNode
			, aiAnimation const & p_aiAnimation );
		void doProcessAnimationNodes( castor3d::Mesh & p_mesh
			, castor3d::SkeletonAnimation & p_animation
			, int64_t p_ticksPerMilliSecond
			, castor3d::Skeleton & p_skeleton
			, aiNode const & p_aiNode
			, aiAnimation const & p_aiAnimation
			, castor3d::SkeletonAnimationObjectSPtr p_object );
		void doProcessAnimationNodeKeys( aiNodeAnim const & p_aiNodeAnim
			, int64_t p_ticksPerMilliSecond
			, castor3d::SkeletonAnimationObject & p_object );
		void doProcessAnimationMeshes( castor3d::Mesh & p_mesh
			, castor3d::Submesh & p_submesh
			, aiMesh const & p_aiMesh
			, aiMeshAnim const & p_aiMeshAnim );

	public:
		static castor::String const Name;

	private:
		int m_anonymous;
		std::map< castor::String, uint32_t > m_mapBoneByID;
		std::vector< castor3d::BoneSPtr > m_arrayBones;
	};
}

#endif
