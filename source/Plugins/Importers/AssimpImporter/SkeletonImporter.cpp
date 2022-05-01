#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#pragma GCC diagnostic ignored "-Werror=overloaded-virtual"

#include "AssimpImporter/SkeletonImporter.hpp"
#include "AssimpImporter/AssimpHelpers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Interpolator.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp>
#include <Castor3D/Render/RenderLoop.hpp>

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
		static aiNodeAnim const * findNodeAnim( const aiAnimation & animation
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

		static void processSkeletonNodes( castor::String const & prefix
			, std::map< aiNode const *, aiBone const * > const & bonesNodes
			, castor3d::Skeleton & skeleton
			, aiNode const & parentAiNode )
		{
			for ( auto node : castor::makeArrayView( parentAiNode.mChildren, parentAiNode.mNumChildren ) )
			{
				castor::String name = prefix + node->mName.C_Str();
				auto skelNode = skeleton.findNode( name );

				if ( !skelNode )
				{
					auto it = bonesNodes.find( node );

					if ( it == bonesNodes.end() )
					{
						skelNode = skeleton.createNode( name );
					}
					else
					{
						skelNode = skeleton.createBone( name, makeMatrix4x4f( it->second->mOffsetMatrix ) );
					}
				}

				processSkeletonNodes( prefix, bonesNodes, skeleton, *node );
			}
		}

		static void processSkeletonAnimNodes( castor::String const & prefix
			, std::map< aiNode const *, aiBone const * > const & bonesNodes
			, castor3d::Skeleton & skeleton
			, aiScene const & aiScene
			, aiNode const & parentAiNode )
		{
			for ( auto node : castor::makeArrayView( parentAiNode.mChildren, parentAiNode.mNumChildren ) )
			{
				castor::String name = prefix + node->mName.C_Str();
				auto skelNode = skeleton.findNode( name );

				for ( auto aiAnimation : castor::makeArrayView( aiScene.mAnimations, aiScene.mNumAnimations ) )
				{
					const aiNodeAnim * aiNodeAnim = skeletons::findNodeAnim( *aiAnimation, node->mName.C_Str() );

					if ( aiNodeAnim && !skelNode )
					{
						auto it = bonesNodes.find( node );

						if ( it == bonesNodes.end() )
						{
							skelNode = skeleton.createNode( name );
						}
						else
						{
							skelNode = skeleton.createBone( name, makeMatrix4x4f( it->second->mOffsetMatrix ) );
						}
					}
				}

				processSkeletonAnimNodes( prefix, bonesNodes, skeleton, aiScene, *node );
			}
		}
	}

	//*********************************************************************************************

	SkeletonImporter::SkeletonImporter( castor3d::MeshImporter & importer )
		: m_importer{ importer }
	{
	}

	void SkeletonImporter::import( castor::String const & prefix
		, castor::Path const & fileName
		, uint32_t importFlags
		, aiScene const & aiScene
		, castor3d::Scene & scene )
	{
		for ( auto aiMesh : castor::makeArrayView( aiScene.mMeshes, aiScene.mNumMeshes ) )
		{
			for ( auto aiBone : castor::makeArrayView( aiMesh->mBones, aiMesh->mNumBones ) )
			{
				m_bonesNodes.emplace( aiScene.mRootNode->FindNode( aiBone->mName.C_Str() ), aiBone );
			}
		}

		m_prefix = prefix;
		m_fileName = fileName;
		m_importFlags = importFlags;
		doProcessSkeletons( aiScene
			, scene
			, castor::makeArrayView( aiScene.mMeshes, aiScene.mNumMeshes ) );
	}

	bool SkeletonImporter::isBoneNode( aiNode const & aiNode )const
	{
		return m_bonesNodes.find( &aiNode ) != m_bonesNodes.end();
	}

	castor3d::SkeletonSPtr SkeletonImporter::getSkeleton( aiMesh const & aiMesh )const
	{
		auto it = m_meshSkeletons.find( &aiMesh );

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
		for ( auto aiMesh : aiMeshes )
		{
			if ( aiMesh->HasBones() )
			{
				auto skeleton = std::make_shared< castor3d::Skeleton >( scene );
				skeleton->setGlobalInverseTransform( makeMatrix4x4f( aiScene.mRootNode->mTransformation ).getInverse() );
				castor3d::log::info << cuT( "  Skeleton found" ) << std::endl;
				doProcessBones( *skeleton, castor::makeArrayView( aiMesh->mBones, aiMesh->mNumBones ) );
				doProcessSkeletonNodes( *skeleton, aiScene, *aiScene.mRootNode );
				doCheckNewSkeleton( skeleton );
				m_meshSkeletons.emplace( aiMesh, skeleton );
			}
		}

		for ( auto skeleton : m_skeletons )
		{
			doLinkSkeletonNodes( *skeleton, *aiScene.mRootNode );

			for ( auto aiAnimation : castor::makeArrayView( aiScene.mAnimations, aiScene.mNumAnimations ) )
			{
				doProcessSkeletonAnimation( m_fileName.getFileName()
					, *skeleton
					, *aiScene.mRootNode
					, *aiAnimation );
			}

			if ( castor::string::upperCase( m_fileName.getExtension() ) == cuT( "MD5MESH" ) )
			{
				// Workaround to load multiple animations with MD5 models.
				castor::PathArray files;
				castor::File::listDirectoryFiles( m_fileName.getPath(), files );

				for ( auto file : files )
				{
					if ( castor::string::lowerCase( file.getExtension() ) == cuT( "md5anim" ) )
					{
						// The .md5anim with the same name as the .md5mesh has already been loaded by assimp.
						if ( file.getFileName() != m_fileName.getFileName() )
						{
							Assimp::Importer importer;
							auto animScene = importer.ReadFile( file, m_importFlags );

							for ( auto aiAnimation : castor::makeArrayView( animScene->mAnimations, animScene->mNumAnimations ) )
							{
								doProcessSkeletonAnimation( file.getFileName()
									, *skeleton
									, *animScene->mRootNode
									, *aiAnimation );
							}

							importer.FreeScene();
						}
					}
				}
			}
		}
	}

	void SkeletonImporter::doProcessBones( castor3d::Skeleton & skeleton
		, castor::ArrayView< aiBone * > aiBones )
	{
		for ( auto aiBone : aiBones )
		{
			castor::String name = m_prefix + castor::string::stringCast< castor::xchar >( aiBone->mName.C_Str() );
			auto node = skeleton.findNode( name );

			if ( !node )
			{
				castor3d::log::debug << cuT( "    Bone: [" ) << name << cuT( "]" ) << std::endl;
				skeleton.createBone( name
					, makeMatrix4x4f( aiBone->mOffsetMatrix ) );
			}
		}
	}

	bool SkeletonImporter::doCheckNewSkeleton( castor3d::SkeletonSPtr & skeleton )
	{
		auto isSubSkeleton = []( castor3d::Skeleton const & lhs
			, castor3d::Skeleton const & rhs )
		{
			return std::all_of( lhs.getBones().begin()
				, lhs.getBones().end()
				, [&rhs]( castor3d::BoneNode const * lookup )
				{
					auto node = rhs.findNode( lookup->getName() );
					return node != nullptr
						&& node->getType() == castor3d::SkeletonNode::eBone;
				} );
		};

		auto it = std::find_if( m_skeletons.begin()
			, m_skeletons.end()
			, [&skeleton, isSubSkeleton]( castor3d::SkeletonSPtr const & lookup )
			{
				return isSubSkeleton( *skeleton, *lookup )
					|| isSubSkeleton( *lookup, *skeleton );
			} );

		if ( it == m_skeletons.end() )
		{
			m_skeletons.insert( skeleton );
			return true;
		}

		auto & merged = **it;

		for ( auto & node : skeleton->getNodes() )
		{
			if ( !merged.findNode( node->getName() ) )
			{
				if ( node->getType() == castor3d::SkeletonNode::eBone )
				{
					merged.createBone( node->getName()
						, static_cast< castor3d::BoneNode const & >( *node ).getInverseTransform() );
				}
				else
				{
					merged.createNode( node->getName() );
				}

			}
		}

		skeleton = *it;
		return false;
	}

	void SkeletonImporter::doLinkSkeletonNodes( castor3d::Skeleton & skeleton
		, aiNode const & aiNode )
	{
		for ( auto & node : skeleton.getNodes() )
		{
			auto skelNode = aiNode.FindNode( node->getName().substr( m_prefix.size() ).c_str() );
			CU_Require( skelNode );

			if ( skelNode->mParent )
			{
				castor::String name{ m_prefix + castor::string::stringCast< castor::xchar >( skelNode->mParent->mName.C_Str() ) };
				auto parent = skeleton.findNode( name );

				if ( parent )
				{
					skeleton.setNodeParent( *node, *parent );
				}
			}
		}
	}

	void SkeletonImporter::doProcessSkeletonAnimation( castor::String const & animName
		, castor3d::Skeleton & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation )
	{
		castor::String name{ m_prefix + castor::string::stringCast< castor::xchar >( aiAnimation.mName.C_Str() ) };

		if ( name.empty() )
		{
			name = animName;
		}

		castor3d::log::info << cuT( "  Skeleton Animation found: [" ) << name << cuT( "]" ) << std::endl;
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
			, nullptr
			, keyframes
			, notAnimated );

		for ( auto & object : notAnimated )
		{
			for ( auto & keyFrame : keyframes )
			{
				if ( !keyFrame.second->hasObject( *object ) )
				{
					keyFrame.second->addAnimationObject( *object, object->getNodeTransform() );
				}
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
	}

	void SkeletonImporter::doProcessSkeletonAnimationNodes( castor3d::SkeletonAnimation & animation
		, int64_t ticksPerSecond
		, castor3d::Skeleton & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation
		, castor3d::SkeletonAnimationObjectSPtr parent
		, SkeletonAnimationKeyFrameMap & keyFrames
		, SkeletonAnimationObjectSet & notAnimated )
	{
		const aiNodeAnim * aiNodeAnim = skeletons::findNodeAnim( aiAnimation, aiNode.mName.C_Str() );
		castor::String name = m_prefix + castor::string::stringCast< castor::xchar >( aiNode.mName.data );
		auto skelNode = skeleton.findNode( name );

		if ( aiNodeAnim || skelNode )
		{
			castor3d::SkeletonAnimationObjectSPtr object;
			bool added = false;

			if ( skelNode )
			{
				if ( skelNode->getType() == castor3d::SkeletonNode::eBone )
				{
					auto bone = &static_cast< castor3d::BoneNode & >( *skelNode );

					if ( !animation.hasObject( castor3d::SkeletonAnimationObjectType::eBone, name ) )
					{
						object = animation.addObject( *bone, parent );
						added = true;
					}
					else
					{
						object = animation.getObject( castor3d::SkeletonAnimationObjectType::eBone, name );
					}
				}
				else
				{
					if ( !animation.hasObject( castor3d::SkeletonAnimationObjectType::eNode, name ) )
					{
						object = animation.addObject( *skelNode, parent );
						added = true;
					}
					else
					{
						object = animation.getObject( castor3d::SkeletonAnimationObjectType::eNode, name );
					}
				}
			}
			else
			{
				skelNode = skeleton.createNode( name );

				if ( parent )
				{
					auto objNode = ( parent->getType() == castor3d::SkeletonAnimationObjectType::eNode
						? static_cast< castor3d::SkeletonAnimationNode const & >( *parent ).getNode()
						: &static_cast< castor3d::SkeletonNode & >( *static_cast< castor3d::SkeletonAnimationBone const & >( *parent ).getBone() ) );
					skeleton.setNodeParent( *skelNode, *objNode );
				}

				if ( !animation.hasObject( castor3d::SkeletonAnimationObjectType::eNode, name ) )
				{
					object = animation.addObject( *skelNode, parent );
					added = true;
				}
				else
				{
					object = animation.getObject( castor3d::SkeletonAnimationObjectType::eNode, name );
				}
			}

			if ( object )
			{
				if ( added )
				{
					if ( parent && object != parent )
					{
						parent->addChild( object );
					}

					object->setNodeTransform( makeMatrix4x4f( aiNode.mTransformation ) );
				}
				else
				{
					CU_Ensure( object->getNodeTransform() == makeMatrix4x4f( aiNode.mTransformation ) );
					CU_Ensure( object->getParent() == parent || object == parent );
				}

				if ( aiNodeAnim )
				{
					doProcessAnimationNodeKeys( *aiNodeAnim
						, ticksPerSecond
						, *object
						, animation
						, keyFrames );
				}
				else
				{
					notAnimated.insert( object );
				}

				parent = object;
			}
		}

		for ( auto node : castor::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doProcessSkeletonAnimationNodes( animation
				, ticksPerSecond
				, skeleton
				, *node
				, aiAnimation
				, parent
				, keyFrames
				, notAnimated );
		}
	}

	void SkeletonImporter::doProcessSkeletonNodes( castor3d::Skeleton & skeleton
		, aiScene const & aiScene
		, aiNode const & node )
	{
		auto bones = skeleton.getBones();

		for ( auto bone : bones )
		{
			auto boneNode = node.FindNode( bone->getName().substr( m_prefix.size() ).c_str() );
			CU_Require( boneNode );
			skeletons::processSkeletonNodes( m_prefix
				, m_bonesNodes
				, skeleton
				, *boneNode );
		}

		skeletons::processSkeletonAnimNodes( m_prefix
			, m_bonesNodes
			, skeleton
			, aiScene
			, *aiScene.mRootNode );
	}

	void SkeletonImporter::doProcessAnimationNodeKeys( aiNodeAnim const & aiNodeAnim
		, int64_t ticksPerSecond
		, castor3d::SkeletonAnimationObject & object
		, castor3d::SkeletonAnimation & animation
		, SkeletonAnimationKeyFrameMap & keyframes )
	{
		processAnimationNodeKeys( aiNodeAnim
			, m_importer.getEngine()->getRenderLoop().getWantedFps()
			, ticksPerSecond
			, animation
			, keyframes
			, [&object]( castor3d::SkeletonAnimationKeyFrame & keyframe
				, castor::Point3f const & position
				, castor::Quaternion const & orientation
				, castor::Point3f const & scale )
			{
				keyframe.addAnimationObject( object, position, orientation, scale );
			} );
	}

	//*********************************************************************************************
}
