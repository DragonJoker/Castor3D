/*
See LICENSE file in root folder
*/
#ifndef ___C3DAssimp_Helpers___
#define ___C3DAssimp_Helpers___

#include <Castor3D/Animation/Interpolator.hpp>
#include <Castor3D/Material/Texture/TextureModule.hpp>
#include <Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp>

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d_assimp
{
	inline c3d::String makeString( aiString const & name )
	{
		return c3d::makeString( name.C_Str() );
	}

	inline c3d::MbString toUtf8( aiString const & name )
	{
		return c3d::toUtf8( makeString( name ) );
	}

	inline c3d::String normalizeName( c3d::String const & name )
	{
		return c3d::File::normaliseFileName( name, cuT( "-"_sv ) );
	}

	inline c3d::Matrix4x4f fromAssimp( aiMatrix4x4 const & aiMatrix )
	{
		c3d::Array< float, 16u > data
			{ aiMatrix.a1, aiMatrix.b1, aiMatrix.c1, aiMatrix.d1
			, aiMatrix.a2, aiMatrix.b2, aiMatrix.c2, aiMatrix.d2
			, aiMatrix.a3, aiMatrix.b3, aiMatrix.c3, aiMatrix.d3
			, aiMatrix.a4, aiMatrix.b4, aiMatrix.c4, aiMatrix.d4 };
		return c3d::Matrix4x4f{ data.data() };
	}

	inline c3d::Matrix4x4f fromAssimp( c3d::Point3f const & direction
		, c3d::Point3f const & up )
	{
		c3d::Matrix4x4f result;
		result.setIdentity();
		c3d::Point3f xaxis = c3d::point::getNormalised( c3d::point::cross( up, direction ) );
		c3d::Point3f yaxis = c3d::point::getNormalised( c3d::point::cross( direction, xaxis ) );

		result.setColumn( 0u, { xaxis->x, yaxis->x, direction->x, 0.0f } );
		result.setColumn( 1u, { xaxis->y, yaxis->y, direction->y, 0.0f } );
		result.setColumn( 2u, { xaxis->z, yaxis->z, direction->z, 0.0f } );

		return result;
	}

	inline c3d::Milliseconds fromAssimp( double ticks
		, int64_t ticksPerSecond )
	{
		// Turn ticks to seconds.
		auto time = ticks / double( ticksPerSecond );
		// Turn seconds to milliseconds.
		return c3d::Milliseconds{ int64_t( time * 1000.0 ) };
	}

	inline c3d::Point3f fromAssimp( aiVector3D const & v )
	{
		return c3d::Point3f{ v.x, v.y, v.z };
	}

	inline c3d::WrapMode fromAssimp( aiTextureMapMode v )
	{
		switch ( v )
		{
		case aiTextureMapMode_Wrap:
			return c3d::WrapMode::eRepeat;
		case aiTextureMapMode_Clamp:
			return c3d::WrapMode::eClampToEdge;
		case aiTextureMapMode_Decal:
			return c3d::WrapMode::eClampToBorder;
		case aiTextureMapMode_Mirror:
			return c3d::WrapMode::eMirroredRepeat;
		default:
			return c3d::WrapMode::eRepeat;
		}
	}

	enum class GlFilter
		: uint32_t
	{
		NEAREST = 0x2600,
		LINEAR = 0x2601,
		NEAREST_MIPMAP_NEAREST = 0x2700,
		LINEAR_MIPMAP_NEAREST = 0x2701,
		NEAREST_MIPMAP_LINEAR = 0x2702,
		LINEAR_MIPMAP_LINEAR = 0x2703,
	};

	inline c3d::FilterMode fromAssimp( GlFilter const & v )
	{
		switch ( v )
		{
		case GlFilter::NEAREST:
		case GlFilter::NEAREST_MIPMAP_NEAREST:
		case GlFilter::NEAREST_MIPMAP_LINEAR:
			return c3d::FilterMode::eNearest;
		default:
			return c3d::FilterMode::eLinear;
		}
	}

	inline c3d::MipmapMode getMipFilter( GlFilter const & v )
	{
		switch ( v )
		{
		case GlFilter::NEAREST:
		case GlFilter::NEAREST_MIPMAP_NEAREST:
		case GlFilter::LINEAR_MIPMAP_NEAREST:
			return c3d::MipmapMode::eNearest;
		default:
			return c3d::MipmapMode ::eLinear;
		}
	}

	inline c3d::Quaternion fromAssimp( aiQuaternion const & v )
	{
		c3d::Quaternion result;
		result->x = v.x;
		result->y = v.y;
		result->z = v.z;
		result->w = v.w;
		return result;
	}

	inline c3d::String getLongestCommonSubstring( c3d::String const & a, c3d::String const & b )
	{
		auto result = c3d::string::getLongestCommonSubstring( a, b );
		return c3d::string::trim( result
			, true
			, true
			, c3d::StringView{ cuT( " \r\t-_/\\|*$<>[](){}" ) } );
	}

	inline bool isValidMesh( aiMesh const & mesh )
	{
		auto faces = c3d::makeArrayView( mesh.mFaces, mesh.mNumFaces );
		auto count = uint32_t( std::count_if( faces.begin()
			, faces.end()
			, []( aiFace const & face )
			{
				return face.mNumIndices == 3
					|| face.mNumIndices == 4;
			} ) );
		return count > 0 && mesh.HasPositions();
	}

	template< typename aiMeshType >
	inline void createVertexBuffer( aiMeshType const & aiMesh
		, c3d::Point3fArray & positions
		, c3d::Point3fArray & normals
		, c3d::Point4fArray & tangents
		, c3d::Point3fArray & bitangents
		, c3d::Point3fArray & texcoords0
		, c3d::Point3fArray & texcoords1
		, c3d::Point3fArray & texcoords2
		, c3d::Point3fArray & texcoords3
		, c3d::Point3fArray & colours )
	{
		uint32_t index{ 0u };

		if ( aiMesh.HasPositions() )
		{
			positions.resize( aiMesh.mNumVertices );

			for ( auto & pos : positions )
			{
				pos[0] = float( aiMesh.mVertices[index].x );
				pos[1] = float( aiMesh.mVertices[index].y );
				pos[2] = float( aiMesh.mVertices[index].z );
				++index;
			}
		}

		if ( aiMesh.HasNormals() )
		{
			normals.resize( aiMesh.mNumVertices );
			index = 0u;

			for ( auto & nml : normals )
			{
				nml[0] = float( aiMesh.mNormals[index].x );
				nml[1] = float( aiMesh.mNormals[index].y );
				nml[2] = float( aiMesh.mNormals[index].z );
				++index;
			}
		}

		if ( aiMesh.HasTextureCoords( 0 ) )
		{
			texcoords0.resize( aiMesh.mNumVertices );
			index = 0u;

			for ( auto & tex : texcoords0 )
			{
				tex[0] = float( aiMesh.mTextureCoords[0][index].x );
				tex[1] = float( aiMesh.mTextureCoords[0][index].y );
				tex[2] = float( aiMesh.mTextureCoords[0][index].z );
				++index;
			}
		}

		if ( aiMesh.HasTextureCoords( 1 ) )
		{
			texcoords1.resize( aiMesh.mNumVertices );
			index = 0u;

			for ( auto & tex : texcoords1 )
			{
				tex[0] = float( aiMesh.mTextureCoords[1][index].x );
				tex[1] = float( aiMesh.mTextureCoords[1][index].y );
				tex[2] = float( aiMesh.mTextureCoords[1][index].z );
				++index;
			}
		}

		if ( aiMesh.HasTextureCoords( 2 ) )
		{
			texcoords2.resize( aiMesh.mNumVertices );
			index = 0u;

			for ( auto & tex : texcoords2 )
			{
				tex[0] = float( aiMesh.mTextureCoords[2][index].x );
				tex[1] = float( aiMesh.mTextureCoords[2][index].y );
				tex[2] = float( aiMesh.mTextureCoords[2][index].z );
				++index;
			}
		}

		if ( aiMesh.HasTextureCoords( 3 ) )
		{
			texcoords3.resize( aiMesh.mNumVertices );
			index = 0u;

			for ( auto & tex : texcoords3 )
			{
				tex[0] = float( aiMesh.mTextureCoords[3][index].x );
				tex[1] = float( aiMesh.mTextureCoords[3][index].y );
				tex[2] = float( aiMesh.mTextureCoords[3][index].z );
				++index;
			}
		}

		if ( aiMesh.HasVertexColors( 0u ) )
		{
			colours.resize( aiMesh.mNumVertices );
			index = 0u;

			for ( auto & col : colours )
			{
				col[0] = float( aiMesh.mColors[0][index].r );
				col[1] = float( aiMesh.mColors[0][index].g );
				col[2] = float( aiMesh.mColors[0][index].b );
				++index;
			}
		}

		if ( aiMesh.HasTangentsAndBitangents()
			&& ( aiMesh.HasTextureCoords( 0 )
				|| aiMesh.HasTextureCoords( 1 )
				|| aiMesh.HasTextureCoords( 2 )
				|| aiMesh.HasTextureCoords( 3 ) ) )
		{
			tangents.resize( aiMesh.mNumVertices );
			index = 0u;

			for ( auto & tan : tangents )
			{
				tan[0] = float( aiMesh.mTangents[index].x );
				tan[1] = float( aiMesh.mTangents[index].y );
				tan[2] = float( aiMesh.mTangents[index].z );
				tan[3] = 1.0f;
				++index;
			}

			bitangents.resize( aiMesh.mNumVertices );
			index = 0u;

			for ( auto & bit : bitangents )
			{
				bit[0] = float( aiMesh.mBitangents[index].x );
				bit[1] = float( aiMesh.mBitangents[index].y );
				bit[2] = float( aiMesh.mBitangents[index].z );
				++index;
			}
		}
	}

	inline c3d::Vector< c3d::SubmeshAnimationBuffer > gatherMeshAnimBuffers( c3d::Point3fArray const & positions
		, c3d::Point3fArray const & normals
		, c3d::Point4fArray const & tangents
		, c3d::Point3fArray const & bitangents
		, c3d::Point3fArray const & texcoords0
		, c3d::Point3fArray const & texcoords1
		, c3d::Point3fArray const & texcoords2
		, c3d::Point3fArray const & texcoords3
		, c3d::Point3fArray const & colours
		, c3d::ArrayView< aiAnimMesh * > animMeshes )
	{
		c3d::Vector< c3d::SubmeshAnimationBuffer > result;

		for ( auto aiAnimMesh : animMeshes )
		{
			c3d::SubmeshAnimationBuffer buffer;
			createVertexBuffer( *aiAnimMesh
				, buffer.positions
				, buffer.normals
				, buffer.tangents
				, buffer.bitangents
				, buffer.texcoords0
				, buffer.texcoords1
				, buffer.texcoords2
				, buffer.texcoords3
				, buffer.colours );

			if ( aiAnimMesh->HasPositions() )
			{
				auto it = buffer.positions.begin();

				for ( auto & ref : positions )
				{
					*it -= ref;
					++it;
				}
			}

			if ( aiAnimMesh->HasNormals() )
			{
				auto it = buffer.normals.begin();

				for ( auto & ref : normals )
				{
					*it -= ref;
					++it;
				}
			}

			if ( aiAnimMesh->HasTextureCoords( 0u ) )
			{
				auto it = buffer.texcoords0.begin();

				for ( auto & ref : texcoords0 )
				{
					*it -= ref;
					++it;
				}
			}

			if ( aiAnimMesh->HasTextureCoords( 1u ) )
			{
				auto it = buffer.texcoords1.begin();

				for ( auto & ref : texcoords1 )
				{
					*it -= ref;
					++it;
				}
			}

			if ( aiAnimMesh->HasTextureCoords( 2u ) )
			{
				auto it = buffer.texcoords2.begin();

				for ( auto & ref : texcoords2 )
				{
					*it -= ref;
					++it;
				}
			}

			if ( aiAnimMesh->HasTextureCoords( 3u ) )
			{
				auto it = buffer.texcoords3.begin();

				for ( auto & ref : texcoords3 )
				{
					*it -= ref;
					++it;
				}
			}

			if ( aiAnimMesh->HasTangentsAndBitangents()
				&& ( aiAnimMesh->HasTextureCoords( 0u )
					|| aiAnimMesh->HasTextureCoords( 1u )
					|| aiAnimMesh->HasTextureCoords( 2u )
					|| aiAnimMesh->HasTextureCoords( 3u ) ) )
			{
				auto tit = buffer.tangents.begin();

				for ( auto & ref : tangents )
				{
					*tit -= ref;
					++tit;
				}

				auto bit = buffer.bitangents.begin();

				for ( auto & ref : bitangents )
				{
					*bit -= ref;
					++bit;
				}
			}

			if ( aiAnimMesh->HasVertexColors( 0u ) )
			{
				auto it = buffer.colours.begin();

				for ( auto & ref : colours )
				{
					*it -= ref;
					++it;
				}
			}

			result.emplace_back( c3d::move( buffer ) );
		}

		return result;
	}

	inline std::tuple< uint32_t, double, double > getNodeAnimFrameTicks( aiNodeAnim const & aiNodeAnim )
	{
		return { std::max( { aiNodeAnim.mNumPositionKeys
				, aiNodeAnim.mNumRotationKeys
				, aiNodeAnim.mNumScalingKeys } )
			, std::min( { ( aiNodeAnim.mNumPositionKeys > 0
					? aiNodeAnim.mPositionKeys[aiNodeAnim.mNumPositionKeys - 1u].mTime
					: std::numeric_limits< double >::max() )
				, ( aiNodeAnim.mNumRotationKeys > 0
					? aiNodeAnim.mRotationKeys[aiNodeAnim.mNumRotationKeys - 1u].mTime
					: std::numeric_limits< double >::max() )
				, ( aiNodeAnim.mNumScalingKeys > 0
					? aiNodeAnim.mScalingKeys[aiNodeAnim.mNumScalingKeys - 1u].mTime
					: std::numeric_limits< double >::max() ) } )
			, std::max( { ( aiNodeAnim.mNumPositionKeys > 0
					? aiNodeAnim.mPositionKeys[aiNodeAnim.mNumPositionKeys - 1u].mTime
					: 0.0 )
				, ( aiNodeAnim.mNumRotationKeys > 0
					? aiNodeAnim.mRotationKeys[aiNodeAnim.mNumRotationKeys - 1u].mTime
					: 0.0 )
				, ( aiNodeAnim.mNumScalingKeys > 0
					? aiNodeAnim.mScalingKeys[aiNodeAnim.mNumScalingKeys - 1u].mTime
					: 0.0 ) } ) };
	}

	inline std::tuple< uint32_t, double, double > getAnimationFrameTicks( aiAnimation const & aiAnimation )
	{
		if ( aiAnimation.mDuration > 0.0 )
		{
			uint32_t count = 0u;
			for ( auto nodeAnim : c3d::makeArrayView( aiAnimation.mChannels, aiAnimation.mNumChannels ) )
			{
				count = std::max( { count
					, nodeAnim->mNumPositionKeys
					, nodeAnim->mNumRotationKeys
					, nodeAnim->mNumScalingKeys } );
			}
			return { count, 0.0, aiAnimation.mDuration };
		}

		uint32_t count = 0u;
		double maxTicks = 0.0;
		double minTicks = std::numeric_limits< double >::max();

		for ( auto nodeAnim : c3d::makeArrayView( aiAnimation.mChannels, aiAnimation.mNumChannels ) )
		{
			count = std::max( { count
				, nodeAnim->mNumPositionKeys
				, nodeAnim->mNumRotationKeys
				, nodeAnim->mNumScalingKeys } );
			minTicks = std::min( { minTicks
				, ( nodeAnim->mNumPositionKeys > 0
					? nodeAnim->mPositionKeys[nodeAnim->mNumPositionKeys - 1u].mTime
					: std::numeric_limits< double >::max() )
				, ( nodeAnim->mNumRotationKeys > 0
					? nodeAnim->mRotationKeys[nodeAnim->mNumRotationKeys - 1u].mTime
					: std::numeric_limits< double >::max() )
				, ( nodeAnim->mNumScalingKeys > 0
					? nodeAnim->mScalingKeys[nodeAnim->mNumScalingKeys - 1u].mTime
					: std::numeric_limits< double >::max() ) } );
			maxTicks = std::max( { maxTicks
				, ( nodeAnim->mNumPositionKeys > 0
					? nodeAnim->mPositionKeys[nodeAnim->mNumPositionKeys - 1u].mTime
					: 0.0 )
				, ( nodeAnim->mNumRotationKeys > 0
					? nodeAnim->mRotationKeys[nodeAnim->mNumRotationKeys - 1u].mTime
					: 0.0 )
				, ( nodeAnim->mNumScalingKeys > 0
					? nodeAnim->mScalingKeys[nodeAnim->mNumScalingKeys - 1u].mTime
					: 0.0 ) } );
		}

		return { count, std::max( 0.0, minTicks ), maxTicks };
	}

	inline aiNode const * findMeshNode( uint32_t meshIndex
		, aiNode const & node )
	{
		auto meshes = c3d::makeArrayView( node.mMeshes, node.mNumMeshes );

		if ( auto meshIt = std::find( meshes.begin(), meshes.end(), meshIndex );
			meshIt != meshes.end() )
		{
			return &node;
		}

		aiNode const * result{};
		auto children = c3d::makeArrayView( node.mChildren, node.mNumChildren );
		auto childIt = children.begin();

		while ( !result && childIt != children.end() )
		{
			result = findMeshNode( meshIndex, **childIt );
			++childIt;
		}

		return result;
	}

	inline aiNode const * findRootSkeletonNode( aiNode const & sceneRootNode
		, c3d::ArrayView< aiBone * > bones
		, aiNode const * meshNode )
	{
		c3d::Vector< aiNode const * > bonesRootNodes;
		auto insertNode = [&bonesRootNodes]( aiNode const * node )
		{
			if ( std::all_of( bonesRootNodes.begin()
				, bonesRootNodes.end()
				, [node]( aiNode const * lookup )
				{
					return lookup->FindNode( node->mName ) == nullptr;
				} ) )
			{
				std::erase_if( bonesRootNodes
					, [node]( aiNode const * lookup )
					{
						return node->FindNode( lookup->mName ) != nullptr;
					} );
				bonesRootNodes.push_back( node );
			}
		};

		for ( auto bone : bones )
		{
			auto node = sceneRootNode.FindNode( bone->mName );
			insertNode( node );

			while ( node->mParent )
			{
				node = node->mParent;

				if ( node == meshNode
					|| node->FindNode( meshNode->mName ) )
				{
					break;
				}

				insertNode( node );
			}
		}

		if ( bonesRootNodes.size() > 1u )
		{
			bonesRootNodes.clear();
			bonesRootNodes.push_back( &sceneRootNode );
		}

		return *bonesRootNodes.begin();
	}

	inline c3d::String findSkeletonName( c3d::StringMap< c3d::Matrix4x4f > const & bonesNodes
		, aiNode const & rootNode )
	{
		c3d::Vector< aiNode const * > bones;
		c3d::Vector< aiNode const * > work;
		work.push_back( &rootNode );
		auto name = makeString( rootNode.mName );

		while ( !work.empty() )
		{
			auto node = work.back();
			work.pop_back();

			for ( auto child : c3d::makeArrayView( node->mChildren, node->mNumChildren ) )
			{
				work.push_back( child );
			}

			if ( auto nodeName = makeString( node->mName );
				bonesNodes.end() != bonesNodes.find( nodeName ) )
			{
				name = getLongestCommonSubstring( name, nodeName );
			}

			if ( name.empty() )
			{
				return makeString( rootNode.mName );
			}
		}

		if ( name.empty() )
		{
			name = makeString( rootNode.mName );
		}

		return normalizeName( name );
	}

	template< typename KeyT >
	struct KeyDataTyperT;

	template<>
	struct KeyDataTyperT< aiVectorKey >
	{
		using Type = c3d::Point3f;
	};

	template<>
	struct KeyDataTyperT< aiQuatKey >
	{
		using Type = c3d::Quaternion;
	};

	template< typename KeyT >
	using KeyDataTypeT = typename KeyDataTyperT< KeyT >::Type;

	template< typename KeyT >
	inline c3d::Map< c3d::Milliseconds, KeyDataTypeT< KeyT > > processKeys( c3d::ArrayView< KeyT > const & keys
		, c3d::Milliseconds minTime
		, c3d::Milliseconds maxTime
		, int64_t ticksPerSecond
		, c3d::Set< c3d::Milliseconds > & times )
	{
		c3d::Map< c3d::Milliseconds, KeyDataTypeT< KeyT > > result;

		for ( auto const & key : keys )
		{
			if ( auto time = fromAssimp( key.mTime, ticksPerSecond );
				time >= minTime && time <= maxTime )
			{
				times.insert( time );
				result.emplace( time, fromAssimp( key.mValue ) );
			}
		}

		return result;
	}

	template< typename aiAnimT
		, typename AnimationT
		, typename KeyFrameT
		, typename FuncT >
	inline void processAnimationNodeKeys( aiAnimT const & aiAnim
		, uint32_t wantedFps
		, c3d::Milliseconds minTime
		, c3d::Milliseconds maxTime
		, c3d::NodeTransform const & defaultTransform
		, int64_t ticksPerSecond
		, AnimationT & animation
		, c3d::Map< c3d::Milliseconds, c3d::UniquePtr< KeyFrameT > > & keyframes
		, FuncT fillKeyFrame )
	{
		c3d::Set< c3d::Milliseconds > times;
		auto translates = processKeys( c3d::makeArrayView( aiAnim.mPositionKeys, aiAnim.mNumPositionKeys )
			, minTime, maxTime, ticksPerSecond
			, times );
		auto scales = processKeys( c3d::makeArrayView( aiAnim.mScalingKeys, aiAnim.mNumScalingKeys )
			, minTime, maxTime, ticksPerSecond
			, times );
		auto rotates = processKeys( c3d::makeArrayView( aiAnim.mRotationKeys, aiAnim.mNumRotationKeys )
			, minTime, maxTime, ticksPerSecond
			, times );
		c3d::AnimationImporter::synchroniseKeys( translates, rotates, scales
			, defaultTransform, wantedFps, minTime, maxTime
			, animation, keyframes, fillKeyFrame );
	}
}

#endif
