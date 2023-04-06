#include "Castor3DTestCommon.hpp"

#include <CastorUtils/Design/BlockGuard.hpp>

#include <cmath>

using namespace castor;
using namespace castor3d;

namespace Testing
{
	namespace details
	{
		std::map< castor::String, SceneNodeRPtr > sortNodes( Scene const & scene )
		{
			std::map< castor::String, SceneNodeRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getSceneNodeCache() );

			for ( auto & it : scene.getSceneNodeCache() )
			{
				result.emplace( it.first, it.second.get() );
			}

			return result;
		}

		std::map< castor::String, GeometryRPtr > sortGeometries( Scene const & scene )
		{
			std::map< castor::String, GeometryRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getGeometryCache() );

			for ( auto & it : scene.getGeometryCache() )
			{
				result.emplace( it.first, it.second.get() );
			}

			return result;
		}

		std::map< castor::String, LightRPtr > sortLights( Scene const & scene )
		{
			std::map< castor::String, LightRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getLightCache() );

			for ( auto & it : scene.getLightCache() )
			{
				result.emplace( it.first, it.second.get() );
			}

			return result;
		}

		std::map< castor::String, CameraRPtr > sortCameras( Scene const & scene )
		{
			std::map< castor::String, CameraRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getCameraCache() );

			for ( auto & it : scene.getCameraCache() )
			{
				result.emplace( it.first, it.second.get() );
			}

			return result;
		}

		std::map< castor::String, AnimatedObjectGroupRPtr > sortAnimatedGroups( Scene const & scene )
		{
			std::map< castor::String, AnimatedObjectGroupRPtr > result;
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
		, m_testDataFolder{ Engine::getDataDirectory() / cuT( "Castor3DTest" ) / cuT( "data" ) }
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
		result = result && CT_EQUAL( static_cast< Animable const & >( *lhs.getParent() ), static_cast< Animable const & >( *rhs.getParent() ) );
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
		result = result && CT_EQUAL( static_cast< Mesh const & >( *lhs.getMesh().lock() )
			, static_cast< Mesh const & >( *rhs.getMesh().lock() ) );
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

		default:
			CT_FAILURE( "Unsupported LightType" );
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
		result = result && CT_EQUAL( lhs.getInnerCutOff(), rhs.getInnerCutOff() );
		result = result && CT_EQUAL( lhs.getOuterCutOff(), rhs.getOuterCutOff() );
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

	bool C3DTestCase::compare( SkinComponent const & lhs, SkinComponent const & rhs )
	{
		auto lhsData = lhs.getData();
		auto rhsData = rhs.getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::PositionsComponent const & lhs, castor3d::PositionsComponent const & rhs )
	{
		auto lhsData = lhs.getData();
		auto rhsData = rhs.getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::NormalsComponent const & lhs, castor3d::NormalsComponent const & rhs )
	{
		auto lhsData = lhs.getData();
		auto rhsData = rhs.getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::TangentsComponent const & lhs, castor3d::TangentsComponent const & rhs )
	{
		auto lhsData = lhs.getData();
		auto rhsData = rhs.getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Texcoords0Component const & lhs, castor3d::Texcoords0Component const & rhs )
	{
		auto lhsData = lhs.getData();
		auto rhsData = rhs.getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Texcoords1Component const & lhs, castor3d::Texcoords1Component const & rhs )
	{
		auto lhsData = lhs.getData();
		auto rhsData = rhs.getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Texcoords2Component const & lhs, castor3d::Texcoords2Component const & rhs )
	{
		auto lhsData = lhs.getData();
		auto rhsData = rhs.getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::Texcoords3Component const & lhs, castor3d::Texcoords3Component const & rhs )
	{
		auto lhsData = lhs.getData();
		auto rhsData = rhs.getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( castor3d::ColoursComponent const & lhs, castor3d::ColoursComponent const & rhs )
	{
		auto lhsData = lhs.getData();
		auto rhsData = rhs.getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( Face const & lhs, Face const & rhs )
	{
		bool result{ CT_EQUAL( lhs[0], rhs[0] ) };
		result = result && CT_EQUAL( lhs[1], rhs[1] );
		result = result && CT_EQUAL( lhs[2], rhs[2] );
		return result;
	}

	bool C3DTestCase::compare( TriFaceMapping const & lhs, TriFaceMapping const & rhs )
	{
		auto lhsData = lhs.getFaces();
		auto rhsData = rhs.getFaces();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( Line const & lhs, Line const & rhs )
	{
		bool result{ CT_EQUAL( lhs[0], rhs[0] ) };
		result = result && CT_EQUAL( lhs[1], rhs[1] );
		return result;
	}

	bool C3DTestCase::compare( LinesMapping const & lhs, LinesMapping const & rhs )
	{
		auto lhsData = lhs.getFaces();
		auto rhsData = rhs.getFaces();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( SubmeshComponent const & lhs, SubmeshComponent const & rhs )
	{
		bool result = CT_EQUAL( lhs.getType(), rhs.getType() );
		//result = result && CT_EQUAL( lhs.getProgramFlags(), rhs.getProgramFlags() );

		if ( result )
		{
			if ( lhs.getType() == PositionsComponent::Name )
			{
				result = CT_EQUAL( static_cast< PositionsComponent const & >( lhs )
					, static_cast< PositionsComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == NormalsComponent::Name )
			{
				result = CT_EQUAL( static_cast< NormalsComponent const & >( lhs )
					, static_cast< NormalsComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == TangentsComponent::Name )
			{
				result = CT_EQUAL( static_cast< TangentsComponent const & >( lhs )
					, static_cast< TangentsComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == Texcoords0Component::Name )
			{
				result = CT_EQUAL( static_cast< Texcoords0Component const & >( lhs )
					, static_cast< Texcoords0Component const & >( rhs ) );
			}
			else if ( lhs.getType() == Texcoords1Component::Name )
			{
				result = CT_EQUAL( static_cast< Texcoords1Component const & >( lhs )
					, static_cast< Texcoords1Component const & >( rhs ) );
			}
			else if ( lhs.getType() == Texcoords2Component::Name )
			{
				result = CT_EQUAL( static_cast< Texcoords2Component const & >( lhs )
					, static_cast< Texcoords2Component const & >( rhs ) );
			}
			else if ( lhs.getType() == Texcoords3Component::Name )
			{
				result = CT_EQUAL( static_cast< Texcoords3Component const & >( lhs )
					, static_cast< Texcoords3Component const & >( rhs ) );
			}
			else if ( lhs.getType() == ColoursComponent::Name )
			{
				result = CT_EQUAL( static_cast< ColoursComponent const & >( lhs )
					, static_cast< ColoursComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == SkinComponent::Name )
			{
				result = CT_EQUAL( static_cast< SkinComponent const & >( lhs )
					, static_cast< SkinComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == TriFaceMapping::Name )
			{
				result = CT_EQUAL( static_cast< TriFaceMapping const & >( lhs )
					, static_cast< TriFaceMapping const & >( rhs ) );
			}
			else if ( lhs.getType() == LinesMapping::Name )
			{
				result = CT_EQUAL( static_cast< LinesMapping const & >( lhs )
					, static_cast< LinesMapping const & >( rhs ) );
			}
		}

		return result;
	}

	bool C3DTestCase::compare( Submesh const & lhs, Submesh const & rhs )
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

	bool C3DTestCase::compare( Skeleton const & lhs, Skeleton const & rhs )
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
			result = result && CT_EQUAL( static_cast< Animable const & >( lhs ), static_cast< Animable const & >( rhs ) );
		}

		return result;
	}

	bool C3DTestCase::compare( SkeletonNode const & lhs, SkeletonNode const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		result = result && CT_EQUAL( lhs.getParent() == nullptr, rhs.getParent() == nullptr );

		if ( result && lhs.getParent() )
		{
			result = CT_EQUAL( lhs.getParent()->getName(), rhs.getParent()->getName() );
		}

		return result;
	}

	bool C3DTestCase::compare( BoneNode const & lhs, BoneNode const & rhs )
	{
		bool result{ compare( static_cast< SkeletonNode const & >( lhs )
			, static_cast< SkeletonNode const & >( rhs ) ) };
		result = result && CT_EQUAL( lhs.getInverseTransform(), rhs.getInverseTransform() );
		result = result && CT_EQUAL( lhs.getId(), rhs.getId() );
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

		default:
			CT_FAILURE( "Unsupported AnimationType" );
			break;
		}

		return result;
	}

	bool C3DTestCase::compare( SkeletonAnimationInstance const & lhs, SkeletonAnimationInstance const & rhs )
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
