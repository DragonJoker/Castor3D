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
			, std::vector< castor::Matrix4x4f > const & skinOffsetMatrices
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
			, std::vector< castor::Matrix4x4f > const & skinOffsetMatrices
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

				if ( parentSkelNode )
				{
					skeleton.setNodeParent( *skelNode, *parentSkelNode );
				}
			}

			return skelNode;
		}

		static void processSkeletonNodes( GltfImporterFile const & file
			, fastgltf::pmr::MaybeSmallVector<std::size_t> const & skinJoints
			, std::vector< castor::Matrix4x4f > const & skinOffsetMatrices
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

		size_t findSkinRootNode( fastgltf::Asset const & impAsset
			, fastgltf::Skin const & impSkin )
		{
			if ( impSkin.skeleton )
			{
				return *impSkin.skeleton;
			}

			// Not found in skin nodes, lookup in parent nodes
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
			std::vector< size_t > parentNodes;
			std::vector< size_t > currentNodes;
			currentNodes.insert( currentNodes.end(), impSkin.joints.begin(), impSkin.joints.end() );

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
						parentNodes.push_back( parentIndex );
					}
				}
			}
			while ( parentNodes.size() > 1 );

			return parentNodes.size() == 1u
				? parentNodes.front()
				: currentNodes.front();
		}
	}

	GltfSkeletonImporter::GltfSkeletonImporter( castor3d::Engine & engine )
		: castor3d::SkeletonImporter{ engine }
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

		castor3d::log::info << cuT( "  Skeleton found: [" ) << name << cuT( "]" ) << std::endl;
		auto & impSkin = *it;
		std::vector< castor::Matrix4x4f > skinOffsetMatrices;

		if ( impSkin.inverseBindMatrices )
		{
			skinOffsetMatrices.reserve( impSkin.joints.size() );
			fastgltf::iterateAccessor< castor::Matrix4x4f >( impAsset
				, impAsset.accessors[*impSkin.inverseBindMatrices]
				, [&skinOffsetMatrices]( castor::Matrix4x4f value )
				{
					skinOffsetMatrices.push_back( std::move( value ) );
				} );
		}
		else
		{
			skinOffsetMatrices.resize( impSkin.joints.size()
				, castor::Matrix4x4f{ 1.0f } );
		}

		auto skinRootNode = skeletons::findSkinRootNode( impAsset, impSkin );
		auto jit = std::find( impSkin.joints.begin(), impSkin.joints.end(), skinRootNode );
		auto mit = skinOffsetMatrices.begin() + std::distance( impSkin.joints.begin(), jit );

		skeleton.setGlobalInverseTransform( jit == impSkin.joints.end() ? castor::Matrix4x4f{ 1.0f } : *mit );
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
