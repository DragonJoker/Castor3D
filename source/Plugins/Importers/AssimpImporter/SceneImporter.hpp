/*
See LICENSE file in root folder
*/
#ifndef ___C3DAssimp_SceneImporter___
#define ___C3DAssimp_SceneImporter___

#include "MeshImporter.hpp"

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/scene.h> // Output data structure
#pragma warning( pop )

namespace c3d_assimp
{
	class SceneImporter
	{
	public:
		SceneImporter( castor3d::MeshImporter & importer
			, SkeletonImporter const & skeletons
			, MeshesImporter const & meshes
			, castor3d::SceneNodePtrArray & nodes
			, castor3d::GeometryPtrStrMap & geometries );

		void import( castor::String const & prefix
			, aiScene const & aiScene
			, castor3d::Scene & scene
			, std::map< uint32_t, MeshData * > const & meshes );
		void import( castor::String const & prefix
			, aiScene const & aiScene
			, MeshIndices const & meshes );
		void importAnims();

	private:
		void doProcessLight( aiLight const & light
			, castor3d::Scene & scene );
		void doProcessSceneNodes( aiScene const & aiScene
			, aiNode const & aiNode
			, castor3d::Scene & scene
			, castor3d::SceneNodeSPtr targetParent
			, castor::Matrix4x4f accTransform );
		void doScaleMesh( aiNode const & aiNode
			, MeshIndices const & meshes );
		void doProcessNodes( aiScene const & aiScene
			, aiNode const & aiNode
			, castor3d::Scene & scene
			, std::map< uint32_t, MeshData * > const & meshes );
		void doProcessAnimationSceneNodes( castor3d::SceneNode & node
			, aiAnimation const & aiAnimation
			, aiNode const & aiNode
			, aiNodeAnim const & aiNodeAnim );
		void doProcessAnimGroups();
		void doAddAnimatedObjectGroup( castor3d::Scene & scene
			, castor3d::SceneNode const & sceneNode
			, castor3d::Skeleton & skeleton
			, castor3d::Mesh & mesh
			, castor3d::Geometry & geometry );

	public:
		static castor::String const Name;

	private:
		castor3d::MeshImporter & m_importer;
		SkeletonImporter const & m_skeletons;
		MeshesImporter const & m_meshes;
		castor3d::SceneNodePtrArray & m_nodes;
		castor3d::GeometryPtrStrMap & m_geometries;
		castor::String m_prefix;
		using NodeAnimGroup = std::pair< castor3d::SceneNode const *, castor3d::AnimatedObjectGroupSPtr >;
		std::vector< NodeAnimGroup > m_animGroups;
	};
}

#endif
