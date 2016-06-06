#include "SceneExportTest.hpp"

#include <Engine.hpp>
#include <MeshManager.hpp>
#include <PluginManager.hpp>
#include <SceneManager.hpp>
#include <WindowManager.hpp>

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
#include <Render/RenderWindow.hpp>
#include <Scene/SceneFileParser.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Testing
{
	namespace
	{
		bool ExportScene( Scene const & p_scene, Path const & p_fileName )
		{
			bool l_result = true;
			Path l_folder = p_fileName.GetPath();

			if ( l_folder.empty() )
			{
				l_folder = Path{ p_fileName.GetFileName() };
			}
			else
			{
				l_folder /= p_fileName.GetFileName();
			}

			if ( !File::DirectoryExists( l_folder ) )
			{
				File::DirectoryCreate( l_folder );
			}

			Path l_filePath = l_folder / p_fileName.GetFileName();

			if ( l_result )
			{
				TextFile l_scnFile( Path{ l_filePath + cuT( ".cscn" ) }, File::eOPEN_MODE_WRITE, File::eENCODING_MODE_ASCII );
				l_result = Scene::TextWriter( String() )( p_scene, l_scnFile );
			}

			Path l_subfolder{ cuT( "Meshes" ) };

			if ( l_result )
			{
				if ( !File::DirectoryExists( l_folder / l_subfolder ) )
				{
					File::DirectoryCreate( l_folder / l_subfolder );
				}

				for ( auto const & l_name : p_scene.GetMeshView() )
				{
					auto l_mesh = p_scene.GetMeshView().Find( l_name );
					Path l_path{ l_folder / l_subfolder / l_name + cuT( ".cmsh" ) };
					BinaryFile l_file{ l_path, File::eOPEN_MODE_WRITE };
					l_result &= BinaryWriter< Mesh >{}.Write( *l_mesh, l_file );
				}
			}

			return l_result;
		}
	}

	SceneExportTest::SceneExportTest( Engine & p_engine )
		: C3DTestCase{ "SceneExportTest", p_engine }
	{
	}

	SceneExportTest::~SceneExportTest()
	{
	}

	void SceneExportTest::DoRegisterTests()
	{
		DoRegisterTest( "SceneExportTest::SimpleScene", std::bind( &SceneExportTest::SimpleScene, this ) );
		DoRegisterTest( "SceneExportTest::InstancedScene", std::bind( &SceneExportTest::InstancedScene, this ) );
		DoRegisterTest( "SceneExportTest::AlphaScene", std::bind( &SceneExportTest::AlphaScene, this ) );
		DoRegisterTest( "SceneExportTest::AnimatedScene", std::bind( &SceneExportTest::AnimatedScene, this ) );
	}

	void SceneExportTest::DoTestScene( String const & p_name )
	{
		SceneFileParser l_srcParser{ m_engine };
		CT_REQUIRE( l_srcParser.ParseFile( TEST_DATA_FOLDER / p_name ) );
		CT_REQUIRE( l_srcParser.ScenesBegin() != l_srcParser.ScenesEnd() );
		auto l_src{ l_srcParser.ScenesBegin()->second };
		CT_REQUIRE( l_src->GetRenderWindowView().begin() != l_src->GetRenderWindowView().end() );
		auto l_window{ m_engine.GetWindowManager().Find( *l_src->GetRenderWindowView().begin() ) };
		l_window->Initialise( Size{ 800, 600 }, WindowHandle{ std::make_shared< TestWindowHandle >() } );
		l_src->Initialise();

		m_engine.GetRenderLoop().RenderSyncFrame();

		Path l_path{ cuT( "TestScene.cscn" ) };
		CT_CHECK( ExportScene( *l_src, l_path ) );

		SceneFileParser l_dstParser{ m_engine };
		CT_REQUIRE( l_dstParser.ParseFile( Path{ cuT( "TestScene" ) } / cuT( "TestScene.cscn" ) ) );
		CT_REQUIRE( l_dstParser.ScenesBegin() != l_dstParser.ScenesEnd() );
		auto l_dst{ l_dstParser.ScenesBegin()->second };
		l_dst->Initialise();

		m_engine.GetRenderLoop().RenderSyncFrame();

		CT_EQUAL( *l_src, *l_dst );
		File::DirectoryDelete( Path{ cuT( "TestScene" ) } );
		l_src->Cleanup();
		l_dst->Cleanup();
		l_src.reset();
		l_dst.reset();
	}

	void SceneExportTest::SimpleScene()
	{
		DoTestScene( cuT( "primitive.cscn" ) );
	}

	void SceneExportTest::InstancedScene()
	{
		DoTestScene( cuT( "instancing.cscn" ) );
	}

	void SceneExportTest::AlphaScene()
	{
		DoTestScene( cuT( "Alpha.zip" ) );
	}

	void SceneExportTest::AnimatedScene()
	{
		DoTestScene( cuT( "Anim.zip" ) );
	}
}
