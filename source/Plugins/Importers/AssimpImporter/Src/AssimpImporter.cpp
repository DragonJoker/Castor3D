#include "AssimpImporter.hpp"

#include <Design/ArrayView.hpp>

#include <Cache/GeometryCache.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/PluginCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/SceneNodeCache.hpp>

#include <Animation/Mesh/MeshAnimation.hpp>
#include <Animation/Mesh/MeshAnimationSubmesh.hpp>
#include <Animation/Skeleton/SkeletonAnimation.hpp>
#include <Animation/Skeleton/SkeletonAnimationBone.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Cache/CacheView.hpp>
#include <Material/LegacyPass.hpp>
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
		aiNodeAnim const * const FindNodeAnim( const aiAnimation & p_animation
			, const String & p_nodeName )
		{
			aiNodeAnim const * l_return = nullptr;
			auto l_it = std::find_if( p_animation.mChannels
				, p_animation.mChannels + p_animation.mNumChannels
				, [&p_nodeName]( aiNodeAnim const * const p_nodeAnim )
			{
				return string::string_cast< xchar >( p_nodeAnim->mNodeName.data ) == p_nodeName;
			} );

			if ( l_it != p_animation.mChannels + p_animation.mNumChannels )
			{
				l_return = *l_it;
			}

			return l_return;
		}

		aiMeshAnim const * const FindMeshAnim( const aiAnimation & p_animation
			, const String & p_meshName )
		{
			aiMeshAnim const * l_return = nullptr;
			auto l_it = std::find_if( p_animation.mMeshChannels
				, p_animation.mMeshChannels + p_animation.mNumMeshChannels
				, [&p_meshName]( aiMeshAnim const * const p_meshAnim )
				{
					return string::string_cast< xchar >( p_meshAnim->mName.data ) == p_meshName;
				} );

			if ( l_it != p_animation.mMeshChannels + p_animation.mNumMeshChannels )
			{
				l_return = *l_it;
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
		void DoFind( std::chrono::milliseconds p_time
			, typename std::map< std::chrono::milliseconds, T > const & p_map
			, typename std::map< std::chrono::milliseconds, T >::const_iterator & p_prv
			, typename std::map< std::chrono::milliseconds, T >::const_iterator & p_cur )
		{
			if ( p_map.empty() )
			{
				p_prv = p_cur = p_map.end();
			}
			else
			{
				p_cur = std::find_if( p_map.begin()
					, p_map.end()
					, [&p_time]( std::pair< std::chrono::milliseconds, T > const & p_pair )
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
		T DoCompute( std::chrono::milliseconds const & p_from
			, Interpolator< T > const & p_interpolator
			, std::map< std::chrono::milliseconds, T > const & p_values )
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
				auto l_dt = l_cur->first - l_prv->first;
				real l_factor = ( p_from - l_prv->first ).count() / real( l_dt.count() );
				l_return = p_interpolator.Interpolate( l_prv->second, l_cur->second, l_factor );
			}

			return l_return;
		}

		void DoProcessPassBaseComponents( LegacyPass & p_pass
			, aiMaterial const & p_aiMaterial )
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

			if ( l_ambient.IsBlack()
				&& l_diffuse.IsBlack()
				&& l_specular.IsBlack()
				&& l_emissive.IsBlack() )
			{
				l_diffuse.r = 1.0;
				l_diffuse.g = 1.0;
				l_diffuse.b = 1.0;
			}

			p_pass.SetOpacity( l_opacity );
			p_pass.SetTwoSided( l_twoSided != 0 );
			p_pass.SetDiffuse( Colour::from_components( l_diffuse.r
				, l_diffuse.g
				, l_diffuse.b
				, 1 ) );
			p_pass.SetSpecular( Colour::from_components( l_specular.r * l_shininessStrength
				, l_specular.g * l_shininessStrength
				, l_specular.b * l_shininessStrength
				, 1 ) );
			p_pass.SetEmissive( float( point::length( Point3f{ l_emissive.r
				, l_emissive.g
				, l_emissive.b } ) ) );

			if ( l_shininess > 0 )
			{
				p_pass.SetShininess( l_shininess );
			}
		}

		void DoLoadTexture( aiString const & p_name
			, Pass & p_pass
			, TextureChannel p_channel
			, Importer const & p_importer )
		{
			if ( p_name.length > 0 )
			{
				p_importer.LoadTexture( Path{ string::string_cast< xchar >( p_name.C_Str() ) }
					, p_pass
					, p_channel );
			}
		}

		void DoProcessPassTextures( Pass & p_pass
			, aiMaterial const & p_aiMaterial
			, Importer const & p_importer )
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

			if ( l_difTexName.length > 0
				&& std::string( l_difTexName.C_Str() ).find( "_Cine_" ) != String::npos
				&& std::string( l_difTexName.C_Str() ).find( "/MI_CH_" ) != String::npos )
			{
				// Workaround for Collada textures.
				String l_strGlob = string::string_cast< xchar >( l_difTexName.C_Str() ) + cuT( ".tga" );
				string::replace( l_strGlob, cuT( "/MI_CH_" ), cuT( "TX_CH_" ) );
				String l_strDiff = l_strGlob;
				String l_strNorm = l_strGlob;
				String l_strSpec = l_strGlob;
				String l_strOpac = l_strGlob;
				p_importer.LoadTexture( Path{ string::replace( l_strDiff, cuT( "_Cine_" ), cuT( "_D_" ) ) }
					, p_pass
					, TextureChannel::eDiffuse );
				p_importer.LoadTexture( Path{ string::replace( l_strNorm, cuT( "_Cine_" ), cuT( "_N_" ) ) }
					, p_pass
					, TextureChannel::eNormal );
				p_importer.LoadTexture( Path{ string::replace( l_strSpec, cuT( "_Cine_" ), cuT( "_S_" ) ) }
					, p_pass
					, TextureChannel::eSpecular );
				p_importer.LoadTexture( Path{ string::replace( l_strOpac, cuT( "_Cine_" ), cuT( "_A_" ) ) }
					, p_pass
					, TextureChannel::eOpacity );
			}
			else
			{
				DoLoadTexture( l_difTexName, p_pass, TextureChannel::eDiffuse, p_importer );
				DoLoadTexture( l_spcTexName, p_pass, TextureChannel::eSpecular, p_importer );
				DoLoadTexture( l_emiTexName, p_pass, TextureChannel::eEmissive, p_importer );
				DoLoadTexture( l_opaTexName, p_pass, TextureChannel::eOpacity, p_importer );
				DoLoadTexture( l_shnTexName, p_pass, TextureChannel::eGloss, p_importer );

				if ( l_nmlTexName.length > 0 )
				{
					DoLoadTexture( l_nmlTexName, p_pass, TextureChannel::eNormal, p_importer );

					if ( l_hgtTexName.length > 0 )
					{
						DoLoadTexture( l_hgtTexName, p_pass, TextureChannel::eHeight, p_importer );
					}
				}
				else if ( l_hgtTexName.length > 0 )
				{
					DoLoadTexture( l_hgtTexName, p_pass, TextureChannel::eNormal, p_importer );
				}
			}
		}

		std::map< std::chrono::milliseconds, Point3r > DoProcessVec3Keys( aiVectorKey const * const p_keys
			, uint32_t p_count
			, int64_t p_ticksPerMilliSecond
			, std::set< std::chrono::milliseconds > & p_times )
		{
			std::map< std::chrono::milliseconds, Point3r > l_result;

			for ( auto const & l_key : make_array_view( p_keys, p_count ) )
			{
				auto l_time = std::chrono::milliseconds{ int64_t( l_key.mTime * 1000 ) } / p_ticksPerMilliSecond;
				p_times.insert( l_time );
				l_result[l_time] = Point3r{ l_key.mValue.x, l_key.mValue.y, l_key.mValue.z };
			}

			return l_result;
		}

		std::map< std::chrono::milliseconds, Quaternion > DoProcessQuatKeys( aiQuatKey const * const p_keys
			, uint32_t p_count
			, int64_t p_ticksPerMilliSecond
			, std::set< std::chrono::milliseconds > & p_times )
		{
			std::map< std::chrono::milliseconds, Quaternion > l_result;

			for ( auto const & l_key : make_array_view( p_keys, p_count ) )
			{
				auto l_time = std::chrono::milliseconds{ int64_t( l_key.mTime * 1000 ) } / p_ticksPerMilliSecond;
				p_times.insert( l_time );
				l_result[l_time] = Quaternion::from_matrix( Matrix4x4r{ Matrix3x3r{ &l_key.mValue.GetMatrix().Transpose().a1 } } );
			}

			return l_result;
		}

		void DoSynchroniseKeys( std::map< std::chrono::milliseconds, Point3r > const & p_translates
			, std::map< std::chrono::milliseconds, Point3r > const & p_scales
			, std::map< std::chrono::milliseconds, Quaternion > const & p_rotates
			, std::set< std::chrono::milliseconds > const & p_times
			, uint32_t p_fps
			, int64_t p_ticksPerMilliSecond
			, SkeletonAnimationObject & p_object )
		{
			InterpolatorT< Point3r, InterpolatorType::eLinear > l_pointInterpolator;
			InterpolatorT< Quaternion, InterpolatorType::eLinear > l_quatInterpolator;

			if ( p_ticksPerMilliSecond / 1000 >= p_fps )
			{
				for ( auto l_time : p_times )
				{
					Point3r l_translate = DoCompute( l_time, l_pointInterpolator, p_translates );
					Point3r l_scale = DoCompute( l_time, l_pointInterpolator, p_scales );
					Quaternion l_rotate = DoCompute( l_time, l_quatInterpolator, p_rotates );
					p_object.AddKeyFrame( l_time, l_translate, l_rotate, l_scale );
				}
			}
			else
			{
				// Limit the key frames per second to 60, to spare RAM...
				std::chrono::milliseconds l_step{ 1000 / std::min< int64_t >( 60, int64_t( p_fps ) ) };
				std::chrono::milliseconds l_maxTime{ *p_times.rbegin() + l_step };

				for ( std::chrono::milliseconds l_time{ 0 }; l_time < l_maxTime; l_time += l_step )
				{
					Point3r l_translate = DoCompute( l_time, l_pointInterpolator, p_translates );
					Point3r l_scale = DoCompute( l_time, l_pointInterpolator, p_scales );
					Quaternion l_rotate = DoCompute( l_time, l_quatInterpolator, p_rotates );
					p_object.AddKeyFrame( l_time, l_translate, l_rotate, l_scale );
				}
			}
		}
	}

	//*********************************************************************************************

	Castor::String const AssimpImporter::Name = cuT( "ASSIMP Importer" );

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

				for ( auto l_aiMesh : make_array_view( l_aiScene->mMeshes, l_aiScene->mNumMeshes ) )
				{
					if ( l_create )
					{
						l_submesh = p_mesh.CreateSubmesh();
					}

					l_create = DoProcessMesh( *p_mesh.GetScene(), p_mesh, *l_skeleton, *l_aiMesh, *l_aiScene, *l_submesh );
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
					for ( auto l_aiAnimation : make_array_view( l_aiScene->mAnimations, l_aiScene->mNumAnimations ) )
					{
						DoProcessAnimation( p_mesh
							, m_fileName.GetFileName()
							, *l_skeleton
							, *l_aiScene->mRootNode
							, *l_aiAnimation );
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

									for ( auto l_aiAnimation : make_array_view( l_scene->mAnimations, l_scene->mNumAnimations ) )
									{
										DoProcessAnimation( p_mesh
											, l_file.GetFileName()
											, *l_skeleton
											, *l_scene->mRootNode
											, *l_aiAnimation );
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

			if ( p_aiMesh.HasBones() )
			{
				std::vector< VertexBoneData > l_arrayBones( p_aiMesh.mNumVertices );
				DoProcessBones( p_skeleton, p_aiMesh.mBones, p_aiMesh.mNumBones, l_arrayBones );
				p_submesh.AddBoneDatas( l_arrayBones );
			}

			for ( auto l_face : make_array_view( p_aiMesh.mFaces, p_aiMesh.mNumFaces ) )
			{
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
				for( auto l_aiAnimation : make_array_view( p_aiScene.mAnimations, p_aiScene.mNumAnimations ) )
				{
					auto l_it = std::find_if( l_aiAnimation->mMeshChannels
						, l_aiAnimation->mMeshChannels + l_aiAnimation->mNumMeshChannels
						, [this, &p_aiMesh, &p_submesh]( aiMeshAnim const * p_aiMeshAnim )
						{
							return p_aiMeshAnim->mName == p_aiMesh.mName;
						} );

					if ( l_it != l_aiAnimation->mMeshChannels + l_aiAnimation->mNumMeshChannels )
					{
						DoProcessAnimationMeshes( p_mesh, p_submesh, p_aiMesh, *( *l_it ) );
					}
				}
			}

			l_return = true;
		}

		return l_return;
	}

	void AssimpImporter::DoProcessAnimationMeshes( Mesh & p_mesh
		, Submesh & p_submesh
		, aiMesh const & p_aiMesh
		, aiMeshAnim const & p_aiMeshAnim )
	{
		if ( p_aiMeshAnim.mNumKeys )
		{
			String l_name{ string::string_cast< xchar >( p_aiMeshAnim.mName.C_Str() ) };
			Logger::LogDebug( cuT( "Mesh animation found: " ) + l_name );
			auto & l_animation = p_mesh.CreateAnimation( l_name );
			MeshAnimationSubmesh l_animSubmesh{ l_animation, p_submesh };

			for ( auto l_aiKey : make_array_view( p_aiMeshAnim.mKeys, p_aiMeshAnim.mNumKeys ) )
			{
				l_animSubmesh.AddBuffer( std::chrono::milliseconds{ int64_t( l_aiKey.mTime * 1000 ) }
					, DoCreateVertexBuffer( *p_aiMesh.mAnimMeshes[l_aiKey.mValue] ) );
			}

			l_animation.AddChild( std::move( l_animSubmesh ) );
		}
	}

	MaterialSPtr AssimpImporter::DoProcessMaterial( Scene & p_scene
		, aiMaterial const & p_aiMaterial )
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
			REQUIRE( l_return->GetType() == MaterialType::eLegacy );
		}
		else
		{
			l_return = l_cache.Add( l_name, MaterialType::eLegacy );
			l_return->CreatePass();
			auto l_pass = l_return->GetTypedPass< MaterialType::eLegacy >( 0 );

			DoProcessPassBaseComponents( *l_pass, p_aiMaterial );
			DoProcessPassTextures( *l_pass, p_aiMaterial, *this );
		}

		return l_return;
	}

	BoneSPtr AssimpImporter::DoAddBone( String const & p_name
		, Matrix4x4r const & p_offset
		, Skeleton & p_skeleton
		, uint32_t & p_index )
	{
		BoneSPtr l_bone = p_skeleton.CreateBone( p_name );
		l_bone->SetOffsetMatrix( p_offset );
		p_index = uint32_t( m_arrayBones.size() );
		m_arrayBones.push_back( l_bone );
		m_mapBoneByID[p_name] = p_index;
		return l_bone;
	}

	void AssimpImporter::DoProcessBones( Skeleton & p_skeleton
		, aiBone const * const * p_aiBones
		, uint32_t p_count
		, std::vector< VertexBoneData > & p_arrayVertices )
	{
		for ( uint32_t i = 0; i < p_count; ++i )
		{
			aiBone const & l_aiBone = *p_aiBones[i];
			String l_name = string::string_cast< xchar >( l_aiBone.mName.C_Str() );
			uint32_t l_index;

			if ( m_mapBoneByID.find( l_name ) == m_mapBoneByID.end() )
			{
				auto l_mtx = l_aiBone.mOffsetMatrix;
				DoAddBone( l_name, Matrix4x4r{ &l_mtx.Transpose().a1 }, p_skeleton, l_index );
			}
			else
			{
				l_index = m_mapBoneByID[l_name];
				aiMatrix4x4 l_mtx{ l_aiBone.mOffsetMatrix };
				ENSURE( m_arrayBones[l_index]->GetOffsetMatrix() == Matrix4x4r( &l_mtx.Transpose().a1 ) );
			}

			for ( auto l_weight : make_array_view( l_aiBone.mWeights, l_aiBone.mNumWeights ) )
			{
				p_arrayVertices[l_weight.mVertexId].AddBoneData( l_index, real( l_weight.mWeight ) );
			}
		}
	}

	void AssimpImporter::DoProcessAnimation( Mesh & p_mesh
		, String const & p_name
		, Skeleton & p_skeleton
		, aiNode const & p_aiNode
		, aiAnimation const & p_aiAnimation )
	{
		String l_name{ string::string_cast< xchar >( p_aiAnimation.mName.C_Str() ) };
		Logger::LogDebug( cuT( "Skeleton animation found: " ) + l_name );

		if ( l_name.empty() )
		{
			l_name = p_name;
		}

		auto & l_animation = p_skeleton.CreateAnimation( l_name );
		int64_t l_ticksPerMilliSecond = int64_t( p_aiAnimation.mTicksPerSecond ? p_aiAnimation.mTicksPerSecond : 25 );
		DoProcessAnimationNodes( p_mesh
			, l_animation
			, l_ticksPerMilliSecond
			, p_skeleton
			, p_aiNode
			, p_aiAnimation
			, nullptr );
		l_animation.UpdateLength();
	}

	void AssimpImporter::DoProcessAnimationNodes( Mesh & p_mesh
		, SkeletonAnimation & p_animation
		, int64_t p_ticksPerMilliSecond
		, Skeleton & p_skeleton
		, aiNode const & p_aiNode
		, aiAnimation const & p_aiAnimation
		, SkeletonAnimationObjectSPtr p_object )
	{
		String l_name = string::string_cast< xchar >( p_aiNode.mName.data );
		const aiNodeAnim * l_aiNodeAnim = FindNodeAnim( p_aiAnimation, l_name );
		SkeletonAnimationObjectSPtr l_object;
		auto l_itBone = m_mapBoneByID.find( l_name );

		if ( !l_aiNodeAnim
			&& l_itBone == m_mapBoneByID.end()
			&& p_aiNode.mNumMeshes )
		{
			uint32_t l_index;
			auto l_bone = DoAddBone( l_name, Matrix4x4r{ 1.0_r }, p_skeleton, l_index );

			for ( auto const & l_mesh : make_array_view( p_aiNode.mMeshes, p_aiNode.mNumMeshes ) )
			{
				auto l_submesh = p_mesh.GetSubmesh( l_mesh );
				REQUIRE( l_submesh != nullptr );

				if ( !l_submesh->HasBoneData() )
				{
					std::vector< VertexBoneData > l_arrayBones( l_submesh->GetPointsCount() );

					for ( auto & l_boneData : l_arrayBones )
					{
						l_boneData.AddBoneData( l_index, 1.0_r );
					}

					l_submesh->AddBoneDatas( l_arrayBones );
				}
			}

			l_itBone = m_mapBoneByID.find( l_name );
		}

		if ( l_itBone != m_mapBoneByID.end() )
		{
			auto l_bone = m_arrayBones[l_itBone->second];
			l_object = p_animation.AddObject( l_bone, p_object );

			if ( p_object->GetType() == SkeletonAnimationObjectType::eBone )
			{
				p_skeleton.SetBoneParent( l_bone, std::static_pointer_cast< SkeletonAnimationBone >( p_object )->GetBone() );
			}
		}
		else
		{
			l_object = p_animation.AddObject( p_aiNode.mName.C_Str(), p_object );
		}

		if ( l_aiNodeAnim )
		{
			DoProcessAnimationNodeKeys( *l_aiNodeAnim, p_ticksPerMilliSecond, *l_object );
		}

		if ( l_object )
		{
			if ( p_object && l_object != p_object )
			{
				p_object->AddChild( l_object );
			}

			if ( !l_object->HasKeyFrames() )
			{
				l_object->SetNodeTransform( Matrix4x4r( &p_aiNode.mTransformation.a1 ).get_transposed() );
			}
		}

		for ( auto l_node : make_array_view( p_aiNode.mChildren, p_aiNode.mNumChildren ) )
		{
			DoProcessAnimationNodes( p_mesh
				, p_animation
				, p_ticksPerMilliSecond
				, p_skeleton
				, *l_node
				, p_aiAnimation
				, l_object );
		}
	}

	void AssimpImporter::DoProcessAnimationNodeKeys( aiNodeAnim const & p_aiNodeAnim
		, int64_t p_ticksPerMilliSecond
		, Castor3D::SkeletonAnimationObject & p_object )
	{
		std::set< std::chrono::milliseconds > l_times;

		auto l_translates = DoProcessVec3Keys( p_aiNodeAnim.mPositionKeys
			, p_aiNodeAnim.mNumPositionKeys
			, p_ticksPerMilliSecond
			, l_times );
		auto l_scales = DoProcessVec3Keys( p_aiNodeAnim.mScalingKeys
			, p_aiNodeAnim.mNumScalingKeys
			, p_ticksPerMilliSecond
			, l_times );
		auto l_rotates = DoProcessQuatKeys( p_aiNodeAnim.mRotationKeys
			, p_aiNodeAnim.mNumRotationKeys
			, p_ticksPerMilliSecond
			, l_times );
		DoSynchroniseKeys( l_translates
			, l_scales
			, l_rotates
			, l_times
			, GetEngine()->GetRenderLoop().GetWantedFps()
			, p_ticksPerMilliSecond
			, p_object );
	}

	//*********************************************************************************************
}
