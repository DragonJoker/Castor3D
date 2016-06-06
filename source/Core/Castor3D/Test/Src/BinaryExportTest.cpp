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

		auto l_src = m_engine.GetMeshManager().Create( l_name, eMESH_TYPE_CUBE, UIntArray{}, RealArray{ { 1.0_r, 1.0_r, 1.0_r } } );

		for ( auto l_submesh : *l_src )
		{
			l_submesh->Initialise();
		}

		CT_CHECK( BinaryWriter< Mesh >{}.Write( *l_src, BinaryFile{ l_path, File::eOPEN_MODE_WRITE } ) );

		auto l_dst = m_engine.GetMeshManager().Create( l_name + cuT( "_imp" ), eMESH_TYPE_CUSTOM );
		CT_CHECK( BinaryParser< Mesh >{}.Parse( *l_dst, BinaryFile{ l_path, File::eOPEN_MODE_READ } ) );

		for ( auto l_submesh : *l_dst )
		{
			l_submesh->Initialise();
		}

		CT_EQUAL( *l_src, *l_dst );
		File::DeleteFile( l_path );
		l_src.reset();
		l_dst.reset();
		m_engine.GetMeshManager().Remove( l_name );
		m_engine.GetMeshManager().Remove( l_name + cuT( "_imp" ) );
	}

	void BinaryExportTest::ImportExport()
	{
		String l_name = cuT( "SimpleTestMesh" );
		Path l_path{ l_name + cuT( ".cmsh" ) };

		auto l_src = m_engine.GetMeshManager().Create( l_name + cuT( "_imp" ), eMESH_TYPE_CUSTOM );
		CT_CHECK( BinaryParser< Mesh >{}.Parse( *l_src, BinaryFile{ TEST_DATA_FOLDER / l_path, File::eOPEN_MODE_READ } ) );

		for ( auto l_submesh : *l_src )
		{
			l_submesh->Initialise();
		}

		CT_CHECK( BinaryWriter< Mesh >{}.Write( *l_src, BinaryFile{ l_path, File::eOPEN_MODE_WRITE } ) );

		auto l_dst = m_engine.GetMeshManager().Create( l_name + cuT( "_exp" ), eMESH_TYPE_CUSTOM );
		CT_CHECK( BinaryParser< Mesh >{}.Parse( *l_dst, BinaryFile{ l_path, File::eOPEN_MODE_READ } ) );

		for ( auto l_submesh : *l_dst )
		{
			l_submesh->Initialise();
		}

		CT_EQUAL( *l_src, *l_dst );
		File::DeleteFile( l_path );
		l_src.reset();
		l_dst.reset();
		m_engine.GetMeshManager().Remove( l_name + cuT( "_imp" ) );
		m_engine.GetMeshManager().Remove( l_name + cuT( "_exp" ) );
	}

	void BinaryExportTest::AnimatedMesh()
	{
		SceneSPtr l_scene;
		SceneFileParser l_parser{ m_engine };
		CT_REQUIRE( l_parser.ParseFile( TEST_DATA_FOLDER / cuT( "Anim.zip" ) ) );
		CT_REQUIRE( l_parser.ScenesBegin() != l_parser.ScenesEnd() );
		l_scene = l_parser.ScenesBegin()->second;
		CT_REQUIRE( l_scene->GetMeshView().begin() != l_scene->GetMeshView().end() );
		String l_name = *l_scene->GetMeshView().begin();
		auto l_src = l_scene->GetMeshView().Find( l_name );

		for ( auto l_submesh : *l_src )
		{
			l_submesh->Initialise();
		}

		Path l_path{ cuT( "TestMesh.cmsh" ) };
		{
			CT_CHECK( BinaryWriter< Mesh >{}.Write( *l_src, BinaryFile{ l_path, File::eOPEN_MODE_WRITE } ) );
		}

		auto l_dst = m_engine.GetMeshManager().Create( l_name + cuT( "_imp" ), eMESH_TYPE_CUSTOM );
		{
			CT_CHECK( BinaryParser< Mesh >{}.Parse( *l_dst, BinaryFile{ l_path, File::eOPEN_MODE_READ } ) );
		}

		for ( auto l_submesh : *l_dst )
		{
			l_submesh->Initialise();
		}

		CT_EQUAL( *l_src, *l_dst );
		File::DeleteFile( l_path );
		l_dst.reset();
		m_engine.GetMeshManager().Remove( l_name + cuT( "_imp" ) );
		l_src.reset();
		l_scene->GetMeshView().Remove( l_name );
		m_engine.GetRenderLoop().Cleanup();
		m_engine.GetSceneManager().Remove( l_scene->GetName() );
		l_scene->Cleanup();
	}

	//*********************************************************************************************
}
