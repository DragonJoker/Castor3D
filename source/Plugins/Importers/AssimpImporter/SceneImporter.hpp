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
		SceneImporter( AssimpImporter & importer
			, SkeletonImporter const & skeletons
			, castor3d::SceneNodePtrArray & nodes
			, castor3d::GeometryPtrStrMap & geometries );

		void import( aiScene const & aiScene
			, castor3d::Scene & scene
			, std::map< uint32_t, MeshData * > const & meshes );
		void import( aiScene const & aiScene
			, MeshIndices const & meshes );
		void importAnims();

	private:
		void doProcessLight( aiLight const & light
			, castor3d::Scene & scene );
		void doProcessSceneNodes( aiScene const & aiScene
			, aiNode const & aiNode
			, castor3d::Scene & scene
			, std::map< uint32_t, MeshData * > const & meshes
			, castor3d::SceneNodeSPtr targetParent
			, castor::Matrix4x4f transform );
		void doTransformMesh( aiNode const & aiNode
			, MeshIndices const & meshes
			, aiMatrix4x4 transformAcc = aiMatrix4x4{} );
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
		AssimpImporter & m_importer;
		SkeletonImporter const & m_skeletons;
		castor3d::SceneNodePtrArray & m_nodes;
		castor3d::SceneNodePtrStrMap m_sortedNodes;
		castor3d::GeometryPtrStrMap & m_geometries;
		using NodeAnimGroup = std::pair< castor3d::SceneNode const *, castor3d::AnimatedObjectGroupSPtr >;
		std::vector< NodeAnimGroup > m_animGroups;
	};
}

#endif
