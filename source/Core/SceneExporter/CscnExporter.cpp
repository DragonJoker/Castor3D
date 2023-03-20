#include "SceneExporter/CscnExporter.hpp"

#include "Text/TextCtrlLayoutControl.hpp"
#include "Text/TextCtrlPanel.hpp"
#include "Text/TextGeometry.hpp"
#include "Text/TextLight.hpp"
#include "Text/TextMaterial.hpp"
#include "Text/TextMesh.hpp"
#include "Text/TextSampler.hpp"
#include "Text/TextScene.hpp"
#include "Text/TextSceneNode.hpp"
#include "Text/TextSkeleton.hpp"
#include "Text/TextStylesHolder.hpp"
#include "Text/TextTheme.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinaryMeshAnimation.hpp>
#include <Castor3D/Binary/BinarySceneNodeAnimation.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Binary/BinarySkeletonAnimation.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/LightCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Gui/ControlsManager.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimation.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/IndexMapping.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimation.hpp>
#include <Castor3D/Scene/Animation/SceneNode/SceneNodeAnimationInstance.hpp>
#include <Castor3D/Scene/Animation/AnimatedSceneNode.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Data/Text/TextFont.hpp>

namespace castor3d::exporter
{
	namespace
	{
		castor::Path normalizePath( castor::Path path )
		{
			return castor::Path{ castor::string::replace( path, "|", "-" ) };
		}

		template< typename ObjType >
		using FilterFuncT = bool ( * )( ObjType const & obj );

		template< typename ObjType, typename ViewType, typename ... Params >
		bool writeView( ViewType const & view
			, castor::String const & elemsName
			, castor::StringStream & file
			, FilterFuncT< ObjType > filter = []( ObjType const & )
			{
				return true;
			}
			, Params && ... params )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				file << ( cuT( "// " ) + elemsName + cuT( "\n" ) );
				log::info << cuT( "Scene::write - " ) << elemsName << cuT( "\n" );
				castor::TextWriter< ObjType > writer{ castor::cuEmptyString
					, std::forward< Params >( params )... };

				for ( auto const & name : view )
				{
					if ( auto elem = view.find( name ).lock() )
					{
						if ( filter( *elem ) )
						{
							result = result && writer( *elem, file );
						}
					}
				}
			}

			return result;
		}

		template< typename ObjType, typename ViewType, typename ... Params >
		bool writeView( ViewType const & view
			, castor::String const & elemsName
			, castor::StringStream & sceneFile
			, castor::StringStream & globalFile
			, FilterFuncT< ObjType > filter = []( ObjType const & )
			{
				return true;
			}
			, Params && ... params )
		{
			bool result = true;
			auto & cache = view.getCache();

			if ( !cache.isEmpty() )
			{
				log::info << cuT( "Scene::write - " ) << elemsName << cuT( "\n" );
				globalFile << ( cuT( "// " ) + elemsName + cuT( "\n" ) );

				if ( !view.isEmpty() )
				{
					sceneFile << ( cuT( "// " ) + elemsName + cuT( "\n" ) );
				}

				castor::TextWriter< ObjType > writer{ castor::cuEmptyString
					, std::forward< Params >( params )... };
				auto lock( castor::makeUniqueLock( cache ) );

				for ( auto const & elemIt : cache )
				{
					auto name = elemIt.first;

					if ( auto elem = elemIt.second )
					{
						if ( filter( *elem ) )
						{
							if ( view.has( name ) )
							{
								result = result && writer( *elem, sceneFile );
							}
							else
							{
								result = result && writer( *elem, globalFile );
							}
						}
					}
				}
			}

			return result;
		}

		template< typename ObjType, typename CacheType, typename ... Params >
		bool writeCache( CacheType const & cache
			, castor::String const & elemsName
			, castor::StringStream & file
			, FilterFuncT< ObjType > filter = []( ObjType const & )
			{
				return true;
			}
			, Params && ... params )
		{
			bool result = true;

			if ( !cache.isEmpty() )
			{
				file << ( cuT( "// " ) + elemsName + cuT( "\n" ) );
				log::info << ( cuT( "Scene::write - " ) + elemsName );
				castor::TextWriter< ObjType > writer{ castor::cuEmptyString
					, std::forward< Params >( params )... };
				auto lock( castor::makeUniqueLock( cache ) );

				for ( auto const & elemIt : cache )
				{
					if ( auto elem = elemIt.second.get() )
					{
						if ( filter( *elem ) )
						{
							result = result && writer( *elem, file );
						}
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
					if ( auto elem = elemIt.second.get() )
					{
						if ( filter( *elem ) )
						{
							result = result && writer( *elem, file );
						}
					}
				}
			}

			return result;
		}
	}

	namespace unsplitted
	{
		namespace
		{
			bool writeSkeletons( castor::Path const & folder
				, castor::Path const & filePath
				, Scene const & scene
				, castor::TextWriter< Scene >::Options & options
				, castor::StringStream & stream )
			{
				bool result = false;
				{
					result = writeCache< Skeleton >( scene.getSkeletonCache()
						, cuT( "Skeletons" )
						, options.subfolder
						, stream
						, []( Skeleton const & object )
						{
							return true;
						} );
				}
				return result;
			}

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
							auto mesh = object.getMesh().lock();
							return mesh && mesh->isSerialisable();
						} );
				}
				return result;
			}
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
			ObjectWriterOptionsT( ExportOptions const & poptions
				, ObjectT const & pobject
				, GeometryCache const & pgeometries
				, castor::StringStream & pskeletons
				, castor::StringStream & pmeshes
				, castor::StringStream & pnodes
				, castor::StringStream & pobjects
				, castor::Path ppath
				, castor::String pname
				, castor::String psubfolder
				, castor::String poutputName
				, bool psingleMesh )
				: options{ poptions }
				, object{ pobject }
				, geometries{ pgeometries }
				, skeletons{ pskeletons }
				, meshes{ pmeshes }
				, nodes{ pnodes }
				, objects{ pobjects }
				, path{ ppath }
				, name{ pname }
				, subfolder{ psubfolder }
				, outputName{ poutputName }
				, singleMesh{ psingleMesh }
			{
			}

			template< typename ObjectU >
			ObjectWriterOptionsT( ObjectWriterOptionsT< ObjectU > const & poptions
				, ObjectT const & pobject
				, castor::String pname )
				: options{ poptions.options }
				, object{ pobject }
				, geometries{ poptions.geometries }
				, skeletons{ poptions.skeletons }
				, meshes{ poptions.meshes }
				, nodes{ poptions.nodes }
				, objects{ poptions.objects }
				, path{ poptions.path }
				, name{ pname }
				, subfolder{ poptions.subfolder }
				, outputName{ poptions.outputName }
				, singleMesh{ poptions.singleMesh }
			{
			}

			ExportOptions const & options;
			ObjectT const & object;
			GeometryCache const & geometries;
			castor::StringStream & skeletons;
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

		template<>
		struct ObjectWriterOptionsT< castor3d::SceneNode >
		{
			ObjectWriterOptionsT( ExportOptions const & options
				, SceneNode const & object
				, castor::Path path
				, castor::String name
				, castor::String subfolder
				, castor::String outputName )
				: options{ options }
				, object{ object }
				, path{ path }
				, name{ name }
				, subfolder{ subfolder }
				, outputName{ outputName }
			{
			}

			template< typename ObjectU >
			ObjectWriterOptionsT( ObjectWriterOptionsT< ObjectU > const & options
				, SceneNode const & object
				, castor::String name )
				: options{ options.options }
				, object{ object }
				, path{ options.path }
				, name{ name }
				, subfolder{ options.subfolder }
				, outputName{ options.outputName }
			{
			}

			ExportOptions const & options;
			SceneNode const & object;
			castor::Path path;
			castor::String name;
			castor::String subfolder;
			castor::String outputName;
		};

		using SceneNodeWriterOptions = ObjectWriterOptionsT< castor3d::SceneNode >;

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
						if ( auto submesh = options.object.getSubmesh( 0u ) )
						{
							position = submesh->getBoundingBox().getCenter();
						}
					}

					auto stream = castor::makeStringStream();
					stream << position[0] << cuT( " " ) << position[1] << cuT( " " ) << position[2];

					options.meshes << ( cuT( "\nmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );
					options.meshes << ( cuT( "{\n" ) );
					options.meshes << ( cuT( "\timport \"Meshes/" ) + ( options.subfolder.empty() ? castor::cuEmptyString : ( options.subfolder + cuT( "/" ) ) ) + options.name + cuT( ".cmsh\"\n" ) );

					if ( auto skeleton = options.object.getSkeleton() )
					{
						options.meshes << ( cuT( "\tskeleton \"" ) + skeleton->getName() + cuT( "\"\n" ) );
					}

					if ( auto material = options.object.getSubmesh( 0u )->getDefaultMaterial() )
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
								auto subMaterial = geomIt.second->getMaterial( *split.submesh );

								if ( subMaterial )
								{
									options.objects << ( cuT( "\tmaterial \"" ) + subMaterial->getName() + cuT( "\"\n" ) );
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
					if ( options.singleMesh )
					{
						options.meshes << ( cuT( "\nmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );
						options.meshes << ( cuT( "{\n" ) );
						options.meshes << ( cuT( "\timport \"Meshes/" ) + ( options.subfolder.empty() ? castor::cuEmptyString : ( options.subfolder + cuT( "/" ) ) ) + options.name + cuT( ".cmsh\"\n" ) );

						if ( auto skeleton = options.object.getSkeleton() )
						{
							options.meshes << ( cuT( "\tskeleton \"" ) + skeleton->getName() + cuT( "\"\n" ) );
						}

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

				if constexpr ( SplitT )
				{
					options.skeletons << ( cuT( "\nskeleton \"Skeleton_" ) + options.name + cuT( "\"\n" ) );
					options.skeletons << ( cuT( "{\n" ) );
					options.skeletons << ( cuT( "\timport \"Skeletons/" ) + ( options.subfolder.empty() ? castor::cuEmptyString : ( options.subfolder + cuT( "/" ) ) ) + options.name + cuT( ".cskl\"\n" ) );
					options.skeletons << ( cuT( "}\n" ) );
				}
				else
				{
					if ( options.singleMesh )
					{
						options.skeletons << ( cuT( "\nskeleton \"Skeleton_" ) + options.name + cuT( "\"\n" ) );
						options.skeletons << ( cuT( "{\n" ) );
						options.skeletons << ( cuT( "\timport \"Skeletons/" ) + ( options.subfolder.empty() ? castor::cuEmptyString : ( options.subfolder + cuT( "/" ) ) ) + options.name + cuT( ".cskl\"\n" ) );
						options.skeletons << ( cuT( "}\n" ) );
					}

					for ( auto & animation : options.object.getAnimations() )
					{
						if ( result )
						{
							castor::BinaryFile animFile{ normalizePath( options.path / ( options.name + cuT( "-" ) + animation.first + cuT( ".cska" ) ) )
								, castor::File::OpenMode::eWrite };
							result = castor3d::BinaryWriter< SkeletonAnimation >{}.write( static_cast< SkeletonAnimation const & >( *animation.second ), animFile );
						}
					}
				}

				return result;
			}
		};

		template< bool SplitT >
		struct ObjectPostWriterT< SplitT, castor3d::SceneNode >
		{
			bool operator()( SceneNodeWriterOptions const & options
				, SplitInfo const & split )
			{
				bool result = true;

				if constexpr ( !SplitT )
				{
					for ( auto & animation : options.object.getAnimations() )
					{
						if ( result )
						{
							castor::BinaryFile animFile{ normalizePath( options.path / ( options.name + cuT( "-" ) + animation.first + cuT( ".csna" ) ) )
								, castor::File::OpenMode::eWrite };
							result = castor3d::BinaryWriter< SceneNodeAnimation >{}.write( static_cast< SceneNodeAnimation const & >( *animation.second ), animFile );
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
							auto newPath = normalizePath( options.path / ( name + cuT( ".cmsh" ) ) );
							auto mesh = std::make_unique< castor3d::Mesh >( name, *options.object.getScene() );

							if ( auto skeleton = options.object.getSkeleton() )
							{
								mesh->setSkeleton( skeleton );
							}

							auto dstSubmesh = mesh->createSubmesh();
							dstSubmesh->disableSceneUpdate();

							if ( auto positions = srcSubmesh->getComponent< castor3d::PositionsComponent >() )
							{
								dstSubmesh->addComponent( positions->clone( *dstSubmesh ) );
							}

							if ( auto normals = srcSubmesh->getComponent< castor3d::NormalsComponent >() )
							{
								dstSubmesh->addComponent( normals->clone( *dstSubmesh ) );
							}

							if ( auto tangents = srcSubmesh->getComponent< castor3d::TangentsComponent >() )
							{
								dstSubmesh->addComponent( tangents->clone( *dstSubmesh ) );
							}

							if ( auto texcoords = srcSubmesh->getComponent< castor3d::Texcoords0Component >() )
							{
								dstSubmesh->addComponent( texcoords->clone( *dstSubmesh ) );
							}

							if ( auto texcoords = srcSubmesh->getComponent< castor3d::Texcoords1Component >() )
							{
								dstSubmesh->addComponent( texcoords->clone( *dstSubmesh ) );
							}

							if ( auto texcoords = srcSubmesh->getComponent< castor3d::Texcoords2Component >() )
							{
								dstSubmesh->addComponent( texcoords->clone( *dstSubmesh ) );
							}

							if ( auto texcoords = srcSubmesh->getComponent< castor3d::Texcoords3Component >() )
							{
								dstSubmesh->addComponent( texcoords->clone( *dstSubmesh ) );
							}

							if ( auto colours = srcSubmesh->getComponent< castor3d::ColoursComponent >() )
							{
								dstSubmesh->addComponent( colours->clone( *dstSubmesh ) );
							}

							if ( auto indexMapping = srcSubmesh->getIndexMapping() )
							{
								dstSubmesh->addComponent( std::static_pointer_cast< castor3d::IndexMapping >( indexMapping->clone( *dstSubmesh ) ) );
							}

							if ( auto bones = srcSubmesh->getComponent< castor3d::SkinComponent >() )
							{
								dstSubmesh->addComponent( bones->clone( *dstSubmesh ) );
							}

							if ( auto morph = srcSubmesh->getComponent< castor3d::MorphComponent >() )
							{
								dstSubmesh->addComponent( morph->clone( *dstSubmesh ) );
							}

							dstSubmesh->setDefaultMaterial( srcSubmesh->getDefaultMaterial() );
							dstSubmesh->computeContainers();

							if ( options.options.recenter )
							{
								castor::Point3f position = dstSubmesh->getBoundingBox().getCenter();

								for ( auto & point : dstSubmesh->getPositions() )
								{
									point -= position;
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
					auto newPath = normalizePath( options.path / ( options.name + cuT( ".cmsh" ) ) );
					{
						castor::BinaryFile file{ newPath, castor::File::OpenMode::eWrite };
						castor3d::BinaryWriter< castor3d::Mesh > writer;
						result = writer.write( options.object, file );
					}

					for ( auto & animation : options.object.getAnimations() )
					{
						if ( result )
						{
							castor::BinaryFile animFile{ normalizePath( options.path / ( options.object.getName() + cuT( "-" ) + animation.first + cuT( ".cmsa" ) ) )
								, castor::File::OpenMode::eWrite };
							result = castor3d::BinaryWriter< MeshAnimation >{}.write( static_cast< MeshAnimation const & >( *animation.second ), animFile );
						}
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
				auto newPath = normalizePath( options.path / ( options.name + cuT( ".cskl" ) ) );
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
		bool writeSamplers( castor::Path const & folder
			, castor::Path const & filePath
			, Scene const & scene
			, castor::TextWriter< Scene >::Options & options )
		{
			log::info << cuT( "Scene::write - Samplers\n" );
			castor::StringStream sceneStream;
			castor::StringStream globalStream;
			std::set< castor3d::SamplerSPtr > sceneSamplers;
			std::set< castor3d::SamplerSPtr > globalSamplers;

			for ( auto & materialIt : scene.getEngine()->getMaterialCache() )
			{
				auto materialName = materialIt.first;

				if ( auto material = materialIt.second )
				{
					if ( scene.hasMaterial( materialName ) )
					{
						for ( auto & pass : *material )
						{
							for ( auto & unit : pass->getTextureUnits() )
							{
								sceneSamplers.insert( unit->getSampler().lock() );
							}
						}
					}
					else
					{
						for ( auto & pass : *material )
						{
							for ( auto & unit : pass->getTextureUnits() )
							{
								globalSamplers.insert( unit->getSampler().lock() );
							}
						}
					}
				}
			}

			castor::TextWriter< Sampler > writer{ castor::cuEmptyString };
			bool result = true;

			for ( auto & sampler : sceneSamplers )
			{
				result = result && writer( *sampler, sceneStream );
			}

			for ( auto & sampler : globalSamplers )
			{
				result = result && writer( *sampler, globalStream );
			}

			if ( result && !sceneStream.str().empty() )
			{
				options.sceneSamplersFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Samplers.cscn" ) );
				castor::TextFile file{ folder / options.sceneSamplersFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( "// Samplers\n" ) > 0
					&& file.writeText( sceneStream.str() ) > 0;
			}

			if ( result && !globalStream.str().empty() )
			{
				options.globalSamplersFile = cuT( "Helpers" ) / castor::Path( cuT( "Global-Samplers.cscn" ) );
				castor::TextFile file{ folder / options.globalSamplersFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( "// Samplers\n" ) > 0
					&& file.writeText( globalStream.str() ) > 0;
			}

			return result;
		}

		bool writeMaterials( castor::Path const & folder
			, castor::Path const & filePath
			, Scene const & scene
			, castor::TextWriter< Scene >::Options & options )
		{
			bool result = false;
			castor::StringStream sceneStream;
			castor::StringStream globalStream;
			result = writeView< Material >( scene.getMaterialView()
				, cuT( "Materials" )
				, sceneStream
				, globalStream
				, []( Material const & object )
				{
					return object.isSerialisable();
				}
				, options.rootFolder
				, options.subfolder );

			if ( result && !sceneStream.str().empty() )
			{
				options.sceneMaterialsFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Materials.cscn" ) );
				castor::TextFile file{ folder / options.sceneMaterialsFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( sceneStream.str() ) > 0;
			}

			if ( result && !globalStream.str().empty() )
			{
				options.globalMaterialsFile = cuT( "Helpers" ) / castor::Path( cuT( "Global-Materials.cscn" ) );
				castor::TextFile file{ folder / options.globalMaterialsFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( globalStream.str() ) > 0;
			}

			return result;
		}

		bool writeFonts( castor::Path const & folder
			, castor::Path const & filePath
			, Scene const & scene
			, castor::TextWriter< Scene >::Options & options )
		{
			bool result = false;
			castor::StringStream sceneStream;
			castor::StringStream globalStream;
			result = writeView< castor::Font >( scene.getFontView()
				, cuT( "Fonts" )
				, sceneStream
				, globalStream
				, []( castor::Font const & object )
				{
					return object.isSerialisable();
				}
				, options.rootFolder );

			if ( result && !sceneStream.str().empty() )
			{
				options.sceneFontsFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Fonts.cscn" ) );
				castor::TextFile file{ folder / options.sceneFontsFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( sceneStream.str() ) > 0;
			}

			if ( result && !globalStream.str().empty() )
			{
				options.globalFontsFile = cuT( "Helpers" ) / castor::Path( cuT( "Global-Fonts.cscn" ) );
				castor::TextFile file{ folder / options.globalFontsFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( globalStream.str() ) > 0;
			}

			return result;
		}

		bool writeGuiThemes( castor::Path const & folder
			, castor::Path const & filePath
			, Scene const & scene
			, castor::TextWriter< Scene >::Options & options )
		{
			auto & manager = static_cast< ControlsManager const & >( *scene.getEngine()->getUserInputListener() );
			castor::TextWriter< Theme > globalWriter{ castor::cuEmptyString, nullptr };
			castor::TextWriter< Theme > sceneWriter{ castor::cuEmptyString, &scene };
			castor::StringStream sceneStream;
			castor::StringStream globalStream;
			bool result{ true };

			for ( auto & theme : manager.getThemes() )
			{
				if ( theme.first == "Debug" )
				{
					continue;
				}

				if ( result )
				{
					result = sceneWriter( *theme.second, sceneStream );
				}

				if ( result )
				{
					result = globalWriter( *theme.second, globalStream );
				}
			}

			if ( result && !globalStream.str().empty() )
			{
				options.globalThemesFile = cuT( "Helpers" ) / castor::Path( cuT( "Global-GUI-Themes.cscn" ) );
				castor::TextFile file{ folder / options.globalThemesFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( "// GUI Themes\n" ) > 0
					&& file.writeText( globalStream.str() ) > 0;
			}

			if ( result && !sceneStream.str().empty() )
			{
				options.sceneThemesFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-GUI-Themes.cscn" ) );
				castor::TextFile file{ folder / options.sceneThemesFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( "// GUI Themes\n" ) > 0
					&& file.writeText( sceneStream.str() ) > 0;
			}

			return result;
		}

		bool writeGuiStyles( castor::Path const & folder
			, castor::Path const & filePath
			, Scene const & scene
			, castor::TextWriter< Scene >::Options & options )
		{
			auto & manager = static_cast< ControlsManager const & >( *scene.getEngine()->getUserInputListener() );
			castor::TextWriter< StylesHolder > globalWriter{ castor::cuEmptyString, nullptr, "" };
			castor::TextWriter< StylesHolder > sceneWriter{ castor::cuEmptyString, &scene, "" };
			castor::StringStream sceneStream;
			castor::StringStream globalStream;
			auto result = sceneWriter( manager, sceneStream );

			if ( result )
			{
				result = globalWriter( manager, globalStream );
			}

			if ( result && !globalStream.str().empty() )
			{
				options.globalStylesFile = cuT( "Helpers" ) / castor::Path( cuT( "Global-GUI-Styles.cscn" ) );
				castor::TextFile file{ folder / options.globalStylesFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( "// GUI Styles\n" ) > 0
					&& file.writeText( globalStream.str() ) > 0;
			}

			if ( result && !sceneStream.str().empty() )
			{
				options.sceneStylesFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-GUI-Styles.cscn" ) );
				castor::TextFile file{ folder / options.sceneStylesFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( "// GUI Styles\n" ) > 0
					&& file.writeText( sceneStream.str() ) > 0;
			}

			return result;
		}

		bool writeGuiControls( castor::Path const & folder
			, castor::Path const & filePath
			, Scene const & scene
			, castor::TextWriter< Scene >::Options & options )
		{
			auto & manager = static_cast< ControlsManager const & >( *scene.getEngine()->getUserInputListener() );
			castor::TextWriter< StylesHolder > writer{ castor::cuEmptyString, nullptr, "" };
			castor::StringStream sceneStream;
			castor::StringStream globalStream;
			auto filter = [&scene, &sceneStream, &globalStream]( Control const & control ) -> castor::StringStream *
			{
				if ( control.getName() == "Debug/Main"
					|| control.getName() == "Debug/RenderPasses" )
				{
					return nullptr;
				}

				if ( !control.hasScene() )
				{
					return &globalStream;
				}

				if ( control.hasScene()
					&& &control.getScene() == &scene )
				{
					return &sceneStream;
				}

				return nullptr;
			};

			bool result = true;

			for ( auto control : manager.getRootControls() )
			{
				if ( result )
				{
					if ( auto stream = filter( *control ) )
					{
						result = writeControl( writer, *control, *stream );
					}
				}
			}

			if ( result && !globalStream.str().empty() )
			{
				options.globalControlsFile = cuT( "Helpers" ) / castor::Path( cuT( "Global-GUI-Controls.cscn" ) );
				castor::TextFile file{ folder / options.globalControlsFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( "// GUI Controls\n" ) > 0
					&& file.writeText( globalStream.str() ) > 0;
			}

			if ( result && !sceneStream.str().empty() )
			{
				options.sceneControlsFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-GUI-Controls.cscn" ) );
				castor::TextFile file{ folder / options.sceneControlsFile
					, castor::File::OpenMode::eWrite };
				result = file.writeText( "// GUI Controls\n" ) > 0
					&& file.writeText( sceneStream.str() ) > 0;
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
			if ( result && !stream.str().empty() )
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
					auto node = it.second;

					if ( node->hasAnimation() )
					{
						auto found = node->getScene()->getAnimatedObjectGroupCache().findObject( node->getName() + "_Node" );

						if ( !found.empty() )
						{
							auto animNode = static_cast< AnimatedSceneNode * >( found.front() );

							if ( animNode->isPlayingAnimation() )
							{
								auto & anim = animNode->getPlayingAnimation();
								auto pos = node->getPosition();
								auto rot = node->getOrientation();
								auto scl = node->getScale();
								node->setPosition( anim.getInitialPosition() );
								node->setOrientation( anim.getInitialOrientation() );
								node->setScale( anim.getInitialScale() );
								result = result
									&& writer( *node, stream );
								node->setPosition( pos );
								node->setOrientation( rot );
								node->setScale( scl );
							}
						}
					}
					else
					{
						result = result
							&& writer( *node, stream );
					}

					result = postWriteT< false >( SceneNodeWriterOptions{ exportOptions
							, *node
							, options.rootFolder / options.nodesFile.getPath()
							, it.first
							, options.subfolder
							, it.first }
						, { nullptr, nullptr } );
				}
			}
			return result;
		}

		castor::TextWriter< castor3d::Scene >::Options prepareExport( ExportOptions const & exportOptions
			, castor::Path const & fileName
			, castor::Path & folder
			, castor::Path & filePath
			, castor::Path & skeletonFolder
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

			skeletonFolder = folder / cuT( "Skeletons" );
			meshFolder = folder / cuT( "Meshes" );

			if ( exportOptions.dataSubfolders )
			{
				skeletonFolder /= dataSubfolder;
				meshFolder /= dataSubfolder;
			}

			if ( !castor::File::directoryExists( skeletonFolder ) )
			{
				castor::File::directoryCreate( skeletonFolder );
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

			options.skeletonsFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Skeletons.cscn" ) );
			options.meshesFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Meshes.cscn" ) );
			options.objectsFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Objects.cscn" ) );
			options.nodesFile = cuT( "Helpers" ) / castor::Path( filePath.getFileName( false ) + cuT( "-Nodes.cscn" ) );
			options.scale = exportOptions.scale;
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

		void printRenderWindow( castor::String const & sceneName
			, castor::String const & cameraName
			, castor::StringStream & stream )
		{
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
			stream << "		scene \"" << sceneName << "\"\n";
			stream << "		camera \"" << cameraName << "\"\n";
			stream << "		tone_mapping \"linear\"\n";
			stream << "	}\n";
			stream << "}\n";
		}

		bool finaliseExport( castor3d::Mesh const * singleMesh
			, castor::TextWriter< castor3d::Scene >::Options & options
			, castor::StringStream const & skeletons
			, castor::StringStream const & meshes
			, castor::StringStream const & nodes
			, castor::StringStream const & objects
			, castor3d::Scene const & scene
			, castor::Path const & folder
			, castor::Path const & filePath )
		{
			bool result = true;
			auto skl = skeletons.str();
			auto msh = meshes.str();
			auto obj = objects.str();
			auto nod = nodes.str();

			if ( !skl.empty() )
			{
				castor::TextFile sklFile{ folder / options.skeletonsFile
					, castor::File::OpenMode::eWrite };
				result = sklFile.writeText( skl ) > 0;
			}
			else
			{
				options.skeletonsFile.clear();
			}

			if ( result && !msh.empty() )
			{
				castor::TextFile mshFile{ folder / options.meshesFile
					, castor::File::OpenMode::eWrite };
				result = mshFile.writeText( msh ) > 0;
			}
			else
			{
				options.meshesFile.clear();
			}

			if ( result && !obj.empty() )
			{
				castor::TextFile objFile{ folder / options.objectsFile
					, castor::File::OpenMode::eWrite };
				result = objFile.writeText( obj ) > 0;
			}
			else
			{
				options.objectsFile.clear();
			}

			if ( result && !nod.empty() )
			{
				castor::TextFile nodFile{ folder / options.nodesFile
					, castor::File::OpenMode::eWrite };
				result = nodFile.writeText( nod ) > 0;
			}
			else
			{
				options.nodesFile.clear();
			}

			if ( result )
			{
				castor::StringStream stream;

				if ( singleMesh )
				{
					auto name = filePath.getFileName();
					stream << "// Global configuration\n";
					stream << "materials " << scene.getDefaultLightingModelName() << "\n";

					if ( !options.globalSamplersFile.empty() )
					{
						stream << "include \"" << options.globalSamplersFile << "\"\n";
					}

					if ( !options.globalMaterialsFile.empty() )
					{
						stream << "include \"" << options.globalMaterialsFile << "\"\n";
					}

					if ( !options.globalFontsFile.empty() )
					{
						stream << "include \"" << options.globalFontsFile << "\"\n";
					}

					if ( !options.globalThemesFile.empty() )
					{
						stream << "include \"" << options.globalThemesFile << "\"\n";
					}

					if ( !options.globalStylesFile.empty() )
					{
						stream << "include \"" << options.globalStylesFile << "\"\n";
					}

					if ( !options.globalControlsFile.empty() )
					{
						stream << "include \"" << options.globalControlsFile << "\"\n";
					}

					stream << "\n";
					stream << "scene \"" << name << "\"\n";
					stream << "{\n";
					stream << "	// Scene configuration\n";
					stream << "	ambient_light 1.0 1.0 1.0\n";
					stream << "	background_colour 0.50000 0.50000 0.50000\n";

					if ( !options.sceneSamplersFile.empty() )
					{
						stream << "include \"" << options.sceneSamplersFile << "\"\n";
					}

					if ( !options.sceneMaterialsFile.empty() )
					{
						stream << "include \"" << options.sceneMaterialsFile << "\"\n";
					}

					if ( !options.sceneFontsFile.empty() )
					{
						stream << "include \"" << options.sceneFontsFile << "\"\n";
					}

					if ( !options.sceneThemesFile.empty() )
					{
						stream << "include \"" << options.sceneThemesFile << "\"\n";
					}

					if ( !options.sceneStylesFile.empty() )
					{
						stream << "include \"" << options.sceneStylesFile << "\"\n";
					}

					if ( !options.sceneControlsFile.empty() )
					{
						stream << "include \"" << options.sceneControlsFile << "\"\n";
					}

					if ( !skl.empty() )
					{
						stream << "	include \"Helpers/" << name << "-Skeletons.cscn\"\n";
					}

					if ( !msh.empty() )
					{
						stream << "	include \"Helpers/" << name << "-Meshes.cscn\"\n";
					}

					if ( !nod.empty() )
					{
						stream << "	include \"Helpers/" << name << "-Nodes.cscn\"\n";
					}

					if ( !nod.empty() )
					{
						stream << "	include \"Helpers/" << name << "-Objects.cscn\"\n";
					}

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
					stream << "	}\n";
					stream << "}\n";
					printRenderWindow( name, "MainCamera", stream );
				}
				else
				{
					result = castor::TextWriter< Scene >{ castor::cuEmptyString, options }( scene, stream );

					if ( scene.getEngine()->getRenderWindows().empty() )
					{
						if ( !scene.getCameraCache().isEmpty() )
						{
							auto camera = scene.getCameraCache().begin()->second;
							printRenderWindow( scene.getName()
								, camera->getName()
								, stream );
						}
					}
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
		castor::Path skeletonFolder;
		castor::Path meshFolder;
		auto options = prepareExport( m_options
			, outputFolder
			, folder
			, filePath
			, skeletonFolder
			, meshFolder );
		bool result = writeSamplers( folder
			, filePath
			, scene
			, options );

		if ( result )
		{
			result = writeMaterials( folder
				, filePath
				, scene
				, options );
		}

		if ( result )
		{
			castor::StringStream skeletons;
			castor::StringStream meshes;
			castor::StringStream nodes;
			castor::StringStream objects;
			skeletons << "// Skeletons\n";
			meshes << "// Meshes\n";
			nodes << "// Nodes\n";
			objects << "// Objects\n";
			auto skeleton = mesh.getSkeleton();

			if ( m_options.splitPerMaterial )
			{
				if ( skeleton )
				{
					result = writeObjectT< true >( SkeletonWriterOptions{ m_options
							, *skeleton
							, scene.getGeometryCache()
							, skeletons
							, meshes
							, nodes
							, objects
							, skeletonFolder
							, skeleton->getName()
							, options.subfolder
							, outputName
							, true }
						, { nullptr, nullptr } );
				}

				if ( result )
				{
					result = writeObjectT< true >( MeshWriterOptions{ m_options
							, mesh
							, scene.getGeometryCache()
							, skeletons
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
			}
			else
			{
				if ( skeleton )
				{
					result = writeObjectT< false >( SkeletonWriterOptions{ m_options
							, *skeleton
							, scene.getGeometryCache()
							, skeletons
							, meshes
							, nodes
							, objects
							, skeletonFolder
							, skeleton->getName()
							, options.subfolder
							, outputName
							, true }
						, { nullptr, nullptr } );
				}

				if ( result )
				{
					result = writeObjectT< false >( MeshWriterOptions{ m_options
							, mesh
							, scene.getGeometryCache()
							, skeletons
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
			}

			if ( result )
			{
				result = finaliseExport( &mesh
					, options
					, skeletons
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
		castor::Path skeletonFolder;
		castor::Path meshFolder;
		auto options = prepareExport( m_options
			, fileName
			, folder
			, filePath
			, skeletonFolder
			, meshFolder );
		bool result = writeMaterials( folder
			, filePath
			, scene
			, options );

		if ( result )
		{
			result = writeFonts( folder
				, filePath
				, scene
				, options );
		}

		if ( result )
		{
			result = writeGuiThemes( folder
				, filePath
				, scene
				, options );
		}

		if ( result )
		{
			result = writeGuiStyles( folder
				, filePath
				, scene
				, options );
		}

		if ( result )
		{
			result = writeGuiControls( folder
				, filePath
				, scene
				, options );
		}

		if ( result )
		{
			result = writeLights( folder
				, filePath
				, scene
				, options );

			castor::StringStream skeletons;
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

				for ( auto & skelIt : scene.getSkeletonCache() )
				{
					result = writeObjectT< true >( SkeletonWriterOptions{ m_options
							, *skelIt.second
							, scene.getGeometryCache()
							, skeletons
							, meshes
							, nodes
							, objects
							, skeletonFolder
							, skelIt.first
							, options.subfolder
							, skelIt.first
							, false }
						, { nullptr, nullptr } );
				}

				for ( auto & meshIt : scene.getMeshCache() )
				{
					if ( result && meshIt.second->isSerialisable() )
					{
						result = writeObjectT< true >( MeshWriterOptions{ m_options
								, *meshIt.second
								, scene.getGeometryCache()
								, skeletons
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
				result = unsplitted::writeSkeletons( folder
					, filePath
					, scene
					, options
					, skeletons );

				if ( result )
				{
					result = unsplitted::writeMeshes( folder
						, filePath
						, scene
						, options
						, meshes );
				}

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

					for ( auto const & skelIt : scene.getSkeletonCache() )
					{
						result = writeObjectT< false >( SkeletonWriterOptions{ m_options
								, *skelIt.second
								, scene.getGeometryCache()
								, skeletons
								, meshes
								, nodes
								, objects
								, skeletonFolder
								, skelIt.first
								, options.subfolder
								, skelIt.first
								, false }
							, { nullptr, nullptr } );
					}

					for ( auto const & meshIt : scene.getMeshCache() )
					{
						if ( result && meshIt.second->isSerialisable() )
						{
							result = writeObjectT< false >( MeshWriterOptions{ m_options
									, *meshIt.second
									, scene.getGeometryCache()
									, skeletons
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
					, skeletons
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

