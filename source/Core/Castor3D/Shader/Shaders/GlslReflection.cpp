#include "Castor3D/Shader/Shaders/GlslReflection.hpp"

#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

using namespace castor;

namespace castor3d::shader
{
	ReflectionModel::ReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils
		, PassFlags const & passFlags )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_passFlags{ passFlags }
	{
	}

	ReflectionModel::ReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils )
		: m_writer{ writer }
		, m_utils{ utils }
	{
	}

	sdw::Float ReflectionModel::computeFresnel( LightMaterial & material
		, Surface const & surface
		, SceneData const & sceneData
		, sdw::Float const & refractionRatio )
	{
		declareComputeFresnel();
		return m_computeFresnel( surface
			, material.getRoughness()
			, sceneData.cameraPosition
			, refractionRatio );
	}

	sdw::Vec4 ReflectionModel::computeScreenSpace( Surface const & surface
		, SceneData const & sceneData
		, MatrixData const & matrixData
		, sdw::SampledImage2DR32 const & depthMap
		, sdw::SampledImage2DRgba32 const & normalsMap
		, sdw::SampledImage2DRgba32 const & colourMap
		, sdw::Vec2 const & texcoord
		, sdw::Vec4 const & ssrSettings )
	{
		declareComputeScreenSpace( matrixData );
		return m_computeScreenSpace( surface
			, sceneData.cameraPosition
			, depthMap
			, normalsMap
			, colourMap
			, texcoord
			, ssrSettings );
	}

	void ReflectionModel::declareComputeScreenSpace( MatrixData const & matrixData )
	{
		if ( m_computeScreenSpace )
		{
			return;
		}

		m_computeScreenSpace = m_writer.implementFunction< sdw::Vec4 >( "c3d_computeScreenSpace"
			, [&]( Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::SampledImage2DR32 const & depthMap
				, sdw::SampledImage2DRgba32 const & normalsMap
				, sdw::SampledImage2DRgba32 const & colourMap
				, sdw::Vec2 const & texcoord
				, sdw::Vec4 const & ssrSettings )
			{
				auto epsilon = m_writer.declConstant( "epsilon", 0.00001_f );

				auto viewDir = m_writer.declLocale( "viewDir"
					, normalize( surface.worldPosition - worldEye ) );
				auto reflectionVector = m_writer.declLocale< sdw::Vec3 >( "reflectionVector"
					, normalize( reflect( -viewDir, surface.worldNormal ) ) );
				auto sceneZ = m_writer.declLocale( "sceneZ"
					, 0.0_f );
				auto stepCount = m_writer.declLocale( "stepCount"
					, 0.0_f );
				auto forwardStepCount = m_writer.declLocale( "forwardStepCount"
					, ssrSettings.y() );
				auto rayMarchPosition = m_writer.declLocale( "rayMarchPosition"
					, surface.worldPosition );
				auto rayMarchTexPosition = m_writer.declLocale( "rayMarchTexPosition"
					, vec4( 0.0_f ) );

				WHILE( m_writer, stepCount < ssrSettings.y() )
				{
					rayMarchPosition += reflectionVector.xyz() * ssrSettings.x();
					rayMarchTexPosition = matrixData.worldToCurProj( vec4( -rayMarchPosition, 1.0_f ) );

					IF( m_writer, abs( rayMarchTexPosition.w() ) < epsilon )
					{
						rayMarchTexPosition.w() = epsilon;
					}
					FI;

					rayMarchTexPosition.xy() /= rayMarchTexPosition.w();
					rayMarchTexPosition.xy() = vec2( rayMarchTexPosition.x(), -rayMarchTexPosition.y() ) * 0.5 + 0.5;

					sceneZ = depthMap.lod( rayMarchTexPosition.xy(), 0.0_f );
					sceneZ = matrixData.curProjToWorld( m_utils, rayMarchTexPosition.xy(), sceneZ ).z();

					IF( m_writer, -sceneZ <= -rayMarchPosition.z() )
					{
						forwardStepCount = stepCount;
						stepCount = ssrSettings.y();
					}
					ELSE
					{
						stepCount += 1.0_f;
					}
					FI;
				}
				ELIHW;

				IF( m_writer, forwardStepCount < ssrSettings.y() )
				{
					stepCount = 0.0_f;

					WHILE( m_writer, stepCount < ssrSettings.z() )
					{
						rayMarchPosition -= reflectionVector.xyz() * ssrSettings.x() / ssrSettings.z();
						rayMarchTexPosition = matrixData.worldToCurProj( vec4( -rayMarchPosition, 1.0_f ) );

						IF( m_writer, abs( rayMarchTexPosition.w() ) < epsilon )
						{
							rayMarchTexPosition.w() = epsilon;
						}
						FI;

						rayMarchTexPosition.xy() /= rayMarchTexPosition.w();
						rayMarchTexPosition.xy() = vec2( rayMarchTexPosition.x(), -rayMarchTexPosition.y() ) * 0.5 + 0.5;

						sceneZ = depthMap.lod( rayMarchTexPosition.xy(), 0.0_f );
						sceneZ = matrixData.curProjToWorld( m_utils, rayMarchTexPosition.xy(), sceneZ ).z();

						IF( m_writer, -sceneZ > -rayMarchPosition.z() )
						{
							stepCount = ssrSettings.z();
						}
						ELSE
						{
							stepCount += 1.0_f;
						}
						FI;
					}
					ELIHW;
				}
				FI;

				auto nDotV = m_writer.declLocale( "nDotV"
					, abs( dot( surface.worldNormal, viewDir ) ) + epsilon );
				auto ssrReflectionNormal = m_writer.declLocale( "ssrReflectionNormal"
					, normalsMap.sample( rayMarchTexPosition.xy() ).xyz() );
				auto ssrDistanceFactor = m_writer.declLocale( "ssrDistanceFactor"
					, vec2( distance( 0.5_f, texcoord.x() ), distance( 0.5_f, texcoord.y() ) ) * 2 );
				auto ssrFactor = m_writer.declLocale( "ssrFactor"
					, ( 1.0_f - abs( nDotV ) )
					* ( 1.0 - forwardStepCount / ssrSettings.y() )
					* clamp( 1.0 - ssrDistanceFactor.x() - ssrDistanceFactor.y(), 0.0_f, 1.0_f )
					* ( 1.0 / ( 1.0 + abs( sceneZ - rayMarchPosition.z() ) * ssrSettings.w() ) )
					* ( 1.0 - clamp( dot( ssrReflectionNormal, surface.worldNormal ), 0.0_f, 1.0_f ) ) );

				auto reflectionColor = m_writer.declLocale( "reflectionColor"
					, colourMap.sample( rayMarchTexPosition.xy() ).rgb() );

				return vec4( reflectionColor, ssrFactor );
			}
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3{ m_writer, "worldEye" }
			, sdw::InSampledImage2DR32{ m_writer, "depthMap" }
			, sdw::InSampledImage2DRgba32{ m_writer, "normalsMap" }
			, sdw::InSampledImage2DRgba32{ m_writer, "colourMap" }
			, sdw::InVec2{ m_writer, "texcoord" }
			, sdw::InVec4{ m_writer, "ssrSetting" } );
	}

	void ReflectionModel::declareComputeFresnel()
	{
		if ( m_computeFresnel )
		{
			return;
		}

		m_computeFresnel = m_writer.implementFunction< sdw::Float >( "c3d_computeFresnel"
			, [&]( Surface const & surface
				, sdw::Float const & roughness
				, sdw::Vec3 const & worldEye
				, sdw::Float const & refractionRatio )
			{
				auto incident = m_writer.declLocale( "incident"
					, normalize( surface.worldPosition - worldEye ) );
				auto subRatio = m_writer.declLocale( "subRatio"
					, 1.0_f - refractionRatio );
				auto addRatio = m_writer.declLocale( "addRatio"
					, 1.0_f + refractionRatio );
				auto reflectance = m_writer.declLocale( "reflectance"
					, ( subRatio * subRatio ) / ( addRatio * addRatio ) );
				auto product = m_writer.declLocale( "product"
					, max( 0.0_f, dot( incident, surface.worldNormal ) ) );
				m_writer.returnStmt( sdw::fma( max( 1.0_f - roughness, reflectance ) - reflectance
					, pow( 1.0_f - product, 5.0_f )
					, reflectance ) );
			}
			, InSurface{ m_writer, "surface" }
			, sdw::InFloat{ m_writer, "roughness" }
			, sdw::InVec3{ m_writer, "worldEye" }
			, sdw::InFloat{ m_writer, "refractionRatio" } );
	}
}
