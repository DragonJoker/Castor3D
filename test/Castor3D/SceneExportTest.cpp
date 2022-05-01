#include "SceneExportTest.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Animation/AnimationKeyFrame.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
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
#include <Castor3D/Text/TextScene.hpp>

#include <SceneExporter/CscnExporter.hpp>

#include <CastorUtils/Data/BinaryFile.hpp>

using namespace castor;
using namespace castor3d;

namespace Testing
{
	namespace
	{
		bool exportScene( Scene const & p_scene, Path const & p_fileName )
		{
			castor3d::exporter::CscnSceneExporter exporter{ castor3d::exporter::ExportOptions{} };
			return exporter.exportScene( p_scene, p_fileName );
		}

		template< typename ObjT, typename CacheT >
		void renameObject( ObjT p_object, CacheT & p_cache )
		{
			auto name = p_object->getName();
			p_object->rename( name + cuT( "_ren" ) );
			p_cache.remove( name );
			p_cache.add( p_object->getName(), p_object );
		}

		void cleanup( SceneRPtr & scene )
		{
			auto & engine = *scene->getEngine();
			scene->cleanup();
			engine.getRenderLoop().renderSyncFrame();
			engine.getSceneCache().remove( scene->getName() );
		}
	}

	SceneExportTest::SceneExportTest( Engine & engine )
		: C3DTestCase{ "SceneExportTest", engine }
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

	SceneRPtr SceneExportTest::doParseScene( Path const & p_path )
	{
		SceneFileParser dstParser{ m_engine };
		CT_REQUIRE( dstParser.parseFile( p_path ) );
		CT_REQUIRE( dstParser.scenesBegin() != dstParser.scenesEnd() );
		auto scene{ dstParser.scenesBegin()->second };
		auto window = std::make_shared< RenderWindow >( "SceneExportTest"
			, m_engine
			, Size{ 800u, 600u }
			, ashes::WindowHandle{ std::make_unique< TestWindowHandle >() } );

		if ( window )
		{
			m_engine.getRenderLoop().renderSyncFrame();
			m_engine.getRenderLoop().renderSyncFrame();
		}

		return scene;
	}

	void SceneExportTest::doTestScene( String const & p_name )
	{
		SceneRPtr src{ doParseScene( m_testDataFolder / p_name ) };
		Path path{ cuT( "TestScene.cscn" ) };
		CT_CHECK( exportScene( *src, path ) );
		auto window = std::make_shared< RenderWindow >( "SceneExportTest"
			, m_engine
			, Size{ 800u, 600u }
			, ashes::WindowHandle{ std::make_unique< TestWindowHandle >() } );

		m_engine.getSceneCache().rename( src->getName()
			, src->getName() + cuT( "_ren" ) );
		SceneRPtr dst{ doParseScene( Path{ cuT( "TestScene" ) } / cuT( "TestScene.cscn" ) ) };
		CT_EQUAL( *src, *dst );
		File::directoryDelete( Path{ cuT( "TestScene" ) } );
		cleanup( dst );
		cleanup( src );
		doCleanupEngine();
	}
}
