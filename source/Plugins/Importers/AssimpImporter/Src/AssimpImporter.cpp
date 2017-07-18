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
			aiNodeAnim const * result = nullptr;
			auto it = std::find_if( p_animation.mChannels
				, p_animation.mChannels + p_animation.mNumChannels
				, [&p_nodeName]( aiNodeAnim const * const p_nodeAnim )
			{
				return string::string_cast< xchar >( p_nodeAnim->mNodeName.data ) == p_nodeName;
			} );

			if ( it != p_animation.mChannels + p_animation.mNumChannels )
			{
				result = *it;
			}

			return result;
		}

		aiMeshAnim const * const FindMeshAnim( const aiAnimation & p_animation
			, const String & p_meshName )
		{
			aiMeshAnim const * result = nullptr;
			auto it = std::find_if( p_animation.mMeshChannels
				, p_animation.mMeshChannels + p_animation.mNumMeshChannels
				, [&p_meshName]( aiMeshAnim const * const p_meshAnim )
				{
					return string::string_cast< xchar >( p_meshAnim->mName.data ) == p_meshName;
				} );

			if ( it != p_animation.mMeshChannels + p_animation.mNumMeshChannels )
			{
				result = *it;
			}

			return result;
		}

		template< typename aiMeshType >
		InterleavedVertexArray DoCreateVertexBuffer( aiMeshType const & p_aiMesh )
		{
			InterleavedVertexArray vertices{ p_aiMesh.mNumVertices };
			uint32_t index{ 0u };

			for ( auto & vertex : vertices )
			{
				vertex.m_pos[0] = real( p_aiMesh.mVertices[index].x );
				vertex.m_pos[1] = real( p_aiMesh.mVertices[index].y );
				vertex.m_pos[2] = real( p_aiMesh.mVertices[index].z );
				++index;
			}

			if ( p_aiMesh.HasNormals() )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.m_nml[0] = real( p_aiMesh.mNormals[index].x );
					vertex.m_nml[1] = real( p_aiMesh.mNormals[index].y );
					vertex.m_nml[2] = real( p_aiMesh.mNormals[index].z );
					++index;
				}
			}

			if ( p_aiMesh.HasTangentsAndBitangents() )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.m_tan[0] = real( p_aiMesh.mTangents[index].x );
					vertex.m_tan[1] = real( p_aiMesh.mTangents[index].y );
					vertex.m_tan[2] = real( p_aiMesh.mTangents[index].z );
					++index;
				}

				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.m_bin[0] = real( p_aiMesh.mBitangents[index].x );
					vertex.m_bin[1] = real( p_aiMesh.mBitangents[index].y );
					vertex.m_bin[2] = real( p_aiMesh.mBitangents[index].z );
					++index;
				}
			}

			if ( p_aiMesh.HasTextureCoords( 0 ) )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.m_tex[0] = real( p_aiMesh.mTextureCoords[0][index].x );
					vertex.m_tex[1] = real( p_aiMesh.mTextureCoords[0][index].y );
					vertex.m_tex[2] = real( p_aiMesh.mTextureCoords[0][index].z );
					++index;
				}
			}

			return vertices;
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
			T result;

			if ( p_values.size() == 1 )
			{
				result = p_values.begin()->second;
			}
			else
			{
				auto prv = p_values.begin();
				auto cur = p_values.begin();
				DoFind( p_from, p_values, prv, cur );
				auto dt = cur->first - prv->first;
				real factor = ( p_from - prv->first ).count() / real( dt.count() );
				result = p_interpolator.Interpolate( prv->second, cur->second, factor );
			}

			return result;
		}

		void DoProcessPassBaseComponents( LegacyPass & p_pass
			, aiMaterial const & p_aiMaterial )
		{
			aiColor3D ambient( 1, 1, 1 );
			p_aiMaterial.Get( AI_MATKEY_COLOR_AMBIENT, ambient );
			aiColor3D diffuse( 1, 1, 1 );
			p_aiMaterial.Get( AI_MATKEY_COLOR_DIFFUSE, diffuse );
			aiColor3D specular( 1, 1, 1 );
			p_aiMaterial.Get( AI_MATKEY_COLOR_SPECULAR, specular );
			aiColor3D emissive( 1, 1, 1 );
			p_aiMaterial.Get( AI_MATKEY_COLOR_EMISSIVE, emissive );
			float opacity = 1;
			p_aiMaterial.Get( AI_MATKEY_OPACITY, opacity );
			float shininess = 0.5f;
			p_aiMaterial.Get( AI_MATKEY_SHININESS, shininess );
			float shininessStrength = 1.0f;
			p_aiMaterial.Get( AI_MATKEY_SHININESS_STRENGTH, shininessStrength );
			int twoSided = 0;
			p_aiMaterial.Get( AI_MATKEY_TWOSIDED, twoSided );

			if ( ambient.IsBlack()
				&& diffuse.IsBlack()
				&& specular.IsBlack()
				&& emissive.IsBlack() )
			{
				diffuse.r = 1.0;
				diffuse.g = 1.0;
				diffuse.b = 1.0;
			}

			p_pass.SetOpacity( opacity );
			p_pass.SetTwoSided( twoSided != 0 );
			p_pass.SetDiffuse( Colour::from_components( diffuse.r
				, diffuse.g
				, diffuse.b
				, 1 ) );
			p_pass.SetSpecular( Colour::from_components( specular.r * shininessStrength
				, specular.g * shininessStrength
				, specular.b * shininessStrength
				, 1 ) );
			p_pass.SetEmissive( float( point::length( Point3f{ emissive.r
				, emissive.g
				, emissive.b } ) ) );

			if ( shininess > 0 )
			{
				p_pass.SetShininess( shininess );
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
			aiString ambTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_AMBIENT, 0 ), ambTexName );
			aiString difTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0 ), difTexName );
			aiString spcTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0 ), spcTexName );
			aiString emiTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_EMISSIVE, 0 ), emiTexName );
			aiString nmlTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_NORMALS, 0 ), nmlTexName );
			aiString hgtTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_HEIGHT, 0 ), hgtTexName );
			aiString opaTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_OPACITY, 0 ), opaTexName );
			aiString shnTexName;
			p_aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_SHININESS, 0 ), shnTexName );

			if ( difTexName.length > 0
				&& std::string( difTexName.C_Str() ).find( "_Cine_" ) != String::npos
				&& std::string( difTexName.C_Str() ).find( "/MI_CH_" ) != String::npos )
			{
				// Workaround for Collada textures.
				String strGlob = string::string_cast< xchar >( difTexName.C_Str() ) + cuT( ".tga" );
				string::replace( strGlob, cuT( "/MI_CH_" ), cuT( "TX_CH_" ) );
				String strDiff = strGlob;
				String strNorm = strGlob;
				String strSpec = strGlob;
				String strOpac = strGlob;
				p_importer.LoadTexture( Path{ string::replace( strDiff, cuT( "_Cine_" ), cuT( "_D_" ) ) }
					, p_pass
					, TextureChannel::eDiffuse );
				p_importer.LoadTexture( Path{ string::replace( strNorm, cuT( "_Cine_" ), cuT( "_N_" ) ) }
					, p_pass
					, TextureChannel::eNormal );
				p_importer.LoadTexture( Path{ string::replace( strSpec, cuT( "_Cine_" ), cuT( "_S_" ) ) }
					, p_pass
					, TextureChannel::eSpecular );
				p_importer.LoadTexture( Path{ string::replace( strOpac, cuT( "_Cine_" ), cuT( "_A_" ) ) }
					, p_pass
					, TextureChannel::eOpacity );
			}
			else
			{
				DoLoadTexture( difTexName, p_pass, TextureChannel::eDiffuse, p_importer );
				DoLoadTexture( spcTexName, p_pass, TextureChannel::eSpecular, p_importer );
				DoLoadTexture( emiTexName, p_pass, TextureChannel::eEmissive, p_importer );
				DoLoadTexture( opaTexName, p_pass, TextureChannel::eOpacity, p_importer );
				DoLoadTexture( shnTexName, p_pass, TextureChannel::eGloss, p_importer );

				if ( nmlTexName.length > 0 )
				{
					DoLoadTexture( nmlTexName, p_pass, TextureChannel::eNormal, p_importer );

					if ( hgtTexName.length > 0 )
					{
						DoLoadTexture( hgtTexName, p_pass, TextureChannel::eHeight, p_importer );
					}
				}
				else if ( hgtTexName.length > 0 )
				{
					DoLoadTexture( hgtTexName, p_pass, TextureChannel::eNormal, p_importer );
				}
			}
		}

		std::map< std::chrono::milliseconds, Point3r > DoProcessVec3Keys( aiVectorKey const * const p_keys
			, uint32_t p_count
			, int64_t p_ticksPerMilliSecond
			, std::set< std::chrono::milliseconds > & p_times )
		{
			std::map< std::chrono::milliseconds, Point3r > result;

			for ( auto const & key : make_array_view( p_keys, p_count ) )
			{
				auto time = std::chrono::milliseconds{ int64_t( key.mTime * 1000 ) } / p_ticksPerMilliSecond;
				p_times.insert( time );
				result[time] = Point3r{ key.mValue.x, key.mValue.y, key.mValue.z };
			}

			return result;
		}

		std::map< std::chrono::milliseconds, Quaternion > DoProcessQuatKeys( aiQuatKey const * const p_keys
			, uint32_t p_count
			, int64_t p_ticksPerMilliSecond
			, std::set< std::chrono::milliseconds > & p_times )
		{
			std::map< std::chrono::milliseconds, Quaternion > result;

			for ( auto const & key : make_array_view( p_keys, p_count ) )
			{
				auto time = std::chrono::milliseconds{ int64_t( key.mTime * 1000 ) } / p_ticksPerMilliSecond;
				p_times.insert( time );
				result[time] = Quaternion::from_matrix( Matrix4x4r{ Matrix3x3r{ &key.mValue.GetMatrix().Transpose().a1 } } );
			}

			return result;
		}

		void DoSynchroniseKeys( std::map< std::chrono::milliseconds, Point3r > const & p_translates
			, std::map< std::chrono::milliseconds, Point3r > const & p_scales
			, std::map< std::chrono::milliseconds, Quaternion > const & p_rotates
			, std::set< std::chrono::milliseconds > const & p_times
			, uint32_t p_fps
			, int64_t p_ticksPerMilliSecond
			, SkeletonAnimationObject & p_object )
		{
			InterpolatorT< Point3r, InterpolatorType::eLinear > pointInterpolator;
			InterpolatorT< Quaternion, InterpolatorType::eLinear > quatInterpolator;

			if ( p_ticksPerMilliSecond / 1000 >= p_fps )
			{
				for ( auto time : p_times )
				{
					Point3r translate = DoCompute( time, pointInterpolator, p_translates );
					Point3r scale = DoCompute( time, pointInterpolator, p_scales );
					Quaternion rotate = DoCompute( time, quatInterpolator, p_rotates );
					p_object.AddKeyFrame( time, translate, rotate, scale );
				}
			}
			else
			{
				// Limit the key frames per second to 60, to spare RAM...
				std::chrono::milliseconds step{ 1000 / std::min< int64_t >( 60, int64_t( p_fps ) ) };
				std::chrono::milliseconds maxTime{ *p_times.rbegin() + step };

				for ( std::chrono::milliseconds time{ 0 }; time < maxTime; time += step )
				{
					Point3r translate = DoCompute( time, pointInterpolator, p_translates );
					Point3r scale = DoCompute( time, pointInterpolator, p_scales );
					Quaternion rotate = DoCompute( time, quatInterpolator, p_rotates );
					p_object.AddKeyFrame( time, translate, rotate, scale );
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
		auto mesh = p_scene.GetMeshCache().Add( cuT( "Mesh_PLY" ) );
		bool result = DoImportMesh( *mesh );

		if ( result )
		{
			SceneNodeSPtr node = p_scene.GetSceneNodeCache().Add( mesh->GetName(), p_scene.GetObjectRootNode() );
			GeometrySPtr geometry = p_scene.GetGeometryCache().Add( mesh->GetName(), node, nullptr );
			geometry->SetMesh( mesh );
			m_geometries.insert( { geometry->GetName(), geometry } );
		}

		return result;
	}

	bool AssimpImporter::DoImportMesh( Mesh & p_mesh )
	{
		bool result{ false };
		m_mapBoneByID.clear();
		m_arrayBones.clear();
		SubmeshSPtr submesh;
		Assimp::Importer importer;
		uint32_t flags = aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices
			| aiProcess_OptimizeMeshes
			| aiProcess_OptimizeGraph
			| aiProcess_FixInfacingNormals
			| aiProcess_LimitBoneWeights
			| aiProcess_Debone;
		bool tangentSpace = false;
		xchar buffer[1024] = { 0 };

		if ( m_parameters.Get( cuT( "normals" ), buffer ) )
		{
			String normals = buffer;

			if ( normals == cuT( "smooth" ) )
			{
				flags |= aiProcess_GenSmoothNormals;
			}
		}

		if ( m_parameters.Get( cuT( "tangent_space" ), tangentSpace ) && tangentSpace )
		{
			flags |= aiProcess_CalcTangentSpace;
		}

		// And have it read the given file with some postprocessing
		aiScene const * aiScene = importer.ReadFile( string::string_cast< char >( m_fileName ), flags );

		if ( aiScene )
		{
			SkeletonSPtr skeleton = std::make_shared< Skeleton >( *p_mesh.GetScene() );
			skeleton->SetGlobalInverseTransform( Matrix4x4r( &aiScene->mRootNode->mTransformation.Transpose().Inverse().a1 ) );

			if ( aiScene->HasMeshes() )
			{
				bool create = true;

				for ( auto aiMesh : make_array_view( aiScene->mMeshes, aiScene->mNumMeshes ) )
				{
					if ( create )
					{
						submesh = p_mesh.CreateSubmesh();
					}

					create = DoProcessMesh( *p_mesh.GetScene(), p_mesh, *skeleton, *aiMesh, *aiScene, *submesh );
				}

				if ( m_arrayBones.empty() )
				{
					skeleton.reset();
				}
				else
				{
					p_mesh.SetSkeleton( skeleton );
				}

				if ( skeleton )
				{
					for ( auto aiAnimation : make_array_view( aiScene->mAnimations, aiScene->mNumAnimations ) )
					{
						DoProcessAnimation( p_mesh
							, m_fileName.GetFileName()
							, *skeleton
							, *aiScene->mRootNode
							, *aiAnimation );
					}

					importer.FreeScene();

					if ( string::upper_case( m_fileName.GetExtension() ) == cuT( "MD5MESH" ) )
					{
						// Workaround to load multiple animations with MD5 models.
						PathArray files;
						File::ListDirectoryFiles( m_fileName.GetPath(), files );

						for ( auto file : files )
						{
							if ( string::lower_case( file.GetExtension() ) == cuT( "md5anim" ) )
							{
								// The .md5anim with the same name as the .md5mesh has already been loaded by assimp.
								if ( file.GetFileName() != m_fileName.GetFileName() )
								{
									auto scene = importer.ReadFile( file, flags );

									for ( auto aiAnimation : make_array_view( scene->mAnimations, scene->mNumAnimations ) )
									{
										DoProcessAnimation( p_mesh
											, file.GetFileName()
											, *skeleton
											, *scene->mRootNode
											, *aiAnimation );
									}

									importer.FreeScene();
								}
							}
						}
					}
				}
				else
				{
					importer.FreeScene();
				}

				result = true;
			}
			else
			{
				importer.FreeScene();
			}
		}
		else
		{
			// The import failed, report it
			Logger::LogError( std::stringstream() << "Scene import failed : " << importer.GetErrorString() );
		}

		return result;
	}

	bool AssimpImporter::DoProcessMesh( Scene & p_scene, Mesh & p_mesh, Skeleton & p_skeleton, aiMesh const & p_aiMesh, aiScene const & p_aiScene, Submesh & p_submesh )
	{
		bool result = false;
		MaterialSPtr material;

		if ( p_aiMesh.mMaterialIndex < p_aiScene.mNumMaterials )
		{
			material = DoProcessMaterial( p_scene, *p_aiScene.mMaterials[p_aiMesh.mMaterialIndex] );
		}

		if ( p_aiMesh.HasFaces() && p_aiMesh.HasPositions() && material )
		{
			p_submesh.SetDefaultMaterial( material );
			p_submesh.Ref( material );
			p_submesh.AddPoints( DoCreateVertexBuffer( p_aiMesh ) );

			if ( p_aiMesh.HasBones() )
			{
				std::vector< VertexBoneData > arrayBones( p_aiMesh.mNumVertices );
				DoProcessBones( p_skeleton, p_aiMesh.mBones, p_aiMesh.mNumBones, arrayBones );
				p_submesh.AddBoneDatas( arrayBones );
			}

			for ( auto face : make_array_view( p_aiMesh.mFaces, p_aiMesh.mNumFaces ) )
			{
				if ( face.mNumIndices == 3 )
				{
					p_submesh.AddFace( face.mIndices[0], face.mIndices[1], face.mIndices[2] );
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
				for( auto aiAnimation : make_array_view( p_aiScene.mAnimations, p_aiScene.mNumAnimations ) )
				{
					auto it = std::find_if( aiAnimation->mMeshChannels
						, aiAnimation->mMeshChannels + aiAnimation->mNumMeshChannels
						, [this, &p_aiMesh, &p_submesh]( aiMeshAnim const * p_aiMeshAnim )
						{
							return p_aiMeshAnim->mName == p_aiMesh.mName;
						} );

					if ( it != aiAnimation->mMeshChannels + aiAnimation->mNumMeshChannels )
					{
						DoProcessAnimationMeshes( p_mesh, p_submesh, p_aiMesh, *( *it ) );
					}
				}
			}

			result = true;
		}

		return result;
	}

	void AssimpImporter::DoProcessAnimationMeshes( Mesh & p_mesh
		, Submesh & p_submesh
		, aiMesh const & p_aiMesh
		, aiMeshAnim const & p_aiMeshAnim )
	{
		if ( p_aiMeshAnim.mNumKeys )
		{
			String name{ string::string_cast< xchar >( p_aiMeshAnim.mName.C_Str() ) };
			Logger::LogDebug( cuT( "Mesh animation found: " ) + name );
			auto & animation = p_mesh.CreateAnimation( name );
			MeshAnimationSubmesh animSubmesh{ animation, p_submesh };

			for ( auto aiKey : make_array_view( p_aiMeshAnim.mKeys, p_aiMeshAnim.mNumKeys ) )
			{
				animSubmesh.AddBuffer( std::chrono::milliseconds{ int64_t( aiKey.mTime * 1000 ) }
					, DoCreateVertexBuffer( *p_aiMesh.mAnimMeshes[aiKey.mValue] ) );
			}

			animation.AddChild( std::move( animSubmesh ) );
		}
	}

	MaterialSPtr AssimpImporter::DoProcessMaterial( Scene & p_scene
		, aiMaterial const & p_aiMaterial )
	{
		MaterialSPtr result;
		auto & cache = p_scene.GetMaterialView();
		aiString mtlname;
		p_aiMaterial.Get( AI_MATKEY_NAME, mtlname );
		String name = string::string_cast< xchar >( mtlname.C_Str() );

		if ( name.empty() )
		{
			name = m_fileName.GetFileName() + string::to_string( m_anonymous++ );
		}

		if ( cache.Has( name ) )
		{
			result = cache.Find( name );
			REQUIRE( result->GetType() == MaterialType::eLegacy );
		}
		else
		{
			result = cache.Add( name, MaterialType::eLegacy );
			result->CreatePass();
			auto pass = result->GetTypedPass< MaterialType::eLegacy >( 0 );

			DoProcessPassBaseComponents( *pass, p_aiMaterial );
			DoProcessPassTextures( *pass, p_aiMaterial, *this );
		}

		return result;
	}

	BoneSPtr AssimpImporter::DoAddBone( String const & p_name
		, Matrix4x4r const & p_offset
		, Skeleton & p_skeleton
		, uint32_t & p_index )
	{
		BoneSPtr bone = p_skeleton.CreateBone( p_name );
		bone->SetOffsetMatrix( p_offset );
		p_index = uint32_t( m_arrayBones.size() );
		m_arrayBones.push_back( bone );
		m_mapBoneByID[p_name] = p_index;
		return bone;
	}

	void AssimpImporter::DoProcessBones( Skeleton & p_skeleton
		, aiBone const * const * p_aiBones
		, uint32_t p_count
		, std::vector< VertexBoneData > & p_arrayVertices )
	{
		for ( uint32_t i = 0; i < p_count; ++i )
		{
			aiBone const & aiBone = *p_aiBones[i];
			String name = string::string_cast< xchar >( aiBone.mName.C_Str() );
			uint32_t index;

			if ( m_mapBoneByID.find( name ) == m_mapBoneByID.end() )
			{
				auto mtx = aiBone.mOffsetMatrix;
				DoAddBone( name, Matrix4x4r{ &mtx.Transpose().a1 }, p_skeleton, index );
			}
			else
			{
				index = m_mapBoneByID[name];
				aiMatrix4x4 mtx{ aiBone.mOffsetMatrix };
				ENSURE( m_arrayBones[index]->GetOffsetMatrix() == Matrix4x4r( &mtx.Transpose().a1 ) );
			}

			for ( auto weight : make_array_view( aiBone.mWeights, aiBone.mNumWeights ) )
			{
				p_arrayVertices[weight.mVertexId].AddBoneData( index, real( weight.mWeight ) );
			}
		}
	}

	void AssimpImporter::DoProcessAnimation( Mesh & p_mesh
		, String const & p_name
		, Skeleton & p_skeleton
		, aiNode const & p_aiNode
		, aiAnimation const & p_aiAnimation )
	{
		String name{ string::string_cast< xchar >( p_aiAnimation.mName.C_Str() ) };
		Logger::LogDebug( cuT( "Skeleton animation found: " ) + name );

		if ( name.empty() )
		{
			name = p_name;
		}

		auto & animation = p_skeleton.CreateAnimation( name );
		int64_t ticksPerMilliSecond = int64_t( p_aiAnimation.mTicksPerSecond ? p_aiAnimation.mTicksPerSecond : 25 );
		DoProcessAnimationNodes( p_mesh
			, animation
			, ticksPerMilliSecond
			, p_skeleton
			, p_aiNode
			, p_aiAnimation
			, nullptr );
		animation.UpdateLength();
	}

	void AssimpImporter::DoProcessAnimationNodes( Mesh & p_mesh
		, SkeletonAnimation & p_animation
		, int64_t p_ticksPerMilliSecond
		, Skeleton & p_skeleton
		, aiNode const & p_aiNode
		, aiAnimation const & p_aiAnimation
		, SkeletonAnimationObjectSPtr p_object )
	{
		String name = string::string_cast< xchar >( p_aiNode.mName.data );
		const aiNodeAnim * aiNodeAnim = FindNodeAnim( p_aiAnimation, name );
		SkeletonAnimationObjectSPtr object;
		auto itBone = m_mapBoneByID.find( name );

		if ( !aiNodeAnim
			&& itBone == m_mapBoneByID.end()
			&& p_aiNode.mNumMeshes )
		{
			uint32_t index;
			auto bone = DoAddBone( name, Matrix4x4r{ 1.0_r }, p_skeleton, index );

			for ( auto const & mesh : make_array_view( p_aiNode.mMeshes, p_aiNode.mNumMeshes ) )
			{
				auto submesh = p_mesh.GetSubmesh( mesh );
				REQUIRE( submesh != nullptr );

				if ( !submesh->HasBoneData() )
				{
					std::vector< VertexBoneData > arrayBones( submesh->GetPointsCount() );

					for ( auto & boneData : arrayBones )
					{
						boneData.AddBoneData( index, 1.0_r );
					}

					submesh->AddBoneDatas( arrayBones );
				}
			}

			itBone = m_mapBoneByID.find( name );
		}

		if ( itBone != m_mapBoneByID.end() )
		{
			auto bone = m_arrayBones[itBone->second];
			object = p_animation.AddObject( bone, p_object );

			if ( p_object->GetType() == SkeletonAnimationObjectType::eBone )
			{
				p_skeleton.SetBoneParent( bone, std::static_pointer_cast< SkeletonAnimationBone >( p_object )->GetBone() );
			}
		}
		else
		{
			object = p_animation.AddObject( p_aiNode.mName.C_Str(), p_object );
		}

		if ( aiNodeAnim )
		{
			DoProcessAnimationNodeKeys( *aiNodeAnim, p_ticksPerMilliSecond, *object );
		}

		if ( object )
		{
			if ( p_object && object != p_object )
			{
				p_object->AddChild( object );
			}

			if ( !object->HasKeyFrames() )
			{
				object->SetNodeTransform( Matrix4x4r( &p_aiNode.mTransformation.a1 ).get_transposed() );
			}
		}

		for ( auto node : make_array_view( p_aiNode.mChildren, p_aiNode.mNumChildren ) )
		{
			DoProcessAnimationNodes( p_mesh
				, p_animation
				, p_ticksPerMilliSecond
				, p_skeleton
				, *node
				, p_aiAnimation
				, object );
		}
	}

	void AssimpImporter::DoProcessAnimationNodeKeys( aiNodeAnim const & p_aiNodeAnim
		, int64_t p_ticksPerMilliSecond
		, Castor3D::SkeletonAnimationObject & p_object )
	{
		std::set< std::chrono::milliseconds > times;

		auto translates = DoProcessVec3Keys( p_aiNodeAnim.mPositionKeys
			, p_aiNodeAnim.mNumPositionKeys
			, p_ticksPerMilliSecond
			, times );
		auto scales = DoProcessVec3Keys( p_aiNodeAnim.mScalingKeys
			, p_aiNodeAnim.mNumScalingKeys
			, p_ticksPerMilliSecond
			, times );
		auto rotates = DoProcessQuatKeys( p_aiNodeAnim.mRotationKeys
			, p_aiNodeAnim.mNumRotationKeys
			, p_ticksPerMilliSecond
			, times );
		DoSynchroniseKeys( translates
			, scales
			, rotates
			, times
			, GetEngine()->GetRenderLoop().GetWantedFps()
			, p_ticksPerMilliSecond
			, p_object );
	}

	//*********************************************************************************************
}
