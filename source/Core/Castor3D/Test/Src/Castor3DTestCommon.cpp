#include "Castor3DTestCommon.hpp"

#include <Cache/AnimatedObjectGroupCache.hpp>
#include <Cache/CameraCache.hpp>
#include <Cache/GeometryCache.hpp>
#include <Cache/LightCache.hpp>
#include <Cache/SceneNodeCache.hpp>

#include <Animation/Animable.hpp>
#include <Animation/Animation.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Skeleton/Skeleton.hpp>
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

using namespace Castor;
using namespace Castor3D;

using Castor::operator<<;
using Castor3D::operator<<;

namespace Testing
{
	C3DTestCase::C3DTestCase( std::string const & p_name, Castor3D::Engine & engine )
		: TestCase{ p_name }
		, m_engine{ engine }
		, m_testDataFolder{ Engine::GetDataDirectory() / cuT( "Castor3DTest" ) / cuT( "data" ) }
	{
	}

	void C3DTestCase::DeCleanupEngine()
	{
		m_engine.Cleanup();
		m_engine.Initialise( 1, false );
	}

	bool C3DTestCase::compare( Angle const & p_a, Angle const & p_b )
	{
		return p_a.radians() == p_b.radians();
	}

	bool C3DTestCase::compare( Castor::Quaternion const & p_a, Castor::Quaternion const & p_b )
	{
		Castor::Angle alphaA;
		Castor::Angle alphaB;
		Castor::Point3r axisA;
		Castor::Point3r axisB;
		p_a.to_axis_angle( axisA, alphaA );
		p_b.to_axis_angle( axisB, alphaB );
		auto result = CT_EQUAL( axisA, axisB );
		return result && CT_EQUAL( alphaA, alphaB );
	}

	bool C3DTestCase::compare( Scene const & p_a, Scene const & p_b )
	{
		auto lockA = make_unique_lock( p_a.GetSceneNodeCache() );
		auto lockB = make_unique_lock( p_b.GetSceneNodeCache() );
		auto itA = p_a.GetSceneNodeCache().begin();
		auto endItA =  p_a.GetSceneNodeCache().end();
		auto itB = p_b.GetSceneNodeCache().begin();
		auto endItB = p_b.GetSceneNodeCache().end();
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
			auto lockA = make_unique_lock( p_a.GetGeometryCache() );
			auto lockB = make_unique_lock( p_b.GetGeometryCache() );
			auto itA = p_a.GetGeometryCache().begin();
			auto endItA =  p_a.GetGeometryCache().end();
			auto itB = p_b.GetGeometryCache().begin();
			auto endItB = p_b.GetGeometryCache().end();

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
			auto lockA = make_unique_lock( p_a.GetLightCache() );
			auto lockB = make_unique_lock( p_b.GetLightCache() );
			auto itA = p_a.GetLightCache().begin();
			auto endItA =  p_a.GetLightCache().end();
			auto itB = p_b.GetLightCache().begin();
			auto endItB = p_b.GetLightCache().end();

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
			auto lockA = make_unique_lock( p_a.GetCameraCache() );
			auto lockB = make_unique_lock( p_b.GetCameraCache() );
			auto itA = p_a.GetCameraCache().begin();
			auto endItA =  p_a.GetCameraCache().end();
			auto itB = p_b.GetCameraCache().begin();
			auto endItB = p_b.GetCameraCache().end();

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
			auto lockA = make_unique_lock( p_a.GetAnimatedObjectGroupCache() );
			auto lockB = make_unique_lock( p_b.GetAnimatedObjectGroupCache() );
			auto itA = p_a.GetAnimatedObjectGroupCache().begin();
			auto endItA =  p_a.GetAnimatedObjectGroupCache().end();
			auto itB = p_b.GetAnimatedObjectGroupCache().begin();
			auto endItB = p_b.GetAnimatedObjectGroupCache().end();

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
		bool result{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		result &= CT_EQUAL( p_a.GetOrientation(), p_b.GetOrientation() );
		result &= CT_EQUAL( p_a.GetPosition(), p_b.GetPosition() );
		result &= CT_EQUAL( p_a.GetScale(), p_b.GetScale() );
		result &= CT_EQUAL( p_a.GetParent() != nullptr, p_b.GetParent() != nullptr );

		if ( result && p_a.GetParent() && p_b.GetParent() )
		{
			result &= CT_EQUAL( p_a.GetParent()->GetName(), p_b.GetParent()->GetName() );
		}

		return result;
	}

	bool C3DTestCase::compare( Animable const & p_a, Animable const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetAnimations().size(), p_b.GetAnimations().size() ) };
		auto itA = p_a.GetAnimations().begin();
		auto const endItA = p_a.GetAnimations().end();
		auto itB = p_b.GetAnimations().begin();
		auto const endItB = p_b.GetAnimations().end();

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
		bool result{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		result &= CT_EQUAL( p_a.GetParent()->GetName(), p_b.GetParent()->GetName() );
		result &= CT_EQUAL( static_cast< Animable const & >( p_a ), static_cast< Animable const & >( p_b ) );
		return result;
	}

	bool C3DTestCase::compare( Light const & p_a, Light const & p_b )
	{
		bool result{ CT_EQUAL( static_cast< MovableObject const & >( p_a ), static_cast< MovableObject const & >( p_b ) ) };
		result &= CT_EQUAL( *p_a.GetCategory(), *p_b.GetCategory() );
		return result;
	}

	bool C3DTestCase::compare( Geometry const & p_a, Geometry const & p_b )
	{
		bool result{ CT_EQUAL( static_cast< MovableObject const & >( p_a ), static_cast< MovableObject const & >( p_b ) ) };
		result &= CT_EQUAL( *p_a.GetMesh(), *p_b.GetMesh() );
		return result;
	}

	bool C3DTestCase::compare( Camera const & p_a, Camera const & p_b )
	{
		bool result{ CT_EQUAL( static_cast< MovableObject const & >( p_a ), static_cast< MovableObject const & >( p_b ) ) };
		result &= CT_EQUAL( p_a.GetViewport(), p_b.GetViewport() );
		return result;
	}

	bool C3DTestCase::compare( LightCategory const & p_a, LightCategory const & p_b )
	{
		CT_REQUIRE( p_a.GetLightType() == p_b.GetLightType() );
		bool result{ CT_EQUAL( p_a.GetIntensity(), p_b.GetIntensity() ) };
		result &= CT_EQUAL( p_a.GetColour(), p_b.GetColour() );

		switch ( p_a.GetLightType() )
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
		bool result{ CT_EQUAL( p_a.GetDirection(), p_b.GetDirection() ) };
		return result;
	}

	bool C3DTestCase::compare( PointLight const & p_a, PointLight const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetAttenuation(), p_b.GetAttenuation() ) };
		return result;
	}

	bool C3DTestCase::compare( SpotLight const & p_a, SpotLight const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetExponent(), p_b.GetExponent() ) };
		result &= CT_EQUAL( p_a.GetAttenuation(), p_b.GetAttenuation() );
		result &= CT_EQUAL( p_a.GetCutOff(), p_b.GetCutOff() );
		return result;
	}

	bool C3DTestCase::compare( Viewport const & p_a, Viewport const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetLeft(), p_b.GetLeft() ) };
		result &= CT_EQUAL( p_a.GetRight(), p_b.GetRight() );
		result &= CT_EQUAL( p_a.GetTop(), p_b.GetTop() );
		result &= CT_EQUAL( p_a.GetBottom(), p_b.GetBottom() );
		result &= CT_EQUAL( p_a.GetNear(), p_b.GetNear() );
		result &= CT_EQUAL( p_a.GetFar(), p_b.GetFar() );
		result &= CT_EQUAL( p_a.GetFovY(), p_b.GetFovY() );
		result &= CT_EQUAL( p_a.GetRatio(), p_b.GetRatio() );
		result &= CT_EQUAL( p_a.GetWidth(), p_b.GetWidth() );
		result &= CT_EQUAL( p_a.GetHeight(), p_b.GetHeight() );
		result &= CT_EQUAL( p_a.GetType(), p_b.GetType() );
		return result;
	}

	bool C3DTestCase::compare( Mesh const & p_a, Mesh const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		result &= CT_EQUAL( p_a.GetSubmeshCount(), p_b.GetSubmeshCount() );
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

		result &= CT_EQUAL( p_a.GetSkeleton() != nullptr, p_b.GetSkeleton() != nullptr );

		if ( result && p_a.GetSkeleton() )
		{
			result = CT_EQUAL( *p_a.GetSkeleton(), *p_b.GetSkeleton() );
		}

		return result;
	}

	bool C3DTestCase::compare( Submesh const & p_a, Submesh const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetPointsCount(), p_b.GetPointsCount() ) };
		result &= CT_EQUAL( std::make_pair( p_a.GetVertexBuffer().GetData(), p_a.GetVertexBuffer().GetSize() )
							  , std::make_pair( p_b.GetVertexBuffer().GetData(), p_b.GetVertexBuffer().GetSize() ) );
		result &= CT_EQUAL( std::make_pair( p_a.GetIndexBuffer().GetData(), p_a.GetIndexBuffer().GetSize() )
								, std::make_pair( p_b.GetIndexBuffer().GetData(), p_b.GetIndexBuffer().GetSize() ) );

		result &= CT_EQUAL( p_a.HasBonesBuffer(), p_b.HasBonesBuffer() );

		if ( result && p_a.HasBonesBuffer() )
		{
			result &= CT_EQUAL( std::make_pair( p_a.GetBonesBuffer().GetData(), p_a.GetBonesBuffer().GetSize() )
								  , std::make_pair( p_b.GetBonesBuffer().GetData(), p_b.GetBonesBuffer().GetSize() ) );
		}

		result &= CT_EQUAL( p_a.HasMatrixBuffer(), p_b.HasMatrixBuffer() );

		if ( result && p_a.HasMatrixBuffer() )
		{
			result &= CT_EQUAL( std::make_pair( p_a.GetMatrixBuffer().GetData(), p_a.GetMatrixBuffer().GetSize() )
								  , std::make_pair( p_b.GetMatrixBuffer().GetData(), p_b.GetMatrixBuffer().GetSize() ) );
		}

		return result;
	}

	bool C3DTestCase::compare( Skeleton const & p_a, Skeleton const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetGlobalInverseTransform(), p_b.GetGlobalInverseTransform() ) };

		if ( result )
		{
			result = CT_EQUAL( p_a.GetBonesCount(), p_b.GetBonesCount() );
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
		bool result{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		result &= CT_EQUAL( p_a.GetParent() == nullptr, p_b.GetParent() == nullptr );

		if ( result && p_a.GetParent() )
		{
			result = CT_EQUAL( p_a.GetParent()->GetName(), p_b.GetParent()->GetName() );
		}

		result &= CT_EQUAL( p_a.GetOffsetMatrix(), p_b.GetOffsetMatrix() );
		return result;
	}

	bool C3DTestCase::compare( Animation const & p_a, Animation const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		CT_REQUIRE( p_a.GetType() == p_b.GetType() );

		if ( p_a.GetType() == AnimationType::eSkeleton )
		{
			result = CT_EQUAL( static_cast< SkeletonAnimation const & >( p_a ), static_cast< SkeletonAnimation const & >( p_b ) );
		}

		return result;
	}

	bool C3DTestCase::compare( SkeletonAnimation const & p_a, SkeletonAnimation const & p_b )
	{
		bool result = CT_EQUAL( p_a.GetObjects().size(), p_b.GetObjects().size() );

		if ( result )
		{
			auto itA = p_a.GetObjects().begin();
			auto const endItA = p_a.GetObjects().end();
			auto itB = p_b.GetObjects().begin();
			auto const endItB = p_b.GetObjects().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( itA->first, itB->first );
				result &= CT_EQUAL( *itA->second, *itB->second );
				++itA;
				++itB;
			}
		}

		result &= CT_EQUAL( p_a.GetRootObjects().size(), p_b.GetRootObjects().size() );

		if ( result )
		{
			auto itA = p_a.GetRootObjects().begin();
			auto const endItA = p_a.GetRootObjects().end();
			auto itB = p_b.GetRootObjects().begin();
			auto const endItB = p_b.GetRootObjects().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result = CT_EQUAL( *( *itA ), *( *itB ) );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( SkeletonAnimationObject const & p_a, SkeletonAnimationObject const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		result &= CT_EQUAL( p_a.GetInterpolationMode(), p_b.GetInterpolationMode() );
		result &= CT_EQUAL( p_a.GetNodeTransform(), p_b.GetNodeTransform() );
		result &= CT_EQUAL( p_a.GetLength(), p_b.GetLength() );
		result &= CT_EQUAL( p_a.GetType(), p_b.GetType() );
		result &= CT_EQUAL( p_a.GetParent() == nullptr, p_b.GetParent() == nullptr );

		if ( result && p_a.GetParent() )
		{
			result = CT_EQUAL( p_a.GetParent()->GetName(), p_b.GetParent()->GetName() );
		}

		if ( result )
		{
			result = CT_EQUAL( p_a.GetKeyFrames().size(), p_b.GetKeyFrames().size() );
			auto itA = p_a.GetKeyFrames().begin();
			auto const endItA = p_a.GetKeyFrames().end();
			auto itB = p_b.GetKeyFrames().begin();
			auto const endItB = p_b.GetKeyFrames().end();

			while ( result && itA != endItA )
			{
				result = CT_EQUAL( *itA, *itB );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			result = CT_EQUAL( p_a.GetChildren().size(), p_b.GetChildren().size() );
			auto itA = p_a.GetChildren().begin();
			auto const endItA = p_a.GetChildren().end();
			auto itB = p_b.GetChildren().begin();
			auto const endItB = p_b.GetChildren().end();

			while ( result && itA != endItA && itB != endItB )
			{
				auto objectA = *itA;
				auto objectB = *itB;
				CT_REQUIRE( objectA->GetType() == objectB->GetType() );
				result = CT_EQUAL( *objectA, *objectB );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( KeyFrame const & p_a, KeyFrame const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetTransform(), p_b.GetTransform() ) };
		result &= CT_EQUAL( p_a.GetTimeIndex(), p_b.GetTimeIndex() );
		return result;
	}

	bool C3DTestCase::C3DTestCase::compare( AnimatedObjectGroup const & p_a, AnimatedObjectGroup const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };

		if ( result )
		{
			result = CT_EQUAL( p_a.GetAnimations().size(), p_b.GetAnimations().size() );
			auto itA = p_a.GetAnimations().begin();
			auto const endItA = p_a.GetAnimations().end();
			auto itB = p_b.GetAnimations().begin();
			auto const endItB = p_b.GetAnimations().end();

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
			result = CT_EQUAL( p_a.GetObjects().size(), p_b.GetObjects().size() );
			auto itA = p_a.GetObjects().begin();
			auto const endItA = p_a.GetObjects().end();
			auto itB = p_b.GetObjects().begin();
			auto const endItB = p_b.GetObjects().end();

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

	bool C3DTestCase::C3DTestCase::compare( AnimatedObject const & p_a, AnimatedObject const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };

		if ( result )
		{
			result = CT_EQUAL( p_a.GetAnimations().size(), p_b.GetAnimations().size() );
			auto itA = p_a.GetAnimations().begin();
			auto const endItA = p_a.GetAnimations().end();
			auto itB = p_b.GetAnimations().begin();
			auto const endItB = p_b.GetAnimations().end();

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

	bool C3DTestCase::C3DTestCase::compare( AnimationInstance const & p_a, AnimationInstance const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetAnimation().GetName(), p_b.GetAnimation().GetName() ) };
		result &= CT_EQUAL( p_a.GetScale(), p_b.GetScale() );
		result &= CT_EQUAL( p_a.GetState(), p_b.GetState() );
		result &= CT_EQUAL( p_a.IsLooped(), p_b.IsLooped() );
		CT_REQUIRE( p_a.GetAnimation().GetType() == p_b.GetAnimation().GetType() );

		switch ( p_a.GetAnimation().GetType() )
		{
		case AnimationType::eSkeleton:
			result &= CT_EQUAL( static_cast< SkeletonAnimationInstance const & >( p_a ), static_cast< SkeletonAnimationInstance const & >( p_b ) );
			break;
		}

		return result;
	}

	bool C3DTestCase::C3DTestCase::compare( SkeletonAnimationInstance const & p_a, SkeletonAnimationInstance const & p_b )
	{
		bool result{ CT_EQUAL( p_a.GetRootObjectsCount(), p_b.GetRootObjectsCount() ) };
		result &= CT_EQUAL( p_a.GetObjectsCount(), p_b.GetObjectsCount() );

		if ( result )
		{
			auto itA = p_a.begin();
			auto const endItA = p_a.end();
			auto itB = p_b.begin();
			auto const endItB = p_b.end();

			while ( result && itA != endItA && itB != endItB )
			{
				result &= CT_EQUAL( *( *itA ), *( *itB ) );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::C3DTestCase::compare( SkeletonAnimationInstanceObject const & p_a, SkeletonAnimationInstanceObject const & p_b )
	{
		bool result{ true };

		if ( result )
		{
			result = CT_EQUAL( p_a.GetChildren().size(), p_b.GetChildren().size() );
			auto itA = p_a.GetChildren().begin();
			auto const endItA = p_a.GetChildren().end();
			auto itB = p_b.GetChildren().begin();
			auto const endItB = p_b.GetChildren().end();

			while ( result && itA != endItA && itB != endItB )
			{
				result &= CT_EQUAL( *( *itA ), *( *itB ) );
				++itA;
				++itB;
			}
		}

		return result;
	}

	//*********************************************************************************************
}
