#include "Castor3DPrerequisites.hpp"

#include "Engine.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslPhongLighting.hpp"
#include "Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Shader/Shaders/GlslSpecularBrdfLighting.hpp"

IMPLEMENT_EXPORTED_OWNED_BY( castor3d::Engine, Engine )
IMPLEMENT_EXPORTED_OWNED_BY( castor3d::RenderSystem, RenderSystem )
IMPLEMENT_EXPORTED_OWNED_BY( castor3d::Scene, Scene )

using namespace castor;
using namespace glsl;

namespace castor3d
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

	String getName( ElementType p_type )
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

	namespace shader
	{
		namespace legacy
		{
			void computePreLightingMapContributions( glsl::GlslWriter & writer
				, glsl::Vec3 & normal
				, glsl::Float & shininess
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags )
			{
				using namespace glsl;
				auto texCoord( writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

				if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
				{
					auto vtx_normal( writer.getBuiltin< Vec3 >( cuT( "vtx_normal" ) ) );
					auto vtx_tangent( writer.getBuiltin< Vec3 >( cuT( "vtx_tangent" ) ) );
					auto vtx_bitangent( writer.getBuiltin< Vec3 >( cuT( "vtx_bitangent" ) ) );
					auto c3d_mapNormal( writer.getBuiltin< Sampler2D >( ShaderProgram::MapNormal ) );

					auto tbn = writer.declLocale( cuT( "tbn" ), mat3( normalize( vtx_tangent ), normalize( vtx_bitangent ), normal ) );
					auto v3MapNormal = writer.declLocale( cuT( "v3MapNormal" ), texture( c3d_mapNormal, texCoord.xy() ).xyz() );
					v3MapNormal = normalize( v3MapNormal * 2.0_f - vec3( 1.0_f, 1.0, 1.0 ) );
					normal = normalize( tbn * v3MapNormal );
				}

				if ( checkFlag( textureFlags, TextureChannel::eGloss ) )
				{
					auto c3d_mapGloss( writer.getBuiltin< Sampler2D >( ShaderProgram::MapGloss ) );
					shininess = texture( c3d_mapGloss, texCoord.xy() ).r();
				}
			}

			void computePostLightingMapContributions( glsl::GlslWriter & writer
				, glsl::Vec3 & diffuse
				, glsl::Vec3 & specular
				, glsl::Vec3 & emissive
				, glsl::Float const & gamma
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags )
			{
				using namespace glsl;
				auto texCoord( writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

				if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
				{
					auto c3d_mapDiffuse( writer.getBuiltin< Sampler2D >( ShaderProgram::MapDiffuse ) );
					diffuse *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
						, gamma
						, texture( c3d_mapDiffuse, texCoord.xy() ).xyz() );
				}

				if ( checkFlag( textureFlags, TextureChannel::eSpecular ) )
				{
					auto c3d_mapSpecular( writer.getBuiltin< Sampler2D >( ShaderProgram::MapSpecular ) );
					specular *= texture( c3d_mapSpecular, texCoord.xy() ).xyz();
				}

				if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
				{
					auto c3d_mapEmissive( writer.getBuiltin< Sampler2D >( ShaderProgram::MapEmissive ) );
					emissive *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
						, gamma
						, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
				}
			}

			std::shared_ptr< PhongLightingModel > createLightingModel( glsl::GlslWriter & writer
				, ShadowType shadows )
			{
				auto result = std::make_shared< PhongLightingModel >( shadows, writer );
				result->declareModel();
				return result;
			}

			std::shared_ptr< PhongLightingModel > createLightingModel( glsl::GlslWriter & writer
				, LightType lightType
				, ShadowType shadows )
			{
				auto result = std::make_shared< PhongLightingModel >( shadows, writer );

				switch ( lightType )
				{
				case LightType::eDirectional:
					result->declareDirectionalModel();
					writer.declUniform< DirectionalLight >( cuT( "light" ) );
					break;

				case LightType::ePoint:
					result->declarePointModel();
					writer.declUniform< PointLight >( cuT( "light" ) );
					break;

				case LightType::eSpot:
					result->declareSpotModel();
					writer.declUniform< SpotLight >( cuT( "light" ) );
					break;

				default:
					FAILURE( "Invalid light type" );
					break;
				}

				return result;
			}
		}

		namespace pbr
		{
			namespace mr
			{
				void computePreLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & p_normal
					, glsl::Float & p_metallic
					, glsl::Float & p_roughness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags )
				{
					using namespace glsl;
					auto texCoord( writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

					if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
					{
						auto vtx_normal( writer.getBuiltin< Vec3 >( cuT( "vtx_normal" ) ) );
						auto vtx_tangent( writer.getBuiltin< Vec3 >( cuT( "vtx_tangent" ) ) );
						auto vtx_bitangent( writer.getBuiltin< Vec3 >( cuT( "vtx_bitangent" ) ) );
						auto c3d_mapNormal( writer.getBuiltin< Sampler2D >( ShaderProgram::MapNormal ) );

						auto tbn = writer.declLocale( cuT( "tbn" ), mat3( normalize( vtx_tangent ), normalize( vtx_bitangent ), p_normal ) );
						auto v3MapNormal = writer.declLocale( cuT( "v3MapNormal" ), texture( c3d_mapNormal, texCoord.xy() ).xyz() );
						v3MapNormal = normalize( v3MapNormal * 2.0_f - vec3( 1.0_f, 1.0, 1.0 ) );
						p_normal = normalize( tbn * v3MapNormal );
					}

					if ( checkFlag( textureFlags, TextureChannel::eMetallic ) )
					{
						auto c3d_mapMetallic( writer.getBuiltin< Sampler2D >( ShaderProgram::MapMetallic ) );
						p_metallic = texture( c3d_mapMetallic, texCoord.xy() ).r();
					}

					if ( checkFlag( textureFlags, TextureChannel::eRoughness ) )
					{
						auto c3d_mapRoughness( writer.getBuiltin< Sampler2D >( ShaderProgram::MapRoughness ) );
						p_roughness = texture( c3d_mapRoughness, texCoord.xy() ).r();
					}
				}

				void computePostLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & p_albedo
					, glsl::Vec3 & p_emissive
					, glsl::Float const & p_gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags )
				{
					using namespace glsl;
					auto texCoord( writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

					if ( checkFlag( textureFlags, TextureChannel::eAlbedo ) )
					{
						auto c3d_mapAlbedo( writer.getBuiltin< Sampler2D >( ShaderProgram::MapAlbedo ) );
						p_albedo *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
							, p_gamma
							, texture( c3d_mapAlbedo, texCoord.xy() ).xyz() );
					}

					if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
					{
						auto c3d_mapEmissive( writer.getBuiltin< Sampler2D >( ShaderProgram::MapEmissive ) );
						p_emissive *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
							, p_gamma
							, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
					}
				}

				std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, ShadowType shadows )
				{
					auto result = std::make_shared< MetallicBrdfLightingModel >( shadows, writer );
					result->declareModel();
					return result;
				}

				std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, LightType lightType
					, ShadowType shadows )
				{
					auto result = std::make_shared< MetallicBrdfLightingModel >( shadows, writer );

					switch ( lightType )
					{
					case LightType::eDirectional:
						result->declareDirectionalModel();
						writer.declUniform< DirectionalLight >( cuT( "light" ) );
						break;

					case LightType::ePoint:
						result->declarePointModel();
						writer.declUniform< PointLight >( cuT( "light" ) );
						break;

					case LightType::eSpot:
						result->declareSpotModel();
						writer.declUniform< SpotLight >( cuT( "light" ) );
						break;

					default:
						FAILURE( "Invalid light type" );
						break;
					}

					return result;
				}
			}
			namespace sg
			{
				void computePreLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & p_normal
					, glsl::Vec3 & p_specular
					, glsl::Float & p_glossiness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags )
				{
					using namespace glsl;
					auto texCoord( writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

					if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
					{
						auto vtx_normal( writer.getBuiltin< Vec3 >( cuT( "vtx_normal" ) ) );
						auto vtx_tangent( writer.getBuiltin< Vec3 >( cuT( "vtx_tangent" ) ) );
						auto vtx_bitangent( writer.getBuiltin< Vec3 >( cuT( "vtx_bitangent" ) ) );
						auto c3d_mapNormal( writer.getBuiltin< Sampler2D >( ShaderProgram::MapNormal ) );

						auto tbn = writer.declLocale( cuT( "tbn" ), mat3( normalize( vtx_tangent ), normalize( vtx_bitangent ), p_normal ) );
						auto v3MapNormal = writer.declLocale( cuT( "v3MapNormal" ), texture( c3d_mapNormal, texCoord.xy() ).xyz() );
						v3MapNormal = normalize( v3MapNormal * 2.0_f - vec3( 1.0_f, 1.0, 1.0 ) );
						p_normal = normalize( tbn * v3MapNormal );
					}

					if ( checkFlag( textureFlags, TextureChannel::eSpecular ) )
					{
						auto c3d_mapSpecular( writer.getBuiltin< Sampler2D >( ShaderProgram::MapSpecular ) );
						p_specular = texture( c3d_mapSpecular, texCoord.xy() ).rgb();
					}

					if ( checkFlag( textureFlags, TextureChannel::eGloss ) )
					{
						auto c3d_mapGloss( writer.getBuiltin< Sampler2D >( ShaderProgram::MapGloss ) );
						p_glossiness = texture( c3d_mapGloss, texCoord.xy() ).r();
					}
				}

				void computePostLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & p_diffuse
					, glsl::Vec3 & p_emissive
					, glsl::Float const & p_gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags )
				{
					using namespace glsl;
					auto texCoord( writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

					if ( checkFlag( textureFlags, TextureChannel::eAlbedo ) )
					{
						auto c3d_mapDiffuse( writer.getBuiltin< Sampler2D >( ShaderProgram::MapDiffuse ) );
						p_diffuse *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
							, p_gamma
							, texture( c3d_mapDiffuse, texCoord.xy() ).xyz() );
					}

					if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
					{
						auto c3d_mapEmissive( writer.getBuiltin< Sampler2D >( ShaderProgram::MapEmissive ) );
						p_emissive *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
							, p_gamma
							, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
					}
				}

				std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, ShadowType shadows )
				{
					auto result = std::make_shared< SpecularBrdfLightingModel >( shadows, writer );
					result->declareModel();
					return result;
				}

				std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, LightType lightType
					, ShadowType shadows )
				{
					auto result = std::make_shared< SpecularBrdfLightingModel >( shadows, writer );

					switch ( lightType )
					{
					case LightType::eDirectional:
						result->declareDirectionalModel();
						writer.declUniform< DirectionalLight >( cuT( "light" ) );
						break;

					case LightType::ePoint:
						result->declarePointModel();
						writer.declUniform< PointLight >( cuT( "light" ) );
						break;

					case LightType::eSpot:
						result->declareSpotModel();
						writer.declUniform< SpotLight >( cuT( "light" ) );
						break;

					default:
						FAILURE( "Invalid light type" );
						break;
					}

					return result;
				}
			}
		}

		ParallaxShadowFunction declareParallaxShadowFunc( glsl::GlslWriter & writer
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags )
		{
			ParallaxShadowFunction result;

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				using namespace glsl;
				auto c3d_mapHeight( writer.getBuiltin< Sampler2D >( ShaderProgram::MapHeight ) );
				auto c3d_heightScale( writer.getBuiltin< Float >( cuT( "c3d_heightScale" ) ) );

				result = writer.implementFunction< Float >( cuT( "ParallaxSoftShadowMultiplier" )
					, [&]( Vec3 const & p_lightDir
					, Vec2 const p_initialTexCoord
					, Float p_initialHeight )
					{
						auto shadowMultiplier = writer.declLocale( cuT( "shadowMultiplier" ), 1.0_f );
						auto minLayers = writer.declLocale( cuT( "minLayers" ), 10.0_f );
						auto maxLayers = writer.declLocale( cuT( "maxLayers" ), 20.0_f );

						// calculate lighting only for surface oriented to the light source
						IF( writer, dot( vec3( 0.0_f, 0, 1 ), p_lightDir ) > 0.0_f )
						{
							// calculate initial parameters
							auto numSamplesUnderSurface = writer.declLocale( cuT( "numSamplesUnderSurface" ), 0.0_f );
							shadowMultiplier = 0;
							auto numLayers = writer.declLocale( cuT( "numLayers" )
								, mix( maxLayers
									, minLayers
									, glsl::abs( dot( vec3( 0.0_f, 0.0, 1.0 ), p_lightDir ) ) ) );
							auto layerHeight = writer.declLocale( cuT( "layerHeight" ), p_initialHeight / numLayers );
							auto texStep = writer.declLocale( cuT( "deltaTexCoords" ), writer.paren( p_lightDir.xy() * c3d_heightScale ) / p_lightDir.z() / numLayers );

							// current parameters
							auto currentLayerHeight = writer.declLocale( cuT( "currentLayerHeight" ), p_initialHeight - layerHeight );
							auto currentTextureCoords = writer.declLocale( cuT( "currentTextureCoords" ), p_initialTexCoord + texStep );
							auto heightFromTexture = writer.declLocale( cuT( "heightFromTexture" ), texture( c3d_mapHeight, currentTextureCoords ).r() );
							auto stepIndex = writer.declLocale( cuT( "stepIndex" ), 1_i );

							// while point is below depth 0.0 )
							WHILE( writer, currentLayerHeight > 0.0_f )
							{
								// if point is under the surface
								IF( writer, heightFromTexture < currentLayerHeight )
								{
									// calculate partial shadowing factor
									numSamplesUnderSurface += 1;
									auto newShadowMultiplier = writer.declLocale( cuT( "newShadowMultiplier" )
										, writer.paren( currentLayerHeight - heightFromTexture )
											* writer.paren( 1.0_f - stepIndex / numLayers ) );
									shadowMultiplier = max( shadowMultiplier, newShadowMultiplier );
								}
								FI;

								// offset to the next layer
								stepIndex += 1;
								currentLayerHeight -= layerHeight;
								currentTextureCoords += texStep;
								heightFromTexture = texture( c3d_mapHeight, currentTextureCoords ).r();
							}
							ELIHW;

							// Shadowing factor should be 1 if there were no points under the surface
							IF( writer, numSamplesUnderSurface < 1.0_f )
							{
								shadowMultiplier = 1.0_f;
							}
							ELSE
							{
								shadowMultiplier = 1.0_f - shadowMultiplier;
							}
							FI;
						}
						FI;

						writer.returnStmt( shadowMultiplier );
					}, InVec3{ &writer, cuT( "p_lightDir" ) }
					, InVec2{ &writer, cuT( "p_initialTexCoord" ) }
					, InFloat{ &writer, cuT( "p_initialHeight" ) } );
			}

			return result;
		}

		ParallaxFunction declareParallaxMappingFunc( glsl::GlslWriter & writer
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags )
		{
			using namespace glsl;
			ParallaxFunction result;

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto c3d_mapHeight( writer.getBuiltin< Sampler2D >( ShaderProgram::MapHeight ) );
				auto c3d_heightScale( writer.getBuiltin< Float >( cuT( "c3d_heightScale" ) ) );

				result = writer.implementFunction< Vec2 >( cuT( "ParallaxMapping" ),
					[&]( Vec2 const & p_texCoords, Vec3 const & p_viewDir )
					{
						// number of depth layers
						auto minLayers = writer.declLocale( cuT( "minLayers" ), 10.0_f );
						auto maxLayers = writer.declLocale( cuT( "maxLayers" ), 20.0_f );
						auto numLayers = writer.declLocale( cuT( "numLayers" )
							, mix( maxLayers
								, minLayers
								, glsl::abs( dot( vec3( 0.0_f, 0.0, 1.0 ), p_viewDir ) ) ) );
						// calculate the size of each layer
						auto layerDepth = writer.declLocale( cuT( "layerDepth" ), Float( 1.0f / numLayers ) );
						// depth of current layer
						auto currentLayerDepth = writer.declLocale( cuT( "currentLayerDepth" ), 0.0_f );
						// the amount to shift the texture coordinates per layer (from vector P)
						auto p = writer.declLocale( cuT( "p" ), p_viewDir.xy() * c3d_heightScale );
						auto deltaTexCoords = writer.declLocale( cuT( "deltaTexCoords" ), p / numLayers );

						auto currentTexCoords = writer.declLocale( cuT( "currentTexCoords" ), p_texCoords );
						auto currentDepthMapValue = writer.declLocale( cuT( "currentDepthMapValue" ), texture( c3d_mapHeight, currentTexCoords ).r() );

						WHILE( writer, currentLayerDepth < currentDepthMapValue )
						{
							// shift texture coordinates along direction of P
							currentTexCoords -= deltaTexCoords;
							// get depthmap value at current texture coordinates
							currentDepthMapValue = texture( c3d_mapHeight, currentTexCoords ).r();
							// get depth of next layer
							currentLayerDepth += layerDepth;
						}
						ELIHW;

						// get texture coordinates before collision (reverse operations)
						auto prevTexCoords = writer.declLocale( cuT( "prevTexCoords" ), currentTexCoords + deltaTexCoords );

						// get depth after and before collision for linear interpolation
						auto afterDepth = writer.declLocale( cuT( "afterDepth" ), currentDepthMapValue - currentLayerDepth );
						auto beforeDepth = writer.declLocale( cuT( "beforeDepth" ), texture( c3d_mapHeight, prevTexCoords ).r() - currentLayerDepth + layerDepth );

						// interpolation of texture coordinates
						auto weight = writer.declLocale( cuT( "weight" ), afterDepth / writer.paren( afterDepth - beforeDepth ) );
						auto finalTexCoords = writer.declLocale( cuT( "finalTexCoords" ), prevTexCoords * weight + currentTexCoords * writer.paren( 1.0_f - weight ) );

						writer.returnStmt( finalTexCoords );
					}, InVec2{ &writer, cuT( "p_texCoords" ) }
					, InVec3{ &writer, cuT( "p_viewDir" ) } );
			}

			return result;
		}
	}

	bool isShadowMapProgram( ProgramFlags const & p_flags )
	{
		return checkFlag( p_flags, ProgramFlag::eShadowMapDirectional )
			|| checkFlag( p_flags, ProgramFlag::eShadowMapSpot )
			|| checkFlag( p_flags, ProgramFlag::eShadowMapPoint );
	}

	ShadowType getShadowType( SceneFlags const & p_flags )
	{
		auto shadow = SceneFlag( uint16_t( p_flags ) & uint16_t( SceneFlag::eShadowFilterStratifiedPoisson ) );

		switch ( shadow )
		{
		case SceneFlag::eShadowFilterRaw:
			return ShadowType::eRaw;

		case SceneFlag::eShadowFilterPoisson:
			return ShadowType::ePoisson;

		case SceneFlag::eShadowFilterStratifiedPoisson:
			return ShadowType::eStratifiedPoisson;

		default:
			return ShadowType::eNone;
		}
	}

	FogType getFogType( SceneFlags const & p_flags )
	{
		auto fog = SceneFlag( uint16_t( p_flags ) & uint16_t( SceneFlag::eFogSquaredExponential ) );

		switch ( fog )
		{
		case SceneFlag::eFogLinear:
			return FogType::eLinear;

		case SceneFlag::eFogExponential:
			return FogType::eExponential;

		case SceneFlag::eFogSquaredExponential:
			return FogType::eSquaredExponential;

		default:
			return FogType::eDisabled;
		}
	}
}
