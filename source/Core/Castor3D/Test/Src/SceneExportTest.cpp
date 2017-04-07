#include "SceneExportTest.hpp"

#include <Engine.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/PluginCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/WindowCache.hpp>

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
#include <Render/RenderWindow.hpp>
#include <Scene/SceneFileParser.hpp>

#include <Data/BinaryFile.hpp>

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
				TextFile l_scnFile( Path{ l_filePath + cuT( ".cscn" ) }, File::OpenMode::eWrite, File::EncodingMode::eASCII );
				l_result = Scene::TextWriter( String() )( p_scene, l_scnFile );
			}

			Path l_subfolder{ cuT( "Meshes" ) };

			if ( l_result )
			{
				if ( !File::DirectoryExists( l_folder / l_subfolder ) )
				{
					File::DirectoryCreate( l_folder / l_subfolder );
				}

				auto l_lock = make_unique_lock( p_scene.GetMeshCache() );

				for ( auto const & l_it : p_scene.GetMeshCache() )
				{
					auto l_mesh = l_it.second;
					Path l_path{ l_folder / l_subfolder / l_it.first + cuT( ".cmsh" ) };
					BinaryFile l_file{ l_path, File::OpenMode::eWrite };
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
		DoTestScene( cuT( "Msaa.zip" ) );
	}

	void SceneExportTest::AnimatedScene()
	{
		DoTestScene( cuT( "Anim.zip" ) );
	}

	SceneSPtr SceneExportTest::DoParseScene( Path const & p_path )
	{
		SceneFileParser l_dstParser{ m_engine };
		CT_REQUIRE( l_dstParser.ParseFile( p_path ) );
		CT_REQUIRE( l_dstParser.ScenesBegin() != l_dstParser.ScenesEnd() );
		SceneSPtr l_scene{ l_dstParser.ScenesBegin()->second };
		auto & l_windows = m_engine.GetRenderWindowCache();
		RenderWindowSPtr l_window;

		l_windows.lock();
		auto l_it = std::find_if( l_windows.begin()
			, l_windows.end()
			, [l_scene]( auto & p_pair )
			{
				return p_pair.second->GetScene()->GetName() == l_scene->GetName();
			} );

		if ( l_it != l_windows.end() )
		{
			l_window = l_it->second;
		}

		l_windows.unlock();

		if ( l_window )
		{
			l_window->Initialise( Size{ 800, 600 }, WindowHandle{ std::make_shared< TestWindowHandle >() } );
			m_engine.GetRenderLoop().RenderSyncFrame();
		}

		return l_scene;
	}

	RenderWindowSPtr GetWindow( Engine & p_engine, String const & p_sceneName )
	{
		auto & l_windows = p_engine.GetRenderWindowCache();
		l_windows.lock();
		auto l_it = std::find_if( l_windows.begin()
			, l_windows.end()
			, [p_sceneName]( auto & p_pair )
		{
			return p_pair.second->GetScene()->GetName() == p_sceneName;
		} );

		RenderWindowSPtr l_window;

		if ( l_it != l_windows.end() )
		{
			l_window = l_it->second;
			l_window->SetName( l_it->first + cuT( "_exp" ) );
			l_windows.Remove( l_it->first );
			l_windows.Add( l_window->GetName(), l_window );
		}

		l_windows.unlock();
		return l_window;
	}

	void SceneExportTest::DoTestScene( String const & p_name )
	{
		SceneSPtr l_src{ DoParseScene( m_testDataFolder / p_name ) };
		Path l_path{ cuT( "TestScene.cscn" ) };
		CT_CHECK( ExportScene( *l_src, l_path ) );

		auto l_name = l_src->GetName();
		m_engine.GetSceneCache().Remove( l_name );
		l_src->SetName( l_name + cuT( "_exp" ) );
		m_engine.GetSceneCache().Add( l_src->GetName(), l_src );
		auto l_srcWindow = GetWindow( m_engine, l_src->GetName() );
		CT_CHECK( l_srcWindow != nullptr );

		SceneSPtr l_dst{ DoParseScene( Path{ cuT( "TestScene" ) } / cuT( "TestScene.cscn" ) ) };
		CT_EQUAL( *l_src, *l_dst );
		auto l_dstWindow = GetWindow( m_engine, l_dst->GetName() );
		CT_CHECK( l_dstWindow != nullptr );
		File::DirectoryDelete( Path{ cuT( "TestScene" ) } );
		l_src->Cleanup();
		l_srcWindow->Cleanup();
		m_engine.GetRenderLoop().RenderSyncFrame();
		m_engine.GetSceneCache().Remove( l_name + cuT( "_exp" ) );
		m_engine.GetRenderWindowCache().Remove( l_srcWindow->GetName() + cuT( "_exp" ) );
		l_src.reset();
		l_dst->Cleanup();
		l_dstWindow->Cleanup();
		m_engine.GetRenderLoop().RenderSyncFrame();
		m_engine.GetSceneCache().Remove( l_name );
		m_engine.GetRenderWindowCache().Remove( l_dstWindow->GetName() );
		l_dst.reset();
	}
}
