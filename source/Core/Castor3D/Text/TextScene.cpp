#include "Castor3D/Text/TextScene.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/CacheView.hpp"
#include "Castor3D/Cache/CameraCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/MeshCache.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/ParticleSystemCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Cache/SceneNodeCache.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Text/TextAnimatedObjectGroup.hpp"
#include "Castor3D/Text/TextBackground.hpp"
#include "Castor3D/Text/TextCamera.hpp"
#include "Castor3D/Text/TextFog.hpp"
#include "Castor3D/Text/TextGeometry.hpp"
#include "Castor3D/Text/TextLight.hpp"
#include "Castor3D/Text/TextMaterial.hpp"
#include "Castor3D/Text/TextMesh.hpp"
#include "Castor3D/Text/TextParticleSystem.hpp"
#include "Castor3D/Text/TextOverlay.hpp"
#include "Castor3D/Text/TextRenderTarget.hpp"
#include "Castor3D/Text/TextRenderWindow.hpp"
#include "Castor3D/Text/TextSampler.hpp"
#include "Castor3D/Text/TextSceneNode.hpp"
#include "Castor3D/Text/TextVoxelSceneData.hpp"

#include <CastorUtils/Data/Text/TextFont.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

using namespace castor3d;

namespace castor
{
	//*************************************************************************************************

	namespace
	{
		template< typename ObjectT >
		bool writable( ObjectT const & object )
		{
			return true;
		}

		template<>
		bool writable< SceneNode >( SceneNode const & object )
		{
			return object.getName().find( cuT( "_REye" ) ) == String::npos
				&& object.getName().find( cuT( "_LEye" ) ) == String::npos;
		}

		template<>
		bool writable< AnimatedObjectGroup >( AnimatedObjectGroup const & object )
		{
			return object.getName() != cuT( "C3D_Textures" );
		}

		template<>
		bool writable< Overlay >( Overlay const & object )
		{
			return !object.getParent();
		}

		template<>
		bool writable< Mesh >( Mesh const & object )
		{
			return object.isSerialisable();
		}

		template<>
		bool writable< Camera >( Camera const & object )
		{
			return object.getName().find( cuT( "_REye" ) ) == String::npos
				&& object.getName().find( cuT( "_LEye" ) ) == String::npos;
		}

		template<>
		bool writable< Geometry >( Geometry const & object )
		{
			return object.getMesh()
				&& object.getMesh()->isSerialisable();
		}

		template< typename CacheTypeT, typename FilterT >
		bool writeCache( TextFile & file
			, CacheTypeT const & cache
			, String const & elemsName
			, TextWriterBase const & writer
			, FilterT filter )
		{
			bool result = true;
			bool empty = cache.isEmpty();

			if constexpr ( std::is_same_v< CacheTypeT, AnimatedObjectGroupCache > )
			{
				empty = cache.getObjectCount() <= 1u;
			}

			if ( !empty )
			{
				result = file.writeText( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) ) > 0;
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;
				auto lock( castor::makeUniqueLock( cache ) );

				for ( auto const & it : cache )
				{
					if ( result && filter( *it.second ) )
					{
						result = writer.write( file, *it.second );
					}
				}
			}

			return result;
		}

		template< typename CacheTypeT, typename FilterT >
		bool writeIncludedCache( TextFile & file
			, CacheTypeT const & cache
			, String const & elemsName
			, Path const & includePath
			, TextWriterBase const & writer
			, FilterT filter )
		{
			if ( !includePath.empty() )
			{
				return true;
			}

			return writeCache( file, cache, elemsName, writer, filter );
		}
		
		template< typename ViewTypeT, typename FilterT >
		bool writeView( TextFile & file
			, ViewTypeT const & view
			, String const & elemsName
			, TextWriterBase const & writer
			, FilterT filter )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				result = file.writeText( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) ) > 0;
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;

				for ( auto const & name : view )
				{
					if ( result )
					{
						auto elem = view.find( name );

						if ( filter( *elem ) )
						{
							result = writer.write( file, *elem );
						}
					}
				}
			}

			return result;
		}
		
		template< typename ViewTypeT >
		bool writeView( TextFile & file
			, ViewTypeT const & view
			, String const & elemsName
			, TextWriterBase const & writer )
		{
			return writeView( file, view, elemsName, writer
				, []( auto const & lookup )
				{
					return true;
				} );
		}

		template< typename ViewTypeT >
		bool writeIncludedView( TextFile & file
			, ViewTypeT const & view
			, String const & elemsName
			, Path const & includePath
			, TextWriterBase const & writer )
		{
			if ( !includePath.empty() )
			{
				return true;
			}

			return writeView( file, view, elemsName, writer );
		}

		bool writeInclude( TextFile & file
			, Path const & path
			, TextWriterBase const & writer )
		{
			if ( !path.empty() )
			{
				return writer.writePath( file, cuT( "include" ), path );
			}

			return true;
		}

		bool writeNodes( TextFile & file
			, SceneNode::SceneNodePtrStrMap const & nodes
			, String const & elemsName
			, TextWriterBase const & writer )
		{
			bool result = true;

			if ( !nodes.empty() )
			{
				result = file.writeText( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) ) > 0;
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;

				for ( auto const & it : nodes )
				{
					auto node = it.second.lock();

					if ( result && writable( *node ) )
					{
						result = writer.write( file, *node );
					}
				}
			}

			return result;
		}

		bool writeIncludedNodes( TextFile & file
			, SceneNode::SceneNodePtrStrMap const & nodes
			, String const & elemsName
			, Path const & includePath
			, TextWriterBase const & writer )
		{
			if ( !includePath.empty() )
			{
				return true;
			}

			return writeNodes( file, nodes, elemsName, writer );
		}
	}

	//*************************************************************************************************

	TextWriter< Scene >::TextWriter( String const & tabs
		, Options options )
		: TextWriterT< Scene >{ tabs }
		, m_options{ options }
	{
	}

	bool TextWriter< Scene >::TextWriter::operator()( Scene const & scene
		, TextFile & file )
	{
		log::info << cuT( "Scene::write - Scene Name" ) << std::endl;
		bool result = writeComment( file, cuT( "Global configuration" ) )
			&& writeOpt( file, "debug_overlays", scene.getEngine()->getRenderLoop().hasDebugOverlays() )
			&& writeOpt( file, "lpv_grid_size", scene.getEngine()->getLpvGridSize(), 32u )
			&& write( file, "materials", castor3d::getName( scene.getMaterialsType() ) );

		if ( result )
		{
			result = false;

			if ( auto block = beginBlock( file, "scene", scene.getName() ) )
			{
				result = writeComment( file, cuT( "Scene configuration" ) );

				if ( result )
				{
					log::info << cuT( "Scene::write - Ambient light" ) << std::endl;
					result = write( file, cuT( "ambient_light" ), scene.getAmbientLight() )
						&& write( file, cuT( "background_colour" ), scene.getBackgroundColour() )
						&& write( file, cuT( "lpv_indirect_attenuation" ), scene.getLpvIndirectAttenuation() )
						&& writeInclude( file, m_options.materialsFile, *this )
						&& writeInclude( file, m_options.meshesFile, *this )
						&& writeInclude( file, m_options.nodesFile, *this )
						&& writeInclude( file, m_options.objectsFile, *this )
						&& writeInclude( file, m_options.lightsFile, *this )
						&& write( file, *scene.getBackground() )
						&& write( file, scene.getFog() )
						&& write( file, scene.getVoxelConeTracingConfig() )
						&& writeView( file, scene.getFontView(), cuT( "Fonts" ), *this )
						&& writeIncludedView( file, scene.getSamplerView(), cuT( "Samplers" ), m_options.materialsFile, *this )
						&& writeIncludedView( file, scene.getMaterialView(), cuT( "Materials" ), m_options.materialsFile, *this )
						&& writeView( file, scene.getOverlayView(), cuT( "Overlays" ), *this, writable< Overlay > )
						&& writeIncludedCache( file, scene.getMeshCache(), cuT( "Meshes" ), m_options.meshesFile, *this, writable< Mesh > )
						&& writeNodes( file, scene.getCameraRootNode()->getChildren(), cuT( "Cameras nodes" ), *this )
						&& writeCache( file, scene.getCameraCache(), cuT( "Cameras" ), *this, writable< Camera > )
						&& writeIncludedNodes( file, scene.getCameraRootNode()->getChildren(), cuT( "Objects nodes" ), m_options.nodesFile, *this )
						&& writeIncludedCache( file, scene.getLightCache(), cuT( "Lights" ), m_options.lightsFile, *this, writable< Light > )
						&& writeIncludedCache( file, scene.getGeometryCache(), cuT( "Geometries" ), m_options.objectsFile, *this, writable< Geometry > )
						&& writeCache( file, scene.getParticleSystemCache(), cuT( "Particle systems" ), *this, writable< ParticleSystem > )
						&& writeCache( file, scene.getAnimatedObjectGroupCache(), cuT( "Animated object groups" ), *this, writable< AnimatedObjectGroup > );
				}
			}

			if ( result )
			{
				struct IsWritable
				{
					Scene const & scene;
					IsWritable( Scene const & scene )
						: scene{ scene }
					{
					}

					bool operator()( RenderWindow const & window )
					{
						return window.getRenderTarget()->getScene()->getName() == scene.getName();
					}
				};

				result = writeCache( file, scene.getEngine()->getRenderWindowCache(), cuT( "Windows" ), *this, IsWritable{ scene } );
			}
		}

		return result;
	}

	//*************************************************************************************************
}
