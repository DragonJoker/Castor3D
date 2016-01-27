#include "AssimpImporter.hpp"

#include <InitialiseEvent.hpp>
#include <ImporterPlugin.hpp>
#include <MaterialManager.hpp>
#include <Bone.hpp>
#include <MeshManager.hpp>
#include <SceneManager.hpp>
#include <MovingObjectBase.hpp>

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

C3D_Assimp_API ImporterPlugin::ExtensionArray GetExtensions()
{
	ImporterPlugin::ExtensionArray l_extensions;
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "DAE" ), cuT( "Collada" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "BLEND" ), cuT( "Blender" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "BVH" ), cuT( "3 Biovision BVH" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "3DS" ), cuT( "3D Studio Max 3DS" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "ASE" ), cuT( "3D Studio Max ASE" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "OBJ" ), cuT( "Wavefront Object" ) ) );
	//l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "PLY" ), cuT( "Stanford Polygon Library" ) ) ); // Crashes on big meshes.
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "MD2" ), cuT( "Quake II" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "MD3" ), cuT( "Quake III" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "LWO" ), cuT( "LightWave Model" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "IFC" ), cuT( "IFC-STEP, Industry Foundation Classes" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "NFF" ), cuT( "Sense8 WorldToolkit" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "SMD" ), cuT( "Valve Model" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "VTA" ), cuT( "Valve Model" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "MDL" ), cuT( "3 Quake I" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "PK3" ), cuT( "Quake 3 BSP" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "MDC" ), cuT( "1 RtCW" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "MD5MESH" ), cuT( "Doom 3" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "XML" ), cuT( "Ogre XML Mesh" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "X" ), cuT( "DirectX X" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "Q3O" ), cuT( "Quick3D" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "Q3S" ), cuT( "Quick3D" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "RAW" ), cuT( "Raw Triangles" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "AC" ), cuT( "AC3D" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "STL" ), cuT( "Stereolithography" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "DXF" ), cuT( "Autodesk DXF" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "IRRMESH" ), cuT( "Irrlicht Mesh" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "IRR" ), cuT( "Irrlicht Scene" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "OFF" ), cuT( "Object File Format" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "TER" ), cuT( "Terragen Terrain" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "MDL" ), cuT( "3D GameStudio Model" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "HMP" ), cuT( "3D GameStudio Terrain" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "MS3D" ), cuT( "3 Milkshape 3D" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "LWS" ), cuT( "LightWave Scene" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "LXO" ), cuT( "Modo Model" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "CSM" ), cuT( "CharacterStudio Motion" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "COB" ), cuT( "TrueSpace" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "SCN" ), cuT( "TrueSpace" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "XGL" ), cuT( "2 XGL" ) ) );
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "ZGL" ), cuT( "2 XGL" ) ) );
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
	namespace detail
	{
		const aiNodeAnim * FindNodeAnim( const aiAnimation* p_animation, const String p_nodeName )
		{
			const aiNodeAnim * l_return = NULL;

			for ( uint32_t i = 0; i < p_animation->mNumChannels && !l_return; ++i )
			{
				const aiNodeAnim * l_pNodeAnim = p_animation->mChannels[i];

				if ( string::string_cast< xchar >( l_pNodeAnim->mNodeName.data ) == p_nodeName )
				{
					l_return = l_pNodeAnim;
				}
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
		DoImportMesh();
		SceneSPtr l_scene;

		if ( m_mesh )
		{
			m_mesh->GenerateBuffers();
			l_scene = GetOwner()->GetSceneManager().Create( cuT( "Scene_ASSIMP" ), *GetOwner(), cuT( "Scene_ASSIMP" ) );
			SceneNodeSPtr l_node = l_scene->CreateSceneNode( m_mesh->GetName(), l_scene->GetObjectRootNode() );
			GeometrySPtr l_pGeometry = l_scene->CreateGeometry( m_mesh->GetName() );
			l_pGeometry->AttachTo( l_node );
			l_pGeometry->SetMesh( m_mesh );
			m_mesh.reset();
		}

		return l_scene;
	}

	MeshSPtr AssimpImporter::DoImportMesh()
	{
		m_mapBoneByID.clear();
		m_arrayBones.clear();
		String l_name = m_fileName.GetFileName();
		String l_meshName = l_name.substr( 0, l_name.find_last_of( '.' ) );

		if ( GetOwner()->GetMeshManager().Has( l_meshName ) )
		{
			m_mesh = GetOwner()->GetMeshManager().Find( l_meshName );
		}
		else
		{
			m_mesh = GetOwner()->GetMeshManager().Create( l_meshName, eMESH_TYPE_CUSTOM, UIntArray(), RealArray() );
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
				m_mesh->SetSkeleton( l_skeleton );

				if ( l_aiScene->HasMeshes() )
				{
					bool l_create = true;

					for ( uint32_t i = 0; i < l_aiScene->mNumMeshes; ++i )
					{
						if ( l_create )
						{
							l_submesh = m_mesh->CreateSubmesh();
						}

						l_create = DoProcessMesh( l_skeleton, l_aiScene->mMeshes[i], l_aiScene, l_submesh );
					}

					if ( l_skeleton->begin() == l_skeleton->end() )
					{
						m_mesh->SetSkeleton( nullptr );
						l_skeleton.reset();
					}

					if ( l_skeleton )
					{
						if ( l_aiScene->HasAnimations() )
						{
							for ( uint32_t i = 0; i < l_aiScene->mNumAnimations; ++i )
							{
								DoProcessAnimation( m_fileName.GetFileName(), l_skeleton, l_aiScene->mRootNode, l_aiScene->mAnimations[i] );
							}
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
					GetOwner()->GetMeshManager().Remove( l_meshName );
					m_mesh.reset();
					l_importer.FreeScene();
				}
			}
			else
			{
				// The import failed, report it
				Logger::LogError( std::stringstream() << "Scene import failed : " << l_importer.GetErrorString() );
				GetOwner()->GetMeshManager().Remove( l_meshName );
				m_mesh.reset();
			}
		}

		MeshSPtr l_return( m_mesh );
		m_mesh.reset();
		return l_return;
	}


	bool AssimpImporter::DoProcessMesh( SkeletonSPtr p_skeleton, aiMesh const * p_aiMesh, aiScene const * p_aiScene, SubmeshSPtr p_submesh )
	{
		bool l_return = false;
		MaterialSPtr l_material;

		if ( p_aiMesh->mMaterialIndex < p_aiScene->mNumMaterials )
		{
			l_material = DoProcessMaterial( p_aiScene->mMaterials[p_aiMesh->mMaterialIndex] );
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

			std::vector< stVERTEX_BONE_DATA > l_arrayBones( p_aiMesh->mNumVertices );

			if ( p_aiMesh->HasBones() && p_skeleton )
			{
				DoProcessBones( p_skeleton, p_aiMesh->mBones, p_aiMesh->mNumBones, l_arrayBones );
				l_vertices.m_pBones = &l_arrayBones[0];
			}

			p_submesh->AddPoints( l_vertices );

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

	MaterialSPtr AssimpImporter::DoProcessMaterial( aiMaterial const * p_pAiMaterial )
	{
		MaterialSPtr l_return;
		PassSPtr l_pass;
		MaterialManager & l_mtlManager = GetOwner()->GetMaterialManager();
		aiString l_mtlname;
		p_pAiMaterial->Get( AI_MATKEY_NAME, l_mtlname );
		String l_name = string::string_cast< xchar >( l_mtlname.C_Str() );
		aiColor3D l_ambient( 1, 1, 1 );
		aiColor3D l_diffuse( 1, 1, 1 );
		aiColor3D l_specular( 1, 1, 1 );
		aiColor3D l_emissive( 1, 1, 1 );
		aiString l_ambTexName;
		aiString l_difTexName;
		aiString l_hgtTexName;
		aiString l_nmlTexName;
		aiString l_opaTexName;
		aiString l_shnTexName;
		aiString l_spcTexName;

		if ( l_name.empty() )
		{
			l_name = m_fileName.GetFileName() + string::to_string( m_anonymous++ );
		}

		if ( l_mtlManager.Has( l_name ) )
		{
			l_return = l_mtlManager.Find( l_name );
		}
		else
		{
			float l_opacity = 1;
			float l_shininess = 0.5f;
			float l_shininessStrength = 1.0f;
			int l_twoSided = 0;
			l_return = l_mtlManager.Create( l_name, *GetOwner(), l_name );
			l_return->CreatePass();
			l_pass = l_return->GetPass( 0 );
			p_pAiMaterial->Get( AI_MATKEY_COLOR_AMBIENT, l_ambient );
			p_pAiMaterial->Get( AI_MATKEY_COLOR_DIFFUSE, l_diffuse );
			p_pAiMaterial->Get( AI_MATKEY_COLOR_SPECULAR, l_specular );
			p_pAiMaterial->Get( AI_MATKEY_COLOR_EMISSIVE, l_emissive );
			p_pAiMaterial->Get( AI_MATKEY_OPACITY, l_opacity );
			p_pAiMaterial->Get( AI_MATKEY_SHININESS, l_shininess );
			p_pAiMaterial->Get( AI_MATKEY_SHININESS_STRENGTH, l_shininessStrength );
			p_pAiMaterial->Get( AI_MATKEY_TWOSIDED, l_twoSided );
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_AMBIENT, 0 ), l_ambTexName );
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0 ), l_difTexName );
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_NORMALS, 0 ), l_nmlTexName );
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_HEIGHT, 0 ), l_hgtTexName );
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_OPACITY, 0 ), l_opaTexName );
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_SHININESS, 0 ), l_shnTexName );
			p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0 ), l_spcTexName );

			if ( l_ambient.IsBlack() && l_diffuse.IsBlack() && l_specular.IsBlack() && l_emissive.IsBlack() )
			{
				l_diffuse.r = 1.0;
				l_diffuse.g = 1.0;
				l_diffuse.b = 1.0;
			}

			l_pass->SetAmbient( Colour::from_components( l_ambient.r, l_ambient.g, l_ambient.b, 1 ) );
			l_pass->SetDiffuse( Colour::from_components( l_diffuse.r, l_diffuse.g, l_diffuse.b, 1 ) );
			l_pass->SetSpecular( Colour::from_components( l_specular.r * l_shininessStrength, l_specular.g * l_shininessStrength, l_specular.b * l_shininessStrength, 1 ) );
			l_pass->SetEmissive( Colour::from_components( l_emissive.r, l_emissive.g, l_emissive.b, 1 ) );

			if ( l_shininess > 0 )
			{
				l_pass->SetShininess( l_shininess );
			}

			l_pass->SetAlpha( l_opacity );
			l_pass->SetTwoSided( l_twoSided != 0 );

			if ( l_difTexName.length > 0 && std::string( l_difTexName.C_Str() ).find( "_Cine_" ) != String::npos && std::string( l_difTexName.C_Str() ).find( "/MI_CH_" ) != String::npos )
			{
				String l_strGlob = string::string_cast< xchar >( l_difTexName.C_Str() ) + cuT( ".tga" );
				string::replace( l_strGlob, cuT( "/MI_CH_" ), cuT( "TX_CH_" ) );
				String l_strDiff = l_strGlob;
				String l_strNorm = l_strGlob;
				String l_strSpec = l_strGlob;
				String l_strOpac = l_strGlob;
				DoAddTexture( string::replace( l_strDiff, cuT( "_Cine_" ), cuT( "_D_" ) ), l_pass, eTEXTURE_CHANNEL_DIFFUSE );
				DoAddTexture( string::replace( l_strNorm, cuT( "_Cine_" ), cuT( "_N_" ) ), l_pass, eTEXTURE_CHANNEL_NORMAL );
				DoAddTexture( string::replace( l_strSpec, cuT( "_Cine_" ), cuT( "_S_" ) ), l_pass, eTEXTURE_CHANNEL_SPECULAR );
				DoAddTexture( string::replace( l_strOpac, cuT( "_Cine_" ), cuT( "_A_" ) ), l_pass, eTEXTURE_CHANNEL_OPACITY );
			}
			else
			{
				if ( l_ambTexName.length > 0 )
				{
					DoAddTexture( string::string_cast< xchar >( l_ambTexName.C_Str() ), l_pass, eTEXTURE_CHANNEL_AMBIENT );
				}

				if ( l_difTexName.length > 0 )
				{
					DoAddTexture( string::string_cast< xchar >( l_difTexName.C_Str() ), l_pass, eTEXTURE_CHANNEL_DIFFUSE );
				}

				if ( l_opaTexName.length > 0 )
				{
					DoAddTexture( string::string_cast< xchar >( l_opaTexName.C_Str() ), l_pass, eTEXTURE_CHANNEL_OPACITY );
				}

				if ( l_shnTexName.length > 0 )
				{
					DoAddTexture( string::string_cast< xchar >( l_shnTexName.C_Str() ), l_pass, eTEXTURE_CHANNEL_GLOSS );
				}

				if ( l_spcTexName.length > 0 )
				{
					DoAddTexture( string::string_cast< xchar >( l_spcTexName.C_Str() ), l_pass, eTEXTURE_CHANNEL_SPECULAR );
				}

				if ( l_nmlTexName.length > 0 )
				{
					DoAddTexture( string::string_cast< xchar >( l_nmlTexName.C_Str() ), l_pass, eTEXTURE_CHANNEL_NORMAL );

					if ( l_hgtTexName.length > 0 )
					{
						DoAddTexture( string::string_cast< xchar >( l_hgtTexName.C_Str() ), l_pass, eTEXTURE_CHANNEL_HEIGHT );
					}
				}
				else if ( l_hgtTexName.length > 0 )
				{
					DoAddTexture( string::string_cast< xchar >( l_hgtTexName.C_Str() ), l_pass, eTEXTURE_CHANNEL_NORMAL );
				}
			}
		}

		return l_return;
	}

	void AssimpImporter::DoProcessBones( SkeletonSPtr p_pSkeleton, aiBone ** p_pBones, uint32_t p_count, std::vector< stVERTEX_BONE_DATA > & p_arrayVertices )
	{
		for ( uint32_t i = 0; i < p_count; ++i )
		{
			aiBone * l_aiBone = p_pBones[i];
			String l_name = string::string_cast< xchar >( l_aiBone->mName.C_Str() );
			uint32_t l_index;

			if ( m_mapBoneByID.find( l_name ) == m_mapBoneByID.end() )
			{
				BoneSPtr l_bone = std::make_shared< Bone >( *p_pSkeleton );
				l_bone->SetName( l_name );
				l_bone->SetOffsetMatrix( Matrix4x4r( &l_aiBone->mOffsetMatrix.Transpose().a1 ) );
				l_index = uint32_t( m_arrayBones.size() );
				m_arrayBones.push_back( l_bone );
				m_mapBoneByID[l_name] = l_index;
				p_pSkeleton->AddBone( l_bone );
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

	void AssimpImporter::DoProcessAnimationNodes( AnimationSPtr p_animation, real p_ticksPerSecond, SkeletonSPtr p_skeleton, aiNode * p_aiNode, aiAnimation * p_aiAnimation, MovingObjectBaseSPtr p_object )
	{
		String l_name = string::string_cast< xchar >( p_aiNode->mName.data );
		const aiNodeAnim * l_aiNodeAnim = detail::FindNodeAnim( p_aiAnimation, l_name );
		MovingObjectBaseSPtr l_object;

		if ( l_aiNodeAnim )
		{
			auto l_itBone = std::find_if( p_skeleton->begin(), p_skeleton->end(), [&]( BoneSPtr p_bone )
			{
				return p_bone->GetName() == l_name;
			} );

			if ( l_itBone != p_skeleton->end() )
			{
				l_object = p_animation->AddMovingObject( *l_itBone, p_object );
			}
			else
			{
				l_object = p_animation->AddMovingObject( p_aiNode->mName.C_Str(), p_object );
			}

			// We treat translations
			for ( uint32_t i = 0; i < l_aiNodeAnim->mNumPositionKeys; ++i )
			{
				Point3r l_translate( l_aiNodeAnim->mPositionKeys[i].mValue.x, l_aiNodeAnim->mPositionKeys[i].mValue.y, l_aiNodeAnim->mPositionKeys[i].mValue.z );
				l_object->AddTranslateKeyFrame( real( l_aiNodeAnim->mPositionKeys[i].mTime ) / p_ticksPerSecond, l_translate );
			}

			// Then we treat scalings
			for ( uint32_t i = 0; i < l_aiNodeAnim->mNumScalingKeys; ++i )
			{
				Point3r l_scale( l_aiNodeAnim->mScalingKeys[i].mValue.x, l_aiNodeAnim->mScalingKeys[i].mValue.y, l_aiNodeAnim->mScalingKeys[i].mValue.z );
				l_object->AddScaleKeyFrame( real( l_aiNodeAnim->mScalingKeys[i].mTime ) / p_ticksPerSecond, l_scale );
			}

			// And eventually the rotations
			for ( uint32_t i = 0; i < l_aiNodeAnim->mNumRotationKeys; ++i )
			{
				Quaternion l_rotate;
				l_rotate.from_matrix( Matrix3x3r( &l_aiNodeAnim->mRotationKeys[i].mValue.GetMatrix().Transpose().a1 ) );
				l_object->AddRotateKeyFrame( real( l_aiNodeAnim->mRotationKeys[i].mTime ) / p_ticksPerSecond, l_rotate );
			}
		}

		if ( !l_object )
		{
			l_object = p_animation->AddMovingObject( p_aiNode->mName.C_Str(), p_object );
		}

		if ( p_object )
		{
			p_object->AddChild( l_object );
		}

		if ( !l_object->HasKeyFrames() )
		{
			l_object->SetNodeTransform( Matrix4x4r( &p_aiNode->mTransformation.a1 ) );
		}

		for ( uint32_t i = 0; i < p_aiNode->mNumChildren; i++ )
		{
			DoProcessAnimationNodes( p_animation, p_ticksPerSecond, p_skeleton, p_aiNode->mChildren[i], p_aiAnimation, l_object );
		}
	}

	void AssimpImporter::DoAddTexture( String const & p_path, PassSPtr p_pass, eTEXTURE_CHANNEL p_channel )
	{
		if ( p_pass )
		{
			TextureUnitSPtr l_pTexture;
			Path l_pathImage = m_filePath / p_path;

			if ( !File::FileExists( l_pathImage ) )
			{
				l_pathImage = m_filePath / cuT( "Texture" ) / p_path;
			}

			if ( File::FileExists( l_pathImage ) )
			{
				l_pTexture = p_pass->AddTextureUnit();

				try
				{
					l_pTexture->SetAutoMipmaps( true );

					if ( !l_pTexture->LoadTexture( l_pathImage ) )
					{
						p_pass->DestroyTextureUnit( l_pTexture->GetIndex() - 1 );
						l_pTexture.reset();
					}
					else
					{
						l_pTexture->SetChannel( p_channel );
					}
				}
				catch ( ... )
				{
					p_pass->DestroyTextureUnit( l_pTexture->GetIndex() - 1 );
					l_pTexture.reset();
					Logger::LogWarning( cuT( "Error encountered while loading texture file " ) + p_path );
				}
			}
			else
			{
				Logger::LogWarning( cuT( "Couldn't load texture file " ) + p_path );
			}
		}
	}
}

//*************************************************************************************************
