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
#ifndef ___C3D_FBX_IMPORTER_H___
#define ___C3D_FBX_IMPORTER_H___

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

#include <fbxsdk.h>

namespace C3dFbx
{
	/*!
	\author		Sylvain DOREMUS
	\date		02/02/2016
	\~english
	\brief		FBX file importer.
	\~french
	\brief		Importeur de fichiers FBX.
	*/
	class FbxSdkImporter
		: public castor3d::Importer
	{
	public:
		FbxSdkImporter( castor3d::Engine & p_engine );
		~FbxSdkImporter();

		static castor3d::ImporterUPtr create( castor3d::Engine & p_engine );

	private:
		/**
		 *\copydoc		castor3d::Importer::doImportScene
		 */
		bool doImportScene( castor3d::Scene & p_scene )override;
		/**
		 *\copydoc		castor3d::Importer::doImportMesh
		 */
		bool doImportMesh( castor3d::Mesh & p_mesh )override;

		void doLoadMeshes( castor3d::Mesh & p_mesh
			, castor3d::Scene & p_scene
			, FbxNode * p_fbxNode );
		void doLoadSkeleton( castor3d::Mesh & p_mesh
			, FbxNode * p_fbxScene );
		void doLoadMaterials( castor3d::Scene & p_scene
			, FbxScene * p_fbxScene );
		void doLoadAnimations( castor3d::Mesh & p_mesh
			, FbxScene * p_fbxScene );
		void doProcessBones( FbxNode * p_fbxNode
			, castor3d::Skeleton & p_skeleton
			, castor3d::BoneSPtr p_parent );
		void doProcessBonesWeights( FbxNode * p_fbxNode
			, castor3d::Skeleton const & p_skeleton
			, std::vector< castor3d::VertexBoneData > & p_boneData );
		void doProcessSkeletonAnimations( FbxScene * p_fbxScene
			, FbxNode * p_fbxNode
			, FbxSkin * p_fbxSkin
			, castor3d::Skeleton & p_skeleton );
		castor3d::SubmeshSPtr doProcessMesh( castor3d::Mesh & p_mesh
			, FbxMesh * p_fbxMesh
			, castor::Matrix4x4r const & p_transform );
		castor3d::TextureUnitSPtr doLoadTexture( FbxFileTexture * p_fbxTexture
			, castor3d::Pass & p_pass
			, castor3d::TextureChannel p_channel );
		castor3d::TextureUnitSPtr doLoadTexture( FbxLayeredTexture * p_fbxTexture
			, castor3d::Pass & p_pass
			, castor3d::TextureChannel p_channel );
		castor3d::TextureUnitSPtr doLoadTexture( FbxProceduralTexture * p_fbxTexture
			, castor3d::Pass & p_pass
			, castor3d::TextureChannel p_channel );
		castor3d::TextureUnitSPtr doLoadTexture( castor3d::Scene & p_scene
			, FbxPropertyT< FbxDouble3 > const & p_property
			, castor3d::Pass & p_pass
			, castor3d::TextureChannel p_channel );

	private:
		int m_anonymous;
		std::map< castor::String, uint32_t > m_mapBoneByID;
		std::vector< castor3d::BoneSPtr > m_arrayBones;
	};
}

#endif
