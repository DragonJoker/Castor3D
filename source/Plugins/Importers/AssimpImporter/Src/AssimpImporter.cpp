#include "AssimpImporter.hpp"

#include <GeometryManager.hpp>
#include <MaterialManager.hpp>
#include <MeshManager.hpp>
#include <PluginManager.hpp>
#include <SceneManager.hpp>
#include <SceneNodeManager.hpp>

#include <Animation/AnimationObject.hpp>
#include <Animation/Bone.hpp>
#include <Animation/SkeletonAnimationBone.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Plugin/ImporterPlugin.hpp>
#include <Manager/ManagerView.hpp>

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

C3D_Assimp_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Assimp_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_Assimp_API String GetName()
{
	return cuT( "ASSIMP Importer" );
}

C3D_Assimp_API ImporterPlugin::ExtensionArray GetExtensions( Engine * p_engine )
{
	ImporterPlugin::ExtensionArray l_extensions =
	{
		ImporterPlugin::Extension{ cuT( "DAE" ), cuT( "Collada" ) },
		ImporterPlugin::Extension{ cuT( "BLEND" ), cuT( "Blender" ) },
		ImporterPlugin::Extension{ cuT( "BVH" ), cuT( "3 Biovision BVH" ) },
		ImporterPlugin::Extension{ cuT( "IFC" ), cuT( "IFC-STEP, Industry Foundation Classes" ) },
		ImporterPlugin::Extension{ cuT( "NFF" ), cuT( "Sense8 WorldToolkit" ) },
		ImporterPlugin::Extension{ cuT( "SMD" ), cuT( "Valve Model" ) },
		ImporterPlugin::Extension{ cuT( "VTA" ), cuT( "Valve Model" ) },
		ImporterPlugin::Extension{ cuT( "MDL" ), cuT( "3 Quake I" ) },
		ImporterPlugin::Extension{ cuT( "PK3" ), cuT( "Quake 3 BSP" ) },
		ImporterPlugin::Extension{ cuT( "MDC" ), cuT( "1 RtCW" ) },
		ImporterPlugin::Extension{ cuT( "MD5MESH" ), cuT( "Doom 3" ) },
		ImporterPlugin::Extension{ cuT( "XML" ), cuT( "Ogre XML Mesh" ) },
		ImporterPlugin::Extension{ cuT( "X" ), cuT( "DirectX X" ) },
		ImporterPlugin::Extension{ cuT( "Q3O" ), cuT( "Quick3D" ) },
		ImporterPlugin::Extension{ cuT( "Q3S" ), cuT( "Quick3D" ) },
		ImporterPlugin::Extension{ cuT( "RAW" ), cuT( "Raw Triangles" ) },
		ImporterPlugin::Extension{ cuT( "AC" ), cuT( "AC3D" ) },
		ImporterPlugin::Extension{ cuT( "STL" ), cuT( "Stereolithography" ) },
		ImporterPlugin::Extension{ cuT( "DXF" ), cuT( "Autodesk DXF" ) },
		ImporterPlugin::Extension{ cuT( "IRRMESH" ), cuT( "Irrlicht Mesh" ) },
		ImporterPlugin::Extension{ cuT( "IRR" ), cuT( "Irrlicht Scene" ) },
		ImporterPlugin::Extension{ cuT( "OFF" ), cuT( "Object File Format" ) },
		ImporterPlugin::Extension{ cuT( "TER" ), cuT( "Terragen Terrain" ) },
		ImporterPlugin::Extension{ cuT( "MDL" ), cuT( "3D GameStudio Model" ) },
		ImporterPlugin::Extension{ cuT( "HMP" ), cuT( "3D GameStudio Terrain" ) },
		ImporterPlugin::Extension{ cuT( "MS3D" ), cuT( "3 Milkshape 3D" ) },
		ImporterPlugin::Extension{ cuT( "LWS" ), cuT( "LightWave Scene" ) },
		ImporterPlugin::Extension{ cuT( "LXO" ), cuT( "Modo Model" ) },
		ImporterPlugin::Extension{ cuT( "CSM" ), cuT( "CharacterStudio Motion" ) },
		ImporterPlugin::Extension{ cuT( "COB" ), cuT( "TrueSpace" ) },
		ImporterPlugin::Extension{ cuT( "SCN" ), cuT( "TrueSpace" ) },
		ImporterPlugin::Extension{ cuT( "XGL" ), cuT( "2 XGL" ) },
		ImporterPlugin::Extension{ cuT( "ZGL" ), cuT( "2 XGL" ) },
	};

	std::set< String > l_alreadyLoaded;

	for ( auto l_it : p_engine->GetPluginManager().GetPlugins( Castor3D::ePLUGIN_TYPE_IMPORTER ) )
	{
		auto const l_importer = std::static_pointer_cast< ImporterPlugin >( l_it.second );

		if ( l_importer->GetName() != GetName() )
		{
			for ( auto l_extension : l_importer->GetExtensions() )
			{
				l_alreadyLoaded.insert( l_extension.first );
			}
		}
	}

	if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "3DS" ) ) )
	{
		l_extensions.emplace_back( cuT( "3DS" ), cuT( "3D Studio Max 3DS" ) );
	}

	if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "ASE" ) ) )
	{
		l_extensions.emplace_back( cuT( "ASE" ), cuT( "3D Studio Max ASE" ) );
	}

	if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "OBJ" ) ) )
	{
		l_extensions.emplace_back( cuT( "OBJ" ), cuT( "Wavefront Object" ) );
	}

	if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "PLY" ) ) )
	{
		// Assimp's implementation crashes on big meshes.
		l_extensions.emplace_back( cuT( "PLY" ), cuT( "Stanford Polygon Library" ) );
	}

	if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "MD2" ) ) )
	{
		l_extensions.emplace_back( cuT( "MD2" ), cuT( "Quake II" ) );
	}

	if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "MD3" ) ) )
	{
		l_extensions.emplace_back( cuT( "MD3" ), cuT( "Quake III" ) );
	}

	if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "LWO" ) ) )
	{
		l_extensions.emplace_back( cuT( "LWO" ), cuT( "LightWave Model" ) );
	}

	return l_extensions;
}

C3D_Assimp_API void Create( Engine * p_engine, ImporterPlugin * p_plugin )
{
	ImporterSPtr l_pImporter = std::make_shared< C3dAssimp::AssimpImporter >( *p_engine );
	p_plugin->AttachImporter( l_pImporter );
}

C3D_Assimp_API void Destroy( ImporterPlugin * p_plugin )
{
	p_plugin->DetachImporter();
}

C3D_Assimp_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_Assimp_API void OnUnload( Castor3D::Engine * p_engine )
{
}

//*************************************************************************************************

namespace C3dAssimp
{
	namespace
	{
		const aiNodeAnim * FindNodeAnim( const aiAnimation * p_animation, const String p_nodeName )
		{
			const aiNodeAnim * l_return = nullptr;

			for ( uint32_t i = 0; i < p_animation->mNumChannels && !l_return; ++i )
			{
				const aiNodeAnim * l_nodeAnim = p_animation->mChannels[i];

				if ( string::string_cast< xchar >( l_nodeAnim->mNodeName.data ) == p_nodeName )
				{
					l_return = l_nodeAnim;
				}
			}

			return l_return;
		}

		template< typename T >
		void DoFind( real p_time,
					 typename std::map< real, T > const & p_map,
					 typename std::map< real, T >::const_iterator & p_prv,
					 typename std::map< real, T >::const_iterator & p_cur )
		{
			if ( p_map.empty() )
			{
				p_prv = p_cur = p_map.end();
			}
			else
			{
				p_cur = std::find_if( p_map.begin(), p_map.end(), [&p_time]( std::pair< real, T > const & p_pair )
				{
					return p_pair.first > p_time;
				} );

				if ( p_cur == p_map.end() )
				{
					--p_cur;
				}

				p_prv = p_cur;

				if ( p_prv != p_map.begin() )
				{
					p_prv--;
				}

				ENSURE( p_prv != p_cur );
			}
		}

		template< typename T >
		T DoCompute( real p_from, Interpolator< T > const & p_interpolator, std::map< real, T > const & p_values )
		{
			T l_return;

			if ( p_values.size() == 1 )
			{
				l_return = p_values.begin()->second;
			}
			else
			{
				auto l_prv = p_values.begin();
				auto l_cur = p_values.begin();
				DoFind( p_from, p_values, l_prv, l_cur );
				real l_dt = l_cur->first - l_prv->first;
				real l_factor = ( p_from - l_prv->first ) / l_dt;
				l_return = p_interpolator.Interpolate( l_prv->second, l_cur->second, l_factor );
			}

			return l_return;
		}
	}

	AssimpImporter::AssimpImporter( Engine & p_engine )
		: Importer( p_engine )
		, m_anonymous( 0 )
	{
	}

	AssimpImporter::~AssimpImporter()
	{
	}

	SceneSPtr AssimpImporter::DoImportScene()
	{
		SceneSPtr l_scene = GetEngine()->GetSceneManager().Create( cuT( "Scene_ASSIMP" ), *GetEngine() );
		DoImportMesh( *l_scene );

		if ( m_mesh )
		{
			SceneNodeSPtr l_node = l_scene->GetSceneNodeManager().Create( m_mesh->GetName(), l_scene->GetObjectRootNode() );
			GeometrySPtr l_geometry = l_scene->GetGeometryManager().Create( m_mesh->GetName(), l_node );

			for ( auto && l_submesh : *m_mesh )
			{
				m_mesh->GetEngine()->PostEvent( MakeInitialiseEvent( *l_submesh ) );
			}

			l_geometry->SetMesh( m_mesh );
			m_mesh.reset();
		}

		return l_scene;
	}

	MeshSPtr AssimpImporter::DoImportMesh( Scene & p_scene )
	{
		m_mapBoneByID.clear();
		m_arrayBones.clear();
		String l_name = m_fileName.GetFileName();
		String l_meshName = l_name.substr( 0, l_name.find_last_of( '.' ) );

		if ( GetEngine()->GetMeshManager().Has( l_meshName ) )
		{
			m_mesh = GetEngine()->GetMeshManager().Find( l_meshName );
		}
		else
		{
			m_mesh = GetEngine()->GetMeshManager().Create( l_meshName, eMESH_TYPE_CUSTOM, UIntArray(), RealArray() );
		}

		if ( !m_mesh->GetSubmeshCount() )
		{
			SubmeshSPtr l_submesh;
			Assimp::Importer l_importer;
			uint32_t l_flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_FixInfacingNormals;
			bool l_tangentSpace = false;
			xchar l_buffer[1024] = { 0 };

			if ( m_parameters.Get( cuT( "normals" ), l_buffer ) )
			{
				String l_normals = l_buffer;

				if ( l_normals == cuT( "smooth" ) )
				{
					l_flags |= aiProcess_GenSmoothNormals;
				}
			}

			if ( m_parameters.Get( cuT( "tangent_space" ), l_tangentSpace ) && l_tangentSpace )
			{
				l_flags |= aiProcess_CalcTangentSpace;
			}

			// And have it read the given file with some postprocessing
			aiScene const * l_aiScene = l_importer.ReadFile( string::string_cast< char >( m_fileName ), l_flags );

			if ( l_aiScene )
			{
				SkeletonSPtr l_skeleton = std::make_shared< Skeleton >();
				l_skeleton->SetGlobalInverseTransform( Matrix4x4r( &l_aiScene->mRootNode->mTransformation.Transpose().Inverse().a1 ) );

				if ( l_aiScene->HasMeshes() )
				{
					bool l_create = true;

					for ( uint32_t i = 0; i < l_aiScene->mNumMeshes; ++i )
					{
						if ( l_create )
						{
							l_submesh = m_mesh->CreateSubmesh();
						}

						l_create = DoProcessMesh( p_scene, l_skeleton, l_aiScene->mMeshes[i], l_aiScene, l_submesh );
					}

					if ( m_arrayBones.empty() )
					{
						l_skeleton.reset();
					}
					else
					{
						m_mesh->SetSkeleton( l_skeleton );
					}

					if ( l_skeleton )
					{
						for ( uint32_t i = 0; i < l_aiScene->mNumAnimations; ++i )
						{
							DoProcessAnimation( m_fileName.GetFileName(), l_skeleton, l_aiScene->mRootNode, l_aiScene->mAnimations[i] );
						}

						l_importer.FreeScene();

						if ( string::upper_case( m_fileName.GetExtension() ) == cuT( "MD5MESH" ) )
						{
							// Workaround to load multiple animations with MD5 models.
							PathArray l_files;
							File::ListDirectoryFiles( m_fileName.GetPath(), l_files );

							for ( auto l_file : l_files )
							{
								if ( string::lower_case( l_file.GetExtension() ) == cuT( "md5anim" ) )
								{
									// The .md5anim with the same name as the .md5mesh has already been loaded by assimp.
									if ( l_file.GetFileName() != m_fileName.GetFileName() )
									{
										auto l_scene = l_importer.ReadFile( l_file, l_flags );

										for ( uint32_t i = 0; i < l_scene->mNumAnimations; ++i )
										{
											DoProcessAnimation( l_file.GetFileName(), l_skeleton, l_scene->mRootNode, l_scene->mAnimations[i] );
										}

										l_importer.FreeScene();
									}
								}
							}
						}
					}
					else
					{
						l_importer.FreeScene();
					}
				}
				else
				{
					GetEngine()->GetMeshManager().Remove( l_meshName );
					m_mesh.reset();
					l_importer.FreeScene();
				}
			}
			else
			{
				// The import failed, report it
				Logger::LogError( std::stringstream() << "Scene import failed : " << l_importer.GetErrorString() );
				GetEngine()->GetMeshManager().Remove( l_meshName );
				m_mesh.reset();
			}
		}

		MeshSPtr l_return( m_mesh );
		m_mesh.reset();
		return l_return;
	}


	bool AssimpImporter::DoProcessMesh( Scene & p_scene, SkeletonSPtr p_skeleton, aiMesh const * p_aiMesh, aiScene const * p_aiScene, SubmeshSPtr p_submesh )
	{
		bool l_return = false;
		MaterialSPtr l_material;

		if ( p_aiMesh->mMaterialIndex < p_aiScene->mNumMaterials )
		{
			l_material = DoProcessMaterial( p_scene, p_aiScene->mMaterials[p_aiMesh->mMaterialIndex] );
		}

		if ( p_aiMesh->HasFaces() && p_aiMesh->HasPositions() && l_material )
		{
			p_submesh->SetDefaultMaterial( l_material );
			p_submesh->Ref( l_material );
			stVERTEX_GROUP l_vertices = { 0 };
			l_vertices.m_uiCount = p_aiMesh->mNumVertices;
#if CASTOR_USE_DOUBLE
			std::vector< real > vtx;
			std::vector< real > nml;
			std::vector< real > tan;
			std::vector< real > bin;
			std::vector< real > tex;
			vtx.reserve( p_aiMesh->mNumVertices * 3 );

			for ( unsigned int i = 0; i < p_aiMesh->mNumVertices; ++i )
			{
				vtx.push_back( p_aiMesh->mVertices[i].x );
				vtx.push_back( p_aiMesh->mVertices[i].y );
				vtx.push_back( p_aiMesh->mVertices[i].z );
			}

			l_vertices.m_pVtx = vtx.data();

			if ( p_aiMesh->mNormals )
			{
				nml.reserve( p_aiMesh->mNumVertices * 3 );

				for ( unsigned int i = 0; i < p_aiMesh->mNumVertices; ++i )
				{
					nml.push_back( p_aiMesh->mNormals[i].x );
					nml.push_back( p_aiMesh->mNormals[i].y );
					nml.push_back( p_aiMesh->mNormals[i].z );
				}

				l_vertices.m_pNml = nml.data();
			}

			if ( p_aiMesh->mTangents )
			{
				tan.reserve( p_aiMesh->mNumVertices * 3 );

				for ( unsigned int i = 0; i < p_aiMesh->mNumVertices; ++i )
				{
					tan.push_back( p_aiMesh->mTangents[i].x );
					tan.push_back( p_aiMesh->mTangents[i].y );
					tan.push_back( p_aiMesh->mTangents[i].z );
				}

				l_vertices.m_pTan = tan.data();
			}

			if ( p_aiMesh->mBitangents )
			{
				bin.reserve( p_aiMesh->mNumVertices * 3 );

				for ( unsigned int i = 0; i < p_aiMesh->mNumVertices; ++i )
				{
					bin.push_back( p_aiMesh->mBitangents[i].x );
					bin.push_back( p_aiMesh->mBitangents[i].y );
					bin.push_back( p_aiMesh->mBitangents[i].z );
				}

				l_vertices.m_pBin = bin.data();
			}

			if ( p_aiMesh->HasTextureCoords( 0 ) )
			{
				tex.reserve( p_aiMesh->mNumVertices * 3 );

				for ( unsigned int i = 0; i < p_aiMesh->mNumVertices; ++i )
				{
					tex.push_back( p_aiMesh->mTextureCoords[0][i].x );
					tex.push_back( p_aiMesh->mTextureCoords[0][i].y );
					tex.push_back( p_aiMesh->mTextureCoords[0][i].z );
				}

				l_vertices.m_pTex = tex.data();
			}

#else
			l_vertices.m_pVtx = ( real * )p_aiMesh->mVertices;
			l_vertices.m_pNml = ( real * )p_aiMesh->mNormals;
			l_vertices.m_pTan = ( real * )p_aiMesh->mTangents;
			l_vertices.m_pBin = ( real * )p_aiMesh->mBitangents;

			if ( p_aiMesh->HasTextureCoords( 0 ) )
			{
				l_vertices.m_pTex = ( real * )p_aiMesh->mTextureCoords[0];
			}

#endif

			p_submesh->AddPoints( l_vertices );

			std::vector< stVERTEX_BONE_DATA > l_arrayBones( p_aiMesh->mNumVertices );

			if ( p_aiMesh->HasBones() && p_skeleton )
			{
				DoProcessBones( p_skeleton, p_aiMesh->mBones, p_aiMesh->mNumBones, l_arrayBones );
				p_submesh->AddBoneDatas( l_arrayBones );
			}

			for ( uint32_t l_index = 0; l_index < p_aiMesh->mNumFaces; l_index++ )
			{
				aiFace const & l_face = p_aiMesh->mFaces[l_index];

				if ( l_face.mNumIndices == 3 )
				{
					p_submesh->AddFace( l_face.mIndices[0], l_face.mIndices[1], l_face.mIndices[2] );
				}
			}

			if ( !l_vertices.m_pNml )
			{
				p_submesh->ComputeNormals( true );
			}
			else if ( !l_vertices.m_pTan )
			{
				p_submesh->ComputeTangentsFromNormals();
			}

			l_return = true;
		}

		return l_return;
	}

	void AssimpImporter::DoLoadTexture( aiString const & p_name, Pass & p_pass, eTEXTURE_CHANNEL p_channel )
	{
		if ( p_name.length > 0 )
		{
			LoadTexture( string::string_cast< xchar >( p_name.C_Str() ), p_pass, p_channel );
		}
	}

	MaterialSPtr AssimpImporter::DoProcessMaterial( Scene & p_scene, aiMaterial const * p_pAiMaterial )
	{
		MaterialSPtr l_return;
		auto & l_manager = p_scene.GetMaterialView();
		aiString l_mtlname;
		p_pAiMaterial->Get( AI_MATKEY_NAME, l_mtlname );
		String l_name = string::string_cast< xchar >( l_mtlname.C_Str() );

		if ( l_name.empty() )
		{
			l_name = m_fileName.GetFileName() + string::to_string( m_anonymous++ );
		}

		if ( l_manager.Has( l_name ) )
		{
			l_return = l_manager.Find( l_name );
		}
		else
		{
			aiColor3D l_ambient( 1, 1, 1 );
			p_pAiMaterial->Get( AI_MATKEY_COLOR_AMBIENT, l_ambient );
			aiColor3D l_diffuse( 1, 1, 1 );
			p_pAiMaterial->Get( AI_MATKEY_COLOR_DIFFUSE, l_diffuse );
			aiColor3D l_specular( 1, 1, 1 );
			p_pAiMaterial->Get( AI_MATKEY_COLOR_SPECULAR, l_specular );
			aiColor3D l_emissive( 1, 1, 1 );
			p_pAiMaterial->Get( AI_MATKEY_COLOR_EMISSIVE, l_emissive );
			float l_opacity = 1;
			p_pAiMaterial->Get( AI_MATKEY_OPACITY, l_opacity );
			float l_shininess = 0.5f;
			p_pAiMaterial->Get( AI_MATKEY_SHININESS, l_shininess );
			float l_shininessStrength = 1.0f;
			p_pAiMaterial->Get( AI_MATKEY_SHININESS_STRENGTH, l_shininessStrength );
			int l_twoSided = 0;
			p_pAiMaterial->Get( AI_MATKEY_TWOSIDED, l_twoSided );

			aiString l_ambTexName;
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_AMBIENT, 0 ), l_ambTexName );
			aiString l_difTexName;
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0 ), l_difTexName );
			aiString l_spcTexName;
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0 ), l_spcTexName );
			aiString l_emiTexName;
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_EMISSIVE, 0 ), l_emiTexName );
			aiString l_nmlTexName;
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_NORMALS, 0 ), l_nmlTexName );
			aiString l_hgtTexName;
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_HEIGHT, 0 ), l_hgtTexName );
			aiString l_opaTexName;
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_OPACITY, 0 ), l_opaTexName );
			aiString l_shnTexName;
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_SHININESS, 0 ), l_shnTexName );

			if ( l_ambient.IsBlack() && l_diffuse.IsBlack() && l_specular.IsBlack() && l_emissive.IsBlack() )
			{
				l_diffuse.r = 1.0;
				l_diffuse.g = 1.0;
				l_diffuse.b = 1.0;
			}

			l_return = l_manager.Create( l_name, *GetEngine() );
			l_return->CreatePass();
			auto l_pass = l_return->GetPass( 0 );

			l_pass->SetAlpha( l_opacity );
			l_pass->SetTwoSided( l_twoSided != 0 );
			l_pass->SetAmbient( Colour::from_components( l_ambient.r, l_ambient.g, l_ambient.b, 1 ) );
			l_pass->SetDiffuse( Colour::from_components( l_diffuse.r, l_diffuse.g, l_diffuse.b, 1 ) );
			l_pass->SetSpecular( Colour::from_components( l_specular.r * l_shininessStrength, l_specular.g * l_shininessStrength, l_specular.b * l_shininessStrength, 1 ) );
			l_pass->SetEmissive( Colour::from_components( l_emissive.r, l_emissive.g, l_emissive.b, 1 ) );

			if ( l_shininess > 0 )
			{
				l_pass->SetShininess( l_shininess );
			}

			if ( l_difTexName.length > 0 && std::string( l_difTexName.C_Str() ).find( "_Cine_" ) != String::npos && std::string( l_difTexName.C_Str() ).find( "/MI_CH_" ) != String::npos )
			{
				// Workaround for Collada textures.
				String l_strGlob = string::string_cast< xchar >( l_difTexName.C_Str() ) + cuT( ".tga" );
				string::replace( l_strGlob, cuT( "/MI_CH_" ), cuT( "TX_CH_" ) );
				String l_strDiff = l_strGlob;
				String l_strNorm = l_strGlob;
				String l_strSpec = l_strGlob;
				String l_strOpac = l_strGlob;
				LoadTexture( string::replace( l_strDiff, cuT( "_Cine_" ), cuT( "_D_" ) ), *l_pass, eTEXTURE_CHANNEL_DIFFUSE );
				LoadTexture( string::replace( l_strNorm, cuT( "_Cine_" ), cuT( "_N_" ) ), *l_pass, eTEXTURE_CHANNEL_NORMAL );
				LoadTexture( string::replace( l_strSpec, cuT( "_Cine_" ), cuT( "_S_" ) ), *l_pass, eTEXTURE_CHANNEL_SPECULAR );
				LoadTexture( string::replace( l_strOpac, cuT( "_Cine_" ), cuT( "_A_" ) ), *l_pass, eTEXTURE_CHANNEL_OPACITY );
			}
			else
			{
				DoLoadTexture( l_ambTexName, *l_pass, eTEXTURE_CHANNEL_AMBIENT );
				DoLoadTexture( l_difTexName, *l_pass, eTEXTURE_CHANNEL_DIFFUSE );
				DoLoadTexture( l_spcTexName, *l_pass, eTEXTURE_CHANNEL_SPECULAR );
				DoLoadTexture( l_emiTexName, *l_pass, eTEXTURE_CHANNEL_EMISSIVE );
				DoLoadTexture( l_opaTexName, *l_pass, eTEXTURE_CHANNEL_OPACITY );
				DoLoadTexture( l_shnTexName, *l_pass, eTEXTURE_CHANNEL_GLOSS );

				if ( l_nmlTexName.length > 0 )
				{
					DoLoadTexture( l_nmlTexName, *l_pass, eTEXTURE_CHANNEL_NORMAL );

					//if ( l_hgtTexName.length > 0 )
					//{
					//	DoLoadTexture( l_hgtTexName, *l_pass, eTEXTURE_CHANNEL_HEIGHT );
					//}
				}
				else if ( l_hgtTexName.length > 0 )
				{
					DoLoadTexture( l_hgtTexName, *l_pass, eTEXTURE_CHANNEL_NORMAL );
				}
			}
		}

		return l_return;
	}

	void AssimpImporter::DoProcessBones( SkeletonSPtr p_skeleton, aiBone ** p_pBones, uint32_t p_count, std::vector< stVERTEX_BONE_DATA > & p_arrayVertices )
	{
		for ( uint32_t i = 0; i < p_count; ++i )
		{
			aiBone * l_aiBone = p_pBones[i];
			String l_name = string::string_cast< xchar >( l_aiBone->mName.C_Str() );
			uint32_t l_index;

			if ( m_mapBoneByID.find( l_name ) == m_mapBoneByID.end() )
			{
				BoneSPtr l_bone = std::make_shared< Bone >( *p_skeleton );
				l_bone->SetName( l_name );
				l_bone->SetOffsetMatrix( Matrix4x4r{ &l_aiBone->mOffsetMatrix.Transpose().a1 } );
				l_index = uint32_t( m_arrayBones.size() );
				m_arrayBones.push_back( l_bone );
				m_mapBoneByID[l_name] = l_index;
				p_skeleton->AddBone( l_bone );
			}
			else
			{
				l_index = m_mapBoneByID[l_name];
				ENSURE( m_arrayBones[l_index]->GetOffsetMatrix() == Matrix4x4r( &l_aiBone->mOffsetMatrix.Transpose().a1 ) );
			}

			for ( uint32_t j = 0; j < l_aiBone->mNumWeights; ++j )
			{
				p_arrayVertices[l_aiBone->mWeights[j].mVertexId].AddBoneData( l_index, real( l_aiBone->mWeights[j].mWeight ) );
			}
		}
	}

	AnimationSPtr AssimpImporter::DoProcessAnimation( String const & p_name, SkeletonSPtr p_skeleton, aiNode * p_node, aiAnimation * p_aiAnimation )
	{
		String l_name = string::string_cast< xchar >( p_aiAnimation->mName.C_Str() );

		if ( l_name.empty() )
		{
			l_name = p_name;
		}

		AnimationSPtr l_animation = p_skeleton->CreateAnimation( l_name );
		real l_ticksPerSecond = real( p_aiAnimation->mTicksPerSecond ? p_aiAnimation->mTicksPerSecond : 25.0_r );
		DoProcessAnimationNodes( l_animation, l_ticksPerSecond, p_skeleton, p_node, p_aiAnimation, nullptr );
		return l_animation;
	}

	void AssimpImporter::DoProcessAnimationNodes( AnimationSPtr p_animation, real p_ticksPerSecond, SkeletonSPtr p_skeleton, aiNode * p_aiNode, aiAnimation * p_aiAnimation, AnimationObjectSPtr p_object )
	{
		String l_name = string::string_cast< xchar >( p_aiNode->mName.data );
		const aiNodeAnim * l_aiNodeAnim = FindNodeAnim( p_aiAnimation, l_name );
		AnimationObjectSPtr l_object;

		if ( l_aiNodeAnim )
		{
			auto l_itBone = m_mapBoneByID.find( l_name );

			if ( l_itBone != m_mapBoneByID.end() )
			{
				auto l_bone = m_arrayBones[l_itBone->second];
				l_object = p_animation->AddObject( l_bone, p_object );

				if ( p_object->GetType() == eANIMATION_OBJECT_TYPE_BONE )
				{
					p_skeleton->SetBoneParent( l_bone, std::static_pointer_cast< SkeletonAnimationBone >( p_object )->GetBone() );
				}
			}
			else
			{
				l_object = p_animation->AddObject( p_aiNode->mName.C_Str(), p_object );
			}

			std::map< real, Point3r > l_translates;
			std::map< real, Point3r > l_scales;
			std::map< real, Quaternion > l_rotates;
			std::set< real > l_times;

			// We process translations
			for ( uint32_t i = 0; i < l_aiNodeAnim->mNumPositionKeys; ++i )
			{
				l_times.insert( real( l_aiNodeAnim->mPositionKeys[i].mTime ) );
				l_translates[real( l_aiNodeAnim->mPositionKeys[i].mTime )] = Point3r{ l_aiNodeAnim->mPositionKeys[i].mValue.x, l_aiNodeAnim->mPositionKeys[i].mValue.y, l_aiNodeAnim->mPositionKeys[i].mValue.z };
			}

			// Then we process scalings
			for ( uint32_t i = 0; i < l_aiNodeAnim->mNumScalingKeys; ++i )
			{
				l_times.insert( real( l_aiNodeAnim->mPositionKeys[i].mTime ) );
				l_scales[real( l_aiNodeAnim->mScalingKeys[i].mTime )] = Point3r{ l_aiNodeAnim->mScalingKeys[i].mValue.x, l_aiNodeAnim->mScalingKeys[i].mValue.y, l_aiNodeAnim->mScalingKeys[i].mValue.z };
			}

			// And eventually the rotations
			for ( uint32_t i = 0; i < l_aiNodeAnim->mNumRotationKeys; ++i )
			{
				l_times.insert( real( l_aiNodeAnim->mPositionKeys[i].mTime ) );
				Quaternion l_rotate;
				l_rotate.from_matrix( Matrix4x4r{ Matrix3x3r{ &l_aiNodeAnim->mRotationKeys[i].mValue.GetMatrix().Transpose().a1 } } );
				l_rotates[real( l_aiNodeAnim->mRotationKeys[i].mTime )] = l_rotate;
			}

			// We process translations
			KeyFrameRealMap l_keyframes;
			InterpolatorT< Point3r, eINTERPOLATOR_MODE_LINEAR > l_pointInterpolator;
			InterpolatorT< Quaternion, eINTERPOLATOR_MODE_LINEAR > l_quatInterpolator;

			for ( auto l_time : l_times )
			{
				Point3r l_translate = DoCompute( l_time, l_pointInterpolator, l_translates );
				Point3r l_scale = DoCompute( l_time, l_pointInterpolator, l_scales );
				Quaternion l_rotate = DoCompute( l_time, l_quatInterpolator, l_rotates );
				l_object->AddKeyFrame( l_time / p_ticksPerSecond, l_translate, l_rotate, l_scale );
			}
		}

		if ( !l_object )
		{
			l_object = p_animation->AddObject( p_aiNode->mName.C_Str(), p_object );
		}

		if ( l_object )
		{
			if ( p_object )
			{
				p_object->AddChild( l_object );
			}

			if ( !l_object->HasKeyFrames() )
			{
				l_object->SetNodeTransform( Matrix4x4r( &p_aiNode->mTransformation.a1 ) );
			}
		}

		for ( uint32_t i = 0; i < p_aiNode->mNumChildren; i++ )
		{
			DoProcessAnimationNodes( p_animation, p_ticksPerSecond, p_skeleton, p_aiNode->mChildren[i], p_aiAnimation, l_object );
		}
	}
}

//*************************************************************************************************
