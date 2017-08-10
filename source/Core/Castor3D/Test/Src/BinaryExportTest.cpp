#include "BinaryExportTest.hpp"

#include <Engine.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/PluginCache.hpp>
#include <Cache/SceneCache.hpp>

#include <Animation/Animation.hpp>
#include <Animation/KeyFrame.hpp>
#include <Animation/Skeleton/SkeletonAnimation.hpp>
#include <Animation/Skeleton/SkeletonAnimationBone.hpp>
#include <Animation/Skeleton/SkeletonAnimationNode.hpp>
#include <Cache/CacheView.hpp>
#include <Mesh/Importer.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
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
		String name = cuT( "SimpleTestMesh" );
		Path path{ name + cuT( ".cmsh" ) };
		Scene scene{ cuT( "TestScene" ), m_engine };

		CT_EQUAL( sizeof( Point3f ), sizeof( float ) * 3 );
		CT_EQUAL( sizeof( Point2f ), sizeof( float ) * 2 );

		auto src = scene.getMeshCache().add( name );
		Parameters parameters;
		parameters.add( cuT( "width" ), cuT( "1.0" ) );
		parameters.add( cuT( "height" ), cuT( "1.0" ) );
		parameters.add( cuT( "depth" ), cuT( "1.0" ) );
		m_engine.getMeshFactory().create( cuT( "cube" ) )->generate( *src, parameters );

		for ( auto submesh : *src )
		{
			submesh->initialise();
		}

		{
			BinaryFile file{ path, File::OpenMode::eWrite };
			BinaryWriter< Mesh > writer;
			CT_CHECK( writer.write( *src, file ) );
		}

		auto dst = scene.getMeshCache().add( name + cuT( "_imp" ) );
		{
			BinaryFile file{ path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			CT_CHECK( parser.parse( *dst, file ) );
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
		src.reset();
		dst.reset();
		DeCleanupEngine();
	}

	void BinaryExportTest::ImportExport()
	{
		String name = cuT( "SimpleTestMesh" );
		Path path{ name + cuT( ".cmsh" ) };
		Scene scene{ cuT( "TestScene" ), m_engine };

		auto src = scene.getMeshCache().add( name );
		Parameters parameters;
		parameters.add( cuT( "width" ), cuT( "1.0" ) );
		parameters.add( cuT( "height" ), cuT( "1.0" ) );
		parameters.add( cuT( "depth" ), cuT( "1.0" ) );
		m_engine.getMeshFactory().create( cuT( "cube" ) )->generate( *src, parameters );
		{
			BinaryFile file{ m_testDataFolder / path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			CT_CHECK( parser.parse( *src, file ) );
		}

		for ( auto submesh : *src )
		{
			submesh->initialise();
		}

		{
			BinaryFile file{ path, File::OpenMode::eWrite };
			BinaryWriter< Mesh > writer;
			CT_CHECK( writer.write( *src, file ) );
		}

		auto dst = scene.getMeshCache().add( name + cuT( "_exp" ) );
		{
			BinaryFile file{ path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			CT_CHECK( parser.parse( *dst, file ) );
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
		src.reset();
		dst.reset();
		DeCleanupEngine();
	}

	void BinaryExportTest::AnimatedMesh()
	{
		SceneSPtr scene;
		SceneFileParser parser{ m_engine };
		CT_REQUIRE( parser.parseFile( m_testDataFolder / cuT( "Anim.zip" ) ) );
		CT_REQUIRE( parser.scenesBegin() != parser.scenesEnd() );
		scene = parser.scenesBegin()->second;
		scene->getMeshCache().lock();
		CT_REQUIRE( scene->getMeshCache().begin() != scene->getMeshCache().end() );
		auto src = scene->getMeshCache().begin()->second;
		scene->getMeshCache().unlock();
		auto name = src->getName();

		for ( auto submesh : *src )
		{
			submesh->initialise();
		}

		Path path{ cuT( "TestMesh.cmsh" ) };
		{
			BinaryFile file{ path, File::OpenMode::eWrite };
			BinaryWriter< Mesh > writer;
			CT_CHECK( writer.write( *src, file ) );
		}

		Scene sceneDst{ cuT( "TestScene" ), m_engine };
		auto dst = sceneDst.getMeshCache().add( name + cuT( "_imp" ) );
		{
			BinaryFile file{ path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			CT_CHECK( parser.parse( *dst, file ) );
		}

		for ( auto submesh : *dst )
		{
			submesh->initialise();
		}

		auto & lhs = *src;
		auto & rhs = *dst;
		CT_EQUAL( lhs, rhs );
		File::deleteFile( path );
		scene->cleanup();
		sceneDst.cleanup();
		m_engine.getSceneCache().remove( scene->getName() );
		m_engine.getRenderWindowCache().clear();
		src.reset();
		dst.reset();
		scene.reset();
		DeCleanupEngine();
	}

	//*********************************************************************************************
}
