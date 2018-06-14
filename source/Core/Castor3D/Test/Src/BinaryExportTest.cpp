#include "BinaryExportTest.hpp"

#include <Engine.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/PluginCache.hpp>
#include <Cache/SceneCache.hpp>

#include <Animation/Animation.hpp>
#include <Animation/AnimationKeyFrame.hpp>
#include <Animation/Skeleton/SkeletonAnimation.hpp>
#include <Animation/Skeleton/SkeletonAnimationBone.hpp>
#include <Animation/Skeleton/SkeletonAnimationNode.hpp>
#include <Binary/BinaryMesh.hpp>
#include <Binary/BinarySkeleton.hpp>
#include <Cache/CacheView.hpp>
#include <Mesh/Importer.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Skeleton/Bone.hpp>
#include <Mesh/Skeleton/Skeleton.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Plugin/ImporterPlugin.hpp>
#include <Render/RenderLoop.hpp>
#include <Scene/SceneFileParser.hpp>

#include <Data/BinaryFile.hpp>

using namespace castor;
using namespace castor3d;

namespace Testing
{
	BinaryExportTest::BinaryExportTest( Engine & engine )
		: C3DTestCase{ "BinaryExportTest", engine }
	{
	}

	BinaryExportTest::~BinaryExportTest()
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

		auto src = scene.getMeshCache().add( name );
		Parameters parameters;
		parameters.add( cuT( "width" ), cuT( "1.0" ) );
		parameters.add( cuT( "height" ), cuT( "1.0" ) );
		parameters.add( cuT( "depth" ), cuT( "1.0" ) );
		m_engine.getMeshFactory().create( cuT( "cube" ) )->generate( *src, parameters );

		doTestMesh( src );
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

		auto src = scene.getMeshCache().add( name );
		{
			BinaryFile file{ m_testDataFolder / path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			auto result = CT_CHECK( parser.parse( *src, file ) );

			if ( result && File::fileExists( m_testDataFolder / ( name + cuT( ".cskl" ) ) ) )
			{
				auto skeleton = std::make_shared< Skeleton >( *src->getScene() );
				BinaryFile file{ m_testDataFolder / ( name + cuT( ".cskl" ) )
					, File::OpenMode::eRead };
				result = CT_CHECK( BinaryParser< Skeleton >().parse( *skeleton, file ) );

				if ( result )
				{
					src->setSkeleton( skeleton );
				}
			}
		}

		doTestMesh( src );
	}

	void BinaryExportTest::doTestMesh( MeshSPtr & src )
	{
		auto device = m_engine.getRenderSystem()->createDevice( renderer::WindowHandle{ std::make_unique< TestWindowHandle >() }, 0u );
		device->enable();
		Scene & scene = *src->getScene();
		String name = src->getName();
		Path path{ name + cuT( ".cmsh" ) };

		for ( auto & submesh : *src )
		{
			submesh->initialise();
		}

		{
			BinaryFile file{ path, File::OpenMode::eWrite };
			BinaryWriter< Mesh > writer;
			auto result = CT_CHECK( writer.write( *src, file ) );
			auto skeleton = src->getSkeleton();

			if ( result && skeleton )
			{
				BinaryFile file{ Path{ path.getFileName() + cuT( ".cskl" ) }, File::OpenMode::eWrite };
				result = CT_CHECK( BinaryWriter< Skeleton >().write( *skeleton, file ) );
			}
		}

		auto dst = scene.getMeshCache().add( name + cuT( "_imp" ) );
		{
			BinaryFile file{ path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			auto result = CT_CHECK( parser.parse( *dst, file ) );

			if ( result && File::fileExists( Path{ path.getFileName() + cuT( ".cskl" ) } ) )
			{
				auto skeleton = std::make_shared< Skeleton >( *dst->getScene() );
				BinaryFile file{ Path{ path.getFileName() + cuT( ".cskl" ) }
					, File::OpenMode::eRead };
				result = CT_CHECK( BinaryParser< Skeleton >().parse( *skeleton, file ) );

				if ( result )
				{
					dst->setSkeleton( skeleton );
				}
			}
		}

		for ( auto submesh : *dst )
		{
			submesh->initialise();
		}

		auto & lhs = *src;
		auto & rhs = *dst;
		CT_EQUAL( lhs, rhs );
		File::deleteFile( path );
		scene.cleanup();
		m_engine.getRenderLoop().renderSyncFrame();
		src->cleanup();
		dst->cleanup();
		src.reset();
		dst.reset();
		device->disable();
		m_engine.getRenderSystem()->unregisterDevice( *device );
		device.reset();
		doCleanupEngine();
	}

	//*********************************************************************************************
}
