#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d::shader, SssTransmittance )

namespace castor3d::shader
{
	SssTransmittance::SssTransmittance( sdw::ShaderWriter & writer
		, ShadowOptions shadowOptions
		, SssProfiles const & sssProfiles )
		: m_writer{ writer }
		, m_sssProfiles{ sssProfiles }
		, m_shadows{ shadowOptions.type }
	{
	}
		
	sdw::Vec3 SssTransmittance::compute( DebugOutput & debugOutput
	, BlendComponents const & components
		, DirectionalLight const & plight
		, DirectionalShadowData const & pshadow
		, LightSurface const & plightSurface )
	{
		if ( !checkFlag( m_shadows, SceneFlag::eShadowDirectional )
			|| !m_writer.hasGlobalVariable( Shadow::MapDepthDirectional ) )
		{
			return vec3( 0.0_f );
		}

		if ( !m_computeDirectional )
		{
			m_computeDirectional = m_writer.implementFunction< sdw::Vec3 >( "C3D_computeSssTransmittanceDirectional"
				, [&]( sdw::UInt const & sssProfileIndex
					, sdw::Float const & sssTransmittance
					, DirectionalLight const & light
					, sdw::Mat4x4 const & lightTransform
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition )
				{
					auto result = m_writer.declLocale( "result"
						, vec3( 0.0_f ) );
					auto c3d_mapNormalDepthDirectional = m_writer.getVariable< sdw::CombinedImage2DArrayR32 >( Shadow::MapDepthDirectional );

					// We shrink the position inwards the surface to avoid artifacts.
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, vec4( wsPosition - wsNormal * 0.005_f, 1.0_f ) );

					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, lightTransform * shrinkedPos );
					lightSpacePosition.xy() /= lightSpacePosition.w();
					lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
						, vec2( 0.5_f )
						, vec2( 0.5_f ) );
					auto shadowDepth = m_writer.declLocale( "shadowDepth"
						, c3d_mapNormalDepthDirectional.lod( vec3( lightSpacePosition.xy(), 0.0_f ), 0.0_f ) );
					m_writer.returnStmt( doCompute( debugOutput
						, lightSpacePosition.z()
						, shadowDepth
						, sssProfileIndex
						, wsNormal
						, sssTransmittance
						, -light.direction() /* vertexTolight */ ) );
				}
				, sdw::InUInt{ m_writer, "sssProfileIndex" }
				, sdw::InFloat{ m_writer, "sssTransmittance" }
				, InDirectionalLight{ m_writer, "light" }
				, sdw::InMat4{ m_writer, "lightTransform" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" } );
		}

		auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex", true );
		auto sssTransmittance = components.getMember< sdw::Float >( "transmittance", true );
		return m_computeDirectional( sssProfileIndex
			, sssTransmittance
			, plight
			, pshadow.transforms()[0]
			, plightSurface.N()
			, plightSurface.worldPosition() );
	}

	sdw::Vec3 SssTransmittance::compute( DebugOutput & debugOutput
		, BlendComponents const & components
		, PointLight const & plight
		, PointShadowData const & pshadow
		, LightSurface const & plightSurface )
	{
		if ( !checkFlag( m_shadows, SceneFlag::eShadowPoint )
			|| !m_writer.hasGlobalVariable( Shadow::MapDepthPoint ) )
		{
			return vec3( 0.0_f );
		}

		if ( !m_computePoint )
		{
			m_computePoint = m_writer.implementFunction< sdw::Vec3 >( "C3D_computeSssTransmittancePoint"
				, [&]( sdw::UInt const & sssProfileIndex
					, sdw::Float const & sssTransmittance
					, PointLight const & light
					, sdw::Int const & shadowMapIndex
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition )
				{
					auto result = m_writer.declLocale( "result"
						, vec3( 0.0_f ) );
					auto c3d_mapNormalDepthPoint = m_writer.getVariable< sdw::CombinedImageCubeArrayR32 >( Shadow::MapDepthPoint );

					// We shrink the position inwards the surface to avoid artifacts.
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, wsPosition - wsNormal * 0.005_f );

					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, shrinkedPos - light.position() );
					auto shadowDepth = m_writer.declLocale( "shadowDepth"
						, c3d_mapNormalDepthPoint.lod( vec4( lightToVertex, m_writer.cast< sdw::Float >( shadowMapIndex ) ), 0.0_f ) );
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, normalize( -lightToVertex ) );
					m_writer.returnStmt( doCompute( debugOutput
						, ( shrinkedPos - light.position() ).z() / computeRange( light )
						, shadowDepth
						, sssProfileIndex
						, wsNormal
						, sssTransmittance
						, vertexToLight ) );
				}
				, sdw::InUInt{ m_writer, "sssProfileIndex" }
				, sdw::InFloat{ m_writer, "sssTransmittance" }
				, InPointLight{ m_writer, "light" }
				, sdw::InInt{ m_writer, "shadowMapIndex" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" } );
		}

		auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex", true );
		auto sssTransmittance = components.getMember< sdw::Float >( "transmittance", true );
		return m_computePoint( sssProfileIndex
			, sssTransmittance
			, plight
			, plight.shadowMapIndex()
			, plightSurface.N()
			, plightSurface.worldPosition() );
	}

	sdw::Vec3 SssTransmittance::compute( DebugOutput & debugOutput
		, BlendComponents const & components
		, SpotLight const & plight
		, SpotShadowData const & pshadow
		, LightSurface const & plightSurface )
	{
		if ( !checkFlag( m_shadows, SceneFlag::eShadowSpot )
			|| !m_writer.hasGlobalVariable( Shadow::MapDepthSpot ) )
		{
			return vec3( 0.0_f );
		}

		if ( !m_computeSpot )
		{
			m_computeSpot = m_writer.implementFunction< sdw::Vec3 >( "C3D_computeSssTransmittanceSpot"
				, [&]( sdw::UInt const & sssProfileIndex
					, sdw::Float const & sssTransmittance
					, SpotLight const & light
					, sdw::Mat4x4 const & lightTransform
					, sdw::Int const & shadowMapIndex
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition )
				{
					auto result = m_writer.declLocale( "result"
						, vec3( 0.0_f ) );
					auto c3d_mapDepthSpot = m_writer.getVariable< sdw::CombinedImage2DArrayR32 >( Shadow::MapDepthSpot );

					// We shrink the position inwards the surface to avoid artifacts.
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, wsPosition - wsNormal * 0.005_f );
					debugOutput.registerOutput( "SSSTransmittance", "Shrinked Position", shrinkedPos );

					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, lightTransform * vec4( shrinkedPos, 1.0_f ) );
					debugOutput.registerOutput( "SSSTransmittance", "Raw Light Space Position", lightSpacePosition );
					lightSpacePosition /= lightSpacePosition.w();
					debugOutput.registerOutput( "SSSTransmittance", "Perspective Corrected Light Space Position", lightSpacePosition );
					lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
						, vec2( 0.5_f )
						, vec2( 0.5_f ) );
					debugOutput.registerOutput( "SSSTransmittance", "Offseted Light Space Position", lightSpacePosition );
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, shrinkedPos - light.position() );
					debugOutput.registerOutput( "SSSTransmittance", "Raw Light To Vertex", lightToVertex );
					auto shadowDepth = m_writer.declLocale( "shadowDepth"
						, c3d_mapDepthSpot.lod( vec3( lightSpacePosition.xy(), m_writer.cast< sdw::Float >( shadowMapIndex ) ), 0.0_f ) );
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, normalize( -lightToVertex ) );
					auto invShadowDepth = m_writer.declLocale( "invShadowDepth"
						, ( 1.0_f - shadowDepth ) );
					auto invLightSpaceDepth = m_writer.declLocale( "invLightSpaceDepth"
						, ( 1.0_f - lightSpacePosition.z() ) );
					m_writer.returnStmt( doCompute( debugOutput
						, invLightSpaceDepth
						, invShadowDepth
						, sssProfileIndex
						, wsNormal
						, sssTransmittance
						, vertexToLight ) );
				}
				, sdw::InUInt{ m_writer, "sssProfileIndex" }
				, sdw::InFloat{ m_writer, "sssTransmittance" }
				, InSpotLight{ m_writer, "light" }
				, sdw::InMat4{ m_writer, "lightTransform" }
				, sdw::InInt{ m_writer, "shadowMapIndex" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" } );
		}

		auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex", true );
		auto sssTransmittance = components.getMember< sdw::Float >( "transmittance", true );
		return m_computeSpot( sssProfileIndex
			, sssTransmittance
			, plight
			, pshadow.transform()
			, plight.shadowMapIndex()
			, plightSurface.N()
			, plightSurface.worldPosition() );
	}

	sdw::Vec3 SssTransmittance::doCompute( DebugOutput & debugOutput
		, sdw::Float const & lightSpaceDepth
		, sdw::Float const & shadowDepth
		, sdw::UInt const & sssProfileIndex
		, sdw::Vec3 const & worldNormal
		, sdw::Float const & translucency
		, sdw::Vec3 const & vertexToLight )
	{
		auto sssProfile = m_writer.declLocale( "sssProfile"
			, m_sssProfiles.getProfile( m_writer.cast< sdw::UInt >( sssProfileIndex ) ) );
		auto factor = m_writer.declLocale( "factor"
			, vec3( 0.0_f ) );
		debugOutput.registerOutput( "SSSTransmittance", "Translucency", translucency );
		debugOutput.registerOutput( "SSSTransmittance", "Shadow Depth", shadowDepth );
		debugOutput.registerOutput( "SSSTransmittance", "Light Space Depth", lightSpaceDepth );
		debugOutput.registerOutput( "SSSTransmittance", "World Normal", sdw::fma( worldNormal, vec3( 0.5_f ), vec3( 0.5_f ) ) );
		debugOutput.registerOutput( "SSSTransmittance", "Vertex To Light", sdw::fma( vertexToLight, vec3( 0.5_f ), vec3( 0.5_f ) ) );

		/**
		* Calculate the scale of the effect.
		*/
		auto scale = m_writer.declLocale( "scale"
			, sssProfile.thicknessScale() * translucency / sssProfile.gaussianWidth() );
		debugOutput.registerOutput( "SSSTransmittance", "Scale", scale );

		/**
		* Now we calculate the thickness from the light point of view:
		*/
		auto thickness = m_writer.declLocale( "thickness"
			, scale * sdw::abs( shadowDepth - lightSpaceDepth ) );
		debugOutput.registerOutput( "SSSTransmittance", "Raw Thickness", sdw::abs( shadowDepth - lightSpaceDepth ) );
		debugOutput.registerOutput( "SSSTransmittance", "Thickness", thickness );

		/**
		* Armed with the thickness, we can now calculate the color by means of the
		* transmittance profile.
		*/
		auto dd = m_writer.declLocale( "dd"
			, thickness * thickness );
		auto profile = m_writer.declLocale( "profile"
			, vec3( 0.0_f ) );

		FOR( m_writer, sdw::UInt, i, 0u, i < sssProfile.transmittanceProfileSize(), ++i )
		{
			auto profileFactors = m_writer.declLocale( "profileFactors"
				, sssProfile.transmittanceProfile()[i] );
			profile += profileFactors.rgb() * exp( dd / profileFactors.a() );
		}
		ROF;
		debugOutput.registerOutput( "SSSTransmittance", "Profile", profile );
		/**
		* Using the profile, we finally approximate the transmitted lighting from
		* the back of the object:
		*/
		factor = profile
			* translucency
			* clamp( 0.3_f + dot( vertexToLight, worldNormal )
				, 0.0_f
				, 1.0_f );
		debugOutput.registerOutput( "SSSTransmittance", "Factor", factor );
		return factor;
	}
}
