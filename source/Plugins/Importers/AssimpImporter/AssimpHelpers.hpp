/*
See LICENSE file in root folder
*/
#ifndef ___C3DAssimp_Helpers___
#define ___C3DAssimp_Helpers___

#include <Castor3D/Animation/Interpolator.hpp>
#include <Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp>

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/scene.h>
#include <assimp/types.h>
#pragma warning( pop )

namespace c3d_assimp
{
	static castor::String makeString( aiString const & name )
	{
		return castor::string::stringCast< castor::xchar >( name.C_Str() );
	}

	static castor::String normalizeName( castor::String name )
	{
		castor::string::replace( name, "\\", "-" );
		return castor::string::replace( name, "/", "-" );
	}

	static castor::Matrix4x4f fromAssimp( aiMatrix4x4 const & aiMatrix )
	{
		std::array< float, 16u > data
			{ aiMatrix.a1, aiMatrix.b1, aiMatrix.c1, aiMatrix.d1
			, aiMatrix.a2, aiMatrix.b2, aiMatrix.c2, aiMatrix.d2
			, aiMatrix.a3, aiMatrix.b3, aiMatrix.c3, aiMatrix.d3
			, aiMatrix.a4, aiMatrix.b4, aiMatrix.c4, aiMatrix.d4 };
		return castor::Matrix4x4f{ data.data() };
	}

	static castor::Matrix4x4f fromAssimp( aiMatrix3x3 const & aiMatrix )
	{
		std::array< float, 9u > data
			{ aiMatrix.a1, aiMatrix.b1, aiMatrix.c1
			, aiMatrix.a2, aiMatrix.b2, aiMatrix.c2
			, aiMatrix.a3, aiMatrix.b3, aiMatrix.c3 };
		return castor::Matrix4x4f{ castor::Matrix3x3f{ data.data() } };
	}

	static castor::Matrix4x4f fromAssimp( castor::Point3f const & direction
		, castor::Point3f const & up )
	{
		castor::Matrix4x4f result;
		result.setIdentity();
		castor::Point3f xaxis = castor::point::getNormalised( castor::point::cross( up, direction ) );
		castor::Point3f yaxis = castor::point::getNormalised( castor::point::cross( direction, xaxis ) );

		result.setColumn( 0u, { xaxis->x, yaxis->x, direction->x, 0.0f } );
		result.setColumn( 1u, { xaxis->y, yaxis->y, direction->y, 0.0f } );
		result.setColumn( 2u, { xaxis->z, yaxis->z, direction->z, 0.0f } );

		return result;
	}

	static castor::Milliseconds fromAssimp( double ticks
		, int64_t ticksPerSecond )
	{
		// Turn ticks to seconds.
		auto time = ticks / double( ticksPerSecond );
		// Turn seconds to milliseconds.
		return castor::Milliseconds{ int64_t( time * 1000.0 ) };
	}

	static castor::Point3f fromAssimp( aiVector3D const & v )
	{
		return castor::Point3f{ v.x, v.y, v.z };
	}

	static castor::Quaternion fromAssimp( aiQuaternion const & v )
	{
		castor::Quaternion result;
		result.quat.x = v.x;
		result.quat.y = v.y;
		result.quat.z = v.z;
		result.quat.w = v.w;
		return result;
	}

	template< typename aiMeshType >
	static void createVertexBuffer( aiMeshType const & aiMesh
		, castor::Point3fArray & positions
		, castor::Point3fArray & normals
		, castor::Point3fArray & tangents
		, castor::Point3fArray & texcoords0
		, castor::Point3fArray & texcoords1
		, castor::Point3fArray & texcoords2
		, castor::Point3fArray & texcoords3 )
	{
		positions.resize( aiMesh.mNumVertices );
		normals.resize( aiMesh.mNumVertices );
		uint32_t index{ 0u };

		for ( auto & pos : positions )
		{
			pos[0] = float( aiMesh.mVertices[index].x );
			pos[1] = float( aiMesh.mVertices[index].y );
			pos[2] = float( aiMesh.mVertices[index].z );
			++index;
		}

		if ( aiMesh.HasNormals() )
		{
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
			tangents.resize( aiMesh.mNumVertices );
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
			tangents.resize( aiMesh.mNumVertices );
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
			tangents.resize( aiMesh.mNumVertices );
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
			tangents.resize( aiMesh.mNumVertices );
			index = 0u;

			for ( auto & tex : texcoords3 )
			{
				tex[0] = float( aiMesh.mTextureCoords[3][index].x );
				tex[1] = float( aiMesh.mTextureCoords[3][index].y );
				tex[2] = float( aiMesh.mTextureCoords[3][index].z );
				++index;
			}
		}

		if ( aiMesh.HasTangentsAndBitangents() )
		{
			tangents.resize( aiMesh.mNumVertices );
			index = 0u;

			for ( auto & tan : tangents )
			{
				tan[0] = float( aiMesh.mTangents[index].x );
				tan[1] = float( aiMesh.mTangents[index].y );
				tan[2] = float( aiMesh.mTangents[index].z );
				++index;
			}
		}
	}

	static std::vector< castor3d::SubmeshAnimationBuffer > gatherMeshAnimBuffers( castor::Point3fArray const & positions
		, castor::Point3fArray const & normals
		, castor::Point3fArray const & tangents
		, castor::Point3fArray const & texcoords0
		, castor::Point3fArray const & texcoords1
		, castor::Point3fArray const & texcoords2
		, castor::Point3fArray const & texcoords3
		, castor::ArrayView< aiAnimMesh * > animMeshes )
	{
		std::vector< castor3d::SubmeshAnimationBuffer > result;

		for ( auto aiAnimMesh : animMeshes )
		{
			castor3d::SubmeshAnimationBuffer buffer;
			createVertexBuffer( *aiAnimMesh
				, buffer.positions
				, buffer.normals
				, buffer.tangents
				, buffer.texcoords0
				, buffer.texcoords1
				, buffer.texcoords2
				, buffer.texcoords3 );

			if ( aiAnimMesh->HasPositions() )
			{
				auto it = buffer.positions.begin();

				for ( auto & point : positions )
				{
					*it -= point;
					++it;
				}
			}

			if ( aiAnimMesh->HasNormals() )
			{
				auto it = buffer.normals.begin();

				for ( auto & point : normals )
				{
					*it -= point;
					++it;
				}
			}

			if ( aiAnimMesh->HasTextureCoords( 0u ) )
			{
				auto it = buffer.texcoords0.begin();

				for ( auto & point : texcoords0 )
				{
					*it -= point;
					++it;
				}
			}

			if ( aiAnimMesh->HasTextureCoords( 1u ) )
			{
				auto it = buffer.texcoords1.begin();

				for ( auto & point : texcoords1 )
				{
					*it -= point;
					++it;
				}
			}

			if ( aiAnimMesh->HasTextureCoords( 2u ) )
			{
				auto it = buffer.texcoords2.begin();

				for ( auto & point : texcoords2 )
				{
					*it -= point;
					++it;
				}
			}

			if ( aiAnimMesh->HasTextureCoords( 3u ) )
			{
				auto it = buffer.texcoords3.begin();

				for ( auto & point : texcoords3 )
				{
					*it -= point;
					++it;
				}
			}

			if ( aiAnimMesh->HasTangentsAndBitangents() )
			{
				auto it = buffer.tangents.begin();

				for ( auto & point : tangents )
				{
					*it -= point;
					++it;
				}
			}

			result.emplace_back( std::move( buffer ) );
		}

		return result;
	}

		static void applyMorphTarget( float weight
			, castor3d::SubmeshAnimationBuffer const & target
			, castor::Point3fArray & positions
			, castor::Point3fArray & normals
			, castor::Point3fArray & tangents
			, castor::Point3fArray & texcoords0
			, castor::Point3fArray & texcoords1
			, castor::Point3fArray & texcoords2
			, castor::Point3fArray & texcoords3 )
		{
			if ( weight != 0.0f )
			{
				if ( !positions.empty() )
				{
					auto posIt = positions.begin();
					auto bufferIt = target.positions.begin();

					while ( bufferIt != target.positions.end() )
					{
						auto & buf = *bufferIt;
						*posIt += buf * weight;
						++posIt;
						++bufferIt;
					}
				}

				if ( !normals.empty() )
				{
					auto nmlIt = normals.begin();
					auto bufferIt = target.normals.begin();

					while ( bufferIt != target.normals.end() )
					{
						auto & buf = *bufferIt;
						*nmlIt += buf * weight;
						++nmlIt;
						++bufferIt;
					}
				}

				if ( !tangents.empty() )
				{
					auto tanIt = tangents.begin();
					auto bufferIt = target.tangents.begin();

					while ( bufferIt != target.tangents.end() )
					{
						auto & buf = *bufferIt;
						*tanIt += buf * weight;
						++tanIt;
						++bufferIt;
					}
				}

				if ( !texcoords0.empty() )
				{
					auto texIt = texcoords0.begin();
					auto bufferIt = target.texcoords0.begin();

					while ( bufferIt != target.texcoords0.end() )
					{
						auto & buf = *bufferIt;
						*texIt += buf * weight;
						++texIt;
						++bufferIt;
					}
				}

				if ( !texcoords1.empty() )
				{
					auto texIt = texcoords1.begin();
					auto bufferIt = target.texcoords1.begin();

					while ( bufferIt != target.texcoords1.end() )
					{
						auto & buf = *bufferIt;
						*texIt += buf * weight;
						++texIt;
						++bufferIt;
					}
				}

				if ( !texcoords2.empty() )
				{
					auto texIt = texcoords2.begin();
					auto bufferIt = target.texcoords2.begin();

					while ( bufferIt != target.texcoords2.end() )
					{
						auto & buf = *bufferIt;
						*texIt += buf * weight;
						++texIt;
						++bufferIt;
					}
				}

				if ( !texcoords3.empty() )
				{
					auto texIt = texcoords3.begin();
					auto bufferIt = target.texcoords3.begin();

					while ( bufferIt != target.texcoords3.end() )
					{
						auto & buf = *bufferIt;
						*texIt += buf * weight;
						++texIt;
						++bufferIt;
					}
				}
			}
		}

	static std::pair< uint32_t, double > getNodeAnimFrameTicks( aiNodeAnim const & aiNodeAnim )
	{
		return { std::max( { aiNodeAnim.mNumPositionKeys
				, aiNodeAnim.mNumRotationKeys
				, aiNodeAnim.mNumScalingKeys } )
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

	static std::pair< uint32_t, double > getAnimationFrameTicks( aiAnimation const & aiAnimation )
	{
		uint32_t count = 0u;
		double ticks = 0.0;

		for ( auto nodeAnim : castor::makeArrayView( aiAnimation.mChannels, aiAnimation.mNumChannels ) )
		{
			count = std::max( { count
				, nodeAnim->mNumPositionKeys
				, nodeAnim->mNumRotationKeys
				, nodeAnim->mNumScalingKeys } );
			ticks = std::max( { ticks
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

		return { count, ticks };
	}

	template< typename KeyT >
	struct KeyDataTyperT;

	template<>
	struct KeyDataTyperT< aiVectorKey >
	{
		using Type = castor::Point3f;
	};

	template<>
	struct KeyDataTyperT< aiQuatKey >
	{
		using Type = castor::Quaternion;
	};

	template< typename KeyT >
	using KeyDataTypeT = typename KeyDataTyperT< KeyT >::Type;

	template< typename KeyT >
	static std::map< castor::Milliseconds, KeyDataTypeT< KeyT > > processKeys( castor::ArrayView< KeyT > const & keys
		, int64_t ticksPerSecond
		, std::set< castor::Milliseconds > & times )
	{
		std::map< castor::Milliseconds, KeyDataTypeT< KeyT > > result;

		for ( auto const & key : keys )
		{
			if ( key.mTime >= 0 )
			{
				auto time = fromAssimp( key.mTime, ticksPerSecond );
				times.insert( time );
				result.emplace( time, fromAssimp( key.mValue ) );
			}
		}

		return result;
	}

	template< typename KeyFrameT, typename AnimationT >
	static KeyFrameT & getKeyFrame( castor::Milliseconds const & time
		, AnimationT & animation
		, std::map< castor::Milliseconds, std::unique_ptr< KeyFrameT > > & keyframes )
	{
		auto it = keyframes.find( time );

		if ( it == keyframes.end() )
		{
			it = keyframes.emplace( time
				, std::make_unique< KeyFrameT >( animation, time ) ).first;
		}

		return *it->second;
	}

	template< typename T >
	static void findValue( castor::Milliseconds time
		, typename std::map< castor::Milliseconds, T > const & map
		, typename std::map< castor::Milliseconds, T >::const_iterator & prv
		, typename std::map< castor::Milliseconds, T >::const_iterator & cur )
	{
		if ( map.empty() )
		{
			prv = map.end();
			cur = map.end();
		}
		else
		{
			cur = std::find_if( map.begin()
				, map.end()
				, [&time]( std::pair< castor::Milliseconds, T > const & pair )
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
		}
	}

	template< typename T >
	static T interpolate( castor::Milliseconds const & time
		, castor3d::Interpolator< T > const & interpolator
		, std::map< castor::Milliseconds, T > const & values )
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
			findValue( time, values, prv, cur );

			if ( prv != cur )
			{
				auto dt = cur->first - prv->first;
				float factor = float( ( time - prv->first ).count() ) / float( dt.count() );
				result = interpolator.interpolate( prv->second, cur->second, factor );
			}
			else
			{
				result = prv->second;
			}
		}

		return result;
	}

	template< typename AnimationT, typename KeyFrameT, typename FuncT >
	static void synchroniseKeys( std::map< castor::Milliseconds, castor::Point3f > const & translates
		, std::map< castor::Milliseconds, castor::Point3f > const & scales
		, std::map< castor::Milliseconds, castor::Quaternion > const & rotates
		, std::set< castor::Milliseconds > const & times
		, uint32_t fps
		, castor::Milliseconds maxTime
		, AnimationT & animation
		, std::map< castor::Milliseconds, std::unique_ptr< KeyFrameT > > & keyframes
		, FuncT fillKeyFrame )
	{
		castor3d::InterpolatorT< castor::Point3f, castor3d::InterpolatorType::eLinear > pointInterpolator;
		castor3d::InterpolatorT< castor::Quaternion, castor3d::InterpolatorType::eLinear > quatInterpolator;
		// Limit the key frames per second to 60, to spare RAM...
		auto wantedFps = std::min< int64_t >( 60, int64_t( fps ) );
		castor::Milliseconds step{ 1000 / wantedFps };

		for ( auto time = 0_ms; time <= maxTime; time += step )
		{
			auto translate = interpolate( time, pointInterpolator, translates );
			auto scale = interpolate( time, pointInterpolator, scales );
			auto rotate = interpolate( time, quatInterpolator, rotates );
			fillKeyFrame( getKeyFrame( time, animation, keyframes )
				, translate
				, rotate
				, scale );
		}
	}

	template< typename aiAnimT
		, typename AnimationT
		, typename KeyFrameT
		, typename FuncT >
	static void processAnimationNodeKeys( aiAnimT const & aiAnim
		, uint32_t wantedFps
		, castor::Milliseconds maxTime
		, int64_t ticksPerSecond
		, AnimationT & animation
		, std::map< castor::Milliseconds, std::unique_ptr< KeyFrameT > > & keyframes
		, FuncT fillKeyFrame )
	{
		std::set< castor::Milliseconds > times;
		auto translates = processKeys( castor::makeArrayView( aiAnim.mPositionKeys
				, aiAnim.mNumPositionKeys )
			, ticksPerSecond
			, times );
		auto scales = processKeys( castor::makeArrayView( aiAnim.mScalingKeys
				, aiAnim.mNumScalingKeys )
			, ticksPerSecond
			, times );
		auto rotates = processKeys( castor::makeArrayView( aiAnim.mRotationKeys
				, aiAnim.mNumRotationKeys )
			, ticksPerSecond
			, times );
		synchroniseKeys( translates
			, scales
			, rotates
			, times
			, wantedFps
			, maxTime
			, animation
			, keyframes
			, fillKeyFrame );
	}
}

#endif
