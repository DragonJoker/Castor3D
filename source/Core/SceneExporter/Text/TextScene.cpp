#include "TextScene.hpp"

#include "TextAnimatedObjectGroup.hpp"
#include "TextBackground.hpp"
#include "TextCamera.hpp"
#include "TextFog.hpp"
#include "TextGeometry.hpp"
#include "TextLight.hpp"
#include "TextMaterial.hpp"
#include "TextMesh.hpp"
#include "TextOverlay.hpp"
#include "TextParticleSystem.hpp"
#include "TextRenderTarget.hpp"
#include "TextRenderWindow.hpp"
#include "TextSampler.hpp"
#include "TextSceneNode.hpp"
#include "TextSkeleton.hpp"
#include "TextVctConfig.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/AnimatedObjectGroupCache.hpp>
#include <Castor3D/Cache/BillboardCache.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/LightCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Render/RenderLoop.hpp>

#include <CastorUtils/Data/Text/TextFont.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>
#include <CastorUtils/Design/ResourceCache.hpp>

namespace castor
{
	using namespace castor3d;

	//*************************************************************************************************

	namespace txtscn
	{
		struct IsSceneWritable
		{
			Scene const & scene;

			explicit IsSceneWritable( Scene const & scene )
				: scene{ scene }
			{
			}

			bool operator()( RenderWindow const & window )const
			{
				auto target = window.getRenderTarget();

				if ( !target )
				{
					return false;
				}

				auto scn = target->getScene();

				if ( !scn )
				{
					return false;
				}

				return scn->getName() == scene.getName();
			}
		};

		template< typename ObjectT >
		static bool writable( ObjectT const & object )
		{
			return true;
		}

		template<>
		bool writable< castor::Font >( castor::Font const & object )
		{
			return object.isSerialisable();
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
		bool writable< Material >( Material const & object )
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
			auto mesh = object.getMesh();
			return mesh && mesh->isSerialisable();
		}

		template< typename ObjType >
		using FilterFuncT = bool ( * )( ObjType const & obj );

		template< typename ObjType
			, typename CacheTypeT
			, typename ... Params >
		static bool writeCache( StringStream & file
			, CacheTypeT const & cache
			, String const & elemsName
			, TextWriterBase const & writer
			, FilterFuncT< ObjType > filter = []( ObjType const & )
			{
				return true;
			}
			, Params && ... params )
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
				castor::TextWriter< ObjType > subWriter{ writer.tabs()
					, std::forward< Params >( params )... };
				auto lock( castor::makeUniqueLock( cache ) );

				for ( auto const & it : cache )
				{
					if ( result && it.second && filter( *it.second ) )
					{
						result = subWriter( *it.second, file );
					}
				}
			}

			return result;
		}

		template< typename ObjType
			, typename CacheTypeT
			, typename ... Params >
		static bool writeKeyedContainer( StringStream & file
			, CacheTypeT const & cache
			, String const & elemsName
			, TextWriterBase const & writer
			, std::function< bool( ObjType const & ) > filter = []( ObjType const & )
			{
				return true;
			}
			, Params && ... params )
		{
			bool result = true;

			if ( !cache.empty() )
			{
				file << ( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) );
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;
				castor::TextWriter< ObjType > subWriter{ writer.tabs()
					, std::forward< Params >( params )... };

				for ( auto const & it : cache )
				{
					if ( result && it.second && filter( *it.second ) )
					{
						result = subWriter( *it.second, file );
					}
				}
			}

			return result;
		}

		template< typename ObjType
			, typename CacheTypeT
			, typename ... Params >
		static bool writeIncludedCache( StringStream & file
			, CacheTypeT const & cache
			, String const & elemsName
			, Path const & includePath
			, TextWriterBase const & writer
			, FilterFuncT< ObjType > filter = []( ObjType const & )
			{
				return true;
			}
			, Params && ... params )
		{
			if ( !includePath.empty() )
			{
				return true;
			}

			return writeCache( file, cache, elemsName, writer
				, filter
				, std::forward< Params >( params )... );
		}

		template< typename ObjType
			, typename ViewTypeT
			, typename ... Params >
		static bool writeView( StringStream & file
			, ViewTypeT const & view
			, String const & elemsName
			, TextWriterBase const & writer
			, FilterFuncT< ObjType > filter = []( ObjType const & )
			{
				return true;
			}
			, Params && ... params )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				file << ( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) );
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;
				castor::TextWriter< ObjType > subWriter{ writer.tabs()
					, std::forward< Params >( params )... };

				for ( auto const & name : view )
				{
					if ( result )
					{
						if ( auto pelem = view.find( name ) )
						{
							auto & elem = static_cast< typename ViewTypeT::ElementT const & >( *pelem );

							if ( filter( elem ) )
							{
								result = subWriter( elem, file );
							}
						}
					}
				}
			}

			return result;
		}

		template< typename ObjType
			, typename ViewTypeT
			, typename ... Params >
		static bool writeIncludedView( StringStream & file
			, ViewTypeT const & view
			, String const & elemsName
			, Path const & includePath
			, TextWriterBase const & writer
			, FilterFuncT< ObjType > filter = []( ObjType const & )
			{
				return true;
			}
			, Params && ... params )
		{
			if ( !includePath.empty() )
			{
				return true;
			}

			return writeView( file, view, elemsName, writer
				, filter
				, std::forward< Params >( params )... );
		}

		static bool writeInclude( StringStream & file
			, Path const & path
			, TextWriterBase const & writer )
		{
			if ( !path.empty() )
			{
				return writer.writePath( file, cuT( "include" ), path );
			}

			return true;
		}

		static bool writeNodes( StringStream & file
			, SceneNode::SceneNodeMap const & nodes
			, String const & elemsName
			, float scale
			, TextWriterBase const & writer )
		{
			bool result = true;

			if ( !nodes.empty() )
			{
				file << ( cuT( "\n" ) + writer.tabs() + cuT( "//" ) + elemsName + cuT( "\n" ) );
				log::info << writer.tabs() << cuT( "Scene::write - " ) << elemsName << std::endl;
				castor::TextWriter< SceneNode > subWriter{ writer.tabs(), scale };

				for ( auto const & it : nodes )
				{
					auto node = it.second;

					if ( result && node && writable( *node ) )
					{
						result = subWriter( *node, file );
					}
				}
			}

			return result;
		}

		static bool writeIncludedNodes( StringStream & file
			, SceneNode::SceneNodeMap const & nodes
			, String const & elemsName
			, Path const & includePath
			, float scale
			, TextWriterBase const & writer )
		{
			if ( !includePath.empty() )
			{
				return true;
			}

			return writeNodes( file, nodes, elemsName, scale, writer );
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
			&& write( file, "materials", scene.getDefaultLightingModelName() )
			&& txtscn::writeInclude( file, m_options.globalFontsFile, *this )
			&& txtscn::writeInclude( file, m_options.globalSamplersFile, *this )
			&& txtscn::writeInclude( file, m_options.globalMaterialsFile, *this )
			&& txtscn::writeInclude( file, m_options.globalThemesFile, *this )
			&& txtscn::writeInclude( file, m_options.globalStylesFile, *this )
			&& txtscn::writeInclude( file, m_options.globalControlsFile, *this );

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
						&& txtscn::writeIncludedView( file, scene.getFontView(), cuT( "Fonts" ), m_options.sceneFontsFile, *this, txtscn::writable< castor::Font >, m_options.rootFolder )
						&& txtscn::writeInclude( file, m_options.sceneSamplersFile, *this )
						&& txtscn::writeInclude( file, m_options.sceneMaterialsFile, *this )
						&& txtscn::writeInclude( file, m_options.sceneThemesFile, *this )
						&& txtscn::writeInclude( file, m_options.sceneStylesFile, *this )
						&& txtscn::writeInclude( file, m_options.sceneControlsFile, *this )
						&& txtscn::writeInclude( file, m_options.skeletonsFile, *this )
						&& txtscn::writeInclude( file, m_options.meshesFile, *this )
						&& txtscn::writeInclude( file, m_options.nodesFile, *this )
						&& txtscn::writeInclude( file, m_options.objectsFile, *this )
						&& txtscn::writeInclude( file, m_options.lightsFile, *this )
						&& writeSub( file, *scene.getBackground(), m_options.rootFolder )
						&& writeSub( file, scene.getFog() )
						&& writeSub( file, scene.getVoxelConeTracingConfig() )
						&& txtscn::writeIncludedView( file, scene.getSamplerView(), cuT( "Samplers" ), m_options.sceneMaterialsFile, *this, txtscn::writable< Sampler > )
						&& txtscn::writeIncludedView( file, scene.getMaterialView(), cuT( "Materials" ), m_options.sceneMaterialsFile, *this, txtscn::writable< Material >, m_options.rootFolder, m_options.subfolder )
						&& txtscn::writeCache( file, scene.getOverlayCache(), cuT( "Overlays" ), *this, txtscn::writable< Overlay > )
						&& txtscn::writeIncludedCache( file, scene.getSkeletonCache(), cuT( "Skeletons" ), m_options.skeletonsFile, *this, txtscn::writable< Skeleton >, m_options.subfolder )
						&& txtscn::writeIncludedCache( file, scene.getMeshCache(), cuT( "Meshes" ), m_options.meshesFile, *this, txtscn::writable< Mesh >, m_options.subfolder )
						&& txtscn::writeNodes( file, scene.getCameraRootNode()->getChildren(), cuT( "Cameras nodes" ), m_options.scale, *this )
						&& txtscn::writeCache( file, scene.getCameraCache(), cuT( "Cameras" ), *this, txtscn::writable< Camera > )
						&& txtscn::writeIncludedNodes( file, scene.getObjectRootNode()->getChildren(), cuT( "Objects nodes" ), m_options.nodesFile, m_options.scale, *this )
						&& txtscn::writeIncludedCache( file, scene.getLightCache(), cuT( "Lights" ), m_options.lightsFile, *this, txtscn::writable< Light > )
						&& txtscn::writeIncludedCache( file, scene.getGeometryCache(), cuT( "Geometries" ), m_options.objectsFile, *this, txtscn::writable< Geometry > )
						&& txtscn::writeCache( file, scene.getParticleSystemCache(), cuT( "Particle systems" ), *this, txtscn::writable< ParticleSystem > )
						&& txtscn::writeCache( file, scene.getAnimatedObjectGroupCache(), cuT( "Animated object groups" ), *this, txtscn::writable< AnimatedObjectGroup > );
				}
			}

			if ( result )
			{
				txtscn::IsSceneWritable isWritable{ scene };
				result = txtscn::writeKeyedContainer< RenderWindow >( file, scene.getEngine()->getRenderWindows(), cuT( "Windows" ), *this, [&isWritable]( RenderWindow const & wnd ) { return isWritable( wnd ); } );
			}
		}

		return result;
	}

	//*************************************************************************************************
}
