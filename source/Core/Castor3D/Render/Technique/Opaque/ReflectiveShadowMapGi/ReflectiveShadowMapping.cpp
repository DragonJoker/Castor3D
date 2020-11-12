#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/ReflectiveShadowMapping.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;

#define C3D_RSMGIUseCascasdes 0

namespace castor3d
{
	ReflectiveShadowMapping::ReflectiveShadowMapping( sdw::ShaderWriter & writer
		, sdw::ArraySsboT< sdw::Vec2 > & rsmSamples
		, LightType lightType )
		: m_writer{ writer }
		, m_rsmSamples{ rsmSamples }
	{
		switch ( lightType )
		{
		case castor3d::LightType::eDirectional:
			doDeclareDirectional();
			break;
		case castor3d::LightType::ePoint:
			doDeclarePoint();
			break;
		case castor3d::LightType::eSpot:
			doDeclareSpot();
			break;
		default:
			CU_Failure( "Unsupported LightType" );
			break;
		}
	}

	sdw::Vec3 ReflectiveShadowMapping::directional( shader::DirectionalLight const & light
		, sdw::Vec3 const & viewPosition
		, sdw::Vec3 const & worldPosition
		, sdw::Vec3 const & worldNormal
		, shader::RsmConfigData const & rsmData )
	{
		return m_directional( light
			, viewPosition
			, worldPosition
			, worldNormal
			, rsmData );
	}

	sdw::Vec3 ReflectiveShadowMapping::point( shader::PointLight const & light
		, sdw::Vec3 const & worldPosition
		, sdw::Vec3 const & worldNormal
		, shader::RsmConfigData const & rsmData )
	{
		return m_point( light
			, worldPosition
			, worldNormal
			, rsmData );
	}

	sdw::Vec3 ReflectiveShadowMapping::spot( shader::SpotLight const & light
		, sdw::Vec3 const & worldPosition
		, sdw::Vec3 const & worldNormal
		, shader::RsmConfigData const & rsmData )
	{
		return m_spot( light
			, worldPosition
			, worldNormal
			, rsmData );
	}

	void ReflectiveShadowMapping::doDeclareDirectional()
	{
		using namespace sdw;
		m_directional = m_writer.implementFunction< Vec3 >( "reflectiveShadowMapping"
			, [&]( shader::DirectionalLight const & light
				, Vec3 const & viewPosition
				, Vec3 const & worldPosition
				, Vec3 const & worldNormal
				, shader::RsmConfigData const & rsmData )
			{
				auto c3d_rsmNormalMap = m_writer.getVariable< SampledImage2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormalLinear ) );
				auto c3d_rsmPositionMap = m_writer.getVariable< SampledImage2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition ) );
				auto c3d_rsmFluxMap = m_writer.getVariable< SampledImage2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eFlux ) );
				auto maxCount = m_writer.declLocale( "maxCount"
					, max( light.m_cascadeCount, 1_u ) - 1_u );
				auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
					, 0_u );

#if C3D_RSMGIUseCascasdes

				// Get cascade index for the current fragment's view position
				FOR( m_writer, UInt, i, 0u, i < maxCount, ++i )
				{
					IF( m_writer, viewPosition.z() < light.m_splitDepths[i] )
					{
						cascadeIndex = i + 1_u;
					}
					FI;
				}
				ROF;

				auto rMax = m_writer.declLocale< Float >( "rMax"
					, rsmData.rsmRMax / light.m_splitScales[cascadeIndex] );
#else
				auto rMax = m_writer.declLocale< Float >( "rMax"
					, rsmData.rsmRMax / light.m_splitScales[cascadeIndex / 4][cascadeIndex % 4] );
#endif
				auto lightSpacePosition = m_writer.declLocale< Vec4 >( "lightSpacePosition"
					, light.m_transforms[cascadeIndex] * vec4( worldPosition, 1.0 ) );
				lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
					, vec2( 0.5_f )
					, vec2( 0.5_f ) );
				auto indirectIllumination = m_writer.declLocale( "indirectIllumination"
					, vec3( 0.0_f ) );

				FOR( m_writer, UInt, i, 0_u, i < rsmData.rsmSampleCount, ++i )
				{
					auto rnd = m_writer.declLocale( "rnd"
						, m_rsmSamples[i].xy() );

					auto coords = m_writer.declLocale( "coords"
						, vec3( lightSpacePosition.xy() + rMax * rnd
							, m_writer.cast< Float >( cascadeIndex ) ) );

					auto vplPositionWS = m_writer.declLocale( "vplPositionWS"
						, c3d_rsmPositionMap.sample( coords ).xyz() );
					auto vplNormalWS = m_writer.declLocale( "vplNormalWS"
						, c3d_rsmNormalMap.sample( coords ).xyz() );
					auto vplFlux = m_writer.declLocale( "vplFlux"
						, c3d_rsmFluxMap.sample( coords ).xyz() );
					auto dot1 = m_writer.declLocale( "dot1"
						, max( 0.0_f, dot( vplNormalWS, worldPosition - vplPositionWS ) ) );
					auto dot2 = m_writer.declLocale( "dot2"
						, max( 0.0_f, dot( worldNormal, vplPositionWS - worldPosition ) ) );
					auto diff = m_writer.declLocale( "diff"
						, worldPosition - vplPositionWS );
					auto sqdist = m_writer.declLocale( "sqdist"
						, dot( diff, diff ) );

					auto result = m_writer.declLocale( "result"
						, vplFlux * ( dot1 * dot2 ) / ( sqdist * sqdist ) );

					result *= rnd.x() * rnd.x();
					indirectIllumination += result;
				}
				ROF;

				m_writer.returnStmt( clamp( indirectIllumination * rsmData.rsmIntensity
					, vec3( 0.0_f )
					, vec3( 1.0_f ) ) );
			}
			, shader::InDirectionalLight{ m_writer, "light" }
			, InVec3{ m_writer, "viewPosition" }
			, InVec3{ m_writer, "worldPosition" }
			, InVec3{ m_writer, "worldNormal" }
			, shader::InRsmConfigData{ m_writer, "rsmData" } );
	}

	void ReflectiveShadowMapping::doDeclarePoint()
	{
		using namespace sdw;
		m_point = m_writer.implementFunction< Vec3 >( "reflectiveShadowMapping"
			, [&]( shader::PointLight const & light
				, Vec3 const & worldPosition
				, Vec3 const & worldNormal
				, shader::RsmConfigData const & rsmData )
			{
				auto c3d_rsmNormalMap = m_writer.getVariable< SampledImageCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eNormalLinear ) );
				auto c3d_rsmPositionMap = m_writer.getVariable< SampledImageCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::ePosition ) );
				auto c3d_rsmFluxMap = m_writer.getVariable< SampledImageCubeArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eFlux ) );
				auto indirectIllumination = m_writer.declLocale( "indirectIllumination"
					, vec3( 0.0_f ) );
				auto rMax = m_writer.declLocale< Float >( "rMax"
					, rsmData.rsmRMax );

				FOR( m_writer, UInt, i, 0_u, i < rsmData.rsmSampleCount, ++i )
				{
					auto rnd = m_writer.declLocale( "rnd"
						, m_rsmSamples[i].xy() );
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, worldPosition - light.m_position );
					auto coords = m_writer.declLocale( "coords"
						, vec4( vec3( vertexToLight.xy() + rMax * rnd, vertexToLight.z() )
							, m_writer.cast< Float >( rsmData.rsmIndex ) ) );

					auto vplPositionWS = m_writer.declLocale( "vplPositionWS"
						, c3d_rsmPositionMap.sample( coords ).xyz() );
					auto vplNormalWS = m_writer.declLocale( "vplNormalWS"
						, c3d_rsmNormalMap.sample( coords ).xyz() );
					auto vplFlux = m_writer.declLocale( "vplFlux"
						, c3d_rsmFluxMap.sample( coords ).xyz() );
					auto diff = m_writer.declLocale( "diff"
						, worldPosition - vplPositionWS );
					auto dot1 = m_writer.declLocale( "dot1"
						, max( 0.0_f, dot( vplNormalWS, diff ) ) );
					auto dot2 = m_writer.declLocale( "dot2"
						, max( 0.0_f, dot( worldNormal, vplPositionWS - worldPosition ) ) );
					auto sqdist = m_writer.declLocale( "sqdist"
						, dot( diff, diff ) );

					auto result = m_writer.declLocale( "result"
						, vplFlux * ( dot1 * dot2 ) / ( sqdist * sqdist ) );

					result *= rnd.x() * rnd.x();
					indirectIllumination += result;
				}
				ROF;

				m_writer.returnStmt( clamp( indirectIllumination * rsmData.rsmIntensity
					, vec3( 0.0_f )
					, vec3( 1.0_f ) ) );
			}
			, shader::InPointLight{ m_writer, "light" }
			, InVec3{ m_writer, "worldPosition" }
			, InVec3{ m_writer, "worldNormal" }
			, shader::InRsmConfigData{ m_writer, "rsmData" } );
	}

	void ReflectiveShadowMapping::doDeclareSpot()
	{
		using namespace sdw;
		m_spot = m_writer.implementFunction< Vec3 >( "reflectiveShadowMapping"
			, [&]( shader::SpotLight const & light
				, Vec3 const & worldPosition
				, Vec3 const & worldNormal
				, shader::RsmConfigData const & rsmData )
			{
				auto c3d_rsmNormalMap = m_writer.getVariable< SampledImage2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eNormalLinear ) );
				auto c3d_rsmPositionMap = m_writer.getVariable< SampledImage2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::ePosition ) );
				auto c3d_rsmFluxMap = m_writer.getVariable< SampledImage2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eFlux ) );
				auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
					, light.m_transform * vec4( worldPosition, 1.0 ) );
				lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
					, vec2( 0.5_f )
					, vec2( 0.5_f ) );
				lightSpacePosition /= lightSpacePosition.w();
				lightSpacePosition.xy() += vec2( 0.5_f );
				auto indirectIllumination = m_writer.declLocale( "indirectIllumination"
					, vec3( 0.0_f ) );
				auto rMax = m_writer.declLocale( "rMax"
					, vec2( rsmData.rsmRMax ) );

				FOR( m_writer, UInt, i, 0_u, i < rsmData.rsmSampleCount, ++i )
				{
					auto rnd = m_writer.declLocale( "rnd"
						, m_rsmSamples[i].xy() );

					auto coords = m_writer.declLocale( "coords"
						, vec3( sdw::fma( rnd, rMax, lightSpacePosition.xy() ), rsmData.rsmIndex ) );

					auto vplPositionWS = m_writer.declLocale( "vplPositionWS"
						, c3d_rsmPositionMap.sample( coords ).xyz() );
					auto vplNormalWS = m_writer.declLocale( "vplNormalWS"
						, c3d_rsmNormalMap.sample( coords ).xyz() );
					auto vplFlux = m_writer.declLocale( "vplFlux"
						, c3d_rsmFluxMap.sample( coords ).xyz() );
					auto diff = m_writer.declLocale( "diff"
						, worldPosition - vplPositionWS );
					auto dot1 = m_writer.declLocale( "dot1"
						, max( 0.0_f, dot( vplNormalWS, diff ) ) );
					auto dot2 = m_writer.declLocale( "dot2"
						, max( 0.0_f, dot( worldNormal, vplPositionWS - worldPosition ) ) );
					auto sqdist = m_writer.declLocale( "sqdist"
						, dot( diff, diff ) );

					auto result = m_writer.declLocale( "result"
						, vplFlux * ( dot1 * dot2 ) / ( sqdist * sqdist ) );

					result *= rnd.x() * rnd.x();
					indirectIllumination += result;
				}
				ROF;

				m_writer.returnStmt( clamp( indirectIllumination * rsmData.rsmIntensity
					, vec3( 0.0_f )
					, vec3( 1.0_f ) ) );
			}
			, shader::InSpotLight{ m_writer, "light" }
			, InVec3{ m_writer, "worldPosition" }
			, InVec3{ m_writer, "worldNormal" }
			, shader::InRsmConfigData{ m_writer, "rsmData" } );
	}
}
