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

			for ( auto const & [name, value] : scene.getSceneNodeCache() )
			{
				result.try_emplace( name, value.get() );
			}

			return result;
		}

		static castor::StringMap< castor3d::GeometryRPtr > sortGeometries( castor3d::Scene const & scene )
		{
			castor::StringMap< castor3d::GeometryRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getGeometryCache() );

			for ( auto const & [name, value] : scene.getGeometryCache() )
			{
				result.try_emplace( name, value.get() );
			}

			return result;
		}

		static castor::StringMap< castor3d::LightRPtr > sortLights( castor3d::Scene const & scene )
		{
			castor::StringMap< castor3d::LightRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getLightCache() );

			for ( auto const & [name, value] : scene.getLightCache() )
			{
				result.try_emplace( name, value.get() );
			}

			return result;
		}

		static castor::StringMap< castor3d::CameraRPtr > sortCameras( castor3d::Scene const & scene )
		{
			castor::StringMap< castor3d::CameraRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getCameraCache() );

			for ( auto const & [name, value] : scene.getCameraCache() )
			{
				result.try_emplace( name, value.get() );
			}

			return result;
		}

		static castor::StringMap< castor3d::AnimatedObjectGroupRPtr > sortAnimatedGroups( castor3d::Scene const & scene )
		{
			castor::StringMap< castor3d::AnimatedObjectGroupRPtr > result;
			auto lock = castor::makeUniqueLock( scene.getAnimatedObjectGroupCache() );

			for ( auto const & [name, value] : scene.getAnimatedObjectGroupCache() )
			{
				result.try_emplace( name, value.get() );
			}

			return result;
		}

		void printAllocs( castor::Vector< castor::Pair< castor3d::MemChunk, castor::String > > const & allocs
			, castor::String const & step )
		{
			castor3d::log::debug << "Allocations - " << step << std::endl;

			for ( auto const & [chunk, stack] : allocs )
			{
				castor3d::log::debug << stack << std::endl << std::endl;
			}
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
			, [this, inTest = castor::move( test )]()
			{
				auto guard = castor::makeBlockGuard( [this]()
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

	bool C3DTestCase::compare( castor::Angle const & lhs, castor::Angle const & rhs )
	{
		return CT_EQUAL( lhs.radians(), rhs.radians() );
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
			auto sortedRhs = details::sortNodes( rhs );
			auto itLhs = sortedLhs.begin();
			auto endItLhs = sortedLhs.end();
			auto itRhs = sortedRhs.begin();
			auto endItRhs = sortedRhs.end();

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				if ( result
					&& itLhs->first.find( cuT( "_REye" ) ) == castor::String::npos
					&& itLhs->first.find( cuT( "_LEye" ) ) == castor::String::npos )
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
					 && itLhs->first.find( cuT( "_REye" ) ) == castor::String::npos
					 && itLhs->first.find( cuT( "_LEye" ) ) == castor::String::npos )
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
		bool result{ CT_EQUAL( lhs.getColour(), rhs.getColour() ) };

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
		bool result{ CT_EQUAL( lhs.getIllumination(), rhs.getIllumination() ) };
		return result;
	}

	bool C3DTestCase::compare( castor3d::PointLight const & lhs, castor3d::PointLight const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getRange(), rhs.getRange() ) };
		result = result && CT_EQUAL( lhs.getIntensity(), rhs.getIntensity() );
		return result;
	}

	bool C3DTestCase::compare( castor3d::SpotLight const & lhs, castor3d::SpotLight const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getExponent(), rhs.getExponent() ) };
		result = result && CT_EQUAL( lhs.getRange(), rhs.getRange() );
		result = result && CT_EQUAL( lhs.getInnerCutOff(), rhs.getInnerCutOff() );
		result = result && CT_EQUAL( lhs.getOuterCutOff(), rhs.getOuterCutOff() );
		result = result && CT_EQUAL( lhs.getIntensity(), rhs.getIntensity() );
		return result;
	}

	bool C3DTestCase::compare( castor3d::LightInstance const & lhs, castor3d::LightInstance const & rhs )
	{
		CT_REQUIRE( lhs.getLightType() == rhs.getLightType() );
		bool result{ true };

		switch ( lhs.getLightType() )
		{
		case castor3d::LightType::eDirectional:
			result = result && CT_EQUAL( static_cast< castor3d::DirectionalLightInstance const & >( lhs ), static_cast< castor3d::DirectionalLightInstance const & >( rhs ) );
			break;

		case castor3d::LightType::ePoint:
			result = result && CT_EQUAL( static_cast< castor3d::PointLightInstance const & >( lhs ), static_cast< castor3d::PointLightInstance const & >( rhs ) );
			break;

		case castor3d::LightType::eSpot:
			result = result && CT_EQUAL( static_cast< castor3d::SpotLightInstance const & >( lhs ), static_cast< castor3d::SpotLightInstance const & >( rhs ) );
			break;

		default:
			CT_FAILURE( "Unsupported LightType" );
			break;
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::DirectionalLightInstance const & lhs, castor3d::DirectionalLightInstance const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getDirection(), rhs.getDirection() ) };
		return result;
	}

	bool C3DTestCase::compare( castor3d::PointLightInstance const & /*lhs*/, castor3d::PointLightInstance const & /*rhs*/ )
	{
		return true;
	}

	bool C3DTestCase::compare( castor3d::SpotLightInstance const & /*lhs*/, castor3d::SpotLightInstance const & /*rhs*/ )
	{
		return true;
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

	bool C3DTestCase::compare( castor3d::Skeleton const & lhs, castor3d::Skeleton const & rhs )
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
				result = CT_EQUAL( static_cast< castor3d::SkeletonAnimationKeyFrame const & >( **itLhs )
					, static_cast< castor3d::SkeletonAnimationKeyFrame const & >( **itRhs ) );
				++itLhs;
				++itRhs;
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
			auto itLhs = lhs.begin();
			auto const endItLhs = lhs.end();
			auto itRhs = rhs.begin();
			auto const endItRhs = rhs.end();
			result = CT_EQUAL( std::distance( itLhs, endItLhs ), std::distance( itRhs, endItRhs ) );

			while ( result && itLhs != endItLhs && itRhs != endItRhs )
			{
				result = CT_EQUAL( itLhs->first, itRhs->first );
				//result = result && CT_EQUAL( itLhs->second, itRhs->second );
				++itLhs;
				++itRhs;
			}
		}

		return result;
	}

	bool C3DTestCase::compare( castor3d::SkeletonAnimationKeyFrame const & lhs, castor3d::SkeletonAnimationKeyFrame const & rhs )
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

	bool C3DTestCase::compare( castor3d::AnimatedObjectGroup const & lhs, castor3d::AnimatedObjectGroup const & rhs )
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

	bool C3DTestCase::compare( castor3d::AnimatedObject const & lhs, castor3d::AnimatedObject const & rhs )
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

	bool C3DTestCase::compare( castor3d::AnimationInstance const & lhs, castor3d::AnimationInstance const & rhs )
	{
		bool result{ CT_EQUAL( lhs.getAnimation().getName(), rhs.getAnimation().getName() ) };
		result = result && CT_EQUAL( lhs.getScale(), rhs.getScale() );
		result = result && CT_EQUAL( lhs.getState(), rhs.getState() );
		result = result && CT_EQUAL( lhs.isLooped(), rhs.isLooped() );
		CT_REQUIRE( lhs.getAnimation().getType() == rhs.getAnimation().getType() );

		if ( lhs.getAnimation().getType() == castor3d::AnimationType::eSkeleton )
		{
			result = result && CT_EQUAL( static_cast< castor3d::SkeletonAnimationInstance const & >( lhs ), static_cast< castor3d::SkeletonAnimationInstance const & >( rhs ) );
		}
		else
		{
			CT_FAILURE( "Unsupported castor3d::AnimationType" );
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

	bool C3DTestCase::compare( castor3d::VertexBoneData::Weights const & lhs, castor3d::VertexBoneData::Weights const & rhs )
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

	bool C3DTestCase::compare( castor3d::InterleavedVertex const & lhs, castor3d::InterleavedVertex const & rhs )
	{
		bool result = CT_EQUAL( lhs.pos, rhs.pos );
		result = result && CT_EQUAL( lhs.nml, rhs.nml );
		result = result && CT_EQUAL( lhs.tan, rhs.tan );
		result = result && CT_EQUAL( lhs.tex, rhs.tex );
		return result;
	}

	bool C3DTestCase::compare( castor3d::AllocationStats const & lhs, castor3d::AllocationStats const & rhs )
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

	bool C3DTestCase::compare( castor3d::DeviceCounts const & lhs, castor3d::DeviceCounts const & rhs )
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

	bool C3DTestCase::compare( castor3d::EngineCounts const & lhs, castor3d::EngineCounts const & rhs )
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
