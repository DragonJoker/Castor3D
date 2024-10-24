#include "GltfImporter/GltfSkeletonImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>

namespace c3d_gltf
{
	namespace skeletons
	{
		static castor3d::SkeletonNode * addNode( GltfImporterFile const & file
			, castor3d::Skeleton & skeleton
			, fastgltf::pmr::MaybeSmallVector<std::size_t> const & skinJoints
			, castor::Vector< castor::Matrix4x4f > const & skinOffsetMatrices
			, castor::String const & name
			, size_t nodeIndex )
		{
			auto jit = std::find( skinJoints.begin(), skinJoints.end(), nodeIndex );

			if ( jit == skinJoints.end() )
			{
				castor3d::log::debug << "    Skeleton Node [" << name << "]" << std::endl;
				return skeleton.createNode( name );
			}

			castor3d::log::debug << "    Skeleton Bone [" << name << "]" << std::endl;
			auto mit = skinOffsetMatrices.begin() + std::distance( skinJoints.begin(), jit );
			return skeleton.createBone( name, *mit );
		}

		static castor3d::SkeletonNode * processSkeletonNode( GltfImporterFile const & file
			, fastgltf::pmr::MaybeSmallVector<std::size_t> const & skinJoints
			, castor::Vector< castor::Matrix4x4f > const & skinOffsetMatrices
			, castor3d::Skeleton & skeleton
			, size_t nodeIndex
			, castor3d::SkeletonNode * parentSkelNode )
		{
			auto name = file.getNodeName( nodeIndex, 0u );
			auto skelNode = skeleton.findNode( name );

			if ( !skelNode )
			{
				skelNode = addNode( file, skeleton, skinJoints, skinOffsetMatrices, name, nodeIndex );
				auto & node = file.getAsset().nodes[nodeIndex];
				auto transform = convert( node.transform );
				castor::Matrix4x4f matrix;
				castor::matrix::setTransform( matrix, transform.translate, transform.scale, transform.rotate );
				castor::matrix::decompose( matrix, transform.translate, transform.scale, transform.rotate );
				skelNode->setTransform( { transform } );
				castor3d::log::trace << "        Translation [" << skelNode->getTransform().translate << "]" << std::endl;
				castor3d::log::trace << "        Rotation [" << skelNode->getTransform().rotate << "]" << std::endl;
				castor3d::log::trace << "        Scale [" << skelNode->getTransform().scale << "]" << std::endl;

				if ( parentSkelNode )
				{
					skeleton.setNodeParent( *skelNode, *parentSkelNode );
				}
			}

			return skelNode;
		}

		static void processSkeletonNodes( GltfImporterFile const & file
			, fastgltf::pmr::MaybeSmallVector<std::size_t> const & skinJoints
			, castor::Vector< castor::Matrix4x4f > const & skinOffsetMatrices
			, castor3d::Skeleton & skeleton
			, size_t parentIndex
			, castor3d::SkeletonNode * parentSkelNode )
		{
			for ( auto nodeIndex : file.getAsset().nodes[parentIndex].children )
			{
				processSkeletonNodes( file
					, skinJoints
					, skinOffsetMatrices
					, skeleton
					, nodeIndex
					, processSkeletonNode( file
						, skinJoints
						, skinOffsetMatrices
						, skeleton
						, nodeIndex
						, parentSkelNode ) );
			}
		}

		template< typename FuncT >
		static void findSkinRootNodeInSkeletonNodes( GltfImporterFile const & file
			, castor::Set< size_t > & currentNodes
			, castor::Set< size_t > & parentNodes
			, FuncT findParentNode )
		{
			do
			{
				if ( !parentNodes.empty() )
				{
					currentNodes = parentNodes;
					parentNodes.clear();
				}

				for ( auto nodeIndex : currentNodes )
				{
					auto parentIndex = findParentNode( nodeIndex );

					if ( parentIndex != size_t{ ~0u }
						&& file.isSkeletonNode( parentIndex ) )
					{
						parentNodes.insert( parentIndex );
					}
				}
			}
			while ( parentNodes.size() > 1 && currentNodes != parentNodes );
		}

		template< typename FuncT >
		static void findSkinRootNodeInOtherNodes( GltfImporterFile const & file
			, castor::Set< size_t > & currentNodes
			, castor::Set< size_t > & parentNodes
			, FuncT findParentNode )
		{
			do
			{
				if ( !parentNodes.empty() )
				{
					currentNodes = parentNodes;
					parentNodes.clear();
				}

				for ( auto nodeIndex : currentNodes )
				{
					auto parentIndex = findParentNode( nodeIndex );

					if ( parentIndex != size_t{ ~0u } )
					{
						parentNodes.insert( parentIndex );
					}
				}
			}
			while ( parentNodes.size() > 1 && currentNodes != parentNodes );
		}

		static size_t findSkinRootNode( GltfImporterFile const & file
			, fastgltf::Skin const & impSkin )
		{
			fastgltf::Asset const & impAsset = file.getAsset();

			auto findParentNode = [&impAsset]( size_t nodeIndex )
			{
				auto pit = std::find_if( impAsset.nodes.begin()
					, impAsset.nodes.end()
					, [&nodeIndex]( fastgltf::Node const & lookup )
					{
						auto cit = std::find( lookup.children.begin()
							, lookup.children.end()
							, nodeIndex );
						return cit != lookup.children.end();
					} );

				if ( pit != impAsset.nodes.end() )
				{
					return size_t( std::distance( impAsset.nodes.begin(), pit ) );
				}

				return size_t{ ~0u };
			};
			castor::Set< size_t > currentNodes;

			for ( auto nodeIndex : impSkin.joints )
			{
				currentNodes.insert( nodeIndex );
			}

			castor::Set< size_t > parentNodes;
			findSkinRootNodeInSkeletonNodes( file, currentNodes, parentNodes, findParentNode );

			if ( parentNodes.size() > 1 )
			{
				// Skeleton doesn't have a single common node within its bones.
				// Recover common parent node to the remaining ones.
				if ( impSkin.skeleton )
				{
					return *impSkin.skeleton;
				}

				findSkinRootNodeInOtherNodes( file, currentNodes, parentNodes, findParentNode );
			}

			return parentNodes.size() == 1u
				? *parentNodes.begin()
				: *currentNodes.begin();
		}

		static castor::Matrix4x4f getTransformMatrix( auto const & nodeTransform )
		{
			auto transform = convert( nodeTransform );
			castor::Matrix4x4f result;
			castor::matrix::setTransform( result, transform.translate, transform.scale, transform.rotate );
			return result;
		}
	}

	GltfSkeletonImporter::GltfSkeletonImporter( castor3d::Engine & engine )
		: castor3d::SkeletonImporter{ engine, cuT( "Gltf" ) }
	{
	}

	bool GltfSkeletonImporter::doImportSkeleton( castor3d::Skeleton & skeleton )
	{
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto & impAsset = file.getAsset();
		auto name = skeleton.getName();
		uint32_t index{};
		auto it = std::find_if( impAsset.skins.begin()
			, impAsset.skins.end()
			, [&file, &name, &index]( fastgltf::Skin const & lookup )
			{
				return name == file.getSkinName( index++ );
			} );

		if ( it == impAsset.skins.end() )
		{
			return false;
		}

		auto & impSkin = *it;
		castor::Vector< castor::Matrix4x4f > skinOffsetMatrices;

		if ( impSkin.inverseBindMatrices )
		{
			skinOffsetMatrices.reserve( impSkin.joints.size() );
			iterateAccessor< castor::Matrix4x4f >( impAsset
				, impAsset.accessors[*impSkin.inverseBindMatrices]
				, [&skinOffsetMatrices]( castor::Matrix4x4f value )
				{
					skinOffsetMatrices.push_back( castor::move( value ) );
				}
				, file.getAdapter() );
		}
		else
		{
			skinOffsetMatrices.resize( impSkin.joints.size()
				, castor::Matrix4x4f{ 1.0f } );
		}

		auto skinRootNode = skeletons::findSkinRootNode( file, impSkin );

		skeleton.setGlobalInverseTransform( impSkin.skeleton
			? skeletons::getTransformMatrix( impAsset.nodes[*impSkin.skeleton].transform )
			: castor::Matrix4x4f{ 1.0f } );
		skeletons::processSkeletonNodes( file
			, impSkin.joints
			, skinOffsetMatrices
			, skeleton
			, skinRootNode
			, skeletons::processSkeletonNode( file
				, impSkin.joints
				, skinOffsetMatrices
				, skeleton
				, skinRootNode
				, nullptr ) );

		return true;
	}
}
