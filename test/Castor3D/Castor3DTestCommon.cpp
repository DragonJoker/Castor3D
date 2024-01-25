#include "Castor3DTestCommon.hpp"

#include <CastorUtils/Design/BlockGuard.hpp>

#include <cmath>

namespace Testing
{
	namespace details
	{
		static castor::StringMap< castor3d::SceneNodeRPtr > sortNodes( castor3d::Scene const & scene )
		{
			castor::StringMap< castor3d::SceneNodeRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getSceneNodeCache() );

			for ( auto & it : scene.getSceneNodeCache() )
			{
				result.emplace( it.first, it.second.get() );
			}

			return result;
		}

		static castor::StringMap< castor3d::GeometryRPtr > sortGeometries( castor3d::Scene const & scene )
		{
			castor::StringMap< castor3d::GeometryRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getGeometryCache() );

			for ( auto & it : scene.getGeometryCache() )
			{
				result.emplace( it.first, it.second.get() );
			}

			return result;
		}

		static castor::StringMap< castor3d::LightRPtr > sortLights( castor3d::Scene const & scene )
		{
			castor::StringMap< castor3d::LightRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getLightCache() );

			for ( auto & it : scene.getLightCache() )
			{
				result.emplace( it.first, it.second.get() );
			}

			return result;
		}

		static castor::StringMap< castor3d::CameraRPtr > sortCameras( castor3d::Scene const & scene )
		{
			castor::StringMap< castor3d::CameraRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getCameraCache() );

			for ( auto & it : scene.getCameraCache() )
			{
				result.emplace( it.first, it.second.get() );
			}

			return result;
		}

		static castor::StringMap< castor3d::AnimatedObjectGroupRPtr > sortAnimatedGroups( castor3d::Scene const & scene )
		{
			castor::StringMap< castor3d::AnimatedObjectGroupRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getAnimatedObjectGroupCache() );

			for ( auto & it : scene.getAnimatedObjectGroupCache() )
			{
				result.emplace( it.first, it.second.get() );
			}

			return result;
		}
	}

	C3DTestCase::C3DTestCase( std::string const & name
		, castor3d::Engine & engine )
		: TestCase{ name }
		, m_engine{ engine }
		, m_testDataFolder{ castor3d::Engine::getDataDirectory() / cuT( "Castor3DTest" ) / cuT( "data" ) }
	{
	}

	void C3DTestCase::doRegisterTest( std::string const & name
		, TestFunction test )
	{
		TestCase::doRegisterTest( name
			, [this, test]()
			{
				auto guard = castor::makeBlockGuard( [this]()
					{
						m_engine.initialise( 1, false );
					}
					, [this]()
					{
						m_engine.cleanup();
					} );
				test();
			} );
	}

	bool C3DTestCase::compare( castor::Angle const & lhs, castor::Angle const & rhs )
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

	bool C3DTestCase::compare( castor3d::Scene const & lhs, castor3d::Scene const & rhs )
	{
		bool result = true;
		{
			auto sortedLhs = details::sortNodes( lhs );
			auto sortedRhs = details::sortNodes( lhs );
			auto itA = sortedLhs.begin();
			auto endItA = sortedLhs.end();
			auto itB = sortedRhs.begin();
			auto endItB = sortedRhs.end();

			while ( result && itA != endItA && itB != endItB )
			{
				if ( result
					&& itA->first.find( cuT( "_REye" ) ) == castor::String::npos
					&& itA->first.find( cuT( "_LEye" ) ) == castor::String::npos )
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
			auto sortedLhs = details::sortGeometries( lhs );
			auto sortedRhs = details::sortGeometries( lhs );
			auto itA = sortedLhs.begin();
			auto endItA = sortedLhs.end();
			auto itB = sortedRhs.begin();
			auto endItB = sortedRhs.end();

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
			auto sortedLhs = details::sortLights( lhs );
			auto sortedRhs = details::sortLights( lhs );
			auto itA = sortedLhs.begin();
			auto endItA = sortedLhs.end();
			auto itB = sortedRhs.begin();
			auto endItB = sortedRhs.end();

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
			auto sortedLhs = details::sortCameras( lhs );
			auto sortedRhs = details::sortCameras( lhs );
			auto itA = sortedLhs.begin();
			auto endItA = sortedLhs.end();
			auto itB = sortedRhs.begin();
			auto endItB = sortedRhs.end();

			while ( result && itA != endItA && itB != endItB )
			{
				if ( result
					 && itA->first.find( cuT( "_REye" ) ) == castor::String::npos
					 && itA->first.find( cuT( "_LEye" ) ) == castor::String::npos )
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
			auto sortedLhs = details::sortAnimatedGroups( lhs );
			auto sortedRhs = details::sortAnimatedGroups( lhs );
			auto itA = sortedLhs.begin();
			auto endItA = sortedLhs.end();
			auto itB = sortedRhs.begin();
			auto endItB = sortedRhs.end();

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

	bool C3DTestCase::compare( castor3d::SceneNode const & lhs, castor3d::SceneNode const & rhs )
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

	bool C3DTestCase::compare( castor3d::Animable const & lhs, castor3d::Animable const & rhs )
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

	bool C3DTestCase::compare( castor3d::MovableObject const & lhs, castor3d::MovableObject const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		result = result && CT_EQUAL( lhs.getParent()->getName(), rhs.getParent()->getName() );
		result = result && CT_EQUAL( static_cast< castor3d::Animable const & >( *lhs.getParent() ), static_cast< castor3d::Animable const & >( *rhs.getParent() ) );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Light const & lhs, castor3d::Light const & rhs )
	{
		bool result{ CT_EQUAL( static_cast< castor3d::MovableObject const & >( lhs ), static_cast< castor3d::MovableObject const & >( rhs ) ) };
		result = result && CT_EQUAL( *lhs.getCategory(), *rhs.getCategory() );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Geometry const & lhs, castor3d::Geometry const & rhs )
	{
		bool result{ CT_EQUAL( static_cast< castor3d::MovableObject const & >( lhs ), static_cast< castor3d::MovableObject const & >( rhs ) ) };
		result = result && CT_EQUAL( static_cast< castor3d::Mesh const & >( *lhs.getMesh() )
			, static_cast< castor3d::Mesh const & >( *rhs.getMesh() ) );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Camera const & lhs, castor3d::Camera const & rhs )
	{
		bool result{ CT_EQUAL( static_cast< castor3d::MovableObject const & >( lhs ), static_cast< castor3d::MovableObject const & >( rhs ) ) };
		result = result && CT_EQUAL( lhs.getViewport(), rhs.getViewport() );
		return result;
	}

	bool C3DTestCase::compare( castor3d::LightCategory const & lhs, castor3d::LightCategory const & rhs )
	{
		CT_REQUIRE( lhs.getLightType() == rhs.getLightType() );
		bool result{ CT_EQUAL( lhs.getIntensity(), rhs.getIntensity() ) };
		result = result && CT_EQUAL( lhs.getColour(), rhs.getColour() );

		switch ( lhs.getLightType() )
		{
		case castor3d::LightType::eDirectional:
			result = result && CT_EQUAL( static_cast< castor3d::DirectionalLight const & >( lhs ), static_cast< castor3d::DirectionalLight const & >( rhs ) );
			break;

		case castor3d::LightType::ePoint:
			result = result && CT_EQUAL( static_cast< castor3d::PointLight const & >( lhs ), static_cast< castor3d::PointLight const & >( rhs ) );
			break;

		case castor3d::LightType::eSpot:
			result = result && CT_EQUAL( static_cast< castor3d::SpotLight const & >( lhs ), static_cast< castor3d::SpotLight const & >( rhs ) );
			break;

		default:
			CT_FAILURE( "Unsupported LightType" );
			break;
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::DirectionalLight const & lhs, castor3d::DirectionalLight const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getDirection(), rhs.getDirection() ) };
		return result;
	}

	bool C3DTestCase::compare( castor3d::PointLight const & lhs, castor3d::PointLight const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getRange(), rhs.getRange() ) };
		return result;
	}

	bool C3DTestCase::compare( castor3d::SpotLight const & lhs, castor3d::SpotLight const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getExponent(), rhs.getExponent() ) };
		result = result && CT_EQUAL( lhs.getRange(), rhs.getRange() );
		result = result && CT_EQUAL( lhs.getInnerCutOff(), rhs.getInnerCutOff() );
		result = result && CT_EQUAL( lhs.getOuterCutOff(), rhs.getOuterCutOff() );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Viewport const & lhs, castor3d::Viewport const & rhs )
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

	bool C3DTestCase::compare( castor3d::Mesh const & lhs, castor3d::Mesh const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getSubmeshCount(), rhs.getSubmeshCount() ) };
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

	bool C3DTestCase::compare( castor3d::SkinComponent const & lhs, castor3d::SkinComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::PositionsComponent const & lhs, castor3d::PositionsComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::NormalsComponent const & lhs, castor3d::NormalsComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::TangentsComponent const & lhs, castor3d::TangentsComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::BitangentsComponent const & lhs, castor3d::BitangentsComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Texcoords0Component const & lhs, castor3d::Texcoords0Component const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Texcoords1Component const & lhs, castor3d::Texcoords1Component const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Texcoords2Component const & lhs, castor3d::Texcoords2Component const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Texcoords3Component const & lhs, castor3d::Texcoords3Component const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::ColoursComponent const & lhs, castor3d::ColoursComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Face const & lhs, castor3d::Face const & rhs )
	{
		bool result{ CT_EQUAL( lhs[0], rhs[0] ) };
		result = result && CT_EQUAL( lhs[1], rhs[1] );
		result = result && CT_EQUAL( lhs[2], rhs[2] );
		return result;
	}

	bool C3DTestCase::compare( castor3d::TriFaceMapping const & lhs, castor3d::TriFaceMapping const & rhs )
	{
		auto lhsData = lhs.getData().getFaces();
		auto rhsData = rhs.getData().getFaces();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Line const & lhs, castor3d::Line const & rhs )
	{
		bool result{ CT_EQUAL( lhs[0], rhs[0] ) };
		result = result && CT_EQUAL( lhs[1], rhs[1] );
		return result;
	}

	bool C3DTestCase::compare( castor3d::LinesMapping const & lhs, castor3d::LinesMapping const & rhs )
	{
		auto lhsData = lhs.getData().getFaces();
		auto rhsData = rhs.getData().getFaces();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::SubmeshComponent const & lhs, castor3d::SubmeshComponent const & rhs )
	{
		bool result = CT_EQUAL( lhs.getType(), rhs.getType() );
		//result = result && CT_EQUAL( lhs.getProgramFlags(), rhs.getProgramFlags() );

		if ( result )
		{
			if ( lhs.getType() == castor3d::PositionsComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::PositionsComponent const & >( lhs )
					, static_cast< castor3d::PositionsComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == castor3d::NormalsComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::NormalsComponent const & >( lhs )
					, static_cast< castor3d::NormalsComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == castor3d::TangentsComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::TangentsComponent const & >( lhs )
					, static_cast< castor3d::TangentsComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == castor3d::BitangentsComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::BitangentsComponent const & >( lhs )
					, static_cast< castor3d::BitangentsComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == castor3d::Texcoords0Component::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::Texcoords0Component const & >( lhs )
					, static_cast< castor3d::Texcoords0Component const & >( rhs ) );
			}
			else if ( lhs.getType() == castor3d::Texcoords1Component::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::Texcoords1Component const & >( lhs )
					, static_cast< castor3d::Texcoords1Component const & >( rhs ) );
			}
			else if ( lhs.getType() == castor3d::Texcoords2Component::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::Texcoords2Component const & >( lhs )
					, static_cast< castor3d::Texcoords2Component const & >( rhs ) );
			}
			else if ( lhs.getType() == castor3d::Texcoords3Component::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::Texcoords3Component const & >( lhs )
					, static_cast< castor3d::Texcoords3Component const & >( rhs ) );
			}
			else if ( lhs.getType() == castor3d::ColoursComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::ColoursComponent const & >( lhs )
					, static_cast< castor3d::ColoursComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == castor3d::SkinComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::SkinComponent const & >( lhs )
					, static_cast< castor3d::SkinComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == castor3d::TriFaceMapping::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::TriFaceMapping const & >( lhs )
					, static_cast< castor3d::TriFaceMapping const & >( rhs ) );
			}
			else if ( lhs.getType() == castor3d::LinesMapping::TypeName )
			{
				result = CT_EQUAL( static_cast< castor3d::LinesMapping const & >( lhs )
					, static_cast< castor3d::LinesMapping const & >( rhs ) );
			}
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::Submesh const & lhs, castor3d::Submesh const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getPointsCount(), rhs.getPointsCount() ) };

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

	bool C3DTestCase::compare( castor3d::Skeleton const & lhs, castor3d::Skeleton const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getGlobalInverseTransform(), rhs.getGlobalInverseTransform() ) };

		if ( result )
		{
			result = CT_EQUAL( lhs.getNodesCount(), rhs.getNodesCount() );
			auto itA = lhs.getNodes().begin();
			auto const endItA = lhs.getNodes().end();
			auto itB = rhs.getNodes().begin();
			auto const endItB = rhs.getNodes().end();

			while ( result && itA != endItA && itB != endItB )
			{
				auto & nodeA = *itA;
				auto & nodeB = *itB;
				CT_REQUIRE( nodeA != nullptr && nodeB != nullptr );
				result = CT_EQUAL( *nodeA, *nodeB );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			result = CT_EQUAL( lhs.getBonesCount(), rhs.getBonesCount() );
			auto itA = lhs.getBones().begin();
			auto const endItA = lhs.getBones().end();
			auto itB = rhs.getBones().begin();
			auto const endItB = rhs.getBones().end();

			while ( result && itA != endItA && itB != endItB )
			{
				auto & boneA = *itA;
				auto & boneB = *itB;
				CT_REQUIRE( boneA != nullptr && boneB != nullptr );
				result = CT_EQUAL( *boneA, *boneB );
				++itA;
				++itB;
			}
		}

		if ( result )
		{
			result = result && CT_EQUAL( static_cast< castor3d::Animable const & >( lhs ), static_cast< castor3d::Animable const & >( rhs ) );
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::SkeletonNode const & lhs, castor3d::SkeletonNode const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		result = result && CT_EQUAL( lhs.getParent() == nullptr, rhs.getParent() == nullptr );

		if ( result && lhs.getParent() )
		{
			result = CT_EQUAL( lhs.getParent()->getName(), rhs.getParent()->getName() );
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::BoneNode const & lhs, castor3d::BoneNode const & rhs )
	{
		bool result{ compare( static_cast< castor3d::SkeletonNode const & >( lhs )
			, static_cast< castor3d::SkeletonNode const & >( rhs ) ) };
		result = result && CT_EQUAL( lhs.getInverseTransform(), rhs.getInverseTransform() );
		result = result && CT_EQUAL( lhs.getId(), rhs.getId() );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Animation const & lhs, castor3d::Animation const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		CT_REQUIRE( lhs.getType() == rhs.getType() );

		if ( lhs.getType() == castor3d::AnimationType::eSkeleton )
		{
			result = CT_EQUAL( static_cast< castor3d::SkeletonAnimation const & >( lhs ), static_cast< castor3d::SkeletonAnimation const & >( rhs ) );
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::SkeletonAnimation const & lhs, castor3d::SkeletonAnimation const & rhs )
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
				result = CT_EQUAL( static_cast< castor3d::SkeletonAnimationKeyFrame const & >( **itA )
					, static_cast< castor3d::SkeletonAnimationKeyFrame const & >( **itB ) );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::SkeletonAnimationObject const & lhs, castor3d::SkeletonAnimationObject const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		result = result && CT_EQUAL( lhs.getInterpolationMode(), rhs.getInterpolationMode() );
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

	bool C3DTestCase::compare( castor3d::AnimationKeyFrame const & lhs, castor3d::AnimationKeyFrame const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getTimeIndex(), rhs.getTimeIndex() ) };
		return result;
	}

	bool C3DTestCase::compare( castor3d::MeshMorphTarget const & lhs, castor3d::MeshMorphTarget const & rhs )
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
				result = CT_EQUAL( itA->object->getName(), itB->object->getName() );
				result = result && CT_EQUAL( itA->transform.translate, itB->transform.translate );
				result = result && CT_EQUAL( itA->transform.scale, itB->transform.scale );
				result = result && CT_EQUAL( itA->transform.rotate, itB->transform.rotate );
				++itA;
				++itB;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::AnimatedObjectGroup const & lhs, castor3d::AnimatedObjectGroup const & rhs )
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
				result = result && CT_EQUAL( itA->second.state, itB->second.state );
				result = result && CT_EQUAL( itA->second.scale, itB->second.scale );
				result = result && CT_EQUAL( itA->second.looped, itB->second.looped );
				result = result && CT_EQUAL( itA->second.startingPoint, itB->second.startingPoint );
				result = result && CT_EQUAL( itA->second.stoppingPoint, itB->second.stoppingPoint );
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

	bool C3DTestCase::compare( castor3d::AnimatedObject const & lhs, castor3d::AnimatedObject const & rhs )
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

	bool C3DTestCase::compare( castor3d::AnimationInstance const & lhs, castor3d::AnimationInstance const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getAnimation().getName(), rhs.getAnimation().getName() ) };
		result = result && CT_EQUAL( lhs.getScale(), rhs.getScale() );
		result = result && CT_EQUAL( lhs.getState(), rhs.getState() );
		result = result && CT_EQUAL( lhs.isLooped(), rhs.isLooped() );
		CT_REQUIRE( lhs.getAnimation().getType() == rhs.getAnimation().getType() );

		switch ( lhs.getAnimation().getType() )
		{
		case castor3d::AnimationType::eSkeleton:
			result = result && CT_EQUAL( static_cast< castor3d::SkeletonAnimationInstance const & >( lhs ), static_cast< castor3d::SkeletonAnimationInstance const & >( rhs ) );
			break;

		default:
			CT_FAILURE( "Unsupported castor3d::AnimationType" );
			break;
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::SkeletonAnimationInstance const & lhs, castor3d::SkeletonAnimationInstance const & rhs )
	{
		return CT_EQUAL( lhs.getObjectsCount(), rhs.getObjectsCount() );
	}

	bool C3DTestCase::compare( castor3d::SkeletonAnimationInstanceObject const & lhs, castor3d::SkeletonAnimationInstanceObject const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getFinalTransform(), rhs.getFinalTransform() ) };
		auto & childrenA = lhs.getChildren();
		auto & childrenB = rhs.getChildren();
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

	bool C3DTestCase::compare( castor3d::SkeletonAnimationInstanceKeyFrame const & lhs, castor3d::SkeletonAnimationInstanceKeyFrame const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getTimeIndex(), rhs.getTimeIndex() ) };
		return result;
	}

	bool C3DTestCase::compare( castor3d::VertexBoneData const & lhs, castor3d::VertexBoneData const & rhs )
	{
		bool result{ CT_EQUAL( lhs.m_ids, rhs.m_ids ) };
		result = result && CT_EQUAL( lhs.m_weights, rhs.m_weights );
		return result;
	}

	bool C3DTestCase::compare( castor3d::VertexBoneData::Ids const & lhs, castor3d::VertexBoneData::Ids const & rhs )
	{
		bool result = true;
		auto itA = lhs.begin();
		auto itB = rhs.begin();

		while ( result && itA != lhs.end() )
		{
			result = CT_EQUAL( *itA, *itB );
			++itA;
			++itB;
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::VertexBoneData::Weights const & lhs, castor3d::VertexBoneData::Weights const & rhs )
	{
		bool result = true;
		auto itA = lhs.begin();
		auto itB = rhs.begin();

		while ( result && itA != lhs.end() )
		{
			result = CT_EQUAL( *itA, *itB );
			++itA;
			++itB;
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::InterleavedVertex const & lhs, castor3d::InterleavedVertex const & rhs )
	{
		bool result = CT_EQUAL( lhs.pos, rhs.pos );
		result = result && CT_EQUAL( lhs.nml, rhs.nml );
		result = result && CT_EQUAL( lhs.tan, rhs.tan );
		result = result && CT_EQUAL( lhs.tex, rhs.tex );
		return result;
	}

	//*********************************************************************************************
}
