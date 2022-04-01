#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/ReflectiveShadowMapping.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include <ShaderWriter/Source.hpp>

#define C3D_RSMGIUseCascasdes 1

namespace castor3d
{
	namespace rsmvpl
	{
		template< typename ImageT, typename CoordT >
		static sdw::Vec3 evaluateVPL( sdw::ShaderWriter & writer
			, ImageT const & rsmNormalMap
			, ImageT const & rsmPositionMap
			, ImageT const & rsmFluxMap
			, CoordT const & coords
			, sdw::Vec3 const & worldPosition
			, sdw::Vec3 const & worldNormal
			, sdw::Float const & xi1 )
		{
			auto vplWorldPosition = writer.declLocale( "vplWorldPosition"
				, rsmPositionMap.lod( coords, 0.0_f ).xyz() );
			auto vplWorldNormal = writer.declLocale( "vplWorldNormal"
				, rsmNormalMap.lod( coords, 0.0_f ).xyz() );
			auto vplFlux = writer.declLocale( "vplFlux"
				, rsmFluxMap.lod( coords, 0.0_f ).xyz() );

			auto diff = writer.declLocale( "diff"
				, worldPosition - vplWorldPosition );
			auto dot1 = writer.declLocale( "dot1"
				, max( 0.0_f, dot( vplWorldNormal, diff ) ) );
			auto dot2 = writer.declLocale( "dot2"
				, max( 0.0_f, dot( worldNormal, -diff ) ) );
			auto sqlength = writer.declLocale( "sqlength"
				, dot( diff, diff ) );

			auto result = writer.declLocale( "result"
				, vplFlux * ( dot1 * dot2 ) );
			result *= xi1 * xi1 / ( sqlength * sqlength );

			return result;
		}
	}

	ReflectiveShadowMapping::ReflectiveShadowMapping( sdw::ShaderWriter & writer
		, sdw::ArrayStorageBufferT< sdw::Vec4 > & rsmSamples )
		: m_writer{ writer }
		, m_rsmSamples{ rsmSamples }
	{
	}

	sdw::Vec3 ReflectiveShadowMapping::directional( shader::DirectionalShadowData const & pshadowData
		, sdw::Vec3 const & pviewPosition
		, sdw::Vec3 const & pworldPosition
		, sdw::Vec3 const & pworldNormal
		, shader::RsmConfigData const & prsmData )
	{
		if ( !m_directional )
		{
			m_directional = m_writer.implementFunction< sdw::Vec3 >( "reflectiveShadowMapping"
				, [&]( shader::DirectionalShadowData const & shadowData
					, sdw::Vec3 const & viewPosition
					, sdw::Vec3 const & worldPosition
					, sdw::Vec3 const & worldNormal
					, shader::RsmConfigData const & rsmData )
				{
					auto c3d_rsmNormalMap = m_writer.getVariable< sdw::CombinedImage2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormal ) );
					auto c3d_rsmPositionMap = m_writer.getVariable< sdw::CombinedImage2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition ) );
					auto c3d_rsmFluxMap = m_writer.getVariable< sdw::CombinedImage2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eFlux ) );
					auto maxCount = m_writer.declLocale( "maxCount"
						, m_writer.cast< sdw::Int >( max( shadowData.cascadeCount(), 1_u ) - 1_u ) );
					auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
						, vec3( m_writer.cast< sdw::Float >( maxCount ), 1.0_f, 0.0_f ) );
					auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
						, m_writer.cast< sdw::UInt >( cascadeFactors.x() ) );
					auto rMax = m_writer.declLocale( "rMax"
						, vec2( rsmData.maxRadius / shadowData.splitScales()[cascadeIndex / 4u][cascadeIndex % 4u] ) / vec2( c3d_rsmFluxMap.getSize( 0_i ).xy() ) );
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, shadowData.transforms()[cascadeIndex] * vec4( worldPosition, 1.0_f ) );
					lightSpacePosition.xyz() /= lightSpacePosition.w();
					lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
						, vec2( 0.5_f )
						, vec2( 0.5_f ) );
					auto indirectIllumination = m_writer.declLocale( "indirectIllumination"
						, vec3( 0.0_f ) );

					FOR( m_writer, sdw::UInt, i, 0_u, i < rsmData.sampleCount, ++i )
					{
						auto rnd = m_writer.declLocale( "rnd"
							, m_rsmSamples[i] );
						auto coords = m_writer.declLocale( "coords"
							, vec3( sdw::fma( rnd.xy(), rMax, lightSpacePosition.xy() )
								, m_writer.cast < sdw::Float > ( cascadeIndex ) ) );
						indirectIllumination += rsmvpl::evaluateVPL( m_writer
							, c3d_rsmNormalMap
							, c3d_rsmPositionMap
							, c3d_rsmFluxMap
							, coords
							, worldPosition
							, worldNormal
							, rnd.z() );
					}
					ROF

					m_writer.returnStmt( clamp( indirectIllumination * rsmData.intensity
						, vec3( 0.0_f )
						, vec3( 1.0_f ) ) );
				}
				, shader::InDirectionalShadowData{ m_writer, "shadowData" }
				, sdw::InVec3{ m_writer, "viewPosition" }
				, sdw::InVec3{ m_writer, "worldPosition" }
				, sdw::InVec3{ m_writer, "worldNormal" }
				, shader::InRsmConfigData{ m_writer, "rsmData" } );
		}

		return m_directional( pshadowData
			, pviewPosition
			, pworldPosition
			, pworldNormal
			, prsmData );
	}

	sdw::Vec3 ReflectiveShadowMapping::point( shader::PointShadowData const & pshadowData
		, sdw::Vec3 const & plightPosition
		, sdw::Vec3 const & pworldPosition
		, sdw::Vec3 const & pworldNormal
		, shader::RsmConfigData const & prsmData )
	{
		if ( !m_point )
		{
			m_point = m_writer.implementFunction< sdw::Vec3 >( "reflectiveShadowMapping"
				, [&]( shader::PointShadowData const & shadowData
					, sdw::Vec3 const & lightPosition
					, sdw::Vec3 const & worldPosition
					, sdw::Vec3 const & worldNormal
					, shader::RsmConfigData const & rsmData )
				{
					auto c3d_rsmNormalMap = m_writer.getVariable< sdw::CombinedImageCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eNormal ) );
					auto c3d_rsmPositionMap = m_writer.getVariable< sdw::CombinedImageCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::ePosition ) );
					auto c3d_rsmFluxMap = m_writer.getVariable< sdw::CombinedImageCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eFlux ) );
					auto indirectIllumination = m_writer.declLocale( "indirectIllumination"
						, vec3( 0.0_f ) );
					auto rMax = m_writer.declLocale( "rMax"
						, vec2( rsmData.maxRadius ) / vec2( c3d_rsmFluxMap.getSize( 0_i ).xy() ) );
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, worldPosition - lightPosition );

					FOR( m_writer, sdw::UInt, i, 0_u, i < rsmData.sampleCount, ++i )
					{
						auto rnd = m_writer.declLocale( "rnd"
							, m_rsmSamples[i] );
						auto coords = m_writer.declLocale( "coords"
							, vec4( sdw::fma( rnd.xy(), rMax, lightSpacePosition.xy() )
								, lightSpacePosition.z()
								, m_writer.cast< sdw::Float >( rsmData.index ) ) );
						indirectIllumination += rsmvpl::evaluateVPL( m_writer
							, c3d_rsmNormalMap
							, c3d_rsmPositionMap
							, c3d_rsmFluxMap
							, coords
							, worldPosition
							, worldNormal
							, rnd.z() );
					}
					ROF

					m_writer.returnStmt( clamp( indirectIllumination * rsmData.intensity
						, vec3( 0.0_f )
						, vec3( 1.0_f ) ) );
				}
				, shader::InPointShadowData{ m_writer, "shadowData" }
				, sdw::InVec3{ m_writer, "lightPosition" }
				, sdw::InVec3{ m_writer, "worldPosition" }
				, sdw::InVec3{ m_writer, "worldNormal" }
				, shader::InRsmConfigData{ m_writer, "rsmData" } );
		}

		return m_point( pshadowData
			, plightPosition
			, pworldPosition
			, pworldNormal
			, prsmData );
	}

	sdw::Vec3 ReflectiveShadowMapping::spot( shader::SpotShadowData const & pshadowData
		, sdw::Vec3 const & pworldPosition
		, sdw::Vec3 const & pworldNormal
		, shader::RsmConfigData const & prsmData )
	{
		if ( !m_spot )
		{
			m_spot = m_writer.implementFunction< sdw::Vec3 >( "reflectiveShadowMapping"
				, [&]( shader::SpotShadowData const & shadowData
					, sdw::Vec3 const & worldPosition
					, sdw::Vec3 const & worldNormal
					, shader::RsmConfigData const & rsmData )
				{
					auto c3d_rsmNormalMap = m_writer.getVariable< sdw::CombinedImage2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eNormal ) );
					auto c3d_rsmPositionMap = m_writer.getVariable< sdw::CombinedImage2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::ePosition ) );
					auto c3d_rsmFluxMap = m_writer.getVariable< sdw::CombinedImage2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eFlux ) );
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, shadowData.transform() * vec4( worldPosition, 1.0_f ) );
					lightSpacePosition.xyz() /= lightSpacePosition.w();
					lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
						, vec2( 0.5_f )
						, vec2( 0.5_f ) );
					auto indirectIllumination = m_writer.declLocale( "indirectIllumination"
						, vec3( 0.0_f ) );
					auto rMax = m_writer.declLocale( "rMax"
						, vec2( rsmData.maxRadius ) / vec2( c3d_rsmFluxMap.getSize( 0_i ).xy() ) );

					FOR( m_writer, sdw::UInt, i, 0_u, i < rsmData.sampleCount, ++i )
					{
						auto rnd = m_writer.declLocale( "rnd"
							, m_rsmSamples[i] );
						auto coords = m_writer.declLocale( "coords"
							, vec3( sdw::fma( rnd.xy(), rMax, lightSpacePosition.xy() )
								, m_writer.cast< sdw::Float >( rsmData.index ) ) );
						indirectIllumination += rsmvpl::evaluateVPL( m_writer
							, c3d_rsmNormalMap
							, c3d_rsmPositionMap
							, c3d_rsmFluxMap
							, coords
							, worldPosition
							, worldNormal
							, rnd.z() );
					}
					ROF

					m_writer.returnStmt( clamp( indirectIllumination * rsmData.intensity
						, vec3( 0.0_f )
						, vec3( 1.0_f ) ) );
				}
				, shader::InSpotShadowData{ m_writer, "shadowData" }
				, sdw::InVec3{ m_writer, "worldPosition" }
				, sdw::InVec3{ m_writer, "worldNormal" }
				, shader::InRsmConfigData{ m_writer, "rsmData" } );
		}

		return m_spot( pshadowData
			, pworldPosition
			, pworldNormal
			, prsmData );
	}
}
