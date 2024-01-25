#include "SceneExportTest.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Animation/AnimationKeyFrame.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp>
#include <Castor3D/Plugin/ImporterPlugin.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <SceneExporter/CscnExporter.hpp>

#include <CastorUtils/Data/BinaryFile.hpp>

namespace Testing
{
	namespace
	{
		bool exportScene( castor3d::Scene const & scene, castor::Path const & fileName )
		{
			castor3d::exporter::CscnSceneExporter exporter{ castor3d::exporter::ExportOptions{} };
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

		void cleanup( castor3d::SceneRPtr scene )
		{
			auto & engine = *scene->getEngine();
			engine.getRenderLoop().renderSyncFrame();
			scene->cleanup();
			engine.getRenderLoop().renderSyncFrame();
			engine.removeScene( scene->getName() );
		}
	}

	SceneExportTest::SceneExportTest( castor3d::Engine & engine )
		: C3DTestCase{ "SceneExportTest", engine }
	{
	}

	void SceneExportTest::doRegisterTests()
	{
		doRegisterTest( "SceneExportTest::SimpleScene", std::bind( &SceneExportTest::SimpleScene, this ) );
		doRegisterTest( "SceneExportTest::InstancedScene", std::bind( &SceneExportTest::InstancedScene, this ) );
		doRegisterTest( "SceneExportTest::AlphaScene", std::bind( &SceneExportTest::AlphaScene, this ) );
		doRegisterTest( "SceneExportTest::AnimatedScene", std::bind( &SceneExportTest::AnimatedScene, this ) );
		doRegisterTest( "SceneExportTest::LoadSceneThenAnother", std::bind( &SceneExportTest::LoadSceneThenAnother, this ) );
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

	castor3d::SceneRPtr SceneExportTest::doParseScene( castor::Path const & path
		, bool initialise )
	{
		castor3d::SceneFileParser dstParser{ m_engine };
		CT_REQUIRE( dstParser.parseFile( path ) );
		CT_REQUIRE( dstParser.scenesBegin() != dstParser.scenesEnd() );
		auto result = dstParser.scenesBegin()->second;

		if ( initialise )
		{
			result->initialise();
		}

		return result;
	}

	void SceneExportTest::doTestScene( castor::String const & name )
	{
		castor3d::SceneRPtr src{ doParseScene( m_testDataFolder / name ) };
		castor::Path path = castor::Path{ cuT( "TestScene" ) } / cuT( "TestScene.cscn" );
		CT_CHECK( exportScene( *src, path ) );
		m_engine.getSceneCache().rename( src->getName()
			, src->getName() + cuT( "_ren" ) );
		castor3d::SceneRPtr dst{ doParseScene( path ) };
		CT_EQUAL( *src, *dst );
		castor::File::directoryDelete( castor::Path{ cuT( "TestScene" ) } );
		cleanup( dst );
		cleanup( src );
	}
}
