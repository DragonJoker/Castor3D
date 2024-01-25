#include "BinaryExportTest.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Animation/AnimationKeyFrame.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Model/Mesh/MeshGenerator.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Plugin/ImporterPlugin.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/Data/BinaryFile.hpp>

namespace Testing
{
	BinaryExportTest::BinaryExportTest( castor3d::Engine & engine )
		: C3DTestCase{ "BinaryExportTest", engine }
	{
	}

	void BinaryExportTest::doRegisterTests()
	{
		doRegisterTest( "BinaryExportTest::SimpleMesh", std::bind( &BinaryExportTest::SimpleMesh, this ) );
		doRegisterTest( "BinaryExportTest::ImportExport", std::bind( &BinaryExportTest::ImportExport, this ) );
		doRegisterTest( "BinaryExportTest::AnimatedMesh", std::bind( &BinaryExportTest::AnimatedMesh, this ) );
	}

	void BinaryExportTest::SimpleMesh()
	{
		CT_EQUAL( sizeof( castor::Point3f ), sizeof( float ) * 3 );
		CT_EQUAL( sizeof( castor::Point2f ), sizeof( float ) * 2 );

		castor::String name = cuT( "SimpleTestMesh" );
		castor::Path path{ name + cuT( ".cmsh" ) };
		castor3d::Scene scene{ cuT( "TestScene" ), m_engine };

		auto src = scene.addNewMesh( name, scene );
		CT_REQUIRE( src != nullptr );
		castor3d::Parameters parameters;
		parameters.add( cuT( "width" ), cuT( "1.0" ) );
		parameters.add( cuT( "height" ), cuT( "1.0" ) );
		parameters.add( cuT( "depth" ), cuT( "1.0" ) );
		m_engine.getMeshFactory().create( cuT( "cube" ) )->generate( *src, parameters );

		doTestMesh( *src );

		scene.cleanup();
		m_engine.getRenderLoop().renderSyncFrame();
	}

	void BinaryExportTest::ImportExport()
	{
		doTestMeshFile( cuT( "SimpleTestMesh" ) );
	}

	void BinaryExportTest::AnimatedMesh()
	{
		doTestMeshFile( cuT( "AnimTestMesh" ) );
	}

	void BinaryExportTest::doTestMeshFile( castor::String const & name )
	{
		castor::Path path{ name + cuT( ".cmsh" ) };
		castor3d::Scene scene{ cuT( "TestScene" ), m_engine };

		auto src = scene.addNewMesh( name, scene );
		CT_REQUIRE( src != nullptr );
		{
			castor::BinaryFile mshfile{ m_testDataFolder / path, castor::File::OpenMode::eRead };
			castor3d::BinaryParser< castor3d::Mesh > parser;
			auto result = CT_CHECK( parser.parse( *src, mshfile ) );

			if ( result && castor::File::fileExists( m_testDataFolder / ( name + cuT( ".cskl" ) ) ) )
			{
				src->computeContainers();
				auto skeleton = src->getScene()->addNewSkeleton( name, *src->getScene() );
				castor::BinaryFile sklfile{ m_testDataFolder / ( name + cuT( ".cskl" ) )
					, castor::File::OpenMode::eRead };
				result = CT_CHECK( castor3d::BinaryParser< castor3d::Skeleton >().parse( *skeleton, sklfile ) );

				if ( result )
				{
					src->setSkeleton( skeleton );
				}
			}
		}

		doTestMesh( *src );

		scene.cleanup();
		m_engine.getRenderLoop().renderSyncFrame();
	}

	void BinaryExportTest::doTestMesh( castor3d::Mesh & src )
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto surface = renderSystem.getInstance().createSurface( renderSystem.getPhysicalDevice()
			, ashes::WindowHandle{ castor::make_unique< TestWindowHandle >() } );
		auto & device = renderSystem.getRenderDevice();
		castor3d::Scene & scene = *src.getScene();
		castor::String name = src.getName();
		castor::Path path{ name + cuT( ".cmsh" ) };

		for ( auto & submesh : src )
		{
			submesh->initialise( device );
		}

		{
			castor::BinaryFile mshfile{ path, castor::File::OpenMode::eWrite };
			castor3d::BinaryWriter< castor3d::Mesh > writer;
			auto result = CT_CHECK( writer.write( src, mshfile ) );
			auto skeleton = src.getSkeleton();

			if ( result && skeleton )
			{
				castor::BinaryFile sklfile{ castor::Path{ path.getFileName() + cuT( ".cskl" ) }, castor::File::OpenMode::eWrite };
				result = CT_CHECK( castor3d::BinaryWriter< castor3d::Skeleton >().write( *skeleton, sklfile ) );
			}
		}

		auto dst = scene.createMesh( name + cuT( "_imp" ), scene );
		CT_REQUIRE( dst != nullptr );
		{
			castor::BinaryFile mshfile{ path, castor::File::OpenMode::eRead };
			castor3d::BinaryParser< castor3d::Mesh > parser;
			auto result = CT_CHECK( parser.parse( *dst, mshfile ) );

			if ( result && castor::File::fileExists( castor::Path{ path.getFileName() + cuT( ".cskl" ) } ) )
			{
				dst->computeContainers();
				auto skeleton = dst->getScene()->addNewSkeleton( path.getFileName(), *dst->getScene() );
				castor::BinaryFile sklfile{ castor::Path{ path.getFileName() + cuT( ".cskl" ) }
					, castor::File::OpenMode::eRead };
				result = CT_CHECK( castor3d::BinaryParser< castor3d::Skeleton >().parse( *skeleton, sklfile ) );

				if ( result )
				{
					dst->setSkeleton( skeleton );
				}
			}

			dst->initialise();
		}

		auto & rhs = static_cast< castor3d::Mesh const & >( *dst );
		CT_EQUAL( src, rhs );
		castor::File::deleteFile( path );
		m_engine.getRenderLoop().renderSyncFrame();
		dst->cleanup();
		m_engine.getRenderLoop().renderSyncFrame();
	}

	//*********************************************************************************************
}
