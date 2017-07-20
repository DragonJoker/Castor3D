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

using namespace Castor;
using namespace Castor3D;

namespace Testing
{
	BinaryExportTest::BinaryExportTest( Engine & p_engine )
		: C3DTestCase{ "BinaryExportTest", p_engine }
	{
	}

	BinaryExportTest::~BinaryExportTest()
	{
	}

	void BinaryExportTest::DoRegisterTests()
	{
		DoRegisterTest( "BinaryExportTest::SimpleMesh", std::bind( &BinaryExportTest::SimpleMesh, this ) );
		DoRegisterTest( "BinaryExportTest::ImportExport", std::bind( &BinaryExportTest::ImportExport, this ) );
		DoRegisterTest( "BinaryExportTest::AnimatedMesh", std::bind( &BinaryExportTest::AnimatedMesh, this ) );
	}

	void BinaryExportTest::SimpleMesh()
	{
		String name = cuT( "SimpleTestMesh" );
		Path path{ name + cuT( ".cmsh" ) };
		Scene scene{ cuT( "TestScene" ), m_engine };

		auto src = scene.GetMeshCache().Add( name );
		Parameters parameters;
		parameters.Add( cuT( "width" ), cuT( "1.0" ) );
		parameters.Add( cuT( "height" ), cuT( "1.0" ) );
		parameters.Add( cuT( "depth" ), cuT( "1.0" ) );
		m_engine.GetMeshFactory().Create( cuT( "cube" ) )->Generate( *src, parameters );

		for ( auto submesh : *src )
		{
			submesh->Initialise();
		}

		{
			BinaryFile file{ path, File::OpenMode::eWrite };
			BinaryWriter< Mesh > writer;
			CT_CHECK( writer.Write( *src, file ) );
		}

		auto dst = scene.GetMeshCache().Add( name + cuT( "_imp" ) );
		{
			BinaryFile file{ path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			CT_CHECK( parser.Parse( *dst, file ) );
		}

		for ( auto submesh : *dst )
		{
			submesh->Initialise();
		}

		auto & lhs = *src;
		auto & rhs = *dst;
		CT_EQUAL( lhs, rhs );
		File::DeleteFile( path );
		scene.Cleanup();
		m_engine.GetRenderLoop().RenderSyncFrame();
		src.reset();
		dst.reset();
		DeCleanupEngine();
	}

	void BinaryExportTest::ImportExport()
	{
		String name = cuT( "SimpleTestMesh" );
		Path path{ name + cuT( ".cmsh" ) };
		Scene scene{ cuT( "TestScene" ), m_engine };

		auto src = scene.GetMeshCache().Add( name );
		Parameters parameters;
		parameters.Add( cuT( "width" ), cuT( "1.0" ) );
		parameters.Add( cuT( "height" ), cuT( "1.0" ) );
		parameters.Add( cuT( "depth" ), cuT( "1.0" ) );
		m_engine.GetMeshFactory().Create( cuT( "cube" ) )->Generate( *src, parameters );
		{
			BinaryFile file{ m_testDataFolder / path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			CT_CHECK( parser.Parse( *src, file ) );
		}

		for ( auto submesh : *src )
		{
			submesh->Initialise();
		}

		{
			BinaryFile file{ path, File::OpenMode::eWrite };
			BinaryWriter< Mesh > writer;
			CT_CHECK( writer.Write( *src, file ) );
		}

		auto dst = scene.GetMeshCache().Add( name + cuT( "_exp" ) );
		{
			BinaryFile file{ path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			CT_CHECK( parser.Parse( *dst, file ) );
		}

		for ( auto submesh : *dst )
		{
			submesh->Initialise();
		}

		auto & lhs = *src;
		auto & rhs = *dst;
		CT_EQUAL( lhs, rhs );
		File::DeleteFile( path );
		scene.Cleanup();
		m_engine.GetRenderLoop().RenderSyncFrame();
		src.reset();
		dst.reset();
		DeCleanupEngine();
	}

	void BinaryExportTest::AnimatedMesh()
	{
		SceneSPtr scene;
		SceneFileParser parser{ m_engine };
		CT_REQUIRE( parser.ParseFile( m_testDataFolder / cuT( "Anim.zip" ) ) );
		CT_REQUIRE( parser.ScenesBegin() != parser.ScenesEnd() );
		scene = parser.ScenesBegin()->second;
		scene->GetMeshCache().lock();
		CT_REQUIRE( scene->GetMeshCache().begin() != scene->GetMeshCache().end() );
		auto src = scene->GetMeshCache().begin()->second;
		scene->GetMeshCache().unlock();
		auto name = src->GetName();

		for ( auto submesh : *src )
		{
			submesh->Initialise();
		}

		Path path{ cuT( "TestMesh.cmsh" ) };
		{
			BinaryFile file{ path, File::OpenMode::eWrite };
			BinaryWriter< Mesh > writer;
			CT_CHECK( writer.Write( *src, file ) );
		}

		Scene sceneDst{ cuT( "TestScene" ), m_engine };
		auto dst = sceneDst.GetMeshCache().Add( name + cuT( "_imp" ) );
		{
			BinaryFile file{ path, File::OpenMode::eRead };
			BinaryParser< Mesh > parser;
			CT_CHECK( parser.Parse( *dst, file ) );
		}

		for ( auto submesh : *dst )
		{
			submesh->Initialise();
		}

		auto & lhs = *src;
		auto & rhs = *dst;
		CT_EQUAL( lhs, rhs );
		File::DeleteFile( path );
		scene->Cleanup();
		sceneDst.Cleanup();
		m_engine.GetSceneCache().Remove( scene->GetName() );
		m_engine.GetRenderWindowCache().Clear();
		src.reset();
		dst.reset();
		scene.reset();
		DeCleanupEngine();
	}

	//*********************************************************************************************
}
