#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#pragma GCC diagnostic ignored "-Werror=overloaded-virtual"

#include "AssimpImporter/SkeletonImporter.hpp"
#include "AssimpImporter/AssimpHelpers.hpp"
#include "AssimpImporter/AssimpImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Interpolator.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <CastorUtils/Data/Path.hpp>
#include <CastorUtils/Design/ArrayView.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/Importer.hpp> // Importer interface
#pragma warning( pop )

namespace c3d_assimp
{
	//*********************************************************************************************

	namespace skeletons
	{
		static aiNode const * findRootSkeletonNode( aiNode const & sceneRootNode
			, castor::ArrayView< aiBone * > bones
			, uint32_t aiMeshIndex
			, std::map< uint32_t, aiNode * > meshesNodes
			, std::map< castor::String, BoneData > const & bonesNodes )
		{
			auto meshNode = meshesNodes.find( aiMeshIndex )->second;
			std::vector< aiNode const * > bonesRootNodes;
			auto insertNode = [&bonesRootNodes]( aiNode const * node )
			{
				if ( std::all_of( bonesRootNodes.begin()
					, bonesRootNodes.end()
					, [node]( aiNode const * lookup )
					{
						return lookup->FindNode( node->mName ) == nullptr;
					} ) )
				{
					std::erase_if( bonesRootNodes
						, [node]( aiNode const * lookup )
						{
							return node->FindNode( lookup->mName ) != nullptr;
						} );
					bonesRootNodes.push_back( node );
				}
			};

			for ( auto bone : bones )
			{
				auto node = sceneRootNode.FindNode( bone->mName );
				insertNode( node );

				while ( node->mParent )
				{
					node = node->mParent;

					if ( node == meshNode
						|| node->FindNode( meshNode->mName ) )
					{
						break;
					}

					insertNode( node );
				}
			}

			CU_Require( bonesRootNodes.size() == 1u );
			return *bonesRootNodes.begin();
		}

		static castor3d::SkeletonNode * addNode( castor3d::Skeleton & skeleton
			, std::map< castor::String, BoneData > const & bonesNodes
			, castor::String const & nodeName
			, castor::String const & name )
		{
			auto it = bonesNodes.find( nodeName );

			if ( it == bonesNodes.end() )
			{
				castor3d::log::debug << "  Skeleton Node [" << nodeName << "]" << std::endl;
				return skeleton.createNode( name );
			}

			castor3d::log::debug << "  Skeleton Bone [" << nodeName << "]" << std::endl;
			return skeleton.createBone( name, it->second.inverseTransform );
		}

		static castor3d::SkeletonNode * processSkeletonNode( AssimpImporter const & importer
			, std::map< castor::String, BoneData > const & bonesNodes
			, castor3d::Skeleton & skeleton
			, aiNode const & aiNode
			, castor3d::SkeletonNode * parentSkelNode )
		{
			auto nodeName = makeString( aiNode.mName );
			auto name = importer.getInternalName( nodeName );
			auto skelNode = skeleton.findNode( name );

			if ( !skelNode )
			{
				skelNode = addNode( skeleton, bonesNodes, nodeName, name );

				if ( parentSkelNode )
				{
					skeleton.setNodeParent( *skelNode, *parentSkelNode );
				}
			}

			return skelNode;
		}

		static void processSkeletonNodes( AssimpImporter const & importer
			, std::map< castor::String, BoneData > const & bonesNodes
			, castor3d::Skeleton & skeleton
			, aiNode const & parentAiNode
			, castor3d::SkeletonNode * parentSkelNode )
		{
			for ( auto node : castor::makeArrayView( parentAiNode.mChildren, parentAiNode.mNumChildren ) )
			{
				processSkeletonNodes( importer
					, bonesNodes
					, skeleton
					, *node
					, processSkeletonNode( importer
						, bonesNodes
						, skeleton
						, *node
						, parentSkelNode ) );
			}
		}

		static void completeSkeleton( AssimpImporter const & importer
			, std::map< castor::String, BoneData > const & bonesNodes
			, castor3d::Skeleton & skeleton
			, aiNode const & preRootNode
			, aiNode const * previousPreRootNode )
		{
			auto rootNode = skeleton.getRootNode();

			while ( previousPreRootNode != &preRootNode )
			{
				auto nodeName = makeString( previousPreRootNode->mName );
				auto name = importer.getInternalName( nodeName );
				auto node = addNode( skeleton, bonesNodes, nodeName, name );
				skeleton.setNodeParent( *rootNode , *node );
				rootNode = node;
				previousPreRootNode = previousPreRootNode->mParent;
			}

			skeleton.setGlobalInverseTransform( makeMatrix4x4f( preRootNode.mChildren[0]->mTransformation ).getInverse() );
			processSkeletonNodes( importer, bonesNodes, skeleton, *previousPreRootNode, rootNode );
		}

		static void replaceInverseTransforms( AssimpImporter const & importer
			, std::map< castor::String, BoneData > const & bonesNodes
			, castor3d::Skeleton & skeleton )
		{
			for ( auto & node : skeleton.getNodes() )
			{
				if ( node->getType() == castor3d::SkeletonNodeType::eBone )
				{
					auto & bone = static_cast< castor3d::BoneNode & >( *node );
					auto it = bonesNodes.find( importer.getExternalName( node->getName() ) );

					if ( it != bonesNodes.end() )
					{
						bone.setInverseTransform( it->second.inverseTransform );
					}
					else
					{
						bone.setInverseTransform( castor::Matrix4x4f{ 1.0f } );
					}
				}
			}
		}

		static bool isAnimForSkeleton( AssimpImporter const & importer
			, aiAnimation const & animation
			, castor3d::Skeleton const & skeleton )
		{
			auto channels = castor::makeArrayView( animation.mChannels, animation.mNumChannels );
			return ( skeleton.getNodes().end() == std::find_if( skeleton.getNodes().begin()
				, skeleton.getNodes().end()
				, [&importer, &channels]( castor3d::SkeletonNodeUPtr const & nodeLookup )
				{
					return channels.end() == std::find_if( channels.begin()
						, channels.end()
						, [&nodeLookup, &importer]( aiNodeAnim const * animLookup )
						{
							auto name = importer.getInternalName( animLookup->mNodeName );
							return nodeLookup->getName() == name;
						} );
				} ) )
				|| ( channels.end() == std::find_if( channels.begin()
					, channels.end()
					, [&importer, &skeleton]( aiNodeAnim const * animLookup )
					{
						auto name = importer.getInternalName( animLookup->mNodeName );
						return skeleton.findNode( name ) == nullptr;
					} ) );
		}

		static aiNodeAnim const * findNodeAnim( aiAnimation const & animation
			, const castor::String & nodeName )
		{
			aiNodeAnim const * result = nullptr;
			auto it = std::find_if( animation.mChannels
				, animation.mChannels + animation.mNumChannels
				, [&nodeName]( aiNodeAnim const * const p_nodeAnim )
				{
					return castor::string::stringCast< castor::xchar >( p_nodeAnim->mNodeName.data ) == nodeName;
				} );

			if ( it != animation.mChannels + animation.mNumChannels )
			{
				result = *it;
			}

			return result;
		}
	}

	//*********************************************************************************************

	SkeletonImporter::SkeletonImporter( AssimpImporter & importer )
		: m_importer{ importer }
	{
	}

	void SkeletonImporter::import( castor::Path const & fileName
		, aiScene const & aiScene
		, castor3d::Scene & scene
		, bool replaceInverseTransforms )
	{
		if ( replaceInverseTransforms )
		{
			m_bonesNodes.clear();
		}

		m_meshesNodes.clear();
		std::vector< aiNode * > work;
		work.push_back( aiScene.mRootNode );

		while ( !work.empty() )
		{
			auto node = work.back();
			work.pop_back();
			work.insert( work.end()
				, node->mChildren
				, node->mChildren + node->mNumChildren );

			for ( auto aiMeshIndex : castor::makeArrayView( node->mMeshes, node->mNumMeshes ) )
			{
				m_meshesNodes.emplace( aiMeshIndex, node );
			}
		}

		for ( auto aiMesh : castor::makeArrayView( aiScene.mMeshes, aiScene.mNumMeshes ) )
		{
			for ( auto aiBone : castor::makeArrayView( aiMesh->mBones, aiMesh->mNumBones ) )
			{
				m_bonesNodes.emplace( makeString( aiBone->mName )
					, BoneData{ makeMatrix4x4f( aiBone->mOffsetMatrix ) } );
			}
		}

		m_replaceInverseTransforms = replaceInverseTransforms;
		m_needsAnimsReparse = false;
		m_fileName = fileName;
		doProcessSkeletons( aiScene
			, scene
			, castor::makeArrayView( aiScene.mMeshes, aiScene.mNumMeshes ) );
	}

	bool SkeletonImporter::isBoneNode( aiNode const & aiNode )const
	{
		return m_bonesNodes.find( makeString( aiNode.mName ) ) != m_bonesNodes.end();
	}

	castor3d::SkeletonRPtr SkeletonImporter::getSkeleton( aiMesh const & aiMesh
		, uint32_t aiMeshIndex )const
	{
		auto it = m_meshSkeletons.find( makeString( aiMesh.mName ) + "_" + castor::string::toString( aiMeshIndex ) );

		if ( it != m_meshSkeletons.end() )
		{
			return it->second;
		}

		return nullptr;
	}

	void SkeletonImporter::doProcessSkeletons( aiScene const & aiScene
		, castor3d::Scene & scene
		, castor::ArrayView< aiMesh * > aiMeshes )
	{
		uint32_t meshIndex = 0u;

		for ( auto aiMesh : aiMeshes )
		{
			if ( aiMesh->HasBones() )
			{
				auto rootNode = skeletons::findRootSkeletonNode( *aiScene.mRootNode
					, castor::makeArrayView( aiMesh->mBones, aiMesh->mNumBones )
					, meshIndex
					, m_meshesNodes
					, m_bonesNodes );
				auto preRootNode = rootNode->mParent
					? rootNode->mParent
					: rootNode;
				castor3d::SkeletonRPtr skeleton;
				auto nodeName = makeString( preRootNode->mName );
				auto it = m_skeletons.find( nodeName );

				if ( it == m_skeletons.end() )
				{
					// Is the loaded skeletons' pre-root node in the current pre-root node ?
					it = std::find_if( m_skeletons.begin()
						, m_skeletons.end()
						, [preRootNode]( auto & lookup )
						{
							return preRootNode->FindNode( lookup.first.c_str() ) != nullptr;
						} );

					if ( it != m_skeletons.end() )
					{
						// If so, complete the skeleton and update the list
						skeleton = it->second;
						m_needsAnimsReparse = true;
						std::map< castor::String, BoneData > fake;
						skeletons::completeSkeleton( m_importer
							, fake
							, *skeleton
							, *preRootNode
							, aiScene.mRootNode->FindNode( it->first.c_str() ) );
						m_skeletons.erase( it );
						it = m_skeletons.emplace( nodeName, skeleton ).first;
					}
				}

				if ( it == m_skeletons.end() )
				{
					it = std::find_if( m_skeletons.begin()
						, m_skeletons.end()
						, [preRootNode]( auto & lookup )
						{
							auto inIt = std::find_if( lookup.second->getNodes().begin()
								, lookup.second->getNodes().end()
								, [preRootNode]( castor3d::SkeletonNodeUPtr const & lookupNode )
								{
									return preRootNode->FindNode( lookupNode->getName().c_str() ) != nullptr;
								} );
							return inIt != lookup.second->getNodes().end();
						} );

					if ( it != m_skeletons.end() )
					{
						skeleton = it->second;
						m_skeletons.erase( it );
						it = m_skeletons.emplace( nodeName, skeleton ).first;

						if ( m_replaceInverseTransforms )
						{
							skeletons::replaceInverseTransforms( m_importer
								, m_bonesNodes
								, *skeleton );
						}
					}
				}

				if ( it == m_skeletons.end() )
				{
					it = m_skeletons.emplace( nodeName, nullptr ).first;
					castor3d::log::info << cuT( "  Skeleton found" ) << std::endl;
					it->second = scene.addNewSkeleton( normalizeName( m_importer.getInternalName( rootNode->mName ) ), scene );
					skeleton = it->second;
					skeleton->setGlobalInverseTransform( makeMatrix4x4f( rootNode->mTransformation ).getInverse() );
					skeletons::processSkeletonNodes( m_importer
						, m_bonesNodes
						, *skeleton
						, *rootNode
						, skeletons::processSkeletonNode( m_importer
							, m_bonesNodes
							, *skeleton
							, *rootNode
							, nullptr ) );
				}
				else
				{
					skeleton = it->second;
				}

				m_meshSkeletons.emplace( makeString( aiMesh->mName ) + "_" + castor::string::toString( meshIndex ), skeleton );
			}

			++meshIndex;
		}

		for ( auto skeletonIt : m_skeletons )
		{
			auto & skeleton = *skeletonIt.second;

			for ( auto aiAnimation : castor::makeArrayView( aiScene.mAnimations, aiScene.mNumAnimations ) )
			{
				if ( skeletons::isAnimForSkeleton( m_importer, *aiAnimation, skeleton ) )
				{
					m_needsAnimsReparse = true;
					doProcessSkeletonAnimation( m_fileName.getFileName()
						, skeleton
						, *aiScene.mRootNode
						, *aiAnimation );
				}
			}
		}
	}

	void SkeletonImporter::doProcessSkeletonAnimation( castor::String const & animName
		, castor3d::Skeleton & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation )
	{
		castor::String name{ normalizeName( makeString( aiAnimation.mName ) ) };

		if ( name.empty() )
		{
			name = normalizeName( animName );
		}

		auto & animation = skeleton.createAnimation( name );
		int64_t ticksPerSecond = aiAnimation.mTicksPerSecond != 0.0
			? int64_t( aiAnimation.mTicksPerSecond )
			: 25ll;
		SkeletonAnimationKeyFrameMap keyframes;
		SkeletonAnimationObjectSet notAnimated;
		doProcessSkeletonAnimationNodes( animation
			, ticksPerSecond
			, skeleton
			, aiNode
			, aiAnimation
			, keyframes
			, notAnimated );

		for ( auto & object : notAnimated )
		{
			castor::Matrix4x4f transform{ 1.0f };

			if ( object->getName().find( "$AssimpFbx$_PreRotation" ) == castor::String::npos )
			{
				auto node = aiNode.FindNode( m_importer.getExternalName( object->getName() ).c_str() );
				CU_Require( node );
				transform = makeMatrix4x4f( node->mTransformation );
			}

			for ( auto & keyFrame : keyframes )
			{
				keyFrame.second->addAnimationObject( *object, transform );
			}
		}

		for ( auto & keyFrame : keyframes )
		{
			animation.addKeyFrame( std::move( keyFrame.second ) );
		}

		animation.updateLength();

		if ( animation.getLength() == 0_ms )
		{
			skeleton.removeAnimation( name );
		}
		else
		{
			castor3d::log::info << cuT( "  Skeleton Animation found: [" ) << name << cuT( "]" ) << std::endl;
		}
	}

	void SkeletonImporter::doProcessSkeletonAnimationNodes( castor3d::SkeletonAnimation & animation
		, int64_t ticksPerSecond
		, castor3d::Skeleton & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation
		, SkeletonAnimationKeyFrameMap & keyFrames
		, SkeletonAnimationObjectSet & notAnimated )
	{
		for ( auto & skelNode : skeleton.getNodes() )
		{
			auto name = skelNode->getName();
			auto nodeName = m_importer.getExternalName( name );
			const aiNodeAnim * aiNodeAnim = skeletons::findNodeAnim( aiAnimation
				, nodeName );
			auto parentSkelNode = skelNode->getParent();
			castor3d::SkeletonAnimationObjectSPtr parent;

			if ( parentSkelNode )
			{
				parent = animation.getObject( parentSkelNode->getType()
					, parentSkelNode->getName() );
				CU_Require( parent );
			}

			castor3d::SkeletonAnimationObjectSPtr object;
			CU_Require( !animation.hasObject( skelNode->getType(), name ) );

			if ( skelNode->getType() == castor3d::SkeletonNodeType::eBone )
			{
				object = animation.addObject( static_cast< castor3d::BoneNode & >( *skelNode )
					, parent );
			}
			else
			{
				object = animation.addObject( *skelNode, parent );
			}

			if ( parent )
			{
				parent->addChild( object );
			}

			if ( aiNodeAnim )
			{
				processAnimationNodeKeys( *aiNodeAnim
					, m_importer.getEngine()->getRenderLoop().getWantedFps()
					, ticksPerSecond
					, animation
					, keyFrames
					, [&object]( castor3d::SkeletonAnimationKeyFrame & keyframe
						, castor::Point3f const & position
						, castor::Quaternion const & orientation
						, castor::Point3f const & scale )
					{
						keyframe.addAnimationObject( *object, position, orientation, scale );
					} );
			}
			else
			{
				notAnimated.insert( object );
			}
		}
	}

	//*********************************************************************************************
}
