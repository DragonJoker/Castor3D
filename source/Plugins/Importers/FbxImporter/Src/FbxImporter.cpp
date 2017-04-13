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

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace C3dFbx
{
	namespace
	{
		void DoInitializeSdkObjects( FbxManager *& p_fbxManager, FbxScene *& p_fbxScene )
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

		bool DoLoadScene( FbxManager * p_fbxManager, FbxDocument * p_fbxScene, Path const & p_fileName )
		{
			int lFileMajor, lFileMinor, lFileRevision;
			int lSDKMajor, lSDKMinor, lSDKRevision;
			std::string l_fileName = string::string_cast< char >( p_fileName );

			// Get the file version number generate by the FBX SDK.
			FbxManager::GetFileFormatVersion( lSDKMajor, lSDKMinor, lSDKRevision );

			// Create an importer.
			FbxImporter * l_fbxImporter = FbxImporter::Create( p_fbxManager, "" );

			// Initialize the importer by providing a filename.
			const bool lImportStatus = l_fbxImporter->Initialize( l_fileName.c_str(), -1, p_fbxManager->GetIOSettings() );
			l_fbxImporter->GetFileVersion( lFileMajor, lFileMinor, lFileRevision );

			if ( !lImportStatus )
			{
				FbxString error = l_fbxImporter->GetStatus().GetErrorString();
				Logger::LogError( "Call to FbxImporter::Initialize() failed." );
				Logger::LogError( std::stringstream() << "Error returned: " << error.Buffer() << "\n" );

				if ( l_fbxImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion )
				{
					Logger::LogError( std::stringstream() << "FBX file format version for this FBX SDK is " << lSDKMajor << "." << lSDKMinor << "." << lSDKRevision );
					Logger::LogError( std::stringstream() << "FBX file format version for file " << l_fileName << " is " << lFileMajor << "." << lFileMinor << "." << lFileRevision << "\n" );
				}

				return false;
			}

			Logger::LogDebug( std::stringstream() << "FBX file format version for this FBX SDK is " << lSDKMajor << "." << lSDKMinor << "." << lSDKRevision );

			if ( l_fbxImporter->IsFBX() )
			{
				Logger::LogDebug( std::stringstream() << "FBX file format version for file " << l_fileName << " is " << lFileMajor << "." << lFileMinor << "." << lFileRevision << "\n" );

				// From this point, it is possible to access animation stack information without
				// the expense of loading the entire file.

				Logger::LogDebug( "Animation Stack Information" );

				int lAnimStackCount = l_fbxImporter->GetAnimStackCount();

				Logger::LogDebug( std::stringstream() << "    Number of Animation Stacks: " << lAnimStackCount );
				Logger::LogDebug( std::stringstream() << "    Current Animation Stack: \"" << l_fbxImporter->GetActiveAnimStackName().Buffer() << "\"\n" );

				for ( int i = 0; i < lAnimStackCount; i++ )
				{
					FbxTakeInfo * lTakeInfo = l_fbxImporter->GetTakeInfo( i );

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
				p_fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_MATERIAL, true );
				p_fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_TEXTURE, true );
				p_fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_LINK, true );
				p_fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_SHAPE, true );
				p_fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_GOBO, true );
				p_fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_ANIMATION, true );
				p_fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_GLOBAL_SETTINGS, true );
			}

			// Import the scene.
			int l_status = l_fbxImporter->Import( p_fbxScene );

			if ( l_status == false && l_fbxImporter->GetStatus().GetCode() == FbxStatus::ePasswordError )
			{
				Logger::LogInfo( "Please enter password: " );

				std::string l_password;

				FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
				std::cin >> l_password;
				FBXSDK_CRT_SECURE_NO_WARNING_END

				FbxString l_string( l_password.c_str() );

				p_fbxManager->GetIOSettings()->SetStringProp( IMP_FBX_PASSWORD, l_string );
				p_fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_PASSWORD_ENABLE, true );

				l_status = l_fbxImporter->Import( p_fbxScene );

				if ( l_status == false && l_fbxImporter->GetStatus().GetCode() == FbxStatus::ePasswordError )
				{
					CASTOR_EXCEPTION( cuT( "\nPassword is wrong, import aborted.\n" ) );
				}
			}

			// Destroy the importer.
			l_fbxImporter->Destroy();

			return l_status != 0;;
		}

		inline uint32_t DoGetIndex( FbxMesh * p_mesh, uint32_t p_poly, uint32_t p_vertex )
		{
			uint32_t l_return = p_mesh->GetPolygonVertex( p_poly, p_vertex );
			assert( l_return != 0xFFFFFFFF && "Index is out of range" );
			return l_return;
		}

		inline FaceIndices DoGetFace( FbxMesh * p_fbxMesh, uint32_t p_poly, uint32_t a, uint32_t b, uint32_t c )
		{
			auto l_a = DoGetIndex( p_fbxMesh, p_poly, a );
			auto l_b = DoGetIndex( p_fbxMesh, p_poly, b );
			auto l_c = DoGetIndex( p_fbxMesh, p_poly, c );
			return { { l_a, l_b, l_c } };
		}

		template< typename T >
		inline void DoFillBufferFromValue( T const & p_value, real *& p_buffer )
		{
			CASTOR_EXCEPTION( "Missing buffer filler from FBX value" );
		}

		template<>
		inline void DoFillBufferFromValue< FbxVector4 >( FbxVector4 const & p_value, real *& p_buffer )
		{
			*p_buffer++ = real( p_value[0] );
			*p_buffer++ = real( p_value[1] );
			*p_buffer++ = real( p_value[2] );
		}

		template<>
		inline void DoFillBufferFromValue< FbxVector2 >( FbxVector2 const & p_value, real *& p_buffer )
		{
			*p_buffer++ = real( p_value[0] );
			*p_buffer++ = real( p_value[1] );
			p_buffer++;
		}

		template< typename T >
		inline bool DoRetrieveMeshValues( String const & p_name, FbxMesh * p_mesh, FbxLayerElementTemplate< T > * p_fbxValues, std::vector< real > & p_values )
		{
			bool l_return = false;

			if ( p_fbxValues )
			{
				uint32_t l_count = p_fbxValues->GetDirectArray().GetCount();
				bool l_useIndex = p_fbxValues->GetReferenceMode() == FbxLayerElement::eIndexToDirect
								  || p_fbxValues->GetReferenceMode() == FbxLayerElement::eIndex;

				switch ( p_fbxValues->GetMappingMode() )
				{
				case FbxLayerElement::eByControlPoint:
					if ( l_useIndex )
					{
						auto l_buffer = p_values.data();

						for ( auto i = 0u; i < l_count; ++i )
						{
							DoFillBufferFromValue( p_fbxValues->GetDirectArray().GetAt( p_fbxValues->GetIndexArray().GetAt( i ) ), l_buffer );
						}
					}
					else
					{
						auto l_buffer = p_values.data();

						for ( auto i = 0u; i < l_count; ++i )
						{
							DoFillBufferFromValue( p_fbxValues->GetDirectArray().GetAt( i ), l_buffer );
						}
					}

					break;

				case FbxLayerElement::eByPolygonVertex:
					if ( l_useIndex )
					{
						uint32_t l_vertexIndex = 0;

						for ( int i = 0; i < p_mesh->GetPolygonCount(); ++i )
						{
							for ( int j = 0; j < p_mesh->GetPolygonSize( i ); ++j )
							{
								int l_cpIndex = p_mesh->GetPolygonVertex( i, j );
								auto l_buffer = p_values.data() + ( l_cpIndex * 3 );
								DoFillBufferFromValue( p_fbxValues->GetDirectArray().GetAt( p_fbxValues->GetIndexArray().GetAt( l_vertexIndex ) ), l_buffer );
								++l_vertexIndex;
							}
						}
					}
					else
					{
						uint32_t l_vertexIndex = 0;

						for ( int i = 0; i < p_mesh->GetPolygonCount(); ++i )
						{
							for ( int j = 0; j < p_mesh->GetPolygonSize( i ); ++j )
							{
								int l_cpIndex = p_mesh->GetPolygonVertex( i, j );
								auto l_buffer = p_values.data() + ( l_cpIndex * 3 );
								DoFillBufferFromValue( p_fbxValues->GetDirectArray().GetAt( l_vertexIndex ), l_buffer );
								++l_vertexIndex;
							}
						}
					}

					break;
				}

				l_return = true;
			}
			else
			{
				Logger::LogWarning( StringStream() << cuT( "No " ) << p_name << cuT( " for this mesh." ) );
			}

			return l_return;
		}

		FbxAMatrix DoGetGeometryTransformation( FbxNode * p_fbxNode )
		{
			if ( !p_fbxNode )
			{
				CASTOR_EXCEPTION( "Null for mesh geometry" );
			}

			const FbxVector4 l_translate = p_fbxNode->GetGeometricTranslation( FbxNode::eSourcePivot );
			const FbxVector4 l_rotate = p_fbxNode->GetGeometricRotation( FbxNode::eSourcePivot );
			const FbxVector4 l_scale = p_fbxNode->GetGeometricScaling( FbxNode::eSourcePivot );

			return FbxAMatrix( l_translate, l_rotate, l_scale );
		}

		uint32_t DoFindBoneIndex( String const & p_name, Skeleton const & p_skeleton )
		{
			auto l_it = std::find_if( p_skeleton.begin(), p_skeleton.end(), [&]( BoneSPtr p_bone )
			{
				return p_bone->GetName() == p_name;
			} );

			uint32_t l_return = 0xFFFFFFFF;

			if ( l_it != p_skeleton.end() )
			{
				l_return = uint32_t( std::distance( p_skeleton.begin(), l_it ) );
			}
			else
			{
				CASTOR_EXCEPTION( "Skeleton information in FBX file is corrupted" );
			}

			return l_return;
		}

		template< typename FbxType >
		void DoTraverseDepthFirst( FbxNode * p_fbxNode, FbxNodeAttribute::EType p_type, std::function< void( FbxNode *, FbxType * ) > p_function )
		{
			if ( p_fbxNode )
			{
				if ( p_fbxNode->GetNodeAttribute() )
				{
					if ( p_fbxNode->GetNodeAttribute()->GetAttributeType() == p_type )
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
		FbxManager * l_fbxManager = nullptr;
		FbxScene * l_fbxScene = nullptr;

		// Prepare the FBX SDK.
		DoInitializeSdkObjects( l_fbxManager, l_fbxScene );

		// TODO

		DoDestroySdkObjects( l_fbxManager );

		return false;
	}

	bool FbxSdkImporter::DoImportMesh( Mesh & p_mesh )
	{
		bool l_return{ false };
		m_mapBoneByID.clear();
		m_arrayBones.clear();
		FbxManager * l_fbxManager = nullptr;
		FbxScene * l_fbxScene = nullptr;

		// Prepare the FBX SDK.
		DoInitializeSdkObjects( l_fbxManager, l_fbxScene );

		if ( l_fbxScene )
		{
			if ( DoLoadScene( l_fbxManager, l_fbxScene, m_fileName ) )
			{
				FbxGeometryConverter l_converter( l_fbxManager );
				l_converter.SplitMeshesPerMaterial( l_fbxScene, true );

				if ( l_converter.Triangulate( l_fbxScene, true ) )
				{
					DoLoadMaterials( *p_mesh.GetScene(), l_fbxScene );
					DoLoadSkeleton( p_mesh, l_fbxScene->GetRootNode() );
					DoLoadMeshes( p_mesh, *p_mesh.GetScene(), l_fbxScene->GetRootNode() );

					if ( p_mesh.GetSkeleton() )
					{
						DoLoadAnimations( p_mesh, l_fbxScene );
					}

					l_return = true;
				}
			}
		}

		DoDestroySdkObjects( l_fbxManager );
		return l_return;
	}

	void FbxSdkImporter::DoLoadMeshes( Mesh & p_mesh, Scene & p_scene, FbxNode * p_fbxNode )
	{
		DoTraverseDepthFirst< FbxMesh >( p_fbxNode, FbxNodeAttribute::eMesh, [this, &p_scene, &p_mesh]( FbxNode * p_fbxNode, FbxMesh * p_fbxMesh )
		{
			SubmeshSPtr l_submesh = DoProcessMesh( p_mesh, p_fbxMesh );
			auto l_material = p_fbxNode->GetMaterial( 0 );

			if ( l_material )
			{
				l_submesh->SetDefaultMaterial( p_scene.GetMaterialView().Find( l_material->GetName() ) );
			}

			if ( p_mesh.GetSkeleton() )
			{
				FbxMesh * l_mesh = p_fbxNode->GetMesh();
				std::vector< VertexBoneData > l_boneData{ l_submesh->GetPointsCount() };
				DoProcessBonesWeights( p_fbxNode, *p_mesh.GetSkeleton(), l_boneData );
				l_submesh->AddBoneDatas( l_boneData );
			}
		} );
	}

	void FbxSdkImporter::DoLoadSkeleton( Mesh & p_mesh, FbxNode * p_fbxNode )
	{
		SkeletonSPtr l_skeleton = std::make_shared< Skeleton >( *p_mesh.GetScene() );

		for ( int i = 0; i < p_fbxNode->GetChildCount(); ++i )
		{
			DoProcessBones( p_fbxNode->GetChild( i ), *l_skeleton, nullptr );
		}

		if ( std::distance( l_skeleton->begin(), l_skeleton->end() ) > 0 )
		{
			p_mesh.SetSkeleton( l_skeleton );
		}
	}

	void FbxSdkImporter::DoLoadAnimations( Mesh & p_mesh, FbxScene * p_scene )
	{
		DoTraverseDepthFirst< FbxMesh >( p_scene->GetRootNode(), FbxNodeAttribute::eMesh, [this, &p_scene, &p_mesh]( FbxNode * p_fbxNode, FbxMesh * p_fbxMesh )
		{
			FbxMesh * l_mesh = p_fbxNode->GetMesh();
			uint32_t l_deformersCount = l_mesh->GetDeformerCount();

			for ( uint32_t l_deformerIndex = 0; l_deformerIndex < l_deformersCount; ++l_deformerIndex )
			{
				FbxSkin * l_skin = reinterpret_cast< FbxSkin * >( l_mesh->GetDeformer( l_deformerIndex, FbxDeformer::eSkin ) );

				if ( l_skin )
				{
					DoProcessSkeletonAnimations( p_scene, p_fbxNode, l_skin, *p_mesh.GetSkeleton() );
				}
			}
		} );

		for ( auto const & l_it : p_mesh.GetSkeleton()->GetAnimations() )
		{
			Logger::LogDebug( StringStream() << cuT( "Found Animation: " ) << l_it.first );
		}
	}

	void FbxSdkImporter::DoProcessSkeletonAnimations( FbxScene * p_fbxScene, FbxNode * p_fbxNode, FbxSkin * p_fbxSkin, Skeleton & p_skeleton )
	{
		uint32_t l_clustersCount = p_fbxSkin->GetClusterCount();
		const Point3r l_geoTranslate{ p_fbxNode->GetGeometricTranslation( FbxNode::eSourcePivot ).Buffer() };
		const Quaternion l_geoRotate{ p_fbxNode->GetGeometricRotation( FbxNode::eSourcePivot ).Buffer() };
		const Point3r l_geoScale{ p_fbxNode->GetGeometricScaling( FbxNode::eSourcePivot ).Buffer() };

		for ( uint32_t l_clusterIndex = 0; l_clusterIndex < l_clustersCount; ++l_clusterIndex )
		{
			FbxCluster * l_cluster = p_fbxSkin->GetCluster( l_clusterIndex );
			auto l_link = l_cluster->GetLink();
			uint32_t l_animationsCount = p_fbxScene->GetSrcObjectCount( FbxCriteria::ObjectType( FbxAnimStack::ClassId ) );
			auto l_bone = *( p_skeleton.begin() + DoFindBoneIndex( string::string_cast< xchar >( l_link->GetName() ), p_skeleton ) );

			for ( uint32_t l_animationIndex = 0; l_animationIndex < l_animationsCount; ++l_animationIndex )
			{
				FbxAnimStack * l_animStack = p_fbxScene->GetSrcObject< FbxAnimStack >( l_animationIndex );
				auto l_name = l_animStack->GetName();
				auto & l_animation = p_skeleton.CreateAnimation( string::string_cast< xchar >( l_name ) );
				FbxTakeInfo * l_takeInfo = p_fbxScene->GetTakeInfo( l_name );
				uint64_t l_start = l_takeInfo->mLocalTimeSpan.GetStart().GetFrameCount( FbxTime::eFrames24 );
				uint64_t l_finish = l_takeInfo->mLocalTimeSpan.GetStop().GetFrameCount( FbxTime::eFrames24 );
				uint64_t l_animationLength = l_finish - l_start + 1;
				SkeletonAnimationObjectSPtr l_object;

				if ( l_bone->GetParent() )
				{
					l_object = l_animation.AddObject( l_bone, l_animation.GetObject( *l_bone->GetParent() ) );
				}
				else
				{
					l_object = l_animation.AddObject( l_bone, nullptr );
				}

				if ( l_object )
				{
					auto l_inc = std::chrono::milliseconds{ 1000 / 24 };
					auto l_from = 0_ms;

					for ( uint64_t i = l_start; i <= l_finish; ++i )
					{
						FbxTime l_time;
						l_time.SetFrame( i, FbxTime::eFrames24 );
						Point3r l_translate = l_geoTranslate
											  + Point3r{ ( p_fbxNode->EvaluateLocalTranslation( l_time ) ).Buffer() }
											  + Point3r{ ( l_link->EvaluateLocalTranslation( l_time ) ).Buffer() };
						Quaternion l_rotate = l_geoRotate
											  * Quaternion{ ( p_fbxNode->EvaluateLocalRotation( l_time ) ).Buffer() }
											  * Quaternion{ ( l_link->EvaluateLocalRotation( l_time ) ).Buffer() };
						Point3r l_scale = l_geoScale
										  * Point3r{ ( p_fbxNode->EvaluateLocalScaling( l_time ) ).Buffer() }
										  * Point3r{ ( l_link->EvaluateLocalScaling( l_time ) ).Buffer() };
						l_object->AddKeyFrame( l_from, l_translate, l_rotate, l_scale );
						l_from += l_inc;
					}
				}

				l_animation.UpdateLength();
			}
		}
	}

	void FbxSdkImporter::DoProcessBones( FbxNode * p_fbxNode, Skeleton & p_skeleton, BoneSPtr p_parent )
	{
		auto l_bone = p_parent;

		if ( p_fbxNode->GetNodeAttribute() && p_fbxNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton )
		{
			FbxSkeleton * l_skeleton = reinterpret_cast< FbxSkeleton * >( p_fbxNode );
			String l_name = string::string_cast< xchar >( l_skeleton->GetName() );

			if ( m_mapBoneByID.find( l_name ) == m_mapBoneByID.end() )
			{
				l_bone = std::make_shared< Bone >( p_skeleton );
				l_bone->SetName( l_name );
				m_arrayBones.push_back( l_bone );
				m_mapBoneByID[l_name] = uint32_t( m_mapBoneByID.size() );
				p_skeleton.AddBone( l_bone );

				if ( p_parent )
				{
					p_skeleton.SetBoneParent( l_bone, p_parent );
				}
			}
			else
			{
				l_bone = m_arrayBones[m_mapBoneByID[l_name]];
			}
		}

		for ( int i = 0; i < p_fbxNode->GetChildCount(); i++ )
		{
			DoProcessBones( p_fbxNode->GetChild( i ), p_skeleton, l_bone );
		}
	}

	void FbxSdkImporter::DoProcessBonesWeights( FbxNode * p_fbxNode, Skeleton const & p_skeleton, std::vector< VertexBoneData > & p_boneData )
	{
		FbxMesh * l_mesh = p_fbxNode->GetMesh();
		uint32_t l_deformersCount = l_mesh->GetDeformerCount();
		FbxAMatrix l_geometryTransform = DoGetGeometryTransformation( p_fbxNode );

		for ( uint32_t l_deformerIndex = 0; l_deformerIndex < l_deformersCount; ++l_deformerIndex )
		{
			FbxSkin * l_skin = reinterpret_cast< FbxSkin * >( l_mesh->GetDeformer( l_deformerIndex, FbxDeformer::eSkin ) );

			if ( l_skin )
			{
				uint32_t l_clustersCount = l_skin->GetClusterCount();

				for ( uint32_t l_clusterIndex = 0; l_clusterIndex < l_clustersCount; ++l_clusterIndex )
				{
					FbxCluster * l_cluster = l_skin->GetCluster( l_clusterIndex );
					uint32_t l_boneIndex = DoFindBoneIndex( string::string_cast< xchar >( l_cluster->GetLink()->GetName() ), p_skeleton );
					auto l_bone = *( p_skeleton.begin() + l_boneIndex );
					// The transformation of the mesh at binding time.
					FbxAMatrix l_transformMatrix;
					l_cluster->GetTransformMatrix( l_transformMatrix );
					// The transformation of the cluster(joint) at binding time from joint space to world space.
					FbxAMatrix l_transformLinkMatrix;
					l_cluster->GetTransformLinkMatrix( l_transformLinkMatrix );
					// Update the information in mSkeleton.
					l_bone->SetOffsetMatrix( Matrix4x4r{ &( l_transformLinkMatrix.Inverse() * l_transformMatrix * l_geometryTransform ).Buffer()[0][0] } );

					// Associate each joint with the control points it affects.
					uint32_t l_indicesCount = l_cluster->GetControlPointIndicesCount();

					for ( uint32_t i = 0; i < l_indicesCount; ++i )
					{
						p_boneData[l_cluster->GetControlPointIndices()[i]].AddBoneData( l_boneIndex, real( l_cluster->GetControlPointWeights()[i] ) );
					}
				}
			}
		}
	}

	void FbxSdkImporter::DoLoadMaterials( Scene & p_scene, FbxScene * p_fbxScene )
	{
		auto & l_cache = p_scene.GetMaterialView();
		FbxArray< FbxSurfaceMaterial * > l_mats;
		p_fbxScene->FillMaterialArray( l_mats );
		MaterialPtrStrMap l_materials;

		auto l_parseLambert = [this, &p_scene]( FbxSurfaceLambert * p_lambert, LegacyPass & p_pass )
		{
			auto l_lambert = static_cast< FbxSurfaceLambert * >( p_lambert );
			//p_pass.SetAlpha( float( l_lambert->TransparencyFactor ) );
			FbxDouble3 l_diffuse = l_lambert->Diffuse;
			p_pass.SetDiffuse( Colour::from_rgb( { float( l_diffuse[0] ), float( l_diffuse[1] ), float( l_diffuse[2] ) } ) * l_lambert->DiffuseFactor );
			FbxDouble3 l_emissive = l_lambert->Emissive;
			p_pass.SetEmissive( float( point::length( Point3f{ float( l_emissive[0] ), float( l_emissive[1] ), float( l_emissive[2] ) } ) * l_lambert->EmissiveFactor ) );

			DoLoadTexture( p_scene, l_lambert->Diffuse, p_pass, TextureChannel::eDiffuse );
			DoLoadTexture( p_scene, l_lambert->Emissive, p_pass, TextureChannel::eEmissive );
			DoLoadTexture( p_scene, l_lambert->NormalMap, p_pass, TextureChannel::eNormal );
			DoLoadTexture( p_scene, l_lambert->TransparentColor, p_pass, TextureChannel::eOpacity );
		};

		for ( int i = 0; i < l_mats.Size(); ++i )
		{
			FbxSurfaceMaterial * l_fbxMaterial = l_mats[i];

			if ( l_materials.find( l_fbxMaterial->GetName() ) == l_materials.end() )
			{
				auto l_material = l_cache.Add( l_fbxMaterial->GetName(), MaterialType::eLegacy );
				REQUIRE( l_material->GetType() == MaterialType::eLegacy );
				auto l_pass = l_material->GetTypedPass< MaterialType::eLegacy >( 0u );
				Logger::LogDebug( StringStream() << "Material: " << l_fbxMaterial->GetName() );
				String l_model = string::string_cast< xchar >( l_fbxMaterial->ShadingModel.Get().Buffer() );
				Logger::LogDebug( StringStream() << "    ShadingModel: " << l_model );

				if ( l_fbxMaterial->GetClassId() == FbxSurfaceLambert::ClassId )
				{
					l_parseLambert( static_cast< FbxSurfaceLambert * >( l_fbxMaterial ), *l_pass );
				}
				else if ( l_fbxMaterial->GetClassId() == FbxSurfacePhong::ClassId )
				{
					auto l_phong = static_cast< FbxSurfacePhong * >( l_fbxMaterial );
					l_parseLambert( l_phong, *l_pass );
					l_pass->SetShininess( float( l_phong->Shininess ) );
					FbxDouble3 l_specular = l_phong->Specular;
					l_pass->SetSpecular( Colour::from_rgb( { float( l_specular[0] ), float( l_specular[1] ), float( l_specular[2] ) } ) * l_phong->SpecularFactor );
					DoLoadTexture( p_scene, l_phong->Specular, *l_pass, TextureChannel::eSpecular );
					DoLoadTexture( p_scene, l_phong->Shininess, *l_pass, TextureChannel::eGloss );
				}
			}
		}
	}

	SubmeshSPtr FbxSdkImporter::DoProcessMesh( Mesh & p_mesh, FbxMesh * p_fbxMesh )
	{
		p_fbxMesh->GenerateNormals();
		auto l_submesh = p_mesh.CreateSubmesh();
		l_submesh->SetDefaultMaterial( GetEngine()->GetMaterialCache().Find( cuT( "White" ) ) );
		bool l_tangentSpace = false;

		Logger::LogDebug( StringStream() << "Mesh: " << p_fbxMesh->GetName() );
		Logger::LogDebug( StringStream() << "    Points: " << p_fbxMesh->GetControlPointsCount() );
		Logger::LogDebug( StringStream() << "    Polygons: " << p_fbxMesh->GetPolygonCount() );

		// Vertex
		uint32_t l_pointsCount = p_fbxMesh->GetControlPointsCount();
		std::vector< real > l_vtx( l_pointsCount * 3 );
		std::vector< real > l_nml( l_pointsCount * 3 );
		std::vector< real > l_tan( l_pointsCount * 3 );
		std::vector< real > l_bit( l_pointsCount * 3 );
		std::vector< real > l_tex( l_pointsCount * 3 );
		std::vector< InterleavedVertex > l_vertices{ l_pointsCount };
		uint32_t l_index{ 0u };

		// Positions
		for ( auto & l_vertex : l_vertices )
		{
			FbxVector4 l_fbxVertex = p_fbxMesh->GetControlPointAt( l_index++ );
			l_vertex.m_pos[0] = real( l_fbxVertex[0] );
			l_vertex.m_pos[1] = real( l_fbxVertex[1] );
			l_vertex.m_pos[2] = real( l_fbxVertex[2] );
		}

		// Normals
		auto l_normals = p_fbxMesh->GetLayer( 0 )->GetNormals();

		if ( DoRetrieveMeshValues( cuT( "Normals" ), p_fbxMesh, l_normals, l_nml ) )
		{
			l_index = 0u;
			real * l_buffer{ l_nml.data() };

			for ( auto & l_vertex : l_vertices )
			{
				std::memcpy( l_vertex.m_nml.data(), l_buffer, sizeof( l_vertex.m_nml ) );
				l_buffer += 3;
			}
		}

		// Texture UVs
		auto l_uvs = p_fbxMesh->GetLayer( 0 )->GetUVs();

		if ( DoRetrieveMeshValues( cuT( "Texture UVs" ), p_fbxMesh, l_uvs, l_tex ) )
		{
			l_index = 0u;
			real * l_buffer{ l_tex.data() };

			for ( auto & l_vertex : l_vertices )
			{
				std::memcpy( l_vertex.m_tex.data(), l_buffer, sizeof( l_vertex.m_tex ) );
				l_buffer += 3;
			}
		}

		if ( m_parameters.Get( cuT( "tangent_space" ), l_tangentSpace ) && l_tangentSpace )
		{
			p_fbxMesh->GenerateTangentsData( 0 );

			// Tangents
			auto l_tangents = p_fbxMesh->GetLayer( 0 )->GetTangents();

			if ( DoRetrieveMeshValues( cuT( "Tangents" ), p_fbxMesh, l_tangents, l_tan ) )
			{
				l_index = 0u;
				real * l_buffer{ l_tan.data() };

				for ( auto & l_vertex : l_vertices )
				{
					std::memcpy( l_vertex.m_tan.data(), l_buffer, sizeof( l_vertex.m_tan ) );
					l_buffer += 3;
				}
			}

			// Bitangents
			auto l_bitangents = p_fbxMesh->GetLayer( 0 )->GetBinormals();

			if ( DoRetrieveMeshValues( cuT( "Bitangents" ), p_fbxMesh, l_bitangents, l_bit ) )
			{
				l_index = 0u;
				real * l_buffer{ l_bit.data() };

				for ( auto & l_vertex : l_vertices )
				{
					std::memcpy( l_vertex.m_bin.data(), l_buffer, sizeof( l_vertex.m_bin ) );
					l_buffer += 3;
				}
			}
		}

		// Faces
		uint32_t l_polyCount = p_fbxMesh->GetPolygonCount();
		std::vector< FaceIndices > l_faces;
		l_faces.reserve( l_polyCount );

		for ( auto l_poly = 0u; l_poly < l_polyCount; ++l_poly )
		{
			l_faces.push_back( DoGetFace( p_fbxMesh, l_poly, 0, 1, 2 ) );
		}

		l_submesh->AddPoints( l_vertices );
		l_submesh->AddFaceGroup( l_faces );

		return l_submesh;
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
		TextureUnitSPtr l_texture = nullptr;
		FbxTexture * l_fbxtex = nullptr;
		int l_index = 0;
		FbxObject * l_object = p_property.GetSrcObject( l_index++ );

		while ( l_object && !l_texture )
		{
			if ( l_object->GetClassId() == FbxFileTexture::ClassId )
			{
				l_fbxtex = static_cast< FbxTexture * >( l_object );
				l_texture = DoLoadTexture( static_cast< FbxFileTexture * >( l_object ), p_pass, p_channel );
			}
			else if ( l_object->GetClassId() == FbxLayeredTexture::ClassId )
			{
				l_fbxtex = static_cast< FbxTexture * >( l_object );
				l_texture = DoLoadTexture( static_cast< FbxLayeredTexture * >( l_object ), p_pass, p_channel );
			}
			else if ( l_object->GetClassId() == FbxProceduralTexture::ClassId )
			{
				l_fbxtex = static_cast< FbxTexture * >( l_object );
				l_texture = DoLoadTexture( static_cast< FbxProceduralTexture * >( l_object ), p_pass, p_channel );
			}

			l_object = p_property.GetSrcObject( l_index++ );
		}

		if ( l_texture )
		{
			Logger::LogDebug( StringStream() << "    Texture: " << l_fbxtex->GetName() << cuT( " at channel " ) << TEXTURE_CHANNEL_NAME[p_channel] );
			l_texture->SetChannel( p_channel );
			l_texture->SetAlphaValue( float( l_fbxtex->Alpha ) );

			static const WrapMode l_mode[2] =
			{
				WrapMode::eRepeat,
				WrapMode::eClampToBorder,
			};

			SamplerSPtr l_sampler;

			if ( !p_scene.GetSamplerView().Has( l_fbxtex->GetName() ) )
			{
				l_sampler = p_scene.GetSamplerView().Add( l_fbxtex->GetName() );
				l_sampler->SetWrappingMode( TextureUVW::eU, l_mode[l_fbxtex->WrapModeU] );
				l_sampler->SetWrappingMode( TextureUVW::eV, l_mode[l_fbxtex->WrapModeV] );
			}
			else
			{
				l_sampler = p_scene.GetSamplerView().Find( l_fbxtex->GetName() );
			}

			l_texture->SetSampler( l_sampler );
		}

		return l_texture;
	}
}

//*************************************************************************************************
