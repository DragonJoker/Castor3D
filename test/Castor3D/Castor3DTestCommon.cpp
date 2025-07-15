#include "Castor3DTestCommon.hpp"

#include <CastorUtils/Design/BlockGuard.hpp>

#include <cmath>

namespace Testing
{
	namespace details
	{
		static c3d::StringMap< c3d::SceneNodeRPtr > sortNodes( c3d::Scene const & scene )
		{
			c3d::StringMap< c3d::SceneNodeRPtr > result;
			auto lock = c3d::makeUniqueLock( scene.getSceneNodeCache() );

			for ( auto const & [name, value] : scene.getSceneNodeCache() )
			{
				result.try_emplace( name, value.get() );
			}

			return result;
		}

		static c3d::StringMap< c3d::GeometryRPtr > sortGeometries( c3d::Scene const & scene )
		{
			c3d::StringMap< c3d::GeometryRPtr > result;
			auto lock = c3d::makeUniqueLock( scene.getGeometryCache() );

			for ( auto const & [name, value] : scene.getGeometryCache() )
			{
				result.try_emplace( name, value.get() );
			}

			return result;
		}

		static c3d::StringMap< c3d::LightRPtr > sortLights( c3d::Scene const & scene )
		{
			c3d::StringMap< c3d::LightRPtr > result;
			auto lock = c3d::makeUniqueLock( scene.getLightCache() );

			for ( auto const & [name, value] : scene.getLightCache() )
			{
				result.try_emplace( name, value.get() );
			}

			return result;
		}

		static c3d::StringMap< c3d::CameraRPtr > sortCameras( c3d::Scene const & scene )
		{
			c3d::StringMap< c3d::CameraRPtr > result;
			auto lock = c3d::makeUniqueLock( scene.getCameraCache() );

			for ( auto const & [name, value] : scene.getCameraCache() )
			{
				result.try_emplace( name, value.get() );
			}

			return result;
		}

		static c3d::StringMap< c3d::AnimatedObjectGroupRPtr > sortAnimatedGroups( c3d::Scene const & scene )
		{
			c3d::StringMap< c3d::AnimatedObjectGroupRPtr > result;
			auto lock = c3d::makeUniqueLock( scene.getAnimatedObjectGroupCache() );

			for ( auto const & [name, value] : scene.getAnimatedObjectGroupCache() )
			{
				result.try_emplace( name, value.get() );
			}

			return result;
		}

		void printAllocs( c3d::Vector< c3d::Pair< c3d::MemChunk, c3d::String > > const & allocs
			, c3d::String const & step )
		{
			c3d::log::debug << "Allocations - " << step << std::endl;

			for ( auto const & [chunk, stack] : allocs )
			{
				c3d::log::debug << stack << std::endl << std::endl;
			}
		}
	}

	C3DTestCase::C3DTestCase( std::string const & name
		, c3d::Engine & engine )
		: TestCase{ name }
		, m_engine{ engine }
		, m_testDataFolder{ c3d::Engine::getDataDirectory() / cuT( "Castor3DTest" ) / cuT( "data" ) }
	{
	}

	void C3DTestCase::doRegisterTest( std::string const & name
		, TestFunction test )
	{
		TestCase::doRegisterTest( name
			, [this, inTest = c3d::move( test )]()
			{
				auto guard = c3d::makeBlockGuard( [this]()
					{
						m_engine.initialise( 1, false );
					}
					, [this]()
					{
						m_engine.cleanup();
					} );
				inTest();
			} );
	}

	bool C3DTestCase::compare( c3d::Angle const & lhs, c3d::Angle const & rhs )
	{
		return CT_EQUAL( lhs.radians(), rhs.radians() );
	}

	bool C3DTestCase::compare( c3d::Quaternion const & lhs, c3d::Quaternion const & rhs )
	{
		c3d::Angle alphaA;
		c3d::Angle alphaB;
		c3d::Point3f axisA;
		c3d::Point3f axisB;
		lhs.toAxisAngle( axisA, alphaA );
		rhs.toAxisAngle( axisB, alphaB );
		auto result = CT_EQUAL( axisA, axisB );
		return result && CT_EQUAL( alphaA, alphaB );
	}

	bool C3DTestCase::compare( c3d::Scene const & lhs, c3d::Scene const & rhs )
	{
		bool result = true;
		{
			auto sortedLhs = details::sortNodes( lhs );
			auto sortedRhs = details::sortNodes( rhs );
			auto itLhs = sortedLhs.begin();
			auto endItLhs = sortedLhs.end();
			auto itRhs = sortedRhs.begin();
			auto endItRhs = sortedRhs.end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				if ( result
					&& itLhs->first.find( cuT( "_REye" ) ) == c3d::String::npos
					&& itLhs->first.find( cuT( "_LEye" ) ) == c3d::String::npos )
				{
					result = CT_EQUAL( itLhs->first, itRhs->first );
					result = result && CT_EQUAL( *itLhs->second, *itRhs->second );
				}

				++itLhs;
				++itRhs;
			}
		}

		if ( result )
		{
			auto sortedLhs = details::sortGeometries( lhs );
			auto sortedRhs = details::sortGeometries( rhs );
			auto itLhs = sortedLhs.begin();
			auto endItLhs = sortedLhs.end();
			auto itRhs = sortedRhs.begin();
			auto endItRhs = sortedRhs.end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				result = CT_EQUAL( itLhs->first, itRhs->first );
				result = result && CT_EQUAL( *itLhs->second, *itRhs->second );
				++itLhs;
				++itRhs;
			}
		}

		if ( result )
		{
			auto sortedLhs = details::sortLights( lhs );
			auto sortedRhs = details::sortLights( rhs );
			auto itLhs = sortedLhs.begin();
			auto endItLhs = sortedLhs.end();
			auto itRhs = sortedRhs.begin();
			auto endItRhs = sortedRhs.end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				result = CT_EQUAL( itLhs->first, itRhs->first );
				result = result && CT_EQUAL( *itLhs->second, *itRhs->second );
				++itLhs;
				++itRhs;
			}
		}

		if ( result )
		{
			auto sortedLhs = details::sortCameras( lhs );
			auto sortedRhs = details::sortCameras( rhs );
			auto itLhs = sortedLhs.begin();
			auto endItLhs = sortedLhs.end();
			auto itRhs = sortedRhs.begin();
			auto endItRhs = sortedRhs.end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				if ( result
					 && itLhs->first.find( cuT( "_REye" ) ) == c3d::String::npos
					 && itLhs->first.find( cuT( "_LEye" ) ) == c3d::String::npos )
				{
					result = CT_EQUAL( itLhs->first, itRhs->first );
					result = result && CT_EQUAL( *itLhs->second, *itRhs->second );
				}

				++itLhs;
				++itRhs;
			}
		}

		if ( result )
		{
			auto sortedLhs = details::sortAnimatedGroups( lhs );
			auto sortedRhs = details::sortAnimatedGroups( rhs );
			auto itLhs = sortedLhs.begin();
			auto endItLhs = sortedLhs.end();
			auto itRhs = sortedRhs.begin();
			auto endItRhs = sortedRhs.end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				if ( result )
				{
					result = CT_EQUAL( itLhs->first, itRhs->first );
					result = result && CT_EQUAL( *itLhs->second, *itRhs->second );
				}

				++itLhs;
				++itRhs;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::SceneNode const & lhs, c3d::SceneNode const & rhs )
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

	bool C3DTestCase::compare( c3d::Animable const & lhs, c3d::Animable const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getAnimations().size(), rhs.getAnimations().size() ) };
		auto itLhs = lhs.getAnimations().begin();
		auto const endItLhs = lhs.getAnimations().end();
		auto itRhs = rhs.getAnimations().begin();
		auto const endItRhs = rhs.getAnimations().end();

		while ( result && itLhs != endItLhs && itRhs != endItRhs )
		{
			result = CT_EQUAL( itLhs->first, itRhs->first );
			result = result && CT_EQUAL( *itLhs->second, *itRhs->second );
			++itLhs;
			++itRhs;
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::MovableObject const & lhs, c3d::MovableObject const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		result = result && CT_EQUAL( lhs.getParent()->getName(), rhs.getParent()->getName() );
		result = result && CT_EQUAL( static_cast< c3d::Animable const & >( *lhs.getParent() ), static_cast< c3d::Animable const & >( *rhs.getParent() ) );
		return result;
	}

	bool C3DTestCase::compare( c3d::Light const & lhs, c3d::Light const & rhs )
	{
		bool result{ CT_EQUAL( static_cast< c3d::MovableObject const & >( lhs ), static_cast< c3d::MovableObject const & >( rhs ) ) };
		result = result && CT_EQUAL( *lhs.getCategory(), *rhs.getCategory() );
		return result;
	}

	bool C3DTestCase::compare( c3d::Geometry const & lhs, c3d::Geometry const & rhs )
	{
		bool result{ CT_EQUAL( static_cast< c3d::MovableObject const & >( lhs ), static_cast< c3d::MovableObject const & >( rhs ) ) };
		result = result && CT_EQUAL( static_cast< c3d::Mesh const & >( *lhs.getMesh() )
			, static_cast< c3d::Mesh const & >( *rhs.getMesh() ) );
		return result;
	}

	bool C3DTestCase::compare( c3d::Camera const & lhs, c3d::Camera const & rhs )
	{
		bool result{ CT_EQUAL( static_cast< c3d::MovableObject const & >( lhs ), static_cast< c3d::MovableObject const & >( rhs ) ) };
		result = result && CT_EQUAL( lhs.getViewport(), rhs.getViewport() );
		return result;
	}

	bool C3DTestCase::compare( c3d::LightCategory const & lhs, c3d::LightCategory const & rhs )
	{
		CT_REQUIRE( lhs.getLightType() == rhs.getLightType() );
		bool result{ CT_EQUAL( lhs.getColour(), rhs.getColour() ) };

		switch ( lhs.getLightType() )
		{
		case c3d::LightType::eDirectional:
			result = result && CT_EQUAL( static_cast< c3d::DirectionalLight const & >( lhs ), static_cast< c3d::DirectionalLight const & >( rhs ) );
			break;

		case c3d::LightType::ePoint:
			result = result && CT_EQUAL( static_cast< c3d::PointLight const & >( lhs ), static_cast< c3d::PointLight const & >( rhs ) );
			break;

		case c3d::LightType::eSpot:
			result = result && CT_EQUAL( static_cast< c3d::SpotLight const & >( lhs ), static_cast< c3d::SpotLight const & >( rhs ) );
			break;

		default:
			CT_FAILURE( "Unsupported LightType" );
			break;
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::DirectionalLight const & lhs, c3d::DirectionalLight const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getIllumination(), rhs.getIllumination() ) };
		return result;
	}

	bool C3DTestCase::compare( c3d::PointLight const & lhs, c3d::PointLight const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getRange(), rhs.getRange() ) };
		result = result && CT_EQUAL( lhs.getIntensity(), rhs.getIntensity() );
		return result;
	}

	bool C3DTestCase::compare( c3d::SpotLight const & lhs, c3d::SpotLight const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getExponent(), rhs.getExponent() ) };
		result = result && CT_EQUAL( lhs.getRange(), rhs.getRange() );
		result = result && CT_EQUAL( lhs.getInnerCutOff(), rhs.getInnerCutOff() );
		result = result && CT_EQUAL( lhs.getOuterCutOff(), rhs.getOuterCutOff() );
		result = result && CT_EQUAL( lhs.getIntensity(), rhs.getIntensity() );
		return result;
	}

	bool C3DTestCase::compare( c3d::LightInstance const & lhs, c3d::LightInstance const & rhs )
	{
		CT_REQUIRE( lhs.getLightType() == rhs.getLightType() );
		bool result{ true };

		switch ( lhs.getLightType() )
		{
		case c3d::LightType::eDirectional:
			result = result && CT_EQUAL( static_cast< c3d::DirectionalLightInstance const & >( lhs ), static_cast< c3d::DirectionalLightInstance const & >( rhs ) );
			break;

		case c3d::LightType::ePoint:
			result = result && CT_EQUAL( static_cast< c3d::PointLightInstance const & >( lhs ), static_cast< c3d::PointLightInstance const & >( rhs ) );
			break;

		case c3d::LightType::eSpot:
			result = result && CT_EQUAL( static_cast< c3d::SpotLightInstance const & >( lhs ), static_cast< c3d::SpotLightInstance const & >( rhs ) );
			break;

		default:
			CT_FAILURE( "Unsupported LightType" );
			break;
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::DirectionalLightInstance const & lhs, c3d::DirectionalLightInstance const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getDirection(), rhs.getDirection() ) };
		return result;
	}

	bool C3DTestCase::compare( c3d::PointLightInstance const & /*lhs*/, c3d::PointLightInstance const & /*rhs*/ )
	{
		CT_SUCCESS();
		return true;
	}

	bool C3DTestCase::compare( c3d::SpotLightInstance const & /*lhs*/, c3d::SpotLightInstance const & /*rhs*/ )
	{
		CT_SUCCESS();
		return true;
	}

	bool C3DTestCase::compare( c3d::Viewport const & lhs, c3d::Viewport const & rhs )
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

	bool C3DTestCase::compare( c3d::Mesh const & lhs, c3d::Mesh const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getSubmeshCount(), rhs.getSubmeshCount() ) };
		auto itLhs = lhs.begin();
		auto const endItLhs = lhs.end();
		auto itRhs = rhs.begin();
		auto const endItRhs = rhs.end();

		while ( result && itLhs != endItLhs && itRhs != endItRhs )
		{
			result = CT_EQUAL( *( *( itLhs ) ), *( *( itRhs ) ) );
			++itLhs;
			++itRhs;
		}

		result = result && CT_EQUAL( lhs.getSkeleton() != nullptr, rhs.getSkeleton() != nullptr );

		if ( result && lhs.getSkeleton() )
		{
			result = CT_EQUAL( *lhs.getSkeleton(), *rhs.getSkeleton() );
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::SkinComponent const & lhs, c3d::SkinComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::PositionsComponent const & lhs, c3d::PositionsComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::NormalsComponent const & lhs, c3d::NormalsComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::TangentsComponent const & lhs, c3d::TangentsComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::BitangentsComponent const & lhs, c3d::BitangentsComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::Texcoords0Component const & lhs, c3d::Texcoords0Component const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::Texcoords1Component const & lhs, c3d::Texcoords1Component const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::Texcoords2Component const & lhs, c3d::Texcoords2Component const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::Texcoords3Component const & lhs, c3d::Texcoords3Component const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::ColoursComponent const & lhs, c3d::ColoursComponent const & rhs )
	{
		auto lhsData = lhs.getData().getData();
		auto rhsData = rhs.getData().getData();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::Face const & lhs, c3d::Face const & rhs )
	{
		bool result{ CT_EQUAL( lhs[0], rhs[0] ) };
		result = result && CT_EQUAL( lhs[1], rhs[1] );
		result = result && CT_EQUAL( lhs[2], rhs[2] );
		return result;
	}

	bool C3DTestCase::compare( c3d::TriFaceMapping const & lhs, c3d::TriFaceMapping const & rhs )
	{
		auto lhsData = lhs.getData().getFaces();
		auto rhsData = rhs.getData().getFaces();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::Line const & lhs, c3d::Line const & rhs )
	{
		bool result{ CT_EQUAL( lhs[0], rhs[0] ) };
		result = result && CT_EQUAL( lhs[1], rhs[1] );
		return result;
	}

	bool C3DTestCase::compare( c3d::LineMapping const & lhs, c3d::LineMapping const & rhs )
	{
		auto lhsData = lhs.getData().getFaces();
		auto rhsData = rhs.getData().getFaces();
		auto result = CT_EQUAL( lhsData, rhsData );
		return result;
	}

	bool C3DTestCase::compare( c3d::SubmeshComponent const & lhs, c3d::SubmeshComponent const & rhs )
	{
		bool result = CT_EQUAL( lhs.getType(), rhs.getType() );

		if ( result )
		{
			if ( lhs.getType() == c3d::PositionsComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::PositionsComponent const & >( lhs )
					, static_cast< c3d::PositionsComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == c3d::NormalsComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::NormalsComponent const & >( lhs )
					, static_cast< c3d::NormalsComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == c3d::TangentsComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::TangentsComponent const & >( lhs )
					, static_cast< c3d::TangentsComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == c3d::BitangentsComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::BitangentsComponent const & >( lhs )
					, static_cast< c3d::BitangentsComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == c3d::Texcoords0Component::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::Texcoords0Component const & >( lhs )
					, static_cast< c3d::Texcoords0Component const & >( rhs ) );
			}
			else if ( lhs.getType() == c3d::Texcoords1Component::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::Texcoords1Component const & >( lhs )
					, static_cast< c3d::Texcoords1Component const & >( rhs ) );
			}
			else if ( lhs.getType() == c3d::Texcoords2Component::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::Texcoords2Component const & >( lhs )
					, static_cast< c3d::Texcoords2Component const & >( rhs ) );
			}
			else if ( lhs.getType() == c3d::Texcoords3Component::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::Texcoords3Component const & >( lhs )
					, static_cast< c3d::Texcoords3Component const & >( rhs ) );
			}
			else if ( lhs.getType() == c3d::ColoursComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::ColoursComponent const & >( lhs )
					, static_cast< c3d::ColoursComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == c3d::SkinComponent::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::SkinComponent const & >( lhs )
					, static_cast< c3d::SkinComponent const & >( rhs ) );
			}
			else if ( lhs.getType() == c3d::TriFaceMapping::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::TriFaceMapping const & >( lhs )
					, static_cast< c3d::TriFaceMapping const & >( rhs ) );
			}
			else if ( lhs.getType() == c3d::LineMapping::TypeName )
			{
				result = CT_EQUAL( static_cast< c3d::LineMapping const & >( lhs )
					, static_cast< c3d::LineMapping const & >( rhs ) );
			}
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::Submesh const & lhs, c3d::Submesh const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getPointsCount(), rhs.getPointsCount() ) };

		if ( result )
		{
			result = result && CT_EQUAL( lhs.getComponents().size(), rhs.getComponents().size() );

			for ( auto const & [lhsName, lhsComp] : lhs.getComponents() )
			{
				if ( result )
				{
					auto itRhs = rhs.getComponents().find( lhsName );
					result = CT_CHECK( itRhs != rhs.getComponents().end() );
					result = result && CT_EQUAL( *lhsComp, *itRhs->second );
				}
			}
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::Skeleton const & lhs, c3d::Skeleton const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getGlobalInverseTransform(), rhs.getGlobalInverseTransform() ) };

		if ( result )
		{
			result = CT_EQUAL( lhs.getNodesCount(), rhs.getNodesCount() );
			auto itLhs = lhs.getNodes().begin();
			auto const endItLhs = lhs.getNodes().end();
			auto itRhs = rhs.getNodes().begin();
			auto const endItRhs = rhs.getNodes().end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				auto & nodeA = *itLhs;
				auto & nodeB = *itRhs;
				CT_REQUIRE( nodeA != nullptr && nodeB != nullptr );
				result = CT_EQUAL( *nodeA, *nodeB );
				++itLhs;
				++itRhs;
			}
		}

		if ( result )
		{
			result = CT_EQUAL( lhs.getBonesCount(), rhs.getBonesCount() );
			auto itLhs = lhs.getBones().begin();
			auto const endItLhs = lhs.getBones().end();
			auto itRhs = rhs.getBones().begin();
			auto const endItRhs = rhs.getBones().end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				auto & boneA = *itLhs;
				auto & boneB = *itRhs;
				CT_REQUIRE( boneA != nullptr && boneB != nullptr );
				result = CT_EQUAL( *boneA, *boneB );
				++itLhs;
				++itRhs;
			}
		}

		if ( result )
		{
			result = result && CT_EQUAL( static_cast< c3d::Animable const & >( lhs ), static_cast< c3d::Animable const & >( rhs ) );
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::SkeletonNode const & lhs, c3d::SkeletonNode const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		result = result && CT_EQUAL( lhs.getParent() == nullptr, rhs.getParent() == nullptr );

		if ( result && lhs.getParent() )
		{
			result = CT_EQUAL( lhs.getParent()->getName(), rhs.getParent()->getName() );
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::BoneNode const & lhs, c3d::BoneNode const & rhs )
	{
		bool result{ compare( static_cast< c3d::SkeletonNode const & >( lhs )
			, static_cast< c3d::SkeletonNode const & >( rhs ) ) };
		result = result && CT_EQUAL( lhs.getInverseTransform(), rhs.getInverseTransform() );
		result = result && CT_EQUAL( lhs.getId(), rhs.getId() );
		return result;
	}

	bool C3DTestCase::compare( c3d::Animation const & lhs, c3d::Animation const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };
		CT_REQUIRE( lhs.getType() == rhs.getType() );

		if ( lhs.getType() == c3d::AnimationType::eSkeleton )
		{
			result = CT_EQUAL( static_cast< c3d::SkeletonAnimation const & >( lhs ), static_cast< c3d::SkeletonAnimation const & >( rhs ) );
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::SkeletonAnimation const & lhs, c3d::SkeletonAnimation const & rhs )
	{
		bool result = CT_EQUAL( lhs.getObjects().size(), rhs.getObjects().size() );

		if ( result )
		{
			auto itLhs = lhs.getObjects().begin();
			auto const endItLhs = lhs.getObjects().end();
			auto itRhs = rhs.getObjects().begin();
			auto const endItRhs = rhs.getObjects().end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				result = CT_EQUAL( itLhs->first, itRhs->first );
				result = result && CT_EQUAL( *itLhs->second, *itRhs->second );
				++itLhs;
				++itRhs;
			}
		}

		result = result && CT_EQUAL( lhs.getRootObjects().size(), rhs.getRootObjects().size() );

		if ( result )
		{
			auto itLhs = lhs.getRootObjects().begin();
			auto const endItLhs = lhs.getRootObjects().end();
			auto itRhs = rhs.getRootObjects().begin();
			auto const endItRhs = rhs.getRootObjects().end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				result = CT_EQUAL( *( *itLhs ), *( *itRhs ) );
				++itLhs;
				++itRhs;
			}
		}

		if ( result )
		{
			auto itLhs = lhs.begin();
			auto const endItLhs = lhs.end();
			auto itRhs = rhs.begin();
			auto const endItRhs = rhs.end();
			result = CT_EQUAL( std::distance( itLhs, endItLhs ), std::distance( itRhs, endItRhs ) );

			while ( result && itLhs != endItLhs )
			{
				result = CT_EQUAL( static_cast< c3d::SkeletonAnimationKeyFrame const & >( **itLhs )
					, static_cast< c3d::SkeletonAnimationKeyFrame const & >( **itRhs ) );
				++itLhs;
				++itRhs;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::SkeletonAnimationObject const & lhs, c3d::SkeletonAnimationObject const & rhs )
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
			auto itLhs = lhs.getChildren().begin();
			auto const endItLhs = lhs.getChildren().end();
			auto itRhs = rhs.getChildren().begin();
			auto const endItRhs = rhs.getChildren().end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				auto objectA = *itLhs;
				auto objectB = *itRhs;
				CT_REQUIRE( objectA->getType() == objectB->getType() );
				result = CT_EQUAL( *objectA, *objectB );
				++itLhs;
				++itRhs;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::AnimationKeyFrame const & lhs, c3d::AnimationKeyFrame const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getTimeIndex(), rhs.getTimeIndex() ) };
		return result;
	}

	bool C3DTestCase::compare( c3d::MeshMorphTarget const & lhs, c3d::MeshMorphTarget const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getTimeIndex(), rhs.getTimeIndex() ) };

		if ( result )
		{
			auto itLhs = lhs.begin();
			auto const endItLhs = lhs.end();
			auto itRhs = rhs.begin();
			auto const endItRhs = rhs.end();
			result = CT_EQUAL( std::distance( itLhs, endItLhs ), std::distance( itRhs, endItRhs ) );

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				result = CT_EQUAL( itLhs->first, itRhs->first );
				++itLhs;
				++itRhs;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::SkeletonAnimationKeyFrame const & lhs, c3d::SkeletonAnimationKeyFrame const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getTimeIndex(), rhs.getTimeIndex() ) };

		if ( result )
		{
			auto itLhs = lhs.begin();
			auto const endItLhs = lhs.end();
			auto itRhs = rhs.begin();
			auto const endItRhs = rhs.end();
			result = CT_EQUAL( std::distance( itLhs, endItLhs ), std::distance( itRhs, endItRhs ) );

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				result = CT_EQUAL( itLhs->object->getName(), itRhs->object->getName() );
				result = result && CT_EQUAL( itLhs->transform.translate, itRhs->transform.translate );
				result = result && CT_EQUAL( itLhs->transform.scale, itRhs->transform.scale );
				result = result && CT_EQUAL( itLhs->transform.rotate, itRhs->transform.rotate );
				++itLhs;
				++itRhs;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::AnimatedObjectGroup const & lhs, c3d::AnimatedObjectGroup const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };

		if ( result )
		{
			result = CT_EQUAL( lhs.getAnimations().size(), rhs.getAnimations().size() );
			auto itLhs = lhs.getAnimations().begin();
			auto const endItLhs = lhs.getAnimations().end();
			auto itRhs = rhs.getAnimations().begin();
			auto const endItRhs = rhs.getAnimations().end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				result = CT_EQUAL( itLhs->first, itRhs->first );
				result = result && CT_EQUAL( itLhs->second.state, itRhs->second.state );
				result = result && CT_EQUAL( itLhs->second.scale, itRhs->second.scale );
				result = result && CT_EQUAL( itLhs->second.looped, itRhs->second.looped );
				result = result && CT_EQUAL( itLhs->second.startingPoint, itRhs->second.startingPoint );
				result = result && CT_EQUAL( itLhs->second.stoppingPoint, itRhs->second.stoppingPoint );
				++itLhs;
				++itRhs;
			}
		}

		if ( result )
		{
			result = CT_EQUAL( lhs.getObjects().size(), rhs.getObjects().size() );
			auto itLhs = lhs.getObjects().begin();
			auto const endItLhs = lhs.getObjects().end();
			auto itRhs = rhs.getObjects().begin();
			auto const endItRhs = rhs.getObjects().end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				result = CT_EQUAL( itLhs->first, itRhs->first );
				result = result && CT_EQUAL( *itLhs->second, *itRhs->second );
				++itLhs;
				++itRhs;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::AnimatedObject const & lhs, c3d::AnimatedObject const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getName(), rhs.getName() ) };

		if ( result )
		{
			result = CT_EQUAL( lhs.getAnimations().size(), rhs.getAnimations().size() );
			auto itLhs = lhs.getAnimations().begin();
			auto const endItLhs = lhs.getAnimations().end();
			auto itRhs = rhs.getAnimations().begin();
			auto const endItRhs = rhs.getAnimations().end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				result = CT_EQUAL( itLhs->first, itRhs->first );
				result = result && CT_EQUAL( *itLhs->second, *itRhs->second );
				++itLhs;
				++itRhs;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::AnimationInstance const & lhs, c3d::AnimationInstance const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getAnimation().getName(), rhs.getAnimation().getName() ) };
		result = result && CT_EQUAL( lhs.getScale(), rhs.getScale() );
		result = result && CT_EQUAL( lhs.getState(), rhs.getState() );
		result = result && CT_EQUAL( lhs.isLooped(), rhs.isLooped() );
		CT_REQUIRE( lhs.getAnimation().getType() == rhs.getAnimation().getType() );

		if ( lhs.getAnimation().getType() == c3d::AnimationType::eSkeleton )
		{
			result = result && CT_EQUAL( static_cast< c3d::SkeletonAnimationInstance const & >( lhs ), static_cast< c3d::SkeletonAnimationInstance const & >( rhs ) );
		}
		else
		{
			CT_FAILURE( "Unsupported c3d::AnimationType" );
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::SkeletonAnimationInstance const & lhs, c3d::SkeletonAnimationInstance const & rhs )
	{
		return CT_EQUAL( lhs.getObjectsCount(), rhs.getObjectsCount() );
	}

	bool C3DTestCase::compare( c3d::SkeletonAnimationInstanceObject const & lhs, c3d::SkeletonAnimationInstanceObject const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getFinalTransform(), rhs.getFinalTransform() ) };
		auto & childrenLhs = lhs.getChildren();
		auto & childrenRhs = rhs.getChildren();
		result = result && ( childrenLhs.size() == childrenRhs.size() );
		auto itLhs = childrenLhs.begin();
		auto itRhs = childrenRhs.begin();

		while ( result && itLhs != childrenLhs.end() )
		{
			result = CT_EQUAL( *itLhs, *itRhs );
			++itLhs;
			++itRhs;
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::SkeletonAnimationInstanceKeyFrame const & lhs, c3d::SkeletonAnimationInstanceKeyFrame const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getTimeIndex(), rhs.getTimeIndex() ) };
		return result;
	}

	bool C3DTestCase::compare( c3d::VertexBoneData const & lhs, c3d::VertexBoneData const & rhs )
	{
		bool result{ CT_EQUAL( lhs.m_ids, rhs.m_ids ) };
		result = result && CT_EQUAL( lhs.m_weights, rhs.m_weights );
		return result;
	}

	bool C3DTestCase::compare( c3d::VertexBoneData::Ids const & lhs, c3d::VertexBoneData::Ids const & rhs )
	{
		bool result = true;
		auto itLhs = lhs.begin();
		auto itRhs = rhs.begin();

		while ( result && itLhs != lhs.end() )
		{
			result = CT_EQUAL( *itLhs, *itRhs );
			++itLhs;
			++itRhs;
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::VertexBoneData::Weights const & lhs, c3d::VertexBoneData::Weights const & rhs )
	{
		bool result = true;
		auto itLhs = lhs.begin();
		auto itRhs = rhs.begin();

		while ( result && itLhs != lhs.end() )
		{
			result = CT_EQUAL( *itLhs, *itRhs );
			++itLhs;
			++itRhs;
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::InterleavedVertex const & lhs, c3d::InterleavedVertex const & rhs )
	{
		bool result = CT_EQUAL( lhs.pos, rhs.pos );
		result = result && CT_EQUAL( lhs.nml, rhs.nml );
		result = result && CT_EQUAL( lhs.tan, rhs.tan );
		result = result && CT_EQUAL( lhs.tex, rhs.tex );
		return result;
	}

	bool C3DTestCase::compare( c3d::AllocationStats const & lhs, c3d::AllocationStats const & rhs )
	{
		// They're both fully free (valid since buffers are not deallocated)
		if ( lhs.total == lhs.available
			&& rhs.total == rhs.available )
		{
			return true;
		}

		bool result = CT_EQUAL( lhs.total, rhs.total );
		result = result && CT_EQUAL( lhs.available, rhs.available );
		return result;
	}

	bool C3DTestCase::compare( c3d::DeviceCounts const & lhs, c3d::DeviceCounts const & rhs )
	{
		bool result = CT_EQUAL( lhs.vertexAllocated, rhs.vertexAllocated );
		result = result && CT_EQUAL( lhs.bufferAllocated, rhs.bufferAllocated );
		result = result && CT_EQUAL( lhs.indexAllocated, rhs.indexAllocated );
		result = result && CT_EQUAL( lhs.geometryAllocated, rhs.geometryAllocated );
		result = result && CT_EQUAL( lhs.uboAllocated, rhs.uboAllocated );

		if ( !compare( lhs.uboAllocated, rhs.uboAllocated ) )
		{
			details::printAllocs( lhs.uboAllocations, "Lhs" );
			details::printAllocs( rhs.uboAllocations, "Rhs" );
			result = false;
		}

		return result;
	}

	bool C3DTestCase::compare( c3d::EngineCounts const & lhs, c3d::EngineCounts const & rhs )
	{
		bool result = CT_EQUAL( lhs.fontCount, rhs.fontCount );
		result = result && CT_EQUAL( lhs.materialCount, rhs.materialCount );
		result = result && CT_EQUAL( lhs.overlayCount, rhs.overlayCount );
		result = result && CT_EQUAL( lhs.samplerCount, rhs.samplerCount );
		result = result && CT_EQUAL( lhs.device, rhs.device );
		return result;
	}

	//*********************************************************************************************
}
