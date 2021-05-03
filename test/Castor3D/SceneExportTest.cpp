#include "SceneExportTest.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Animation/AnimationKeyFrame.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/MeshCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Cache/SceneCache.hpp>
#include <Castor3D/Cache/WindowCache.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Bone.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp>
#include <Castor3D/Plugin/ImporterPlugin.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Text/TextScene.hpp>

#include <CastorUtils/Data/BinaryFile.hpp>

using namespace castor;
using namespace castor3d;

namespace Testing
{
	namespace
	{
		bool ExportScene( Scene const & p_scene, Path const & p_fileName )
		{
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
			auto stream = castor::makeStringStream();
			auto result = castor::TextWriter< Scene >( String() )( p_scene, stream );

			Path subfolder{ cuT( "Meshes" ) };

			if ( result )
			{
				if ( !File::directoryExists( folder / subfolder ) )
				{
					File::directoryCreate( folder / subfolder );
				}

				auto lock = castor::makeUniqueLock( p_scene.getMeshCache() );

				for ( auto const & it : p_scene.getMeshCache() )
				{
					auto mesh = it.second;
					Path path{ folder / subfolder / it.second->getName() + cuT( ".cmsh" ) };
					BinaryFile file{ path, File::OpenMode::eWrite };
					result &= castor3d::BinaryWriter< Mesh >{}.write( *mesh, file );

					auto skeleton = mesh->getSkeleton();

					if ( result && skeleton )
					{
						BinaryFile file{ folder / subfolder / ( it.second->getName() + cuT( ".cskl" ) ), File::OpenMode::eWrite };
						result = castor3d::BinaryWriter< Skeleton >{}.write( *skeleton, file );
					}
				}
			}

			if ( result )
			{
				TextFile scnFile( Path{ filePath + cuT( ".cscn" ) }, File::OpenMode::eWrite, File::EncodingMode::eASCII );
				result = scnFile.writeText( stream.str() ) > 0;
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

		void cleanup( SceneSPtr & scene )
		{
			auto & engine = *scene->getEngine();
			scene->cleanup();
			engine.getRenderLoop().renderSyncFrame();
			engine.getSceneCache().remove( scene->getName() );
			scene.reset();
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
		SceneSPtr src{ doParseScene( m_testDataFolder / p_name ) };
		Path path{ cuT( "TestScene.cscn" ) };
		CT_CHECK( ExportScene( *src, path ) );
		auto window = std::make_shared< RenderWindow >( "SceneExportTest"
			, m_engine
			, Size{ 800u, 600u }
			, ashes::WindowHandle{ std::make_unique< TestWindowHandle >() } );
		
		RenameObject( src, m_engine.getSceneCache() );
		SceneSPtr dst{ doParseScene( Path{ cuT( "TestScene" ) } / cuT( "TestScene.cscn" ) ) };
		CT_EQUAL( *src, *dst );
		File::directoryDelete( Path{ cuT( "TestScene" ) } );
		cleanup( dst );
		cleanup( src );
		doCleanupEngine();
	}
}
