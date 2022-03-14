#include "Castor3D/Shader/Shaders/GlslReflection.hpp"

#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

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

	sdw::Vec4 ReflectionModel::computeScreenSpace( MatrixData const & matrixData
		, sdw::Vec3 const & viewPosition
		, sdw::Vec3 const & worldNormal
		, sdw::Vec2 const & texcoord
		, sdw::Vec4 const & ssrSettings
		, sdw::CombinedImage2DR32 const & depthMap
		, sdw::CombinedImage2DRgba32 const & normalMap
		, sdw::CombinedImage2DRgba32 const & colourMap )
	{
		declareComputeScreenSpace( matrixData );
		return m_computeScreenSpace( viewPosition
			, worldNormal
			, texcoord
			, ssrSettings
			, depthMap
			, normalMap
			, colourMap );
	}

	sdw::Boolean ReflectionModel::traceScreenSpace( sdw::Vec3 csOrigin
		, sdw::Vec3 csDirection
		, sdw::Mat4 projectToPixelMatrix
		, sdw::CombinedImage2DR32 csZBuffer
		, sdw::Vec2 csZBufferSize
		, sdw::Float csZThickness
		, sdw::Boolean csZBufferIsHyperbolic
		, sdw::Vec3 clipInfo
		, sdw::Float nearPlaneZ
		, sdw::Float stride
		, sdw::Float jitterFraction
		, sdw::Float maxSteps
		, sdw::Float maxRayTraceDistance
		, sdw::Vec2 & hitPixel
		, sdw::Vec3 & csHitPoint )
	{
		declareTraceScreenSpace();
		return m_traceScreenSpace( csOrigin
			, csDirection
			, projectToPixelMatrix
			, csZBuffer
			, csZBufferSize
			, csZThickness
			, csZBufferIsHyperbolic
			, clipInfo
			, nearPlaneZ
			, stride
			, jitterFraction
			, maxSteps
			, maxRayTraceDistance
			, hitPixel
			, csHitPoint );
	}

	void ReflectionModel::declareTraceScreenSpace()
	{
		if ( m_traceScreenSpace )
		{
			return;
		}

		auto reconstructCSZ = m_writer.implementFunction< sdw::Float >( "c3d_reconstructCSZ"
			, [&]( sdw::Float const & depthBufferValue
				, sdw::Vec3 const clipInfo )
		{
			m_writer.returnStmt( clipInfo[0] / ( depthBufferValue * clipInfo[1] + clipInfo[2] ) );
		}
		, sdw::InFloat{ m_writer, "depthBufferValue" }
		, sdw::InVec3{ m_writer, "clipInfo" } );

		m_traceScreenSpace = m_writer.implementFunction< sdw::Boolean >( "c3d_traceScreenSpace"
			, [&]( sdw::Vec3 const & csOrigin
				, sdw::Vec3 const & csDirection
				, sdw::Mat4 const & projectToPixelMatrix
				, sdw::CombinedImage2DR32 const & csZBuffer
				, sdw::Vec2 const & csZBufferSize
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
			FI;

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
				FI;

// Camera-space z of the background
				sceneZMax = csZBuffer.fetch( ivec2( hitPixel ), 0_i );

				// This compiles away when csZBufferIsHyperbolic = false
				IF( m_writer, csZBufferIsHyperbolic )
				{
					sceneZMax = reconstructCSZ( sceneZMax, clipInfo );
				}
				FI;
			} // pixel on ray
			ROF;

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

	void ReflectionModel::declareComputeScreenSpace( MatrixData const & matrixData )
	{
		if ( m_computeScreenSpace )
		{
			return;
		}

		m_computeScreenSpace = m_writer.implementFunction< sdw::Vec4 >( "c3d_computeScreenSpace"
			, [&]( sdw::Vec3 const & viewPosition
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
					, vec4( 0.0_f ) );

				WHILE( m_writer, stepCount < ssrForwardMaxStepCount )
				{
					rayMarchPosition += reflectionVector.xyz() * ssrStepSize;
					rayMarchTexPosition = matrixData.viewToScreenUV( m_utils, vec4( -rayMarchPosition, 1.0_f ) );

					sceneZ = depthMap.lod( rayMarchTexPosition.xy(), 0.0_f );
					sceneZ = matrixData.projToView( m_utils, rayMarchTexPosition.xy(), sceneZ ).z();

					IF( m_writer, -sceneZ <= -rayMarchPosition.z() )
					{
						forwardStepCount = stepCount;
						stepCount = ssrForwardMaxStepCount;
					}
					ELSE
					{
						stepCount += 1.0_f;
					}
					FI;
				}
				ELIHW;

				IF( m_writer, forwardStepCount < ssrForwardMaxStepCount )
				{
					stepCount = 0.0_f;

					WHILE( m_writer, stepCount < ssrBackwardMaxStepCount )
					{
						rayMarchPosition -= reflectionVector.xyz() * ssrStepSize / ssrBackwardMaxStepCount;
						rayMarchTexPosition = matrixData.viewToScreenUV( m_utils, vec4( -rayMarchPosition, 1.0_f ) );

						sceneZ = depthMap.lod( rayMarchTexPosition.xy(), 0.0_f );
						sceneZ = matrixData.projToView( m_utils, rayMarchTexPosition.xy(), sceneZ ).z();

						IF( m_writer, -sceneZ > -rayMarchPosition.z() )
						{
							stepCount = ssrBackwardMaxStepCount;
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
					, abs( dot( worldNormal, viewDir ) ) + epsilon );
				auto ssrReflectionNormal = m_writer.declLocale( "ssrReflectionNormal"
					, normalMap.sample( rayMarchTexPosition.xy() ).xyz() );
				auto ssrDistanceFactor = m_writer.declLocale( "ssrDistanceFactor"
					, vec2( distance( 0.5_f, texcoord.x() ), distance( 0.5_f, texcoord.y() ) ) * 2.0f );
				auto ssrFactor = m_writer.declLocale( "ssrFactor"
					, ( 1.0_f - abs( nDotV ) )
						* ( 1.0f - forwardStepCount / ssrForwardMaxStepCount )
						* clamp( 1.0f - ssrDistanceFactor.x() - ssrDistanceFactor.y(), 0.0_f, 1.0_f )
						* ( 1.0f / ( 1.0f + abs( sceneZ - rayMarchPosition.z() ) * ssrDepthMult ) )
						* ( 1.0f - clamp( dot( ssrReflectionNormal, worldNormal ), 0.0_f, 1.0_f ) ) );

				auto reflectionColor = m_writer.declLocale( "reflectionColor"
					, colourMap.sample( rayMarchTexPosition.xy() ).rgb() );

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
					, max( 0.0_f, dot( -incident, surface.worldNormal ) ) );
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
