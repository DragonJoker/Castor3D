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
		aiNodeAnim const * const FindNodeAnim( const aiAnimation & animation
			, const String & nodeName )
		{
			aiNodeAnim const * result = nullptr;
			auto it = std::find_if( animation.mChannels
				, animation.mChannels + animation.mNumChannels
				, [&nodeName]( aiNodeAnim const * const p_nodeAnim )
			{
				return string::string_cast< xchar >( p_nodeAnim->mNodeName.data ) == nodeName;
			} );

			if ( it != animation.mChannels + animation.mNumChannels )
			{
				result = *it;
			}

			return result;
		}

		aiMeshAnim const * const FindMeshAnim( const aiAnimation & animation
			, const String & meshName )
		{
			aiMeshAnim const * result = nullptr;
			auto it = std::find_if( animation.mMeshChannels
				, animation.mMeshChannels + animation.mNumMeshChannels
				, [&meshName]( aiMeshAnim const * const p_meshAnim )
				{
					return string::string_cast< xchar >( p_meshAnim->mName.data ) == meshName;
				} );

			if ( it != animation.mMeshChannels + animation.mNumMeshChannels )
			{
				result = *it;
			}

			return result;
		}

		template< typename aiMeshType >
		InterleavedVertexArray DoCreateVertexBuffer( aiMeshType const & aiMesh )
		{
			InterleavedVertexArray vertices{ aiMesh.mNumVertices };
			uint32_t index{ 0u };

			for ( auto & vertex : vertices )
			{
				vertex.m_pos[0] = real( aiMesh.mVertices[index].x );
				vertex.m_pos[1] = real( aiMesh.mVertices[index].y );
				vertex.m_pos[2] = real( aiMesh.mVertices[index].z );
				++index;
			}

			if ( aiMesh.HasNormals() )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.m_nml[0] = real( aiMesh.mNormals[index].x );
					vertex.m_nml[1] = real( aiMesh.mNormals[index].y );
					vertex.m_nml[2] = real( aiMesh.mNormals[index].z );
					++index;
				}
			}

			if ( aiMesh.HasTangentsAndBitangents() )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.m_tan[0] = real( aiMesh.mTangents[index].x );
					vertex.m_tan[1] = real( aiMesh.mTangents[index].y );
					vertex.m_tan[2] = real( aiMesh.mTangents[index].z );
					++index;
				}

				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.m_bin[0] = real( aiMesh.mBitangents[index].x );
					vertex.m_bin[1] = real( aiMesh.mBitangents[index].y );
					vertex.m_bin[2] = real( aiMesh.mBitangents[index].z );
					++index;
				}
			}

			if ( aiMesh.HasTextureCoords( 0 ) )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.m_tex[0] = real( aiMesh.mTextureCoords[0][index].x );
					vertex.m_tex[1] = real( aiMesh.mTextureCoords[0][index].y );
					vertex.m_tex[2] = real( aiMesh.mTextureCoords[0][index].z );
					++index;
				}
			}

			return vertices;
		}

		template< typename T >
		void DoFind( Castor::Milliseconds time
			, typename std::map< Castor::Milliseconds, T > const & map
			, typename std::map< Castor::Milliseconds, T >::const_iterator & prv
			, typename std::map< Castor::Milliseconds, T >::const_iterator & cur )
		{
			if ( map.empty() )
			{
				prv = cur = map.end();
			}
			else
			{
				cur = std::find_if( map.begin()
					, map.end()
					, [&time]( std::pair< Castor::Milliseconds, T > const & pair )
					{
						return pair.first > time;
					} );

				if ( cur == map.end() )
				{
					--cur;
				}

				prv = cur;

				if ( prv != map.begin() )
				{
					prv--;
				}

				ENSURE( prv != cur );
			}
		}

		template< typename T >
		T DoCompute( Castor::Milliseconds const & from
			, Interpolator< T > const & interpolator
			, std::map< Castor::Milliseconds, T > const & values )
		{
			T result;

			if ( values.size() == 1 )
			{
				result = values.begin()->second;
			}
			else
			{
				auto prv = values.begin();
				auto cur = values.begin();
				DoFind( from, values, prv, cur );
				auto dt = cur->first - prv->first;
				real factor = ( from - prv->first ).count() / real( dt.count() );
				result = interpolator.Interpolate( prv->second, cur->second, factor );
			}

			return result;
		}

		void DoProcessPassBaseComponents( LegacyPass & pass
			, aiMaterial const & aiMaterial )
		{
			aiColor3D ambient( 1, 1, 1 );
			aiMaterial.Get( AI_MATKEY_COLOR_AMBIENT, ambient );
			aiColor3D diffuse( 1, 1, 1 );
			aiMaterial.Get( AI_MATKEY_COLOR_DIFFUSE, diffuse );
			aiColor3D specular( 1, 1, 1 );
			aiMaterial.Get( AI_MATKEY_COLOR_SPECULAR, specular );
			aiColor3D emissive( 1, 1, 1 );
			aiMaterial.Get( AI_MATKEY_COLOR_EMISSIVE, emissive );
			float opacity = 1;
			aiMaterial.Get( AI_MATKEY_OPACITY, opacity );
			float shininess = 0.5f;
			aiMaterial.Get( AI_MATKEY_SHININESS, shininess );
			float shininessStrength = 1.0f;
			aiMaterial.Get( AI_MATKEY_SHININESS_STRENGTH, shininessStrength );
			int twoSided = 0;
			aiMaterial.Get( AI_MATKEY_TWOSIDED, twoSided );

			if ( ambient.IsBlack()
				&& diffuse.IsBlack()
				&& specular.IsBlack()
				&& emissive.IsBlack() )
			{
				diffuse.r = 1.0;
				diffuse.g = 1.0;
				diffuse.b = 1.0;
			}

			pass.SetOpacity( opacity );
			pass.SetTwoSided( twoSided != 0 );
			pass.SetDiffuse( Colour::from_components( diffuse.r
				, diffuse.g
				, diffuse.b
				, 1 ) );
			pass.SetSpecular( Colour::from_components( specular.r * shininessStrength
				, specular.g * shininessStrength
				, specular.b * shininessStrength
				, 1 ) );
			pass.SetEmissive( float( point::length( Point3f{ emissive.r
				, emissive.g
				, emissive.b } ) ) );

			if ( shininess > 0 )
			{
				pass.SetShininess( shininess );
			}
		}

		void DoLoadTexture( aiString const & name
			, Pass & pass
			, TextureChannel channel
			, Importer const & importer )
		{
			if ( name.length > 0 )
			{
				importer.LoadTexture( Path{ string::string_cast< xchar >( name.C_Str() ) }
					, pass
					, channel );
			}
		}

		void DoProcessPassTextures( Pass & pass
			, aiMaterial const & aiMaterial
			, Importer const & importer )
		{
			aiString ambTexName;
			aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_AMBIENT, 0 ), ambTexName );
			aiString difTexName;
			aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0 ), difTexName );
			aiString spcTexName;
			aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0 ), spcTexName );
			aiString emiTexName;
			aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_EMISSIVE, 0 ), emiTexName );
			aiString nmlTexName;
			aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_NORMALS, 0 ), nmlTexName );
			aiString hgtTexName;
			aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_HEIGHT, 0 ), hgtTexName );
			aiString opaTexName;
			aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_OPACITY, 0 ), opaTexName );
			aiString shnTexName;
			aiMaterial.Get( AI_MATKEY_TEXTURE( aiTextureType_SHININESS, 0 ), shnTexName );

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
				importer.LoadTexture( Path{ string::replace( strDiff, cuT( "_Cine_" ), cuT( "_D_" ) ) }
					, pass
					, TextureChannel::eDiffuse );
				importer.LoadTexture( Path{ string::replace( strNorm, cuT( "_Cine_" ), cuT( "_N_" ) ) }
					, pass
					, TextureChannel::eNormal );
				importer.LoadTexture( Path{ string::replace( strSpec, cuT( "_Cine_" ), cuT( "_S_" ) ) }
					, pass
					, TextureChannel::eSpecular );
				importer.LoadTexture( Path{ string::replace( strOpac, cuT( "_Cine_" ), cuT( "_A_" ) ) }
					, pass
					, TextureChannel::eOpacity );
			}
			else
			{
				DoLoadTexture( difTexName, pass, TextureChannel::eDiffuse, importer );
				DoLoadTexture( spcTexName, pass, TextureChannel::eSpecular, importer );
				DoLoadTexture( emiTexName, pass, TextureChannel::eEmissive, importer );
				DoLoadTexture( opaTexName, pass, TextureChannel::eOpacity, importer );
				DoLoadTexture( shnTexName, pass, TextureChannel::eGloss, importer );

				if ( nmlTexName.length > 0 )
				{
					DoLoadTexture( nmlTexName, pass, TextureChannel::eNormal, importer );

					if ( hgtTexName.length > 0 )
					{
						DoLoadTexture( hgtTexName, pass, TextureChannel::eHeight, importer );
					}
				}
				else if ( hgtTexName.length > 0 )
				{
					DoLoadTexture( hgtTexName, pass, TextureChannel::eNormal, importer );
				}
			}
		}

		std::map< Castor::Milliseconds, Point3r > DoProcessVec3Keys( aiVectorKey const * const keys
			, uint32_t count
			, int64_t ticksPerMilliSecond
			, std::set< Castor::Milliseconds > & times )
		{
			std::map< Castor::Milliseconds, Point3r > result;

			for ( auto const & key : make_array_view( keys, count ) )
			{
				auto time = Castor::Milliseconds{ int64_t( key.mTime * 1000 ) } / ticksPerMilliSecond;
				times.insert( time );
				result[time] = Point3r{ key.mValue.x, key.mValue.y, key.mValue.z };
			}

			return result;
		}

		std::map< Castor::Milliseconds, Quaternion > DoProcessQuatKeys( aiQuatKey const * const keys
			, uint32_t count
			, int64_t ticksPerMilliSecond
			, std::set< Castor::Milliseconds > & times )
		{
			std::map< Castor::Milliseconds, Quaternion > result;

			for ( auto const & key : make_array_view( keys, count ) )
			{
				auto time = Castor::Milliseconds{ int64_t( key.mTime * 1000 ) } / ticksPerMilliSecond;
				times.insert( time );
				result[time] = Quaternion::from_matrix( Matrix4x4r{ Matrix3x3r{ &key.mValue.GetMatrix().Transpose().a1 } } );
			}

			return result;
		}

		void DoSynchroniseKeys( std::map< Castor::Milliseconds, Point3r > const & translates
			, std::map< Castor::Milliseconds, Point3r > const & scales
			, std::map< Castor::Milliseconds, Quaternion > const & rotates
			, std::set< Castor::Milliseconds > const & times
			, uint32_t fps
			, int64_t ticksPerMilliSecond
			, SkeletonAnimationObject & object )
		{
			InterpolatorT< Point3r, InterpolatorType::eLinear > pointInterpolator;
			InterpolatorT< Quaternion, InterpolatorType::eLinear > quatInterpolator;

			if ( ticksPerMilliSecond / 1000 >= fps )
			{
				for ( auto time : times )
				{
					Point3r translate = DoCompute( time, pointInterpolator, translates );
					Point3r scale = DoCompute( time, pointInterpolator, scales );
					Quaternion rotate = DoCompute( time, quatInterpolator, rotates );
					object.AddKeyFrame( time, translate, rotate, scale );
				}
			}
			else
			{
				// Limit the key frames per second to 60, to spare RAM...
				Castor::Milliseconds step{ 1000 / std::min< int64_t >( 60, int64_t( fps ) ) };
				Castor::Milliseconds maxTime{ *times.rbegin() + step };

				for ( Castor::Milliseconds time{ 0 }; time < maxTime; time += step )
				{
					Point3r translate = DoCompute( time, pointInterpolator, translates );
					Point3r scale = DoCompute( time, pointInterpolator, scales );
					Quaternion rotate = DoCompute( time, quatInterpolator, rotates );
					object.AddKeyFrame( time, translate, rotate, scale );
				}
			}
		}
	}

	//*********************************************************************************************

	Castor::String const AssimpImporter::Name = cuT( "ASSIMP Importer" );

	AssimpImporter::AssimpImporter( Engine & engine )
		: Importer( engine )
		, m_anonymous( 0 )
	{
	}

	AssimpImporter::~AssimpImporter()
	{
	}

	ImporterUPtr AssimpImporter::Create( Engine & engine )
	{
		return std::make_unique< AssimpImporter >( engine );
	}

	bool AssimpImporter::DoImportScene( Scene & scene )
	{
		auto mesh = scene.GetMeshCache().Add( cuT( "Mesh_PLY" ) );
		bool result = DoImportMesh( *mesh );

		if ( result )
		{
			SceneNodeSPtr node = scene.GetSceneNodeCache().Add( mesh->GetName(), scene.GetObjectRootNode() );
			GeometrySPtr geometry = scene.GetGeometryCache().Add( mesh->GetName(), node, nullptr );
			geometry->SetMesh( mesh );
			m_geometries.insert( { geometry->GetName(), geometry } );
		}

		return result;
	}

	bool AssimpImporter::DoImportMesh( Mesh & mesh )
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
			SkeletonSPtr skeleton = std::make_shared< Skeleton >( *mesh.GetScene() );
			skeleton->SetGlobalInverseTransform( Matrix4x4r( &aiScene->mRootNode->mTransformation.Transpose().Inverse().a1 ) );

			if ( aiScene->HasMeshes() )
			{
				bool create = true;

				for ( auto aiMesh : make_array_view( aiScene->mMeshes, aiScene->mNumMeshes ) )
				{
					if ( create )
					{
						submesh = mesh.CreateSubmesh();
					}

					create = DoProcessMesh( *mesh.GetScene(), mesh, *skeleton, *aiMesh, *aiScene, *submesh );
				}

				if ( m_arrayBones.empty() )
				{
					skeleton.reset();
				}
				else
				{
					mesh.SetSkeleton( skeleton );
				}

				if ( skeleton )
				{
					for ( auto aiAnimation : make_array_view( aiScene->mAnimations, aiScene->mNumAnimations ) )
					{
						DoProcessAnimation( mesh
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
										DoProcessAnimation( mesh
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

	bool AssimpImporter::DoProcessMesh( Scene & scene
		, Mesh & mesh
		, Skeleton & skeleton
		, aiMesh const & aiMesh
		, aiScene const & aiScene
		, Submesh & submesh )
	{
		bool result = false;
		MaterialSPtr material;

		if ( aiMesh.mMaterialIndex < aiScene.mNumMaterials )
		{
			material = DoProcessMaterial( scene, *aiScene.mMaterials[aiMesh.mMaterialIndex] );
		}

		if ( aiMesh.HasFaces() && aiMesh.HasPositions() && material )
		{
			submesh.SetDefaultMaterial( material );
			submesh.Ref( material );
			submesh.AddPoints( DoCreateVertexBuffer( aiMesh ) );

			if ( aiMesh.HasBones() )
			{
				std::vector< VertexBoneData > arrayBones( aiMesh.mNumVertices );
				DoProcessBones( skeleton, aiMesh.mBones, aiMesh.mNumBones, arrayBones );
				submesh.AddBoneDatas( arrayBones );
			}

			for ( auto face : make_array_view( aiMesh.mFaces, aiMesh.mNumFaces ) )
			{
				if ( face.mNumIndices == 3 )
				{
					submesh.AddFace( face.mIndices[0], face.mIndices[1], face.mIndices[2] );
				}
			}

			if ( !aiMesh.mNormals )
			{
				submesh.ComputeNormals( true );
			}
			else if ( !aiMesh.mTangents )
			{
				submesh.ComputeTangentsFromNormals();
			}

			if ( aiScene.HasAnimations() )
			{
				for( auto aiAnimation : make_array_view( aiScene.mAnimations, aiScene.mNumAnimations ) )
				{
					auto it = std::find_if( aiAnimation->mMeshChannels
						, aiAnimation->mMeshChannels + aiAnimation->mNumMeshChannels
						, [this, &aiMesh, &submesh]( aiMeshAnim const * p_aiMeshAnim )
						{
							return p_aiMeshAnim->mName == aiMesh.mName;
						} );

					if ( it != aiAnimation->mMeshChannels + aiAnimation->mNumMeshChannels )
					{
						DoProcessAnimationMeshes( mesh, submesh, aiMesh, *( *it ) );
					}
				}
			}

			result = true;
		}

		return result;
	}

	void AssimpImporter::DoProcessAnimationMeshes( Mesh & mesh
		, Submesh & submesh
		, aiMesh const & aiMesh
		, aiMeshAnim const & aiMeshAnim )
	{
		if ( aiMeshAnim.mNumKeys )
		{
			String name{ string::string_cast< xchar >( aiMeshAnim.mName.C_Str() ) };
			Logger::LogDebug( cuT( "Mesh animation found: " ) + name );
			auto & animation = mesh.CreateAnimation( name );
			MeshAnimationSubmesh animSubmesh{ animation, submesh };

			for ( auto aiKey : make_array_view( aiMeshAnim.mKeys, aiMeshAnim.mNumKeys ) )
			{
				animSubmesh.AddBuffer( Castor::Milliseconds{ int64_t( aiKey.mTime * 1000 ) }
					, DoCreateVertexBuffer( *aiMesh.mAnimMeshes[aiKey.mValue] ) );
			}

			animation.AddChild( std::move( animSubmesh ) );
		}
	}

	MaterialSPtr AssimpImporter::DoProcessMaterial( Scene & scene
		, aiMaterial const & aiMaterial )
	{
		MaterialSPtr result;
		auto & cache = scene.GetMaterialView();
		aiString mtlname;
		aiMaterial.Get( AI_MATKEY_NAME, mtlname );
		String name = string::string_cast< xchar >( mtlname.C_Str() );

		if ( name.empty() )
		{
			name = m_fileName.GetFileName() + string::to_string( m_anonymous++ );
		}

		if ( cache.Has( name ) )
		{
			result = cache.Find( name );
		}
		else
		{
			result = cache.Add( name, MaterialType::eLegacy );
			result->CreatePass();
			auto pass = result->GetTypedPass< MaterialType::eLegacy >( 0 );

			DoProcessPassBaseComponents( *pass, aiMaterial );
			DoProcessPassTextures( *pass, aiMaterial, *this );
		}

		return result;
	}

	BoneSPtr AssimpImporter::DoAddBone( String const & name
		, Matrix4x4r const & offset
		, Skeleton & skeleton
		, uint32_t & index )
	{
		BoneSPtr bone = skeleton.CreateBone( name );
		bone->SetOffsetMatrix( offset );
		index = uint32_t( m_arrayBones.size() );
		m_arrayBones.push_back( bone );
		m_mapBoneByID[name] = index;
		return bone;
	}

	void AssimpImporter::DoProcessBones( Skeleton & skeleton
		, aiBone const * const * aiBones
		, uint32_t count
		, std::vector< VertexBoneData > & arrayVertices )
	{
		for ( uint32_t i = 0; i < count; ++i )
		{
			aiBone const & aiBone = *aiBones[i];
			String name = string::string_cast< xchar >( aiBone.mName.C_Str() );
			uint32_t index;

			if ( m_mapBoneByID.find( name ) == m_mapBoneByID.end() )
			{
				auto mtx = aiBone.mOffsetMatrix;
				DoAddBone( name, Matrix4x4r{ &mtx.Transpose().a1 }, skeleton, index );
			}
			else
			{
				index = m_mapBoneByID[name];
				aiMatrix4x4 mtx{ aiBone.mOffsetMatrix };
				ENSURE( m_arrayBones[index]->GetOffsetMatrix() == Matrix4x4r( &mtx.Transpose().a1 ) );
			}

			for ( auto weight : make_array_view( aiBone.mWeights, aiBone.mNumWeights ) )
			{
				arrayVertices[weight.mVertexId].AddBoneData( index, real( weight.mWeight ) );
			}
		}
	}

	void AssimpImporter::DoProcessAnimation( Mesh & mesh
		, String const & animName
		, Skeleton & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation )
	{
		String name{ string::string_cast< xchar >( aiAnimation.mName.C_Str() ) };
		Logger::LogDebug( cuT( "Skeleton animation found: " ) + name );

		if ( name.empty() )
		{
			name = animName;
		}

		auto & animation = skeleton.CreateAnimation( name );
		int64_t ticksPerMilliSecond = int64_t( aiAnimation.mTicksPerSecond ? aiAnimation.mTicksPerSecond : 25 );
		DoProcessAnimationNodes( mesh
			, animation
			, ticksPerMilliSecond
			, skeleton
			, aiNode
			, aiAnimation
			, nullptr );
		animation.UpdateLength();
	}

	void AssimpImporter::DoProcessAnimationNodes( Mesh & mesh
		, SkeletonAnimation & p_animation
		, int64_t ticksPerMilliSecond
		, Skeleton & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation
		, SkeletonAnimationObjectSPtr parent )
	{
		String name = string::string_cast< xchar >( aiNode.mName.data );
		const aiNodeAnim * aiNodeAnim = FindNodeAnim( aiAnimation, name );
		SkeletonAnimationObjectSPtr object;
		auto itBone = m_mapBoneByID.find( name );

		if ( !aiNodeAnim
			&& itBone == m_mapBoneByID.end()
			&& aiNode.mNumMeshes )
		{
			uint32_t index;
			auto bone = DoAddBone( name, Matrix4x4r{ 1.0_r }, skeleton, index );

			for ( auto const & aiMesh : make_array_view( aiNode.mMeshes, aiNode.mNumMeshes ) )
			{
				auto submesh = mesh.GetSubmesh( aiMesh );
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
			object = p_animation.AddObject( bone, parent );

			if ( parent->GetType() == SkeletonAnimationObjectType::eBone )
			{
				skeleton.SetBoneParent( bone, std::static_pointer_cast< SkeletonAnimationBone >( parent )->GetBone() );
			}
		}
		else
		{
			object = p_animation.AddObject( aiNode.mName.C_Str(), parent );
		}

		if ( aiNodeAnim )
		{
			DoProcessAnimationNodeKeys( *aiNodeAnim, ticksPerMilliSecond, *object );
		}

		if ( object )
		{
			if ( parent && object != parent )
			{
				parent->AddChild( object );
			}

			if ( !object->HasKeyFrames() )
			{
				object->SetNodeTransform( Matrix4x4r( &aiNode.mTransformation.a1 ).get_transposed() );
			}
		}

		for ( auto node : make_array_view( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			DoProcessAnimationNodes( mesh
				, p_animation
				, ticksPerMilliSecond
				, skeleton
				, *node
				, aiAnimation
				, object );
		}
	}

	void AssimpImporter::DoProcessAnimationNodeKeys( aiNodeAnim const & aiNodeAnim
		, int64_t ticksPerMilliSecond
		, Castor3D::SkeletonAnimationObject & object )
	{
		std::set< Castor::Milliseconds > times;

		auto translates = DoProcessVec3Keys( aiNodeAnim.mPositionKeys
			, aiNodeAnim.mNumPositionKeys
			, ticksPerMilliSecond
			, times );
		auto scales = DoProcessVec3Keys( aiNodeAnim.mScalingKeys
			, aiNodeAnim.mNumScalingKeys
			, ticksPerMilliSecond
			, times );
		auto rotates = DoProcessQuatKeys( aiNodeAnim.mRotationKeys
			, aiNodeAnim.mNumRotationKeys
			, ticksPerMilliSecond
			, times );
		DoSynchroniseKeys( translates
			, scales
			, rotates
			, times
			, GetEngine()->GetRenderLoop().GetWantedFps()
			, ticksPerMilliSecond
			, object );
	}

	//*********************************************************************************************
}
