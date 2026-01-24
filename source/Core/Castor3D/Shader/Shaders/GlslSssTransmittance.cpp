#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"

CU_ImplementSmartPtr( c3d::shader, SssTransmittance )

namespace c3d::shader
{
	SssTransmittance::SssTransmittance( sdw::ShaderWriter & writer
		, Shadow & shadows
		, ShadowOptions shadowOptions
		, SssProfiles const & sssProfiles
		, sdw::CombinedImage1DArrayRgba16 const & sssDiffusionProfiles )
		: m_writer{ writer }
		, m_shadows{ shadows }
		, m_sssProfiles{ sssProfiles }
		, m_sssDiffusionProfiles{ sssDiffusionProfiles }
		, m_shadowsType{ shadowOptions.type }
	{
	}
		
	sdw::Vec3 SssTransmittance::compute( DebugOutputCategory const & debugOutput
		, BlendComponents const & components
		, DirectionalLight const & plight
		, DirectionalShadowData const & pshadow
		, LightSurface const & plightSurface
		, sdw::Vec3 const & plightRadiance )
	{
		if ( !checkFlag( m_shadowsType, SceneFlag::eShadowDirectional )
			|| !m_shadows.hasMapDepthDirectional() )
		{
			return vec3( 0.0_f );
		}

		if ( !m_computeDirectional )
		{
			m_computeDirectional = m_writer.implementFunction< sdw::Vec3 >( "C3D_computeSssTransmittanceDirectional"
				, [this, &debugOutput]( shader::ShadowData const & shadows
					, sdw::UInt const & sssProfileIndex
					, sdw::Float const & transmittanceFactor
					, DirectionalLight const & light
					, sdw::Mat4x4 const & lightTransform
					, sdw::Vec3 const & wsVertexToLight
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition
					, sdw::Vec3 const & lightRadiance )
				{
					auto result = m_writer.declLocale( "result"
						, vec3( 0.0_f ) );

					// We shrink the position inwards the surface to avoid artifacts.
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, vec4( wsPosition - wsNormal * 0.005_f, 1.0_f ) );

					auto shadowDepths = m_writer.declLocale( "shadowDepth"
						, m_shadows.getDirectionalShadowDepths( shadows, lightTransform
							, wsNormal, shrinkedPos.xyz(), wsVertexToLight, 0_u ) );
					result = doCompute( debugOutput
						, shadowDepths.x()
						, shadowDepths.y()
						, sssProfileIndex
						, wsNormal
						, transmittanceFactor
						, wsVertexToLight );
					debugOutput.registerOutput( cuT( "Final Transmittance" ), result * lightRadiance );
					m_writer.returnStmt( result * lightRadiance );
				}
				, InShadowData{ m_writer, "shadows" }
				, sdw::InUInt{ m_writer, "sssProfileIndex" }
				, sdw::InFloat{ m_writer, "transmittanceFactor" }
				, InDirectionalLight{ m_writer, "light" }
				, sdw::InMat4{ m_writer, "lightTransform" }
				, sdw::InVec3{ m_writer, "wsVertexToLight" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" }
				, sdw::InVec3{ m_writer, "lightRadiance" } );
		}

		auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex", true );
		auto transmittanceFactor = components.getMember< sdw::Float >( "transmittance", true );
		return m_computeDirectional( pshadow.base()
			, sssProfileIndex
			, transmittanceFactor
			, plight
			, pshadow.transforms()[0]
			, plightSurface.vertexToLight().value()
			, plightSurface.N().value()
			, plightSurface.worldPosition().value().xyz()
			, plightRadiance );
	}

	sdw::Vec3 SssTransmittance::compute( DebugOutputCategory const & debugOutput
		, BlendComponents const & components
		, PointLight const & plight
		, PointShadowData const & pshadow
		, LightSurface const & plightSurface
		, sdw::Vec3 const & plightRadiance )
	{
		if ( !checkFlag( m_shadowsType, SceneFlag::eShadowPoint )
			|| !m_shadows.hasMapDepthPoint() )
		{
			return vec3( 0.0_f );
		}

		if ( !m_computePoint )
		{
			m_computePoint = m_writer.implementFunction< sdw::Vec3 >( "C3D_computeSssTransmittancePoint"
				, [this, &debugOutput]( shader::ShadowData const & shadows
					, sdw::UInt const & sssProfileIndex
					, sdw::Float const & transmittanceFactor
					, PointLight const & light
					, sdw::Int const & shadowMapIndex
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition
					, sdw::Vec3 const & lightRadiance )
				{
					auto result = m_writer.declLocale( "result"
						, vec3( 0.0_f ) );

					// We shrink the position inwards the surface to avoid artifacts.
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, wsPosition - wsNormal * 0.005_f );

					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, shrinkedPos - light.position() );
					auto shadowDepths = m_writer.declLocale( "shadowDepth"
						, m_shadows.getPointShadowDepths( shadows, computeRange( light ), wsNormal, lightToVertex, shadowMapIndex ) );
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, normalize( -lightToVertex ) );
					result = doCompute( debugOutput
						, shadowDepths.x()
						, shadowDepths.y()
						, sssProfileIndex
						, wsNormal
						, transmittanceFactor
						, vertexToLight );
					debugOutput.registerOutput( cuT( "Final Transmittance" ), result * lightRadiance );
					m_writer.returnStmt( result * lightRadiance );
				}
				, InShadowData{ m_writer, "shadows" }
				, sdw::InUInt{ m_writer, "sssProfileIndex" }
				, sdw::InFloat{ m_writer, "transmittanceFactor" }
				, InPointLight{ m_writer, "light" }
				, sdw::InInt{ m_writer, "shadowMapIndex" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" }
				, sdw::InVec3{ m_writer, "lightRadiance" } );
		}

		auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex", true );
		auto transmittanceFactor = components.getMember< sdw::Float >( "transmittance", true );
		return m_computePoint( pshadow.base()
			, sssProfileIndex
			, transmittanceFactor
			, plight
			, plight.shadowMapIndex()
			, plightSurface.N().value()
			, plightSurface.worldPosition().value().xyz()
			, plightRadiance );
	}

	sdw::Vec3 SssTransmittance::compute( DebugOutputCategory const & debugOutput
		, BlendComponents const & components
		, SpotLight const & plight
		, SpotShadowData const & pshadow
		, LightSurface const & plightSurface
		, sdw::Vec3 const & plightRadiance )
	{
		if ( !checkFlag( m_shadowsType, SceneFlag::eShadowSpot )
			|| !m_shadows.hasMapDepthSpot() )
		{
			return vec3( 0.0_f );
		}

		if ( !m_computeSpot )
		{
			m_computeSpot = m_writer.implementFunction< sdw::Vec3 >( "C3D_computeSssTransmittanceSpot"
				, [this, &debugOutput]( shader::ShadowData const & shadows
					, sdw::UInt const & sssProfileIndex
					, sdw::Float const & transmittanceFactor
					, SpotLight const & light
					, sdw::Mat4x4 const & lightTransform
					, sdw::Int const & shadowMapIndex
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition
					, sdw::Vec3 const & lightRadiance )
				{
					auto result = m_writer.declLocale( "result"
						, vec3( 0.0_f ) );
					// We shrink the position inwards the surface to avoid artifacts.
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, wsPosition - wsNormal * 0.005_f );
					auto wsVertexToLight = m_writer.declLocale( "wsVertexToLight"
						, light.position() - shrinkedPos );
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, normalize( wsVertexToLight ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( vertexToLight, light.direction() ) );

					sdwIF( m_writer, spotFactor > light.outerCutOffCos() )
					{
						spotFactor = clamp( ( spotFactor - light.outerCutOffCos() ) / light.cutOffsCosDiff(), 0.0_f, 1.0_f );
						auto shadowDepths = m_writer.declLocale( "shadowDepth"
							, m_shadows.getSpotShadowDepths( shadows, lightTransform, wsNormal, shrinkedPos.xyz(), wsVertexToLight, shadowMapIndex ) );
						result = spotFactor * doCompute( debugOutput
							, shadowDepths.x()
							, shadowDepths.y()
							, sssProfileIndex
							, wsNormal
							, transmittanceFactor
							, vertexToLight );
					}
					sdwFI

					debugOutput.registerOutput( cuT( "Final Transmittance" ), result * lightRadiance );
					m_writer.returnStmt( result * lightRadiance );
				}
				, InShadowData{ m_writer, "shadows" }
				, sdw::InUInt{ m_writer, "sssProfileIndex" }
				, sdw::InFloat{ m_writer, "transmittanceFactor" }
				, InSpotLight{ m_writer, "light" }
				, sdw::InMat4{ m_writer, "lightTransform" }
				, sdw::InInt{ m_writer, "shadowMapIndex" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" }
				, sdw::InVec3{ m_writer, "lightRadiance" } );
		}

		auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex", true );
		auto transmittanceFactor = components.getMember< sdw::Float >( "transmittance", true );
		return m_computeSpot( pshadow.base()
			, sssProfileIndex
			, transmittanceFactor
			, plight
			, pshadow.transform()
			, plight.shadowMapIndex()
			, plightSurface.N().value()
			, plightSurface.worldPosition().value().xyz()
			, plightRadiance );
	}

	sdw::Vec3 SssTransmittance::doCompute( DebugOutputCategory const & debugOutput
		, sdw::Float const & lightSpaceDepth
		, sdw::Float const & shadowDepth
		, sdw::UInt const & sssProfileIndex
		, sdw::Vec3 const & worldNormal
		, sdw::Float const & transmittanceFactor
		, sdw::Vec3 const & vertexToLight )
	{
		auto sssProfile = m_writer.declLocale( "sssProfile"
			, m_sssProfiles.getProfile( sssProfileIndex ) );
		auto factor = m_writer.declLocale( "factor"
			, vec3( 0.0_f ) );
		debugOutput.registerOutput( cuT( "Transmittance Factor" ), transmittanceFactor );
		debugOutput.registerOutput( cuT( "Shadow Depth" ), shadowDepth );
		debugOutput.registerOutput( cuT( "Light Space Depth" ), lightSpaceDepth );

		/**
		* Calculate the scale of the effect.
		*/
		auto scale = m_writer.declLocale( "scale"
			, sssProfile.thicknessScale() / sssProfile.gaussianWidth() );
		debugOutput.registerOutput( cuT( "Profile Scale / 10" ), sssProfile.thicknessScale() / 10.0f );
		debugOutput.registerOutput( cuT( "Profile Gaussian Width / 30" ), sssProfile.gaussianWidth() / 30.0f );
		debugOutput.registerOutput( cuT( "Final Scale" ), scale );

		/**
		* Now we calculate the thickness, in mm, from the light point of view:
		*/
		auto thickness = m_writer.declLocale( "thickness"
			, 1000.0_f * scale * sdw::abs( shadowDepth - lightSpaceDepth ) );
		debugOutput.registerOutput( cuT( "Raw Thickness" ), sdw::abs( shadowDepth - lightSpaceDepth ) );
		debugOutput.registerOutput( cuT( "Final Thickness / 1000" ), thickness / 1000.0f );

		/**
		* Armed with the thickness, we can now calculate the color by means of the
		* transmittance profile.
		*/
		auto profile = m_writer.declLocale( "profile"
			, m_sssDiffusionProfiles.lod( vec2( thickness, m_writer.cast< sdw::Float >( sssProfileIndex ) / 511.0_f ), 0.0_f ).rgb() );
		debugOutput.registerOutput( cuT( "Final Profile" ), profile );
		/**
		* Using the profile, we finally approximate the transmitted lighting from
		* the back of the object:
		*/
		factor = profile
			* transmittanceFactor
			* clamp( 0.3_f + dot( vertexToLight, -worldNormal )
				, 0.0_f
				, 1.0_f );
		debugOutput.registerOutput( cuT( "Raw Transmittance" ), factor );
		return factor;
	}
}
