#include "BinaryExportTest.hpp"

#include <Engine.hpp>
#include <MeshManager.hpp>
#include <PluginManager.hpp>
#include <SceneManager.hpp>

#include <Animation/Animation.hpp>
#include <Animation/KeyFrame.hpp>
#include <Animation/Skeleton/SkeletonAnimation.hpp>
#include <Animation/Skeleton/SkeletonAnimationBone.hpp>
#include <Animation/Skeleton/SkeletonAnimationNode.hpp>
#include <Manager/ManagerView.hpp>
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

		auto l_src = l_scene.GetMeshManager().Create( l_name, eMESH_TYPE_CUBE, UIntArray{}, RealArray{ { 1.0_r, 1.0_r, 1.0_r } } );

		for ( auto l_submesh : *l_src )
		{
			l_submesh->Initialise();
		}

		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_WRITE };
			CT_CHECK( BinaryWriter< Mesh >{}.Write( *l_src, l_file ) );
		}

		auto l_dst = l_scene.GetMeshManager().Create( l_name + cuT( "_imp" ), eMESH_TYPE_CUSTOM );
		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_READ };
			CT_CHECK( BinaryParser< Mesh >{}.Parse( *l_dst, l_file ) );
		}

		for ( auto l_submesh : *l_dst )
		{
			l_submesh->Initialise();
		}

		CT_EQUAL( *l_src, *l_dst );
		File::DeleteFile( l_path );
		l_src.reset();
		l_dst.reset();
		l_scene.GetMeshManager().Remove( l_name );
		l_scene.GetMeshManager().Remove( l_name + cuT( "_imp" ) );
	}

	void BinaryExportTest::ImportExport()
	{
		String l_name = cuT( "SimpleTestMesh" );
		Path l_path{ l_name + cuT( ".cmsh" ) };
		Scene l_scene{ cuT( "TestScene" ), m_engine };

		auto l_src = l_scene.GetMeshManager().Create( l_name + cuT( "_imp" ), eMESH_TYPE_CUSTOM );
		{
			BinaryFile l_file{ TEST_DATA_FOLDER / l_path, File::eOPEN_MODE_READ };
			CT_CHECK( BinaryParser< Mesh >{}.Parse( *l_src, l_file ) );
		}

		for ( auto l_submesh : *l_src )
		{
			l_submesh->Initialise();
		}

		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_WRITE };
			CT_CHECK( BinaryWriter< Mesh >{}.Write( *l_src, l_file ) );
		}

		auto l_dst = l_scene.GetMeshManager().Create( l_name + cuT( "_exp" ), eMESH_TYPE_CUSTOM );
		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_READ };
			CT_CHECK( BinaryParser< Mesh >{}.Parse( *l_dst, l_file ) );
		}

		for ( auto l_submesh : *l_dst )
		{
			l_submesh->Initialise();
		}

		CT_EQUAL( *l_src, *l_dst );
		File::DeleteFile( l_path );
		l_src.reset();
		l_dst.reset();
		l_scene.GetMeshManager().Remove( l_name + cuT( "_imp" ) );
		l_scene.GetMeshManager().Remove( l_name + cuT( "_exp" ) );
		m_engine.GetRenderLoop().Cleanup();
	}

	void BinaryExportTest::AnimatedMesh()
	{
		SceneSPtr l_scene;
		SceneFileParser l_parser{ m_engine };
		CT_REQUIRE( l_parser.ParseFile( TEST_DATA_FOLDER / cuT( "Anim.zip" ) ) );
		CT_REQUIRE( l_parser.ScenesBegin() != l_parser.ScenesEnd() );
		l_scene = l_parser.ScenesBegin()->second;
		l_scene->GetMeshManager().lock();
		CT_REQUIRE( l_scene->GetMeshManager().begin() != l_scene->GetMeshManager().end() );
		auto l_src = l_scene->GetMeshManager().begin()->second;
		l_scene->GetMeshManager().unlock();
		auto l_name = l_src->GetName();

		for ( auto l_submesh : *l_src )
		{
			l_submesh->Initialise();
		}

		Path l_path{ cuT( "TestMesh.cmsh" ) };
		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_WRITE };
			CT_CHECK( BinaryWriter< Mesh >{}.Write( *l_src, l_file ) );
		}

		Scene l_sceneDst{ cuT( "TestScene" ), m_engine };
		auto l_dst = l_sceneDst.GetMeshManager().Create( l_name + cuT( "_imp" ), eMESH_TYPE_CUSTOM );
		{
			BinaryFile l_file{ l_path, File::eOPEN_MODE_READ };
			CT_CHECK( BinaryParser< Mesh >{}.Parse( *l_dst, l_file ) );
		}

		for ( auto l_submesh : *l_dst )
		{
			l_submesh->Initialise();
		}

		CT_EQUAL( *l_src, *l_dst );
		File::DeleteFile( l_path );
		l_dst.reset();
		l_sceneDst.GetMeshManager().Remove( l_name + cuT( "_imp" ) );
		l_src.reset();
		l_scene->GetMeshManager().Remove( l_name );
		m_engine.GetRenderLoop().Cleanup();
		m_engine.GetSceneManager().Remove( l_scene->GetName() );
		l_scene->Cleanup();
		m_engine.GetRenderLoop().Cleanup();
	}

	//*********************************************************************************************
}
