#include "Castor3D/Text/TextScene.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderLoop.hpp"

using namespace castor3d;

namespace castor
{
	//*************************************************************************************************

	namespace
	{
		template< typename ObjType, typename ViewType >
		bool writeView( ViewType const & view
			, String const & elemsName
			, String const & tabs
			, TextFile & file )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				result = file.writeText( cuT( "\n" ) + tabs + cuT( "\t// " ) + elemsName + cuT( "\n" ) ) > 0;
				log::info << cuT( "Scene::write - " ) << elemsName << std::endl;

				for ( auto const & name : view )
				{
					if ( result )
					{
						auto elem = view.find( name );
						result = typename ObjType::TextWriter{ tabs + cuT( "\t" ) }( *elem, file );
					}
				}
			}

			return result;
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
		bool result = ( writeComment( cuT( "Global configuration\n" ), file )
			&& writeOpt( "debug_overlays", scene.getEngine()->getRenderLoop().hasDebugOverlays(), file )
			&& write( "lpv_grid_size", scene.getEngine()->getLpvGridSize(), file )
			&& write( "materials", castor3d::getName( scene.getMaterialsType() ), file );

		if ( result && beginBlock( "scene", scene.getName(), file ) )
		{
			result = writeComment( cuT( "Scene configuration" ), file );

			if ( result )
			{
				log::info << cuT( "Scene::write - Ambient light" ) << std::endl;
				result = file.print( 256, cuT( "%s\tambient_light " ), m_tabs.c_str() ) > 0
					&& RgbColour::TextWriter( String() )( scene.getAmbientLight(), file )
					&& file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< Scene >::checkError( result, "Scene ambient light" );
			}

			if ( result )
			{
				log::info << cuT( "Scene::write - Background colour" ) << std::endl;
				result = file.print( 256, cuT( "%s\tbackground_colour " ), m_tabs.c_str() ) > 0
					&& RgbColour::TextWriter( String() )( scene.getBackgroundColour(), file )
					&& file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< Scene >::checkError( result, "Scene background colour" );
			}

			if ( result )
			{
				log::info << cuT( "Scene::write - LPV indirect attenuation" ) << std::endl;
				result = file.writeText( m_tabs + cuT( "\tlpv_indirect_attenuation " ) + string::toString( scene.getLpvIndirectAttenuation() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Scene >::checkError( result, "Scene LPV indirect attenuation" );
			}

			if ( result && !m_options.materialsFile.empty() )
			{
				log::info << cuT( "Scene::write - Materials file" ) << std::endl;
				String path = m_options.materialsFile;
				string::replace( path, cuT( "\\" ), cuT( "/" ) );
				result = file.writeText( m_tabs + cuT( "\tinclude \"" ) + path + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< Scene >::checkError( result, "Scene Materials file" );
			}

			if ( result && !m_options.meshesFile.empty() )
			{
				log::info << cuT( "Scene::write - Meshes file" ) << std::endl;
				String path = m_options.meshesFile;
				string::replace( path, cuT( "\\" ), cuT( "/" ) );
				result = file.writeText( m_tabs + cuT( "\tinclude \"" ) + path + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< Scene >::checkError( result, "Scene Meshes file" );
			}

			if ( result && !m_options.nodesFile.empty() )
			{
				log::info << cuT( "Scene::write - Nodes file" ) << std::endl;
				String path = m_options.nodesFile;
				string::replace( path, cuT( "\\" ), cuT( "/" ) );
				result = file.writeText( m_tabs + cuT( "\tinclude \"" ) + path + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< Scene >::checkError( result, "Scene Nodes file" );
			}

			if ( result && !m_options.objectsFile.empty() )
			{
				log::info << cuT( "Scene::write - Objects file" ) << std::endl;
				String path = m_options.objectsFile;
				string::replace( path, cuT( "\\" ), cuT( "/" ) );
				result = file.writeText( m_tabs + cuT( "\tinclude \"" ) + path + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< Scene >::checkError( result, "Scene Objects file" );
			}

			if ( result && !m_options.lightsFile.empty() )
			{
				log::info << cuT( "Scene::write - Lights file" ) << std::endl;
				String path = m_options.lightsFile;
				string::replace( path, cuT( "\\" ), cuT( "/" ) );
				result = file.writeText( m_tabs + cuT( "\tinclude \"" ) + path + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< Scene >::checkError( result, "Scene Lights file" );
			}

			if ( result )
			{
				log::info << cuT( "Scene::write - Background" ) << std::endl;
				BackgroundTextWriter writer{ file, m_tabs + cuT( "\t" ) };
				scene.getBackground()->accept( writer );
			}

			if ( result && scene.getFog().getType() != FogType::eDisabled )
			{
				log::info << cuT( "Scene::write - Fog type" ) << std::endl;
				result = file.writeText( m_tabs + cuT( "\tfog_type " ) + castor3d::getName( scene.getFog().getType() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Scene >::checkError( result, "Scene fog type" );

				if ( result )
				{
					log::info << cuT( "Scene::write - Fog density" ) << std::endl;
					result = file.writeText( m_tabs + cuT( "\tfog_density " ) + string::toString( scene.getFog().getDensity() ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< Scene >::checkError( result, "Scene fog density" );
				}
			}

			if ( result && scene.getVoxelConeTracingConfig().enabled )
			{
				log::info << cuT( "Scene::write - Voxel Cone Tracing" ) << std::endl;
				result = VoxelSceneData::TextWriter( m_tabs + cuT( "\t" ) )( scene.getVoxelConeTracingConfig(), file );
			}

			if ( result )
			{
				result = writeView< castor::Font >( scene.getFontView()
					, cuT( "Fonts" )
					, m_tabs
					, file );
			}

			if ( result
				&& m_options.materialsFile.empty() )
			{
				result = writeView< Sampler >( scene.getSamplerView()
					, cuT( "Samplers" )
					, m_tabs
					, file );

				if ( result )
				{
					result = writeView< Material >( scene.getMaterialView()
						, cuT( "Materials" )
						, m_tabs
						, file );
				}
			}

			if ( result
				&& !scene.getOverlayView().isEmpty() )
			{
				result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Overlays\n" ) ) > 0;

				if ( result )
				{
					log::info << cuT( "Scene::write - Overlays" ) << std::endl;

					for ( auto const & name : scene.getOverlayView() )
					{
						auto overlay = scene.getOverlayView().find( name );

						if ( !overlay->getParent() )
						{
							result = result && Overlay::TextWriter( m_tabs + cuT( "\t" ) )( *overlay, file );
						}
					}
				}
			}

			if ( result
				&& m_options.meshesFile.empty()
				&& !scene.getMeshCache().isEmpty() )
			{
				result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Meshes\n" ) ) > 0;

				if ( result )
				{
					log::info << cuT( "Scene::write - Meshes" ) << std::endl;
					using LockType = std::unique_lock< MeshCache const >;
					LockType lock{ castor::makeUniqueLock( scene.getMeshCache() ) };

					for ( auto const & it : scene.getMeshCache() )
					{
						if ( result && it.second->isSerialisable() )
						{
							result = Mesh::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
						}
					}
				}
			}

			if ( result
				&& !scene.getCameraRootNode()->getChildren().empty() )
			{
				result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Cameras nodes\n" ) ) > 0;

				if ( result )
				{
					log::info << cuT( "Scene::write - Cameras nodes" ) << std::endl;

					for ( auto const & it : scene.getCameraRootNode()->getChildren() )
					{
						if ( result
							&& it.first.find( cuT( "_REye" ) ) == String::npos
							&& it.first.find( cuT( "_LEye" ) ) == String::npos )
						{
							result = SceneNode::TextWriter( m_tabs + cuT( "\t" ) )( *it.second.lock(), file );
						}
					}
				}
			}

			if ( result
				&& !scene.getCameraCache().isEmpty() )
			{
				result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Cameras\n" ) ) > 0;

				if ( result )
				{
					log::info << cuT( "Scene::write - Cameras" ) << std::endl;
					using LockType = std::unique_lock< CameraCache const >;
					LockType lock{ castor::makeUniqueLock( scene.getCameraCache() ) };

					for ( auto const & it : scene.getCameraCache() )
					{
						if ( result
							&& it.first.find( cuT( "_REye" ) ) == String::npos
							&& it.first.find( cuT( "_LEye" ) ) == String::npos )
						{
							result = Camera::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
						}
					}
				}
			}

			if ( result
				&& m_options.nodesFile.empty()
				&& !scene.getObjectRootNode()->getChildren().empty() )
			{
				result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Objects nodes\n" ) ) > 0;

				if ( result )
				{
					log::info << cuT( "Scene::write - Objects nodes" ) << std::endl;

					for ( auto const & it : scene.getObjectRootNode()->getChildren() )
					{
						result = result && SceneNode::TextWriter( m_tabs + cuT( "\t" ) )( *it.second.lock(), file );
					}
				}
			}

			if ( result
				&& m_options.lightsFile.empty()
				&& !scene.getLightCache().isEmpty() )
			{
				result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Lights\n" ) ) > 0;

				if ( result )
				{
					log::info << cuT( "Scene::write - Lights" ) << std::endl;
					using LockType = std::unique_lock< LightCache const >;
					LockType lock{ castor::makeUniqueLock( scene.getLightCache() ) };

					for ( auto const & it : scene.getLightCache() )
					{
						result = result && Light::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
					}
				}
			}

			if ( result
				&& m_options.objectsFile.empty()
				&& !scene.getGeometryCache().isEmpty() )
			{
				result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Geometries\n" ) ) > 0;

				if ( result )
				{
					log::info << cuT( "Scene::write - Geometries" ) << std::endl;
					using LockType = std::unique_lock< GeometryCache const >;
					LockType lock{ castor::makeUniqueLock( scene.getGeometryCache() ) };

					for ( auto const & it : scene.getGeometryCache() )
					{
						if ( result
							&& it.second->getMesh()
							&& it.second->getMesh()->isSerialisable() )
						{
							result = Geometry::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
						}
					}
				}
			}

			if ( result
				&& !scene.getParticleSystemCache().isEmpty() )
			{
				result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Particle systems\n" ) ) > 0;

				if ( result )
				{
					log::info << cuT( "Scene::write - Particle systems" ) << std::endl;
					using LockType = std::unique_lock< ObjectCache< ParticleSystem, castor::String > const >;
					LockType lock{ castor::makeUniqueLock( scene.getParticleSystemCache() ) };

					for ( auto const & it : scene.getParticleSystemCache() )
					{
						result = result && ParticleSystem::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
					}
				}
			}

			if ( result
				&& scene.getAnimatedObjectGroupCache().getObjectCount() > 1 )
			{
				result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\t// Animated object groups\n" ) ) > 0;

				if ( result )
				{
					log::info << cuT( "Scene::write - Animated object groups" ) << std::endl;
					using LockType = std::unique_lock< AnimatedObjectGroupCache const >;
					LockType lock{ castor::makeUniqueLock( scene.getAnimatedObjectGroupCache() ) };

					for ( auto const & it : scene.getAnimatedObjectGroupCache() )
					{
						if ( it.first != cuT( "C3D_Textures" ) )
						{
							result = result && AnimatedObjectGroup::TextWriter( m_tabs + cuT( "\t" ) )( *it.second, file );
						}
					}
				}
			}

			file.writeText( cuT( "}\n" ) );

			if ( result )
			{
				result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "// Windows\n" ) ) > 0;
			}

			if ( result )
			{
				log::info << cuT( "Scene::write - Windows" ) << std::endl;
				using LockType = std::unique_lock< RenderWindowCache >;
				LockType lock{ castor::makeUniqueLock( scene.getEngine()->getRenderWindowCache() ) };

				for ( auto const & it : scene.getEngine()->getRenderWindowCache() )
				{
					if ( it.second->getRenderTarget()->getScene()->getName() == scene.getName() )
					{
						result = result && RenderWindow::TextWriter( m_tabs )( *it.second, file );
					}
				}
			}
		}

		return result;
	}

	//*************************************************************************************************
}
