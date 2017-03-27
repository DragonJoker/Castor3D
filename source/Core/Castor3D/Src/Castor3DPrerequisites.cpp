#include "Castor3DPrerequisites.hpp"

#include "Engine.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>

IMPLEMENT_EXPORTED_OWNED_BY( Castor3D::Engine, Engine )
IMPLEMENT_EXPORTED_OWNED_BY( Castor3D::RenderSystem, RenderSystem )
IMPLEMENT_EXPORTED_OWNED_BY( Castor3D::Scene, Scene )

using namespace Castor;

namespace Castor3D
{
	template<> String const TopologyNamer< Topology::ePoints >::Name = cuT( "points" );
	template<> String const TopologyNamer< Topology::eLines >::Name = cuT( "lines" );
	template<> String const TopologyNamer< Topology::eLineLoop >::Name = cuT( "line_loop" );
	template<> String const TopologyNamer< Topology::eLineStrip >::Name = cuT( "line_strip" );
	template<> String const TopologyNamer< Topology::eTriangles >::Name = cuT( "triangles" );
	template<> String const TopologyNamer< Topology::eTriangleStrips >::Name = cuT( "triangle_strip" );
	template<> String const TopologyNamer< Topology::eTriangleFan >::Name = cuT( "triangle_fan" );
	template<> String const TopologyNamer< Topology::eQuads >::Name = cuT( "quads" );
	template<> String const TopologyNamer< Topology::eQuadStrips >::Name = cuT( "quad_strip" );
	template<> String const TopologyNamer< Topology::ePolygon >::Name = cuT( "polygon" );

	String GetName( ElementType p_type )
	{
		switch ( p_type )
		{
		case ElementType::eFloat:
			return cuT( "float" );
			break;

		case ElementType::eVec2:
			return cuT( "vec2f" );
			break;

		case ElementType::eVec3:
			return cuT( "vec3f" );
			break;

		case ElementType::eVec4:
			return cuT( "vec4f" );
			break;

		case ElementType::eColour:
			return cuT( "colour" );
			break;

		case ElementType::eInt:
			return cuT( "int" );
			break;

		case ElementType::eIVec2:
			return cuT( "vec2i" );
			break;

		case ElementType::eIVec3:
			return cuT( "vec3i" );
			break;

		case ElementType::eIVec4:
			return cuT( "vec4i" );
			break;

		case ElementType::eUInt:
			return cuT( "uint" );
			break;

		case ElementType::eUIVec2:
			return cuT( "vec2ui" );
			break;

		case ElementType::eUIVec3:
			return cuT( "vec3ui" );
			break;

		case ElementType::eUIVec4:
			return cuT( "vec4ui" );
			break;

		case ElementType::eMat2:
			return cuT( "mat2" );
			break;

		case ElementType::eMat3:
			return cuT( "mat3" );
			break;

		case ElementType::eMat4:
			return cuT( "mat4" );
			break;

		default:
			assert( false && "Unsupported vertex buffer attribute type." );
			break;
		}

		return 0;
	}

	void ComputePreLightingMapContributions(
		GLSL::GlslWriter & p_writer,
		GLSL::Vec3 & p_normal,
		GLSL::Float & p_shininess,
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		SceneFlags const & p_sceneFlags )
	{
		using namespace GLSL;
		auto l_texCoord( p_writer.GetBuiltin< Vec3 >( cuT( "l_texCoord" ) ) );
		
		if ( CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
		{
			auto vtx_normal( p_writer.GetBuiltin< Vec3 >( cuT( "vtx_normal" ) ) );
			auto vtx_tangent( p_writer.GetBuiltin< Vec3 >( cuT( "vtx_tangent" ) ) );
			auto vtx_bitangent( p_writer.GetBuiltin< Vec3 >( cuT( "vtx_bitangent" ) ) );
			auto c3d_mapNormal( p_writer.GetBuiltin< Sampler2D >( ShaderProgram::MapNormal ) );

			auto l_tbn = p_writer.GetLocale( cuT( "l_tbn" ), mat3( vtx_tangent, vtx_bitangent, p_normal ) );
			auto l_v3MapNormal = p_writer.GetLocale( cuT( "l_v3MapNormal" ), texture( c3d_mapNormal, l_texCoord.xy() ).xyz() );
			l_v3MapNormal = l_v3MapNormal * 2.0_f - vec3( 1.0_f, 1.0, 1.0 );
			p_normal = normalize( l_tbn * l_v3MapNormal );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eGloss ) )
		{
			auto c3d_mapGloss( p_writer.GetBuiltin< Sampler2D >( ShaderProgram::MapGloss ) );

			p_shininess = texture( c3d_mapGloss, l_texCoord.xy() ).r();
		}
	}

	void ComputePostLightingMapContributions(
		GLSL::GlslWriter & p_writer,
		GLSL::Vec3 & p_ambient,
		GLSL::Vec3 & p_diffuse,
		GLSL::Vec3 & p_specular,
		GLSL::Vec3 & p_emissive,
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		SceneFlags const & p_sceneFlags )
	{
		using namespace GLSL;
		auto l_texCoord( p_writer.GetBuiltin< Vec3 >( cuT( "l_texCoord" ) ) );

		if ( CheckFlag( p_textureFlags, TextureChannel::eColour ) )
		{
			auto c3d_mapColour( p_writer.GetBuiltin< Sampler2D >( ShaderProgram::MapColour ) );

			p_ambient += texture( c3d_mapColour, l_texCoord.xy() ).xyz();
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eAmbient ) )
		{
			auto c3d_mapAmbient( p_writer.GetBuiltin< Sampler2D >( ShaderProgram::MapAmbient ) );

			p_ambient += texture( c3d_mapAmbient, l_texCoord.xy() ).xyz();
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) )
		{
			auto c3d_mapDiffuse( p_writer.GetBuiltin< Sampler2D >( ShaderProgram::MapDiffuse ) );

			p_diffuse *= texture( c3d_mapDiffuse, l_texCoord.xy() ).xyz();
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eSpecular ) )
		{
			auto c3d_mapSpecular( p_writer.GetBuiltin< Sampler2D >( ShaderProgram::MapSpecular ) );

			p_specular *= texture( c3d_mapSpecular, l_texCoord.xy() ).xyz();
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::eEmissive ) )
		{
			auto c3d_mapEmissive( p_writer.GetBuiltin< Sampler2D >( ShaderProgram::MapEmissive ) );

			p_emissive = texture( c3d_mapEmissive, l_texCoord.xy() ).xyz();
		}
	}

	ParallaxShadowFunction DeclareParallaxShadowFunc( GLSL::GlslWriter & p_writer
		, TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags )
	{
		ParallaxShadowFunction l_result;

		if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
			&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
		{
			using namespace GLSL;
			auto c3d_mapHeight( p_writer.GetBuiltin< Sampler2D >( ShaderProgram::MapHeight ) );
			auto c3d_fheightScale( p_writer.GetBuiltin< Float >( cuT( "c3d_fheightScale" ) ) );

			l_result = p_writer.ImplementFunction< Float >( cuT( "ParallaxSoftShadowMultiplier" )
				, [&]( Vec3 const & p_lightDir
				, Vec2 const p_initialTexCoord
				, Float p_initialHeight )
				{
					auto l_shadowMultiplier = p_writer.GetLocale( cuT( "l_shadowMultiplier" ), 1.0_f );
					auto l_minLayers = p_writer.GetLocale( cuT( "l_minLayers" ), 10.0_f );
					auto l_maxLayers = p_writer.GetLocale( cuT( "l_maxLayers" ), 20.0_f );

					// calculate lighting only for surface oriented to the light source
					IF( p_writer, dot( vec3( 0.0_f, 0, 1 ), p_lightDir ) > 0.0_f )
					{
						// calculate initial parameters
						auto l_numSamplesUnderSurface = p_writer.GetLocale( cuT( "l_numSamplesUnderSurface" ), 0.0_f );
						l_shadowMultiplier = 0;
						auto l_numLayers = p_writer.GetLocale( cuT( "l_numLayers" )
							, mix( l_maxLayers
								, l_minLayers
								, GLSL::abs( dot( vec3( 0.0_f, 0.0, 1.0 ), p_lightDir ) ) ) );
						auto l_layerHeight = p_writer.GetLocale( cuT( "l_layerHeight" ), p_initialHeight / l_numLayers );
						auto l_texStep = p_writer.GetLocale( cuT( "l_deltaTexCoords" ), p_writer.Paren( p_lightDir.xy() * c3d_fheightScale ) / p_lightDir.z() / l_numLayers );

						// current parameters
						auto l_currentLayerHeight = p_writer.GetLocale( cuT( "l_currentLayerHeight" ), p_initialHeight - l_layerHeight );
						auto l_currentTextureCoords = p_writer.GetLocale( cuT( "l_currentTextureCoords" ), p_initialTexCoord + l_texStep );
						auto l_heightFromTexture = p_writer.GetLocale( cuT( "l_heightFromTexture" ), texture( c3d_mapHeight, l_currentTextureCoords ).r() );
						auto l_stepIndex = p_writer.GetLocale( cuT( "l_stepIndex" ), 1_i );

						// while point is below depth 0.0 )
						WHILE( p_writer, l_currentLayerHeight > 0.0_f )
						{
							// if point is under the surface
							IF( p_writer, l_heightFromTexture < l_currentLayerHeight )
							{
								// calculate partial shadowing factor
								l_numSamplesUnderSurface += 1;
								auto l_newShadowMultiplier = p_writer.GetLocale( cuT( "l_newShadowMultiplier" )
									, p_writer.Paren( l_currentLayerHeight - l_heightFromTexture )
										* p_writer.Paren( 1.0_f - l_stepIndex / l_numLayers ) );
								l_shadowMultiplier = max( l_shadowMultiplier, l_newShadowMultiplier );
							}
							FI;

							// offset to the next layer
							l_stepIndex += 1;
							l_currentLayerHeight -= l_layerHeight;
							l_currentTextureCoords += l_texStep;
							l_heightFromTexture = texture( c3d_mapHeight, l_currentTextureCoords ).r();
						}
						ELIHW;

						// Shadowing factor should be 1 if there were no points under the surface
						IF( p_writer, l_numSamplesUnderSurface < 1.0_f )
						{
							l_shadowMultiplier = 1.0_f;
						}
						ELSE
						{
							l_shadowMultiplier = 1.0_f - l_shadowMultiplier;
						}
						FI;
					}
					FI;

					p_writer.Return( l_shadowMultiplier );
				}, InParam< Vec3 >{ &p_writer, cuT( "p_lightDir" ) }
				, InParam< Vec2 >{ &p_writer, cuT( "p_initialTexCoord" ) }
				, InParam< Float >{ &p_writer, cuT( "p_initialHeight" ) } );
		}

		return l_result;
	}

	ParallaxFunction DeclareParallaxMappingFunc( GLSL::GlslWriter & p_writer
		, TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags )
	{
		using namespace GLSL;
		ParallaxFunction l_result;

		if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
			&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
		{
			auto c3d_mapHeight( p_writer.GetBuiltin< Sampler2D >( ShaderProgram::MapHeight ) );
			auto c3d_fheightScale( p_writer.GetBuiltin< Float >( cuT( "c3d_fheightScale" ) ) );

			l_result = p_writer.ImplementFunction< Vec2 >( cuT( "ParallaxMapping" ),
				[&]( Vec2 const & p_texCoords, Vec3 const & p_viewDir )
				{
					// number of depth layers
					auto l_minLayers = p_writer.GetLocale( cuT( "l_minLayers" ), 10.0_f );
					auto l_maxLayers = p_writer.GetLocale( cuT( "l_maxLayers" ), 20.0_f );
					auto l_numLayers = p_writer.GetLocale( cuT( "l_numLayers" )
						, mix( l_maxLayers
							, l_minLayers
							, GLSL::abs( dot( vec3( 0.0_f, 0.0, 1.0 ), p_viewDir ) ) ) );
					// calculate the size of each layer
					auto l_layerDepth = p_writer.GetLocale( cuT( "l_layerDepth" ), Float( 1.0f / l_numLayers ) );
					// depth of current layer
					auto l_currentLayerDepth = p_writer.GetLocale( cuT( "l_currentLayerDepth" ), 0.0_f );
					// the amount to shift the texture coordinates per layer (from vector P)
					auto l_p = p_writer.GetLocale( cuT( "l_p" ), p_viewDir.xy() * c3d_fheightScale );
					auto l_deltaTexCoords = p_writer.GetLocale( cuT( "l_deltaTexCoords" ), l_p / l_numLayers );

					auto l_currentTexCoords = p_writer.GetLocale( cuT( "l_currentTexCoords" ), p_texCoords );
					auto l_currentDepthMapValue = p_writer.GetLocale( cuT( "l_currentDepthMapValue" ), texture( c3d_mapHeight, l_currentTexCoords ).r() );

					WHILE( p_writer, l_currentLayerDepth < l_currentDepthMapValue )
					{
						// shift texture coordinates along direction of P
						l_currentTexCoords -= l_deltaTexCoords;
						// get depthmap value at current texture coordinates
						l_currentDepthMapValue = texture( c3d_mapHeight, l_currentTexCoords ).r();
						// get depth of next layer
						l_currentLayerDepth += l_layerDepth;
					}
					ELIHW;

					// get texture coordinates before collision (reverse operations)
					auto l_prevTexCoords = p_writer.GetLocale( cuT( "l_prevTexCoords" ), l_currentTexCoords + l_deltaTexCoords );

					// get depth after and before collision for linear interpolation
					auto l_afterDepth = p_writer.GetLocale( cuT( "l_afterDepth" ), l_currentDepthMapValue - l_currentLayerDepth );
					auto l_beforeDepth = p_writer.GetLocale( cuT( "l_beforeDepth" ), texture( c3d_mapHeight, l_prevTexCoords ).r() - l_currentLayerDepth + l_layerDepth );

					// interpolation of texture coordinates
					auto l_weight = p_writer.GetLocale( cuT( "l_weight" ), l_afterDepth / p_writer.Paren( l_afterDepth - l_beforeDepth ) );
					auto l_finalTexCoords = p_writer.GetLocale( cuT( "l_finalTexCoords" ), l_prevTexCoords * l_weight + l_currentTexCoords * p_writer.Paren( 1.0_f - l_weight ) );

					p_writer.Return( l_finalTexCoords );
				}, InParam< Vec2 >{ &p_writer, cuT( "p_texCoords" ) }
				, InParam< Vec3 >{ &p_writer, cuT( "p_viewDir" ) } );
		}

		return l_result;
	}

	bool IsShadowMapProgram( ProgramFlags const & p_flags )
	{
		return CheckFlag( p_flags, ProgramFlag::eShadowMapDirectional )
			|| CheckFlag( p_flags, ProgramFlag::eShadowMapSpot )
			|| CheckFlag( p_flags, ProgramFlag::eShadowMapPoint );
	}

	GLSL::ShadowType GetShadowType( SceneFlags const & p_flags )
	{
		auto l_shadow = SceneFlag( uint16_t( p_flags ) & uint16_t( SceneFlag::eShadowFilterStratifiedPoisson ) );

		switch ( l_shadow )
		{
		case SceneFlag::eShadowFilterRaw:
			return GLSL::ShadowType::eRaw;

		case SceneFlag::eShadowFilterPoisson:
			return GLSL::ShadowType::ePoisson;

		case SceneFlag::eShadowFilterStratifiedPoisson:
			return GLSL::ShadowType::eStratifiedPoisson;

		default:
			return GLSL::ShadowType::eNone;
		}
	}

	GLSL::FogType GetFogType( SceneFlags const & p_flags )
	{
		auto l_fog = SceneFlag( uint16_t( p_flags ) & uint16_t( SceneFlag::eFogSquaredExponential ) );

		switch ( l_fog )
		{
		case SceneFlag::eFogLinear:
			return GLSL::FogType::eLinear;

		case SceneFlag::eFogExponential:
			return GLSL::FogType::eExponential;

		case SceneFlag::eFogSquaredExponential:
			return GLSL::FogType::eSquaredExponential;

		default:
			return GLSL::FogType::eDisabled;
		}
	}
}
