/*
See LICENSE file in root folder
*/
#ifndef ___C3DAssimp_Helpers___
#define ___C3DAssimp_Helpers___

#include <Castor3D/Animation/Interpolator.hpp>

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/scene.h> // Output data structure
#pragma warning( pop )

namespace c3d_assimp
{
	static castor::String normalizeName( castor::String name )
	{
		castor::string::replace( name, "\\", "-" );
		return castor::string::replace( name, "/", "-" );
	}

	static castor::Matrix4x4f makeMatrix4x4f( aiMatrix4x4 const & aiMatrix )
	{
		std::array< float, 16u > data
			{ aiMatrix.a1, aiMatrix.b1, aiMatrix.c1, aiMatrix.d1
			, aiMatrix.a2, aiMatrix.b2, aiMatrix.c2, aiMatrix.d2
			, aiMatrix.a3, aiMatrix.b3, aiMatrix.c3, aiMatrix.d3
			, aiMatrix.a4, aiMatrix.b4, aiMatrix.c4, aiMatrix.d4 };
		return castor::Matrix4x4f{ data.data() };
	}

	static castor::Matrix4x4f makeMatrix4x4f( aiMatrix3x3 const & aiMatrix )
	{
		std::array< float, 9u > data
			{ aiMatrix.a1, aiMatrix.b1, aiMatrix.c1
			, aiMatrix.a2, aiMatrix.b2, aiMatrix.c2
			, aiMatrix.a3, aiMatrix.b3, aiMatrix.c3 };
		return castor::Matrix4x4f{ castor::Matrix3x3f{ data.data() } };
	}

	static castor::Matrix4x4f makeMatrix4x4f( castor::Point3f const & direction
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

	static castor::Milliseconds convert( double ticks
		, int64_t ticksPerSecond )
	{
		// Turn ticks to seconds.
		auto time = ticks / double( ticksPerSecond );
		// Turn seconds to milliseconds.
		return castor::Milliseconds{ int64_t( time * 1000.0 ) };
	}

	static castor::Point3f convert( aiVector3D const & v )
	{
		return castor::Point3f{ v.x, v.y, v.z };
	}

	static castor::Quaternion convert( aiQuaternion const & v )
	{
		return castor::Quaternion::fromMatrix( makeMatrix4x4f( v.GetMatrix() ) );
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
				auto time = convert( key.mTime, ticksPerSecond );
				times.insert( time );
				result.emplace( time, convert( key.mValue ) );
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
		, int64_t ticksPerSecond
		, AnimationT & animation
		, std::map< castor::Milliseconds, std::unique_ptr< KeyFrameT > > & keyframes
		, FuncT fillKeyFrame )
	{
		castor3d::InterpolatorT< castor::Point3f, castor3d::InterpolatorType::eLinear > pointInterpolator;
		castor3d::InterpolatorT< castor::Quaternion, castor3d::InterpolatorType::eLinear > quatInterpolator;
		// Limit the key frames per second to 60, to spare RAM...
		auto wantedFps = std::min< int64_t >( 60, int64_t( fps ) );
		castor::Milliseconds step{ 1000 / wantedFps };
		castor::Milliseconds maxTime{ *times.rbegin() };

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
			, ticksPerSecond
			, animation
			, keyframes
			, fillKeyFrame );
	}
}

#endif
