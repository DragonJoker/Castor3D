#include "BinaryExportTest.hpp"

#include <Engine.hpp>
#include <MeshCache.hpp>
#include <PluginCache.hpp>
#include <SceneCache.hpp>

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
		String l_name = cuT( "SimpleTestMesh" );
		Path l_path{ l_name + cuT( ".cmsh" ) };
		Scene l_scene{ cuT( "TestScene" ), m_engine };

		auto l_src = l_scene.GetMeshCache().Add( l_name );
		m_engine.GetMeshFactory().Create( eMESH_TYPE_CUBE )->Generate( *l_src,  UIntArray{}, RealArray{ { 1.0_r, 1.0_r, 1.0_r } } );

		for ( auto l_submesh : *l_src )
		{
			l_submesh->Initialise();
		}

		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_WRITE };
			BinaryWriter< Mesh > l_writer;
			CT_CHECK( l_writer.Write( *l_src, l_file ) );
		}

		auto l_dst = l_scene.GetMeshCache().Add( l_name + cuT( "_imp" ) );
		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_READ };
			BinaryParser< Mesh > l_parser;
			CT_CHECK( l_parser.Parse( *l_dst, l_file ) );
		}

		for ( auto l_submesh : *l_dst )
		{
			l_submesh->Initialise();
		}

		auto & l_lhs = *l_src;
		auto & l_rhs = *l_dst;
		CT_EQUAL( l_lhs, l_rhs );
		File::DeleteFile( l_path );
		l_src.reset();
		l_dst.reset();
		l_scene.GetMeshCache().Remove( l_name );
		l_scene.GetMeshCache().Remove( l_name + cuT( "_imp" ) );
		m_engine.GetRenderLoop().Cleanup();
	}

	void BinaryExportTest::ImportExport()
	{
		String l_name = cuT( "SimpleTestMesh" );
		Path l_path{ l_name + cuT( ".cmsh" ) };
		Scene l_scene{ cuT( "TestScene" ), m_engine };

		auto l_src = l_scene.GetMeshCache().Add( l_name );
		m_engine.GetMeshFactory().Create( eMESH_TYPE_CUBE )->Generate( *l_src, UIntArray{}, RealArray{ { 1.0_r, 1.0_r, 1.0_r } } );
		{
			BinaryFile l_file{ m_testDataFolder / l_path, File::eOPEN_MODE_READ };
			BinaryParser< Mesh > l_parser;
			CT_CHECK( l_parser.Parse( *l_src, l_file ) );
		}

		for ( auto l_submesh : *l_src )
		{
			l_submesh->Initialise();
		}

		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_WRITE };
			BinaryWriter< Mesh > l_writer;
			CT_CHECK( l_writer.Write( *l_src, l_file ) );
		}

		auto l_dst = l_scene.GetMeshCache().Add( l_name + cuT( "_exp" ) );
		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_READ };
			BinaryParser< Mesh > l_parser;
			CT_CHECK( l_parser.Parse( *l_dst, l_file ) );
		}

		for ( auto l_submesh : *l_dst )
		{
			l_submesh->Initialise();
		}

		auto & l_lhs = *l_src;
		auto & l_rhs = *l_dst;
		CT_EQUAL( l_lhs, l_rhs );
		File::DeleteFile( l_path );
		l_src.reset();
		l_dst.reset();
		l_scene.GetMeshCache().Remove( l_name + cuT( "_imp" ) );
		l_scene.GetMeshCache().Remove( l_name + cuT( "_exp" ) );
		m_engine.GetRenderLoop().Cleanup();
	}

	void BinaryExportTest::AnimatedMesh()
	{
		SceneSPtr l_scene;
		SceneFileParser l_parser{ m_engine };
		CT_REQUIRE( l_parser.ParseFile( m_testDataFolder / cuT( "Anim.zip" ) ) );
		CT_REQUIRE( l_parser.ScenesBegin() != l_parser.ScenesEnd() );
		l_scene = l_parser.ScenesBegin()->second;
		l_scene->GetMeshCache().lock();
		CT_REQUIRE( l_scene->GetMeshCache().begin() != l_scene->GetMeshCache().end() );
		auto l_src = l_scene->GetMeshCache().begin()->second;
		l_scene->GetMeshCache().unlock();
		auto l_name = l_src->GetName();

		for ( auto l_submesh : *l_src )
		{
			l_submesh->Initialise();
		}

		Path l_path{ cuT( "TestMesh.cmsh" ) };
		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_WRITE };
			BinaryWriter< Mesh > l_writer;
			CT_CHECK( l_writer.Write( *l_src, l_file ) );
		}

		Scene l_sceneDst{ cuT( "TestScene" ), m_engine };
		auto l_dst = l_sceneDst.GetMeshCache().Add( l_name + cuT( "_imp" ) );
		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_READ };
			BinaryParser< Mesh > l_parser;
			CT_CHECK( l_parser.Parse( *l_dst, l_file ) );
		}

		for ( auto l_submesh : *l_dst )
		{
			l_submesh->Initialise();
		}

		auto & l_lhs = *l_src;
		auto & l_rhs = *l_dst;
		CT_EQUAL( l_lhs, l_rhs );
		File::DeleteFile( l_path );
		l_dst.reset();
		l_sceneDst.GetMeshCache().Remove( l_name + cuT( "_imp" ) );
		l_src.reset();
		l_scene->GetMeshCache().Remove( l_name );
		m_engine.GetRenderLoop().Cleanup();
		m_engine.GetSceneCache().Remove( l_scene->GetName() );
		l_scene->Cleanup();
		m_engine.GetRenderLoop().Cleanup();
	}

	//*********************************************************************************************
}
