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
		struct KeyDataTyperT< castor::Point3f >
		{
			using Type = castor::Point3f;
		};

		template<>
		struct KeyDataTyperT< castor::Quaternion >
		{
			using Type = castor::Point4f;
		};

		template< typename KeyT >
		using KeyDataTypeT = typename KeyDataTyperT< KeyT >::Type;

		template< typename KeyFrameT, typename AnimationT >
		static KeyFrameT & getKeyFrame( castor::Milliseconds const & time
			, AnimationT & animation
			, castor::Map< castor::Milliseconds, castor::UniquePtr< KeyFrameT > > & keyframes )
		{
			auto it = keyframes.find( time );

			if ( it == keyframes.end() )
			{
				it = keyframes.emplace( time
					, castor::makeUnique< KeyFrameT >( animation, time ) ).first;
			}

			return *it->second;
		}

		template< typename T >
		static void findValue( castor::Milliseconds time
			, typename castor::Map< castor::Milliseconds, T > const & map
			, typename castor::Map< castor::Milliseconds, T >::const_iterator & prv
			, typename castor::Map< castor::Milliseconds, T >::const_iterator & cur )
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
					, [&time]( castor::Pair< castor::Milliseconds, T > const & pair )
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
		static T interpolate( castor::Milliseconds const & time
			, castor3d::Interpolator< T > const & interpolator
			, castor::Map< castor::Milliseconds, T > const & values
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
		static void synchroniseKeys( castor::Map< castor::Milliseconds, castor::Point3f > const & translates
			, castor::Map< castor::Milliseconds, castor::Quaternion > const & rotates
			, castor::Map< castor::Milliseconds, castor::Point3f > const & scales
			, [[maybe_unused]] castor::Set< castor::Milliseconds > const & times
			, uint32_t fps
			, castor::Milliseconds minTime
			, castor::Milliseconds maxTime
			, AnimationT & animation
			, castor::Map< castor::Milliseconds, castor::UniquePtr< KeyFrameT > > & keyframes
			, castor3d::NodeTransform const & defaultTransform
			, FuncT fillKeyFrame )
		{
			castor3d::InterpolatorT< castor::Point3f, castor3d::InterpolatorType::eLinear > pointInterpolator;
			castor3d::InterpolatorT< castor::Quaternion, castor3d::InterpolatorType::eLinear > quatInterpolator;

			// Limit the key frames per second to 60, to spare RAM...
			auto wantedFps = std::min< int64_t >( 60, int64_t( fps ) );
			castor::Milliseconds step{ 1000 / wantedFps };

			for ( auto time = minTime; time <= maxTime; time += step )
			{
				auto translate = interpolate( time, pointInterpolator, translates, defaultTransform.translate );
				auto rotate = interpolate( time, quatInterpolator, rotates, defaultTransform.rotate );
				auto scale = interpolate( time, pointInterpolator, scales, defaultTransform.scale );
				fillKeyFrame( getKeyFrame( time - minTime, animation, keyframes )
					, translate
					, rotate
					, scale );
			}
		}

		template< typename KeyT >
		static std::pair< castor::Milliseconds, castor::Milliseconds> processKeys( fastgltf::Asset const & impAsset
			, NodeAnimationChannelSampler const & animChannels
			, fastgltf::AnimationPath channel
			, castor::Map< castor::Milliseconds, KeyT > & result
			, castor::Set< castor::Milliseconds > & allTimes
			, CompressedBufferDataAdapter const & adapter )
		{
			auto it = std::find_if( animChannels.begin()
				, animChannels.end()
				, [channel]( AnimationChannelSampler const & lookup )
				{
					return lookup.first.path == channel;
				} );
			castor::Milliseconds minTime{ std::numeric_limits< int32_t >::max() };
			castor::Milliseconds maxTime{};

			if ( it != animChannels.end() )
			{
				AnimationChannelSampler const & channelSampler = *it;
				castor::Vector< float > times;
				iterateAccessor< float >( impAsset
					, impAsset.accessors[channelSampler.second.inputAccessor]
					, [&times]( float value )
					{
						times.push_back( value );
					}
					, adapter );
				castor::Vector< KeyT > values;
				iterateAccessor< KeyDataTypeT< KeyT > >( impAsset
					, impAsset.accessors[channelSampler.second.outputAccessor]
					, [&values]( KeyDataTypeT< KeyT > value )
					{
						values.push_back( KeyT{ castor::move( value ) } );
					}
					, adapter );
				// for AnimationInterpolation::CubicSpline can have more outputs
				uint32_t weightStride = uint32_t( values.size() / times.size() );
				uint32_t ii = ( channelSampler.second.interpolation == fastgltf::AnimationInterpolation::CubicSpline )
					? 1u
					: 0u;

				for ( uint32_t i = 0u; i < uint32_t( times.size() ); ++i )
				{
					auto timeIndex = castor::Milliseconds{ uint64_t( times[i] * 1000u ) };
					maxTime = std::max( maxTime, timeIndex );
					allTimes.insert( timeIndex );
					uint32_t k = weightStride * i + ii;
					result.emplace( timeIndex, values[k] );
				}

				minTime = result.begin()->first;
			}

			return { minTime, maxTime };
		}

		template< typename AnimationT
			, typename KeyFrameT
			, typename FuncT >
		static void processAnimationNodeKeys( fastgltf::Asset const & impAsset
			, NodeAnimationChannelSampler const & animChannels
			, uint32_t wantedFps
			, AnimationT & animation
			, castor::Map< castor::Milliseconds, castor::UniquePtr< KeyFrameT > > & keyframes
			, castor3d::NodeTransform const & defaultTransform
			, FuncT fillKeyFrame
			, CompressedBufferDataAdapter const & adapter )
		{
			castor::Set< castor::Milliseconds > times;
			castor::Map< castor::Milliseconds, castor::Point3f > translates;
			castor::Map< castor::Milliseconds, castor::Quaternion > rotates;
			castor::Map< castor::Milliseconds, castor::Point3f > scales;
			auto [minTranslateTime, maxTranslateTime] = processKeys( impAsset, animChannels, fastgltf::AnimationPath::Translation, translates, times, adapter );
			auto [minRotateTime, maxRotateTime] = processKeys( impAsset, animChannels, fastgltf::AnimationPath::Rotation, rotates, times, adapter );
			auto [minScaleTime, maxScaleTime] = processKeys( impAsset, animChannels, fastgltf::AnimationPath::Scale, scales, times, adapter );
			synchroniseKeys( translates
				, rotates
				, scales
				, times
				, wantedFps
				, std::min( minTranslateTime, std::min( minRotateTime, minScaleTime ) )
				, std::max( maxTranslateTime, std::max( maxRotateTime, maxScaleTime ) )
				, animation
				, keyframes
				, defaultTransform
				, fillKeyFrame );
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
			, castor3d::SkeletonAnimation & animation
			, castor3d::Skeleton const & skeleton
			, SkeletonAnimationKeyFrameMap & keyFrames
			, SkeletonAnimationObjectSet & notAnimated )
		{
			auto & impAsset = file.getAsset();

			for ( auto & skelNode : skeleton.getNodes() )
			{
				auto name = skelNode->getName();
				auto nodeIndex = file.getSkeletonNodeIndex( name );
				auto impNodeAnim = findNodeAnim( animChannels, nodeIndex );
				auto parentSkelNode = skelNode->getParent();
				castor3d::SkeletonAnimationObjectRPtr parent{};

				if ( parentSkelNode )
				{
					parent = animation.getObject( parentSkelNode->getType()
						, parentSkelNode->getName() );
					CU_Require( parent );
				}

				castor3d::SkeletonAnimationObjectRPtr object{};
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

				if ( !impNodeAnim.empty() )
				{
					processAnimationNodeKeys( impAsset
						, impNodeAnim
						, file.getEngine()->getWantedFps()
						, animation
						, keyFrames
						, object->getNodeTransform()
						, [&object]( castor3d::SkeletonAnimationKeyFrame & keyframe
							, castor::Point3f const & position
							, castor::Quaternion const & orientation
							, castor::Point3f const & scale )
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
			, castor::String const & name
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

	using SceneNodeAnimationKeyFrameMap = castor::Map< castor::Milliseconds, castor3d::SceneNodeAnimationKeyFrameUPtr >;

	GltfAnimationImporter::GltfAnimationImporter( castor3d::Engine & engine )
		: castor3d::AnimationImporter{ engine, cuT( "Gltf" ) }
	{
	}

	bool GltfAnimationImporter::doImportSkeleton( castor3d::SkeletonAnimation & animation )
	{
		auto & file = static_cast< GltfImporterFile & >( *m_file );
		auto name = animation.getName();
		auto & skeleton = static_cast< castor3d::Skeleton const & >( *animation.getAnimable() );
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

		for ( auto & object : notAnimated )
		{
			auto & objTransform = object->getNodeTransform();

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
					kfit->transform.translate = objTransform.translate;
					kfit->transform.rotate = objTransform.rotate;
					kfit->transform.scale = objTransform.scale;
				}
			}
		}

		for ( auto & keyFrame : keyframes )
		{
			animation.addKeyFrame( castor::ptrRefCast< castor3d::AnimationKeyFrame >( keyFrame.second ) );
		}

		return keyframes.size() > 1u;
	}

	bool GltfAnimationImporter::doImportMesh( castor3d::MeshAnimation & animation )
	{
		auto & file = static_cast< GltfImporterFile & >( *m_file );
		auto & impAsset = file.getAsset();
		auto name = animation.getName();
		auto & mesh = static_cast< castor3d::Mesh const & >( *animation.getAnimable() );
		bool hasAnyKeyframes = false;

		for ( auto & submesh : mesh )
		{
			auto index = submesh->getId();
			auto animations = file.getMeshAnimations( mesh, index );
			auto animIt = animations.find( name );

			if ( animIt != animations.end() )
			{
				castor3d::MeshAnimationSubmesh animSubmesh{ animation, * submesh };
				auto & animChannels = animIt->second;
				size_t nodeIndex = anims::getMeshNodeIndex( file, animChannels, mesh.getName(), index );
				auto impNodeAnim = anims::findNodeAnim( animChannels, nodeIndex );
				bool hasKeyframes = false;

				for ( AnimationChannelSampler & channelSampler : impNodeAnim )
				{
					castor::Vector< float > times;
					iterateAccessor< float >( impAsset
						, impAsset.accessors[channelSampler.second.inputAccessor]
						, [&times]( float value )
						{
							times.push_back( value );
						}
						, file.getAdapter() );
					castor::Vector< float > values;
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

					if ( !times.empty() && times.back() > 0 )
					{
						hasKeyframes = true;

						for ( uint32_t i = 0u; i < uint32_t( times.size() ); ++i )
						{
							auto timeIndex = castor::Milliseconds{ uint64_t( times[i] * 1000u ) };
							auto kfit = animation.find( timeIndex );
							castor3d::MeshMorphTarget * kf{};

							if ( kfit == animation.end() )
							{
								auto keyFrame = castor::makeUnique< castor3d::MeshMorphTarget >( animation, timeIndex );
								kf = keyFrame.get();
								animation.addKeyFrame( castor::ptrRefCast< castor3d::AnimationKeyFrame >( keyFrame ) );
							}
							else
							{
								kf = &static_cast< castor3d::MeshMorphTarget & >( **kfit );
							}

							castor::Vector< float > res;
							res.resize( submesh->getMorphTargetsCount() );
							uint32_t k = weightStride * i + ii;
							CU_Require( numMorphs <= submesh->getMorphTargetsCount() );

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
					animation.addChild( castor::move( animSubmesh ) );
				}
			}
		}

		return hasAnyKeyframes;
	}

	bool GltfAnimationImporter::doImportNode( castor3d::SceneNodeAnimation & animation )
	{
		auto & file = static_cast< GltfImporterFile & >( *m_file );
		auto name = animation.getName();
		auto & node = static_cast< castor3d::SceneNode const & >( *animation.getAnimable() );
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
		SceneNodeAnimationKeyFrameMap keyFrames;
		anims::processAnimationNodeKeys( impAsset
			, impNodeAnim
			, file.getEngine()->getWantedFps()
			, animation
			, keyFrames
			, { node.getPosition(), node.getScale(), node.getOrientation() }
			, []( castor3d::SceneNodeAnimationKeyFrame & keyframe
				, castor::Point3f const & position
				, castor::Quaternion const & orientation
				, castor::Point3f const & scale )
			{
				keyframe.setTransform( position, orientation, scale );
			}
			, file.getAdapter() );

		for ( auto & keyFrame : keyFrames )
		{
			animation.addKeyFrame( castor::ptrRefCast< castor3d::AnimationKeyFrame >( keyFrame.second ) );
		}

		return keyFrames.size() > 1u;
	}
}
