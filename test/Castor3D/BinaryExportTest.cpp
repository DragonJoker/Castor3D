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

using namespace castor;
using namespace castor3d;

namespace Testing
{
	BinaryExportTest::BinaryExportTest( Engine & engine )
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
		CT_EQUAL( sizeof( Point3f ), sizeof( float ) * 3 );
		CT_EQUAL( sizeof( Point2f ), sizeof( float ) * 2 );

		String name = cuT( "SimpleTestMesh" );
		Path path{ name + cuT( ".cmsh" ) };
		Scene scene{ cuT( "TestScene" ), m_engine };

		auto src = scene.addNewMesh( name, scene ).lock();
		CT_REQUIRE( src != nullptr );
		Parameters parameters;
		parameters.add( cuT( "width" ), cuT( "1.0" ) );
		parameters.add( cuT( "height" ), cuT( "1.0" ) );
		parameters.add( cuT( "depth" ), cuT( "1.0" ) );
		m_engine.getMeshFactory().create( cuT( "cube" ) )->generate( *src, parameters );

		doTestMesh( *src );
	}

	void BinaryExportTest::ImportExport()
	{
		doTestMeshFile( cuT( "SimpleTestMesh" ) );
	}

	void BinaryExportTest::AnimatedMesh()
	{
		doTestMeshFile( cuT( "AnimTestMesh" ) );
	}

	void BinaryExportTest::doTestMeshFile( String const & name )
	{
		Path path{ name + cuT( ".cmsh" ) };
		Scene scene{ cuT( "TestScene" ), m_engine };

		auto src = scene.addNewMesh( name, scene ).lock();
		CT_REQUIRE( src != nullptr );
		{
			BinaryFile mshfile{ m_testDataFolder / path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			auto result = CT_CHECK( parser.parse( *src, mshfile ) );

			if ( result && File::fileExists( m_testDataFolder / ( name + cuT( ".cskl" ) ) ) )
			{
				src->computeContainers();
				auto skeleton = src->getScene()->addNewSkeleton( name, *src->getScene() );
				BinaryFile sklfile{ m_testDataFolder / ( name + cuT( ".cskl" ) )
					, File::OpenMode::eRead };
				result = CT_CHECK( BinaryParser< Skeleton >().parse( *skeleton, sklfile ) );

				if ( result )
				{
					src->setSkeleton( skeleton );
				}
			}
		}

		doTestMesh( *src );
	}

	void BinaryExportTest::doTestMesh( Mesh & src )
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto surface = renderSystem.getInstance().createSurface( renderSystem.getPhysicalDevice()
			, ashes::WindowHandle{ std::make_unique< TestWindowHandle >() } );
		auto & device = renderSystem.getRenderDevice();
		Scene & scene = *src.getScene();
		String name = src.getName();
		Path path{ name + cuT( ".cmsh" ) };

		for ( auto & submesh : src )
		{
			submesh->initialise( device );
		}

		{
			BinaryFile mshfile{ path, File::OpenMode::eWrite };
			castor3d::BinaryWriter< Mesh > writer;
			auto result = CT_CHECK( writer.write( src, mshfile ) );
			auto skeleton = src.getSkeleton();

			if ( result && skeleton )
			{
				BinaryFile sklfile{ Path{ path.getFileName() + cuT( ".cskl" ) }, File::OpenMode::eWrite };
				result = CT_CHECK( castor3d::BinaryWriter< Skeleton >().write( *skeleton, sklfile ) );
			}
		}

		auto dst = scene.addNewMesh( name + cuT( "_imp" ), scene ).lock();
		CT_REQUIRE( dst != nullptr );
		{
			BinaryFile mshfile{ path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			auto result = CT_CHECK( parser.parse( *dst, mshfile ) );

			if ( result && File::fileExists( Path{ path.getFileName() + cuT( ".cskl" ) } ) )
			{
				dst->computeContainers();
				auto skeleton = dst->getScene()->addNewSkeleton( path.getFileName(), *dst->getScene() );
				BinaryFile sklfile{ Path{ path.getFileName() + cuT( ".cskl" ) }
					, File::OpenMode::eRead };
				result = CT_CHECK( BinaryParser< Skeleton >().parse( *skeleton, sklfile ) );

				if ( result )
				{
					dst->setSkeleton( skeleton );
				}
			}
		}

		for ( auto submesh : *dst )
		{
			submesh->initialise( device );
		}

		auto & rhs = static_cast< Mesh const & >( *dst );
		CT_EQUAL( src, rhs );
		File::deleteFile( path );
		scene.cleanup();
		m_engine.getRenderLoop().renderSyncFrame();
		src.cleanup();
		dst->cleanup();
		dst.reset();
		doCleanupEngine();
	}

	//*********************************************************************************************
}
