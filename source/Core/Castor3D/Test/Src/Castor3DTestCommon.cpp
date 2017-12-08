#include "Castor3DTestCommon.hpp"

#include <Cache/AnimatedObjectGroupCache.hpp>
#include <Cache/CameraCache.hpp>
#include <Cache/GeometryCache.hpp>
#include <Cache/LightCache.hpp>
#include <Cache/SceneNodeCache.hpp>

#include <Animation/Animable.hpp>
#include <Animation/Animation.hpp>
#include <Animation/Mesh/MeshAnimationKeyFrame.hpp>
#include <Animation/Skeleton/SkeletonAnimationKeyFrame.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Skeleton/Skeleton.hpp>
#include <Mesh/SubmeshComponent/BonesComponent.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Animation/AnimatedObject.hpp>
#include <Scene/Animation/AnimatedObjectGroup.hpp>
#include <Scene/Animation/AnimatedSkeleton.hpp>
#include <Scene/Animation/AnimationInstance.hpp>
#include <Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp>
#include <Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Scene/Light/SpotLight.hpp>

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

	void C3DTestCase::DeCleanupEngine()
	{
		m_engine.cleanup();
		m_engine.initialise( 1, false );
	}

	bool C3DTestCase::compare( Angle const & p_a, Angle const & p_b )
	{
		return p_a.radians() == p_b.radians();
	}

	bool C3DTestCase::compare( castor::Quaternion const & p_a, castor::Quaternion const & p_b )
	{
		castor::Angle alphaA;
		castor::Angle alphaB;
		castor::Point3r axisA;
		castor::Point3r axisB;
		p_a.toAxisAngle( axisA, alphaA );
		p_b.toAxisAngle( axisB, alphaB );
		auto result = CT_EQUAL( axisA, axisB );
		return result && CT_EQUAL( alphaA, alphaB );
	}

	bool C3DTestCase::compare( Scene const & p_a, Scene const & p_b )
	{
		auto lockA = makeUniqueLock( p_a.getSceneNodeCache() );
		auto lockB = makeUniqueLock( p_b.getSceneNodeCache() );
		auto itA = p_a.getSceneNodeCache().begin();
		auto endItA =  p_a.getSceneNodeCache().end();
		auto itB = p_b.getSceneNodeCache().begin();
		auto endItB = p_b.getSceneNodeCache().end();
		bool result = true;

		while ( result && itA != endItA && itB != endItB )
		{
			if ( result
					&& itA->first.find( cuT( "_REye" ) ) == String::npos
					&& itA->first.find( cuT( "_LEye" ) ) == String::npos )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result &= CT_EQUAL( *itA->second, *itB->second );
			}

			++itA;
			++itB;
		}

		if ( result )
		{
			auto lockA = makeUniqueLock( p_a.getGeometryCache() );
			auto lockB = makeUniqueLock( p_b.getGeometryCache() );
			auto itA = p_a.getGeometryCache().begin();
			auto endItA =  p_a.getGeometryCache().end();
			auto itB = p_b.getGeometryCache().begin();
			auto endItB = p_b.getGeometryCache().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result &= CT_EQUAL( *itA->second, *itB->second );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			auto lockA = makeUniqueLock( p_a.getLightCache() );
			auto lockB = makeUniqueLock( p_b.getLightCache() );
			auto itA = p_a.getLightCache().begin();
			auto endItA =  p_a.getLightCache().end();
			auto itB = p_b.getLightCache().begin();
			auto endItB = p_b.getLightCache().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result &= CT_EQUAL( *itA->second, *itB->second );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			auto lockA = makeUniqueLock( p_a.getCameraCache() );
			auto lockB = makeUniqueLock( p_b.getCameraCache() );
			auto itA = p_a.getCameraCache().begin();
			auto endItA =  p_a.getCameraCache().end();
			auto itB = p_b.getCameraCache().begin();
			auto endItB = p_b.getCameraCache().end();

			while ( result && itA != endItA && itB != endItB )
			{
				if ( result
					 && itA->first.find( cuT( "_REye" ) ) == String::npos
					 && itA->first.find( cuT( "_LEye" ) ) == String::npos )
				{
					result = CT_EQUAL( itA->first, itB->first );
					result &= CT_EQUAL( *itA->second, *itB->second );
				}

				++itA;
				++itB;
			}
		}

		if ( result )
		{
			auto lockA = makeUniqueLock( p_a.getAnimatedObjectGroupCache() );
			auto lockB = makeUniqueLock( p_b.getAnimatedObjectGroupCache() );
			auto itA = p_a.getAnimatedObjectGroupCache().begin();
			auto endItA =  p_a.getAnimatedObjectGroupCache().end();
			auto itB = p_b.getAnimatedObjectGroupCache().begin();
			auto endItB = p_b.getAnimatedObjectGroupCache().end();

			while ( result && itA != endItA && itB != endItB )
			{
				if ( result )
				{
					result = CT_EQUAL( itA->first, itB->first );
					result &= CT_EQUAL( *itA->second, *itB->second );
				}

				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( SceneNode const & p_a, SceneNode const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getName(), p_b.getName() ) };
		result &= CT_EQUAL( p_a.getOrientation(), p_b.getOrientation() );
		result &= CT_EQUAL( p_a.getPosition(), p_b.getPosition() );
		result &= CT_EQUAL( p_a.getScale(), p_b.getScale() );
		result &= CT_EQUAL( p_a.getParent() != nullptr, p_b.getParent() != nullptr );

		if ( result && p_a.getParent() && p_b.getParent() )
		{
			result &= CT_EQUAL( p_a.getParent()->getName(), p_b.getParent()->getName() );
		}

		return result;
	}

	bool C3DTestCase::compare( Animable const & p_a, Animable const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getAnimations().size(), p_b.getAnimations().size() ) };
		auto itA = p_a.getAnimations().begin();
		auto const endItA = p_a.getAnimations().end();
		auto itB = p_b.getAnimations().begin();
		auto const endItB = p_b.getAnimations().end();

		while ( result && itA != endItA && itB != endItB )
		{
			result = CT_EQUAL( itA->first, itB->first );
			result &= CT_EQUAL( *itA->second, *itB->second );
			++itA;
			++itB;
		}

		return result;
	}

	bool C3DTestCase::compare( MovableObject const & p_a, MovableObject const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getName(), p_b.getName() ) };
		result &= CT_EQUAL( p_a.getParent()->getName(), p_b.getParent()->getName() );
		result &= CT_EQUAL( static_cast< Animable const & >( p_a ), static_cast< Animable const & >( p_b ) );
		return result;
	}

	bool C3DTestCase::compare( Light const & p_a, Light const & p_b )
	{
		bool result{ CT_EQUAL( static_cast< MovableObject const & >( p_a ), static_cast< MovableObject const & >( p_b ) ) };
		result &= CT_EQUAL( *p_a.getCategory(), *p_b.getCategory() );
		return result;
	}

	bool C3DTestCase::compare( Geometry const & p_a, Geometry const & p_b )
	{
		bool result{ CT_EQUAL( static_cast< MovableObject const & >( p_a ), static_cast< MovableObject const & >( p_b ) ) };
		result &= CT_EQUAL( *p_a.getMesh(), *p_b.getMesh() );
		return result;
	}

	bool C3DTestCase::compare( Camera const & p_a, Camera const & p_b )
	{
		bool result{ CT_EQUAL( static_cast< MovableObject const & >( p_a ), static_cast< MovableObject const & >( p_b ) ) };
		result &= CT_EQUAL( p_a.getViewport(), p_b.getViewport() );
		return result;
	}

	bool C3DTestCase::compare( LightCategory const & p_a, LightCategory const & p_b )
	{
		CT_REQUIRE( p_a.getLightType() == p_b.getLightType() );
		bool result{ CT_EQUAL( p_a.getIntensity(), p_b.getIntensity() ) };
		result &= CT_EQUAL( p_a.getColour(), p_b.getColour() );

		switch ( p_a.getLightType() )
		{
		case LightType::eDirectional:
			result &= CT_EQUAL( static_cast< DirectionalLight const & >( p_a ), static_cast< DirectionalLight const & >( p_b ) );
			break;

		case LightType::ePoint:
			result &= CT_EQUAL( static_cast< PointLight const & >( p_a ), static_cast< PointLight const & >( p_b ) );
			break;

		case LightType::eSpot:
			result &= CT_EQUAL( static_cast< SpotLight const & >( p_a ), static_cast< SpotLight const & >( p_b ) );
			break;
		}

		return result;
	}

	bool C3DTestCase::compare( DirectionalLight const & p_a, DirectionalLight const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getDirection(), p_b.getDirection() ) };
		return result;
	}

	bool C3DTestCase::compare( PointLight const & p_a, PointLight const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getAttenuation(), p_b.getAttenuation() ) };
		return result;
	}

	bool C3DTestCase::compare( SpotLight const & p_a, SpotLight const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getExponent(), p_b.getExponent() ) };
		result &= CT_EQUAL( p_a.getAttenuation(), p_b.getAttenuation() );
		result &= CT_EQUAL( p_a.getCutOff(), p_b.getCutOff() );
		return result;
	}

	bool C3DTestCase::compare( Viewport const & p_a, Viewport const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getLeft(), p_b.getLeft() ) };
		result &= CT_EQUAL( p_a.getRight(), p_b.getRight() );
		result &= CT_EQUAL( p_a.getTop(), p_b.getTop() );
		result &= CT_EQUAL( p_a.getBottom(), p_b.getBottom() );
		result &= CT_EQUAL( p_a.getNear(), p_b.getNear() );
		result &= CT_EQUAL( p_a.getFar(), p_b.getFar() );
		result &= CT_EQUAL( p_a.getFovY(), p_b.getFovY() );
		result &= CT_EQUAL( p_a.getRatio(), p_b.getRatio() );
		result &= CT_EQUAL( p_a.getWidth(), p_b.getWidth() );
		result &= CT_EQUAL( p_a.getHeight(), p_b.getHeight() );
		result &= CT_EQUAL( p_a.getType(), p_b.getType() );
		return result;
	}

	bool C3DTestCase::compare( Mesh const & p_a, Mesh const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getName(), p_b.getName() ) };
		result &= CT_EQUAL( p_a.getSubmeshCount(), p_b.getSubmeshCount() );
		auto itA = p_a.begin();
		auto const endItA = p_a.end();
		auto itB = p_b.begin();
		auto const endItB = p_b.end();

		while ( result && itA != endItA && itB != endItB )
		{
			result = CT_EQUAL( *( *( itA ) ), *( *( itB ) ) );
			++itA;
			++itB;
		}

		result &= CT_EQUAL( p_a.getSkeleton() != nullptr, p_b.getSkeleton() != nullptr );

		if ( result && p_a.getSkeleton() )
		{
			result = CT_EQUAL( *p_a.getSkeleton(), *p_b.getSkeleton() );
		}

		return result;
	}

	bool C3DTestCase::compare( BonesComponent const & p_a, BonesComponent const & p_b )
	{
		return CT_EQUAL( std::make_pair( p_a.getBonesBuffer().getData(), p_a.getBonesBuffer().getSize() )
			, std::make_pair( p_b.getBonesBuffer().getData(), p_b.getBonesBuffer().getSize() ) );
	}

	bool C3DTestCase::compare( SubmeshComponent const & p_a, SubmeshComponent const & p_b )
	{
		bool result = CT_EQUAL( p_a.getType(), p_b.getType() );
		result &= CT_EQUAL( p_a.getProgramFlags(), p_b.getProgramFlags() );

		if ( result && p_a.getType() == BonesComponent::Name )
		{
			result &= CT_EQUAL( static_cast< BonesComponent const & >( p_a ), static_cast< BonesComponent const & >( p_b ) );
		}

		return result;
	}

	bool C3DTestCase::compare( Submesh const & p_a, Submesh const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getPointsCount(), p_b.getPointsCount() ) };
		result &= CT_EQUAL( std::make_pair( p_a.getVertexBuffer().getData(), p_a.getVertexBuffer().getSize() )
							  , std::make_pair( p_b.getVertexBuffer().getData(), p_b.getVertexBuffer().getSize() ) );
		result &= CT_EQUAL( std::make_pair( p_a.getIndexBuffer().getData(), p_a.getIndexBuffer().getSize() )
								, std::make_pair( p_b.getIndexBuffer().getData(), p_b.getIndexBuffer().getSize() ) );

		if ( result )
		{
			result &= CT_EQUAL( p_a.getComponents().size(), p_b.getComponents().size() );

			for ( auto & itA : p_a.getComponents() )
			{
				if ( result )
				{
					auto itB = p_b.getComponents().find( itA.first );
					result = CT_CHECK( itB != p_b.getComponents().end() );
					result &= CT_EQUAL( *itA.second, *itB->second );
				}
			}
		}

		return result;
	}

	bool C3DTestCase::compare( Skeleton const & p_a, Skeleton const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getGlobalInverseTransform(), p_b.getGlobalInverseTransform() ) };

		if ( result )
		{
			result = CT_EQUAL( p_a.getBonesCount(), p_b.getBonesCount() );
			auto itA = p_a.begin();
			auto const endItA = p_a.end();
			auto itB = p_b.begin();
			auto const endItB = p_b.end();

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
			result &= CT_EQUAL( static_cast< Animable const & >( p_a ), static_cast< Animable const & >( p_b ) );
		}

		return result;
	}

	bool C3DTestCase::compare( Bone const & p_a, Bone const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getName(), p_b.getName() ) };
		result &= CT_EQUAL( p_a.getParent() == nullptr, p_b.getParent() == nullptr );

		if ( result && p_a.getParent() )
		{
			result = CT_EQUAL( p_a.getParent()->getName(), p_b.getParent()->getName() );
		}

		result &= CT_EQUAL( p_a.getOffsetMatrix(), p_b.getOffsetMatrix() );
		return result;
	}

	bool C3DTestCase::compare( Animation const & p_a, Animation const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getName(), p_b.getName() ) };
		CT_REQUIRE( p_a.getType() == p_b.getType() );

		if ( p_a.getType() == AnimationType::eSkeleton )
		{
			result = CT_EQUAL( static_cast< SkeletonAnimation const & >( p_a ), static_cast< SkeletonAnimation const & >( p_b ) );
		}

		return result;
	}

	bool C3DTestCase::compare( SkeletonAnimation const & p_a, SkeletonAnimation const & p_b )
	{
		bool result = CT_EQUAL( p_a.getObjects().size(), p_b.getObjects().size() );

		if ( result )
		{
			auto itA = p_a.getObjects().begin();
			auto const endItA = p_a.getObjects().end();
			auto itB = p_b.getObjects().begin();
			auto const endItB = p_b.getObjects().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result &= CT_EQUAL( *itA->second, *itB->second );
				++itA;
				++itB;
			}
		}

		result &= CT_EQUAL( p_a.getRootObjects().size(), p_b.getRootObjects().size() );

		if ( result )
		{
			auto itA = p_a.getRootObjects().begin();
			auto const endItA = p_a.getRootObjects().end();
			auto itB = p_b.getRootObjects().begin();
			auto const endItB = p_b.getRootObjects().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( *( *itA ), *( *itB ) );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			auto itA = p_a.begin();
			auto const endItA = p_a.end();
			auto itB = p_b.begin();
			auto const endItB = p_b.end();
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

	bool C3DTestCase::compare( SkeletonAnimationObject const & p_a, SkeletonAnimationObject const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getName(), p_b.getName() ) };
		result &= CT_EQUAL( p_a.getInterpolationMode(), p_b.getInterpolationMode() );
		result &= CT_EQUAL( p_a.getNodeTransform(), p_b.getNodeTransform() );
		result &= CT_EQUAL( p_a.getType(), p_b.getType() );
		result &= CT_EQUAL( p_a.getParent() == nullptr, p_b.getParent() == nullptr );

		if ( result && p_a.getParent() )
		{
			result = CT_EQUAL( p_a.getParent()->getName(), p_b.getParent()->getName() );
		}

		if ( result )
		{
			result = CT_EQUAL( p_a.getChildren().size(), p_b.getChildren().size() );
			auto itA = p_a.getChildren().begin();
			auto const endItA = p_a.getChildren().end();
			auto itB = p_b.getChildren().begin();
			auto const endItB = p_b.getChildren().end();

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

	bool C3DTestCase::compare( AnimationKeyFrame const & p_a, AnimationKeyFrame const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getTimeIndex(), p_b.getTimeIndex() ) };
		return result;
	}

	bool C3DTestCase::compare( castor3d::MeshAnimationKeyFrame const & p_a, castor3d::MeshAnimationKeyFrame const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getTimeIndex(), p_b.getTimeIndex() ) };

		if ( result )
		{
			auto itA = p_a.begin();
			auto const endItA = p_a.end();
			auto itB = p_b.begin();
			auto const endItB = p_b.end();
			result = CT_EQUAL( std::distance( itA, endItA ), std::distance( itB, endItB ) );

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				//result &= CT_EQUAL( itA->second, itB->second );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::SkeletonAnimationKeyFrame const & p_a, castor3d::SkeletonAnimationKeyFrame const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getTimeIndex(), p_b.getTimeIndex() ) };

		if ( result )
		{
			auto itA = p_a.begin();
			auto const endItA = p_a.end();
			auto itB = p_b.begin();
			auto const endItB = p_b.end();
			result = CT_EQUAL( std::distance( itA, endItA ), std::distance( itB, endItB ) );

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first->getName(), itB->first->getName() );
				result &= CT_EQUAL( itA->second, itB->second );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( AnimatedObjectGroup const & p_a, AnimatedObjectGroup const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getName(), p_b.getName() ) };

		if ( result )
		{
			result = CT_EQUAL( p_a.getAnimations().size(), p_b.getAnimations().size() );
			auto itA = p_a.getAnimations().begin();
			auto const endItA = p_a.getAnimations().end();
			auto itB = p_b.getAnimations().begin();
			auto const endItB = p_b.getAnimations().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result &= CT_EQUAL( itA->second.m_state, itB->second.m_state );
				result &= CT_EQUAL( itA->second.m_scale, itB->second.m_scale );
				result &= CT_EQUAL( itA->second.m_looped, itB->second.m_looped );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			result = CT_EQUAL( p_a.getObjects().size(), p_b.getObjects().size() );
			auto itA = p_a.getObjects().begin();
			auto const endItA = p_a.getObjects().end();
			auto itB = p_b.getObjects().begin();
			auto const endItB = p_b.getObjects().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result &= CT_EQUAL( *itA->second, *itB->second );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( AnimatedObject const & p_a, AnimatedObject const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getName(), p_b.getName() ) };

		if ( result )
		{
			result = CT_EQUAL( p_a.getAnimations().size(), p_b.getAnimations().size() );
			auto itA = p_a.getAnimations().begin();
			auto const endItA = p_a.getAnimations().end();
			auto itB = p_b.getAnimations().begin();
			auto const endItB = p_b.getAnimations().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result &= CT_EQUAL( *itA->second, *itB->second );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( AnimationInstance const & p_a, AnimationInstance const & p_b )
	{
		bool result{ CT_EQUAL( p_a.getAnimation().getName(), p_b.getAnimation().getName() ) };
		result &= CT_EQUAL( p_a.getScale(), p_b.getScale() );
		result &= CT_EQUAL( p_a.getState(), p_b.getState() );
		result &= CT_EQUAL( p_a.isLooped(), p_b.isLooped() );
		CT_REQUIRE( p_a.getAnimation().getType() == p_b.getAnimation().getType() );

		switch ( p_a.getAnimation().getType() )
		{
		case AnimationType::eSkeleton:
			result &= CT_EQUAL( static_cast< SkeletonAnimationInstance const & >( p_a ), static_cast< SkeletonAnimationInstance const & >( p_b ) );
			break;
		}

		return result;
	}

	bool C3DTestCase::compare( SkeletonAnimationInstance const & p_a, SkeletonAnimationInstance const & p_b )
	{
		return CT_EQUAL( p_a.getObjectsCount(), p_b.getObjectsCount() );
	}

	//*********************************************************************************************
}
