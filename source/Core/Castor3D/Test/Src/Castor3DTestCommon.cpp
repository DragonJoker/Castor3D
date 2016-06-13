#include "Castor3DTestCommon.hpp"

#include <AnimatedObjectGroupManager.hpp>
#include <CameraManager.hpp>
#include <GeometryManager.hpp>
#include <LightManager.hpp>
#include <SceneNodeManager.hpp>

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

using namespace Castor;
using namespace Castor3D;

namespace Testing
{
	bool C3DTestCase::compare( Scene const & p_a, Scene const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };

		if ( l_return )
		{
			auto l_lockA = make_unique_lock( p_a.GetSceneNodeManager() );
			auto l_lockB = make_unique_lock( p_b.GetSceneNodeManager() );
			auto l_itA = p_a.GetSceneNodeManager().begin();
			auto l_endItA =  p_a.GetSceneNodeManager().end();
			auto l_itB = p_b.GetSceneNodeManager().begin();
			auto l_endItB = p_b.GetSceneNodeManager().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				if ( l_return
					 && l_itA->first.find( cuT( "_REye" ) ) == String::npos
					 && l_itA->first.find( cuT( "_LEye" ) ) == String::npos )
				{
					l_return = CT_EQUAL( l_itA->first, l_itB->first );
					l_return &= CT_EQUAL( *l_itA->second, *l_itB->second );
				}

				++l_itA;
				++l_itB;
			}
		}

		if ( l_return )
		{
			auto l_lockA = make_unique_lock( p_a.GetGeometryManager() );
			auto l_lockB = make_unique_lock( p_b.GetGeometryManager() );
			auto l_itA = p_a.GetGeometryManager().begin();
			auto l_endItA =  p_a.GetGeometryManager().end();
			auto l_itB = p_b.GetGeometryManager().begin();
			auto l_endItB = p_b.GetGeometryManager().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				l_return = CT_EQUAL( l_itA->first, l_itB->first );
				l_return &= CT_EQUAL( *l_itA->second, *l_itB->second );
				++l_itA;
				++l_itB;
			}
		}

		if ( l_return )
		{
			auto l_lockA = make_unique_lock( p_a.GetLightManager() );
			auto l_lockB = make_unique_lock( p_b.GetLightManager() );
			auto l_itA = p_a.GetLightManager().begin();
			auto l_endItA =  p_a.GetLightManager().end();
			auto l_itB = p_b.GetLightManager().begin();
			auto l_endItB = p_b.GetLightManager().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				l_return = CT_EQUAL( l_itA->first, l_itB->first );
				l_return &= CT_EQUAL( *l_itA->second, *l_itB->second );
				++l_itA;
				++l_itB;
			}
		}

		if ( l_return )
		{
			auto l_lockA = make_unique_lock( p_a.GetCameraManager() );
			auto l_lockB = make_unique_lock( p_b.GetCameraManager() );
			auto l_itA = p_a.GetCameraManager().begin();
			auto l_endItA =  p_a.GetCameraManager().end();
			auto l_itB = p_b.GetCameraManager().begin();
			auto l_endItB = p_b.GetCameraManager().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				if ( l_return
					 && l_itA->first.find( cuT( "_REye" ) ) == String::npos
					 && l_itA->first.find( cuT( "_LEye" ) ) == String::npos )
				{
					l_return = CT_EQUAL( l_itA->first, l_itB->first );
					l_return &= CT_EQUAL( *l_itA->second, *l_itB->second );
				}

				++l_itA;
				++l_itB;
			}
		}

		if ( l_return )
		{
			auto l_lockA = make_unique_lock( p_a.GetAnimatedObjectGroupManager() );
			auto l_lockB = make_unique_lock( p_b.GetAnimatedObjectGroupManager() );
			auto l_itA = p_a.GetAnimatedObjectGroupManager().begin();
			auto l_endItA =  p_a.GetAnimatedObjectGroupManager().end();
			auto l_itB = p_b.GetAnimatedObjectGroupManager().begin();
			auto l_endItB = p_b.GetAnimatedObjectGroupManager().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				if ( l_return )
				{
					l_return = CT_EQUAL( l_itA->first, l_itB->first );
					l_return &= CT_EQUAL( *l_itA->second, *l_itB->second );
				}

				++l_itA;
				++l_itB;
			}
		}

		return l_return;
	}

	bool C3DTestCase::compare( SceneNode const & p_a, SceneNode const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		l_return &= CT_EQUAL( p_a.GetOrientation(), p_b.GetOrientation() );
		l_return &= CT_EQUAL( p_a.GetPosition(), p_b.GetPosition() );
		l_return &= CT_EQUAL( p_a.GetScale(), p_b.GetScale() );
		l_return &= CT_EQUAL( p_a.GetParent() != nullptr, p_b.GetParent() != nullptr );

		if ( l_return && p_a.GetParent() && p_b.GetParent() )
		{
			l_return &= CT_EQUAL( p_a.GetParent()->GetName(), p_b.GetParent()->GetName() );
		}

		return l_return;
	}

	bool C3DTestCase::compare( Animable const & p_a, Animable const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetAnimations().size(), p_b.GetAnimations().size() ) };
		auto l_itA = p_a.GetAnimations().begin();
		auto const l_endItA = p_a.GetAnimations().end();
		auto l_itB = p_b.GetAnimations().begin();
		auto const l_endItB = p_b.GetAnimations().end();

		while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
		{
			l_return = CT_EQUAL( l_itA->first, l_itB->first );
			l_return &= CT_EQUAL( *l_itA->second, *l_itB->second );
			++l_itA;
			++l_itB;
		}

		l_return &= CT_EQUAL( p_a.GetAnimatedObject() != nullptr, p_b.GetAnimatedObject() != nullptr );

		if ( l_return && p_a.GetAnimatedObject() && p_b.GetAnimatedObject() )
		{
			l_return &= CT_EQUAL( p_a.GetAnimatedObject()->GetName(), p_b.GetAnimatedObject()->GetName() );
		}

		return l_return;
	}

	bool C3DTestCase::compare( MovableObject const & p_a, MovableObject const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		l_return &= CT_EQUAL( p_a.GetParent()->GetName(), p_b.GetParent()->GetName() );
		l_return &= CT_EQUAL( static_cast< Animable const & >( p_a ), static_cast< Animable const & >( p_b ) );
		return l_return;
	}

	bool C3DTestCase::compare( Light const & p_a, Light const & p_b )
	{
		bool l_return{ CT_EQUAL( static_cast< MovableObject const & >( p_a ), static_cast< MovableObject const & >( p_b ) ) };
		l_return &= CT_EQUAL( *p_a.GetCategory(), *p_b.GetCategory() );
		return l_return;
	}

	bool C3DTestCase::compare( Geometry const & p_a, Geometry const & p_b )
	{
		bool l_return{ CT_EQUAL( static_cast< MovableObject const & >( p_a ), static_cast< MovableObject const & >( p_b ) ) };
		l_return &= CT_EQUAL( *p_a.GetMesh(), *p_b.GetMesh() );
		return l_return;
	}

	bool C3DTestCase::compare( Camera const & p_a, Camera const & p_b )
	{
		bool l_return{ CT_EQUAL( static_cast< MovableObject const & >( p_a ), static_cast< MovableObject const & >( p_b ) ) };
		l_return &= CT_EQUAL( p_a.GetViewport(), p_b.GetViewport() );
		return l_return;
	}

	bool C3DTestCase::compare( LightCategory const & p_a, LightCategory const & p_b )
	{
		CT_REQUIRE( p_a.GetLightType() == p_b.GetLightType() );
		bool l_return{ CT_EQUAL( p_a.GetIntensity(), p_b.GetIntensity() ) };
		l_return &= CT_EQUAL( p_a.GetColour(), p_b.GetColour() );

		switch( p_a.GetLightType() )
		{
		case eLIGHT_TYPE_DIRECTIONAL:
			l_return &= CT_EQUAL( static_cast< DirectionalLight const & >( p_a ), static_cast< DirectionalLight const & >( p_b ) );
			break;

		case eLIGHT_TYPE_POINT:
			l_return &= CT_EQUAL( static_cast< PointLight const & >( p_a ), static_cast< PointLight const & >( p_b ) );
			break;

		case eLIGHT_TYPE_SPOT:
			l_return &= CT_EQUAL( static_cast< SpotLight const & >( p_a ), static_cast< SpotLight const & >( p_b ) );
			break;
		}

		return l_return;
	}

	bool C3DTestCase::compare( DirectionalLight const & p_a, DirectionalLight const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetDirection(), p_b.GetDirection() ) };
		return l_return;
	}

	bool C3DTestCase::compare( PointLight const & p_a, PointLight const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetAttenuation(), p_b.GetAttenuation() ) };
		l_return &= CT_EQUAL( p_a.GetPosition(), p_b.GetPosition() );
		return l_return;
	}

	bool C3DTestCase::compare( SpotLight const & p_a, SpotLight const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetExponent(), p_b.GetExponent() ) };
		l_return &= CT_EQUAL( p_a.GetAttenuation(), p_b.GetAttenuation() );
		l_return &= CT_EQUAL( p_a.GetCutOff(), p_b.GetCutOff() );
		l_return &= CT_EQUAL( p_a.GetPosition(), p_b.GetPosition() );
		return l_return;
	}

	bool C3DTestCase::compare( Viewport const & p_a, Viewport const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetLeft(), p_b.GetLeft() ) };
		l_return &= CT_EQUAL( p_a.GetRight(), p_b.GetRight() );
		l_return &= CT_EQUAL( p_a.GetTop(), p_b.GetTop() );
		l_return &= CT_EQUAL( p_a.GetBottom(), p_b.GetBottom() );
		l_return &= CT_EQUAL( p_a.GetNear(), p_b.GetNear() );
		l_return &= CT_EQUAL( p_a.GetFar(), p_b.GetFar() );
		l_return &= CT_EQUAL( p_a.GetFovY(), p_b.GetFovY() );
		l_return &= CT_EQUAL( p_a.GetRatio(), p_b.GetRatio() );
		l_return &= CT_EQUAL( p_a.GetWidth(), p_b.GetWidth() );
		l_return &= CT_EQUAL( p_a.GetHeight(), p_b.GetHeight() );
		l_return &= CT_EQUAL( p_a.GetType(), p_b.GetType() );
		return l_return;
	}

	bool C3DTestCase::compare( Mesh const & p_a, Mesh const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		l_return &= CT_EQUAL( p_a.GetSubmeshCount(), p_b.GetSubmeshCount() );
		auto l_itA = p_a.begin();
		auto const l_endItA = p_a.end();
		auto l_itB = p_b.begin();
		auto const l_endItB = p_b.end();

		while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
		{
			l_return = CT_EQUAL( *( *( l_itA ) ), *( *( l_itB ) ) );
			++l_itA;
			++l_itB;
		}

		l_return &= CT_EQUAL( p_a.GetSkeleton() != nullptr, p_b.GetSkeleton() != nullptr );

		if ( l_return && p_a.GetSkeleton() )
		{
			l_return = CT_EQUAL( *p_a.GetSkeleton(), *p_b.GetSkeleton() );
		}

		return l_return;
	}

	bool C3DTestCase::compare( Submesh const & p_a, Submesh const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetPointsCount(), p_b.GetPointsCount() ) };
		l_return &= CT_EQUAL( std::make_pair( p_a.GetVertexBuffer().data(), p_a.GetVertexBuffer().GetSize() )
							  , std::make_pair( p_b.GetVertexBuffer().data(), p_b.GetVertexBuffer().GetSize() ) );
		l_return &= CT_EQUAL( p_a.HasIndexBuffer(), p_b.HasIndexBuffer() );

		if ( l_return && p_a.HasIndexBuffer() )
		{
			l_return &= CT_EQUAL( std::make_pair( p_a.GetIndexBuffer().data(), p_a.GetIndexBuffer().GetSize() )
								  , std::make_pair( p_b.GetIndexBuffer().data(), p_b.GetIndexBuffer().GetSize() ) );
		}

		l_return &= CT_EQUAL( p_a.HasBonesBuffer(), p_b.HasBonesBuffer() );

		if ( l_return && p_a.HasBonesBuffer() )
		{
			l_return &= CT_EQUAL( std::make_pair( p_a.GetBonesBuffer().data(), p_a.GetBonesBuffer().GetSize() )
								  , std::make_pair( p_b.GetBonesBuffer().data(), p_b.GetBonesBuffer().GetSize() ) );
		}

		l_return &= CT_EQUAL( p_a.HasMatrixBuffer(), p_b.HasMatrixBuffer() );

		if ( l_return && p_a.HasMatrixBuffer() )
		{
			l_return &= CT_EQUAL( std::make_pair( p_a.GetMatrixBuffer().data(), p_a.GetMatrixBuffer().GetSize() )
								  , std::make_pair( p_b.GetMatrixBuffer().data(), p_b.GetMatrixBuffer().GetSize() ) );
		}

		return l_return;
	}

	bool C3DTestCase::compare( Skeleton const & p_a, Skeleton const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetGlobalInverseTransform(), p_b.GetGlobalInverseTransform() ) };

		if ( l_return )
		{
			l_return = CT_EQUAL( p_a.GetBonesCount(), p_b.GetBonesCount() );
			auto l_itA = p_a.begin();
			auto const l_endItA = p_a.end();
			auto l_itB = p_b.begin();
			auto const l_endItB = p_b.end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				auto l_boneA = *l_itA;
				auto l_boneB = *l_itB;
				CT_REQUIRE( l_boneA != nullptr && l_boneB != nullptr );
				l_return = CT_EQUAL( *l_boneA, *l_boneB );
				++l_itA;
				++l_itB;
			}
		}

		if ( l_return )
		{
			l_return &= CT_EQUAL( static_cast< Animable const & >( p_a ), static_cast< Animable const & >( p_b ) );
		}

		return l_return;
	}

	bool C3DTestCase::compare( Bone const & p_a, Bone const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		l_return &= CT_EQUAL( p_a.GetParent() == nullptr, p_b.GetParent() == nullptr );

		if ( l_return && p_a.GetParent() )
		{
			l_return = CT_EQUAL( p_a.GetParent()->GetName(), p_b.GetParent()->GetName() );
		}

		l_return &= CT_EQUAL( p_a.GetOffsetMatrix(), p_b.GetOffsetMatrix() );
		return l_return;
	}

	bool C3DTestCase::compare( Animation const & p_a, Animation const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		CT_REQUIRE( p_a.GetType() == p_b.GetType() );

		if ( p_a.GetType() == AnimationType::Skeleton )
		{
			l_return = CT_EQUAL( static_cast< SkeletonAnimation const & >( p_a ), static_cast< SkeletonAnimation const & >( p_b ) );
		}

		return l_return;
	}

	bool C3DTestCase::compare( SkeletonAnimation const & p_a, SkeletonAnimation const & p_b )
	{
		bool l_return = CT_EQUAL( p_a.GetObjects().size(), p_b.GetObjects().size() );

		if ( l_return )
		{
			auto l_itA = p_a.GetObjects().begin();
			auto const l_endItA = p_a.GetObjects().end();
			auto l_itB = p_b.GetObjects().begin();
			auto const l_endItB = p_b.GetObjects().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				l_return = CT_EQUAL( l_itA->first, l_itB->first );
				l_return &= CT_EQUAL( *l_itA->second, *l_itB->second );
				++l_itA;
				++l_itB;
			}
		}

		l_return &= CT_EQUAL( p_a.GetRootObjects().size(), p_b.GetRootObjects().size() );

		if ( l_return )
		{
			auto l_itA = p_a.GetRootObjects().begin();
			auto const l_endItA = p_a.GetRootObjects().end();
			auto l_itB = p_b.GetRootObjects().begin();
			auto const l_endItB = p_b.GetRootObjects().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				l_return = CT_EQUAL( *( *l_itA ), *( *l_itB ) );
				++l_itA;
				++l_itB;
			}
		}

		return l_return;
	}

	bool C3DTestCase::compare( SkeletonAnimationObject const & p_a, SkeletonAnimationObject const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		l_return &= CT_EQUAL( p_a.GetInterpolationMode(), p_b.GetInterpolationMode() );
		l_return &= CT_EQUAL( p_a.GetNodeTransform(), p_b.GetNodeTransform() );
		l_return &= CT_EQUAL( p_a.GetLength(), p_b.GetLength() );
		l_return &= CT_EQUAL( p_a.GetType(), p_b.GetType() );
		l_return &= CT_EQUAL( p_a.GetParent() == nullptr, p_b.GetParent() == nullptr );

		if ( l_return && p_a.GetParent() )
		{
			l_return = CT_EQUAL( p_a.GetParent()->GetName(), p_b.GetParent()->GetName() );
		}

		if ( l_return )
		{
			l_return = CT_EQUAL( p_a.GetKeyFrames().size(), p_b.GetKeyFrames().size() );
			auto l_itA = p_a.GetKeyFrames().begin();
			auto const l_endItA = p_a.GetKeyFrames().end();
			auto l_itB = p_b.GetKeyFrames().begin();
			auto const l_endItB = p_b.GetKeyFrames().end();

			while ( l_return && l_itA != l_endItA )
			{
				l_return = CT_EQUAL( *l_itA, *l_itB );
				++l_itA;
				++l_itB;
			}
		}

		if ( l_return )
		{
			l_return = CT_EQUAL( p_a.GetChildren().size(), p_b.GetChildren().size() );
			auto l_itA = p_a.GetChildren().begin();
			auto const l_endItA = p_a.GetChildren().end();
			auto l_itB = p_b.GetChildren().begin();
			auto const l_endItB = p_b.GetChildren().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				auto l_objectA = *l_itA;
				auto l_objectB = *l_itB;
				CT_REQUIRE( l_objectA->GetType() == l_objectB->GetType() );
				l_return = CT_EQUAL( *l_objectA, *l_objectB );
				++l_itA;
				++l_itB;
			}
		}

		return l_return;
	}

	bool C3DTestCase::compare( KeyFrame const & p_a, KeyFrame const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetRotate(), p_b.GetRotate() ) };
		l_return &= CT_EQUAL( p_a.GetTranslate(), p_b.GetTranslate() );
		l_return &= CT_EQUAL( p_a.GetScale(), p_b.GetScale() );
		l_return &= CT_EQUAL( p_a.GetTimeIndex(), p_b.GetTimeIndex() );
		return l_return;
	}

	bool C3DTestCase::C3DTestCase::compare( AnimatedObjectGroup const & p_a, AnimatedObjectGroup const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };

		if ( l_return )
		{
			l_return = CT_EQUAL( p_a.GetAnimations().size(), p_b.GetAnimations().size() );
			auto l_itA = p_a.GetAnimations().begin();
			auto const l_endItA = p_a.GetAnimations().end();
			auto l_itB = p_b.GetAnimations().begin();
			auto const l_endItB = p_b.GetAnimations().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				l_return = CT_EQUAL( l_itA->first, l_itB->first );
				l_return &= CT_EQUAL( l_itA->second, l_itB->second );
				++l_itA;
				++l_itB;
			}
		}

		if ( l_return )
		{
			l_return = CT_EQUAL( p_a.GetObjects().size(), p_b.GetObjects().size() );
			auto l_itA = p_a.GetObjects().begin();
			auto const l_endItA = p_a.GetObjects().end();
			auto l_itB = p_b.GetObjects().begin();
			auto const l_endItB = p_b.GetObjects().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				l_return = CT_EQUAL( l_itA->first, l_itB->first );
				l_return &= CT_EQUAL( *l_itA->second, *l_itB->second );
				++l_itA;
				++l_itB;
			}
		}

		return l_return;
	}

	bool C3DTestCase::C3DTestCase::compare( AnimatedObject const & p_a, AnimatedObject const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };

		if ( l_return )
		{
			l_return = CT_EQUAL( p_a.GetAnimations().size(), p_b.GetAnimations().size() );
			auto l_itA = p_a.GetAnimations().begin();
			auto const l_endItA = p_a.GetAnimations().end();
			auto l_itB = p_b.GetAnimations().begin();
			auto const l_endItB = p_b.GetAnimations().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				l_return = CT_EQUAL( l_itA->first, l_itB->first );
				l_return &= CT_EQUAL( *l_itA->second, *l_itB->second );
				++l_itA;
				++l_itB;
			}
		}

		return l_return;
	}

	bool C3DTestCase::C3DTestCase::compare( AnimationInstance const & p_a, AnimationInstance const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetAnimation().GetName(), p_b.GetAnimation().GetName() ) };
		l_return &= CT_EQUAL( p_a.GetScale(), p_b.GetScale() );
		l_return &= CT_EQUAL( p_a.GetState(), p_b.GetState() );
		l_return &= CT_EQUAL( p_a.IsLooped(), p_b.IsLooped() );
		CT_REQUIRE( p_a.GetAnimation().GetType() == p_b.GetAnimation().GetType() );

		switch ( p_a.GetAnimation().GetType() )
		{
		case AnimationType::Skeleton:
			l_return &= CT_EQUAL( static_cast< SkeletonAnimationInstance const & >( p_a ), static_cast< SkeletonAnimationInstance const & >( p_b ) );
			break;
		}

		return l_return;
	}

	bool C3DTestCase::C3DTestCase::compare( SkeletonAnimationInstance const & p_a, SkeletonAnimationInstance const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetRootObjectsCount(), p_b.GetRootObjectsCount() ) };
		l_return &= CT_EQUAL( p_a.GetObjectsCount(), p_b.GetObjectsCount() );

		if ( l_return )
		{
			auto l_itA = p_a.begin();
			auto const l_endItA = p_a.end();
			auto l_itB = p_b.begin();
			auto const l_endItB = p_b.end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				l_return &= CT_EQUAL( *( *l_itA ), *( *l_itB ) );
				++l_itA;
				++l_itB;
			}
		}

		return l_return;
	}

	bool C3DTestCase::C3DTestCase::compare( SkeletonAnimationInstanceObject const & p_a, SkeletonAnimationInstanceObject const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetInterpolationMode(), p_b.GetInterpolationMode() ) };

		if ( l_return )
		{
			l_return = CT_EQUAL( p_a.GetChildren().size(), p_b.GetChildren().size() );
			auto l_itA = p_a.GetChildren().begin();
			auto const l_endItA = p_a.GetChildren().end();
			auto l_itB = p_b.GetChildren().begin();
			auto const l_endItB = p_b.GetChildren().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				l_return &= CT_EQUAL( *( *l_itA ), *( *l_itB ) );
				++l_itA;
				++l_itB;
			}
		}

		return l_return;
	}

	//*********************************************************************************************
}
