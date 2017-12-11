#include "AssimpImporter.hpp"

#include <Design/ArrayView.hpp>

#include <Cache/GeometryCache.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/PluginCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/SceneNodeCache.hpp>

#include <Animation/Mesh/MeshAnimation.hpp>
#include <Animation/Mesh/MeshAnimationKeyFrame.hpp>
#include <Animation/Mesh/MeshAnimationSubmesh.hpp>
#include <Animation/Skeleton/SkeletonAnimation.hpp>
#include <Animation/Skeleton/SkeletonAnimationKeyFrame.hpp>
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

using namespace castor3d;
using namespace castor;

namespace C3dAssimp
{
	//*********************************************************************************************

	namespace
	{
		aiNodeAnim const * const doFindNodeAnim( const aiAnimation & animation
			, const String & nodeName )
		{
			aiNodeAnim const * result = nullptr;
			auto it = std::find_if( animation.mChannels
				, animation.mChannels + animation.mNumChannels
				, [&nodeName]( aiNodeAnim const * const p_nodeAnim )
			{
				return string::stringCast< xchar >( p_nodeAnim->mNodeName.data ) == nodeName;
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
					return string::stringCast< xchar >( p_meshAnim->mName.data ) == meshName;
				} );

			if ( it != animation.mMeshChannels + animation.mNumMeshChannels )
			{
				result = *it;
			}

			return result;
		}

		template< typename aiMeshType >
		InterleavedVertexArray doCreateVertexBuffer( aiMeshType const & aiMesh )
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
		void doFind( Milliseconds time
			, typename std::map< Milliseconds, T > const & map
			, typename std::map< Milliseconds, T >::const_iterator & prv
			, typename std::map< Milliseconds, T >::const_iterator & cur )
		{
			if ( map.empty() )
			{
				prv = cur = map.end();
			}
			else
			{
				cur = std::find_if( map.begin()
					, map.end()
					, [&time]( std::pair< Milliseconds, T > const & pair )
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
		T doCompute( Milliseconds const & from
			, Interpolator< T > const & interpolator
			, std::map< Milliseconds, T > const & values )
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
				doFind( from, values, prv, cur );
				auto dt = cur->first - prv->first;
				real factor = ( from - prv->first ).count() / real( dt.count() );
				result = interpolator.interpolate( prv->second, cur->second, factor );
			}

			return result;
		}

		void doProcessPassBaseComponents( LegacyPass & pass
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

			pass.setOpacity( opacity );
			pass.setTwoSided( twoSided != 0 );
			pass.setDiffuse( RgbColour::fromComponents( diffuse.r
				, diffuse.g
				, diffuse.b ) );
			pass.setSpecular( RgbColour::fromComponents( specular.r * shininessStrength
				, specular.g * shininessStrength
				, specular.b * shininessStrength ) );
			pass.setEmissive( float( point::length( Point3f{ emissive.r
				, emissive.g
				, emissive.b } ) ) );

			if ( shininess > 0 )
			{
				pass.setShininess( shininess );
			}
		}

		void doLoadTexture( aiString const & name
			, Pass & pass
			, TextureChannel channel
			, Importer const & importer )
		{
			if ( name.length > 0 )
			{
				importer.loadTexture( Path{ string::stringCast< xchar >( name.C_Str() ) }
					, pass
					, channel );
			}
		}

		void doProcessPassTextures( Pass & pass
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
				String strGlob = string::stringCast< xchar >( difTexName.C_Str() ) + cuT( ".tga" );
				string::replace( strGlob, cuT( "/MI_CH_" ), cuT( "TX_CH_" ) );
				String strDiff = strGlob;
				String strNorm = strGlob;
				String strSpec = strGlob;
				String strOpac = strGlob;
				importer.loadTexture( Path{ string::replace( strDiff, cuT( "_Cine_" ), cuT( "_D_" ) ) }
					, pass
					, TextureChannel::eDiffuse );
				importer.loadTexture( Path{ string::replace( strNorm, cuT( "_Cine_" ), cuT( "_N_" ) ) }
					, pass
					, TextureChannel::eNormal );
				importer.loadTexture( Path{ string::replace( strSpec, cuT( "_Cine_" ), cuT( "_S_" ) ) }
					, pass
					, TextureChannel::eSpecular );
				importer.loadTexture( Path{ string::replace( strOpac, cuT( "_Cine_" ), cuT( "_A_" ) ) }
					, pass
					, TextureChannel::eOpacity );
			}
			else
			{
				doLoadTexture( difTexName, pass, TextureChannel::eDiffuse, importer );
				doLoadTexture( spcTexName, pass, TextureChannel::eSpecular, importer );
				doLoadTexture( emiTexName, pass, TextureChannel::eEmissive, importer );
				doLoadTexture( opaTexName, pass, TextureChannel::eOpacity, importer );
				doLoadTexture( shnTexName, pass, TextureChannel::eGloss, importer );

				if ( nmlTexName.length > 0 )
				{
					doLoadTexture( nmlTexName, pass, TextureChannel::eNormal, importer );

					if ( hgtTexName.length > 0 )
					{
						doLoadTexture( hgtTexName, pass, TextureChannel::eHeight, importer );
					}
				}
				else if ( hgtTexName.length > 0 )
				{
					doLoadTexture( hgtTexName, pass, TextureChannel::eNormal, importer );
				}
			}
		}

		std::map< Milliseconds, Point3r > doProcessVec3Keys( aiVectorKey const * const keys
			, uint32_t count
			, int64_t ticksPerMilliSecond
			, std::set< Milliseconds > & times )
		{
			std::map< Milliseconds, Point3r > result;

			for ( auto const & key : makeArrayView( keys, count ) )
			{
				auto time = Milliseconds{ int64_t( key.mTime * 1000 ) } / ticksPerMilliSecond;
				times.insert( time );
				result[time] = Point3r{ key.mValue.x, key.mValue.y, key.mValue.z };
			}

			return result;
		}

		std::map< Milliseconds, Quaternion > doProcessQuatKeys( aiQuatKey const * const keys
			, uint32_t count
			, int64_t ticksPerMilliSecond
			, std::set< Milliseconds > & times )
		{
			std::map< Milliseconds, Quaternion > result;

			for ( auto const & key : makeArrayView( keys, count ) )
			{
				auto time = Milliseconds{ int64_t( key.mTime * 1000 ) } / ticksPerMilliSecond;
				times.insert( time );
				result[time] = Quaternion::fromMatrix( Matrix4x4r{ Matrix3x3r{ &key.mValue.GetMatrix().Transpose().a1 } } );
			}

			return result;
		}

		SkeletonAnimationKeyFrame & doGetKeyFrame( Milliseconds const & time
			, SkeletonAnimation & animation
			, SkeletonAnimationKeyFrameMap & keyframes )
		{
			auto it = keyframes.find( time );

			if ( it == keyframes.end() )
			{
				it = keyframes.emplace( time
					, std::make_unique< SkeletonAnimationKeyFrame >( animation, time ) ).first;
			}

			return *it->second;
		}

		void doSynchroniseKeys( std::map< Milliseconds, Point3r > const & translates
			, std::map< Milliseconds, Point3r > const & scales
			, std::map< Milliseconds, Quaternion > const & rotates
			, std::set< Milliseconds > const & times
			, uint32_t fps
			, int64_t ticksPerMilliSecond
			, SkeletonAnimationObject & object
			, SkeletonAnimation & animation
			, SkeletonAnimationKeyFrameMap & keyframes )
		{
			InterpolatorT< Point3r, InterpolatorType::eLinear > pointInterpolator;
			InterpolatorT< Quaternion, InterpolatorType::eLinear > quatInterpolator;

			if ( ticksPerMilliSecond / 1000 >= fps )
			{
				for ( auto time : times )
				{
					Point3r translate = doCompute( time, pointInterpolator, translates );
					Point3r scale = doCompute( time, pointInterpolator, scales );
					Quaternion rotate = doCompute( time, quatInterpolator, rotates );
					doGetKeyFrame( time, animation, keyframes ).addAnimationObject( object
						, translate
						, rotate
						, scale );
				}
			}
			else
			{
				// Limit the key frames per second to 60, to spare RAM...
				Milliseconds step{ 1000 / std::min< int64_t >( 60, int64_t( fps ) ) };
				Milliseconds maxTime{ *times.rbegin() + step };

				for ( Milliseconds time{ 0 }; time < maxTime; time += step )
				{
					Point3r translate = doCompute( time, pointInterpolator, translates );
					Point3r scale = doCompute( time, pointInterpolator, scales );
					Quaternion rotate = doCompute( time, quatInterpolator, rotates );
					doGetKeyFrame( time, animation, keyframes ).addAnimationObject( object
						, translate
						, rotate
						, scale );
				}
			}
		}
	}

	//*********************************************************************************************

	castor::String const AssimpImporter::Name = cuT( "ASSIMP Importer" );

	AssimpImporter::AssimpImporter( Engine & engine )
		: Importer( engine )
		, m_anonymous( 0 )
	{
	}

	AssimpImporter::~AssimpImporter()
	{
	}

	ImporterUPtr AssimpImporter::create( Engine & engine )
	{
		return std::make_unique< AssimpImporter >( engine );
	}

	bool AssimpImporter::doImportScene( Scene & scene )
	{
		auto mesh = scene.getMeshCache().add( cuT( "Mesh_PLY" ) );
		bool result = doImportMesh( *mesh );

		if ( result )
		{
			SceneNodeSPtr node = scene.getSceneNodeCache().add( mesh->getName(), scene.getObjectRootNode() );
			GeometrySPtr geometry = scene.getGeometryCache().add( mesh->getName(), node, nullptr );
			geometry->setMesh( mesh );
			m_geometries.insert( { geometry->getName(), geometry } );
		}

		return result;
	}

	bool AssimpImporter::doImportMesh( Mesh & mesh )
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
		String normals;

		if ( m_parameters.get( cuT( "normals" ), normals ) )
		{
			if ( normals == cuT( "smooth" ) )
			{
				flags |= aiProcess_GenSmoothNormals;
			}
		}

		if ( m_parameters.get( cuT( "tangent_space" ), tangentSpace ) && tangentSpace )
		{
			flags |= aiProcess_CalcTangentSpace;
		}

		// And have it read the given file with some postprocessing
		aiScene const * aiScene = importer.ReadFile( string::stringCast< char >( m_fileName ), flags );

		if ( aiScene )
		{
			SkeletonSPtr skeleton = std::make_shared< Skeleton >( *mesh.getScene() );
			skeleton->setGlobalInverseTransform( Matrix4x4r( &aiScene->mRootNode->mTransformation.Transpose().Inverse().a1 ) );

			if ( aiScene->HasMeshes() )
			{
				bool create = true;

				for ( auto aiMesh : makeArrayView( aiScene->mMeshes, aiScene->mNumMeshes ) )
				{
					if ( create )
					{
						submesh = mesh.createSubmesh();
					}

					create = doProcessMesh( *mesh.getScene(), mesh, *skeleton, *aiMesh, *aiScene, *submesh );
				}

				if ( m_arrayBones.empty() )
				{
					skeleton.reset();
				}
				else
				{
					mesh.setSkeleton( skeleton );
				}

				if ( skeleton )
				{
					for ( auto aiAnimation : makeArrayView( aiScene->mAnimations, aiScene->mNumAnimations ) )
					{
						doProcessAnimation( mesh
							, m_fileName.getFileName()
							, *skeleton
							, *aiScene->mRootNode
							, *aiAnimation );
					}

					importer.FreeScene();

					if ( string::upperCase( m_fileName.getExtension() ) == cuT( "MD5MESH" ) )
					{
						// Workaround to load multiple animations with MD5 models.
						PathArray files;
						File::listDirectoryFiles( m_fileName.getPath(), files );

						for ( auto file : files )
						{
							if ( string::lowerCase( file.getExtension() ) == cuT( "md5anim" ) )
							{
								// The .md5anim with the same name as the .md5mesh has already been loaded by assimp.
								if ( file.getFileName() != m_fileName.getFileName() )
								{
									auto scene = importer.ReadFile( file, flags );

									for ( auto aiAnimation : makeArrayView( scene->mAnimations, scene->mNumAnimations ) )
									{
										doProcessAnimation( mesh
											, file.getFileName()
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
			Logger::logError( std::stringstream() << "Scene import failed : " << importer.GetErrorString() );
		}

		return result;
	}

	bool AssimpImporter::doProcessMesh( Scene & scene
		, Mesh & mesh
		, Skeleton & skeleton
		, aiMesh const & aiMesh
		, aiScene const & aiScene
		, Submesh & submesh )
	{
		bool result = false;
		MaterialSPtr material;
		std::clog << cuT( "Mesh found: [" ) << aiMesh.mName.C_Str() << cuT( "]" ) << std::endl;

		if ( aiMesh.mMaterialIndex < aiScene.mNumMaterials )
		{
			material = doProcessMaterial( scene, *aiScene.mMaterials[aiMesh.mMaterialIndex] );
			std::clog << cuT( "  Material: [" ) << aiMesh.mMaterialIndex << cuT( "]" ) << std::endl;
		}

		if ( aiMesh.HasFaces() && aiMesh.HasPositions() && material )
		{
			submesh.setDefaultMaterial( material );
			submesh.addPoints( doCreateVertexBuffer( aiMesh ) );

			if ( aiMesh.HasBones() )
			{
				std::vector< VertexBoneData > bonesData( aiMesh.mNumVertices );
				doProcessBones( skeleton, aiMesh.mBones, aiMesh.mNumBones, bonesData );
				auto bones = std::make_shared< BonesComponent >( submesh );
				bones->addBoneDatas( bonesData );
				submesh.addComponent( bones );
			}

			auto mapping = std::make_shared< TriFaceMapping >( submesh );

			for ( auto face : makeArrayView( aiMesh.mFaces, aiMesh.mNumFaces ) )
			{
				if ( face.mNumIndices == 3 )
				{
					mapping->addFace( face.mIndices[0], face.mIndices[1], face.mIndices[2] );
				}
			}

			if ( !aiMesh.mNormals )
			{
				mapping->computeNormals( true );
			}
			else if ( !aiMesh.mTangents )
			{
				mapping->computeTangentsFromNormals();
			}

			submesh.setIndexMapping( mapping );

			if ( aiScene.HasAnimations() )
			{
				for( auto aiAnimation : makeArrayView( aiScene.mAnimations, aiScene.mNumAnimations ) )
				{
					auto it = std::find_if( aiAnimation->mMeshChannels
						, aiAnimation->mMeshChannels + aiAnimation->mNumMeshChannels
						, [this, &aiMesh, &submesh]( aiMeshAnim const * p_aiMeshAnim )
						{
							return p_aiMeshAnim->mName == aiMesh.mName;
						} );

					if ( it != aiAnimation->mMeshChannels + aiAnimation->mNumMeshChannels )
					{
						doProcessAnimationMeshes( mesh, submesh, aiMesh, *( *it ) );
					}
				}
			}

			result = true;
		}

		return result;
	}

	void AssimpImporter::doProcessAnimationMeshes( Mesh & mesh
		, Submesh & submesh
		, aiMesh const & aiMesh
		, aiMeshAnim const & aiMeshAnim )
	{
		if ( aiMeshAnim.mNumKeys )
		{
			String name{ string::stringCast< xchar >( aiMeshAnim.mName.C_Str() ) };
			Logger::logDebug( cuT( "Mesh animation found: [" ) + name + cuT( "]" ) );
			auto & animation = mesh.createAnimation( name );
			MeshAnimationSubmesh animSubmesh{ animation, submesh };
			animation.addChild( std::move( animSubmesh ) );

			for ( auto aiKey : makeArrayView( aiMeshAnim.mKeys, aiMeshAnim.mNumKeys ) )
			{
				MeshAnimationKeyFrameUPtr keyFrame = std::make_unique< MeshAnimationKeyFrame >( animation
					, Milliseconds{ int64_t( aiKey.mTime * 1000 ) } );
				keyFrame->addSubmeshBuffer( submesh, doCreateVertexBuffer( *aiMesh.mAnimMeshes[aiKey.mValue] ) );
				animation.addKeyFrame( std::move( keyFrame ) );
			}
		}
	}

	MaterialSPtr AssimpImporter::doProcessMaterial( Scene & scene
		, aiMaterial const & aiMaterial )
	{
		MaterialSPtr result;
		auto & cache = scene.getMaterialView();
		aiString mtlname;
		aiMaterial.Get( AI_MATKEY_NAME, mtlname );
		String name = string::stringCast< xchar >( mtlname.C_Str() );

		if ( name.empty() )
		{
			name = m_fileName.getFileName() + string::toString( m_anonymous++ );
		}

		if ( cache.has( name ) )
		{
			result = cache.find( name );
		}
		else
		{
			result = cache.add( name, MaterialType::eLegacy );
			result->createPass();
			auto pass = result->getTypedPass< MaterialType::eLegacy >( 0 );

			doProcessPassBaseComponents( *pass, aiMaterial );
			doProcessPassTextures( *pass, aiMaterial, *this );
		}

		return result;
	}

	BoneSPtr AssimpImporter::doAddBone( String const & name
		, Matrix4x4r const & offset
		, Skeleton & skeleton
		, uint32_t & index )
	{
		BoneSPtr bone = skeleton.createBone( name );
		bone->setOffsetMatrix( offset );
		index = uint32_t( m_arrayBones.size() );
		m_arrayBones.push_back( bone );
		m_mapBoneByID[name] = index;
		return bone;
	}

	void AssimpImporter::doProcessBones( Skeleton & skeleton
		, aiBone const * const * aiBones
		, uint32_t count
		, std::vector< VertexBoneData > & arrayVertices )
	{
		for ( uint32_t i = 0; i < count; ++i )
		{
			aiBone const & aiBone = *aiBones[i];
			String name = string::stringCast< xchar >( aiBone.mName.C_Str() );
			uint32_t index;

			if ( m_mapBoneByID.find( name ) == m_mapBoneByID.end() )
			{
				auto mtx = aiBone.mOffsetMatrix;
				doAddBone( name, Matrix4x4r{ &mtx.Transpose().a1 }, skeleton, index );
			}
			else
			{
				index = m_mapBoneByID[name];
				aiMatrix4x4 mtx{ aiBone.mOffsetMatrix };
				ENSURE( m_arrayBones[index]->getOffsetMatrix() == Matrix4x4r( &mtx.Transpose().a1 ) );
			}

			for ( auto weight : makeArrayView( aiBone.mWeights, aiBone.mNumWeights ) )
			{
				arrayVertices[weight.mVertexId].addBoneData( index, real( weight.mWeight ) );
			}
		}
	}

	void AssimpImporter::doProcessAnimation( Mesh & mesh
		, String const & animName
		, Skeleton & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation )
	{
		String name{ string::stringCast< xchar >( aiAnimation.mName.C_Str() ) };
		Logger::logDebug( cuT( "Skeleton animation found: " ) + name );

		if ( name.empty() )
		{
			name = animName;
		}

		auto & animation = skeleton.createAnimation( name );
		int64_t ticksPerMilliSecond = int64_t( aiAnimation.mTicksPerSecond ? aiAnimation.mTicksPerSecond : 25 );
		SkeletonAnimationKeyFrameMap keyframes;
		SkeletonAnimationObjectSet notAnimated;
		doProcessAnimationNodes( mesh
			, animation
			, ticksPerMilliSecond
			, skeleton
			, aiNode
			, aiAnimation
			, nullptr
			, keyframes
			, notAnimated );

		for ( auto & object : notAnimated )
		{
			for ( auto & keyFrame : keyframes )
			{
				if ( !keyFrame.second->hasObject( *object ) )
				{
					keyFrame.second->addAnimationObject( *object, object->getNodeTransform() );
				}
			}
		}

		for ( auto & keyFrame : keyframes )
		{
			animation.addKeyFrame( std::move( keyFrame.second ) );
		}

		animation.updateLength();
	}

	void AssimpImporter::doProcessAnimationNodes( Mesh & mesh
		, SkeletonAnimation & animation
		, int64_t ticksPerMilliSecond
		, Skeleton & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation
		, SkeletonAnimationObjectSPtr parent
		, SkeletonAnimationKeyFrameMap & keyFrames
		, SkeletonAnimationObjectSet & notAnimated )
	{
		String name = string::stringCast< xchar >( aiNode.mName.data );
		const aiNodeAnim * aiNodeAnim = doFindNodeAnim( aiAnimation, name );
		SkeletonAnimationObjectSPtr object;
		auto itBone = m_mapBoneByID.find( name );

		if ( !aiNodeAnim
			&& itBone == m_mapBoneByID.end()
			&& aiNode.mNumMeshes )
		{
			uint32_t index;
			auto bone = doAddBone( name, Matrix4x4r{ 1.0_r }, skeleton, index );

			for ( auto const & aiMesh : makeArrayView( aiNode.mMeshes, aiNode.mNumMeshes ) )
			{
				auto submesh = mesh.getSubmesh( aiMesh );
				REQUIRE( submesh != nullptr );

				if ( !submesh->hasComponent( BonesComponent::Name ) )
				{
					auto bones = std::make_shared< BonesComponent >( *submesh );
					std::vector< VertexBoneData > arrayBones( submesh->getPointsCount() );

					for ( auto & boneData : arrayBones )
					{
						boneData.addBoneData( index, 1.0_r );
					}

					bones->addBoneDatas( arrayBones );
					submesh->addComponent( bones );
				}
			}

			itBone = m_mapBoneByID.find( name );
		}

		if ( itBone != m_mapBoneByID.end() )
		{
			auto bone = m_arrayBones[itBone->second];
			object = animation.addObject( bone, parent );

			if ( parent && parent->getType() == SkeletonAnimationObjectType::eBone )
			{
				skeleton.setBoneParent( bone, std::static_pointer_cast< SkeletonAnimationBone >( parent )->getBone() );
			}
		}
		else
		{
			object = animation.addObject( aiNode.mName.C_Str(), parent );
		}

		if ( object )
		{
			if ( parent && object != parent )
			{
				parent->addChild( object );
			}

			object->setNodeTransform( Matrix4x4r( &aiNode.mTransformation.a1 ).getTransposed() );
		}

		if ( aiNodeAnim )
		{
			doProcessAnimationNodeKeys( *aiNodeAnim
				, ticksPerMilliSecond
				, *object
				, animation
				, keyFrames );
		}
		else
		{
			notAnimated.insert( object );
		}

		for ( auto node : makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doProcessAnimationNodes( mesh
				, animation
				, ticksPerMilliSecond
				, skeleton
				, *node
				, aiAnimation
				, object
				, keyFrames
				, notAnimated );
		}
	}

	void AssimpImporter::doProcessAnimationNodeKeys( aiNodeAnim const & aiNodeAnim
		, int64_t ticksPerMilliSecond
		, SkeletonAnimationObject & object
		, SkeletonAnimation & animation
		, SkeletonAnimationKeyFrameMap & keyframes )
	{
		std::set< Milliseconds > times;

		auto translates = doProcessVec3Keys( aiNodeAnim.mPositionKeys
			, aiNodeAnim.mNumPositionKeys
			, ticksPerMilliSecond
			, times );
		auto scales = doProcessVec3Keys( aiNodeAnim.mScalingKeys
			, aiNodeAnim.mNumScalingKeys
			, ticksPerMilliSecond
			, times );
		auto rotates = doProcessQuatKeys( aiNodeAnim.mRotationKeys
			, aiNodeAnim.mNumRotationKeys
			, ticksPerMilliSecond
			, times );
		doSynchroniseKeys( translates
			, scales
			, rotates
			, times
			, getEngine()->getRenderLoop().getWantedFps()
			, ticksPerMilliSecond
			, object
			, animation
			, keyframes );
	}

	//*********************************************************************************************
}
