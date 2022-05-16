#include "AssimpImporter/AssimpAnimationImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimation.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationObject.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimation.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp>

namespace c3d_assimp
{
	namespace anims
	{
		static aiNodeAnim const * findSkelNodeAnim( aiAnimation const & animation
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

		static void fillKeyFrame( std::vector< castor3d::SubmeshAnimationBuffer > const & meshAnimBuffers
			, castor::ArrayView< uint32_t > values
			, castor::ArrayView< double > weights
			, castor3d::Submesh const & submesh
			, castor3d::MeshAnimationKeyFrame & keyFrame )
		{
			castor3d::SubmeshAnimationBuffer buffer{ submesh.getPositions()
				, submesh.getNormals()
				, submesh.getTangents()
				, submesh.getTexcoords0()
				, submesh.getTexcoords1()
				, submesh.getTexcoords2()
				, submesh.getTexcoords3() };
			auto valueIt = values.begin();
			auto weightIt = weights.begin();

			while ( valueIt != values.end() )
			{
				auto value = *valueIt;
				CU_Require( value < meshAnimBuffers.size() );
				applyMorphTarget( float( *weightIt )
					, meshAnimBuffers[value]
					, buffer.positions
					, buffer.normals
					, buffer.tangents
					, buffer.texcoords0
					, buffer.texcoords1
					, buffer.texcoords2
					, buffer.texcoords3 );
				++valueIt;
				++weightIt;
			}

			keyFrame.addSubmeshBuffer( submesh, buffer );
		}
	}

	using SceneNodeAnimationKeyFrameMap = std::map< castor::Milliseconds, castor3d::SceneNodeAnimationKeyFrameUPtr >;

	AssimpAnimationImporter::AssimpAnimationImporter( castor3d::Engine & engine )
		: castor3d::AnimationImporter{ engine }
	{
	}

	bool AssimpAnimationImporter::doImportSkeleton( castor3d::SkeletonAnimation & animation )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = animation.getName();
		auto & skeleton = static_cast< castor3d::Skeleton const & >( *animation.getAnimable() );
		auto & animations = file.getSkeletonsAnimations( skeleton );
		auto it = animations.find( name );

		if ( it == animations.end() )
		{
			return false;
		}

		std::map< castor::String, castor3d::ObjectTransform > nodeTransforms;
		auto & aiAnimation = *it->second;
		auto & aiNode = *file.getScene().mRootNode;
		auto [frameCount, frameTicks] = getAnimationFrameTicks( aiAnimation );
		castor3d::log::info << cuT( "  Skeleton Animation found: [" ) << name << cuT( "]" ) << std::endl;
		int64_t ticksPerSecond = aiAnimation.mTicksPerSecond != 0.0
			? int64_t( aiAnimation.mTicksPerSecond )
			: 25ll;
		SkeletonAnimationKeyFrameMap keyframes;
		SkeletonAnimationObjectSet notAnimated;
		doProcessSkeletonAnimationNodes( animation
			, fromAssimp( frameTicks, ticksPerSecond )
			, ticksPerSecond
			, skeleton
			, aiNode
			, aiAnimation
			, keyframes
			, notAnimated );

		for ( auto & object : notAnimated )
		{
			auto node = aiNode.FindNode( file.getExternalName( object->getName() ).c_str() );
			castor3d::ObjectTransform objTransform{};

			if ( node )
			{
				aiVector3D scaling, position;
				aiQuaternion rotate;
				node->mTransformation.Decompose( scaling, rotate, position );
				auto translate = fromAssimp( position );
				auto scale = fromAssimp( scaling );
				auto orientation = fromAssimp( rotate );
				objTransform = castor3d::ObjectTransform{ nullptr, translate, scale, orientation };
				auto transformIt = nodeTransforms.emplace( makeString( node->mName )
					, objTransform ).first;
				transformIt->second = objTransform;
			}
			else
			{
				auto transformIt = nodeTransforms.find( file.getExternalName( object->getName() ) );
				CU_Require( transformIt != nodeTransforms.end() );
				objTransform = transformIt->second;
			}

			for ( auto & keyFrame : keyframes )
			{
				auto kfit = keyFrame.second->find( *object );

				if ( kfit == keyFrame.second->end() )
				{
					keyFrame.second->addAnimationObject( *object
						, objTransform.translate
						, objTransform.rotate
						, objTransform.scale );
				}
				else
				{
					kfit->translate = objTransform.translate;
					kfit->rotate = objTransform.rotate;
					kfit->scale = objTransform.scale;
				}
			}
		}

		for ( auto & keyFrame : keyframes )
		{
			animation.addKeyFrame( std::move( keyFrame.second ) );
		}

		return true;
	}

	bool AssimpAnimationImporter::doImportMesh( castor3d::MeshAnimation & animation )
	{
		auto & file = static_cast< AssimpImporterFile & >( *m_file );
		auto name = animation.getName();
		auto & mesh = static_cast< castor3d::Mesh const & >( *animation.getAnimable() );
		uint32_t index{};

		for ( auto submesh : mesh )
		{
			auto & animations = file.getMeshesAnimations( mesh, index );
			auto animIt = animations.find( name );

			if ( animIt == animations.end() )
			{
				return false;
			}

			castor3d::log::info << cuT( "  Mesh Animation found for submesh " ) << index << cuT( ": [" ) << name << cuT( "]" ) << std::endl;
			auto & aiAnimation = *animIt->second.second;
			auto & aiMesh = *animIt->second.first;

			castor3d::MeshAnimationSubmesh animSubmesh{ animation, *submesh };
			animation.addChild( std::move( animSubmesh ) );

			if ( aiAnimation.mKeys->mTime > 0.0 )
			{
				auto kfit = animation.find( 0_ms );
				castor3d::MeshAnimationKeyFrame * kf{};

				if ( kfit == animation.end() )
				{
					auto keyFrame = std::make_unique< castor3d::MeshAnimationKeyFrame >( animation, 0_ms );
					kf = keyFrame.get();
					animation.addKeyFrame( std::move( keyFrame ) );
				}
				else
				{
					kf = &static_cast< castor3d::MeshAnimationKeyFrame & >( **kfit );
				}

				auto & csubmesh = const_cast< castor3d::Submesh const & >( *submesh );
				kf->addSubmeshBuffer( *submesh
					, { csubmesh.getPositions()
						, csubmesh.getNormals()
						, csubmesh.getTangents()
						, csubmesh.getTexcoords0()
						, csubmesh.getTexcoords1()
						, csubmesh.getTexcoords2()
						, csubmesh.getTexcoords3() } );
			}

			for ( auto & morphKey : castor::makeArrayView( aiAnimation.mKeys, aiAnimation.mNumKeys ) )
			{
				auto timeIndex = castor::Milliseconds{ uint64_t( morphKey.mTime ) };
				auto kfit = animation.find( timeIndex );
				castor3d::MeshAnimationKeyFrame * kf{};

				if ( kfit == animation.end() )
				{
					auto keyFrame = std::make_unique< castor3d::MeshAnimationKeyFrame >( animation, timeIndex );
					kf = keyFrame.get();
					animation.addKeyFrame( std::move( keyFrame ) );
				}
				else
				{
					kf = &static_cast< castor3d::MeshAnimationKeyFrame & >( **kfit );
				}

				std::vector< castor3d::SubmeshAnimationBuffer > const * meshAnimBuffers;

				if ( file.hasMeshAnimBuffers( &aiMesh ) )
				{
					meshAnimBuffers = &file.getMeshAnimBuffers( &aiMesh );
				}
				else
				{
					auto positions = submesh->getComponent< castor3d::PositionsComponent >();
					auto normals = submesh->getComponent< castor3d::NormalsComponent >();
					castor::Point3fArray tan;
					castor::Point3fArray tex0;
					castor::Point3fArray tex1;
					castor::Point3fArray tex2;
					castor::Point3fArray tex3;
					castor::Point3fArray * tangents = &tan;
					castor::Point3fArray * texcoords0 = &tex0;
					castor::Point3fArray * texcoords1 = &tex1;
					castor::Point3fArray * texcoords2 = &tex2;
					castor::Point3fArray * texcoords3 = &tex3;

					if ( aiMesh.HasTextureCoords( 0u ) )
					{
						tangents = &submesh->getTangents();
						texcoords0 = &submesh->getTexcoords0();
					}

					if ( aiMesh.HasTextureCoords( 1u ) )
					{
						texcoords1 = &submesh->getTexcoords1();
					}

					if ( aiMesh.HasTextureCoords( 2u ) )
					{
						texcoords2 = &submesh->getTexcoords2();
					}

					if ( aiMesh.HasTextureCoords( 3u ) )
					{
						texcoords3 = &submesh->getTexcoords3();
					}

					meshAnimBuffers = &file.addMeshAnimBuffers( &aiMesh
						, gatherMeshAnimBuffers( positions->getData()
							, normals->getData()
							, *tangents
							, *texcoords0
							, *texcoords1
							, *texcoords2
							, *texcoords3
							, castor::makeArrayView( aiMesh.mAnimMeshes, aiMesh.mNumAnimMeshes ) ) );
				}

				anims::fillKeyFrame( *meshAnimBuffers
					, castor::makeArrayView( morphKey.mValues, morphKey.mNumValuesAndWeights )
					, castor::makeArrayView( morphKey.mWeights, morphKey.mNumValuesAndWeights )
					, *submesh
					, *kf );
			}

			++index;
		}

		return true;
	}

	bool AssimpAnimationImporter::doImportNode( castor3d::SceneNodeAnimation & animation )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = animation.getName();
		auto & node = static_cast< castor3d::SceneNode const & >( *animation.getAnimable() );
		auto & animations = file.getNodesAnimations( node );
		auto it = animations.find( name );

		if ( it == animations.end() )
		{
			return false;
		}

		auto & aiAnimation = *it->second.first;
		auto & aiNodeAnim = *it->second.second;
		auto [frameCount, frameTicks] = getNodeAnimFrameTicks( aiNodeAnim );
		castor3d::log::info << cuT( "  SceneNode Animation found: [" ) << name << cuT( "]" ) << std::endl;
		int64_t ticksPerSecond = aiAnimation.mTicksPerSecond != 0.0
			? int64_t( aiAnimation.mTicksPerSecond )
			: 25ll;
		SceneNodeAnimationKeyFrameMap keyframes;
		processAnimationNodeKeys( aiNodeAnim
			, getEngine()->getWantedFps()
			, fromAssimp( frameTicks, ticksPerSecond )
			, ticksPerSecond
			, animation
			, keyframes
			, []( castor3d::SceneNodeAnimationKeyFrame & keyframe
				, castor::Point3f const & position
				, castor::Quaternion const & orientation
				, castor::Point3f const & scale )
			{
				keyframe.setTransform( position, orientation, scale );
			} );

		for ( auto & keyFrame : keyframes )
		{
			animation.addKeyFrame( std::move( keyFrame.second ) );
		}

		return true;
	}

	void AssimpAnimationImporter::doProcessSkeletonAnimationNodes( castor3d::SkeletonAnimation & animation
		, castor::Milliseconds maxTime
		, int64_t ticksPerSecond
		, castor3d::Skeleton const & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation
		, SkeletonAnimationKeyFrameMap & keyFrames
		, SkeletonAnimationObjectSet & notAnimated )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );

		for ( auto & skelNode : skeleton.getNodes() )
		{
			auto name = skelNode->getName();
			auto nodeName = file.getExternalName( name );
			const aiNodeAnim * aiNodeAnim = anims::findSkelNodeAnim( aiAnimation
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
					, getEngine()->getWantedFps()
					, maxTime
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
}
