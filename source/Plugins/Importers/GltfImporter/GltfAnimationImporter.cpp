#include "GltfImporter/GltfAnimationImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Interpolator.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimation.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationObject.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimation.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp>

namespace c3d_gltf
{
	namespace anims
	{
		template< typename KeyT >
		struct KeyDataTyperT;

		template<>
		struct KeyDataTyperT< c3d::Point3f >
		{
			using Type = c3d::Point3f;
		};

		template<>
		struct KeyDataTyperT< c3d::Quaternion >
		{
			using Type = c3d::Point4f;
		};

		template< typename KeyT >
		using KeyDataTypeT = typename KeyDataTyperT< KeyT >::Type;

		template< typename KeyFrameT, typename AnimationT >
		static KeyFrameT & getKeyFrame( c3d::Milliseconds const & time
			, AnimationT & animation
			, c3d::Map< c3d::Milliseconds, c3d::UniquePtr< KeyFrameT > > & keyframes )
		{
			auto it = keyframes.find( time );

			if ( it == keyframes.end() )
			{
				it = keyframes.emplace( time
					, c3d::makeUnique< KeyFrameT >( animation, time ) ).first;
			}

			return *it->second;
		}

		template< typename T >
		static void findValue( c3d::Milliseconds time
			, typename c3d::Map< c3d::Milliseconds, T > const & map
			, typename c3d::Map< c3d::Milliseconds, T >::const_iterator & prv
			, typename c3d::Map< c3d::Milliseconds, T >::const_iterator & cur )
		{
			if ( map.empty() )
			{
				prv = map.end();
				cur = map.end();
			}
			else
			{
				cur = std::find_if( map.begin()
					, map.end()
					, [&time]( c3d::Pair< c3d::Milliseconds, T > const & pair )
					{
						return pair.first > time;
					} );

				if ( cur == map.end() )
				{
					--cur;
				}

				prv = cur;

				if ( prv != map.begin() )
				{
					prv--;
				}
			}
		}

		template< typename T >
		static T interpolate( c3d::Milliseconds const & time
			, c3d::Interpolator< T > const & interpolator
			, c3d::Map< c3d::Milliseconds, T > const & values
			, T const & defaultValue )
		{
			T result;

			if ( values.empty() )
			{
				result = defaultValue;
			}
			else if ( values.size() == 1 )
			{
				result = values.begin()->second;
			}
			else
			{
				auto prv = values.begin();
				auto cur = values.begin();
				findValue( time, values, prv, cur );

				if ( prv != cur )
				{
					auto dt = cur->first - prv->first;
					float factor = float( ( time - prv->first ).count() ) / float( dt.count() );
					result = interpolator.interpolate( prv->second, cur->second, factor );
				}
				else
				{
					result = prv->second;
				}
			}

			return result;
		}

		template< typename AnimationT, typename KeyFrameT, typename FuncT >
		static void synchroniseKeys( c3d::Map< c3d::Milliseconds, c3d::Point3f > const & translates
			, c3d::Map< c3d::Milliseconds, c3d::Quaternion > const & rotates
			, c3d::Map< c3d::Milliseconds, c3d::Point3f > const & scales
			, [[maybe_unused]] c3d::Set< c3d::Milliseconds > const & times
			, uint32_t fps
			, c3d::Milliseconds minTime
			, c3d::Milliseconds maxTime
			, AnimationT & animation
			, c3d::Map< c3d::Milliseconds, c3d::UniquePtr< KeyFrameT > > & keyframes
			, c3d::NodeTransform const & defaultTransform
			, FuncT fillKeyFrame )
		{
			c3d::InterpolatorT< c3d::Point3f, c3d::InterpolatorType::eLinear > pointInterpolator;
			c3d::InterpolatorT< c3d::Quaternion, c3d::InterpolatorType::eLinear > quatInterpolator;

			// Limit the key frames per second to 60, to spare RAM...
			auto wantedFps = std::min< int64_t >( 60, int64_t( fps ) );
			c3d::Milliseconds step{ 1000 / wantedFps };

			for ( auto time = minTime; time <= maxTime; time += step )
			{
				auto translate = interpolate( time, pointInterpolator, translates, defaultTransform.translate );
				auto rotate = interpolate( time, quatInterpolator, rotates, defaultTransform.rotate );
				auto scale = interpolate( time, pointInterpolator, scales, defaultTransform.scale );
				fillKeyFrame( getKeyFrame( time, animation, keyframes )
					, translate
					, rotate
					, scale );
			}
		}

		template< typename KeyT >
		static void processKeys( fastgltf::Asset const & impAsset
			, NodeAnimationChannelSampler const & animChannels
			, fastgltf::AnimationPath channel
			, c3d::Map< c3d::Milliseconds, KeyT > & result
			, CompressedBufferDataAdapter const & adapter )
		{
			auto it = std::find_if( animChannels.begin()
				, animChannels.end()
				, [channel]( AnimationChannelSampler const & lookup )
				{
					return lookup.first.path == channel;
				} );

			if ( it != animChannels.end() )
			{
				AnimationChannelSampler const & channelSampler = *it;
				c3d::Vector< float > times;
				iterateAccessor< float >( impAsset
					, impAsset.accessors[channelSampler.second.inputAccessor]
					, [&times]( float value )
					{
						times.push_back( value );
					}
					, adapter );
				c3d::Vector< KeyT > values;
				iterateAccessor< KeyDataTypeT< KeyT > >( impAsset
					, impAsset.accessors[channelSampler.second.outputAccessor]
					, [&values]( KeyDataTypeT< KeyT > value )
					{
						values.push_back( KeyT{ c3d::move( value ) } );
					}
					, adapter );
				// for AnimationInterpolation::CubicSpline can have more outputs
				uint32_t weightStride = uint32_t( values.size() / times.size() );
				uint32_t ii = ( channelSampler.second.interpolation == fastgltf::AnimationInterpolation::CubicSpline )
					? 1u
					: 0u;

				for ( uint32_t i = 0u; i < uint32_t( times.size() ); ++i )
				{
					auto timeIndex = c3d::Milliseconds{ uint64_t( times[i] * 1000u ) };
					uint32_t k = weightStride * i + ii;
					result.emplace( timeIndex, values[k] );
				}
			}
		}

		template< typename AnimationT
			, typename KeyFrameT
			, typename FuncT >
		static void processAnimationNodeKeys( fastgltf::Asset const & impAsset
			, NodeAnimationChannelSampler const & animChannels
			, uint32_t wantedFps
			, AnimationT & animation
			, c3d::Map< c3d::Milliseconds, c3d::UniquePtr< KeyFrameT > > & keyframes
			, c3d::Milliseconds const & minTime
			, c3d::Milliseconds const & maxTime
			, c3d::Set< c3d::Milliseconds > const & times
			, c3d::NodeTransform const & defaultTransform
			, FuncT fillKeyFrame
			, CompressedBufferDataAdapter const & adapter )
		{
			c3d::Map< c3d::Milliseconds, c3d::Point3f > translates;
			c3d::Map< c3d::Milliseconds, c3d::Quaternion > rotates;
			c3d::Map< c3d::Milliseconds, c3d::Point3f > scales;
			processKeys( impAsset, animChannels, fastgltf::AnimationPath::Translation, translates, adapter );
			processKeys( impAsset, animChannels, fastgltf::AnimationPath::Rotation, rotates, adapter );
			processKeys( impAsset, animChannels, fastgltf::AnimationPath::Scale, scales, adapter );
			synchroniseKeys( translates
				, rotates
				, scales
				, times
				, wantedFps
				, minTime
				, maxTime
				, animation
				, keyframes
				, defaultTransform
				, fillKeyFrame );
		}

		static void processAnimationNodeKeysTimes( fastgltf::Asset const & impAsset
			, NodeAnimationChannelSampler const & animChannels
			, c3d::Milliseconds & minTime
			, c3d::Milliseconds & maxTime
			, c3d::Set< c3d::Milliseconds > & allTimes
			, CompressedBufferDataAdapter const & adapter )
		{
			for ( auto const & channelSampler : animChannels )
			{
				iterateAccessor< float >( impAsset
					, impAsset.accessors[channelSampler.second.inputAccessor]
					, [&minTime, &maxTime, &allTimes]( float value )
					{
						auto timeIndex = c3d::Milliseconds{ uint64_t( value * 1000u ) };
						maxTime = std::max( maxTime, timeIndex );
						minTime = std::min( minTime, timeIndex );
						allTimes.insert( timeIndex );
					}
					, adapter );
			}
		}

		static NodeAnimationChannelSampler findNodeAnim( AnimationChannelSamplers const & animChannels
			, size_t nodeIndex )
		{
			NodeAnimationChannelSampler result{};

			for ( auto & itPath : animChannels )
			{
				for ( auto & itChannel : itPath.second )
				{
					if ( itChannel.first.nodeIndex == nodeIndex )
					{
						result.push_back( itChannel );
					}
				}
			}

			return result;
		}

		static void processSkeletonAnimationNodes( GltfImporterFile const & file
			, AnimationChannelSamplers const & animChannels
			, c3d::SkeletonAnimation & animation
			, c3d::Skeleton const & skeleton
			, SkeletonAnimationKeyFrameMap & keyFrames
			, SkeletonAnimationObjectSet & notAnimated )
		{
			auto & impAsset = file.getAsset();
			c3d::HashSet< size_t > parsedNodes;

			// In glTF files, nodes can have different keyframes for the same animation.
			// In Castor3D this is not supported, hence we first parse the times to force
			// keyframes synchronisation for the whole animation, and not only for a node
			// (which is already done through processAnimationNodeKeys).
			c3d::Milliseconds minTime{ std::numeric_limits< int32_t >::max() };
			c3d::Milliseconds maxTime{};
			c3d::Set< c3d::Milliseconds > allTimes;
			for ( auto const & channelSampler : animChannels )
			{
				processAnimationNodeKeysTimes( impAsset
					, channelSampler.second
					, minTime
					, maxTime
					, allTimes
					, file.getAdapter() );
			}

			for ( auto & skelNode : skeleton.getNodes() )
			{
				auto name = skelNode->getName();
				auto nodeIndex = file.getSkeletonNodeIndex( name );
				auto impNodeAnim = findNodeAnim( animChannels, nodeIndex );
				auto parentSkelNode = skelNode->getParent();
				c3d::SkeletonAnimationObjectRPtr parent{};

				if ( parentSkelNode )
				{
					parent = animation.getObject( parentSkelNode->getType()
						, parentSkelNode->getName() );
					CU_Require( parent );
				}

				c3d::SkeletonAnimationObjectRPtr object{};
				CU_Require( !animation.hasObject( skelNode->getType(), name ) );

				if ( skelNode->getType() == c3d::SkeletonNodeType::eBone )
				{
					object = animation.addObject( static_cast< c3d::BoneNode & >( *skelNode )
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

				if ( !impNodeAnim.empty() )
				{
					parsedNodes.emplace( nodeIndex );
					processAnimationNodeKeys( impAsset
						, impNodeAnim
						, file.getEngine()->getWantedFps()
						, animation
						, keyFrames
						, minTime
						, maxTime
						, allTimes
						, object->getNodeTransform()
						, [&object]( c3d::SkeletonAnimationKeyFrame & keyframe
							, c3d::Point3f const & position
							, c3d::Quaternion const & orientation
							, c3d::Point3f const & scale )
						{
							keyframe.addAnimationObject( *object, position, orientation, scale );
						}
						, file.getAdapter() );
				}
				else
				{
					notAnimated.insert( object );
				}
			}
		}

		static size_t getMeshNodeIndex( GltfImporterFile const & file
			, AnimationChannelSamplers const & channelSamplers
			, c3d::String const & name
			, uint32_t submeshIndex )
		{
			size_t result{};
			size_t meshIndex = file.getMeshIndex( name, submeshIndex );
			auto it = std::find_if( channelSamplers.begin()
				, channelSamplers.end()
				, [&result, meshIndex, &file]( AnimationChannelSamplers::value_type const & lookup )
				{
					return lookup.second.end() != std::find_if( lookup.second.begin()
						, lookup.second.end()
						, [&result, meshIndex, &file]( AnimationChannelSampler const & channelSampler )
						{
							bool ret = bool( channelSampler.first.nodeIndex );

							if ( ret )
							{
								auto & node = file.getAsset().nodes[*channelSampler.first.nodeIndex];
								ret = node.meshIndex && ( *node.meshIndex == meshIndex );

								if ( ret )
								{
									result = *channelSampler.first.nodeIndex;
								}
							}

							return ret;
						} );
				} );

			if ( it == channelSamplers.end() )
			{
				CU_LoaderError( "Couldn't find node index for animated submesh in animation channels" );
			}

			return result;
		}
	}

	using SceneNodeAnimationKeyFrameMap = c3d::Map< c3d::Milliseconds, c3d::SceneNodeAnimationKeyFrameUPtr >;

	GltfAnimationImporter::GltfAnimationImporter( c3d::Engine & engine )
		: c3d::AnimationImporter{ engine, cuT( "Gltf" ) }
	{
	}

	bool GltfAnimationImporter::doImportSkeleton( c3d::SkeletonAnimation & animation )
	{
		auto & file = static_cast< GltfImporterFile & >( *m_file );
		auto name = animation.getName();
		auto & skeleton = static_cast< c3d::Skeleton const & >( *animation.getAnimable() );
		auto animations = file.getSkinAnimations( skeleton );
		auto animIt = animations.find( name );

		if ( animIt == animations.end() )
		{
			return false;
		}

		SkeletonAnimationKeyFrameMap keyframes;
		SkeletonAnimationObjectSet notAnimated;
		anims::processSkeletonAnimationNodes( file
			, animIt->second
			, animation
			, skeleton
			, keyframes
			, notAnimated );

		if ( !keyframes.empty() )
		{
			for ( auto & object : notAnimated )
			{
				auto & objTransform = object->getNodeTransform();

				for ( auto & [time, keyframe] : keyframes )
				{
					auto kfit = keyframe->find( *object );

					if ( kfit == keyframe->end() )
					{
						keyframe->addAnimationObject( *object
							, objTransform.translate
							, objTransform.rotate
							, objTransform.scale );
					}
					else
					{
						kfit->transform.translate = objTransform.translate;
						kfit->transform.rotate = objTransform.rotate;
						kfit->transform.scale = objTransform.scale;
					}
				}
			}

			for ( auto & [time, keyframe] : keyframes )
			{
				animation.addKeyFrame( c3d::ptrRefCast< c3d::AnimationKeyFrame >( keyframe ) );
			}
		}

		return !keyframes.empty();
	}

	bool GltfAnimationImporter::doImportMesh( c3d::MeshAnimation & animation )
	{
		auto & file = static_cast< GltfImporterFile & >( *m_file );
		auto & impAsset = file.getAsset();
		auto name = animation.getName();
		auto & mesh = static_cast< c3d::Mesh const & >( *animation.getAnimable() );
		bool hasAnyKeyframes = false;

		for ( auto & submesh : mesh )
		{
			auto index = submesh->getId();
			auto animations = file.getMeshAnimations( mesh, index );
			auto animIt = animations.find( name );

			if ( animIt != animations.end()
				&& submesh->hasMorphComponent() )
			{
				c3d::MeshAnimationSubmesh animSubmesh{ animation, *submesh };
				auto & animChannels = animIt->second;
				size_t nodeIndex = anims::getMeshNodeIndex( file, animChannels, mesh.getName(), index );
				auto impNodeAnim = anims::findNodeAnim( animChannels, nodeIndex );
				bool hasKeyframes = false;

				for ( AnimationChannelSampler & channelSampler : impNodeAnim )
				{
					c3d::Vector< float > times;
					iterateAccessor< float >( impAsset
						, impAsset.accessors[channelSampler.second.inputAccessor]
						, [&times]( float value )
						{
							times.push_back( value );
						}
						, file.getAdapter() );
					c3d::Vector< float > values;
					iterateAccessor< float >( impAsset
						, impAsset.accessors[channelSampler.second.outputAccessor]
						, [&values]( float value )
						{
							values.push_back( value );
						}
						, file.getAdapter() );

					// for AnimationInterpolation::CubicSpline can have more outputs
					uint32_t weightStride = uint32_t( values.size() / times.size() );
					uint32_t numMorphs = ( channelSampler.second.interpolation == fastgltf::AnimationInterpolation::CubicSpline )
						? weightStride - 2
						: weightStride;
					uint32_t ii = ( channelSampler.second.interpolation == fastgltf::AnimationInterpolation::CubicSpline )
						? 1u
						: 0u;

					if ( !times.empty()
						&& submesh->getMorphTargetsCount() >= numMorphs )
					{
						hasKeyframes = true;

						for ( uint32_t i = 0u; i < uint32_t( times.size() ); ++i )
						{
							auto timeIndex = c3d::Milliseconds{ uint64_t( times[i] * 1000u ) };
							auto kfit = animation.find( timeIndex );
							c3d::MeshMorphTarget * kf{};

							if ( kfit == animation.end() )
							{
								auto keyFrame = c3d::makeUnique< c3d::MeshMorphTarget >( animation, timeIndex );
								kf = keyFrame.get();
								animation.addKeyFrame( c3d::ptrRefCast< c3d::AnimationKeyFrame >( keyFrame ) );
							}
							else
							{
								kf = &static_cast< c3d::MeshMorphTarget & >( **kfit );
							}

							c3d::Vector< float > res;
							res.resize( submesh->getMorphTargetsCount() );
							uint32_t k = weightStride * i + ii;

							for ( uint32_t value = 0u; value < numMorphs; ++value, ++k )
							{
								res[value] = ( 0.f > values[k] ) ? 0.f : values[k];
							}

							kf->setTargetsWeights( *submesh, res );
						}
					}
				}

				if ( hasKeyframes )
				{
					hasAnyKeyframes = true;
					animation.addChild( c3d::move( animSubmesh ) );
				}
			}
		}

		return hasAnyKeyframes;
	}

	bool GltfAnimationImporter::doImportNode( c3d::SceneNodeAnimation & animation )
	{
		auto & file = static_cast< GltfImporterFile & >( *m_file );
		auto name = animation.getName();
		auto & node = static_cast< c3d::SceneNode const & >( *animation.getAnimable() );
		auto animations = file.getNodeAnimations( node );
		auto animIt = animations.find( name );

		if ( animIt == animations.end() )
		{
			return false;
		}

		auto & impAsset = file.getAsset();
		auto nodeName = node.getName();
		auto nodeIndex = file.getNodeIndex( nodeName );
		auto impNodeAnim = anims::findNodeAnim( animIt->second, nodeIndex );
		c3d::Milliseconds minTime{ std::numeric_limits< int32_t >::max() };
		c3d::Milliseconds maxTime{};
		c3d::Set< c3d::Milliseconds > allTimes;
		anims::processAnimationNodeKeysTimes( impAsset
			, impNodeAnim
			, minTime
			, maxTime
			, allTimes
			, file.getAdapter() );
		SceneNodeAnimationKeyFrameMap keyFrames;
		anims::processAnimationNodeKeys( impAsset
			, impNodeAnim
			, file.getEngine()->getWantedFps()
			, animation
			, keyFrames
			, minTime
			, maxTime
			, allTimes
			, { node.getPosition(), node.getScale(), node.getOrientation() }
			, []( c3d::SceneNodeAnimationKeyFrame & keyframe
				, c3d::Point3f const & position
				, c3d::Quaternion const & orientation
				, c3d::Point3f const & scale )
			{
				keyframe.setTransform( position, orientation, scale );
			}
			, file.getAdapter() );

		if ( !keyFrames.empty() )
		{
			for ( auto & keyFrame : keyFrames )
			{
				animation.addKeyFrame( c3d::ptrRefCast< c3d::AnimationKeyFrame >( keyFrame.second ) );
			}
		}

		return !keyFrames.empty();
	}

	bool GltfAnimationImporter::doImportTexture( c3d::TextureAnimation & animation )
	{
		return true;
	}
}
