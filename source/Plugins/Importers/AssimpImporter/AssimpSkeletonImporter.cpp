#include "AssimpImporter/AssimpSkeletonImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>

namespace c3d_assimp
{
	namespace skeletons
	{
		static castor3d::SkeletonNode * addNode( castor3d::Skeleton & skeleton
			, std::map< castor::String, castor::Matrix4x4f > const & bonesNodes
			, castor::String const & nodeName
			, castor::String const & name )
		{
			auto it = bonesNodes.find( nodeName );

			if ( it == bonesNodes.end() )
			{
				castor3d::log::debug << "    Skeleton Node [" << nodeName << "]" << std::endl;
				return skeleton.createNode( name );
			}

			castor3d::log::debug << "    Skeleton Bone [" << nodeName << "]" << std::endl;
			return skeleton.createBone( name, it->second );
		}

		static castor3d::SkeletonNode * processSkeletonNode( AssimpImporterFile const & file
			, std::map< castor::String, castor::Matrix4x4f > const & bonesNodes
			, castor3d::Skeleton & skeleton
			, aiNode const & aiNode
			, castor3d::SkeletonNode * parentSkelNode )
		{
			auto nodeName = makeString( aiNode.mName );
			auto name = file.getInternalName( nodeName );
			auto skelNode = skeleton.findNode( name );

			if ( !skelNode )
			{
				aiVector3D scaling, position;
				aiQuaternion rotate;
				aiNode.mTransformation.Decompose( scaling, rotate, position );
				auto translate = fromAssimp( position );
				auto scale = fromAssimp( scaling );
				auto orientation = fromAssimp( rotate );
				skelNode = addNode( skeleton, bonesNodes, nodeName, name );

				if ( parentSkelNode )
				{
					skeleton.setNodeParent( *skelNode, *parentSkelNode );
				}
			}

			return skelNode;
		}

		static void processSkeletonNodes( AssimpImporterFile const & file
			, std::map< castor::String, castor::Matrix4x4f > const & bonesNodes
			, castor3d::Skeleton & skeleton
			, aiNode const & parentAiNode
			, castor3d::SkeletonNode * parentSkelNode )
		{
			for ( auto node : castor::makeArrayView( parentAiNode.mChildren, parentAiNode.mNumChildren ) )
			{
				processSkeletonNodes( file
					, bonesNodes
					, skeleton
					, *node
					, processSkeletonNode( file
						, bonesNodes
						, skeleton
						, *node
						, parentSkelNode ) );
			}
		}
	}

	AssimpSkeletonImporter::AssimpSkeletonImporter( castor3d::Engine & engine )
		: castor3d::SkeletonImporter{ engine }
	{
	}

	bool AssimpSkeletonImporter::doImportSkeleton( castor3d::Skeleton & skeleton )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = skeleton.getName();

		if ( file.getListedMeshes().empty()
			&& !file.getSkeletons().empty() )
		{
			name = file.getSkeletons().begin()->first;
		}

		auto it = file.getSkeletons().find( name );

		if ( it == file.getSkeletons().end() )
		{
			return false;
		}

		auto & skelData = it->second;
		skeleton.setGlobalInverseTransform( fromAssimp( skelData.rootNode->mTransformation ).getInverse() );
		skeletons::processSkeletonNodes( file
			, file.getBonesNodes()
			, skeleton
			, *skelData.rootNode
			, skeletons::processSkeletonNode( file
				, file.getBonesNodes()
				, skeleton
				, *skelData.rootNode
				, nullptr ) );
		return true;
	}
}
