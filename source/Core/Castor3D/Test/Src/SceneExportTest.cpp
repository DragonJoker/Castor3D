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

using namespace castor;
using namespace castor3d;

namespace Testing
{
	namespace
	{
		bool ExportScene( Scene const & p_scene, Path const & p_fileName )
		{
			bool result = true;
			Path folder = p_fileName.getPath();

			if ( folder.empty() )
			{
				folder = Path{ p_fileName.getFileName() };
			}
			else
			{
				folder /= p_fileName.getFileName();
			}

			if ( !File::directoryExists( folder ) )
			{
				File::directoryCreate( folder );
			}

			Path filePath = folder / p_fileName.getFileName();

			if ( result )
			{
				TextFile scnFile( Path{ filePath + cuT( ".cscn" ) }, File::OpenMode::eWrite, File::EncodingMode::eASCII );
				result = Scene::TextWriter( String() )( p_scene, scnFile );
			}

			Path subfolder{ cuT( "Meshes" ) };

			if ( result )
			{
				if ( !File::directoryExists( folder / subfolder ) )
				{
					File::directoryCreate( folder / subfolder );
				}

				auto lock = makeUniqueLock( p_scene.getMeshCache() );

				for ( auto const & it : p_scene.getMeshCache() )
				{
					auto mesh = it.second;
					Path path{ folder / subfolder / it.first + cuT( ".cmsh" ) };
					BinaryFile file{ path, File::OpenMode::eWrite };
					result &= BinaryWriter< Mesh >{}.write( *mesh, file );
				}
			}

			return result;
		}

		template< typename ObjT, typename CacheT >
		void RenameObject( ObjT p_object, CacheT & p_cache )
		{
			auto name = p_object->getName();
			p_object->setName( name + cuT( "_ren" ) );
			p_cache.remove( name );
			p_cache.add( p_object->getName(), p_object );
		}

		RenderWindowSPtr getWindow( Engine & engine
			, String const & p_sceneName
			, bool p_rename )
		{
			auto & windows = engine.getRenderWindowCache();
			windows.lock();
			auto it = std::find_if( windows.begin()
				, windows.end()
				, [p_sceneName]( auto & p_pair )
			{
				return p_pair.second->getScene()->getName() == p_sceneName;
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

	SceneExportTest::SceneExportTest( Engine & engine )
		: C3DTestCase{ "SceneExportTest", engine }
	{
	}

	SceneExportTest::~SceneExportTest()
	{
	}

	void SceneExportTest::doRegisterTests()
	{
		doRegisterTest( "SceneExportTest::SimpleScene", std::bind( &SceneExportTest::SimpleScene, this ) );
		doRegisterTest( "SceneExportTest::InstancedScene", std::bind( &SceneExportTest::InstancedScene, this ) );
		doRegisterTest( "SceneExportTest::AlphaScene", std::bind( &SceneExportTest::AlphaScene, this ) );
		doRegisterTest( "SceneExportTest::AnimatedScene", std::bind( &SceneExportTest::AnimatedScene, this ) );
	}

	void SceneExportTest::SimpleScene()
	{
		doTestScene( cuT( "light_directional.cscn" ) );
	}

	void SceneExportTest::InstancedScene()
	{
		doTestScene( cuT( "instancing.cscn" ) );
	}

	void SceneExportTest::AlphaScene()
	{
		doTestScene( cuT( "Alpha.zip" ) );
	}

	void SceneExportTest::AnimatedScene()
	{
		doTestScene( cuT( "Anim.zip" ) );
	}

	SceneSPtr SceneExportTest::doParseScene( Path const & p_path )
	{
		SceneFileParser dstParser{ m_engine };
		CT_REQUIRE( dstParser.parseFile( p_path ) );
		CT_REQUIRE( dstParser.scenesBegin() != dstParser.scenesEnd() );
		SceneSPtr scene{ dstParser.scenesBegin()->second };
		auto window = getWindow( m_engine, scene->getName(), false );

		if ( window )
		{
			window->initialise( Size{ 800, 600 }, WindowHandle{ std::make_shared< TestWindowHandle >() } );
			m_engine.getRenderLoop().renderSyncFrame();
		}

		return scene;
	}

	void SceneExportTest::doTestScene( String const & p_name )
	{
		SceneSPtr src{ doParseScene( m_testDataFolder / p_name ) };
		Path path{ cuT( "TestScene.cscn" ) };
		CT_CHECK( ExportScene( *src, path ) );
		
		RenameObject( src, m_engine.getSceneCache() );
		auto srcWindow = getWindow( m_engine, src->getName(), true );
		CT_CHECK( srcWindow != nullptr );

		SceneSPtr dst{ doParseScene( Path{ cuT( "TestScene" ) } / cuT( "TestScene.cscn" ) ) };
		CT_EQUAL( *src, *dst );
		auto dstWindow = getWindow( m_engine, dst->getName(), false );
		CT_CHECK( dstWindow != nullptr );
		File::directoryDelete( Path{ cuT( "TestScene" ) } );
		src->cleanup();
		srcWindow->cleanup();
		m_engine.getRenderLoop().renderSyncFrame();
		m_engine.getSceneCache().remove( src->getName() );
		m_engine.getRenderWindowCache().remove( srcWindow->getName() );
		srcWindow.reset();
		src.reset();
		dst->cleanup();
		dstWindow->cleanup();
		m_engine.getRenderLoop().renderSyncFrame();
		m_engine.getSceneCache().remove( dst->getName() );
		m_engine.getRenderWindowCache().remove( dstWindow->getName() );
		dstWindow.reset();
		dst.reset();
		DeCleanupEngine();
	}
}
