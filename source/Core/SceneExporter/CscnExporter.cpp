#include "SceneExporter/CscnExporter.hpp"

#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Binary/BinarySkeletonAnimation.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/LightCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/MeshCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
#include <Castor3D/Model/Mesh/ImporterFactory.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BonesComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/IndexMapping.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Text/TextGeometry.hpp>
#include <Castor3D/Text/TextLight.hpp>
#include <Castor3D/Text/TextMaterial.hpp>
#include <Castor3D/Text/TextMesh.hpp>
#include <Castor3D/Text/TextSampler.hpp>
#include <Castor3D/Text/TextScene.hpp>
#include <Castor3D/Text/TextSceneNode.hpp>

namespace castor3d::exporter
{
	namespace
	{
		template< typename ObjType >
		using FilterFuncT = bool ( * )( ObjType const & obj );

		template< typename ObjType, typename ViewType >
		bool writeView( ViewType const & view
			, castor::String const & elemsName
			, castor::StringStream & file
			, FilterFuncT< ObjType > filter = []( ObjType const & )
			{
				return true;
			} )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				file << ( cuT( "// " ) + elemsName + cuT( "\n" ) );
				log::info << cuT( "Scene::write - " ) << elemsName << cuT( "\n" );
				castor::TextWriter< ObjType > writer{ castor::cuEmptyString };

				for ( auto const & name : view )
				{
					auto elem = view.find( name );

					if ( filter( *elem ) )
					{
						result = result && writer( *elem, file );
					}
				}
			}

			return result;
		}

		template< typename ObjType, typename ViewType >
		bool writeView( ViewType const & view
			, castor::String const & elemsName
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file
			, FilterFuncT< ObjType > filter = []( ObjType const & )
			{
				return true;
			} )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				file << ( cuT( "// " ) + elemsName + cuT( "\n" ) );
				log::info << cuT( "Scene::write - " ) << elemsName << cuT( "\n" );
				castor::TextWriter< ObjType > writer{ castor::cuEmptyString, folder, subfolder };

				for ( auto const & name : view )
				{
					auto elem = view.find( name );

					if ( filter( *elem ) )
					{
						result = result && writer( *elem, file );
					}
				}
			}

			return result;
		}

		template< typename ObjType, typename CacheType >
		bool writeCache( CacheType const & cache
			, castor::String const & elemsName
			, castor::StringStream & file
			, FilterFuncT< ObjType > filter = []( ObjType const & )
			{
				return true;
			} )
		{
			bool result = true;

			if ( !cache.isEmpty() )
			{
				file << ( cuT( "// " ) + elemsName + cuT( "\n" ) );
				log::info << ( cuT( "Scene::write - " ) + elemsName );
				castor::TextWriter< ObjType > writer{ castor::cuEmptyString };
				auto lock( castor::makeUniqueLock( cache ) );

				for ( auto const & elemIt : cache )
				{
					if ( filter( *elemIt.second ) )
					{
						result = result && writer( *elemIt.second, file );
					}
				}
			}

			return result;
		}

		template< typename ObjType, typename CacheType >
		bool writeCache( CacheType const & cache
			, castor::String const & elemsName
			, castor::String const & subfolder
			, castor::StringStream & file
			, FilterFuncT< ObjType > filter = []( ObjType const & )
			{
				return true;
			} )
		{
			bool result = true;

			if ( !cache.isEmpty() )
			{
				file << ( cuT( "// " ) + elemsName + cuT( "\n" ) );

				if ( result )
				{
					log::info << ( cuT( "Scene::write - " ) + elemsName );
					castor::TextWriter< ObjType > writer{ castor::cuEmptyString, subfolder };
					auto lock( castor::makeUniqueLock( cache ) );

					for ( auto const & elemIt : cache )
					{
						if ( filter( *elemIt.second ) )
						{
							result = result && writer( *elemIt.second, file );
						}
					}
				}
			}

			return result;
		}
	}

	namespace unsplitted
	{
		bool writeMeshes( castor::Path const & folder
			, castor::Path const & filePath
			, Scene const & scene
			, castor::TextWriter< Scene >::Options & options
			, castor::StringStream & stream )
		{
			bool result = false;
			{
				result = writeCache< Mesh >( scene.getMeshCache()
					, cuT( "Meshes" )
					, options.subfolder
					, stream
					, []( Mesh const & object )
					{
						return object.isSerialisable();
					} );
			}
			return result;
		}

		bool writeObjects( castor::Path const & folder
			, castor::Path const & filePath
			, Scene const & scene
			, castor::TextWriter< Scene >::Options & options
			, castor::StringStream & stream )
		{
			bool result = false;
			{
				result = writeCache< Geometry >( scene.getGeometryCache()
					, cuT( "Objects" )
					, stream
					, []( Geometry const & object )
					{
						return object.getMesh()
							&& object.getMesh()->isSerialisable();
					} );
			}
			return result;
		}
	}

	namespace
	{
		template< bool SplitT, typename ObjectT >
		struct ObjectWriterT;

		template< bool SplitT, typename ObjectT >
		struct ObjectPostWriterT;

		template< typename ObjectT >
		struct ObjectWriterOptionsT
		{
			ObjectWriterOptionsT( ExportOptions const & options
				, ObjectT const & object
				, GeometryCache const & geometries
				, castor::StringStream & meshes
				, castor::StringStream & nodes
				, castor::StringStream & objects
				, castor::Path path
				, castor::String name
				, castor::String subfolder )
				: options{ options }
				, object{ object }
				, geometries{ geometries }
				, meshes{ meshes }
				, nodes{ nodes }
				, objects{ objects }
				, path{ path }
				, name{ name }
				, subfolder{ subfolder }
			{
			}

			template< typename ObjectU >
			ObjectWriterOptionsT( ObjectWriterOptionsT< ObjectU > const & options
				, ObjectT const & object
				, castor::String name )
				: options{ options.options }
				, object{ object }
				, geometries{ options.geometries }
				, meshes{ options.meshes }
				, nodes{ options.nodes }
				, objects{ options.objects }
				, path{ options.path }
				, name{ name }
				, subfolder{ options.subfolder }
			{
			}

			ExportOptions const & options;
			ObjectT const & object;
			GeometryCache const & geometries;
			castor::StringStream & meshes;
			castor::StringStream & nodes;
			castor::StringStream & objects;
			castor::Path path;
			castor::String name;
			castor::String subfolder;
		};

		using MeshWriterOptions = ObjectWriterOptionsT< castor3d::Mesh >;
		using SkeletonWriterOptions = ObjectWriterOptionsT< castor3d::Skeleton >;

		struct SplitInfo
		{
			castor3d::Mesh const * mesh;
			castor3d::Submesh const * submesh;
		};

		template< bool SplitT, typename ObjectT >
		bool writeObjectT( ObjectWriterOptionsT< ObjectT > const & options
			, SplitInfo const & split )
		{
			bool result = false;

			try
			{
				result = ObjectWriterT< SplitT, ObjectT >{}( options, split );
			}
			catch ( castor::Exception & exc )
			{
				std::cerr << "Error encountered while writing file : " << exc.what() << std::endl;
			}
			catch ( std::exception & exc )
			{
				std::cerr << "Error encountered while writing file : " << exc.what() << std::endl;
			}
			catch ( ... )
			{
				std::cerr << "Error encountered while writing file : Unknown exception" << std::endl;
			}

			return result;
		}

		template< bool SplitT, typename ObjectT >
		bool postWriteT( ObjectWriterOptionsT< ObjectT > const & options
			, SplitInfo const & split )
		{
			return ObjectPostWriterT< SplitT, ObjectT >{}( options, split );
		}

		template< bool SplitT >
		struct ObjectPostWriterT< SplitT, castor3d::Mesh >
		{
			bool operator()( MeshWriterOptions const & options
				, SplitInfo const & split )
			{
				bool result = true;

				if constexpr ( SplitT )
				{
					castor::Point3f position;

					if ( options.options.recenter )
					{
						auto & submesh = *options.object.getSubmesh( 0u );
						position = submesh.getBoundingBox().getCenter();
					}

					auto stream = castor::makeStringStream();
					stream << position[0] << cuT( " " ) << position[1] << cuT( " " ) << position[2];

					options.meshes << ( cuT( "\nmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );
					options.meshes << ( cuT( "{\n" ) );
					options.meshes << ( cuT( "\timport \"Meshes/" ) + ( options.subfolder.empty() ? castor::cuEmptyString : ( options.subfolder + cuT( "/" ) ) ) + options.name + cuT( ".cmsh\"\n" ) );
					auto material = options.object.getSubmesh( 0u )->getDefaultMaterial();

					if ( material )
					{
						options.meshes << ( cuT( "\tdefault_material \"" ) + material->getName() + cuT( "\"\n" ) );
					}

					options.meshes << ( cuT( "}\n" ) );
					bool hasGeometries = false;
					{
						auto lock( castor::makeUniqueLock( options.geometries ) );

						for ( auto & geomIt : options.geometries )
						{
							if ( geomIt.second->getMesh().get() == split.mesh )
							{
								hasGeometries = true;
								auto node = geomIt.second->getParent();
								auto nodeName = node->getName() + cuT( "_" ) + options.name;

								options.nodes << ( cuT( "\nscene_node \"" ) + nodeName + cuT( "\"\n" ) );
								options.nodes << ( cuT( "{\n" ) );
								options.nodes << ( cuT( "\tparent \"" ) + node->getName() + cuT( "\"\n" ) );
								options.nodes << ( cuT( "\tposition " ) + stream.str() + cuT( "\n" ) );
								options.nodes << ( cuT( "}\n" ) );

								options.objects << ( cuT( "\nobject \"" ) + geomIt.first + cuT( "_" ) + options.name + cuT( "\"\n" ) );
								options.objects << ( cuT( "{\n" ) );
								options.objects << ( cuT( "\tparent \"" ) + nodeName + cuT( "\"\n" ) );
								options.objects << ( cuT( "\tmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );
								auto material = geomIt.second->getMaterial( *split.submesh );

								if ( material )
								{
									options.objects << ( cuT( "\tmaterial \"" ) + material->getName() + cuT( "\"\n" ) );
								}

								options.objects << ( cuT( "}\n" ) );
							}
						}
					}

					if ( !hasGeometries )
					{
						options.nodes << ( cuT( "\nscene_node \"Node_" ) + options.name + cuT( "\"\n" ) );
						options.nodes << ( cuT( "{\n" ) );
						options.nodes << ( cuT( "\tposition " ) + stream.str() + cuT( "\n" ) );
						options.nodes << ( cuT( "}\n" ) );

						options.objects << ( cuT( "\nobject \"" ) + options.name + cuT( "\"\n" ) );
						options.objects << ( cuT( "{\n" ) );
						options.objects << ( cuT( "\tparent \"Node_" ) + options.name + cuT( "\"\n" ) );
						options.objects << ( cuT( "\tmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );
						options.objects << ( cuT( "}\n" ) );
					}
				}
				else
				{
					auto skeleton = options.object.getSkeleton();

					if ( skeleton )
					{
						result = writeObjectT< false >( SkeletonWriterOptions{ options
								, *skeleton
								, options.object.getName() }
							, split );
					}
				}

				return result;
			}
		};

		template< bool SplitT >
		struct ObjectPostWriterT< SplitT, castor3d::Skeleton >
		{
			bool operator()( SkeletonWriterOptions const & options
				, SplitInfo const & split )
			{
				bool result = true;

				if constexpr ( !SplitT )
				{
					for ( auto & animation : options.object.getAnimations() )
					{
						if ( result )
						{
							castor::BinaryFile animFile{ options.path / ( options.name + cuT( "-" ) + animation.first + cuT( ".cska" ) )
								, castor::File::OpenMode::eWrite };
							result = castor3d::BinaryWriter< SkeletonAnimation >{}.write( static_cast< SkeletonAnimation const & >( *animation.second ), animFile );
						}
					}
				}

				return result;
			}
		};

		template< bool SplitT >
		struct ObjectWriterT< SplitT, castor3d::Mesh >
		{
			bool operator()( MeshWriterOptions const & options
				, SplitInfo const & split )
			{
				bool result = true;

				if constexpr ( SplitT )
				{
					uint32_t index = 0u;

					for ( auto & srcSubmesh : options.object )
					{
						if ( result )
						{
							auto stream = castor::makeStringStream();
							stream << options.name << "-S" << index;

							if ( srcSubmesh->getDefaultMaterial() )
							{
								stream << "_" << srcSubmesh->getDefaultMaterial()->getName();
							}

							auto name = stream.str();
							auto newPath = options.path / ( name + cuT( ".cmsh" ) );
							auto mesh = std::make_unique< castor3d::Mesh >( name, *options.object.getScene() );

							if ( auto skeleton = options.object.getSkeleton() )
							{
								mesh->setSkeleton( skeleton );
							}

							auto dstSubmesh = mesh->createSubmesh();
							dstSubmesh->disableSceneUpdate();
							dstSubmesh->addPoints( srcSubmesh->getPoints() );
							auto indexMapping = srcSubmesh->getIndexMapping();

							if ( indexMapping )
							{
								dstSubmesh->setIndexMapping( std::static_pointer_cast< castor3d::IndexMapping >( indexMapping->clone( *dstSubmesh ) ) );
							}

							auto bones = srcSubmesh->getComponent< castor3d::BonesComponent >();

							if ( bones )
							{
								dstSubmesh->addComponent( bones->getType()
									, bones->clone( *dstSubmesh ) );
							}

							auto morph = srcSubmesh->getComponent< castor3d::MorphComponent >();

							if ( morph )
							{
								dstSubmesh->addComponent( morph->getType()
									, morph->clone( *dstSubmesh ) );
							}

							dstSubmesh->setDefaultMaterial( srcSubmesh->getDefaultMaterial() );
							dstSubmesh->computeContainers();

							if ( options.options.recenter )
							{
								castor::Point3f position = dstSubmesh->getBoundingBox().getCenter();

								for ( auto & point : dstSubmesh->getPoints() )
								{
									point.pos -= position;
								}
							}

							castor::BinaryFile file{ newPath, castor::File::OpenMode::eWrite };
							castor3d::BinaryWriter< castor3d::Mesh > writer;
							result = writer.write( *mesh, file );

							if ( result )
							{
								result = postWriteT< true >( MeshWriterOptions{ options
										, *mesh
										, mesh->getName() }
									, SplitInfo{ &options.object, srcSubmesh.get() } );
							}

							mesh->cleanup();
						}

						++index;
					}
				}
				else
				{
					auto newPath = options.path / ( options.name + cuT( ".cmsh" ) );
					{
						castor::BinaryFile file{ newPath, castor::File::OpenMode::eWrite };
						castor3d::BinaryWriter< castor3d::Mesh > writer;
						result = writer.write( options.object, file );
					}
				}

				if ( result )
				{
					result = postWriteT< false >( options, split );
				}

				return result;
			}
		};

		template< bool SplitT >
		struct ObjectWriterT< SplitT, castor3d::Skeleton >
		{
			bool operator()( SkeletonWriterOptions const & options
				, SplitInfo const & split )
			{
				auto newPath = options.path / ( options.name + cuT( ".cskl" ) );
				castor::BinaryFile file{ newPath, castor::File::OpenMode::eWrite };
				castor3d::BinaryWriter< castor3d::Skeleton > writer;
				auto result = writer.write( options.object, file );

				if ( result )
				{
					result = postWriteT< false >( options, split );
				}

				return result;
			}
		};
	}

	namespace
	{
		bool writeMaterials( castor::Path const & folder
			, castor::Path const & filePath
			, Scene const & scene
			, castor::TextWriter< Scene >::Options & options )
		{
			bool result = false;
			castor::StringStream stream;
			{
				result = writeView< Sampler >( scene.getSamplerView()
					, cuT( "Samplers" )
					, stream );

				if ( result )
				{
					stream << ( cuT( "\n" ) );
					result = writeView< Material >( scene.getMaterialView()
						, cuT( "Materials" )
						, options.rootFolder
						, options.subfolder
						, stream );
				}
			}
			if ( result )
			{
				options.materialsFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Materials.cscn" ) );
				castor::TextFile file{ folder / options.materialsFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( stream.str() ) > 0;
			}
			return result;
		}

		bool writeLights( castor::Path const & folder
			, castor::Path const & filePath
			, Scene const & scene
			, castor::TextWriter< Scene >::Options & options )
		{
			bool result = false;
			castor::StringStream stream;
			{
				result = writeCache< Light >( scene.getLightCache()
					, cuT( "Lights" )
					, stream
					, []( Light const & object )
					{
						return true;
					} );
			}
			if ( result )
			{
				options.lightsFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Lights.cscn" ) );
				castor::TextFile file{ folder / options.lightsFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( stream.str() ) > 0;
			}
			return result;
		}

		bool writeNodes( castor::Path const & folder
			, castor::Path const & filePath
			, Scene const & scene
			, castor::TextWriter< Scene >::Options & options
			, ExportOptions const & exportOptions
			, castor::StringStream & stream )
		{
			stream << "// Nodes\n";
			bool result = true;
			{
				castor::TextWriter< SceneNode > writer{ castor::cuEmptyString
					, exportOptions.scale };

				for ( auto const & it : scene.getObjectRootNode()->getChildren() )
				{
					result = result
						&& writer( *it.second.lock(), stream );
				}
			}
			return result;
		}
	}

	//*********************************************************************************************

	CscnSceneExporter::CscnSceneExporter( ExportOptions options )
		: SceneExporter{ std::move( options ) }
	{
	}

	bool CscnSceneExporter::exportScene( Scene const & scene
		, castor::Path const & fileName )
	{
		castor::Path folder = fileName.getPath() / fileName.getFileName();

		if ( !castor::File::directoryExists( folder ) )
		{
			castor::File::directoryCreate( folder );
		}

		castor::TextWriter< Scene >::Options options;
		options.rootFolder = fileName.getPath();
		castor::String dataSubfolder;

		if ( m_options.dataSubfolders )
		{
			dataSubfolder = fileName.getFileName();
			options.subfolder = dataSubfolder;
			options.rootFolder = options.rootFolder / dataSubfolder;
		}

		castor::Path meshFolder{ folder / cuT( "Meshes" ) };

		if ( m_options.dataSubfolders )
		{
			meshFolder /= dataSubfolder;
		}

		if ( !castor::File::directoryExists( meshFolder ) )
		{
			castor::File::directoryCreate( meshFolder );
		}

		castor::Path filePath{ folder / ( fileName.getFileName() + cuT( ".cscn" ) ) };

		if ( !castor::File::directoryExists( folder / cuT( "Helpers" ) ) )
		{
			castor::File::directoryCreate( folder / cuT( "Helpers" ) );
		}

		bool result = writeMaterials( folder, filePath, scene, options );

		if ( result )
		{
			options.meshesFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Meshes.cscn" ) );
			options.objectsFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Objects.cscn" ) );
			options.nodesFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Nodes.cscn" ) );

			if ( result )
			{
				result = writeLights( folder
					, filePath
					, scene
					, options );
			}

			castor::StringStream meshes;
			castor::StringStream nodes;
			castor::StringStream objects;

			if ( result )
			{
				result = writeNodes( folder
					, filePath
					, scene
					, options
					, m_options
					, nodes );
			}

			if ( m_options.splitPerMaterial )
			{
				meshes << "// Meshes\n";
				objects << "// Objects\n";
				auto lock( castor::makeUniqueLock( scene.getMeshCache() ) );

				for ( auto & meshIt : scene.getMeshCache() )
				{
					if ( result && meshIt.second->isSerialisable() )
					{
						result = writeObjectT< true >( MeshWriterOptions{ m_options
								, *meshIt.second
								, scene.getGeometryCache()
								, meshes
								, nodes
								, objects
								, meshFolder
								, meshIt.first
								, options.subfolder }
							, { nullptr, nullptr } );
					}
				}
			}
			else
			{
				result = unsplitted::writeMeshes( folder
					, filePath
					, scene
					, options
					, meshes );

				if ( result )
				{
					result = unsplitted::writeObjects( folder
						, filePath
						, scene
						, options
						, objects );
				}

				if ( result )
				{
					auto lock( castor::makeUniqueLock( scene.getMeshCache() ) );

					for ( auto const & meshIt : scene.getMeshCache() )
					{
						if ( result && meshIt.second->isSerialisable() )
						{
							result = writeObjectT< false >( MeshWriterOptions{ m_options
									, *meshIt.second
									, scene.getGeometryCache()
									, meshes
									, nodes
									, objects
									, meshFolder
									, meshIt.first
									, options.subfolder }
								, { nullptr, nullptr } );
						}
					}
				}
			}

			if ( result )
			{
				castor::TextFile file{ folder / options.meshesFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( meshes.str() ) > 0;
			}

			if ( result )
			{
				castor::TextFile file{ folder / options.objectsFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( objects.str() ) > 0;
			}

			if ( result )
			{
				castor::TextFile file{ folder / options.nodesFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( nodes.str() ) > 0;
			}

			if ( result )
			{
				castor::StringStream stream;
				result = castor::TextWriter< Scene >( castor::cuEmptyString
					, std::move( options ) )( scene, stream );

				if ( result )
				{
					castor::TextFile scnFile{ castor::Path{ filePath }
						, castor::File::OpenMode::eWrite };
					result = scnFile.writeText( stream.str() ) > 0;
				}
			}
		}

		return result;
	}
}

