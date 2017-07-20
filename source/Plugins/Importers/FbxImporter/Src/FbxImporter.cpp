#include "FbxImporter.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Cache/GeometryCache.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/PluginCache.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/SceneNodeCache.hpp>

#include <Animation/Skeleton/SkeletonAnimation.hpp>
#include <Animation/Skeleton/SkeletonAnimationBone.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Cache/CacheView.hpp>
#include <Material/LegacyPass.hpp>
#include <Mesh/Skeleton/Bone.hpp>
#include <Plugin/ImporterPlugin.hpp>
#include <Texture/Sampler.hpp>

#include <Design/BlockGuard.hpp>
#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

#define DEBUG_NODES_TRAVERSAL 1

namespace C3dFbx
{
	namespace
	{
		void DoInitializeSdkObjects( FbxManager *& p_fbxManager
			, FbxScene *& p_fbxScene )
		{
			//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
			p_fbxManager = FbxManager::Create();

			if ( !p_fbxManager )
			{
				CASTOR_EXCEPTION( cuT( "Error: Unable to create FBX Manager!" ) );
			}

			Logger::LogDebug( StringStream() << "Autodesk FBX SDK version " << p_fbxManager->GetVersion() << std::endl );

			//Create an IOSettings object. This object holds all import/export settings.
			FbxIOSettings * ios = FbxIOSettings::Create( p_fbxManager, IOSROOT );
			p_fbxManager->SetIOSettings( ios );

			//Load plug-ins from the executable directory (optional)
			FbxString lPath = FbxGetApplicationDirectory();
			p_fbxManager->LoadPluginsDirectory( lPath.Buffer() );

			//Create an FBX scene. This object holds most objects imported/exported from/to files.
			p_fbxScene = FbxScene::Create( p_fbxManager, "My Scene" );

			if ( !p_fbxScene )
			{
				CASTOR_EXCEPTION( cuT( "Error: Unable to create FBX scene!" ) );
			}
		}

		void DoDestroySdkObjects( FbxManager * p_fbxManager )
		{
			//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
			if ( p_fbxManager )
			{
				p_fbxManager->Destroy();
			}
		}

		static int const FBX_AUTODETECT_FILE_FORMAT = -1;

		bool DoLoadScene( FbxManager * p_fbxManager
			, FbxDocument * p_fbxScene
			, Path const & p_fileName )
		{
			int lFileMajor, lFileMinor, lFileRevision;
			int lSDKMajor, lSDKMinor, lSDKRevision;
			std::string fileName = string::string_cast< char >( p_fileName );

			// Get the file version number generate by the FBX SDK.
			FbxManager::GetFileFormatVersion( lSDKMajor, lSDKMinor, lSDKRevision );

			// Create an importer.
			FbxImporter * fbxImporter = nullptr;
			auto guard = make_block_guard( [&fbxImporter, &p_fbxManager]()
				{
					fbxImporter = FbxImporter::Create( p_fbxManager, "" );
				},
				[&fbxImporter]()
				{
					// Destroy the importer.
					fbxImporter->Destroy();
				} );

			// Initialize the importer by providing a filename.
			const bool lImportStatus = fbxImporter->Initialize( fileName.c_str()
				, FBX_AUTODETECT_FILE_FORMAT
				, p_fbxManager->GetIOSettings() );
			fbxImporter->GetFileVersion( lFileMajor
				, lFileMinor
				, lFileRevision );

			if ( !lImportStatus )
			{
				FbxString error = fbxImporter->GetStatus().GetErrorString();
				StringStream stream;
				stream << "Call to FbxImporter::Initialize() failed.\n";
				stream << "Error returned: " << error.Buffer() << "\n";

				if ( fbxImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion )
				{
					stream << "FBX file format version for this FBX SDK is " << lSDKMajor << "." << lSDKMinor << "." << lSDKRevision << "\n";
					stream << "FBX file format version for file " << fileName << " is " << lFileMajor << "." << lFileMinor << "." << lFileRevision << "\n";
				}

				CASTOR_EXCEPTION( stream.str() );
			}

			Logger::LogDebug( std::stringstream() << "FBX file format version for this FBX SDK is " << lSDKMajor << "." << lSDKMinor << "." << lSDKRevision );

			if ( fbxImporter->IsFBX() )
			{
				Logger::LogDebug( std::stringstream() << "FBX file format version for file " << fileName << " is " << lFileMajor << "." << lFileMinor << "." << lFileRevision << "\n" );

				// From this point, it is possible to access animation stack information without
				// the expense of loading the entire file.
				Logger::LogDebug( "Animation Stack Information" );

				int lAnimStackCount = fbxImporter->GetAnimStackCount();

				Logger::LogDebug( std::stringstream() << "    Number of Animation Stacks: " << lAnimStackCount );
				Logger::LogDebug( std::stringstream() << "    Current Animation Stack: \"" << fbxImporter->GetActiveAnimStackName().Buffer() << "\"\n" );

				for ( int i = 0; i < lAnimStackCount; i++ )
				{
					FbxTakeInfo * lTakeInfo = fbxImporter->GetTakeInfo( i );

					Logger::LogDebug( std::stringstream() << "    Animation Stack " << i );
					Logger::LogDebug( std::stringstream() << "         Name: \"" << lTakeInfo->mName.Buffer() << "\"" );
					Logger::LogDebug( std::stringstream() << "         Description: \"" << lTakeInfo->mDescription.Buffer() << "\"" );

					// Change the value of the import name if the animation stack should be imported
					// under a different name.
					Logger::LogDebug( std::stringstream() << "         Import Name: \"" << lTakeInfo->mImportName.Buffer() << "\"" );

					// Set the value of the import state to false if the animation stack should be not
					// be imported.
					Logger::LogDebug( std::stringstream() << "         Import State: " << ( lTakeInfo->mSelect ? "true" : "false" ) << "\n" );
				}

				// Set the import states. By default, the import states are always set to
				// true. The code below shows how to change these states.
				auto & ioSettings = *p_fbxManager->GetIOSettings();
				ioSettings.SetBoolProp( IMP_FBX_MATERIAL, true );
				ioSettings.SetBoolProp( IMP_FBX_TEXTURE, true );
				ioSettings.SetBoolProp( IMP_FBX_LINK, true );
				ioSettings.SetBoolProp( IMP_FBX_SHAPE, true );
				ioSettings.SetBoolProp( IMP_FBX_GOBO, true );
				ioSettings.SetBoolProp( IMP_FBX_ANIMATION, true );
				ioSettings.SetBoolProp( IMP_FBX_GLOBAL_SETTINGS, true );
			}

			// Import the scene.
			auto status = fbxImporter->Import( p_fbxScene );

			if ( !status && fbxImporter->GetStatus().GetCode() == FbxStatus::ePasswordError )
			{
				Logger::LogInfo( "Please enter password: " );

				std::string password;

				FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
				std::cin >> password;
				FBXSDK_CRT_SECURE_NO_WARNING_END

				FbxString string( password.c_str() );

				p_fbxManager->GetIOSettings()->SetStringProp( IMP_FBX_PASSWORD, string );
				p_fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_PASSWORD_ENABLE, true );

				status = fbxImporter->Import( p_fbxScene );

				if ( !status && fbxImporter->GetStatus().GetCode() == FbxStatus::ePasswordError )
				{
					CASTOR_EXCEPTION( cuT( "\nPassword is wrong, import aborted.\n" ) );
				}
			}

			return status;
		}

		inline uint32_t DoGetIndex( FbxMesh * p_mesh
			, uint32_t p_poly
			, uint32_t p_vertex )
		{
			uint32_t result = p_mesh->GetPolygonVertex( p_poly, p_vertex );
			assert( result != 0xFFFFFFFF && "Index is out of range" );
			return result;
		}

		inline FaceIndices DoGetFace( FbxMesh * p_fbxMesh
			, uint32_t p_poly
			, uint32_t a
			, uint32_t b
			, uint32_t c )
		{
			return
			{
				{
					DoGetIndex( p_fbxMesh, p_poly, a ),
					DoGetIndex( p_fbxMesh, p_poly, b ),
					DoGetIndex( p_fbxMesh, p_poly, c ),
				}
			};
		}

		template< typename T >
		inline void DoFillBufferFromValue( T const & p_value, real *& p_buffer )
		{
			CASTOR_EXCEPTION( "Missing buffer filler from FBX value" );
		}

		template<>
		inline void DoFillBufferFromValue< FbxVector4 >( FbxVector4 const & p_value
			, real *& p_buffer )
		{
			*p_buffer++ = real( p_value[0] );
			*p_buffer++ = real( p_value[1] );
			*p_buffer++ = real( p_value[2] );
		}

		template<>
		inline void DoFillBufferFromValue< FbxVector2 >( FbxVector2 const & p_value
			, real *& p_buffer )
		{
			*p_buffer++ = real( p_value[0] );
			*p_buffer++ = real( p_value[1] );
			p_buffer++;
		}

		template< typename T >
		inline bool DoRetrieveMeshValues( String const & p_name
			, FbxMesh * p_mesh
			, FbxLayerElementTemplate< T > * p_fbxValues
			, std::vector< real > & p_values )
		{
			bool result = false;

			if ( p_fbxValues )
			{
				uint32_t count = p_fbxValues->GetDirectArray().GetCount();
				bool useIndex = p_fbxValues->GetReferenceMode() == FbxLayerElement::eIndexToDirect
					|| p_fbxValues->GetReferenceMode() == FbxLayerElement::eIndex;

				switch ( p_fbxValues->GetMappingMode() )
				{
				case FbxLayerElement::eByControlPoint:
					if ( useIndex )
					{
						auto buffer = p_values.data();

						for ( auto i = 0u; i < count; ++i )
						{
							DoFillBufferFromValue( p_fbxValues->GetDirectArray().GetAt( p_fbxValues->GetIndexArray().GetAt( i ) )
								, buffer );
						}
					}
					else
					{
						auto buffer = p_values.data();

						for ( auto i = 0u; i < count; ++i )
						{
							DoFillBufferFromValue( p_fbxValues->GetDirectArray().GetAt( i )
								, buffer );
						}
					}

					break;

				case FbxLayerElement::eByPolygonVertex:
					if ( useIndex )
					{
						uint32_t vertexIndex = 0;

						for ( int i = 0; i < p_mesh->GetPolygonCount(); ++i )
						{
							for ( int j = 0; j < p_mesh->GetPolygonSize( i ); ++j )
							{
								int cpIndex = p_mesh->GetPolygonVertex( i, j );
								auto buffer = p_values.data() + ( cpIndex * 3 );
								DoFillBufferFromValue( p_fbxValues->GetDirectArray().GetAt( p_fbxValues->GetIndexArray().GetAt( vertexIndex ) )
									, buffer );
								++vertexIndex;
							}
						}
					}
					else
					{
						uint32_t vertexIndex = 0;

						for ( int i = 0; i < p_mesh->GetPolygonCount(); ++i )
						{
							for ( int j = 0; j < p_mesh->GetPolygonSize( i ); ++j )
							{
								int cpIndex = p_mesh->GetPolygonVertex( i, j );
								auto buffer = p_values.data() + ( cpIndex * 3 );
								DoFillBufferFromValue( p_fbxValues->GetDirectArray().GetAt( vertexIndex )
									, buffer );
								++vertexIndex;
							}
						}
					}

					break;
				}

				result = true;
			}
			else
			{
				Logger::LogWarning( StringStream() << cuT( "No " ) << p_name << cuT( " for this mesh." ) );
			}

			return result;
		}

		FbxAMatrix DoGetGeometryTransformation( FbxNode * p_fbxNode )
		{
			if ( !p_fbxNode )
			{
				CASTOR_EXCEPTION( "Null for mesh geometry" );
			}

			const FbxVector4 translate = p_fbxNode->GetGeometricTranslation( FbxNode::eSourcePivot );
			const FbxVector4 rotate = p_fbxNode->GetGeometricRotation( FbxNode::eSourcePivot );
			const FbxVector4 scale = p_fbxNode->GetGeometricScaling( FbxNode::eSourcePivot );

			return FbxAMatrix( translate, rotate, scale );
		}

		uint32_t DoFindBoneIndex( String const & p_name
			, Skeleton const & p_skeleton )
		{
			auto it = std::find_if( p_skeleton.begin()
				, p_skeleton.end()
				, [&]( BoneSPtr p_bone )
				{
					return p_bone->GetName() == p_name;
				} );

			uint32_t result = 0xFFFFFFFF;

			if ( it != p_skeleton.end() )
			{
				result = uint32_t( std::distance( p_skeleton.begin(), it ) );
			}
			else
			{
				CASTOR_EXCEPTION( "Skeleton information in FBX file is corrupted" );
			}

			return result;
		}

		void PrintNode( FbxNode const & p_node )
		{
			StringStream stream;
			stream << "Node " << p_node.GetName();

			if ( p_node.GetNodeAttribute() )
			{
				static String const TypeName[]
				{
					cuT( "Unknown" ),
					cuT( "Null" ),
					cuT( "Marker" ),
					cuT( "Skeleton" ),
					cuT( "Mesh" ),
					cuT( "Nurbs" ),
					cuT( "Patch" ),
					cuT( "Camera" ),
					cuT( "CameraStereo" ),
					cuT( "CameraSwitcher" ),
					cuT( "Light" ),
					cuT( "OpticalReference" ),
					cuT( "OpticalMarker" ),
					cuT( "NurbsCurve" ),
					cuT( "TrimNurbsSurface" ),
					cuT( "Boundary" ),
					cuT( "NurbsSurface" ),
					cuT( "Shape" ),
					cuT( "LODGroup" ),
					cuT( "SubDiv" ),
					cuT( "CachedEffect" ),
					cuT( "Line" )
				};

				stream << ", type " << TypeName[p_node.GetNodeAttribute()->GetAttributeType()];
			}

			Logger::LogDebug( stream );
		}

		template< typename FbxType >
		using TraverseFunction = std::function< void( FbxNode *, FbxType * ) >;

		template< typename FbxType >
		void DoTraverseDepthFirst( FbxNode * p_fbxNode
			, FbxNodeAttribute::EType p_type
			, TraverseFunction< FbxType > p_function )
		{
			if ( p_fbxNode )
			{
#if DEBUG_NODES_TRAVERSAL
				PrintNode( *p_fbxNode );
#endif
				if ( p_fbxNode->GetNodeAttribute() )
				{
					auto type = p_fbxNode->GetNodeAttribute()->GetAttributeType();

					if ( type == p_type )
					{
						p_function( p_fbxNode, reinterpret_cast< FbxType * >( p_fbxNode->GetNodeAttribute() ) );
					}
				}

				for ( int i = 0; i < p_fbxNode->GetChildCount(); ++i )
				{
					DoTraverseDepthFirst( p_fbxNode->GetChild( i ), p_type, p_function );
				}
			}
		}

		Point3r MakePoint3r( FbxDouble3 const p_point )
		{
			return Point3r{ p_point[0], p_point[1], p_point[2] };
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & Rotate( SquareMatrix< T, 4 > & p_matrix, QuaternionT< U > const & p_quat )
		{
			SquareMatrix< T, 4 > rotate;
			auto const qxx( p_quat.quat.x * p_quat.quat.x );
			auto const qyy( p_quat.quat.y * p_quat.quat.y );
			auto const qzz( p_quat.quat.z * p_quat.quat.z );
			auto const qxz( p_quat.quat.x * p_quat.quat.z );
			auto const qxy( p_quat.quat.x * p_quat.quat.y );
			auto const qyz( p_quat.quat.y * p_quat.quat.z );
			auto const qwx( p_quat.quat.w * p_quat.quat.x );
			auto const qwy( p_quat.quat.w * p_quat.quat.y );
			auto const qwz( p_quat.quat.w * p_quat.quat.z );

			rotate[0][0] = T( 1 - 2 * ( qyy + qzz ) );
			rotate[0][1] = T( 2 * ( qxy - qwz ) );
			rotate[0][2] = T( 2 * ( qxz + qwy ) );
			rotate[0][3] = T( 0 );

			rotate[1][0] = T( 2 * ( qxy + qwz ) );
			rotate[1][1] = T( 1 - 2 * ( qxx + qzz ) );
			rotate[1][2] = T( 2 * ( qyz - qwx ) );
			rotate[1][3] = T( 0 );

			rotate[2][0] = T( 2 * ( qxz - qwy ) );
			rotate[2][1] = T( 2 * ( qyz + qwx ) );
			rotate[2][2] = T( 1 - 2 * ( qxx + qyy ) );
			rotate[3][3] = T( 0 );

			rotate[3][0] = T( 0 );
			rotate[3][1] = T( 0 );
			rotate[3][2] = T( 0 );
			rotate[3][3] = T( 1 );

			return p_matrix *= rotate;
		}
	}

	FbxSdkImporter::FbxSdkImporter( Engine & p_engine )
		: Importer( p_engine )
		, m_anonymous( 0 )
	{
	}

	FbxSdkImporter::~FbxSdkImporter()
	{
	}

	ImporterUPtr FbxSdkImporter::Create( Engine & p_engine )
	{
		return std::make_unique< FbxSdkImporter >( p_engine );
	}

	bool FbxSdkImporter::DoImportScene( Scene & p_scene )
	{
		FbxManager * fbxManager = nullptr;
		FbxScene * fbxScene = nullptr;

		// Prepare the FBX SDK.
		DoInitializeSdkObjects( fbxManager, fbxScene );

		// TODO

		DoDestroySdkObjects( fbxManager );

		return false;
	}

	bool FbxSdkImporter::DoImportMesh( Mesh & p_mesh )
	{
		bool result{ false };
		m_mapBoneByID.clear();
		m_arrayBones.clear();
		FbxManager * fbxManager = nullptr;
		FbxScene * fbxScene = nullptr;

		// Prepare the FBX SDK.
		DoInitializeSdkObjects( fbxManager, fbxScene );

		if ( fbxScene )
		{
			if ( DoLoadScene( fbxManager, fbxScene, m_fileName ) )
			{
				FbxGeometryConverter converter( fbxManager );
				converter.SplitMeshesPerMaterial( fbxScene, true );

				if ( converter.Triangulate( fbxScene, true ) )
				{
					DoLoadMaterials( *p_mesh.GetScene(), fbxScene );
					DoLoadSkeleton( p_mesh, fbxScene->GetRootNode() );
					DoLoadMeshes( p_mesh, *p_mesh.GetScene(), fbxScene->GetRootNode() );

					if ( p_mesh.GetSkeleton() )
					{
						DoLoadAnimations( p_mesh, fbxScene );
					}

					result = true;
				}
			}
		}

		DoDestroySdkObjects( fbxManager );
		return result;
	}

	void FbxSdkImporter::DoLoadMeshes( Mesh & p_mesh, Scene & p_scene, FbxNode * p_fbxNode )
	{
		DoTraverseDepthFirst< FbxMesh >( p_fbxNode, FbxNodeAttribute::eMesh, [this, &p_scene, &p_mesh]( FbxNode * p_fbxNode, FbxMesh * p_fbxMesh )
		{
#if DEBUG_NODES_TRAVERSAL
			Logger::LogDebug( StringStream{} << cuT( "  Mesh " ) << p_fbxMesh->GetName() );
#endif
			Point3r translate = Point3r{ ( p_fbxNode->EvaluateLocalTranslation() ).Buffer() };
			Quaternion rotate = Quaternion{ ( p_fbxNode->EvaluateLocalRotation() ).Buffer() };
			Point3r scale = Point3r{ ( p_fbxNode->EvaluateLocalScaling() ).Buffer() };
			Matrix4x4r transform{ 1.0_r };
			matrix::translate( transform, translate );
			Rotate( transform, rotate );
			matrix::scale( transform, scale );
			SubmeshSPtr submesh = DoProcessMesh( p_mesh
				, p_fbxMesh
				, transform );
			auto material = p_fbxNode->GetMaterial( 0 );

			if ( material )
			{
				submesh->SetDefaultMaterial( p_scene.GetMaterialView().Find( material->GetName() ) );
			}

			if ( p_mesh.GetSkeleton() )
			{
				FbxMesh * mesh = p_fbxNode->GetMesh();
				std::vector< VertexBoneData > boneData{ submesh->GetPointsCount() };
				DoProcessBonesWeights( p_fbxNode, *p_mesh.GetSkeleton(), boneData );
				submesh->AddBoneDatas( boneData );
			}
		} );
	}

	void FbxSdkImporter::DoLoadSkeleton( Mesh & p_mesh, FbxNode * p_fbxNode )
	{
		SkeletonSPtr skeleton = std::make_shared< Skeleton >( *p_mesh.GetScene() );

		for ( int i = 0; i < p_fbxNode->GetChildCount(); ++i )
		{
			DoProcessBones( p_fbxNode->GetChild( i ), *skeleton, nullptr );
		}

		if ( std::distance( skeleton->begin(), skeleton->end() ) > 0 )
		{
			p_mesh.SetSkeleton( skeleton );
		}
	}

	void FbxSdkImporter::DoLoadAnimations( Mesh & p_mesh, FbxScene * p_scene )
	{
		DoTraverseDepthFirst< FbxMesh >( p_scene->GetRootNode(), FbxNodeAttribute::eMesh, [this, &p_scene, &p_mesh]( FbxNode * p_fbxNode, FbxMesh * p_fbxMesh )
		{
			FbxMesh * mesh = p_fbxNode->GetMesh();
			uint32_t deformersCount = mesh->GetDeformerCount();

			for ( uint32_t deformerIndex = 0; deformerIndex < deformersCount; ++deformerIndex )
			{
				FbxSkin * skin = reinterpret_cast< FbxSkin * >( mesh->GetDeformer( deformerIndex, FbxDeformer::eSkin ) );

				if ( skin )
				{
					DoProcessSkeletonAnimations( p_scene, p_fbxNode, skin, *p_mesh.GetSkeleton() );
				}
			}
		} );

		for ( auto const & it : p_mesh.GetSkeleton()->GetAnimations() )
		{
			Logger::LogDebug( StringStream() << cuT( "Found Animation: " ) << it.first );
		}
	}

	void FbxSdkImporter::DoProcessSkeletonAnimations( FbxScene * p_fbxScene, FbxNode * p_fbxNode, FbxSkin * p_fbxSkin, Skeleton & p_skeleton )
	{
		uint32_t clustersCount = p_fbxSkin->GetClusterCount();
		const Point3r geoTranslate{ p_fbxNode->GetGeometricTranslation( FbxNode::eSourcePivot ).Buffer() };
		const Quaternion geoRotate{ p_fbxNode->GetGeometricRotation( FbxNode::eSourcePivot ).Buffer() };
		const Point3r geoScale{ p_fbxNode->GetGeometricScaling( FbxNode::eSourcePivot ).Buffer() };

		for ( uint32_t clusterIndex = 0; clusterIndex < clustersCount; ++clusterIndex )
		{
			FbxCluster * cluster = p_fbxSkin->GetCluster( clusterIndex );
			auto link = cluster->GetLink();
			uint32_t animationsCount = p_fbxScene->GetSrcObjectCount( FbxCriteria::ObjectType( FbxAnimStack::ClassId ) );
			auto bone = *( p_skeleton.begin() + DoFindBoneIndex( string::string_cast< xchar >( link->GetName() ), p_skeleton ) );

			for ( uint32_t animationIndex = 0; animationIndex < animationsCount; ++animationIndex )
			{
				FbxAnimStack * animStack = p_fbxScene->GetSrcObject< FbxAnimStack >( animationIndex );
				auto name = animStack->GetName();
				auto & animation = p_skeleton.CreateAnimation( string::string_cast< xchar >( name ) );
				FbxTakeInfo * takeInfo = p_fbxScene->GetTakeInfo( name );
				uint64_t start = takeInfo->mLocalTimeSpan.GetStart().GetFrameCount( FbxTime::eFrames24 );
				uint64_t finish = takeInfo->mLocalTimeSpan.GetStop().GetFrameCount( FbxTime::eFrames24 );
				uint64_t animationLength = finish - start + 1;
				SkeletonAnimationObjectSPtr object;

				if ( bone->GetParent() )
				{
					object = animation.AddObject( bone, animation.GetObject( *bone->GetParent() ) );
				}
				else
				{
					object = animation.AddObject( bone, nullptr );
				}

				if ( object )
				{
					auto inc = std::chrono::milliseconds{ 1000 / 24 };
					auto from = 0_ms;

					for ( uint64_t i = start; i <= finish; ++i )
					{
						FbxTime time;
						time.SetFrame( i, FbxTime::eFrames24 );
						Point3r translate = geoTranslate
											  + Point3r{ ( p_fbxNode->EvaluateLocalTranslation( time ) ).Buffer() }
											  + Point3r{ ( link->EvaluateLocalTranslation( time ) ).Buffer() };
						Quaternion rotate = geoRotate
											  * Quaternion{ ( p_fbxNode->EvaluateLocalRotation( time ) ).Buffer() }
											  * Quaternion{ ( link->EvaluateLocalRotation( time ) ).Buffer() };
						Point3r scale = geoScale
										  * Point3r{ ( p_fbxNode->EvaluateLocalScaling( time ) ).Buffer() }
										  * Point3r{ ( link->EvaluateLocalScaling( time ) ).Buffer() };
						object->AddKeyFrame( from, translate, rotate, scale );
						from += inc;
					}
				}

				animation.UpdateLength();
			}
		}
	}

	void FbxSdkImporter::DoProcessBones( FbxNode * p_fbxNode, Skeleton & p_skeleton, BoneSPtr p_parent )
	{
		auto bone = p_parent;

		if ( p_fbxNode->GetNodeAttribute() && p_fbxNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton )
		{
			FbxSkeleton * skeleton = reinterpret_cast< FbxSkeleton * >( p_fbxNode );
			String name = string::string_cast< xchar >( skeleton->GetName() );

			if ( m_mapBoneByID.find( name ) == m_mapBoneByID.end() )
			{
				bone = p_skeleton.CreateBone( name );
				m_arrayBones.push_back( bone );
				m_mapBoneByID[name] = uint32_t( m_mapBoneByID.size() );

				if ( p_parent )
				{
					p_skeleton.SetBoneParent( bone, p_parent );
				}
			}
			else
			{
				bone = m_arrayBones[m_mapBoneByID[name]];
			}
		}

		for ( int i = 0; i < p_fbxNode->GetChildCount(); i++ )
		{
			DoProcessBones( p_fbxNode->GetChild( i ), p_skeleton, bone );
		}
	}

	void FbxSdkImporter::DoProcessBonesWeights( FbxNode * p_fbxNode, Skeleton const & p_skeleton, std::vector< VertexBoneData > & p_boneData )
	{
		FbxMesh * mesh = p_fbxNode->GetMesh();
		uint32_t deformersCount = mesh->GetDeformerCount();
		FbxAMatrix geometryTransform = DoGetGeometryTransformation( p_fbxNode );

		for ( uint32_t deformerIndex = 0; deformerIndex < deformersCount; ++deformerIndex )
		{
			FbxSkin * skin = reinterpret_cast< FbxSkin * >( mesh->GetDeformer( deformerIndex, FbxDeformer::eSkin ) );

			if ( skin )
			{
				uint32_t clustersCount = skin->GetClusterCount();

				for ( uint32_t clusterIndex = 0; clusterIndex < clustersCount; ++clusterIndex )
				{
					FbxCluster * cluster = skin->GetCluster( clusterIndex );
					uint32_t boneIndex = DoFindBoneIndex( string::string_cast< xchar >( cluster->GetLink()->GetName() ), p_skeleton );
					auto bone = *( p_skeleton.begin() + boneIndex );
					// The transformation of the mesh at binding time.
					FbxAMatrix transformMatrix;
					cluster->GetTransformMatrix( transformMatrix );
					// The transformation of the cluster(joint) at binding time from joint space to world space.
					FbxAMatrix transformLinkMatrix;
					cluster->GetTransformLinkMatrix( transformLinkMatrix );
					// Update the information in mSkeleton.
					bone->SetOffsetMatrix( Matrix4x4r{ &( transformLinkMatrix.Inverse() * transformMatrix * geometryTransform ).Buffer()[0][0] } );

					// Associate each joint with the control points it affects.
					uint32_t indicesCount = cluster->GetControlPointIndicesCount();

					for ( uint32_t i = 0; i < indicesCount; ++i )
					{
						p_boneData[cluster->GetControlPointIndices()[i]].AddBoneData( boneIndex, real( cluster->GetControlPointWeights()[i] ) );
					}
				}
			}
		}
	}

	void FbxSdkImporter::DoLoadMaterials( Scene & p_scene, FbxScene * p_fbxScene )
	{
		auto & cache = p_scene.GetMaterialView();
		FbxArray< FbxSurfaceMaterial * > mats;
		p_fbxScene->FillMaterialArray( mats );
		MaterialPtrStrMap materials;

		auto parseLambert = [this, &p_scene]( FbxSurfaceLambert * p_lambert, LegacyPass & p_pass )
		{
			auto lambert = static_cast< FbxSurfaceLambert * >( p_lambert );
			//p_pass.SetAlpha( float( lambert->TransparencyFactor ) );
			FbxDouble3 diffuse = lambert->Diffuse;
			p_pass.SetDiffuse( Colour::from_rgb( { float( diffuse[0] ), float( diffuse[1] ), float( diffuse[2] ) } ) * lambert->DiffuseFactor );
			FbxDouble3 emissive = lambert->Emissive;
			p_pass.SetEmissive( float( point::length( Point3f{ float( emissive[0] ), float( emissive[1] ), float( emissive[2] ) } ) * lambert->EmissiveFactor ) );

			DoLoadTexture( p_scene, lambert->Diffuse, p_pass, TextureChannel::eDiffuse );
			DoLoadTexture( p_scene, lambert->Emissive, p_pass, TextureChannel::eEmissive );
			DoLoadTexture( p_scene, lambert->NormalMap, p_pass, TextureChannel::eNormal );
			DoLoadTexture( p_scene, lambert->TransparentColor, p_pass, TextureChannel::eOpacity );
		};

		for ( int i = 0; i < mats.Size(); ++i )
		{
			FbxSurfaceMaterial * fbxMaterial = mats[i];

			if ( materials.find( fbxMaterial->GetName() ) == materials.end() )
			{
				auto material = cache.Add( fbxMaterial->GetName(), MaterialType::eLegacy );
				REQUIRE( material->GetType() == MaterialType::eLegacy );
				material->CreatePass();
				auto pass = material->GetTypedPass< MaterialType::eLegacy >( 0u );
				Logger::LogDebug( StringStream() << "Material: " << fbxMaterial->GetName() );
				String model = string::string_cast< xchar >( fbxMaterial->ShadingModel.Get().Buffer() );
				Logger::LogDebug( StringStream() << "    ShadingModel: " << model );

				if ( fbxMaterial->GetClassId() == FbxSurfaceLambert::ClassId )
				{
					parseLambert( static_cast< FbxSurfaceLambert * >( fbxMaterial ), *pass );
				}
				else if ( fbxMaterial->GetClassId() == FbxSurfacePhong::ClassId )
				{
					auto phong = static_cast< FbxSurfacePhong * >( fbxMaterial );
					parseLambert( phong, *pass );
					pass->SetShininess( float( phong->Shininess ) );
					FbxDouble3 specular = phong->Specular;
					pass->SetSpecular( Colour::from_rgb( { float( specular[0] ), float( specular[1] ), float( specular[2] ) } ) * phong->SpecularFactor );
					DoLoadTexture( p_scene, phong->Specular, *pass, TextureChannel::eSpecular );
					DoLoadTexture( p_scene, phong->Shininess, *pass, TextureChannel::eGloss );
				}
			}
		}
	}

	SubmeshSPtr FbxSdkImporter::DoProcessMesh( Mesh & p_mesh
		, FbxMesh * p_fbxMesh
		, Matrix4x4r const & p_transform )
	{
		p_fbxMesh->GenerateNormals();
		auto submesh = p_mesh.CreateSubmesh();
		submesh->SetDefaultMaterial( GetEngine()->GetMaterialCache().Find( cuT( "White" ) ) );
		bool tangentSpace = false;

		Logger::LogDebug( StringStream() << "Mesh: " << p_fbxMesh->GetName() );
		Logger::LogDebug( StringStream() << "    Points: " << p_fbxMesh->GetControlPointsCount() );
		Logger::LogDebug( StringStream() << "    Polygons: " << p_fbxMesh->GetPolygonCount() );
		Logger::LogDebug( StringStream() << "    Transform:\n" << p_transform );

		// Vertex
		uint32_t pointsCount = p_fbxMesh->GetControlPointsCount();
		auto layerCount = p_fbxMesh->GetLayerCount();
		std::vector< real > vtx( pointsCount * 3 );
		std::vector< real > nml( layerCount * pointsCount * 3 );
		std::vector< real > tan( layerCount * pointsCount * 3 );
		std::vector< real > bit( layerCount * pointsCount * 3 );
		std::vector< real > tex( layerCount * pointsCount * 3 );
		std::vector< InterleavedVertex > vertices{ pointsCount };
		uint32_t index{ 0u };

		// Positions
		for ( auto & vertex : vertices )
		{
			auto fbxVertex = Point3r{ p_fbxMesh->GetControlPointAt( index++ ).Buffer() };
			fbxVertex = p_transform * fbxVertex;
			vertex.m_pos[0] = real( fbxVertex[0] );
			vertex.m_pos[1] = real( fbxVertex[1] );
			vertex.m_pos[2] = real( fbxVertex[2] );
		}

		for ( auto i = 0; i < layerCount; ++i )
		{
			auto & layer = *p_fbxMesh->GetLayer( i );
			// Normals
			auto normals = layer.GetNormals();

			if ( DoRetrieveMeshValues( cuT( "Normals" ), p_fbxMesh, normals, nml ) )
			{
				index = 0u;
				real * buffer{ nml.data() };

				for ( auto & vertex : vertices )
				{
					std::memcpy( vertex.m_nml.data(), buffer, sizeof( vertex.m_nml ) );
					buffer += 3;
				}
			}

			// Texture UVs
			auto uvs = layer.GetUVs();

			if ( DoRetrieveMeshValues( cuT( "Texture UVs" ), p_fbxMesh, uvs, tex ) )
			{
				index = 0u;
				real * buffer{ tex.data() };

				for ( auto & vertex : vertices )
				{
					std::memcpy( vertex.m_tex.data(), buffer, sizeof( vertex.m_tex ) );
					buffer += 3;
				}
			}

			if ( m_parameters.Get( cuT( "tangent_space" ), tangentSpace )
				&& tangentSpace )
			{
				p_fbxMesh->GenerateTangentsData( 0 );

				// Tangents
				auto tangents = layer.GetTangents();

				if ( DoRetrieveMeshValues( cuT( "Tangents" ), p_fbxMesh, tangents, tan ) )
				{
					index = 0u;
					real * buffer{ tan.data() };

					for ( auto & vertex : vertices )
					{
						std::memcpy( vertex.m_tan.data(), buffer, sizeof( vertex.m_tan ) );
						buffer += 3;
					}
				}

				// Bitangents
				auto bitangents = layer.GetBinormals();

				if ( DoRetrieveMeshValues( cuT( "Bitangents" ), p_fbxMesh, bitangents, bit ) )
				{
					index = 0u;
					real * buffer{ bit.data() };

					for ( auto & vertex : vertices )
					{
						std::memcpy( vertex.m_bin.data(), buffer, sizeof( vertex.m_bin ) );
						buffer += 3;
					}
				}
			}
		}

		// Faces
		uint32_t polyCount = p_fbxMesh->GetPolygonCount();
		std::vector< FaceIndices > faces;
		faces.reserve( polyCount );

		for ( auto poly = 0u; poly < polyCount; ++poly )
		{
			faces.push_back( DoGetFace( p_fbxMesh, poly, 0, 1, 2 ) );
		}

		submesh->AddPoints( vertices );
		submesh->AddFaceGroup( faces );

		if ( !tangentSpace )
		{
			submesh->ComputeTangentsFromNormals();
		}

		return submesh;
	}

	TextureUnitSPtr FbxSdkImporter::DoLoadTexture( FbxFileTexture * p_texture, Pass & p_pass, TextureChannel p_channel )
	{
		return LoadTexture( Path{ string::string_cast< xchar >( p_texture->GetRelativeFileName() ) }.GetFileName( true ), p_pass, p_channel );
	}

	TextureUnitSPtr FbxSdkImporter::DoLoadTexture( FbxLayeredTexture * p_texture, Pass & p_pass, TextureChannel p_channel )
	{
		return nullptr;
	}

	TextureUnitSPtr FbxSdkImporter::DoLoadTexture( FbxProceduralTexture * p_texture, Pass & p_pass, TextureChannel p_channel )
	{
		return nullptr;
	}

	std::map< TextureChannel, String > TEXTURE_CHANNEL_NAME =
	{
		{ TextureChannel::eDiffuse, cuT( "Diffuse" ) },
		{ TextureChannel::eNormal, cuT( "Normal" ) },
		{ TextureChannel::eOpacity, cuT( "Opacity" ) },
		{ TextureChannel::eSpecular, cuT( "Specular" ) },
		{ TextureChannel::eHeight, cuT( "Height" ) },
		{ TextureChannel::eGloss, cuT( "Gloss" ) },
		{ TextureChannel::eEmissive, cuT( "Emissive" ) },
	};

	TextureUnitSPtr FbxSdkImporter::DoLoadTexture( Scene & p_scene, FbxPropertyT< FbxDouble3 > const & p_property, Pass & p_pass, TextureChannel p_channel )
	{
		TextureUnitSPtr texture = nullptr;
		FbxTexture * fbxtex = nullptr;
		int index = 0;
		FbxObject * object = p_property.GetSrcObject( index++ );

		while ( object && !texture )
		{
			if ( object->GetClassId() == FbxFileTexture::ClassId )
			{
				fbxtex = static_cast< FbxTexture * >( object );
				texture = DoLoadTexture( static_cast< FbxFileTexture * >( object ), p_pass, p_channel );
			}
			else if ( object->GetClassId() == FbxLayeredTexture::ClassId )
			{
				fbxtex = static_cast< FbxTexture * >( object );
				texture = DoLoadTexture( static_cast< FbxLayeredTexture * >( object ), p_pass, p_channel );
			}
			else if ( object->GetClassId() == FbxProceduralTexture::ClassId )
			{
				fbxtex = static_cast< FbxTexture * >( object );
				texture = DoLoadTexture( static_cast< FbxProceduralTexture * >( object ), p_pass, p_channel );
			}

			object = p_property.GetSrcObject( index++ );
		}

		if ( texture )
		{
			Logger::LogDebug( StringStream() << "    Texture: " << fbxtex->GetName() << cuT( " at channel " ) << TEXTURE_CHANNEL_NAME[p_channel] );
			texture->SetChannel( p_channel );

			static const WrapMode mode[2] =
			{
				WrapMode::eRepeat,
				WrapMode::eClampToBorder,
			};

			SamplerSPtr sampler;

			if ( !p_scene.GetSamplerView().Has( fbxtex->GetName() ) )
			{
				sampler = p_scene.GetSamplerView().Add( fbxtex->GetName() );
				sampler->SetWrappingMode( TextureUVW::eU, mode[fbxtex->WrapModeU] );
				sampler->SetWrappingMode( TextureUVW::eV, mode[fbxtex->WrapModeV] );
			}
			else
			{
				sampler = p_scene.GetSamplerView().Find( fbxtex->GetName() );
			}

			texture->SetSampler( sampler );
		}

		return texture;
	}
}

//*************************************************************************************************
