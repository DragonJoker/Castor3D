#include "GltfImporter/GltfSkeletonImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>

namespace c3d_gltf
{
	namespace skeletons
	{
		static void processSkeletonNodeTransform( GltfImporterFile const & file
			, size_t nodeIndex
			, c3d::SkeletonNode & skelNode )
		{
			auto & node = file.getAsset().nodes[nodeIndex];
			auto transform = convert( node.transform );
			c3d::Matrix4x4f matrix;
			c3d::matrix::setTransform( matrix, transform.translate, transform.scale, transform.rotate );
			c3d::matrix::decompose( matrix, transform.translate, transform.scale, transform.rotate );
			skelNode.setTransform( { transform } );
			c3d::log::trace << "        Translation [" << skelNode.getTransform().translate << "]" << std::endl;
			c3d::log::trace << "        Rotation [" << skelNode.getTransform().rotate << "]" << std::endl;
			c3d::log::trace << "        Scale [" << skelNode.getTransform().scale << "]" << std::endl;
		}

		static c3d::SkeletonNode * processSkeletonJoint( GltfImporterFile const & file
			, c3d::Vector< c3d::Matrix4x4f > const & skinOffsetMatrices
			, c3d::Skeleton & skeleton
			, size_t nodeIndex
			, uint32_t jointIndex )
		{
			auto name = file.getNodeName( nodeIndex, 0u );
			auto skelNode = skeleton.findNode( name );

			if ( !skelNode )
			{
				c3d::log::debug << "    Skeleton Bone [" << name << "]" << std::endl;
				skelNode = skeleton.createBone( name, skinOffsetMatrices[jointIndex] );
				processSkeletonNodeTransform( file, nodeIndex, *skelNode );
			}

			return skelNode;
		}

		static c3d::SkeletonNode * processSkeletonNode( GltfImporterFile const & file
			, c3d::Skeleton & skeleton
			, size_t nodeIndex )
		{
			auto name = file.getNodeName( nodeIndex, 0u );
			auto skelNode = skeleton.findNode( name );

			if ( !skelNode )
			{
				c3d::log::debug << "    Skeleton Node [" << name << "]" << std::endl;
				skelNode = skeleton.createNode( name );
				processSkeletonNodeTransform( file, nodeIndex, *skelNode );
			}

			return skelNode;
		}

		static void processSkeletonNodes( GltfImporterFile const & file
			, c3d::Skeleton & skeleton
			, size_t parentIndex
			, c3d::SkeletonNode * parentSkelNode )
		{
			for ( auto nodeIndex : file.getAsset().nodes[parentIndex].children )
			{
				auto skelNode = processSkeletonNode( file
					, skeleton
					, nodeIndex );

				if ( parentSkelNode )
				{
					skeleton.setNodeParent( *skelNode, *parentSkelNode );
				}

				processSkeletonNodes( file
					, skeleton
					, nodeIndex
					, skelNode );
			}
		}

		template< typename FuncT >
		static void findSkinRootNodeInSkeletonNodes( GltfImporterFile const & file
			, c3d::Set< size_t > & currentNodes
			, c3d::Set< size_t > & parentNodes
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
			, c3d::Set< size_t > & currentNodes
			, c3d::Set< size_t > & parentNodes
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

		static c3d::Matrix4x4f getTransformMatrix( auto const & nodeTransform )
		{
			auto transform = convert( nodeTransform );
			c3d::Matrix4x4f result;
			c3d::matrix::setTransform( result, transform.translate, transform.scale, transform.rotate );
			return result;
		}
	}

	GltfSkeletonImporter::GltfSkeletonImporter( c3d::Engine & engine )
		: c3d::SkeletonImporter{ engine, cuT( "Gltf" ) }
	{
	}

	bool GltfSkeletonImporter::doImportSkeleton( c3d::Skeleton & skeleton )
	{
		auto & file = static_cast< GltfImporterFile & >( *m_file );
		auto & impAsset = file.getAsset();
		auto name = skeleton.getName();
		uint32_t skinIndex{};
		auto it = std::find_if( impAsset.skins.begin()
			, impAsset.skins.end()
			, [&file, &name, &skinIndex]( fastgltf::Skin const & lookup )
			{
				auto result = ( name == file.getSkinName( skinIndex ) );
				++skinIndex;
				return result;
			} );

		if ( it == impAsset.skins.end() )
		{
			return false;
		}

		--skinIndex;
		auto & impSkin = *it;
		c3d::Vector< c3d::Matrix4x4f > skinOffsetMatrices;

		if ( impSkin.inverseBindMatrices )
		{
			skinOffsetMatrices.reserve( impSkin.joints.size() );
			iterateAccessor< c3d::Matrix4x4f >( impAsset
				, impAsset.accessors[*impSkin.inverseBindMatrices]
				, [&skinOffsetMatrices]( c3d::Matrix4x4f value )
				{
					skinOffsetMatrices.push_back( c3d::move( value ) );
				}
				, file.getAdapter() );
		}
		else
		{
			skinOffsetMatrices.resize( impSkin.joints.size()
				, c3d::Matrix4x4f{ 1.0f } );
		}

		skeleton.setGlobalInverseTransform( impSkin.skeleton
			? skeletons::getTransformMatrix( impAsset.nodes[*impSkin.skeleton].transform )
			: c3d::Matrix4x4f{ 1.0f } );

		// First handle the bones listed in the skin
		// They must be processed in the same order as declared in the skin because
		// the mesh accesssor JOINTS_n references them using their index in the skin.
		uint32_t jointIndex{};
		for ( size_t nodeIndex : impSkin.joints )
		{
			skeletons::processSkeletonJoint( file
				, skinOffsetMatrices
				, skeleton
				, nodeIndex
				, jointIndex );
			++jointIndex;
		}

		// Then handle the hierarchy, filling the gaps with skeleton nodes.
		for ( size_t nodeIndex : findSkinRootNodes( file, impSkin ) )
		{
			skeletons::processSkeletonNodes( file
				, skeleton
				, nodeIndex
				, skeletons::processSkeletonNode( file
					, skeleton
					, nodeIndex ) );
		}

		return true;
	}

	c3d::Vector< size_t > findSkinRootNodes( GltfImporterFile const & file
		, fastgltf::Skin const & impSkin )
	{
		if ( impSkin.skeleton )
		{
			return { *impSkin.skeleton };
		}

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
		c3d::Set< size_t > currentNodes;

		for ( auto nodeIndex : impSkin.joints )
		{
			currentNodes.insert( nodeIndex );
		}

		c3d::Set< size_t > parentNodes;
		skeletons::findSkinRootNodeInSkeletonNodes( file, currentNodes, parentNodes, findParentNode );

		if ( parentNodes.empty() && currentNodes.size() > 1 )
		{
			// Skeleton doesn't have a single common node within its bones.
			// Recover common parent node to the remaining ones.
			skeletons::findSkinRootNodeInOtherNodes( file, currentNodes, parentNodes, findParentNode );
		}

		return ( !parentNodes.empty() )
			? c3d::Vector< size_t >{ parentNodes.begin(), parentNodes.end() }
		: c3d::Vector< size_t >{ currentNodes.begin(), currentNodes.end() };
	}
}
