#include "Castor3D/Shader/Shaders/GlslReflection.hpp"

#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslDerivativeValue.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <ShaderWriter/Writer.hpp>

namespace castor3d::shader
{
	ReflectionModel::ReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils
		, uint32_t & envMapBinding
		, uint32_t envMapSet
		, bool allowReflections
		, bool allowRefraction
		, bool allowIbl
		, bool hasEnvMap )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_allowReflections{ allowReflections }
		, m_allowRefraction{ allowRefraction }
		, m_allowIbl{ allowIbl }
		, m_hasEnvMap{ hasEnvMap }
	{
		m_writer.declCombinedImg< FImgCubeArrayRgba32 >( "c3d_mapEnvironment", envMapBinding, envMapSet, m_hasEnvMap );
		++envMapBinding;
	}

	sdw::RetVec3 ReflectionModel::computeIncident( sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & wsCamera )
	{
		return normalize( wsPosition - wsCamera );
	}

	DerivVec3 ReflectionModel::computeIncident( DerivVec3 const & wsPosition
		, sdw::Vec3 const & wsCamera )
	{
		return normalize( wsPosition - wsCamera );
	}

	sdw::Vec3 ReflectionModel::getVolumeTransmissionRay( sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & wsIncident
		, sdw::Float const & thickness
		, sdw::Float const & ior )
	{
		return normalize( refract( wsIncident, normalize( wsNormal ), 1.0_f / ior ) ) * thickness;
	}

	sdw::Vec3 ReflectionModel::applyVolumeAttenuation( sdw::Float const & transmissionDistance
		, sdw::Vec3 const & attenuationColor
		, sdw::Float const & attenuationDistance )
	{
		return transmissionDistance.getWriter()->ternary( attenuationDistance == 0.0_f
			// Attenuation distance is +∞ (which we indicate by zero), i.e. the transmitted color is not attenuated at all.
			, vec3( 1.0_f )
			// Compute light attenuation using Beer's law.
			, pow( attenuationColor, vec3( transmissionDistance / attenuationDistance ) ) );
	}

	sdw::Float ReflectionModel::applyIorToRoughness( sdw::Float const & roughness, sdw::Float const & ior )
	{
		return roughness * clamp( ior * 2.0_f, 0.0_f, 2.0_f );
	}

	void ReflectionModel::computeWithTransmission( BlendComponents & components
		, LightSurface const & lightSurface
		, BackgroundModel & background
		, sdw::CombinedImage2DRgba32 const & mippedScene
		, CameraData const & camera
		, sdw::Vec2 const & sceneUv
		, sdw::UInt const & envMapIndex
		, ReflectionRefraction & output
		, DebugOutputCategory const & debugOutput )
	{
		computeWithTransmission( components
			, lightSurface.N().value()
			, lightSurface.worldPosition().value().xyz()
			, lightSurface.V().value()
			, lightSurface.NdotV().value()
			, background
			, mippedScene
			, camera
			, sceneUv
			, envMapIndex
			, output
			, debugOutput );
	}

	void ReflectionModel::computeWithTransmission( BlendComponents & pcomponents
		, sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pV
		, sdw::Float const & pNdotV
		, BackgroundModel & background
		, sdw::CombinedImage2DRgba32 const & pmippedScene
		, CameraData const & camera
		, sdw::Vec2 const & psceneUv
		, sdw::UInt const & penvMapIndex
		, ReflectionRefraction & poutput
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeWithTransmission )
		{
			m_computeWithTransmission = m_writer.implementFunction< sdw::Void >( "c3d_backgroundBrdfWithTransmission"
				, [&]( BlendComponents components
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition
					, sdw::Vec3 const & V
					, sdw::Float const & NdotV
					, sdw::CombinedImage2DRgba32 const & mippedScene
					, sdw::Vec2 const & sceneUv
					, sdw::UInt envMapIndex
					, ReflectionRefraction output )
				{
					auto envMap = m_writer.getVariable< sdw::CombinedImageCubeArrayRgba32 >( "c3d_mapEnvironment" );
					auto brdf = m_writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapBrdf" );
					auto hasEnvMap = m_writer.declLocale( "hasEnvMap"
						, envMapIndex > 0_u );
					--envMapIndex;
					doComputeReflection( envMap, hasEnvMap, background
						, wsNormal, wsPosition, V
						, envMapIndex, components
						, output.diffuseReflection, output.specularReflection, debugOutput );
					debugOutput.registerOutput( "Specular Reflection", output.specularReflection );
					debugOutput.registerOutput( "Diffuse Reflection", output.diffuseReflection );
					computeDiffuseBrdf( components, background
						, output.diffuseReflection, wsNormal, hasEnvMap, envMapIndex
						, output, debugOutput );

					if( components.hasMember( "transmissionFactor" ) )
					{
						output.specularTransmission = doComputeSpecularTransmission( -V, wsPosition, wsNormal
							, mippedScene, camera
							, sceneUv, components, debugOutput );
						debugOutput.registerOutput( "Specular Transmission", output.specularTransmission );
					}
					else
					{
						debugOutput.registerOutput( "Specular Transmission", 0.0_f );
					}

					doComputeClearcoat( envMap, hasEnvMap, background
						, wsPosition, V, envMapIndex
						, components, output.coating, debugOutput );
					doComputeSheen( brdf, envMap, hasEnvMap, background
						, wsNormal, wsPosition, V, NdotV
						, envMapIndex, components
						, output.sheen, debugOutput );
				}
				, InOutBlendComponents{ m_writer, "components", pcomponents }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InFloat{ m_writer, "NdotV" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "mippedScene" }
				, sdw::InVec2{ m_writer, "sceneUv" }
				, sdw::InUInt{ m_writer, "envMapIndex" }
				, OutReflectionRefraction{ m_writer, "output" } );
		}

		m_computeWithTransmission( pcomponents
			, pwsNormal
			, pwsPosition
			, pV
			, pNdotV
			, pmippedScene
			, psceneUv
			, penvMapIndex
			, poutput );
		poutput.registerDebug( debugOutput );
	}

	void ReflectionModel::computeWithoutTransmission( BlendComponents & components
		, LightSurface const & lightSurface
		, BackgroundModel & background
		, sdw::UInt const & envMapIndex
		, sdw::Vec3 & reflectedDiffuse
		, sdw::Vec3 & reflectedSpecular
		, DebugOutputCategory const & debugOutput )
	{
		computeWithoutTransmission( components
			, lightSurface.N().value()
			, lightSurface.worldPosition().value().xyz()
			, lightSurface.V().value()
			, background, envMapIndex
			, reflectedDiffuse, reflectedSpecular, debugOutput );
	}

	sdw::Boolean ReflectionModel::computeWithoutTransmission( BlendComponents & components
		, sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & V
		, BackgroundModel & background
		, sdw::UInt envMapIndex
		, sdw::Vec3 & reflectedDiffuse
		, sdw::Vec3 & reflectedSpecular
		, DebugOutputCategory const & debugOutput )
	{
		auto envMap = m_writer.getVariable< sdw::CombinedImageCubeArrayRgba32 >( "c3d_mapEnvironment" );
		auto hasEnvMap = m_writer.declLocale( "hasEnvMap"
			, envMapIndex > 0_u );
		--envMapIndex;
		doComputeReflection( envMap, hasEnvMap, background
			, wsNormal, wsPosition, V
			, envMapIndex, components
			, reflectedDiffuse, reflectedSpecular, debugOutput );
		reflectedDiffuse *= components.baseColour;
		return hasEnvMap;
	}

	void ReflectionModel::computeWithoutTransmission( BlendComponents & pcomponents
		, sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pV
		, sdw::Float const & pNdotV
		, BackgroundModel & background
		, sdw::UInt const & penvMapIndex
		, ReflectionRefraction & poutput
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeWithoutTransmission )
		{
			m_computeWithoutTransmission = m_writer.implementFunction< sdw::Void >( "c3d_backgroundBrdfWithoutTransmission"
				, [&]( BlendComponents components
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition
					, sdw::Vec3 const & V
					, sdw::Float const & NdotV
					, sdw::UInt envMapIndex
					, ReflectionRefraction output )
				{
					auto brdf = m_writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapBrdf" );
					auto envMap = m_writer.getVariable< sdw::CombinedImageCubeArrayRgba32 >( "c3d_mapEnvironment" );
					auto hasEnvMap = m_writer.declLocale( "hasEnvMap"
						, envMapIndex > 0_u );
					--envMapIndex;
					doComputeReflection( envMap, hasEnvMap, background
						, wsNormal, wsPosition, V
						, envMapIndex, components
						, output.diffuseReflection, output.specularReflection, debugOutput );
					debugOutput.registerOutput( "Specular Reflection", output.specularReflection );
					debugOutput.registerOutput( "Diffuse Reflection", output.diffuseReflection );
					computeDiffuseBrdf( components, background
						, output.diffuseReflection, wsNormal, hasEnvMap, envMapIndex
						, output, debugOutput );
					debugOutput.registerOutput( "Specular Transmission", 0.0_f );
					doComputeClearcoat( envMap, hasEnvMap, background
						, wsPosition, V, envMapIndex
						, components, output.coating, debugOutput );
					doComputeSheen( brdf, envMap, hasEnvMap, background
						, wsNormal, wsPosition, V, NdotV
						, envMapIndex, components
						, output.sheen, debugOutput );
				}
				, InOutBlendComponents{ m_writer, "components", pcomponents }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InFloat{ m_writer, "NdotV" }
				, sdw::InUInt{ m_writer, "envMapIndex" }
				, OutReflectionRefraction{ m_writer, "output" } );
		}

		m_computeWithoutTransmission( pcomponents
			, pwsNormal
			, pwsPosition
			, pV
			, pNdotV
			, penvMapIndex
			, poutput );
		poutput.registerDebug( debugOutput );
	}

	void ReflectionModel::computeDiffuseBrdf( BlendComponents & components
		, BackgroundModel & background
		, sdw::Vec3 const & reflectedDiffuse
		, sdw::Vec3 const & wsNormal
		, sdw::Boolean const & hasEnvMap
		, sdw::UInt const & envMapIndex
		, shader::ReflectionRefraction & output
		, DebugOutputCategory const & debugOutput )
	{
		auto envMap = m_writer.getVariable< sdw::CombinedImageCubeArrayRgba32 >( "c3d_mapEnvironment" );
		auto brdf = m_writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapBrdf" );
		output.diffuseReflection = reflectedDiffuse * components.baseColour;

		if ( components.hasMember( "diffuseTransmissionFactor" ) )
		{
			doComputeDiffuse( envMap
				, hasEnvMap
				, background
				, -wsNormal
				, envMapIndex
				, components
				, output.diffuseTransmission
				, debugOutput );
			output.diffuseTransmission *= components.diffuseTransmissionColour;
			debugOutput.registerOutput( "Diffuse Transmission", output.diffuseTransmission );

			if ( components.hasMember( "thicknessFactor" ) )
			{
				output.diffuseTransmission *= applyVolumeAttenuation( components.getMember< sdw::Float >( "diffuseTransmissionThickness" )
					, components.attenuationColour
					, components.attenuationDistance );
				debugOutput.registerOutput( "Volume Diffuse Transmission", output.diffuseTransmission );
			}
			else
			{
				debugOutput.registerOutput( "Volume Diffuse Transmission", output.diffuseTransmission );
			}
		}
		else
		{
			debugOutput.registerOutput( "Diffuse Transmission", 0.0_f );
			debugOutput.registerOutput( "Volume Diffuse Transmission", 0.0_f );
		}
	}

	sdw::Vec4 ReflectionModel::computeScreenSpace( CameraData const & cameraData
		, sdw::Vec3 const & pviewPosition
		, sdw::Vec3 const & pworldNormal
		, sdw::Vec2 const & ptexcoord
		, sdw::Vec4 const & pssrSettings
		, sdw::CombinedImage2DR32 const & pdepthMap
		, sdw::CombinedImage2DRgba32 const & pnormalMap
		, sdw::CombinedImage2DRgba32 const & pcolourMap
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeScreenSpace )
		{
			m_computeScreenSpace = m_writer.implementFunction< sdw::Vec4 >( "c3d_computeScreenSpace"
				, [this, &cameraData]( sdw::Vec3 const & viewPosition
					, sdw::Vec3 const & worldNormal
					, sdw::Vec2 const & texcoord
					, sdw::Vec4 const & ssrSettings
					, sdw::CombinedImage2DR32 const & depthMap
					, sdw::CombinedImage2DRgba32 const & normalMap
					, sdw::CombinedImage2DRgba32 const & colourMap )
				{
					auto epsilon = m_writer.declConstant( "epsilon", 0.00001_f );
					auto ssrStepSize = ssrSettings.x();
					auto ssrForwardMaxStepCount = ssrSettings.y();
					auto ssrBackwardMaxStepCount = ssrSettings.z();
					auto ssrDepthMult = ssrSettings.w();

					auto viewDir = m_writer.declLocale( "viewDir"
						, normalize( viewPosition ) );
					auto reflectionVector = m_writer.declLocale( "reflectionVector"
						, normalize( reflect( -viewDir, worldNormal ) ) );
					auto sceneZ = m_writer.declLocale( "sceneZ"
						, 0.0_f );
					auto stepCount = m_writer.declLocale( "stepCount"
						, 0.0_f );
					auto forwardStepCount = m_writer.declLocale( "forwardStepCount"
						, ssrForwardMaxStepCount );
					auto rayMarchPosition = m_writer.declLocale( "rayMarchPosition"
						, viewPosition );
					auto rayMarchTexPosition = m_writer.declLocale( "rayMarchTexPosition"
						, vec2( 0.0_f ) );

					WHILE( m_writer, stepCount < ssrForwardMaxStepCount )
					{
						rayMarchPosition += reflectionVector.xyz() * ssrStepSize;
						rayMarchTexPosition = cameraData.viewToScreenUV( m_utils, vec4( -rayMarchPosition, 1.0_f ) );

						sceneZ = depthMap.lod( rayMarchTexPosition, 0.0_f );
						sceneZ = cameraData.projToView( m_utils, rayMarchTexPosition, sceneZ ).z();

						IF( m_writer, -sceneZ <= -rayMarchPosition.z() )
						{
							forwardStepCount = stepCount;
							stepCount = ssrForwardMaxStepCount;
						}
						ELSE
						{
							stepCount += 1.0_f;
						}
						FI
					}
					ELIHW

					IF( m_writer, forwardStepCount < ssrForwardMaxStepCount )
					{
						stepCount = 0.0_f;

						WHILE( m_writer, stepCount < ssrBackwardMaxStepCount )
						{
							rayMarchPosition -= reflectionVector.xyz() * ssrStepSize / ssrBackwardMaxStepCount;
							rayMarchTexPosition = cameraData.viewToScreenUV( m_utils, vec4( -rayMarchPosition, 1.0_f ) );

							sceneZ = depthMap.lod( rayMarchTexPosition, 0.0_f );
							sceneZ = cameraData.projToView( m_utils, rayMarchTexPosition, sceneZ ).z();

							IF( m_writer, -sceneZ > -rayMarchPosition.z() )
							{
								stepCount = ssrBackwardMaxStepCount;
							}
							ELSE
							{
								stepCount += 1.0_f;
							}
							FI
						}
						ELIHW
					}
					FI

					auto nDotV = m_writer.declLocale( "nDotV"
						, abs( dot( worldNormal, viewDir ) ) + epsilon );
					auto ssrReflectionNormal = m_writer.declLocale( "ssrReflectionNormal"
						, normalMap.sample( rayMarchTexPosition ).xyz() );
					auto ssrDistanceFactor = m_writer.declLocale( "ssrDistanceFactor"
						, vec2( distance( 0.5_f, texcoord.x() ), distance( 0.5_f, texcoord.y() ) ) * 2.0f );
					auto ssrFactor = m_writer.declLocale( "ssrFactor"
						, ( 1.0_f - abs( nDotV ) )
							* ( 1.0f - forwardStepCount / ssrForwardMaxStepCount )
							* clamp( 1.0f - ssrDistanceFactor.x() - ssrDistanceFactor.y(), 0.0_f, 1.0_f )
							* ( 1.0f / ( 1.0f + abs( sceneZ - rayMarchPosition.z() ) * ssrDepthMult ) )
							* ( 1.0f - clamp( dot( ssrReflectionNormal, worldNormal ), 0.0_f, 1.0_f ) ) );

					auto reflectionColor = m_writer.declLocale( "reflectionColor"
						, colourMap.sample( rayMarchTexPosition ).rgb() );

					m_writer.returnStmt( vec4( reflectionColor, ssrFactor ) );
				}
				, sdw::InVec3{ m_writer, "viewPosition" }
				, sdw::InVec3{ m_writer, "worldNormal" }
				, sdw::InVec2{ m_writer, "texcoord" }
				, sdw::InVec4{ m_writer, "ssrSettings" }
				, sdw::InCombinedImage2DR32{ m_writer, "depthMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "normalMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "colourMap" } );
		}

		auto result = m_writer.declLocale( "c3d_ssrResult"
			, m_computeScreenSpace( pviewPosition
				, pworldNormal
				, ptexcoord
				, pssrSettings
				, pdepthMap
				, pnormalMap
				, pcolourMap ) );
		debugOutput.registerOutput( cuT( "SSRResult" ), result.xyz() );
		debugOutput.registerOutput( cuT( "SSRFactor" ), result.www() );
		return result;
	}

	sdw::RetVec4 ReflectionModel::computeScreenSpace( CameraData const & cameraData
		, sdw::Vec3 const & pviewPosition
		, sdw::Vec3 const & pworldNormal
		, sdw::Vec2 const & ptexcoord
		, sdw::Vec4 const & pssrSettings
		, sdw::CombinedImage2DRgba32 const & pdepthObjMap
		, sdw::CombinedImage2DRgba32 const & pnormalMap
		, sdw::CombinedImage2DRgba32 const & pcolourMap
		, DebugOutputCategory const & )
	{
		if ( !m_computeScreenSpace2 )
		{
			m_computeScreenSpace2 = m_writer.implementFunction< sdw::Vec4 >( "c3d_computeScreenSpace2"
				, [&]( sdw::Vec3 const & viewPosition
					, sdw::Vec3 const & worldNormal
					, sdw::Vec2 const & texcoord
					, sdw::Vec4 const & ssrSettings
					, sdw::CombinedImage2DRgba32 const & depthObjMap
					, sdw::CombinedImage2DRgba32 const & normalMap
					, sdw::CombinedImage2DRgba32 const & colourMap )
				{
					auto epsilon = m_writer.declConstant( "epsilon", 0.00001_f );
					auto ssrStepSize = ssrSettings.x();
					auto ssrForwardMaxStepCount = ssrSettings.y();
					auto ssrBackwardMaxStepCount = ssrSettings.z();
					auto ssrDepthMult = ssrSettings.w();

					auto viewDir = m_writer.declLocale( "viewDir"
						, normalize( viewPosition ) );
					auto reflectionVector = m_writer.declLocale( "reflectionVector"
						, normalize( reflect( -viewDir, worldNormal ) ) );
					auto sceneZ = m_writer.declLocale( "sceneZ"
						, 0.0_f );
					auto stepCount = m_writer.declLocale( "stepCount"
						, 0.0_f );
					auto forwardStepCount = m_writer.declLocale( "forwardStepCount"
						, ssrForwardMaxStepCount );
					auto rayMarchPosition = m_writer.declLocale( "rayMarchPosition"
						, viewPosition );
					auto rayMarchTexPosition = m_writer.declLocale( "rayMarchTexPosition"
						, vec2( 0.0_f ) );

					WHILE( m_writer, stepCount < ssrForwardMaxStepCount )
					{
						rayMarchPosition += reflectionVector.xyz() * ssrStepSize;
						rayMarchTexPosition = cameraData.viewToScreenUV( m_utils, vec4( -rayMarchPosition, 1.0_f ) );

						sceneZ = depthObjMap.lod( rayMarchTexPosition, 0.0_f ).r();
						sceneZ = cameraData.projToView( m_utils, rayMarchTexPosition, sceneZ ).z();

						IF( m_writer, -sceneZ <= -rayMarchPosition.z() )
						{
							forwardStepCount = stepCount;
							stepCount = ssrForwardMaxStepCount;
						}
						ELSE
						{
							stepCount += 1.0_f;
						}
						FI
					}
					ELIHW

					IF( m_writer, forwardStepCount < ssrForwardMaxStepCount )
					{
						stepCount = 0.0_f;

						WHILE( m_writer, stepCount < ssrBackwardMaxStepCount )
						{
							rayMarchPosition -= reflectionVector.xyz() * ssrStepSize / ssrBackwardMaxStepCount;
							rayMarchTexPosition = cameraData.viewToScreenUV( m_utils, vec4( -rayMarchPosition, 1.0_f ) );

							sceneZ = depthObjMap.lod( rayMarchTexPosition, 0.0_f ).r();
							sceneZ = cameraData.projToView( m_utils, rayMarchTexPosition, sceneZ ).z();

							IF( m_writer, -sceneZ > -rayMarchPosition.z() )
							{
								stepCount = ssrBackwardMaxStepCount;
							}
							ELSE
							{
								stepCount += 1.0_f;
							}
							FI
						}
						ELIHW
					}
					FI

					auto nDotV = m_writer.declLocale( "nDotV"
						, abs( dot( worldNormal, viewDir ) ) + epsilon );
					auto ssrReflectionNormal = m_writer.declLocale( "ssrReflectionNormal"
						, normalMap.lod( rayMarchTexPosition, 0.0_f ).xyz() );
					auto ssrDistanceFactor = m_writer.declLocale( "ssrDistanceFactor"
						, vec2( distance( 0.5_f, texcoord.x() ), distance( 0.5_f, texcoord.y() ) ) * 2.0f );
					auto ssrFactor = m_writer.declLocale( "ssrFactor"
						, ( 1.0_f - abs( nDotV ) )
							* ( 1.0f - forwardStepCount / ssrForwardMaxStepCount )
							* clamp( 1.0f - ssrDistanceFactor.x() - ssrDistanceFactor.y(), 0.0_f, 1.0_f )
							* ( 1.0f / ( 1.0f + abs( sceneZ - rayMarchPosition.z() ) * ssrDepthMult ) )
							* ( 1.0f - clamp( dot( ssrReflectionNormal, worldNormal ), 0.0_f, 1.0_f ) ) );

					auto reflectionColor = m_writer.declLocale( "reflectionColor"
						, colourMap.lod( rayMarchTexPosition, 0.0_f ).rgb() );

					m_writer.returnStmt( vec4( reflectionColor, ssrFactor ) );
				}
				, sdw::InVec3{ m_writer, "viewPosition" }
				, sdw::InVec3{ m_writer, "worldNormal" }
				, sdw::InVec2{ m_writer, "texcoord" }
				, sdw::InVec4{ m_writer, "ssrSettings" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "depthObjMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "normalMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "colourMap" } );
		}

		return m_computeScreenSpace2( pviewPosition
			, pworldNormal
			, ptexcoord
			, pssrSettings
			, pdepthObjMap
			, pnormalMap
			, pcolourMap );
	}

	sdw::RetBoolean ReflectionModel::traceScreenSpace( sdw::Vec3 pcsOrigin
		, sdw::Vec3 pcsDirection
		, sdw::Mat4 pprojectToPixelMatrix
		, sdw::CombinedImage2DR32 pcsZBuffer
		, sdw::Vec2 pcsZBufferSize
		, sdw::Float pcsZThickness
		, sdw::Boolean pcsZBufferIsHyperbolic
		, sdw::Vec3 pclipInfo
		, sdw::Float pnearPlaneZ
		, sdw::Float pstride
		, sdw::Float pjitterFraction
		, sdw::Float pmaxSteps
		, sdw::Float pmaxRayTraceDistance
		, sdw::Vec2 & phitPixel
		, sdw::Vec3 & pcsHitPoint )
	{
		if ( !m_traceScreenSpace )
		{
			m_traceScreenSpace = m_writer.implementFunction< sdw::Boolean >( "c3d_traceScreenSpace"
				, [&]( sdw::Vec3 const & csOrigin
					, sdw::Vec3 const & csDirection
					, sdw::Mat4 const & projectToPixelMatrix
					, sdw::CombinedImage2DR32 const & csZBuffer
					, sdw::Vec2 const &
					, sdw::Float const & csZThickness
					, sdw::Boolean const & csZBufferIsHyperbolic
					, sdw::Vec3 const & clipInfo
					, sdw::Float const & nearPlaneZ
					, sdw::Float const & stride
					, sdw::Float const & jitterFraction
					, sdw::Float const & maxSteps
					, sdw::Float const & maxRayTraceDistance
					, sdw::Vec2 hitPixel
					, sdw::Vec3 csHitPoint )
			{
				// Clip ray to a near plane in 3D (doesn't have to be *the* near plane, although that would be a good idea)
				auto rayLength = m_writer.declLocale( "rayLength"
					, m_writer.ternary( ( ( csOrigin.z() + csDirection.z() * maxRayTraceDistance ) > nearPlaneZ )
						, ( nearPlaneZ - csOrigin.z() ) / csDirection.z()
						, maxRayTraceDistance ) );
				auto csEndPoint = m_writer.declLocale( "csEndPoint"
					, csDirection * rayLength + csOrigin );

				// Project into screen space
				auto H0 = m_writer.declLocale( "H0"
					, projectToPixelMatrix * vec4( csOrigin, 1.0 ) );
				auto H1 = m_writer.declLocale( "H1"
					, projectToPixelMatrix * vec4( csEndPoint, 1.0 ) );

				// There are a lot of divisions by w that can be turned into multiplications
				// at some minor precision loss...and we need to interpolate these 1/w values
				// anyway.
				//
				// Because the caller was required to clip to the near plane,
				// this homogeneous division (projecting from 4D to 2D) is guaranteed 
				// to succeed. 
				auto k0 = m_writer.declLocale( "k0"
					, 1.0_f / H0.w() );
				auto k1 = m_writer.declLocale( "k1"
					, 1.0_f / H1.w() );

				// Switch the original points to values that interpolate linearly in 2D
				auto Q0 = m_writer.declLocale( "Q0"
					, csOrigin * k0 );
				auto Q1 = m_writer.declLocale( "Q1"
					, csEndPoint * k1 );

				// Screen-space endpoints
				auto P0 = m_writer.declLocale( "P0"
					, H0.xy() * k0 );
				auto P1 = m_writer.declLocale( "P1"
					, H1.xy() * k1 );

				// [Optional clipping to frustum sides here]

				// Initialize to off screen
				hitPixel = vec2( -1.0_f, -1.0_f );

				// If the line is degenerate, make it cover at least one pixel
				// to avoid handling zero-pixel extent as a special case later
				P1 += vec2( m_writer.ternary( m_utils.distanceSquared( P0, P1 ) < 0.0001_f
					, 0.01_f
					, 0.0_f ) );

				auto delta = m_writer.declLocale( "delta"
					, P1 - P0 );

				// Permute so that the primary iteration is in x to reduce
				// large branches later
				auto permute = m_writer.declLocale< sdw::Int >( "permute" , 0_i );
				IF( m_writer, abs( delta.x() ) < abs( delta.y() ) )
				{
					// More-vertical line. Create a permutation that swaps x and y in the output
					permute = 1_i;

					// Directly swizzle the inputs
					delta = delta.yx();
					P1 = P1.yx();
					P0 = P0.yx();
				}
				FI

				// From now on, "x" is the primary iteration direction and "y" is the secondary one

				auto stepDirection = m_writer.declLocale( "stepDirection"
					, sign( delta.x() ) );
				auto invdx = m_writer.declLocale( "invdx"
					, stepDirection / delta.x() );
				auto dP = m_writer.declLocale( "dP"
					, vec2( stepDirection, invdx * delta.y() ) );

				// Track the derivatives of Q and k
				auto dQ = m_writer.declLocale( "dQ"
					, ( Q1 - Q0 ) * invdx );
				auto dk = m_writer.declLocale( "dk"
					, ( k1 - k0 ) * invdx );

				// Scale derivatives by the desired pixel stride
				dP *= stride; dQ *= stride; dk *= stride;

				// Offset the starting values by the jitter fraction
				P0 += dP * jitterFraction; Q0 += dQ * jitterFraction; k0 += dk * jitterFraction;

				// Slide P from P0 to P1, (now-homogeneous) Q from Q0 to Q1, and k from k0 to k1
				auto Q = m_writer.declLocale( "Q"
					, Q0 );
				auto k = m_writer.declLocale( "k"
					, k0 );

				// We track the ray depth at +/- 1/2 pixel to treat pixels as clip-space solid 
				// voxels. Because the depth at -1/2 for a given pixel will be the same as at 
				// +1/2 for the previous iteration, we actually only have to compute one value 
				// per iteration.
				auto prevZMaxEstimate = m_writer.declLocale( "prevZMaxEstimate"
					, csOrigin.z() );
				auto stepCount = m_writer.declLocale( "stepCount"
					, 0.0_f );
				auto rayZMax = m_writer.declLocale( "rayZMax"
					, prevZMaxEstimate );
				auto rayZMin = m_writer.declLocale( "rayZMin"
					, prevZMaxEstimate );
				auto sceneZMax = m_writer.declLocale( "sceneZMax"
					, rayZMax + 1e4f );

				// P1.x is never modified after this point, so pre-scale it by 
				// the step direction for a signed comparison
				auto end = m_writer.declLocale( "end"
					, P1.x() * stepDirection );

				// We only advance the z field of Q in the inner loop, since
				// Q.xy is never used until after the loop terminates.

				FOR( m_writer, sdw::Vec2, P, P0
					, ( ( ( P.x() * stepDirection ) <= end ) &&
						( stepCount < maxSteps ) &&
						( ( rayZMax < sceneZMax - csZThickness ) ||
							( rayZMin > sceneZMax ) ) &&
						( sceneZMax != 0.0_f ) )
					, ( Q.z() += dQ.z(), k += dk, stepCount += 1.0_f, P += dP ) )
				{
					hitPixel = m_writer.ternary( permute != 0_i
						, P.yx()
						, P );

					// The depth range that the ray covers within this loop
					// iteration.  Assume that the ray is moving in increasing z
					// and swap if backwards.  Because one end of the interval is
					// shared between adjacent iterations, we track the previous
					// value and then swap as needed to ensure correct ordering
					rayZMin = prevZMaxEstimate;

					// Compute the value at 1/2 pixel into the future
					rayZMax = ( dQ.z() * 0.5f + Q.z() ) / ( dk * 0.5f + k );
					prevZMaxEstimate = rayZMax;

					IF ( m_writer, rayZMin > rayZMax )
					{
						m_utils.swap( rayZMin, rayZMax );
					}
					FI

					// Camera-space z of the background
					sceneZMax = csZBuffer.fetch( ivec2( hitPixel ), 0_i );

					// This compiles away when csZBufferIsHyperbolic = false
					IF( m_writer, csZBufferIsHyperbolic )
					{
						sceneZMax = m_utils.reconstructCSZ( sceneZMax, clipInfo );
					}
					FI
				} // pixel on ray
				ROF

				Q.xy() += dQ.xy() * stepCount;
				csHitPoint = Q * ( 1.0f / k );

				// Matches the new loop condition:
				m_writer.returnStmt( ( rayZMax >= sceneZMax - csZThickness ) && ( rayZMin <= sceneZMax ) );
			}
			, sdw::InVec3{ m_writer, "csOrigin" }
			, sdw::InVec3{ m_writer, "csDirection" }
			, sdw::InMat4{ m_writer, "projectToPixelMatrix" }
			, sdw::InCombinedImage2DR32{ m_writer, "csZBuffer" }
			, sdw::InVec2{ m_writer, "csZBufferSize" }
			, sdw::InFloat{ m_writer, "csZThickness" }
			, sdw::InBoolean{ m_writer, "csZBufferIsHyperbolic" }
			, sdw::InVec3{ m_writer, "clipInfo" }
			, sdw::InFloat{ m_writer, "nearPlaneZ" }
			, sdw::InFloat{ m_writer, "stride" }
			, sdw::InFloat{ m_writer, "jitterFraction" }
			, sdw::InFloat{ m_writer, "maxSteps" }
			, sdw::InFloat{ m_writer, "maxRayTraceDistance" }
			, sdw::OutVec2{ m_writer, "hitPixel" }
			, sdw::OutVec3{ m_writer, "csHitPoint" } );
		}

		return m_traceScreenSpace( pcsOrigin
			, pcsDirection
			, pprojectToPixelMatrix
			, pcsZBuffer
			, pcsZBufferSize
			, pcsZThickness
			, pcsZBufferIsHyperbolic
			, pclipInfo
			, pnearPlaneZ
			, pstride
			, pjitterFraction
			, pmaxSteps
			, pmaxRayTraceDistance
			, phitPixel
			, pcsHitPoint );
	}

	sdw::RetVec3 ReflectionModel::computeSpecularReflEnvMaps( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::Float const & proughness
		, sdw::UInt const & penvMapIndex
		, sdw::CombinedImageCubeArrayRgba32 const & penvMap
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeSpecularReflEnvMaps )
		{
			m_computeSpecularReflEnvMaps = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeSpecularReflEnvMaps"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::UInt const & envMapIndex
					, sdw::Float const & roughness
					, sdw::CombinedImageCubeArrayRgba32 const & envMap )
				{
					auto reflected = m_writer.declLocale( "reflected"
						, reflect( wsIncident, wsNormal ) );
					m_writer.returnStmt( envMap.lod( vec4( reflected, m_writer.cast< sdw::Float >( envMapIndex ) )
						, roughness * sdw::Float( float( EnvironmentMipLevels ) ) ).xyz() );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InUInt{ m_writer, "envMapIndex" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" } );
		}

		return m_computeSpecularReflEnvMaps( pwsIncident
			, pwsNormal 
			, penvMapIndex
			, proughness
			, penvMap );
	}

	sdw::RetVec4 ReflectionModel::computeSheenReflEnvMaps( sdw::CombinedImage2DRgba32 const & pbrdf
		, sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::CombinedImageCubeArrayRgba32 const & penv
		, sdw::UInt const & penvIndex
		, sdw::Float const & pNdotV
		, BlendComponents & pcomponents
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeSheenReflEnvMaps )
		{
			m_computeSheenReflEnvMaps = m_writer.implementFunction< sdw::Vec4 >( "c3d_computeSheenReflEnvMap"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & sheenColour
					, sdw::CombinedImageCubeArrayRgba32 const & envMap
					, sdw::CombinedImage2DRgba32 const & brdfMap
					, sdw::UInt const & envMapIndex
					, sdw::Float const & roughness
					, sdw::Float const & NdotV )
				{
					auto reflected = m_writer.declLocale( "reflected"
						, reflect( wsIncident, wsNormal ) );
					auto radiance = m_writer.declLocale( "radiance"
						, envMap.lod( vec4( reflected, m_writer.cast< sdw::Float >( envMapIndex ) )
							, roughness * sdw::Float( float( EnvironmentMipLevels ) ) ).xyz() );
					auto brdf = m_writer.declLocale( "brdf"
						, BackgroundModel::getBrdf( brdfMap, NdotV, roughness ) );
					m_writer.returnStmt( vec4( sheenColour * radiance * brdf.z()
						, m_utils.directionalAlbedoSheen( dot( wsNormal, wsIncident ), roughness ) ) );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "sheenColour" }
				, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "brdfMap" }
				, sdw::InUInt{ m_writer, "envMapIndex" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::InFloat{ m_writer, "NdotV" } );
		}

		return m_computeSheenReflEnvMaps( pwsIncident
			, pwsNormal
			, pcomponents.sheenColour
			, penv
			, pbrdf
			, penvIndex
			, pcomponents.sheenRoughness
			, pNdotV );
	}

	sdw::RetVec3 ReflectionModel::computeRefrEnvMaps( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::CombinedImageCubeArrayRgba32 const & penvMap
		, sdw::UInt const & penvMapIndex
		, BlendComponents & components
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeRefrEnvMaps )
		{
			m_computeRefrEnvMaps = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeRefrEnvMap"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::CombinedImageCubeArrayRgba32 const & envMap
					, sdw::UInt const & envMapIndex
					, sdw::Float const & refractionRatio
					, sdw::Float const & roughness )
				{
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					m_writer.returnStmt( envMap.lod( vec4( refracted, m_writer.cast< sdw::Float >( envMapIndex ) )
						, roughness * sdw::Float( float( EnvironmentMipLevels ) ) ).xyz() );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" }
				, sdw::InUInt{ m_writer, "envMapIndex" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		return m_computeRefrEnvMaps( pwsIncident
			, pwsNormal
			, penvMap
			, penvMapIndex
			, components.ior
			, components.perceptualRoughness );
	}

	sdw::RetVec3 ReflectionModel::computeDiffuseEnvMaps( sdw::Vec3 const & pwsDirection
		, sdw::CombinedImageCubeArrayRgba32 const & penvMap
		, sdw::UInt const & penvMapIndex
		, BlendComponents & components
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeDiffuseEnvMaps )
		{
			m_computeDiffuseEnvMaps = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeDiffuseEnvMap"
				, [&]( sdw::Vec3 const & wsDirection
					, sdw::CombinedImageCubeArrayRgba32 const & envMap
					, sdw::UInt const & envMapIndex
					, sdw::Float const & roughness )
				{
					m_writer.returnStmt( envMap.lod( vec4( wsDirection, m_writer.cast< sdw::Float >( envMapIndex ) )
						, roughness * sdw::Float( float( EnvironmentMipLevels ) ) ).xyz() );
				}
				, sdw::InVec3{ m_writer, "wsDirection" }
				, sdw::InCombinedImageCubeArrayRgba32{ m_writer, "envMap" }
				, sdw::InUInt{ m_writer, "envMapIndex" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		return m_computeDiffuseEnvMaps( pwsDirection
			, penvMap
			, penvMapIndex
			, components.perceptualRoughness );
	}

	sdw::RetVec3 ReflectionModel::doComputeSpecularTransmission( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pwsNormal
		, sdw::CombinedImage2DRgba32 const & psceneMap
		, CameraData const & matrices
		, sdw::Vec2 psceneUv
		, BlendComponents & components
		, DebugOutputCategory const & debugOutput )
	{
		if ( !m_computeSpecularTransmission )
		{
			m_computeSpecularTransmission = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeSpecularTransmission"
				, [this, &components, &matrices]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsPosition
					, sdw::Vec3 const & wsNormal
					, sdw::CombinedImage2DRgba32 const & sceneMap
					, sdw::Vec2 sceneUv
					, sdw::Float const & ior
					, sdw::Vec3 const & albedo
					, sdw::Vec3 const & f0
					, sdw::Float const & roughness
					, sdw::Float const & thicknessFactor
					, sdw::Vec3 const & attenuationColour
					, sdw::Float const & attenuationDistance
					, sdw::Float const & dispersion )
				{
					IF( m_writer, thicknessFactor != 0.0_f
						&& ior != 0.0_f )
					{
						auto transmittedLight = m_writer.declLocale( "transmittedLight"
							, vec3( 0.0_f ) );
						auto transmissionRayLength = m_writer.declLocale( "transmissionRayLength"
							, 0.0_f );

						if ( components.hasMember( "dispersion" ) )
						{
							auto halfSpread = m_writer.declLocale( "halfSpread"
								, ( ior - 1.0_f ) * 0.025_f * dispersion );
							auto iors = m_writer.declLocale( "iors"
								, vec3( ior - halfSpread, ior, ior + halfSpread ) );
							auto transmissionRay = m_writer.declLocale( "transmissionRay"
								, vec3( 0.0_f ) );
							auto refractedRayExit = m_writer.declLocale( "refractedRayExit"
								, vec3( 0.0_f ) );
							auto ndcPos = m_writer.declLocale( "ndcPos"
								, vec4( 0.0_f ) );
							auto refractionCoords = m_writer.declLocale( "refractionCoords"
								, vec2( 0.0_f ) );

							for ( int i = 0; i < 3; ++i )
							{
								transmissionRay = getVolumeTransmissionRay( wsNormal, wsIncident, thicknessFactor, iors[i] );
								transmissionRayLength = length( transmissionRay );
								refractedRayExit = wsPosition + transmissionRay;
								ndcPos = matrices.worldToCurProj( vec4( refractedRayExit, 1.0_f ) );
								refractionCoords = ndcPos.xy() / ndcPos.w();
								refractionCoords += 1.0_f;
								refractionCoords /= 2.0_f;

								if ( i == 1 )
								{
									sceneUv = refractionCoords;
								}

								transmittedLight[i] = sceneMap.lod( refractionCoords, applyIorToRoughness( roughness, iors[i] ) * sdw::Float( float( EnvironmentMipLevels ) ) )[i];
							}
						}
						else
						{
							auto transmissionRay = m_writer.declLocale( "transmissionRay"
								, getVolumeTransmissionRay( wsNormal, wsIncident, thicknessFactor, ior ) );
							transmissionRayLength = length( transmissionRay );
							auto refractedRayExit = m_writer.declLocale( "refractedRayExit"
								, wsPosition + transmissionRay );
							auto ndcPos = m_writer.declLocale( "ndcPos"
								, matrices.worldToCurProj( vec4( refractedRayExit, 1.0_f ) ) );
							sceneUv = ( ndcPos.xy() / ndcPos.w() + vec2( 1.0_f ) ) * 0.5_f;

							transmittedLight = sceneMap.lod( sceneUv, applyIorToRoughness( roughness, ior ) * sdw::Float( float( EnvironmentMipLevels ) ) ).rgb();
						}

						auto attenuatedColor = m_writer.declLocale( "attenuatedColor"
							, applyVolumeAttenuation(transmissionRayLength, attenuationColour, attenuationDistance ) );

						if ( m_writer.hasGlobalVariable( "c3d_mapBrdf" ) )
						{
							auto brdfMap = m_writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapBrdf" );

							// Sample GGX LUT to get the specular component.
							auto NdotV = m_writer.declLocale( "NdotV"
								, clamp( dot( wsNormal, -wsIncident ), 0.0_f, 1.0_f ) );
							auto brdfSamplePoint = m_writer.declLocale( "brdfSamplePoint"
								, vec2( NdotV, roughness ) );
							auto brdf = m_writer.declLocale( "brdf"
								, brdfMap.lod( brdfSamplePoint, 0.0_f ) );
							auto F = m_writer.declLocale( "F"
								, m_utils.conductorFresnel( NdotV, f0 ) );
							auto specularColor = m_writer.declLocale( "specularColor"
								, sdw::fma( F, vec3( brdf.x() ), vec3( brdf.y() ) ) );

							m_writer.returnStmt( ( 1.0_f - specularColor ) * transmittedLight * attenuatedColor * albedo );
						}
						else
						{
							m_writer.returnStmt( transmittedLight * attenuatedColor * albedo );
						}
					}
					ELSE
					{
						m_writer.returnStmt( sceneMap.lod( sceneUv, applyIorToRoughness( roughness, ior ) * sdw::Float( float( EnvironmentMipLevels ) ) ).rgb() * albedo );
					}
					FI
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsPosition" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "sceneMap" }
				, sdw::InVec2{ m_writer, "sceneUv" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "albedo" }
				, sdw::InVec3{ m_writer, "f0" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::InFloat{ m_writer, "thicknessFactor" }
				, sdw::InVec3{ m_writer, "attenuationColour" }
				, sdw::InFloat{ m_writer, "attenuationDistance" }
				, sdw::InFloat{ m_writer, "dispersion" } );
		}

		return m_computeSpecularTransmission( pwsIncident
			, pwsPosition
			, pwsNormal
			, psceneMap
			, psceneUv
			, components.ior
			, components.baseColour
			, components.dielectricF0
			, components.perceptualRoughness
			, components.thicknessFactor
			, components.attenuationColour
			, components.attenuationDistance
			, components.getMember( "dispersion", 0.0_f ) );
	}

	void ReflectionModel::doComputeReflection( sdw::CombinedImageCubeArrayRgba32 const & envMap
		, sdw::Boolean const & hasEnvMap
		, BackgroundModel & background
		, sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & V
		, sdw::UInt const & envMapIndex
		, BlendComponents & components
		, sdw::Vec3 & reflectedDiffuse
		, sdw::Vec3 & reflectedSpecular
		, DebugOutputCategory const & debugOutput )
	{
		auto & writer = *envMap.getWriter();

		IF( writer, hasEnvMap && components.hasReflection != 0_u )
		{
			if ( m_allowReflections && background.hasReflectionSupport() )
			{
				// Diffuse reflection from background skybox.
				reflectedDiffuse = background.computeDiffuseReflection( wsNormal
					, debugOutput );
			}
			else
			{
				reflectedDiffuse = vec3( 0.0_f );
			}

			if ( m_hasEnvMap )
			{
				// Specular reflection from environment map.
				reflectedSpecular = computeSpecularReflEnvMaps( -V
					, wsNormal
					, components.perceptualRoughness
					, envMapIndex
					, envMap
					, debugOutput );
			}
		}
		ELSE
		{
			if ( m_allowReflections && background.hasReflectionSupport() )
			{
				// Reflection from background skybox.
				background.computeReflection( wsNormal
					, wsPosition
					, V
					, components
					, reflectedDiffuse
					, reflectedSpecular
					, debugOutput );
			}
		}
		FI
	}

	void ReflectionModel::doComputeSpecularTransmission( sdw::CombinedImageCubeArrayRgba32 const & envMap
		, sdw::Boolean const & hasEnvMap
		, BackgroundModel & background
		, sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & V
		, sdw::UInt const & envMapIndex
		, BlendComponents & components
		, sdw::Vec3 & refracted
		, DebugOutputCategory const & debugOutput )
	{
		auto & writer = *envMap.getWriter();

		IF( writer, components.ior != 0.0_f )
		{
			if ( m_hasEnvMap )
			{
				IF( writer, hasEnvMap )
				{
					refracted = computeRefrEnvMaps( -V
						, wsNormal
						, envMap
						, envMapIndex
						, components
						, debugOutput );
				}
				ELSE
				{
					if ( m_allowRefraction && background.hasRefractionSupport() )
					{
						refracted = background.computeRefraction( wsNormal
							, wsPosition
							, V
							, components
							, debugOutput );
					}
				}
				FI
			}
			else if ( m_allowRefraction && background.hasRefractionSupport() )
			{
				refracted = background.computeRefraction( wsNormal
					, wsPosition
					, V
					, components
					, debugOutput );
			}
		}
		FI
	}

	void ReflectionModel::doComputeDiffuse( sdw::CombinedImageCubeArrayRgba32 const & envMap
		, sdw::Boolean const & hasEnvMap
		, BackgroundModel & background
		, sdw::Vec3 const & wsDirection
		, sdw::UInt const & envMapIndex
		, BlendComponents & components
		, sdw::Vec3 & result
		, DebugOutputCategory const & debugOutput )
	{
		auto & writer = *envMap.getWriter();

		if ( m_hasEnvMap )
		{
			IF( writer, hasEnvMap )
			{
				result = computeDiffuseEnvMaps( wsDirection
					, envMap
					, envMapIndex
					, components
					, debugOutput );
			}
			ELSE
			{
				result = background.computeDiffuse( wsDirection
					, components
					, debugOutput );
			}
			FI
		}
		else
		{
			result = background.computeDiffuse( wsDirection
				, components
				, debugOutput );
		}
	}

	void ReflectionModel::doComputeClearcoat( sdw::CombinedImageCubeArrayRgba32 const & envMap
		, sdw::Boolean const & hasEnvMap
		, BackgroundModel & background
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & V
		, sdw::UInt const & envMapIndex
		, BlendComponents & components
		, sdw::Vec3 & coatReflected
		, DebugOutputCategory const & debugOutput )
	{
		if ( components.hasMember( "clearcoatFactor" ) )
		{
			if ( m_hasEnvMap )
			{
				IF( m_writer, hasEnvMap && components.hasReflection != 0_u )
				{
					coatReflected = computeSpecularReflEnvMaps( -V
						, components.clearcoatNormal
						, components.clearcoatRoughness
						, envMapIndex
						, envMap
						, debugOutput );
				}
				ELSE
				{
					if ( background.hasReflectionSupport() )
					{
						coatReflected = background.computeSpecularReflection( components.clearcoatNormal
							, wsPosition
							, V
							, components.clearcoatRoughness
							, debugOutput );
					}
				}
				FI
			}
			else if ( background.hasReflectionSupport() )
			{
				coatReflected = background.computeSpecularReflection( components.clearcoatNormal
					, wsPosition
					, V
					, components.clearcoatRoughness
					, debugOutput );
			}
		}
	}

	void ReflectionModel::doComputeSheen( sdw::CombinedImage2DRgba32 const & brdf
		, sdw::CombinedImageCubeArrayRgba32 const & envMap
		, sdw::Boolean const & hasEnvMap
		, BackgroundModel & background
		, sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & V
		, sdw::Float const & NdotV
		, sdw::UInt const & envMapIndex
		, BlendComponents & components
		, sdw::Vec4 & sheenReflected
		, DebugOutputCategory const & debugOutput )
	{
		if ( components.hasMember( "sheenColour" ) )
		{
			if ( m_hasEnvMap )
			{
				IF( m_writer, hasEnvMap && components.hasReflection != 0_u )
				{
					sheenReflected = computeSheenReflEnvMaps( brdf
						, -V
						, wsNormal
						, envMap
						, envMapIndex
						, NdotV
						, components
						, debugOutput );
				}
				ELSE
				{
					if ( background.hasReflectionSupport() )
					{
						sheenReflected = background.computeSheenReflection( wsNormal
							, wsPosition
							, V
							, NdotV
							, components
							, brdf
							, debugOutput );
					}
				}
				FI
			}
			else if ( background.hasReflectionSupport() )
			{
				sheenReflected = background.computeSheenReflection( wsNormal
					, wsPosition
					, V
					, NdotV
					, components
					, brdf
					, debugOutput );
			}
		}
	}
}
