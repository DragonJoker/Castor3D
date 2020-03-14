#include "Castor3DTestCommon.hpp"

#include <Castor3D/Animation/Animable.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Cache/AnimatedObjectGroupCache.hpp>
#include <Castor3D/Cache/CameraCache.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/LightCache.hpp>
#include <Castor3D/Cache/SceneNodeCache.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BonesComponent.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Scene/Animation/AnimatedObject.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#include <Castor3D/Scene/Animation/AnimatedSkeleton.hpp>
#include <Castor3D/Scene/Animation/AnimationInstance.hpp>
#include <Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp>
#include <Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

#include <cmath>

using namespace castor;
using namespace castor3d;

using castor::operator<<;
using castor3d::operator<<;

namespace Testing
{
	C3DTestCase::C3DTestCase( std::string const & name
		, castor3d::Engine & engine )
		: TestCase{ name }
		, m_engine{ engine }
		, m_testDataFolder{ Engine::getDataDirectory() / cuT( "Castor3DTest" ) / cuT( "data" ) }
	{
	}

	void C3DTestCase::doCleanupEngine()
	{
		m_engine.cleanup();
		m_engine.initialise( 1, false );
	}

	bool C3DTestCase::compare( Angle const & lhs, Angle const & rhs )
	{
		return lhs.radians() == rhs.radians();
	}

	bool C3DTestCase::compare( castor::Quaternion const & lhs, castor::Quaternion const & rhs )
	{
		castor::Angle alphaA;
		castor::Angle alphaB;
		castor::Point3f axisA;
		castor::Point3f axisB;
		lhs.toAxisAngle( axisA, alphaA );
		rhs.toAxisAngle( axisB, alphaB );
		auto result = CT_EQUAL( axisA, axisB );
		return result && CT_EQUAL( alphaA, alphaB );
	}

	bool C3DTestCase::compare( Scene const & lhs, Scene const & rhs )
	{
		auto lockA = castor::makeUniqueLock( lhs.getSceneNodeCache() );
		auto lockB = castor::makeUniqueLock( rhs.getSceneNodeCache() );
		auto itA = lhs.getSceneNodeCache().begin();
		auto endItA =  lhs.getSceneNodeCache().end();
		auto itB = rhs.getSceneNodeCache().begin();
		auto endItB = rhs.getSceneNodeCache().end();
		bool result = true;

		while ( result && itA != endItA && itB != endItB )
		{
			if ( result
					&& itA->first.find( cuT( "_REye" ) ) == String::npos
					&& itA->first.find( cuT( "_LEye" ) ) == String::npos )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result = result && CT_EQUAL( *itA->second, *itB->second );
			}

			++itA;
			++itB;
		}

		if ( result )
		{
			auto lockA = castor::makeUniqueLock( lhs.getGeometryCache() );
			auto lockB = castor::makeUniqueLock( rhs.getGeometryCache() );
			auto itA = lhs.getGeometryCache().begin();
			auto endItA =  lhs.getGeometryCache().end();
			auto itB = rhs.getGeometryCache().begin();
			auto endItB = rhs.getGeometryCache().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result = result && CT_EQUAL( *itA->second, *itB->second );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			auto lockA = castor::makeUniqueLock( lhs.getLightCache() );
			auto lockB = castor::makeUniqueLock( rhs.getLightCache() );
			auto itA = lhs.getLightCache().begin();
			auto endItA =  lhs.getLightCache().end();
			auto itB = rhs.getLightCache().begin();
			auto endItB = rhs.getLightCache().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result = result && CT_EQUAL( *itA->second, *itB->second );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			auto lockA = castor::makeUniqueLock( lhs.getCameraCache() );
			auto lockB = castor::makeUniqueLock( rhs.getCameraCache() );
			auto itA = lhs.getCameraCache().begin();
			auto endItA =  lhs.getCameraCache().end();
			auto itB = rhs.getCameraCache().begin();
			auto endItB = rhs.getCameraCache().end();

			while ( result && itA != endItA && itB != endItB )
			{
				if ( result
					 && itA->first.find( cuT( "_REye" ) ) == String::npos
					 && itA->first.find( cuT( "_LEye" ) ) == String::npos )
				{
					result = CT_EQUAL( itA->first, itB->first );
					result = result && CT_EQUAL( *itA->second, *itB->second );
				}

				++itA;
				++itB;
			}
		}

		if ( result )
		{
			auto lockA = castor::makeUniqueLock( lhs.getAnimatedObjectGroupCache() );
			auto lockB = castor::makeUniqueLock( rhs.getAnimatedObjectGroupCache() );
			auto itA = lhs.getAnimatedObjectGroupCache().begin();
			auto endItA =  lhs.getAnimatedObjectGroupCache().end();
			auto itB = rhs.getAnimatedObjectGroupCache().begin();
			auto endItB = rhs.getAnimatedObjectGroupCache().end();

			while ( result && itA != endItA && itB != endItB )
			{
				if ( result )
				{
					result = CT_EQUAL( itA->first, itB->first );
					result = result && CT_EQUAL( *itA->second, *itB->second );
				}

				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( SceneNode const & lhs, SceneNode const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		result = result && CT_EQUAL( lhs.getOrientation(), rhs.getOrientation() );
		result = result && CT_EQUAL( lhs.getPosition(), rhs.getPosition() );
		result = result && CT_EQUAL( lhs.getScale(), rhs.getScale() );
		result = result && CT_EQUAL( lhs.getParent() != nullptr, rhs.getParent() != nullptr );

		if ( result && lhs.getParent() && rhs.getParent() )
		{
			result = result && CT_EQUAL( lhs.getParent()->getName(), rhs.getParent()->getName() );
		}

		return result;
	}

	bool C3DTestCase::compare( Animable const & lhs, Animable const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getAnimations().size(), rhs.getAnimations().size() ) };
		auto itA = lhs.getAnimations().begin();
		auto const endItA = lhs.getAnimations().end();
		auto itB = rhs.getAnimations().begin();
		auto const endItB = rhs.getAnimations().end();

		while ( result && itA != endItA && itB != endItB )
		{
			result = CT_EQUAL( itA->first, itB->first );
			result = result && CT_EQUAL( *itA->second, *itB->second );
			++itA;
			++itB;
		}

		return result;
	}

	bool C3DTestCase::compare( MovableObject const & lhs, MovableObject const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		result = result && CT_EQUAL( lhs.getParent()->getName(), rhs.getParent()->getName() );
		result = result && CT_EQUAL( static_cast< Animable const & >( lhs ), static_cast< Animable const & >( rhs ) );
		return result;
	}

	bool C3DTestCase::compare( Light const & lhs, Light const & rhs )
	{
		bool result{ CT_EQUAL( static_cast< MovableObject const & >( lhs ), static_cast< MovableObject const & >( rhs ) ) };
		result = result && CT_EQUAL( *lhs.getCategory(), *rhs.getCategory() );
		return result;
	}

	bool C3DTestCase::compare( Geometry const & lhs, Geometry const & rhs )
	{
		bool result{ CT_EQUAL( static_cast< MovableObject const & >( lhs ), static_cast< MovableObject const & >( rhs ) ) };
		result = result && CT_EQUAL( *lhs.getMesh(), *rhs.getMesh() );
		return result;
	}

	bool C3DTestCase::compare( Camera const & lhs, Camera const & rhs )
	{
		bool result{ CT_EQUAL( static_cast< MovableObject const & >( lhs ), static_cast< MovableObject const & >( rhs ) ) };
		result = result && CT_EQUAL( lhs.getViewport(), rhs.getViewport() );
		return result;
	}

	bool C3DTestCase::compare( LightCategory const & lhs, LightCategory const & rhs )
	{
		CT_REQUIRE( lhs.getLightType() == rhs.getLightType() );
		bool result{ CT_EQUAL( lhs.getIntensity(), rhs.getIntensity() ) };
		result = result && CT_EQUAL( lhs.getColour(), rhs.getColour() );

		switch ( lhs.getLightType() )
		{
		case LightType::eDirectional:
			result = result && CT_EQUAL( static_cast< DirectionalLight const & >( lhs ), static_cast< DirectionalLight const & >( rhs ) );
			break;

		case LightType::ePoint:
			result = result && CT_EQUAL( static_cast< PointLight const & >( lhs ), static_cast< PointLight const & >( rhs ) );
			break;

		case LightType::eSpot:
			result = result && CT_EQUAL( static_cast< SpotLight const & >( lhs ), static_cast< SpotLight const & >( rhs ) );
			break;
		}

		return result;
	}

	bool C3DTestCase::compare( DirectionalLight const & lhs, DirectionalLight const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getDirection(), rhs.getDirection() ) };
		return result;
	}

	bool C3DTestCase::compare( PointLight const & lhs, PointLight const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getAttenuation(), rhs.getAttenuation() ) };
		return result;
	}

	bool C3DTestCase::compare( SpotLight const & lhs, SpotLight const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getExponent(), rhs.getExponent() ) };
		result = result && CT_EQUAL( lhs.getAttenuation(), rhs.getAttenuation() );
		result = result && CT_EQUAL( lhs.getCutOff(), rhs.getCutOff() );
		return result;
	}

	bool C3DTestCase::compare( Viewport const & lhs, Viewport const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getLeft(), rhs.getLeft() ) };
		result = result && CT_EQUAL( lhs.getRight(), rhs.getRight() );
		result = result && CT_EQUAL( lhs.getTop(), rhs.getTop() );
		result = result && CT_EQUAL( lhs.getBottom(), rhs.getBottom() );
		result = result && CT_EQUAL( lhs.getNear(), rhs.getNear() );
		result = result && CT_EQUAL( lhs.getFar(), rhs.getFar() );
		result = result && CT_EQUAL( lhs.getFovY(), rhs.getFovY() );
		result = result && CT_EQUAL( lhs.getRatio(), rhs.getRatio() );
		result = result && CT_EQUAL( lhs.getWidth(), rhs.getWidth() );
		result = result && CT_EQUAL( lhs.getHeight(), rhs.getHeight() );
		result = result && CT_EQUAL( lhs.getType(), rhs.getType() );
		return result;
	}

	bool C3DTestCase::compare( Mesh const & lhs, Mesh const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		result = result && CT_EQUAL( lhs.getSubmeshCount(), rhs.getSubmeshCount() );
		auto itA = lhs.begin();
		auto const endItA = lhs.end();
		auto itB = rhs.begin();
		auto const endItB = rhs.end();

		while ( result && itA != endItA && itB != endItB )
		{
			result = CT_EQUAL( *( *( itA ) ), *( *( itB ) ) );
			++itA;
			++itB;
		}

		result = result && CT_EQUAL( lhs.getSkeleton() != nullptr, rhs.getSkeleton() != nullptr );

		if ( result && lhs.getSkeleton() )
		{
			result = CT_EQUAL( *lhs.getSkeleton(), *rhs.getSkeleton() );
		}

		return result;
	}

	bool C3DTestCase::compare( BonesComponent const & lhs, BonesComponent const & rhs )
	{
		auto * lhsData = lhs.getBonesBuffer().lock( 0u, lhs.getBonesBuffer().getCount(), 0u );
		auto * rhsData = rhs.getBonesBuffer().lock( 0u, rhs.getBonesBuffer().getCount(), 0u );
		auto result = CT_EQUAL( castor::makeArrayView( lhsData, lhs.getBonesBuffer().getCount() )
			, castor::makeArrayView( rhsData, rhs.getBonesBuffer().getCount() ) );
		lhs.getBonesBuffer().unlock();
		rhs.getBonesBuffer().unlock();
		return result;
	}

	bool C3DTestCase::compare( SubmeshComponent const & lhs, SubmeshComponent const & rhs )
	{
		bool result = CT_EQUAL( lhs.getType(), rhs.getType() );
		//result = result && CT_EQUAL( lhs.getProgramFlags(), rhs.getProgramFlags() );

		if ( result && lhs.getType() == BonesComponent::Name )
		{
			result = result && CT_EQUAL( static_cast< BonesComponent const & >( lhs ), static_cast< BonesComponent const & >( rhs ) );
		}

		return result;
	}

	bool C3DTestCase::compare( Submesh const & lhs, Submesh const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getPointsCount(), rhs.getPointsCount() ) };
		auto * lhsVtx = lhs.getVertexBuffer().lock( 0u, lhs.getVertexBuffer().getCount(), 0u );
		auto * rhsVtx = rhs.getVertexBuffer().lock( 0u, rhs.getVertexBuffer().getCount(), 0u );
		result = result && CT_EQUAL( castor::makeArrayView( lhsVtx, lhs.getVertexBuffer().getCount() )
							  , castor::makeArrayView( rhsVtx, rhs.getVertexBuffer().getCount() ) );
		lhs.getVertexBuffer().unlock();
		rhs.getVertexBuffer().unlock();
		auto * lhsIdx = lhs.getIndexBuffer().lock( 0u, lhs.getIndexBuffer().getCount(), 0u );
		auto * rhsIdx = rhs.getIndexBuffer().lock( 0u, rhs.getIndexBuffer().getCount(), 0u );
		result = result && CT_EQUAL( castor::makeArrayView( lhsIdx, lhs.getIndexBuffer().getCount() )
								, castor::makeArrayView( rhsIdx, rhs.getIndexBuffer().getCount() ) );
		lhs.getIndexBuffer().unlock();
		rhs.getIndexBuffer().unlock();

		if ( result )
		{
			result = result && CT_EQUAL( lhs.getComponents().size(), rhs.getComponents().size() );

			for ( auto & itA : lhs.getComponents() )
			{
				if ( result )
				{
					auto itB = rhs.getComponents().find( itA.first );
					result = CT_CHECK( itB != rhs.getComponents().end() );
					result = result && CT_EQUAL( *itA.second, *itB->second );
				}
			}
		}

		return result;
	}

	bool C3DTestCase::compare( Skeleton const & lhs, Skeleton const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getGlobalInverseTransform(), rhs.getGlobalInverseTransform() ) };

		if ( result )
		{
			result = CT_EQUAL( lhs.getBonesCount(), rhs.getBonesCount() );
			auto itA = lhs.begin();
			auto const endItA = lhs.end();
			auto itB = rhs.begin();
			auto const endItB = rhs.end();

			while ( result && itA != endItA && itB != endItB )
			{
				auto boneA = *itA;
				auto boneB = *itB;
				CT_REQUIRE( boneA != nullptr && boneB != nullptr );
				result = CT_EQUAL( *boneA, *boneB );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			result = result && CT_EQUAL( static_cast< Animable const & >( lhs ), static_cast< Animable const & >( rhs ) );
		}

		return result;
	}

	bool C3DTestCase::compare( Bone const & lhs, Bone const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		result = result && CT_EQUAL( lhs.getParent() == nullptr, rhs.getParent() == nullptr );

		if ( result && lhs.getParent() )
		{
			result = CT_EQUAL( lhs.getParent()->getName(), rhs.getParent()->getName() );
		}

		result = result && CT_EQUAL( lhs.getOffsetMatrix(), rhs.getOffsetMatrix() );
		return result;
	}

	bool C3DTestCase::compare( Animation const & lhs, Animation const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		CT_REQUIRE( lhs.getType() == rhs.getType() );

		if ( lhs.getType() == AnimationType::eSkeleton )
		{
			result = CT_EQUAL( static_cast< SkeletonAnimation const & >( lhs ), static_cast< SkeletonAnimation const & >( rhs ) );
		}

		return result;
	}

	bool C3DTestCase::compare( SkeletonAnimation const & lhs, SkeletonAnimation const & rhs )
	{
		bool result = CT_EQUAL( lhs.getObjects().size(), rhs.getObjects().size() );

		if ( result )
		{
			auto itA = lhs.getObjects().begin();
			auto const endItA = lhs.getObjects().end();
			auto itB = rhs.getObjects().begin();
			auto const endItB = rhs.getObjects().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result = result && CT_EQUAL( *itA->second, *itB->second );
				++itA;
				++itB;
			}
		}

		result = result && CT_EQUAL( lhs.getRootObjects().size(), rhs.getRootObjects().size() );

		if ( result )
		{
			auto itA = lhs.getRootObjects().begin();
			auto const endItA = lhs.getRootObjects().end();
			auto itB = rhs.getRootObjects().begin();
			auto const endItB = rhs.getRootObjects().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( *( *itA ), *( *itB ) );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			auto itA = lhs.begin();
			auto const endItA = lhs.end();
			auto itB = rhs.begin();
			auto const endItB = rhs.end();
			result = CT_EQUAL( std::distance( itA, endItA ), std::distance( itB, endItB ) );

			while ( result && itA != endItA )
			{
				result = CT_EQUAL( static_cast< SkeletonAnimationKeyFrame const & >( **itA )
					, static_cast< SkeletonAnimationKeyFrame const & >( **itB ) );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( SkeletonAnimationObject const & lhs, SkeletonAnimationObject const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		result = result && CT_EQUAL( lhs.getInterpolationMode(), rhs.getInterpolationMode() );
		result = result && CT_EQUAL( lhs.getNodeTransform(), rhs.getNodeTransform() );
		result = result && CT_EQUAL( lhs.getType(), rhs.getType() );
		result = result && CT_EQUAL( lhs.getParent() == nullptr, rhs.getParent() == nullptr );

		if ( result && lhs.getParent() )
		{
			result = CT_EQUAL( lhs.getParent()->getName(), rhs.getParent()->getName() );
		}

		if ( result )
		{
			result = CT_EQUAL( lhs.getChildren().size(), rhs.getChildren().size() );
			auto itA = lhs.getChildren().begin();
			auto const endItA = lhs.getChildren().end();
			auto itB = rhs.getChildren().begin();
			auto const endItB = rhs.getChildren().end();

			while ( result && itA != endItA && itB != endItB )
			{
				auto objectA = *itA;
				auto objectB = *itB;
				CT_REQUIRE( objectA->getType() == objectB->getType() );
				result = CT_EQUAL( *objectA, *objectB );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( AnimationKeyFrame const & lhs, AnimationKeyFrame const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getTimeIndex(), rhs.getTimeIndex() ) };
		return result;
	}

	bool C3DTestCase::compare( castor3d::MeshAnimationKeyFrame const & lhs, castor3d::MeshAnimationKeyFrame const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getTimeIndex(), rhs.getTimeIndex() ) };

		if ( result )
		{
			auto itA = lhs.begin();
			auto const endItA = lhs.end();
			auto itB = rhs.begin();
			auto const endItB = rhs.end();
			result = CT_EQUAL( std::distance( itA, endItA ), std::distance( itB, endItB ) );

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				//result = result && CT_EQUAL( itA->second, itB->second );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::SkeletonAnimationKeyFrame const & lhs, castor3d::SkeletonAnimationKeyFrame const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getTimeIndex(), rhs.getTimeIndex() ) };

		if ( result )
		{
			auto itA = lhs.begin();
			auto const endItA = lhs.end();
			auto itB = rhs.begin();
			auto const endItB = rhs.end();
			result = CT_EQUAL( std::distance( itA, endItA ), std::distance( itB, endItB ) );

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first->getName(), itB->first->getName() );
				result = result && CT_EQUAL( itA->second, itB->second );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( AnimatedObjectGroup const & lhs, AnimatedObjectGroup const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };

		if ( result )
		{
			result = CT_EQUAL( lhs.getAnimations().size(), rhs.getAnimations().size() );
			auto itA = lhs.getAnimations().begin();
			auto const endItA = lhs.getAnimations().end();
			auto itB = rhs.getAnimations().begin();
			auto const endItB = rhs.getAnimations().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result = result && CT_EQUAL( itA->second.m_state, itB->second.m_state );
				result = result && CT_EQUAL( itA->second.m_scale, itB->second.m_scale );
				result = result && CT_EQUAL( itA->second.m_looped, itB->second.m_looped );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			result = CT_EQUAL( lhs.getObjects().size(), rhs.getObjects().size() );
			auto itA = lhs.getObjects().begin();
			auto const endItA = lhs.getObjects().end();
			auto itB = rhs.getObjects().begin();
			auto const endItB = rhs.getObjects().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result = result && CT_EQUAL( *itA->second, *itB->second );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( AnimatedObject const & lhs, AnimatedObject const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };

		if ( result )
		{
			result = CT_EQUAL( lhs.getAnimations().size(), rhs.getAnimations().size() );
			auto itA = lhs.getAnimations().begin();
			auto const endItA = lhs.getAnimations().end();
			auto itB = rhs.getAnimations().begin();
			auto const endItB = rhs.getAnimations().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result = result && CT_EQUAL( *itA->second, *itB->second );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( AnimationInstance const & lhs, AnimationInstance const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getAnimation().getName(), rhs.getAnimation().getName() ) };
		result = result && CT_EQUAL( lhs.getScale(), rhs.getScale() );
		result = result && CT_EQUAL( lhs.getState(), rhs.getState() );
		result = result && CT_EQUAL( lhs.isLooped(), rhs.isLooped() );
		CT_REQUIRE( lhs.getAnimation().getType() == rhs.getAnimation().getType() );

		switch ( lhs.getAnimation().getType() )
		{
		case AnimationType::eSkeleton:
			result = result && CT_EQUAL( static_cast< SkeletonAnimationInstance const & >( lhs ), static_cast< SkeletonAnimationInstance const & >( rhs ) );
			break;
		}

		return result;
	}

	bool C3DTestCase::compare( SkeletonAnimationInstance const & lhs, SkeletonAnimationInstance const & rhs )
	{
		return CT_EQUAL( lhs.getObjectsCount(), rhs.getObjectsCount() );
	}

	bool C3DTestCase::compare( castor3d::SkeletonAnimationInstanceObject const & p_a, castor3d::SkeletonAnimationInstanceObject const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getFinalTransform(), p_b.getFinalTransform() ) };
		auto & childrenA = p_a.getChildren();
		auto & childrenB = p_b.getChildren();
		result = result && ( childrenA.size() == childrenB.size() );
		auto itA = childrenA.begin();
		auto itB = childrenB.begin();

		while ( result && itA != childrenA.end() )
		{
			result = CT_EQUAL( *itA, *itB );
			++itA;
			++itB;
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::SkeletonAnimationInstanceKeyFrame const & p_a, castor3d::SkeletonAnimationInstanceKeyFrame const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getTimeIndex(), p_b.getTimeIndex() ) };
		return result;
	}

	bool C3DTestCase::compare( castor3d::VertexBoneData const & p_a, castor3d::VertexBoneData const & p_b )
	{
		bool result{ CT_EQUAL( p_a.m_ids, p_b.m_ids ) };
		result = result && CT_EQUAL( p_a.m_weights, p_b.m_weights );
		return result;
	}

	bool C3DTestCase::compare( castor3d::VertexBoneData::Ids const & p_a, castor3d::VertexBoneData::Ids const & p_b )
	{
		bool result = true;
		auto itA = p_a.begin();
		auto itB = p_b.begin();

		while ( result && itA != p_a.end() )
		{
			result = CT_EQUAL( *itA, *itB );
			++itA;
			++itB;
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::VertexBoneData::Weights const & p_a, castor3d::VertexBoneData::Weights const & p_b )
	{
		bool result = true;
		auto itA = p_a.begin();
		auto itB = p_b.begin();

		while ( result && itA != p_a.end() )
		{
			result = CT_EQUAL( *itA, *itB );
			++itA;
			++itB;
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::InterleavedVertex const & p_a, castor3d::InterleavedVertex const & p_b )
	{
		bool result = CT_EQUAL( p_a.pos, p_b.pos );
		result = result && CT_EQUAL( p_a.nml, p_b.nml );
		result = result && CT_EQUAL( p_a.tan, p_b.tan );
		result = result && CT_EQUAL( p_a.tex, p_b.tex );
		return result;
	}

	//*********************************************************************************************
}
