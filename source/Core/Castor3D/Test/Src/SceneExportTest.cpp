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
			bool result = true;
			Path folder = p_fileName.GetPath();

			if ( folder.empty() )
			{
				folder = Path{ p_fileName.GetFileName() };
			}
			else
			{
				folder /= p_fileName.GetFileName();
			}

			if ( !File::DirectoryExists( folder ) )
			{
				File::DirectoryCreate( folder );
			}

			Path filePath = folder / p_fileName.GetFileName();

			if ( result )
			{
				TextFile scnFile( Path{ filePath + cuT( ".cscn" ) }, File::OpenMode::eWrite, File::EncodingMode::eASCII );
				result = Scene::TextWriter( String() )( p_scene, scnFile );
			}

			Path subfolder{ cuT( "Meshes" ) };

			if ( result )
			{
				if ( !File::DirectoryExists( folder / subfolder ) )
				{
					File::DirectoryCreate( folder / subfolder );
				}

				auto lock = make_unique_lock( p_scene.GetMeshCache() );

				for ( auto const & it : p_scene.GetMeshCache() )
				{
					auto mesh = it.second;
					Path path{ folder / subfolder / it.first + cuT( ".cmsh" ) };
					BinaryFile file{ path, File::OpenMode::eWrite };
					result &= BinaryWriter< Mesh >{}.Write( *mesh, file );
				}
			}

			return result;
		}

		template< typename ObjT, typename CacheT >
		void RenameObject( ObjT p_object, CacheT & p_cache )
		{
			auto name = p_object->GetName();
			p_object->SetName( name + cuT( "_ren" ) );
			p_cache.Remove( name );
			p_cache.Add( p_object->GetName(), p_object );
		}

		RenderWindowSPtr GetWindow( Engine & p_engine
			, String const & p_sceneName
			, bool p_rename )
		{
			auto & windows = p_engine.GetRenderWindowCache();
			windows.lock();
			auto it = std::find_if( windows.begin()
				, windows.end()
				, [p_sceneName]( auto & p_pair )
			{
				return p_pair.second->GetScene()->GetName() == p_sceneName;
			} );

			RenderWindowSPtr window;

			if ( it != windows.end() )
			{
				window = it->second;

				if ( p_rename )
				{
					RenameObject( window, windows );
				}
			}

			windows.unlock();
			return window;
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
		DoTestScene( cuT( "Alpha.zip" ) );
	}

	void SceneExportTest::AnimatedScene()
	{
		DoTestScene( cuT( "Anim.zip" ) );
	}

	SceneSPtr SceneExportTest::DoParseScene( Path const & p_path )
	{
		SceneFileParser dstParser{ m_engine };
		CT_REQUIRE( dstParser.ParseFile( p_path ) );
		CT_REQUIRE( dstParser.ScenesBegin() != dstParser.ScenesEnd() );
		SceneSPtr scene{ dstParser.ScenesBegin()->second };
		auto & windows = m_engine.GetRenderWindowCache();
		auto window = GetWindow( m_engine, scene->GetName(), false );

		if ( window )
		{
			window->Initialise( Size{ 800, 600 }, WindowHandle{ std::make_shared< TestWindowHandle >() } );
			m_engine.GetRenderLoop().RenderSyncFrame();
		}

		return scene;
	}

	void SceneExportTest::DoTestScene( String const & p_name )
	{
		SceneSPtr src{ DoParseScene( m_testDataFolder / p_name ) };
		Path path{ cuT( "TestScene.cscn" ) };
		CT_CHECK( ExportScene( *src, path ) );
		
		RenameObject( src, m_engine.GetSceneCache() );
		auto srcWindow = GetWindow( m_engine, src->GetName(), true );
		CT_CHECK( srcWindow != nullptr );

		SceneSPtr dst{ DoParseScene( Path{ cuT( "TestScene" ) } / cuT( "TestScene.cscn" ) ) };
		CT_EQUAL( *src, *dst );
		auto dstWindow = GetWindow( m_engine, dst->GetName(), false );
		CT_CHECK( dstWindow != nullptr );
		File::DirectoryDelete( Path{ cuT( "TestScene" ) } );
		src->Cleanup();
		srcWindow->Cleanup();
		m_engine.GetRenderLoop().RenderSyncFrame();
		m_engine.GetSceneCache().Remove( src->GetName() );
		m_engine.GetRenderWindowCache().Remove( srcWindow->GetName() );
		srcWindow.reset();
		src.reset();
		dst->Cleanup();
		dstWindow->Cleanup();
		m_engine.GetRenderLoop().RenderSyncFrame();
		m_engine.GetSceneCache().Remove( dst->GetName() );
		m_engine.GetRenderWindowCache().Remove( dstWindow->GetName() );
		dstWindow.reset();
		dst.reset();
		DeCleanupEngine();
	}
}
