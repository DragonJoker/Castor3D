#include "Castor3D/Text/TextScene.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/Cache.hpp"
#include "Castor3D/Cache/CacheView.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
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
		bool writeCache( StringStream & file
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
				file << ( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) );
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;
				auto lock( castor::makeUniqueLock( cache ) );

				for ( auto const & it : cache )
				{
					if ( result && filter( *it.second ) )
					{
						result = writer.writeSub( file, *it.second );
					}
				}
			}

			return result;
		}

		template< typename CacheTypeT, typename FilterT >
		bool writeKeyedContainer( StringStream & file
			, CacheTypeT const & cache
			, String const & elemsName
			, TextWriterBase const & writer
			, FilterT filter )
		{
			bool result = true;

			if ( !cache.empty() )
			{
				file << ( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) );
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;

				for ( auto const & it : cache )
				{
					if ( result && filter( *it.second ) )
					{
						result = writer.writeSub( file, *it.second );
					}
				}
			}

			return result;
		}

		template< typename CacheTypeT, typename FilterT >
		bool writeIncludedCache( StringStream & file
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

		template< typename CacheTypeT, typename FilterT >
		bool writeCache( StringStream & file
			, CacheTypeT const & cache
			, String const & elemsName
			, String const & subfolder
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
				file << ( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) );
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;
				auto lock( castor::makeUniqueLock( cache ) );

				for ( auto const & it : cache )
				{
					if ( result && filter( *it.second ) )
					{
						result = writer.writeSub( file, *it.second, subfolder );
					}
				}
			}

			return result;
		}

		template< typename CacheTypeT, typename FilterT >
		bool writeIncludedCache( StringStream & file
			, CacheTypeT const & cache
			, String const & elemsName
			, Path const & includePath
			, String const & subfolder
			, TextWriterBase const & writer
			, FilterT filter )
		{
			if ( !includePath.empty() )
			{
				return true;
			}

			return writeCache( file, cache, elemsName, subfolder, writer, filter );
		}

		template< typename ViewTypeT, typename FilterT >
		bool writeView( StringStream & file
			, ViewTypeT const & view
			, String const & elemsName
			, TextWriterBase const & writer
			, FilterT filter )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				file << ( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) );
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;

				for ( auto const & name : view )
				{
					if ( result )
					{
						auto elem = view.find( name );

						if ( filter( *elem ) )
						{
							result = writer.writeSub( file, *elem );
						}
					}
				}
			}

			return result;
		}

		template< typename ViewTypeT >
		bool writeView( StringStream & file
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

		template< typename ViewTypeT, typename FilterT >
		bool writeView( StringStream & file
			, ViewTypeT const & view
			, String const & elemsName
			, Path const & folder
			, TextWriterBase const & writer
			, FilterT filter )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				file << ( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) );
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;

				for ( auto const & name : view )
				{
					if ( result )
					{
						auto elem = view.find( name );

						if ( filter( *elem ) )
						{
							result = writer.writeSub( file, *elem, folder );
						}
					}
				}
			}

			return result;
		}

		template< typename ViewTypeT >
		bool writeView( StringStream & file
			, ViewTypeT const & view
			, String const & elemsName
			, Path const & folder
			, TextWriterBase const & writer )
		{
			return writeView( file, view, elemsName, folder, writer
				, []( auto const & lookup )
				{
					return true;
				} );
		}

		template< typename ViewTypeT >
		bool writeIncludedView( StringStream & file
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

		template< typename ViewTypeT, typename FilterT >
		bool writeView( StringStream & file
			, ViewTypeT const & view
			, String const & elemsName
			, Path const & folder
			, String const & subfolder
			, TextWriterBase const & writer
			, FilterT filter )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				file << ( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) );
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;

				for ( auto const & name : view )
				{
					if ( result )
					{
						auto elem = view.find( name );

						if ( filter( *elem ) )
						{
							result = writer.writeSub( file, *elem, folder, subfolder );
						}
					}
				}
			}

			return result;
		}

		template< typename ViewTypeT >
		bool writeView( StringStream & file
			, ViewTypeT const & view
			, String const & elemsName
			, Path const & folder
			, String const & subfolder
			, TextWriterBase const & writer )
		{
			return writeView( file, view, elemsName, folder, subfolder, writer
				, []( auto const & lookup )
				{
					return true;
				} );
		}

		template< typename ViewTypeT >
		bool writeIncludedView( StringStream & file
			, ViewTypeT const & view
			, String const & elemsName
			, Path const & includePath
			, Path const & folder
			, String const & subfolder
			, TextWriterBase const & writer )
		{
			if ( !includePath.empty() )
			{
				return true;
			}

			return writeView( file, view, elemsName, folder, subfolder, writer );
		}

		bool writeInclude( StringStream & file
			, Path const & path
			, TextWriterBase const & writer )
		{
			if ( !path.empty() )
			{
				return writer.writePath( file, cuT( "include" ), path );
			}

			return true;
		}

		bool writeNodes( StringStream & file
			, SceneNode::SceneNodeMap const & nodes
			, String const & elemsName
			, TextWriterBase const & writer )
		{
			bool result = true;

			if ( !nodes.empty() )
			{
				file << ( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) );
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;

				for ( auto const & it : nodes )
				{
					auto node = it.second.lock();

					if ( result && writable( *node ) )
					{
						result = writer.writeSub( file, *node );
					}
				}
			}

			return result;
		}

		bool writeIncludedNodes( StringStream & file
			, SceneNode::SceneNodeMap const & nodes
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
		, StringStream & file )
	{
		log::info << cuT( "Scene::write - Scene Name" ) << std::endl;
		bool result = writeComment( file, cuT( "Global configuration" ) )
			&& writeOpt( file, "debug_overlays", scene.getEngine()->getRenderLoop().hasDebugOverlays() )
			&& writeOpt( file, "lpv_grid_size", scene.getEngine()->getLpvGridSize(), 32u )
			&& write( file, "materials", scene.getPassesName() );

		if ( result )
		{
			result = false;

			if ( auto block{ beginBlock( file, "scene", scene.getName() ) } )
			{
				result = writeComment( file, cuT( "Scene configuration" ) );

				if ( result )
				{
					log::info << cuT( "Scene::write - Ambient light" ) << std::endl;
					result = writeNamedSub( file, cuT( "ambient_light" ), scene.getAmbientLight() )
						&& writeNamedSub( file, cuT( "background_colour" ), scene.getBackgroundColour() )
						&& write( file, cuT( "lpv_indirect_attenuation" ), scene.getLpvIndirectAttenuation() )
						&& writeInclude( file, m_options.materialsFile, *this )
						&& writeInclude( file, m_options.meshesFile, *this )
						&& writeInclude( file, m_options.nodesFile, *this )
						&& writeInclude( file, m_options.objectsFile, *this )
						&& writeInclude( file, m_options.lightsFile, *this )
						&& writeSub( file, *scene.getBackground(), m_options.rootFolder )
						&& writeSub( file, scene.getFog() )
						&& writeSub( file, scene.getVoxelConeTracingConfig() )
						&& writeView( file, scene.getFontView(), cuT( "Fonts" ), m_options.rootFolder, *this )
						&& writeIncludedView( file, scene.getSamplerView(), cuT( "Samplers" ), m_options.materialsFile, *this )
						&& writeIncludedView( file, scene.getMaterialView(), cuT( "Materials" ), m_options.rootFolder, m_options.materialsFile, m_options.subfolder, *this )
						&& writeView( file, scene.getOverlayView(), cuT( "Overlays" ), *this, writable< Overlay > )
						&& writeIncludedCache( file, scene.getMeshCache(), cuT( "Meshes" ), m_options.meshesFile, m_options.subfolder, *this, writable< Mesh > )
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
					explicit IsWritable( Scene const & scene )
						: scene{ scene }
					{
					}

					bool operator()( RenderWindow const & window )
					{
						return window.getRenderTarget()->getScene()->getName() == scene.getName();
					}
				};

				result = writeKeyedContainer( file, scene.getEngine()->getRenderWindows(), cuT( "Windows" ), *this, IsWritable{ scene } );
			}
		}

		return result;
	}

	//*************************************************************************************************
}
