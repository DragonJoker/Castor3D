#include "SceneExportTest.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Animation/AnimationKeyFrame.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Buffer/ObjectBufferPool.hpp>
#include <Castor3D/Buffer/GpuBufferPool.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <SceneExporter/CscnExporter.hpp>

#include <CastorUtils/Data/BinaryFile.hpp>
#include <CastorUtils/Design/CacheView.hpp>

namespace Testing
{
	namespace
	{
		bool exportScene( c3d::Scene const & scene, c3d::Path const & fileName )
		{
			c3d::exporter::CscnSceneExporter exporter{ c3d::exporter::ExportOptions{} };
			return exporter.exportScene( scene, fileName );
		}

		template< typename ObjT, typename CacheT >
		void renameObject( ObjT object, CacheT & cache )
		{
			auto name = object->getName();
			object->rename( name + cuT( "_ren" ) );
			cache.remove( name );
			cache.add( object->getName(), object );
		}

		void cleanup( c3d::SceneRPtr scene )
		{
			auto & engine = *scene->getEngine();
			engine.getRenderLoop().renderSyncFrame();
			engine.getRenderTargetCache().cleanup( *engine.getRenderDevice() );
			scene->cleanup();
			engine.getRenderLoop().renderSyncFrame();
			engine.getRenderTargetCache().clear();
			engine.removeScene( scene->getName() );
		}
	}

	SceneExportTest::SceneExportTest( c3d::Engine & engine )
		: C3DTestCase{ "SceneExportTest", engine }
	{
	}

	void SceneExportTest::doRegisterTests()
	{
		doRegisterTest( "SceneExportTest::SimpleScene", [this]() { SimpleScene(); } );
		doRegisterTest( "SceneExportTest::InstancedScene", [this]() { InstancedScene(); } );
		doRegisterTest( "SceneExportTest::AlphaScene", [this]() { AlphaScene(); } );
		doRegisterTest( "SceneExportTest::AnimatedScene", [this]() { AnimatedScene(); } );
		doRegisterTest( "SceneExportTest::LoadSceneThenAnother", [this]() { LoadSceneThenAnother(); } );
		doRegisterTest( "SceneExportTest::LoadCleanReload", [this]() { LoadCleanReload(); } );
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

	void SceneExportTest::LoadCleanReload()
	{
		doTestCleanReloadScene( cuT( "light_directional.cscn" ) );
		doTestCleanReloadScene( cuT( "instancing.cscn" ) );
		doTestCleanReloadScene( cuT( "Alpha.zip" ) );
		doTestCleanReloadScene( cuT( "Anim.zip" ) );
	}

	void SceneExportTest::LoadSceneThenAnother()
	{
		cleanup( doParseScene( m_testDataFolder / cuT( "light_directional.cscn" ), true ) );
		m_engine.cleanup();
		m_engine.initialise( 1, false );
		cleanup( doParseScene( m_testDataFolder / cuT( "instancing.cscn" ), true ) );
		m_engine.cleanup();
		m_engine.initialise( 1, false );
		cleanup( doParseScene( m_testDataFolder / cuT( "Alpha.zip" ), true ) );
		m_engine.cleanup();
		m_engine.initialise( 1, false );
		cleanup( doParseScene( m_testDataFolder / cuT( "Anim.zip" ), true ) );
	}

	c3d::SceneRPtr SceneExportTest::doParseScene( c3d::Path const & path
		, bool initialise )
	{
		c3d::SceneFileParser dstParser{ m_engine };
		CT_REQUIRE( dstParser.parseFile( path ) );
		CT_REQUIRE( dstParser.scenesBegin() != dstParser.scenesEnd() );
		auto result = dstParser.scenesBegin()->second;

		if ( initialise )
		{
			result->initialise();
		}

		return result;
	}

	void SceneExportTest::doTestScene( c3d::String const & name )
	{
		c3d::SceneRPtr src{ doParseScene( m_testDataFolder / name ) };
		c3d::Path path = c3d::Path{ cuT( "TestScene" ) } / cuT( "TestScene.cscn" );
		CT_CHECK( exportScene( *src, path ) );
		m_engine.getSceneCache().rename( src->getName()
			, src->getName() + cuT( "_ren" ) );
		c3d::SceneRPtr dst{ doParseScene( path ) };
		CT_EQUAL( *src, *dst );
		c3d::File::directoryDelete( c3d::Path{ cuT( "TestScene" ) } );
		cleanup( dst );
		cleanup( src );
	}

	void SceneExportTest::doTestCleanReloadScene( c3d::String const & name )
	{
		c3d::EngineCounts before{ m_engine };
		{
			cleanup( doParseScene( m_testDataFolder / name, true ) );
			c3d::EngineCounts after{ m_engine };
			CT_EQUAL( before, after );
		}
		{
			cleanup( doParseScene( m_testDataFolder / name, true ) );
			c3d::EngineCounts after{ m_engine };
			CT_EQUAL( before, after );
		}
		m_engine.cleanup();
		m_engine.initialise( 1, false );
	}
}
