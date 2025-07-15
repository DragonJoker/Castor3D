#include "SceneExporter/CscnExporter.hpp"

#include "Text/TextCtrlLayoutControl.hpp"
#include "Text/TextCtrlPanel.hpp"
#include "Text/TextGeometry.hpp"
#include "Text/TextLight.hpp"
#include "Text/TextLightGroup.hpp"
#include "Text/TextMaterial.hpp"
#include "Text/TextMesh.hpp"
#include "Text/TextSampler.hpp"
#include "Text/TextScene.hpp"
#include "Text/TextSceneNode.hpp"
#include "Text/TextSkeleton.hpp"
#include "Text/TextStylesHolder.hpp"
#include "Text/TextTextureData.hpp"
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
#include <Castor3D/Cache/TextureCache.hpp>
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

namespace c3d::exporter
{
	namespace
	{
		bool carryOn( bool result, bool ignoreFailures )noexcept
		{
			result = result || ignoreFailures;

			if ( !result )
			{
				log::error << cuT( "Failed\n" );
			}

			return result;
		}

		bool carryOn( bool result, ExportOptions const & options )noexcept
		{
			return carryOn( result, options.ignoreFailures );
		}

		template< typename ObjType >
		using FilterFuncT = bool ( * )( ObjType const & obj );
		template< typename ObjType >
		inline FilterFuncT< ObjType > const defaultFilterT = []( ObjType const & )
			{
				return true;
			};

		template< typename ObjType, typename ViewType, typename ... Params >
		bool writeView( bool ignoreFailures
			, ViewType const & view
			, String const & elemsName
			, StringStream & file
			, FilterFuncT< ObjType > filter = defaultFilterT< ObjType >
			, Params && ... params )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				file << ( cuT( "// " ) + elemsName + cuT( "\n" ) );
				log::info << cuT( "SceneExporter::write - " ) << elemsName << cuT( "\n" );
				TextWriter< ObjType > writer{ cuEmptyString
					, c3d::forward< Params >( params )... };

				for ( auto const & name : view )
				{
					if ( auto elem = view.find( name ) )
					{
						if ( carryOn( result, ignoreFailures ) && filter( *elem ) )
						{
							result = writer( *elem, file );
						}
					}
				}
			}

			return result;
		}

		template< typename ObjType, typename ViewType, typename ... Params >
		bool writeView( bool ignoreFailures
			, ViewType const & view
			, String const & elemsName
			, StringStream & sceneFile
			, StringStream & globalFile
			, FilterFuncT< ObjType > filter = defaultFilterT< ObjType >
			, Params && ... params )
		{
			bool result = true;

			if ( auto & cache = view.getCache();
				!cache.isEmpty() )
			{
				auto scount = 0u;
				auto gcount = 0u;
				StringStream sstream;
				StringStream gstream;
				log::info << cuT( "SceneExporter::write - " ) << elemsName << cuT( "\n" );
				gstream << ( cuT( "// " ) + elemsName + cuT( "\n" ) );

				if ( !view.isEmpty() )
				{
					sstream << ( cuT( "// " ) + elemsName + cuT( "\n" ) );
				}

				TextWriter< ObjType > writer{ cuEmptyString
					, c3d::forward< Params >( params )... };
				auto lock( makeUniqueLock( cache ) );

				for ( auto const & elemIt : cache )
				{
					auto name = elemIt.first;

					if ( auto elem = elemIt.second.get() )
					{
						if ( carryOn( result, ignoreFailures ) && filter( *elem ) )
						{
							if ( view.has( name ) )
							{
								result = writer( *elem, sstream );
								++scount;
							}
							else
							{
								result = writer( *elem, gstream );
								++gcount;
							}
						}
					}
				}

				if ( gcount )
				{
					globalFile << gstream.str();
				}

				if ( scount )
				{
					sceneFile << sstream.str();
				}
			}

			return result;
		}

		template< typename ObjType, typename CacheType, typename ... Params >
		bool writeCache( bool ignoreFailures
			, CacheType const & cache
			, String const & elemsName
			, StringStream & file
			, FilterFuncT< ObjType > filter = defaultFilterT< ObjType >
			, Params && ... params )
		{
			bool result = true;

			if ( !cache.isEmpty() )
			{
				file << ( cuT( "// " ) + elemsName + cuT( "\n" ) );
				log::info << ( cuT( "SceneExporter::write - " ) + elemsName ) << cuT( "\n" );
				TextWriter< ObjType > writer{ cuEmptyString
					, c3d::forward< Params >( params )... };
				auto lock( makeUniqueLock( cache ) );

				for ( auto const & elemIt : cache )
				{
					if ( auto elem = elemIt.second.get() )
					{
						if ( carryOn( result, ignoreFailures ) && filter( *elem ) )
						{
							result = writer( *elem, file );
						}
					}
				}
			}

			return result;
		}

		template< typename ObjType, typename CacheType >
		bool writeCache( bool ignoreFailures
			, CacheType const & cache
			, String const & elemsName
			, String const & subfolder
			, StringStream & file
			, FilterFuncT< ObjType > filter = defaultFilterT< ObjType > )
		{
			bool result = true;

			if ( !cache.isEmpty() )
			{
				file << ( cuT( "// " ) + elemsName + cuT( "\n" ) );
				log::info << ( cuT( "SceneExporter::write - " ) + elemsName ) << cuT( "\n" );
				TextWriter< ObjType > writer{ cuEmptyString, subfolder };
				auto lock( makeUniqueLock( cache ) );

				for ( auto const & elemIt : cache )
				{
					if ( auto elem = elemIt.second.get() )
					{
						if ( carryOn( result, ignoreFailures ) && filter( *elem ) )
						{
							result = writer( *elem, file );
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
			bool writeSkeletons( bool ignoreFailures
				, Path const & /*folder*/
				, Path const & /*filePath*/
				, Scene const & scene
				, TextWriter< Scene >::Options const & options
				, StringStream & stream )
			{
				bool result = false;
				{
					result = writeCache< Skeleton >( ignoreFailures
						, scene.getSkeletonCache()
						, cuT( "Skeletons" )
						, options.subfolder
						, stream
						, []( Skeleton const & )
						{
							return true;
						} );
				}
				return result;
			}

			bool writeMeshes( bool ignoreFailures
				, Path const & /*folder*/
				, Path const & /*filePath*/
				, Scene const & scene
				, TextWriter< Scene >::Options const & options
				, StringStream & stream )
			{
				bool result = false;
				{
					result = writeCache< Mesh >( ignoreFailures
						, scene.getMeshCache()
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

			bool writeObjects( bool ignoreFailures
				, Path const & /*folder*/
				, Path const & /*filePath*/
				, Scene const & scene
				, TextWriter< Scene >::Options const & /*options*/
				, StringStream & stream )
			{
				bool result = false;
				{
					result = writeCache< Geometry >( ignoreFailures
						, scene.getGeometryCache()
						, cuT( "Objects" )
						, stream
						, []( Geometry const & object )
						{
							auto mesh = object.getMesh();
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
				, StringStream & pskeletons
				, StringStream & pmeshes
				, StringStream & pnodes
				, StringStream & pobjects
				, Path ppath
				, String pname
				, String psubfolder
				, String poutputName
				, bool psingleMesh )
				: options{ poptions }
				, object{ pobject }
				, geometries{ pgeometries }
				, skeletons{ pskeletons }
				, meshes{ pmeshes }
				, nodes{ pnodes }
				, objects{ pobjects }
				, path{ c3d::move( ppath ) }
				, name{ c3d::move( pname ) }
				, subfolder{ c3d::move( psubfolder ) }
				, outputName{ c3d::move( poutputName ) }
				, singleMesh{ psingleMesh }
			{
			}

			template< typename ObjectU >
			ObjectWriterOptionsT( ObjectWriterOptionsT< ObjectU > const & poptions
				, ObjectT const & pobject
				, String pname )
				: options{ poptions.options }
				, object{ pobject }
				, geometries{ poptions.geometries }
				, skeletons{ poptions.skeletons }
				, meshes{ poptions.meshes }
				, nodes{ poptions.nodes }
				, objects{ poptions.objects }
				, path{ poptions.path }
				, name{ c3d::move( pname ) }
				, subfolder{ poptions.subfolder }
				, outputName{ poptions.outputName }
				, singleMesh{ poptions.singleMesh }
			{
			}

			ExportOptions const & options;
			ObjectT const & object;
			GeometryCache const & geometries;
			StringStream & skeletons;
			StringStream & meshes;
			StringStream & nodes;
			StringStream & objects;
			Path path;
			String name;
			String subfolder;
			String outputName;
			bool singleMesh;
		};

		using MeshWriterOptions = ObjectWriterOptionsT< Mesh >;
		using SkeletonWriterOptions = ObjectWriterOptionsT< Skeleton >;

		template<>
		struct ObjectWriterOptionsT< SceneNode >
		{
			ObjectWriterOptionsT( ExportOptions const & options
				, SceneNode const & object
				, Path path
				, String name
				, String subfolder
				, String outputName )
				: options{ options }
				, object{ object }
				, path{ c3d::move( path ) }
				, name{ c3d::move( name ) }
				, subfolder{ c3d::move( subfolder ) }
				, outputName{ c3d::move( outputName ) }
			{
			}

			template< typename ObjectU >
			ObjectWriterOptionsT( ObjectWriterOptionsT< ObjectU > const & options
				, SceneNode const & object
				, String name )
				: options{ options.options }
				, object{ object }
				, path{ options.path }
				, name{ c3d::move( name ) }
				, subfolder{ options.subfolder }
				, outputName{ options.outputName }
			{
			}

			ExportOptions const & options;
			SceneNode const & object;
			Path path;
			String name;
			String subfolder;
			String outputName;
		};

		using SceneNodeWriterOptions = ObjectWriterOptionsT< SceneNode >;

		template< typename ObjectT >
		bool carryOn( bool result, ObjectWriterOptionsT< ObjectT > const & options )noexcept
		{
			return carryOn( result, options.options );
		}

		struct SplitInfo
		{
			Mesh const * mesh;
			Submesh const * submesh;
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
			catch ( Exception & exc )
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
		struct ObjectPostWriterT< SplitT, Mesh >
		{
			bool operator()( MeshWriterOptions const & options
				, SplitInfo const & split )const
			{
				bool result = true;

				if constexpr ( SplitT )
				{
					Point3f position;

					if ( options.options.recenter )
					{
						if ( auto submesh = options.object.getSubmesh( 0u ) )
						{
							position = submesh->getBoundingBox().getCenter();
						}
					}

					auto stream = makeStringStream();
					stream << position[0] << cuT( " " ) << position[1] << cuT( " " ) << position[2];

					options.meshes << ( cuT( "\nmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );
					options.meshes << cuT( "{\n" );
					options.meshes << ( cuT( "\timport \"Meshes/" ) + ( options.subfolder.empty() ? cuEmptyString : ( options.subfolder + cuT( "/" ) ) ) + options.name + cuT( ".cmsh\"\n" ) );

					if ( auto skeleton = options.object.getSkeleton() )
					{
						options.meshes << ( cuT( "\tskeleton \"" ) + skeleton->getName() + cuT( "\"\n" ) );
					}

					if ( auto material = options.object.getSubmesh( 0u )->getDefaultMaterial() )
					{
						options.meshes << ( cuT( "\tdefault_material \"" ) + material->getName() + cuT( "\"\n" ) );
					}

					options.meshes << cuT( "}\n" );
					bool hasGeometries = false;
					{
						auto lock( makeUniqueLock( options.geometries ) );

						for ( auto const & [name, geometry] : options.geometries )
						{
							if ( geometry->getMesh() == split.mesh )
							{
								hasGeometries = true;
								auto node = geometry->getParent();
								auto nodeName = node->getName() + cuT( "_" ) + options.name;

								options.nodes << ( cuT( "\nscene_node \"" ) + nodeName + cuT( "\"\n" ) );
								options.nodes << cuT( "{\n" );
								options.nodes << ( cuT( "\tparent \"" ) + node->getName() + cuT( "\"\n" ) );
								options.nodes << ( cuT( "\tposition " ) + stream.str() + cuT( "\n" ) );
								options.nodes << cuT( "}\n" );

								options.objects << ( cuT( "\nobject \"" ) + name + cuT( "_" ) + options.name + cuT( "\"\n" ) );
								options.objects << cuT( "{\n" );
								options.objects << ( cuT( "\tparent \"" ) + nodeName + cuT( "\"\n" ) );
								options.objects << ( cuT( "\tmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );

								if ( auto subMaterial = geometry->getMaterial( *split.submesh ) )
								{
									options.objects << ( cuT( "\tmaterial \"" ) + subMaterial->getName() + cuT( "\"\n" ) );
								}

								options.objects << cuT( "}\n" );
							}
						}
					}

					if ( !hasGeometries )
					{
						options.nodes << ( cuT( "\nscene_node \"Node_" ) + options.name + cuT( "\"\n" ) );
						options.nodes << cuT( "{\n" );
						options.nodes << ( cuT( "\tposition " ) + stream.str() + cuT( "\n" ) );
						options.nodes << cuT( "}\n" );

						options.objects << ( cuT( "\nobject \"" ) + options.name + cuT( "\"\n" ) );
						options.objects << cuT( "{\n" );
						options.objects << ( cuT( "\tparent \"Node_" ) + options.name + cuT( "\"\n" ) );
						options.objects << ( cuT( "\tmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );
						options.objects << cuT( "}\n" );
					}
				}
				else
				{
					if ( options.singleMesh )
					{
						options.meshes << ( cuT( "\nmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );
						options.meshes << cuT( "{\n" );
						options.meshes << ( cuT( "\timport \"Meshes/" ) + ( options.subfolder.empty() ? cuEmptyString : ( options.subfolder + cuT( "/" ) ) ) + options.name + cuT( ".cmsh\"\n" ) );

						if ( auto skeleton = options.object.getSkeleton() )
						{
							options.meshes << ( cuT( "\tskeleton \"" ) + skeleton->getName() + cuT( "\"\n" ) );
						}

						options.meshes << cuT( "}\n" );

						options.nodes << ( cuT( "\nscene_node \"Node_" ) + options.name + cuT( "\"\n" ) );
						options.nodes << cuT( "{\n" );
						options.nodes << cuT( "\tposition 0.0 0.0 0.0\n" );
						options.nodes << cuT( "}\n" );

						options.objects << ( cuT( "\nobject \"" ) + options.name + cuT( "\"\n" ) );
						options.objects << cuT( "{\n" );
						options.objects << ( cuT( "\tparent \"Node_" ) + options.name + cuT( "\"\n" ) );
						options.objects << ( cuT( "\tmesh \"Mesh_" ) + options.name + cuT( "\"\n" ) );
						options.objects << cuT( "\tmaterials\n" );
						options.objects << cuT( "\t{\n" );
						uint32_t index = 0u;

						for ( auto & submesh : options.object )
						{
							if ( auto material = submesh->getDefaultMaterial() )
							{
								options.objects << ( cuT( "\t\tmaterial " ) + string::toString( index ) + cuT( " \"" ) + material->getName() + cuT( "\"\n" ) );
							}

							++index;
						}

						options.objects << cuT( "\t}\n" );
						options.objects << cuT( "}\n" );
					}
				}

				return result;
			}
		};

		template< bool SplitT >
		struct ObjectPostWriterT< SplitT, Skeleton >
		{
			bool operator()( SkeletonWriterOptions const & options
				, SplitInfo const & /*split*/ )const
			{
				bool result = true;

				if constexpr ( SplitT )
				{
					options.skeletons << ( cuT( "\nskeleton \"Skeleton_" ) + options.name + cuT( "\"\n" ) );
					options.skeletons << cuT( "{\n" );
					options.skeletons << ( cuT( "\timport \"Skeletons/" ) + ( options.subfolder.empty() ? cuEmptyString : ( options.subfolder + cuT( "/" ) ) ) + options.name + cuT( ".cskl\"\n" ) );
					options.skeletons << cuT( "}\n" );
				}
				else
				{
					if ( options.singleMesh )
					{
						options.skeletons << ( cuT( "\nskeleton \"Skeleton_" ) + options.name + cuT( "\"\n" ) );
						options.skeletons << cuT( "{\n" );
						options.skeletons << ( cuT( "\timport \"Skeletons/" ) + ( options.subfolder.empty() ? cuEmptyString : ( options.subfolder + cuT( "/" ) ) ) + options.name + cuT( ".cskl\"\n" ) );
						options.skeletons << cuT( "}\n" );
					}

					for ( auto const & [name, animation] : options.object.getAnimations() )
					{
						if ( carryOn( result, options ) )
						{
							BinaryFile animFile{ options.path / File::normaliseFileName( options.name + cuT( "-" ) + name + cuT( ".cska" ) )
								, File::OpenMode::eWrite };
							result = BinaryWriter< SkeletonAnimation >{}.write( static_cast< SkeletonAnimation const & >( *animation ), animFile );
						}
					}
				}

				return result;
			}
		};

		template< bool SplitT >
		struct ObjectPostWriterT< SplitT, SceneNode >
		{
			bool operator()( SceneNodeWriterOptions const & options
				, SplitInfo const & /*split*/ )const
			{
				bool result = true;

				if constexpr ( !SplitT )
				{
					for ( auto const & [name, animation] : options.object.getAnimations() )
					{
						if ( carryOn( result, options ) )
						{
							BinaryFile animFile{ options.path / File::normaliseFileName( options.name + cuT( "-" ) + name + cuT( ".csna" ) )
								, File::OpenMode::eWrite };
							result = BinaryWriter< SceneNodeAnimation >{}.write( static_cast< SceneNodeAnimation const & >( *animation ), animFile );
						}
					}
				}

				return result;
			}
		};

		template< bool SplitT >
		struct ObjectWriterT< SplitT, Mesh >
		{
			bool operator()( MeshWriterOptions const & options
				, SplitInfo const & split )const
			{
				bool result = true;

				if constexpr ( SplitT )
				{
					uint32_t index = 0u;

					for ( auto & srcSubmesh : options.object )
					{
						if ( carryOn( result, options ) )
						{
							auto stream = makeStringStream();
							stream << options.name << "-S" << index;

							if ( srcSubmesh->getDefaultMaterial() )
							{
								stream << "_" << srcSubmesh->getDefaultMaterial()->getName();
							}

							auto name = stream.str();
							auto newPath = options.path / File::normaliseFileName( name + cuT( ".cmsh" ) );
							auto mesh = makeRawUnique< Mesh >( name, *options.object.getScene() );

							if ( auto skeleton = options.object.getSkeleton() )
							{
								mesh->setSkeleton( skeleton );
							}

							auto dstSubmesh = mesh->createSubmesh();
							dstSubmesh->disableSceneUpdate();

							if ( auto positions = srcSubmesh->getComponent< PositionsComponent >() )
							{
								dstSubmesh->addComponent( positions->clone( *dstSubmesh ) );
							}

							if ( auto normals = srcSubmesh->getComponent< NormalsComponent >() )
							{
								dstSubmesh->addComponent( normals->clone( *dstSubmesh ) );
							}

							if ( auto tangents = srcSubmesh->getComponent< TangentsComponent >() )
							{
								dstSubmesh->addComponent( tangents->clone( *dstSubmesh ) );
							}

							if ( auto bitangents = srcSubmesh->getComponent< BitangentsComponent >() )
							{
								dstSubmesh->addComponent( bitangents->clone( *dstSubmesh ) );
							}

							if ( auto texcoords = srcSubmesh->getComponent< Texcoords0Component >() )
							{
								dstSubmesh->addComponent( texcoords->clone( *dstSubmesh ) );
							}

							if ( auto texcoords = srcSubmesh->getComponent< Texcoords1Component >() )
							{
								dstSubmesh->addComponent( texcoords->clone( *dstSubmesh ) );
							}

							if ( auto texcoords = srcSubmesh->getComponent< Texcoords2Component >() )
							{
								dstSubmesh->addComponent( texcoords->clone( *dstSubmesh ) );
							}

							if ( auto texcoords = srcSubmesh->getComponent< Texcoords3Component >() )
							{
								dstSubmesh->addComponent( texcoords->clone( *dstSubmesh ) );
							}

							if ( auto colours = srcSubmesh->getComponent< ColoursComponent >() )
							{
								dstSubmesh->addComponent( colours->clone( *dstSubmesh ) );
							}

							if ( auto indexMapping = srcSubmesh->getIndexMapping() )
							{
								dstSubmesh->addComponent( indexMapping->clone( *dstSubmesh ) );
							}

							if ( auto bones = srcSubmesh->getComponent< SkinComponent >() )
							{
								dstSubmesh->addComponent( bones->clone( *dstSubmesh ) );
							}

							if ( auto morph = srcSubmesh->getComponent< MorphComponent >() )
							{
								dstSubmesh->addComponent( morph->clone( *dstSubmesh ) );
							}

							dstSubmesh->setDefaultMaterial( srcSubmesh->getDefaultMaterial() );
							dstSubmesh->computeContainers();

							if ( options.options.recenter )
							{
								Point3f position = dstSubmesh->getBoundingBox().getCenter();

								for ( auto & point : dstSubmesh->getPositions() )
								{
									point -= position;
								}
							}

							BinaryFile file{ newPath, File::OpenMode::eWrite };
							BinaryWriter< Mesh > writer;
							result = writer.write( *mesh, file );

							if ( carryOn( result, options ) )
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
					auto newPath = options.path / File::normaliseFileName( options.name + cuT( ".cmsh" ) );
					{
						BinaryFile file{ newPath, File::OpenMode::eWrite };
						BinaryWriter< Mesh > writer;
						result = writer.write( options.object, file );
					}

					for ( auto const & [name, animation] : options.object.getAnimations() )
					{
						if ( carryOn( result, options ) )
						{
							BinaryFile animFile{ options.path / File::normaliseFileName( options.object.getName() + cuT( "-" ) + name + cuT( ".cmsa" ) )
								, File::OpenMode::eWrite };
							result = BinaryWriter< MeshAnimation >{}.write( static_cast< MeshAnimation const & >( *animation ), animFile );
						}
					}
				}

				if ( carryOn( result, options ) )
				{
					result = postWriteT< false >( options, split );
				}

				return result;
			}
		};

		template< bool SplitT >
		struct ObjectWriterT< SplitT, Skeleton >
		{
			bool operator()( SkeletonWriterOptions const & options
				, SplitInfo const & split )const
			{
				auto newPath = options.path / File::normaliseFileName( options.name + cuT( ".cskl" ) );
				BinaryFile file{ newPath, File::OpenMode::eWrite };
				BinaryWriter< Skeleton > writer;
				auto result = writer.write( options.object, file );

				if ( carryOn( result, options ) )
				{
					result = postWriteT< false >( options, split );
				}

				return result;
			}
		};
	}

	namespace
	{
		bool writeTextures( bool ignoreFailures
			, Path const & folder
			, Path const & filePath
			, Scene const & scene
			, TextWriter< Scene >::Options & options )
		{
			log::info << cuT( "SceneExporter::write - Textures\n" );
			StringStream sceneStream;
			StringMap< TextureData const * > sorted;

			for ( auto & [hash, textureData] : scene.getEngine()->getTextureUnitCache() )
			{
				auto name = textureData->sourceInfo.name();
				if ( name.find( cuT( "C3D_Default" ) ) == String::npos )
				{
					sorted.try_emplace( name, textureData.get() );
				}
			}

			TextWriter< TextureData > writer{ cuEmptyString
				, *scene.getEngine()
				, options.rootFolder
				, options.subfolder };
			bool result = true;

			for ( auto const & [_, sourceData] : sorted )
			{
				result = carryOn( result, ignoreFailures ) && writer( *sourceData, sceneStream );
			}

			if ( carryOn( result, ignoreFailures ) && !sceneStream.str().empty() )
			{
				options.sceneTexturesFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-Textures.cscn" ) );
				TextFile file{ folder / options.sceneTexturesFile
					, File::OpenMode::eWrite };
				result = file.writeText( cuT( "// Textures\n" ) ) > 0
					&& file.writeText( sceneStream.str() ) > 0;
			}

			return result;
		}

		bool writeSamplers( bool ignoreFailures
			, Path const & folder
			, Path const & filePath
			, Scene const & scene
			, TextWriter< Scene >::Options & options )
		{
			log::info << cuT( "SceneExporter::write - Samplers\n" );
			StringStream sceneStream;
			StringStream globalStream;
			Set< Sampler const * > sceneSamplers;
			Set< Sampler const * > globalSamplers;

			for ( auto const & [materialName, material] : scene.getEngine()->getMaterialCache() )
			{
				if ( material )
				{
					if ( scene.hasMaterial( materialName ) )
					{
						for ( auto const & pass : *material )
						{
							for ( auto & unit : pass->getTextureUnits() )
							{
								if ( unit->getSampler().isSerialisable() )
								{
									sceneSamplers.emplace( &unit->getSampler() );
								}
							}
						}
					}
					else
					{
						for ( auto const & pass : *material )
						{
							for ( auto & unit : pass->getTextureUnits() )
							{
								if ( unit->getSampler().isSerialisable() )
								{
									globalSamplers.emplace( &unit->getSampler() );
								}
							}
						}
					}
				}
			}

			TextWriter< Sampler > writer{ cuEmptyString };
			bool result = true;

			for ( auto & sampler : sceneSamplers )
			{
				result = carryOn( result, ignoreFailures ) && writer( *sampler, sceneStream );
			}

			for ( auto & sampler : globalSamplers )
			{
				result = carryOn( result, ignoreFailures ) && writer( *sampler, globalStream );
			}

			if ( carryOn( result, ignoreFailures ) && !sceneStream.str().empty() )
			{
				options.sceneSamplersFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-Samplers.cscn" ) );
				TextFile file{ folder / options.sceneSamplersFile
					, File::OpenMode::eWrite };
				result = file.writeText( cuT( "// Samplers\n" ) ) > 0
					&& file.writeText( sceneStream.str() ) > 0;
			}

			if ( carryOn( result, ignoreFailures ) && !globalStream.str().empty() )
			{
				options.globalSamplersFile = cuT( "Helpers" ) / Path( cuT( "Global-Samplers.cscn" ) );
				TextFile file{ folder / options.globalSamplersFile
					, File::OpenMode::eWrite };
				result = file.writeText( cuT( "// Samplers\n" ) ) > 0
					&& file.writeText( globalStream.str() ) > 0;
			}

			return result;
		}

		bool writeMaterials( bool ignoreFailures
			, Path const & folder
			, Path const & filePath
			, Scene const & scene
			, TextWriter< Scene >::Options & options )
		{
			bool result = false;
			StringStream sceneStream;
			StringStream globalStream;
			result = writeView< Material >( ignoreFailures
				, scene.getMaterialView()
				, cuT( "Materials" )
				, sceneStream
				, globalStream
				, []( Material const & object )
				{
					return object.isSerialisable();
				}
				, options.rootFolder
				, options.subfolder );

			if ( carryOn( result, ignoreFailures ) && !sceneStream.str().empty() )
			{
				options.sceneMaterialsFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-Materials.cscn" ) );
				TextFile file{ folder / options.sceneMaterialsFile
					, File::OpenMode::eWrite };
				result = file.writeText( sceneStream.str() ) > 0;
			}

			if ( carryOn( result, ignoreFailures ) && !globalStream.str().empty() )
			{
				options.globalMaterialsFile = cuT( "Helpers" ) / Path( cuT( "Global-Materials.cscn" ) );
				TextFile file{ folder / options.globalMaterialsFile
					, File::OpenMode::eWrite };
				result = file.writeText( globalStream.str() ) > 0;
			}

			return result;
		}

		bool writeFonts( bool ignoreFailures
			, Path const & folder
			, Path const & filePath
			, Scene const & scene
			, TextWriter< Scene >::Options & options )
		{
			bool result = false;
			StringStream sceneStream;
			StringStream globalStream;
			result = writeView< Font >( ignoreFailures
				, scene.getFontView()
				, cuT( "Fonts" )
				, sceneStream
				, globalStream
				, []( Font const & object )
				{
					return object.isSerialisable();
				}
				, options.rootFolder );

			if ( carryOn( result, ignoreFailures ) && !sceneStream.str().empty() )
			{
				options.sceneFontsFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-Fonts.cscn" ) );
				TextFile file{ folder / options.sceneFontsFile
					, File::OpenMode::eWrite };
				result = file.writeText( sceneStream.str() ) > 0;
			}

			if ( carryOn( result, ignoreFailures ) && !globalStream.str().empty() )
			{
				options.globalFontsFile = cuT( "Helpers" ) / Path( cuT( "Global-Fonts.cscn" ) );
				TextFile file{ folder / options.globalFontsFile
					, File::OpenMode::eWrite };
				result = file.writeText( globalStream.str() ) > 0;
			}

			return result;
		}

		bool writeGuiThemes( bool ignoreFailures
			, Path const & folder
			, Path const & filePath
			, Scene const & scene
			, TextWriter< Scene >::Options & options )
		{
			log::info << cuT( "SceneExporter::write - GUI Themes\n" );
			auto & manager = static_cast< ControlsManager const & >( *scene.getEngine()->getUserInputListener() );
			TextWriter< Theme > globalWriter{ cuEmptyString, nullptr };
			TextWriter< Theme > sceneWriter{ cuEmptyString, &scene };
			StringStream sceneStream;
			StringStream globalStream;
			bool result{ true };

			for ( auto & [name, theme] : manager.getThemes() )
			{
				if ( name == cuT( "Debug" ) )
				{
					continue;
				}

				if ( carryOn( result, ignoreFailures ) )
				{
					result = sceneWriter( *theme, sceneStream );
				}

				if ( carryOn( result, ignoreFailures ) )
				{
					result = globalWriter( *theme, globalStream );
				}
			}

			if ( carryOn( result, ignoreFailures ) && !globalStream.str().empty() )
			{
				options.globalThemesFile = cuT( "Helpers" ) / Path( cuT( "Global-GUI-Themes.cscn" ) );
				TextFile file{ folder / options.globalThemesFile
					, File::OpenMode::eWrite };
				result = file.writeText( cuT( "// GUI Themes\n" ) ) > 0
					&& file.writeText( globalStream.str() ) > 0;
			}

			if ( carryOn( result, ignoreFailures ) && !sceneStream.str().empty() )
			{
				options.sceneThemesFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-GUI-Themes.cscn" ) );
				TextFile file{ folder / options.sceneThemesFile
					, File::OpenMode::eWrite };
				result = file.writeText( cuT( "// GUI Themes\n" ) ) > 0
					&& file.writeText( sceneStream.str() ) > 0;
			}

			return result;
		}

		bool writeGuiStyles( bool ignoreFailures
			, Path const & folder
			, Path const & filePath
			, Scene const & scene
			, TextWriter< Scene >::Options & options )
		{
			log::info << cuT( "SceneExporter::write - GUI Styles\n" );
			auto & manager = static_cast< ControlsManager const & >( *scene.getEngine()->getUserInputListener() );
			TextWriter< StylesHolder > globalWriter{ cuEmptyString, nullptr, cuT( "" ) };
			TextWriter< StylesHolder > sceneWriter{ cuEmptyString, &scene, cuT( "" ) };
			StringStream sceneStream;
			StringStream globalStream;
			bool result = true;

			if ( !manager.isEmpty() )
			{
				result = sceneWriter( manager, sceneStream );

				if ( carryOn( result, ignoreFailures ) )
				{
					result = globalWriter( manager, globalStream );
				}
			}

			if ( carryOn( result, ignoreFailures ) && !globalStream.str().empty() )
			{
				options.globalStylesFile = cuT( "Helpers" ) / Path( cuT( "Global-GUI-Styles.cscn" ) );
				TextFile file{ folder / options.globalStylesFile
					, File::OpenMode::eWrite };
				result = file.writeText( cuT( "// GUI Styles\n" ) ) > 0
					&& file.writeText( globalStream.str() ) > 0;
			}

			if ( carryOn( result, ignoreFailures ) && !sceneStream.str().empty() )
			{
				options.sceneStylesFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-GUI-Styles.cscn" ) );
				TextFile file{ folder / options.sceneStylesFile
					, File::OpenMode::eWrite };
				result = file.writeText( cuT( "// GUI Styles\n" ) ) > 0
					&& file.writeText( sceneStream.str() ) > 0;
			}

			return result;
		}

		bool writeGuiControls( bool ignoreFailures
			, Path const & folder
			, Path const & filePath
			, Scene const & scene
			, TextWriter< Scene >::Options & options )
		{
			log::info << cuT( "SceneExporter::write - GUI Controls\n" );
			auto & manager = static_cast< ControlsManager const & >( *scene.getEngine()->getUserInputListener() );
			TextWriter< StylesHolder > writer{ cuEmptyString, nullptr, cuT( "" ) };
			StringStream sceneStream;
			StringStream globalStream;
			auto filter = [&scene, &sceneStream, &globalStream]( Control const & control ) -> StringStream *
			{
				if ( control.getName() == cuT( "Debug/Main" )
					|| control.getName() == cuT( "Debug/RenderPasses" ) )
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
				if ( carryOn( result, ignoreFailures ) )
				{
					if ( auto stream = filter( *control ) )
					{
						result = writeControl( writer, *control, *stream );
					}
				}
			}

			if ( carryOn( result, ignoreFailures ) && !globalStream.str().empty() )
			{
				options.globalControlsFile = cuT( "Helpers" ) / Path( cuT( "Global-GUI-Controls.cscn" ) );
				TextFile file{ folder / options.globalControlsFile
					, File::OpenMode::eWrite };
				result = file.writeText( cuT( "// GUI Controls\n" ) ) > 0
					&& file.writeText( globalStream.str() ) > 0;
			}

			if ( carryOn( result, ignoreFailures ) && !sceneStream.str().empty() )
			{
				options.sceneControlsFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-GUI-Controls.cscn" ) );
				TextFile file{ folder / options.sceneControlsFile
					, File::OpenMode::eWrite };
				result = file.writeText( cuT( "// GUI Controls\n" ) ) > 0
					&& file.writeText( sceneStream.str() ) > 0;
			}

			return result;
		}

		bool writeLights( bool ignoreFailures
			, Path const & folder
			, Path const & filePath
			, Scene const & scene
			, TextWriter< Scene >::Options & options )
		{
			bool result = false;
			StringStream stream;
			{
				result = writeCache< Light >( ignoreFailures
					, scene.getLightCache()
					, cuT( "Lights" )
					, stream
					, []( Light const & )
					{
						return true;
					} );
			}
			if ( carryOn( result, ignoreFailures ) )
			{
				result = writeCache< LightGroup >( ignoreFailures
					, scene.getLightGroupCache()
					, cuT( "LightGroups" )
					, stream
					, []( LightGroup const & )
					{
						return true;
					} );
			}
			if ( carryOn( result, ignoreFailures ) && !stream.str().empty() )
			{
				options.lightsFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-Lights.cscn" ) );
				TextFile file{ folder / options.lightsFile
					, File::OpenMode::eWrite };
				result = file.writeText( stream.str() ) > 0;
			}
			return result;
		}

		bool writeNode( Path const & folder
			, Path const & filePath
			, SceneNode & node
			, TextWriter< Scene >::Options & options
			, ExportOptions const & exportOptions
			, StringStream & stream )
		{
			bool result = true;
			TextWriter< SceneNode > writer{ cuEmptyString
				, exportOptions.scale };

			if ( node.hasAnimation() )
			{
				auto found = node.getScene()->getAnimatedObjectGroupCache().findObject( node.getName() + cuT( "_Node" ) );

				if ( !found.empty() )
				{
					auto animNode = static_cast< AnimatedSceneNode * >( found.front() );

					if ( animNode->isPlayingAnimation() )
					{
						auto const & anim = animNode->getPlayingAnimation();
						auto pos = node.getPosition();
						auto rot = node.getOrientation();
						auto scl = node.getScale();
						node.setPosition( anim.getInitialPosition() );
						node.setOrientation( anim.getInitialOrientation() );
						node.setScale( anim.getInitialScale() );
						result = result
							&& writer( node, stream );
						node.setPosition( pos );
						node.setOrientation( rot );
						node.setScale( scl );
					}
				}
			}
			else
			{
				result = carryOn( result, exportOptions )
					&& writer( node, stream );
			}

			for ( auto const & [_, childNode] : node.getChildren() )
			{
				if ( result && childNode )
				{
					result = writeNode( folder, filePath, *childNode, options, exportOptions, stream );
				}
			}

			result = postWriteT< false >( SceneNodeWriterOptions{ exportOptions
					, node
					, options.rootFolder / options.nodesFile.getPath()
					, node.getName()
					, options.subfolder
					, node.getName() }
				, { nullptr, nullptr } );
			return result;
		}

		bool writeNodes( Path const & folder
			, Path const & filePath
			, Scene const & scene
			, TextWriter< Scene >::Options & options
			, ExportOptions const & exportOptions
			, StringStream & stream )
		{
			log::info << cuT( "SceneExporter::write - Scene Nodes\n" );
			stream << "// Nodes\n";
			bool result = true;
			{
				for ( auto const & [_, node] : scene.getObjectRootNode()->getChildren() )
				{
					result = writeNode( folder, filePath, *node, options, exportOptions, stream );
				}
			}
			return result;
		}

		TextWriter< Scene >::Options prepareExport( ExportOptions const & exportOptions
			, Path const & fileName
			, Path & folder
			, Path & filePath
			, Path & skeletonFolder
			, Path & meshFolder )
		{
			folder = fileName.getPath();

			if ( !File::directoryExists( folder ) )
			{
				File::directoryCreate( folder );
			}

			TextWriter< Scene >::Options options;
			options.rootFolder = fileName.getPath();
			String dataSubfolder;

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

			if ( !File::directoryExists( skeletonFolder ) )
			{
				File::directoryCreate( skeletonFolder );
			}

			if ( !File::directoryExists( meshFolder ) )
			{
				File::directoryCreate( meshFolder );
			}

			filePath = folder / ( fileName.getFileName() + cuT( ".cscn" ) );

			if ( !File::directoryExists( folder / cuT( "Helpers" ) ) )
			{
				File::directoryCreate( folder / cuT( "Helpers" ) );
			}

			options.skeletonsFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-Skeletons.cscn" ) );
			options.meshesFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-Meshes.cscn" ) );
			options.objectsFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-Objects.cscn" ) );
			options.nodesFile = cuT( "Helpers" ) / Path( filePath.getFileName( false ) + cuT( "-Nodes.cscn" ) );
			options.scale = exportOptions.scale;
			return options;
		}

		String getCameraPosition( Mesh const & mesh
			, float & farPlane )
		{
			auto const &  aabb = mesh.getBoundingBox();
			auto height = aabb.getDimensions()->y;
			auto z = -( height * 1.5f );
			farPlane = std::abs( z ) + std::max( aabb.getMax()->z, std::max( aabb.getMax()->x, aabb.getMax()->y ) ) * 2.0f;
			return string::toString( aabb.getCenter()->x ) + cuT( " " )
				+ string::toString( aabb.getCenter()->y ) + cuT( " " )
				+ string::toString( z );
		}

		void printRenderWindow( String const & sceneName
			, String const & cameraName
			, StringStream & stream )
		{
			stream << cuT( "\n" );
			stream << cuT( "//Windows\n" );
			stream << cuT( "\n" );
			stream << cuT( "window \"MainWindow\"\n" );
			stream << cuT( "{\n" );
			stream << cuT( "	vsync false\n" );
			stream << cuT( "	fullscreen false\n" );
			stream << cuT( "\n" );
			stream << cuT( "	render_target\n" );
			stream << cuT( "	{\n" );
			stream << cuT( "		size 1920 1080\n" );
			stream << cuT( "		format argb32\n" );
			stream << cuT( "		scene \"" ) << sceneName << cuT( "\"\n" );
			stream << cuT( "		camera \"" ) << cameraName << cuT( "\"\n" );
			stream << cuT( "		tone_mapping \"linear\"\n" );
			stream << cuT( "	}\n" );
			stream << cuT( "}\n" );
		}

		bool finaliseExport( ExportOptions const & exportOptions
			, Mesh const * singleMesh
			, TextWriter< Scene >::Options & options
			, StringStream const & skeletons
			, StringStream const & meshes
			, StringStream const & nodes
			, StringStream const & objects
			, Scene const & scene
			, Path const & folder
			, Path const & filePath )
		{
			bool result = true;
			auto skl = skeletons.str();
			auto msh = meshes.str();
			auto obj = objects.str();
			auto nod = nodes.str();

			if ( !skl.empty() )
			{
				TextFile sklFile{ folder / options.skeletonsFile
					, File::OpenMode::eWrite };
				result = sklFile.writeText( skl ) > 0;
			}
			else
			{
				options.skeletonsFile.clear();
			}

			if ( carryOn( result, exportOptions ) && !msh.empty() )
			{
				TextFile mshFile{ folder / options.meshesFile
					, File::OpenMode::eWrite };
				result = mshFile.writeText( msh ) > 0;
			}
			else
			{
				options.meshesFile.clear();
			}

			if ( carryOn( result, exportOptions ) && !obj.empty() )
			{
				TextFile objFile{ folder / options.objectsFile
					, File::OpenMode::eWrite };
				result = objFile.writeText( obj ) > 0;
			}
			else
			{
				options.objectsFile.clear();
			}

			if ( carryOn( result, exportOptions ) && !nod.empty() )
			{
				TextFile nodFile{ folder / options.nodesFile
					, File::OpenMode::eWrite };
				result = nodFile.writeText( nod ) > 0;
			}
			else
			{
				options.nodesFile.clear();
			}

			if ( carryOn( result, exportOptions ) )
			{
				StringStream stream;

				if ( singleMesh )
				{
					auto name = filePath.getFileName();
					stream << "// Global configuration\n";
					stream << "materials " << scene.getDefaultLightingModelName() << "\n";

					if ( !options.globalSamplersFile.empty() )
					{
						stream << cuT( "	include \"" ) << options.globalSamplersFile << cuT( "\"\n" );
					}

					if ( !options.globalMaterialsFile.empty() )
					{
						stream << "include \"" << options.globalMaterialsFile << cuT( "\"\n" );
					}

					if ( !options.globalFontsFile.empty() )
					{
						stream << "include \"" << options.globalFontsFile << cuT( "\"\n" );
					}

					if ( !options.globalThemesFile.empty() )
					{
						stream << "include \"" << options.globalThemesFile << cuT( "\"\n" );
					}

					if ( !options.globalStylesFile.empty() )
					{
						stream << "include \"" << options.globalStylesFile << cuT( "\"\n" );
					}

					if ( !options.globalControlsFile.empty() )
					{
						stream << "include \"" << options.globalControlsFile << cuT( "\"\n" );
					}

					stream << cuT( "\n" );
					stream << cuT( "scene \"" ) << name << cuT( "\"\n" );
					stream << cuT( "{\n" );
					stream << cuT( "	// Scene configuration\n" );
					stream << cuT( "	ambient_light 1.0 1.0 1.0\n" );
					stream << cuT( "	background_colour 0.50000 0.50000 0.50000\n" );

					if ( !options.sceneSamplersFile.empty() )
					{
						stream << cuT( "	include \"" ) << options.sceneSamplersFile << cuT( "\"\n" );
					}

					if ( !options.sceneMaterialsFile.empty() )
					{
						stream << cuT( "	include \"" ) << options.sceneMaterialsFile << cuT( "\"\n" );
					}

					if ( !options.sceneFontsFile.empty() )
					{
						stream << cuT( "	include \"" ) << options.sceneFontsFile << cuT( "\"\n" );
					}

					if ( !options.sceneThemesFile.empty() )
					{
						stream << cuT( "	include \"" ) << options.sceneThemesFile << cuT( "\"\n" );
					}

					if ( !options.sceneStylesFile.empty() )
					{
						stream << cuT( "	include \"" ) << options.sceneStylesFile << cuT( "\"\n" );
					}

					if ( !options.sceneControlsFile.empty() )
					{
						stream << cuT( "	include \"" ) << options.sceneControlsFile << cuT( "\"\n" );
					}

					if ( !skl.empty() )
					{
						stream << cuT( "	include \"Helpers/" ) << name << cuT( "-Skeletons.cscn\"\n" );
					}

					if ( !msh.empty() )
					{
						stream << cuT( "	include \"Helpers/" ) << name << cuT( "-Meshes.cscn\"\n" );
					}

					if ( !nod.empty() )
					{
						stream << cuT( "	include \"Helpers/" ) << name << cuT( "-Nodes.cscn\"\n" );
					}

					if ( !nod.empty() )
					{
						stream << cuT( "	include \"Helpers/" ) << name << cuT( "-Objects.cscn\"\n" );
					}

					stream << cuT( "\n" );
					stream << cuT( "\n" );
					stream << cuT( "	//Cameras nodes\n" );
					stream << cuT( "\n" );
					stream << cuT( "	scene_node \"MainCameraNode\"\n" );
					stream << cuT( "	{\n" );
					float farPlane = 0.0f;
					stream << cuT( "		position " ) << getCameraPosition( *singleMesh, farPlane ) << cuT( "\n" );
					stream << cuT( "	}\n" );
					stream << cuT( "\n" );
					stream << cuT( "	//Cameras\n" );
					stream << cuT( "\n" );
					stream << cuT( "	camera \"MainCamera\"\n" );
					stream << cuT( "	{\n" );
					stream << cuT( "		parent \"MainCameraNode\"\n" );
					stream << cuT( "\n" );
					stream << cuT( "		viewport\n" );
					stream << cuT( "		{\n" );
					stream << cuT( "			type perspective\n" );
					stream << cuT( "			near 0.100000\n" );
					stream << cuT( "			far " ) << farPlane << cuT( "\n" );
					stream << cuT( "			aspect_ratio 1.77800\n" );
					stream << cuT( "			fov_y 45.0000\n" );
					stream << cuT( "		}\n" );
					stream << cuT( "\n" );
					stream << cuT( "		hdr_config\n" );
					stream << cuT( "		{\n" );
					stream << cuT( "			exposure 1.00000\n" );
					stream << cuT( "			gamma 2.20000\n" );
					stream << cuT( "		}\n" );
					stream << cuT( "	}\n" );
					stream << cuT( "\n" );
					stream << cuT( "	scene_node \"LightNode\"\n" );
					stream << cuT( "	{\n" );
					stream << cuT( "		orientation 1 0 0 90\n" );
					stream << cuT( "	}\n" );
					stream << cuT( "\n" );
					stream << cuT( "	light \"SunLight\"\n" );
					stream << cuT( "	{\n" );
					stream << cuT( "		parent \"LightNode\"\n" );
					stream << cuT( "		type directional\n" );
					stream << cuT( "		colour 1.00000 1.00000 1.00000\n" );
					stream << cuT( "		intensity 8.0 10.0\n" );
					stream << cuT( "	}\n" );
					stream << cuT( "}\n" );
					printRenderWindow( name, cuT( "MainCamera" ), stream );
				}
				else
				{
					result = TextWriter< Scene >{ cuEmptyString, options }( scene, stream );

					if ( scene.getEngine()->getRenderWindows().empty()
						&& !scene.getCameraCache().isEmpty() )
					{
						auto camera = scene.getCameraCache().begin()->second.get();
						printRenderWindow( scene.getName()
							, camera->getName()
							, stream );
					}
				}

				if ( carryOn( result, exportOptions ) )
				{
					TextFile scnFile{ Path{ filePath }
						, File::OpenMode::eWrite };
					result = scnFile.writeText( stream.str() ) > 0;
				}
			}

			return result;
		}
	}

	//*********************************************************************************************

	CscnSceneExporter::CscnSceneExporter( ExportOptions options )
		: SceneExporter{ c3d::move( options ) }
	{
	}

	bool CscnSceneExporter::exportMesh( Scene const & scene
		, Mesh const & mesh
		, Path const & outputFolder
		, String const & outputName )
	{
		Path folder;
		Path filePath;
		Path skeletonFolder;
		Path meshFolder;
		auto options = prepareExport( m_options
			, outputFolder
			, folder
			, filePath
			, skeletonFolder
			, meshFolder );
		bool result = writeSamplers( m_options.ignoreFailures
			, folder
			, filePath
			, scene
			, options );

		if ( carryOn( result ) )
		{
			result = writeMaterials( m_options.ignoreFailures
				, folder
				, filePath
				, scene
				, options );
		}

		if ( carryOn( result ) )
		{
			StringStream skeletons;
			StringStream meshes;
			StringStream nodes;
			StringStream objects;
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

				if ( carryOn( result ) )
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

				if ( carryOn( result ) )
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

			if ( carryOn( result ) )
			{
				result = finaliseExport( m_options
					, &mesh
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

	bool CscnSceneExporter::exportScene( Scene const & scene
		, Path const & fileName )
	{
		Path folder;
		Path filePath;
		Path skeletonFolder;
		Path meshFolder;
		auto options = prepareExport( m_options
			, fileName
			, folder
			, filePath
			, skeletonFolder
			, meshFolder );
		bool result = writeTextures( m_options.ignoreFailures
			, folder
			, filePath
			, scene
			, options );

		if ( carryOn( result ) )
		{
			result = writeSamplers( m_options.ignoreFailures
				, folder
				, filePath
				, scene
				, options );
		}

		if ( carryOn( result ) )
		{
			result = writeMaterials( m_options.ignoreFailures
				, folder
				, filePath
				, scene
				, options );
		}

		if ( carryOn( result ) )
		{
			result = writeFonts( m_options.ignoreFailures
				, folder
				, filePath
				, scene
				, options );
		}

		if ( carryOn( result ) )
		{
			result = writeGuiThemes( m_options.ignoreFailures
				, folder
				, filePath
				, scene
				, options );
		}

		if ( carryOn( result ) )
		{
			result = writeGuiStyles( m_options.ignoreFailures
				, folder
				, filePath
				, scene
				, options );
		}

		if ( carryOn( result ) )
		{
			result = writeGuiControls( m_options.ignoreFailures
				, folder
				, filePath
				, scene
				, options );
		}

		if ( carryOn( result ) )
		{
			result = writeLights( m_options.ignoreFailures
				, folder
				, filePath
				, scene
				, options );

			StringStream skeletons;
			StringStream meshes;
			StringStream nodes;
			StringStream objects;

			if ( carryOn( result ) )
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
				auto lock( makeUniqueLock( scene.getMeshCache() ) );

				for ( auto const & [name, skeleton] : scene.getSkeletonCache() )
				{
					if ( carryOn( result ) )
					{
						result = writeObjectT< true >( SkeletonWriterOptions{ m_options
								, *skeleton
								, scene.getGeometryCache()
								, skeletons
								, meshes
								, nodes
								, objects
								, skeletonFolder
								, name
								, options.subfolder
								, name
								, false }
							, { nullptr, nullptr } );
					}
				}

				for ( auto const & [name, mesh] : scene.getMeshCache() )
				{
					if ( carryOn( result ) && mesh->isSerialisable() )
					{
						result = writeObjectT< true >( MeshWriterOptions{ m_options
								, *mesh
								, scene.getGeometryCache()
								, skeletons
								, meshes
								, nodes
								, objects
								, meshFolder
								, name
								, options.subfolder
								, name
								, false }
							, { nullptr, nullptr } );
					}
				}
			}
			else
			{
				result = unsplitted::writeSkeletons( m_options.ignoreFailures
					, folder
					, filePath
					, scene
					, options
					, skeletons );

				if ( carryOn( result ) )
				{
					result = unsplitted::writeMeshes( m_options.ignoreFailures
						, folder
						, filePath
						, scene
						, options
						, meshes );
				}

				if ( carryOn( result ) )
				{
					result = unsplitted::writeObjects( m_options.ignoreFailures
						, folder
						, filePath
						, scene
						, options
						, objects );
				}

				if ( result )
				{
					auto lock( makeUniqueLock( scene.getMeshCache() ) );

					for ( auto const & [name, skeleton] : scene.getSkeletonCache() )
					{
						if ( carryOn( result ) )
						{
							result = writeObjectT< false >( SkeletonWriterOptions{ m_options
									, *skeleton
									, scene.getGeometryCache()
									, skeletons
									, meshes
									, nodes
									, objects
									, skeletonFolder
									, name
									, options.subfolder
									, name
									, false }
								, { nullptr, nullptr } );
						}
					}

					for ( auto const & [name, mesh] : scene.getMeshCache() )
					{
						if ( carryOn( result ) && mesh->isSerialisable() )
						{
							result = writeObjectT< false >( MeshWriterOptions{ m_options
									, *mesh
									, scene.getGeometryCache()
									, skeletons
									, meshes
									, nodes
									, objects
									, meshFolder
									, name
									, options.subfolder
									, name
									, false }
								, { nullptr, nullptr } );
						}
					}
				}
			}

			if ( carryOn( result ) )
			{
				result = finaliseExport( m_options
					, nullptr
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

	bool CscnSceneExporter::carryOn( bool result )const noexcept
	{
		return result || m_options.ignoreFailures;
	}
}

