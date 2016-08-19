#include "AssimpImporter.hpp"

#include <Design/ArrayView.hpp>

#include <GeometryCache.hpp>
#include <MaterialCache.hpp>
#include <MeshCache.hpp>
#include <PluginCache.hpp>
#include <SceneCache.hpp>
#include <SceneNodeCache.hpp>

#include <Animation/Mesh/MeshAnimation.hpp>
#include <Animation/Mesh/MeshAnimationSubmesh.hpp>
#include <Animation/Skeleton/SkeletonAnimation.hpp>
#include <Animation/Skeleton/SkeletonAnimationBone.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Cache/CacheView.hpp>
#include <Mesh/Skeleton/Bone.hpp>
#include <Plugin/ImporterPlugin.hpp>
#include <Render/RenderLoop.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>

#include <Log/Logger.hpp>

#include <assimp/version.h>

using namespace Castor3D;
using namespace Castor;

namespace C3dAssimp
{
	//*********************************************************************************************

	namespace
	{
		aiNodeAnim const * const FindNodeAnim( const aiAnimation & p_animation, const String & p_nodeName )
		{
			aiNodeAnim const * l_return = nullptr;

			for ( uint32_t i = 0; i < p_animation.mNumChannels && !l_return; ++i )
			{
				aiNodeAnim const * const l_nodeAnim = p_animation.mChannels[i];

				if ( string::string_cast< xchar >( l_nodeAnim->mNodeName.data ) == p_nodeName )
				{
					l_return = l_nodeAnim;
				}
			}

			return l_return;
		}

		aiMeshAnim const * const FindMeshAnim( const aiAnimation & p_animation, const String & p_meshName )
		{
			aiMeshAnim const * l_return = nullptr;

			for ( uint32_t i = 0; i < p_animation.mNumMeshChannels && !l_return; ++i )
			{
				aiMeshAnim const * const l_meshAnim = p_animation.mMeshChannels[i];

				if ( string::string_cast< xchar >( l_meshAnim->mName.data ) == p_meshName )
				{
					l_return = l_meshAnim;
				}
			}

			return l_return;
		}

		template< typename aiMeshType >
		InterleavedVertexArray DoCreateVertexBuffer( aiMeshType const & p_aiMesh )
		{
			InterleavedVertexArray l_vertices{ p_aiMesh.mNumVertices };
			uint32_t l_index{ 0u };

			for ( auto & l_vertex : l_vertices )
			{
				l_vertex.m_pos[0] = real( p_aiMesh.mVertices[l_index].x );
				l_vertex.m_pos[1] = real( p_aiMesh.mVertices[l_index].y );
				l_vertex.m_pos[2] = real( p_aiMesh.mVertices[l_index].z );
				++l_index;
			}

			if ( p_aiMesh.HasNormals() )
			{
				l_index = 0u;

				for ( auto & l_vertex : l_vertices )
				{
					l_vertex.m_nml[0] = real( p_aiMesh.mNormals[l_index].x );
					l_vertex.m_nml[1] = real( p_aiMesh.mNormals[l_index].y );
					l_vertex.m_nml[2] = real( p_aiMesh.mNormals[l_index].z );
					++l_index;
				}
			}

			if ( p_aiMesh.HasTangentsAndBitangents() )
			{
				l_index = 0u;

				for ( auto & l_vertex : l_vertices )
				{
					l_vertex.m_tan[0] = real( p_aiMesh.mTangents[l_index].x );
					l_vertex.m_tan[1] = real( p_aiMesh.mTangents[l_index].y );
					l_vertex.m_tan[2] = real( p_aiMesh.mTangents[l_index].z );
					++l_index;
				}

				l_index = 0u;

				for ( auto & l_vertex : l_vertices )
				{
					l_vertex.m_bin[0] = real( p_aiMesh.mBitangents[l_index].x );
					l_vertex.m_bin[1] = real( p_aiMesh.mBitangents[l_index].y );
					l_vertex.m_bin[2] = real( p_aiMesh.mBitangents[l_index].z );
					++l_index;
				}
			}

			if ( p_aiMesh.HasTextureCoords( 0 ) )
			{
				l_index = 0u;

				for ( auto & l_vertex : l_vertices )
				{
					l_vertex.m_tex[0] = real( p_aiMesh.mTextureCoords[0][l_index].x );
					l_vertex.m_tex[1] = real( p_aiMesh.mTextureCoords[0][l_index].y );
					l_vertex.m_tex[2] = real( p_aiMesh.mTextureCoords[0][l_index].z );
					++l_index;
				}
			}

			return l_vertices;
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

		void DoProcessPassBaseComponents( Pass & p_pass, aiMaterial const & p_aiMaterial )
		{
			aiColor3D l_ambient( 1, 1, 1 );
			p_aiMaterial.Get( AI_MATKEY_COLOR_AMBIENT, l_ambient );
			aiColor3D l_diffuse( 1, 1, 1 );
			p_aiMaterial.Get( AI_MATKEY_COLOR_DIFFUSE, l_diffuse );
			aiColor3D l_specular( 1, 1, 1 );
			p_aiMaterial.Get( AI_MATKEY_COLOR_SPECULAR, l_specular );
			aiColor3D l_emissive( 1, 1, 1 );
			p_aiMaterial.Get( AI_MATKEY_COLOR_EMISSIVE, l_emissive );
			float l_opacity = 1;
			p_aiMaterial.Get( AI_MATKEY_OPACITY, l_opacity );
			float l_shininess = 0.5f;
			p_aiMaterial.Get( AI_MATKEY_SHININESS, l_shininess );
			float l_shininessStrength = 1.0f;
			p_aiMaterial.Get( AI_MATKEY_SHININESS_STRENGTH, l_shininessStrength );
			int l_twoSided = 0;
			p_aiMaterial.Get( AI_MATKEY_TWOSIDED, l_twoSided );

			if ( l_ambient.IsBlack() && l_diffuse.IsBlack() && l_specular.IsBlack() && l_emissive.IsBlack() )
			{
				l_diffuse.r = 1.0;
				l_diffuse.g = 1.0;
				l_diffuse.b = 1.0;
			}

			p_pass.SetAlpha( l_opacity );
			p_pass.SetTwoSided( l_twoSided != 0 );
			p_pass.SetAmbient( Colour::from_components( l_ambient.r, l_ambient.g, l_ambient.b, 1 ) );
			p_pass.SetDiffuse( Colour::from_components( l_diffuse.r, l_diffuse.g, l_diffuse.b, 1 ) );
			p_pass.SetSpecular( Colour::from_components( l_specular.r * l_shininessStrength, l_specular.g * l_shininessStrength, l_specular.b * l_shininessStrength, 1 ) );
			p_pass.SetEmissive( Colour::from_components( l_emissive.r, l_emissive.g, l_emissive.b, 1 ) );

			if ( l_shininess > 0 )
			{
				p_pass.SetShininess( l_shininess );
			}
		}

		void DoLoadTexture( aiString const & p_name, Pass & p_pass, TextureChannel p_channel, Importer const & p_importer )
		{
			if ( p_name.length > 0 )
			{
				p_importer.LoadTexture( Path{ string::string_cast< xchar >( p_name.C_Str() ) }, p_pass, p_channel );
			}
		}

		void DoProcessPassTextures( Pass & p_pass, aiMaterial const & p_aiMaterial, Importer const & p_importer )
		{
			aiString l_ambTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_AMBIENT, 0 ), l_ambTexName );
			aiString l_difTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0 ), l_difTexName );
			aiString l_spcTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0 ), l_spcTexName );
			aiString l_emiTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_EMISSIVE, 0 ), l_emiTexName );
			aiString l_nmlTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_NORMALS, 0 ), l_nmlTexName );
			aiString l_hgtTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_HEIGHT, 0 ), l_hgtTexName );
			aiString l_opaTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_OPACITY, 0 ), l_opaTexName );
			aiString l_shnTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_SHININESS, 0 ), l_shnTexName );

			if ( l_difTexName.length > 0 && std::string( l_difTexName.C_Str() ).find( "_Cine_" ) != String::npos && std::string( l_difTexName.C_Str() ).find( "/MI_CH_" ) != String::npos )
			{
				// Workaround for Collada textures.
				String l_strGlob = string::string_cast< xchar >( l_difTexName.C_Str() ) + cuT( ".tga" );
				string::replace( l_strGlob, cuT( "/MI_CH_" ), cuT( "TX_CH_" ) );
				String l_strDiff = l_strGlob;
				String l_strNorm = l_strGlob;
				String l_strSpec = l_strGlob;
				String l_strOpac = l_strGlob;
				p_importer.LoadTexture( Path{ string::replace( l_strDiff, cuT( "_Cine_" ), cuT( "_D_" ) ) }, p_pass, TextureChannel::Diffuse );
				p_importer.LoadTexture( Path{ string::replace( l_strNorm, cuT( "_Cine_" ), cuT( "_N_" ) ) }, p_pass, TextureChannel::Normal );
				p_importer.LoadTexture( Path{ string::replace( l_strSpec, cuT( "_Cine_" ), cuT( "_S_" ) ) }, p_pass, TextureChannel::Specular );
				p_importer.LoadTexture( Path{ string::replace( l_strOpac, cuT( "_Cine_" ), cuT( "_A_" ) ) }, p_pass, TextureChannel::Opacity );
			}
			else
			{
				DoLoadTexture( l_ambTexName, p_pass, TextureChannel::Ambient, p_importer );
				DoLoadTexture( l_difTexName, p_pass, TextureChannel::Diffuse, p_importer );
				DoLoadTexture( l_spcTexName, p_pass, TextureChannel::Specular, p_importer );
				DoLoadTexture( l_emiTexName, p_pass, TextureChannel::Emissive, p_importer );
				DoLoadTexture( l_opaTexName, p_pass, TextureChannel::Opacity, p_importer );
				DoLoadTexture( l_shnTexName, p_pass, TextureChannel::Gloss, p_importer );

				if ( l_nmlTexName.length > 0 )
				{
					DoLoadTexture( l_nmlTexName, p_pass, TextureChannel::Normal, p_importer );

					if ( l_hgtTexName.length > 0 )
					{
						DoLoadTexture( l_hgtTexName, p_pass, TextureChannel::Height, p_importer );
					}
				}
				else if ( l_hgtTexName.length > 0 )
				{
					DoLoadTexture( l_hgtTexName, p_pass, TextureChannel::Normal, p_importer );
				}
			}
		}
	}

	//*********************************************************************************************

	AssimpImporter::AssimpImporter( Engine & p_engine )
		: Importer( p_engine )
		, m_anonymous( 0 )
	{
	}

	AssimpImporter::~AssimpImporter()
	{
	}

	ImporterUPtr AssimpImporter::Create( Engine & p_engine )
	{
		return std::make_unique< AssimpImporter >( p_engine );
	}

	bool AssimpImporter::DoImportScene( Scene & p_scene )
	{
		auto l_mesh = p_scene.GetMeshCache().Add( cuT( "Mesh_PLY" ) );
		bool l_return = DoImportMesh( *l_mesh );

		if ( l_return )
		{
			SceneNodeSPtr l_node = p_scene.GetSceneNodeCache().Add( l_mesh->GetName(), p_scene.GetObjectRootNode() );
			GeometrySPtr l_geometry = p_scene.GetGeometryCache().Add( l_mesh->GetName(), l_node, nullptr );
			l_geometry->SetMesh( l_mesh );
			m_geometries.insert( { l_geometry->GetName(), l_geometry } );
		}

		return l_return;
	}

	bool AssimpImporter::DoImportMesh( Mesh & p_mesh )
	{
		bool l_return{ false };
		m_mapBoneByID.clear();
		m_arrayBones.clear();
		SubmeshSPtr l_submesh;
		Assimp::Importer l_importer;
		uint32_t l_flags = aiProcess_Triangulate
						   | aiProcess_JoinIdenticalVertices
						   | aiProcess_OptimizeMeshes
						   | aiProcess_OptimizeGraph
						   | aiProcess_FixInfacingNormals
						   | aiProcess_LimitBoneWeights
						   | aiProcess_Debone;
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
			SkeletonSPtr l_skeleton = std::make_shared< Skeleton >( *p_mesh.GetScene() );
			l_skeleton->SetGlobalInverseTransform( Matrix4x4r( &l_aiScene->mRootNode->mTransformation.Transpose().Inverse().a1 ) );

			if ( l_aiScene->HasMeshes() )
			{
				bool l_create = true;

				for ( uint32_t i = 0; i < l_aiScene->mNumMeshes; ++i )
				{
					if ( l_create )
					{
						l_submesh = p_mesh.CreateSubmesh();
					}

					l_create = DoProcessMesh( *p_mesh.GetScene(), p_mesh, *l_skeleton, *l_aiScene->mMeshes[i], *l_aiScene, *l_submesh );
				}

				if ( m_arrayBones.empty() )
				{
					l_skeleton.reset();
				}
				else
				{
					p_mesh.SetSkeleton( l_skeleton );
				}

				if ( l_skeleton )
				{
					for ( uint32_t i = 0; i < l_aiScene->mNumAnimations; ++i )
					{
						DoProcessAnimation( m_fileName.GetFileName(), *l_skeleton, *l_aiScene->mRootNode, *l_aiScene->mAnimations[i] );
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
										DoProcessAnimation( l_file.GetFileName(), *l_skeleton, *l_scene->mRootNode, *l_scene->mAnimations[i] );
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

				l_return = true;
			}
			else
			{
				l_importer.FreeScene();
			}
		}
		else
		{
			// The import failed, report it
			Logger::LogError( std::stringstream() << "Scene import failed : " << l_importer.GetErrorString() );
		}

		return l_return;
	}

	bool AssimpImporter::DoProcessMesh( Scene & p_scene, Mesh & p_mesh, Skeleton & p_skeleton, aiMesh const & p_aiMesh, aiScene const & p_aiScene, Submesh & p_submesh )
	{
		bool l_return = false;
		MaterialSPtr l_material;

		if ( p_aiMesh.mMaterialIndex < p_aiScene.mNumMaterials )
		{
			l_material = DoProcessMaterial( p_scene, *p_aiScene.mMaterials[p_aiMesh.mMaterialIndex] );
		}

		if ( p_aiMesh.HasFaces() && p_aiMesh.HasPositions() && l_material )
		{
			p_submesh.SetDefaultMaterial( l_material );
			p_submesh.Ref( l_material );
			p_submesh.AddPoints( DoCreateVertexBuffer( p_aiMesh ) );

			std::vector< VertexBoneData > l_arrayBones( p_aiMesh.mNumVertices );

			if ( p_aiMesh.HasBones() )
			{
				DoProcessBones( p_skeleton, p_aiMesh.mBones, p_aiMesh.mNumBones, l_arrayBones );
				p_submesh.AddBoneDatas( l_arrayBones );
			}

			for ( uint32_t l_index = 0; l_index < p_aiMesh.mNumFaces; l_index++ )
			{
				aiFace const & l_face = p_aiMesh.mFaces[l_index];

				if ( l_face.mNumIndices == 3 )
				{
					p_submesh.AddFace( l_face.mIndices[0], l_face.mIndices[1], l_face.mIndices[2] );
				}
			}

			if ( !p_aiMesh.mNormals )
			{
				p_submesh.ComputeNormals( true );
			}
			else if ( !p_aiMesh.mTangents )
			{
				p_submesh.ComputeTangentsFromNormals();
			}

			if ( p_aiScene.HasAnimations() )
			{
				std::for_each( p_aiScene.mAnimations, p_aiScene.mAnimations + p_aiScene.mNumAnimations, [this, &p_aiMesh, &p_mesh, &p_submesh]( aiAnimation const * p_aiAnimation )
				{
					auto l_it = std::find_if( p_aiAnimation->mMeshChannels, p_aiAnimation->mMeshChannels + p_aiAnimation->mNumMeshChannels, [this, &p_aiMesh, &p_submesh]( aiMeshAnim const * p_aiMeshAnim )
					{
						return p_aiMeshAnim->mName == p_aiMesh.mName;
					} );

					if ( l_it != p_aiAnimation->mMeshChannels + p_aiAnimation->mNumMeshChannels )
					{
						DoProcessAnimationMeshes( p_mesh, p_submesh, p_aiMesh, *( *l_it ) );
					}
				} );
			}

			l_return = true;
		}

		return l_return;
	}

	void AssimpImporter::DoProcessAnimationMeshes( Mesh & p_mesh, Submesh & p_submesh, aiMesh const & p_aiMesh, aiMeshAnim const & p_aiMeshAnim )
	{
		if ( p_aiMeshAnim.mNumKeys )
		{
			String l_name{ string::string_cast< xchar >( p_aiMeshAnim.mName.C_Str() ) };
			Logger::LogDebug( cuT( "Mesh animation found: " ) + l_name );
			auto & l_animation = p_mesh.CreateAnimation( l_name );
			MeshAnimationSubmesh l_animSubmesh{ l_animation, p_submesh };

			std::for_each( p_aiMeshAnim.mKeys, p_aiMeshAnim.mKeys + p_aiMeshAnim.mNumKeys, [&l_animSubmesh, &p_aiMesh]( aiMeshKey const & p_aiKey )
			{
				l_animSubmesh.AddBuffer( real( p_aiKey.mTime ), DoCreateVertexBuffer( *p_aiMesh.mAnimMeshes[p_aiKey.mValue] ) );
			} );

			l_animation.AddChild( std::move( l_animSubmesh ) );
		}
	}

	MaterialSPtr AssimpImporter::DoProcessMaterial( Scene & p_scene, aiMaterial const & p_aiMaterial )
	{
		MaterialSPtr l_return;
		auto & l_cache = p_scene.GetMaterialView();
		aiString l_mtlname;
		p_aiMaterial.Get( AI_MATKEY_NAME, l_mtlname );
		String l_name = string::string_cast< xchar >( l_mtlname.C_Str() );

		if ( l_name.empty() )
		{
			l_name = m_fileName.GetFileName() + string::to_string( m_anonymous++ );
		}

		if ( l_cache.Has( l_name ) )
		{
			l_return = l_cache.Find( l_name );
		}
		else
		{
			l_return = l_cache.Add( l_name );
			l_return->CreatePass();
			auto l_pass = l_return->GetPass( 0 );

			DoProcessPassBaseComponents( *l_pass, p_aiMaterial );
			DoProcessPassTextures( *l_pass, p_aiMaterial, *this );
		}

		return l_return;
	}

	void AssimpImporter::DoProcessBones( Skeleton & p_skeleton, aiBone const * const * p_aiBones, uint32_t p_count, std::vector< VertexBoneData > & p_arrayVertices )
	{
		for ( uint32_t i = 0; i < p_count; ++i )
		{
			aiBone const & l_aiBone = *p_aiBones[i];
			String l_name = string::string_cast< xchar >( l_aiBone.mName.C_Str() );
			uint32_t l_index;

			if ( m_mapBoneByID.find( l_name ) == m_mapBoneByID.end() )
			{
				BoneSPtr l_bone = std::make_shared< Bone >( p_skeleton );
				l_bone->SetName( l_name );
				aiMatrix4x4 l_mtx{ l_aiBone.mOffsetMatrix };
				l_bone->SetOffsetMatrix( Matrix4x4r{ &l_mtx.Transpose().a1 } );
				l_index = uint32_t( m_arrayBones.size() );
				m_arrayBones.push_back( l_bone );
				m_mapBoneByID[l_name] = l_index;
				p_skeleton.AddBone( l_bone );
			}
			else
			{
				l_index = m_mapBoneByID[l_name];
				aiMatrix4x4 l_mtx{ l_aiBone.mOffsetMatrix };
				ENSURE( m_arrayBones[l_index]->GetOffsetMatrix() == Matrix4x4r( &l_mtx.Transpose().a1 ) );
			}

			for ( uint32_t j = 0; j < l_aiBone.mNumWeights; ++j )
			{
				p_arrayVertices[l_aiBone.mWeights[j].mVertexId].AddBoneData( l_index, real( l_aiBone.mWeights[j].mWeight ) );
			}
		}
	}

	void AssimpImporter::DoProcessAnimation( String const & p_name, Skeleton & p_skeleton, aiNode const & p_aiNode, aiAnimation const & p_aiAnimation )
	{
		String l_name{ string::string_cast< xchar >( p_aiAnimation.mName.C_Str() ) };
		Logger::LogDebug( cuT( "Skeleton animation found: " ) + l_name );

		if ( l_name.empty() )
		{
			l_name = p_name;
		}

		auto & l_animation = p_skeleton.CreateAnimation( l_name );
		real l_ticksPerSecond = real( p_aiAnimation.mTicksPerSecond ? p_aiAnimation.mTicksPerSecond : 25.0_r );
		DoProcessAnimationNodes( l_animation, l_ticksPerSecond, p_skeleton, p_aiNode, p_aiAnimation, nullptr );
		l_animation.UpdateLength();
	}

	void AssimpImporter::DoProcessAnimationNodes( SkeletonAnimation & p_animation, real p_ticksPerSecond, Skeleton & p_skeleton, aiNode const & p_aiNode, aiAnimation const & p_aiAnimation, SkeletonAnimationObjectSPtr p_object )
	{
		String l_name = string::string_cast< xchar >( p_aiNode.mName.data );
		const aiNodeAnim * l_aiNodeAnim = FindNodeAnim( p_aiAnimation, l_name );
		SkeletonAnimationObjectSPtr l_object;

		if ( l_aiNodeAnim )
		{
			auto l_itBone = m_mapBoneByID.find( l_name );

			if ( l_itBone != m_mapBoneByID.end() )
			{
				auto l_bone = m_arrayBones[l_itBone->second];
				l_object = p_animation.AddObject( l_bone, p_object );

				if ( p_object->GetType() == SkeletonAnimationObjectType::Bone )
				{
					p_skeleton.SetBoneParent( l_bone, std::static_pointer_cast< SkeletonAnimationBone >( p_object )->GetBone() );
				}
			}
			else
			{
				l_object = p_animation.AddObject( p_aiNode.mName.C_Str(), p_object );
			}

			std::map< real, Point3r > l_translates;
			std::map< real, Point3r > l_scales;
			std::map< real, Quaternion > l_rotates;
			std::set< real > l_times;

			// We process translations
			for ( auto const & l_translate : ArrayView< aiVectorKey >( l_aiNodeAnim->mPositionKeys, l_aiNodeAnim->mNumPositionKeys ) )
			{
				l_times.insert( real( l_translate.mTime / p_ticksPerSecond ) );
				l_translates[real( l_translate.mTime / p_ticksPerSecond )] = Point3r{ l_translate.mValue.x, l_translate.mValue.y, l_translate.mValue.z };
			}

			// Then we process scalings
			for ( auto const & l_scale : ArrayView< aiVectorKey >( l_aiNodeAnim->mScalingKeys, l_aiNodeAnim->mNumScalingKeys ) )
			{
				l_times.insert( real( l_scale.mTime / p_ticksPerSecond ) );
				l_scales[real( l_scale.mTime / p_ticksPerSecond )] = Point3r{ l_scale.mValue.x, l_scale.mValue.y, l_scale.mValue.z };
			}

			// And eventually the rotations
			for ( auto const & l_rot : ArrayView< aiQuatKey >( l_aiNodeAnim->mRotationKeys, l_aiNodeAnim->mNumRotationKeys ) )
			{
				l_times.insert( real( l_rot.mTime / p_ticksPerSecond ) );
				Quaternion l_rotate;
				l_rotate.from_matrix( Matrix4x4r{ Matrix3x3r{ &l_rot.mValue.GetMatrix().Transpose().a1 } } );
				l_rotates[real( l_rot.mTime / p_ticksPerSecond )] = l_rotate;
			}

			// We synchronise the three arrays
			KeyFrameRealMap l_keyframes;
			InterpolatorT< Point3r, InterpolatorType::Linear > l_pointInterpolator;
			InterpolatorT< Quaternion, InterpolatorType::Linear > l_quatInterpolator;

			if ( p_ticksPerSecond >= GetEngine()->GetRenderLoop().GetWantedFps() )
			{
				for ( auto l_time : l_times )
				{
					Point3r l_translate = DoCompute( l_time, l_pointInterpolator, l_translates );
					Point3r l_scale = DoCompute( l_time, l_pointInterpolator, l_scales );
					Quaternion l_rotate = DoCompute( l_time, l_quatInterpolator, l_rotates );
					l_object->AddKeyFrame( l_time, l_translate, l_rotate, l_scale );
				}
			}
			else
			{
				// Limit the key frames per second to 60, to spare RAM...
				real l_step{ 1.0_r / std::min( 60.0_r, real( GetEngine()->GetRenderLoop().GetWantedFps() ) ) };
				real l_maxTime{ *l_times.rbegin() + l_step };

				for ( real l_time{ 0.0_r }; l_time < l_maxTime; l_time += l_step )
				{
					Point3r l_translate = DoCompute( l_time, l_pointInterpolator, l_translates );
					Point3r l_scale = DoCompute( l_time, l_pointInterpolator, l_scales );
					Quaternion l_rotate = DoCompute( l_time, l_quatInterpolator, l_rotates );
					l_object->AddKeyFrame( l_time, l_translate, l_rotate, l_scale );
				}
			}
		}

		if ( !l_object )
		{
			l_object = p_animation.AddObject( p_aiNode.mName.C_Str(), p_object );
		}

		if ( l_object )
		{
			if ( p_object )
			{
				p_object->AddChild( l_object );
			}

			if ( !l_object->HasKeyFrames() )
			{
				l_object->SetNodeTransform( Matrix4x4r( &p_aiNode.mTransformation.a1 ) );
			}
		}

		for ( uint32_t i = 0; i < p_aiNode.mNumChildren; i++ )
		{
			DoProcessAnimationNodes( p_animation, p_ticksPerSecond, p_skeleton, *p_aiNode.mChildren[i], p_aiAnimation, l_object );
		}
	}

	//*********************************************************************************************
}
