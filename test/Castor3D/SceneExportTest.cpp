#include "SceneExportTest.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Animation/AnimationKeyFrame.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Buffer/ObjectBufferPool.hpp>
#include <Castor3D/Buffer/GpuBufferPool.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
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
			engine.getRenderTargetCache().cleanup( *engine.getRenderDevice() );
			scene->cleanup();
			engine.getRenderLoop().renderSyncFrame();
			engine.getRenderTargetCache().clear();
			engine.removeScene( scene->getName() );
		}

		void printAllocs( castor::Vector< castor::Pair< castor3d::MemChunk, castor::String > > const & allocs
			, castor::String const & step )
		{
			castor3d::log::debug << "Allocations - " << step << std::endl;

			for ( auto const & [chunk, stack] : allocs )
			{
				castor3d::log::debug << stack << std::endl << std::endl;
			}
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
		doRegisterTest( "SceneExportTest::LoadCleanReload", std::bind( &SceneExportTest::LoadCleanReload, this ) );
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

	void SceneExportTest::doTestCleanReloadScene( castor::String const & name )
	{
		struct EngineCounts
		{
			uint32_t fontCount;
			uint32_t materialCount;
			uint32_t overlayCount;
			uint32_t samplerCount;
			castor3d::AllocationStats bufferAllocated;
			castor3d::AllocationStats vertexAllocated;
			castor3d::AllocationStats indexAllocated;
			castor3d::AllocationStats geometryAllocated;
			castor3d::AllocationStats uboAllocated;
			castor::Vector< castor::Pair< castor3d::MemChunk, castor::String > > uboAllocations;

			EngineCounts( castor3d::Engine const & engine )
			{
				fontCount = engine.getFontsCount();
				materialCount = engine.getMaterialsCount();
				overlayCount = engine.getOverlaysCount();
				samplerCount = engine.getSamplersCount();

				auto const & device = *engine.getRenderDevice();
				bufferAllocated = device.bufferPool->getAllocationStats();
				vertexAllocated = device.vertexPools->getAllocationStats();
				indexAllocated = device.indexPools->getAllocationStats();
				geometryAllocated = device.geometryPools->getAllocationStats();
				uboAllocated = device.uboPool->getAllocationStats();
				uboAllocations = device.uboPool->listAllocations();
			}
		};

		EngineCounts before{ m_engine };
		{
			cleanup( doParseScene( m_testDataFolder / name, true ) );
			EngineCounts after{ m_engine };
			CT_EQUAL( before.fontCount, after.fontCount );
			CT_EQUAL( before.materialCount, after.materialCount );
			CT_EQUAL( before.overlayCount, after.overlayCount );
			CT_EQUAL( before.samplerCount, after.samplerCount );
			CT_EQUAL( before.bufferAllocated, after.bufferAllocated );
			CT_EQUAL( before.vertexAllocated, after.vertexAllocated );
			CT_EQUAL( before.indexAllocated, after.indexAllocated );
			CT_EQUAL( before.geometryAllocated, after.geometryAllocated );
			CT_EQUAL( before.uboAllocated, after.uboAllocated );

			if ( !compare( before.uboAllocated, after.uboAllocated ) )
			{
				printAllocs( before.uboAllocations, "Before" );
				printAllocs( after.uboAllocations, "After1" );
			}
		}
		{
			cleanup( doParseScene( m_testDataFolder / name, true ) );
			EngineCounts after{ m_engine };
			CT_EQUAL( before.fontCount, after.fontCount );
			CT_EQUAL( before.materialCount, after.materialCount );
			CT_EQUAL( before.overlayCount, after.overlayCount );
			CT_EQUAL( before.samplerCount, after.samplerCount );
			CT_EQUAL( before.bufferAllocated, after.bufferAllocated );
			CT_EQUAL( before.vertexAllocated, after.vertexAllocated );
			CT_EQUAL( before.indexAllocated, after.indexAllocated );
			CT_EQUAL( before.geometryAllocated, after.geometryAllocated );
			CT_EQUAL( before.uboAllocated, after.uboAllocated );

			if ( !compare( before.uboAllocated, after.uboAllocated ) )
			{
				printAllocs( before.uboAllocations, "Before" );
				printAllocs( after.uboAllocations, "After2" );
			}
		}
		m_engine.cleanup();
		m_engine.initialise( 1, false );
	}
}
