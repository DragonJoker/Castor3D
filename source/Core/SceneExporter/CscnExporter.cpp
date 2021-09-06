#include "SceneExporter/CscnExporter.hpp"

#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Binary/BinarySkeletonAnimation.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/LightCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
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

#include <CastorUtils/Design/ResourceCache.hpp>

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
					auto elem = view.find( name ).lock();

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
					auto elem = view.find( name ).lock();

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
						return object.getMesh().lock()
							&& object.getMesh().lock()->isSerialisable();
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
				, castor::String subfolder
				, castor::String outputName
				, bool singleMesh )
				: options{ options }
				, object{ object }
				, geometries{ geometries }
				, meshes{ meshes }
				, nodes{ nodes }
				, objects{ objects }
				, path{ path }
				, name{ name }
				, subfolder{ subfolder }
				, outputName{ outputName }
				, singleMesh{ singleMesh }
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
				, outputName{ options.outputName }
				, singleMesh{ options.singleMesh }
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
			castor::String outputName;
			bool singleMesh;
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
							if ( geomIt.second->getMesh().lock().get() == split.mesh )
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

					if ( options.singleMesh )
					{
						options.meshes << ( cuT( "\nmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );
						options.meshes << ( cuT( "{\n" ) );
						options.meshes << ( cuT( "\timport \"Meshes/" ) + ( options.subfolder.empty() ? castor::cuEmptyString : ( options.subfolder + cuT( "/" ) ) ) + options.name + cuT( ".cmsh\"\n" ) );
						options.meshes << ( cuT( "}\n" ) );

						options.nodes << ( cuT( "\nscene_node \"Node_" ) + options.name + cuT( "\"\n" ) );
						options.nodes << ( cuT( "{\n" ) );
						options.nodes << ( cuT( "\tposition 0.0 0.0 0.0\n" ) );
						options.nodes << ( cuT( "}\n" ) );

						options.objects << ( cuT( "\nobject \"" ) + options.name + cuT( "\"\n" ) );
						options.objects << ( cuT( "{\n" ) );
						options.objects << ( cuT( "\tparent \"Node_" ) + options.name + cuT( "\"\n" ) );
						options.objects << ( cuT( "\tmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );
						options.objects << ( cuT( "\tmaterials\n" ) );
						options.objects << ( cuT( "\t{\n" ) );
						uint32_t index = 0u;

						for ( auto & submesh : options.object )
						{
							auto material = submesh->getDefaultMaterial();
							options.objects << ( cuT( "\t\tmaterial " ) + castor::string::toString( index++ ) + cuT( " \"" ) + material->getName() + cuT( "\"\n" ) );
						}

						options.objects << cuT( "\t}\n" );
						options.objects << cuT( "}\n" );
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

		castor::TextWriter< castor3d::Scene >::Options prepareExport( ExportOptions const & exportOptions
			, castor::Path const & fileName
			, castor::Path & folder
			, castor::Path & filePath
			, castor::Path & meshFolder )
		{
			folder = fileName.getPath();

			if ( !castor::File::directoryExists( folder ) )
			{
				castor::File::directoryCreate( folder );
			}

			castor::TextWriter< castor3d::Scene >::Options options;
			options.rootFolder = fileName.getPath();
			castor::String dataSubfolder;

			if ( exportOptions.dataSubfolders )
			{
				dataSubfolder = fileName.getFileName();
				options.subfolder = dataSubfolder;
			}

			meshFolder = folder / cuT( "Meshes" );

			if ( exportOptions.dataSubfolders )
			{
				meshFolder /= dataSubfolder;
			}

			if ( !castor::File::directoryExists( meshFolder ) )
			{
				castor::File::directoryCreate( meshFolder );
			}

			filePath = folder / ( fileName.getFileName() + cuT( ".cscn" ) );

			if ( !castor::File::directoryExists( folder / cuT( "Helpers" ) ) )
			{
				castor::File::directoryCreate( folder / cuT( "Helpers" ) );
			}

			options.meshesFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Meshes.cscn" ) );
			options.objectsFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Objects.cscn" ) );
			options.nodesFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Nodes.cscn" ) );
			return options;
		}

		castor::String getCameraPosition( castor3d::Mesh const & mesh
			, float & farPlane )
		{
			auto aabb = mesh.getBoundingBox();
			auto height = aabb.getDimensions()->y;
			auto z = -( height * 1.5f );
			farPlane = std::abs( z ) + std::max( aabb.getMax()->z, std::max( aabb.getMax()->x, aabb.getMax()->y ) ) * 2.0f;
			return castor::string::toString( aabb.getCenter()->x ) + cuT( " " )
				+ castor::string::toString( aabb.getCenter()->y ) + cuT( " " )
				+ castor::string::toString( z );
		}

		bool finaliseExport( castor3d::Mesh const * singleMesh
			, castor::TextWriter< castor3d::Scene >::Options const & options
			, castor::StringStream const & meshes
			, castor::StringStream const & nodes
			, castor::StringStream const & objects
			, castor3d::Scene const & scene
			, castor::Path const & folder
			, castor::Path const & filePath )
		{
			castor::TextFile file{ folder / options.meshesFile
				, castor::File::OpenMode::eWrite };
			auto result = file.writeText( meshes.str() ) > 0;

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

				if ( singleMesh )
				{
					auto name = filePath.getFileName();
					stream << "// Global configuration\n";
					stream << "materials " << scene.getPassesName() << "\n";
					stream << "\n";
					stream << "scene \"" << name << "\"\n";
					stream << "{\n";
					stream << "	// Scene configuration\n";
					stream << "	ambient_light 1.0 1.0 1.0\n";
					stream << "	background_colour 0.50000 0.50000 0.50000\n";
					stream << "	include \"Helpers/" << name << "-Materials.cscn\"\n";
					stream << "	include \"Helpers/" << name << "-Meshes.cscn\"\n";
					stream << "	include \"Helpers/" << name << "-Nodes.cscn\"\n";
					stream << "	include \"Helpers/" << name << "-Objects.cscn\"\n";
					stream << "\n";
					stream << "\n";
					stream << "	//Cameras nodes\n";
					stream << "\n";
					stream << "	scene_node \"MainCameraNode\"\n";
					stream << "	{\n";
					float farPlane = 0.0f;
					stream << "		position " << getCameraPosition( *singleMesh, farPlane ) << "\n";
					stream << "	}\n";
					stream << "\n";
					stream << "	//Cameras\n";
					stream << "\n";
					stream << "	camera \"MainCamera\"\n";
					stream << "	{\n";
					stream << "		parent \"MainCameraNode\"\n";
					stream << "\n";
					stream << "		viewport\n";
					stream << "		{\n";
					stream << "			type perspective\n";
					stream << "			near 0.100000\n";
					stream << "			far " << farPlane << "\n";
					stream << "			aspect_ratio 1.77800\n";
					stream << "			fov_y 45.0000\n";
					stream << "		}\n";
					stream << "\n";
					stream << "		hdr_config\n";
					stream << "		{\n";
					stream << "			exposure 1.00000\n";
					stream << "			gamma 2.20000\n";
					stream << "		}\n";
					stream << "	}\n";
					stream << "\n";
					stream << "	scene_node \"LightNode\"\n";
					stream << "	{\n";
					stream << "		orientation 1 0 0 90\n";
					stream << "	}\n";
					stream << "\n";
					stream << "	light \"SunLight\"\n";
					stream << "	{\n";
					stream << "		parent \"LightNode\"\n";
					stream << "		type directional\n";
					stream << "		colour 1.00000 1.00000 1.00000\n";
					stream << "		intensity 8.0 10.0\n";
					stream << "		shadow_producer false\n";
					stream << "	}\n";
					stream << "}\n";
					stream << "\n";
					stream << "//Windows\n";
					stream << "\n";
					stream << "window \"MainWindow\"\n";
					stream << "{\n";
					stream << "	vsync false\n";
					stream << "	fullscreen false\n";
					stream << "\n";
					stream << "	render_target\n";
					stream << "	{\n";
					stream << "		size 1920 1080\n";
					stream << "		format argb32\n";
					stream << "		scene \"" << name << "\"\n";
					stream << "		camera \"MainCamera\"\n";
					stream << "		tone_mapping \"linear\"\n";
					stream << "	}\n";
					stream << "}\n";
				}
				else
				{
					result = castor::TextWriter< Scene >( castor::cuEmptyString
						, std::move( options ) )( scene, stream );
				}

				if ( result )
				{
					castor::TextFile scnFile{ castor::Path{ filePath }
					, castor::File::OpenMode::eWrite };
					result = scnFile.writeText( stream.str() ) > 0;
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

	bool CscnSceneExporter::exportMesh( castor3d::Scene const & scene
		, castor3d::Mesh const & mesh
		, castor::Path const & outputFolder
		, castor::String const & outputName )
	{
		castor::Path folder;
		castor::Path filePath;
		castor::Path meshFolder;
		auto options = prepareExport( m_options
			, outputFolder
			, folder
			, filePath
			, meshFolder );
		bool result = writeMaterials( folder
			, filePath
			, scene
			, options );

		if ( result )
		{
			castor::StringStream meshes;
			castor::StringStream nodes;
			castor::StringStream objects;
			meshes << "// Meshes\n";
			nodes << "// Nodes\n";
			objects << "// Objects\n";

			if ( m_options.splitPerMaterial )
			{
				result = writeObjectT< true >( MeshWriterOptions{ m_options
						, mesh
						, scene.getGeometryCache()
						, meshes
						, nodes
						, objects
						, meshFolder
						, mesh.getName()
						, options.subfolder
						, outputName
						, true }
					, { nullptr, nullptr } );
			}
			else
			{
				result = writeObjectT< false >( MeshWriterOptions{ m_options
						, mesh
						, scene.getGeometryCache()
						, meshes
						, nodes
						, objects
						, meshFolder
						, mesh.getName()
						, options.subfolder
						, outputName
						, true }
					, { nullptr, nullptr } );
			}

			if ( result )
			{
				result = finaliseExport( &mesh
					, options
					, meshes
					, nodes
					, objects
					, scene
					, folder
					, filePath );
			}
		}

		return result;
	}

	bool CscnSceneExporter::exportScene( castor3d::Scene const & scene
		, castor::Path const & fileName )
	{
		castor::Path folder;
		castor::Path filePath;
		castor::Path meshFolder;
		auto options = prepareExport( m_options
			, fileName
			, folder
			, filePath
			, meshFolder );
		bool result = writeMaterials( folder
			, filePath
			, scene
			, options );

		if ( result )
		{
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
								, options.subfolder
								, meshIt.first
								, false }
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
									, options.subfolder
									, meshIt.first
									, false }
								, { nullptr, nullptr } );
						}
					}
				}
			}

			if ( result )
			{
				result = finaliseExport( nullptr
					, options
					, meshes
					, nodes
					, objects
					, scene
					, folder
					, filePath );
			}
		}

		return result;
	}
}

