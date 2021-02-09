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
		template< typename ObjType, typename ViewType >
		bool writeView( ViewType const & view
			, String const & elemsName
			, TextWriterBase const & writer
			, TextFile & file )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				result = file.writeText( cuT( "\n" ) + writer.tabs() + cuT( "\t//" ) + elemsName + cuT( "\n" ) ) > 0;
				log::info << cuT( "Scene::write - " ) << elemsName << std::endl;

				for ( auto const & name : view )
				{
					if ( result )
					{
						auto elem = view.find( name );
						result = writer.write( *elem, file );
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
		bool result = writeComment( cuT( "Global configuration" ), file )
			&& writeOpt( "debug_overlays", scene.getEngine()->getRenderLoop().hasDebugOverlays(), file )
			&& write( "lpv_grid_size", scene.getEngine()->getLpvGridSize(), file )
			&& write( "materials", castor3d::getName( scene.getMaterialsType() ), file );

		if ( result )
		{
			result = false;

			if ( auto block = beginBlock( "scene", scene.getName(), file ) )
			{
				result = writeComment( cuT( "Scene configuration" ), file );

				if ( result )
				{
					log::info << cuT( "Scene::write - Ambient light" ) << std::endl;
					result = write( cuT( "ambient_light" ), scene.getAmbientLight(), file )
						&& write( cuT( "background_colour" ), scene.getBackgroundColour(), file )
						&& write( cuT( "lpv_indirect_attenuation" ), scene.getLpvIndirectAttenuation(), file );

					if ( result && !m_options.materialsFile.empty() )
					{
						result = writePath( cuT( "include " ), m_options.materialsFile, file );
					}

					if ( result && !m_options.meshesFile.empty() )
					{
						result = writePath( cuT( "include " ), m_options.meshesFile, file );
					}

					if ( result && !m_options.nodesFile.empty() )
					{
						result = writePath( cuT( "include " ), m_options.nodesFile, file );
					}

					if ( result && !m_options.objectsFile.empty() )
					{
						result = writePath( cuT( "include " ), m_options.objectsFile, file );
					}

					if ( result && !m_options.lightsFile.empty() )
					{
						result = writePath( cuT( "include " ), m_options.lightsFile, file );
					}

					if ( result && scene.getBackground()->getType() != BackgroundType::eColour )
					{
						write( *scene.getBackground(), file );
					}

					if ( result && scene.getFog().getType() != FogType::eDisabled )
					{
						write( scene.getFog(), file );
					}

					if ( result && scene.getVoxelConeTracingConfig().enabled )
					{
						write( scene.getVoxelConeTracingConfig(), file );
					}

					if ( result )
					{
						result = writeView< castor::Font >( scene.getFontView()
							, cuT( "Fonts" )
							, *this
							, file );
					}

					if ( result
						&& m_options.materialsFile.empty() )
					{
						result = writeView< Sampler >( scene.getSamplerView()
							, cuT( "Samplers" )
							, *this
							, file );

						if ( result )
						{
							result = writeView< Material >( scene.getMaterialView()
								, cuT( "Materials" )
								, *this
								, file );
						}
					}

					if ( result
						&& !scene.getOverlayView().isEmpty() )
					{
						result = file.writeText( cuT( "\n" ) + tabs() + cuT( "\t// Overlays\n" ) ) > 0;

						if ( result )
						{
							for ( auto const & name : scene.getOverlayView() )
							{
								auto overlay = scene.getOverlayView().find( name );

								if ( !overlay->getParent() )
								{
									result = result && write( *overlay, file );
								}
							}
						}
					}

					if ( result
						&& m_options.meshesFile.empty()
						&& !scene.getMeshCache().isEmpty() )
					{
						result = file.writeText( cuT( "\n" ) + tabs() + cuT( "\t// Meshes\n" ) ) > 0;

						if ( result )
						{
							using LockType = std::unique_lock< MeshCache const >;
							LockType lock{ castor::makeUniqueLock( scene.getMeshCache() ) };

							for ( auto const & it : scene.getMeshCache() )
							{
								if ( result && it.second->isSerialisable() )
								{
									result = write( *it.second, file );
								}
							}
						}
					}

					if ( result
						&& !scene.getCameraRootNode()->getChildren().empty() )
					{
						result = file.writeText( cuT( "\n" ) + tabs() + cuT( "\t// Cameras nodes\n" ) ) > 0;

						if ( result )
						{
							log::info << cuT( "Scene::write - Cameras nodes" ) << std::endl;

							for ( auto const & it : scene.getCameraRootNode()->getChildren() )
							{
								if ( result
									&& it.first.find( cuT( "_REye" ) ) == String::npos
									&& it.first.find( cuT( "_LEye" ) ) == String::npos )
								{
									result = write( *it.second.lock(), file );
								}
							}
						}
					}

					if ( result
						&& !scene.getCameraCache().isEmpty() )
					{
						result = file.writeText( cuT( "\n" ) + tabs() + cuT( "\t// Cameras\n" ) ) > 0;

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
									result = write( *it.second, file );
								}
							}
						}
					}

					if ( result
						&& m_options.nodesFile.empty()
						&& !scene.getObjectRootNode()->getChildren().empty() )
					{
						result = file.writeText( cuT( "\n" ) + tabs() + cuT( "\t// Objects nodes\n" ) ) > 0;

						if ( result )
						{
							log::info << cuT( "Scene::write - Objects nodes" ) << std::endl;

							for ( auto const & it : scene.getObjectRootNode()->getChildren() )
							{
								result = result && write( *it.second.lock(), file );
							}
						}
					}

					if ( result
						&& m_options.lightsFile.empty()
						&& !scene.getLightCache().isEmpty() )
					{
						result = file.writeText( cuT( "\n" ) + tabs() + cuT( "\t// Lights\n" ) ) > 0;

						if ( result )
						{
							log::info << cuT( "Scene::write - Lights" ) << std::endl;
							using LockType = std::unique_lock< LightCache const >;
							LockType lock{ castor::makeUniqueLock( scene.getLightCache() ) };

							for ( auto const & it : scene.getLightCache() )
							{
								result = result && write( *it.second, file );
							}
						}
					}

					if ( result
						&& m_options.objectsFile.empty()
						&& !scene.getGeometryCache().isEmpty() )
					{
						result = file.writeText( cuT( "\n" ) + tabs() + cuT( "\t// Geometries\n" ) ) > 0;

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
									result = write( *it.second, file );
								}
							}
						}
					}

					if ( result
						&& !scene.getParticleSystemCache().isEmpty() )
					{
						result = file.writeText( cuT( "\n" ) + tabs() + cuT( "\t// Particle systems\n" ) ) > 0;

						if ( result )
						{
							log::info << cuT( "Scene::write - Particle systems" ) << std::endl;
							using LockType = std::unique_lock< ObjectCache< ParticleSystem, castor::String > const >;
							LockType lock{ castor::makeUniqueLock( scene.getParticleSystemCache() ) };

							for ( auto const & it : scene.getParticleSystemCache() )
							{
								result = result && write( *it.second, file );
							}
						}
					}

					if ( result
						&& scene.getAnimatedObjectGroupCache().getObjectCount() > 1 )
					{
						result = file.writeText( cuT( "\n" ) + tabs() + cuT( "\t// Animated object groups\n" ) ) > 0;

						if ( result )
						{
							log::info << cuT( "Scene::write - Animated object groups" ) << std::endl;
							using LockType = std::unique_lock< AnimatedObjectGroupCache const >;
							LockType lock{ castor::makeUniqueLock( scene.getAnimatedObjectGroupCache() ) };

							for ( auto const & it : scene.getAnimatedObjectGroupCache() )
							{
								if ( it.first != cuT( "C3D_Textures" ) )
								{
									result = result && write( *it.second, file );
								}
							}
						}
					}
				}

				if ( result )
				{
					result = file.writeText( cuT( "\n" ) + tabs() + cuT( "// Windows\n" ) ) > 0;
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
							result = result && write( *it.second, file );
						}
					}
				}
			}
		}

		return result;
	}

	//*************************************************************************************************
}
