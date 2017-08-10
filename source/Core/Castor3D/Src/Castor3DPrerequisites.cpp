#include "Castor3DPrerequisites.hpp"

#include "Engine.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>
#include <GlslMaterial.hpp>
#include <GlslPhongLighting.hpp>
#include <GlslMetallicBrdfLighting.hpp>
#include <GlslSpecularBrdfLighting.hpp>

IMPLEMENT_EXPORTED_OWNED_BY( castor3d::Engine, Engine )
IMPLEMENT_EXPORTED_OWNED_BY( castor3d::RenderSystem, RenderSystem )
IMPLEMENT_EXPORTED_OWNED_BY( castor3d::Scene, Scene )

using namespace castor;

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

	namespace legacy
	{
		void computePreLightingMapContributions( GLSL::GlslWriter & p_writer
			, GLSL::Vec3 & p_normal
			, GLSL::Float & p_shininess
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )
		{
			using namespace GLSL;
			auto texCoord( p_writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

			if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto vtx_normal( p_writer.getBuiltin< Vec3 >( cuT( "vtx_normal" ) ) );
				auto vtx_tangent( p_writer.getBuiltin< Vec3 >( cuT( "vtx_tangent" ) ) );
				auto vtx_bitangent( p_writer.getBuiltin< Vec3 >( cuT( "vtx_bitangent" ) ) );
				auto c3d_mapNormal( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapNormal ) );

				auto tbn = p_writer.declLocale( cuT( "tbn" ), mat3( normalize( vtx_tangent ), normalize( vtx_bitangent ), p_normal ) );
				auto v3MapNormal = p_writer.declLocale( cuT( "v3MapNormal" ), texture( c3d_mapNormal, texCoord.xy() ).xyz() );
				v3MapNormal = normalize( v3MapNormal * 2.0_f - vec3( 1.0_f, 1.0, 1.0 ) );
				p_normal = normalize( tbn * v3MapNormal );
			}

			if ( checkFlag( textureFlags, TextureChannel::eGloss ) )
			{
				auto c3d_mapGloss( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapGloss ) );

				p_shininess = texture( c3d_mapGloss, texCoord.xy() ).r();
			}
		}

		void computePostLightingMapContributions( GLSL::GlslWriter & p_writer
			, GLSL::Vec3 & p_diffuse
			, GLSL::Vec3 & p_specular
			, GLSL::Vec3 & p_emissive
			, GLSL::Float const & p_gamma
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )
		{
			using namespace GLSL;
			auto texCoord( p_writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

			if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
			{
				auto c3d_mapDiffuse( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapDiffuse ) );
				p_diffuse *= writeFunctionCall< Vec3 >( &p_writer, cuT( "removeGamma" )
					, p_gamma
					, texture( c3d_mapDiffuse, texCoord.xy() ).xyz() );
			}

			if ( checkFlag( textureFlags, TextureChannel::eSpecular ) )
			{
				auto c3d_mapSpecular( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapSpecular ) );

				p_specular *= texture( c3d_mapSpecular, texCoord.xy() ).xyz();
			}

			if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
			{
				auto c3d_mapEmissive( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapEmissive ) );
				p_emissive *= writeFunctionCall< Vec3 >( &p_writer, cuT( "removeGamma" )
					, p_gamma
					, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
			}
		}

		std::shared_ptr< GLSL::PhongLightingModel > createLightingModel( GLSL::GlslWriter & p_writer
			, GLSL::ShadowType p_shadows )
		{
			return std::static_pointer_cast< GLSL::PhongLightingModel >( p_writer.createLightingModel( GLSL::PhongLightingModel::Name
				, p_shadows ) );
		}

		std::shared_ptr< GLSL::PhongLightingModel > createLightingModel( GLSL::GlslWriter & p_writer
			, LightType p_light
			, GLSL::ShadowType p_shadows )
		{
			std::shared_ptr< GLSL::LightingModel > result;

			switch ( p_light )
			{
			case LightType::eDirectional:
			{
				result = p_writer.createDirectionalLightingModel( GLSL::PhongLightingModel::Name
					, p_shadows );
				auto light = p_writer.declUniform< GLSL::DirectionalLight >( cuT( "light" ) );
			}
			break;

			case LightType::ePoint:
			{
				result = p_writer.createPointLightingModel( GLSL::PhongLightingModel::Name
					, p_shadows );
				auto light = p_writer.declUniform< GLSL::PointLight >( cuT( "light" ) );
			}
			break;

			case LightType::eSpot:
			{
				result = p_writer.createSpotLightingModel( GLSL::PhongLightingModel::Name
					, p_shadows );
				auto light = p_writer.declUniform< GLSL::SpotLight >( cuT( "light" ) );
			}
			break;

			default:
				FAILURE( "Invalid light type" );
				break;
			}

			return std::static_pointer_cast< GLSL::PhongLightingModel >( result );
		}
	}

	namespace pbr
	{
		namespace mr
		{
			void computePreLightingMapContributions( GLSL::GlslWriter & p_writer
				, GLSL::Vec3 & p_normal
				, GLSL::Float & p_metallic
				, GLSL::Float & p_roughness
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags )
			{
				using namespace GLSL;
				auto texCoord( p_writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

				if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
				{
					auto vtx_normal( p_writer.getBuiltin< Vec3 >( cuT( "vtx_normal" ) ) );
					auto vtx_tangent( p_writer.getBuiltin< Vec3 >( cuT( "vtx_tangent" ) ) );
					auto vtx_bitangent( p_writer.getBuiltin< Vec3 >( cuT( "vtx_bitangent" ) ) );
					auto c3d_mapNormal( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapNormal ) );

					auto tbn = p_writer.declLocale( cuT( "tbn" ), mat3( normalize( vtx_tangent ), normalize( vtx_bitangent ), p_normal ) );
					auto v3MapNormal = p_writer.declLocale( cuT( "v3MapNormal" ), texture( c3d_mapNormal, texCoord.xy() ).xyz() );
					v3MapNormal = normalize( v3MapNormal * 2.0_f - vec3( 1.0_f, 1.0, 1.0 ) );
					p_normal = normalize( tbn * v3MapNormal );
				}

				if ( checkFlag( textureFlags, TextureChannel::eMetallic ) )
				{
					auto c3d_mapMetallic( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapMetallic ) );
					p_metallic = texture( c3d_mapMetallic, texCoord.xy() ).r();
				}

				if ( checkFlag( textureFlags, TextureChannel::eRoughness ) )
				{
					auto c3d_mapRoughness( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapRoughness ) );
					p_roughness = texture( c3d_mapRoughness, texCoord.xy() ).r();
				}
			}

			void computePostLightingMapContributions( GLSL::GlslWriter & p_writer
				, GLSL::Vec3 & p_albedo
				, GLSL::Vec3 & p_emissive
				, GLSL::Float const & p_gamma
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags )
			{
				using namespace GLSL;
				auto texCoord( p_writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

				if ( checkFlag( textureFlags, TextureChannel::eAlbedo ) )
				{
					auto c3d_mapAlbedo( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapAlbedo ) );
					p_albedo *= writeFunctionCall< Vec3 >( &p_writer, cuT( "removeGamma" )
						, p_gamma
						, texture( c3d_mapAlbedo, texCoord.xy() ).xyz() );
				}

				if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
				{
					auto c3d_mapEmissive( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapEmissive ) );
					p_emissive *= writeFunctionCall< Vec3 >( &p_writer, cuT( "removeGamma" )
						, p_gamma
						, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
				}
			}

			std::shared_ptr< GLSL::MetallicBrdfLightingModel > createLightingModel( GLSL::GlslWriter & p_writer
				, GLSL::ShadowType p_shadows )
			{
				return std::static_pointer_cast< GLSL::MetallicBrdfLightingModel >( p_writer.createLightingModel( GLSL::MetallicBrdfLightingModel::Name
					, p_shadows ) );
			}

			std::shared_ptr< GLSL::MetallicBrdfLightingModel > createLightingModel( GLSL::GlslWriter & p_writer
				, LightType p_light
				, GLSL::ShadowType p_shadows )
			{
				std::shared_ptr< GLSL::LightingModel > result;

				switch ( p_light )
				{
				case LightType::eDirectional:
				{
					result = p_writer.createDirectionalLightingModel( GLSL::MetallicBrdfLightingModel::Name
						, p_shadows );
					auto light = p_writer.declUniform< GLSL::DirectionalLight >( cuT( "light" ) );
				}
				break;

				case LightType::ePoint:
				{
					result = p_writer.createPointLightingModel( GLSL::MetallicBrdfLightingModel::Name
						, p_shadows );
					auto light = p_writer.declUniform< GLSL::PointLight >( cuT( "light" ) );
				}
				break;

				case LightType::eSpot:
				{
					result = p_writer.createSpotLightingModel( GLSL::MetallicBrdfLightingModel::Name
						, p_shadows );
					auto light = p_writer.declUniform< GLSL::SpotLight >( cuT( "light" ) );
				}
				break;

				default:
					FAILURE( "Invalid light type" );
					break;
				}

				return std::static_pointer_cast< GLSL::MetallicBrdfLightingModel >( result );
			}
		}
		namespace sg
		{
			void computePreLightingMapContributions( GLSL::GlslWriter & p_writer
				, GLSL::Vec3 & p_normal
				, GLSL::Vec3 & p_specular
				, GLSL::Float & p_glossiness
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags )
			{
				using namespace GLSL;
				auto texCoord( p_writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

				if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
				{
					auto vtx_normal( p_writer.getBuiltin< Vec3 >( cuT( "vtx_normal" ) ) );
					auto vtx_tangent( p_writer.getBuiltin< Vec3 >( cuT( "vtx_tangent" ) ) );
					auto vtx_bitangent( p_writer.getBuiltin< Vec3 >( cuT( "vtx_bitangent" ) ) );
					auto c3d_mapNormal( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapNormal ) );

					auto tbn = p_writer.declLocale( cuT( "tbn" ), mat3( normalize( vtx_tangent ), normalize( vtx_bitangent ), p_normal ) );
					auto v3MapNormal = p_writer.declLocale( cuT( "v3MapNormal" ), texture( c3d_mapNormal, texCoord.xy() ).xyz() );
					v3MapNormal = normalize( v3MapNormal * 2.0_f - vec3( 1.0_f, 1.0, 1.0 ) );
					p_normal = normalize( tbn * v3MapNormal );
				}

				if ( checkFlag( textureFlags, TextureChannel::eSpecular ) )
				{
					auto c3d_mapSpecular( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapSpecular ) );
					p_specular = texture( c3d_mapSpecular, texCoord.xy() ).rgb();
				}

				if ( checkFlag( textureFlags, TextureChannel::eGloss ) )
				{
					auto c3d_mapGloss( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapGloss ) );
					p_glossiness = texture( c3d_mapGloss, texCoord.xy() ).r();
				}
			}

			void computePostLightingMapContributions( GLSL::GlslWriter & p_writer
				, GLSL::Vec3 & p_diffuse
				, GLSL::Vec3 & p_emissive
				, GLSL::Float const & p_gamma
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags )
			{
				using namespace GLSL;
				auto texCoord( p_writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

				if ( checkFlag( textureFlags, TextureChannel::eAlbedo ) )
				{
					auto c3d_mapDiffuse( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapDiffuse ) );
					p_diffuse *= writeFunctionCall< Vec3 >( &p_writer, cuT( "removeGamma" )
						, p_gamma
						, texture( c3d_mapDiffuse, texCoord.xy() ).xyz() );
				}

				if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
				{
					auto c3d_mapEmissive( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapEmissive ) );
					p_emissive *= writeFunctionCall< Vec3 >( &p_writer, cuT( "removeGamma" )
						, p_gamma
						, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
				}
			}

			std::shared_ptr< GLSL::SpecularBrdfLightingModel > createLightingModel( GLSL::GlslWriter & p_writer
				, GLSL::ShadowType p_shadows )
			{
				return std::static_pointer_cast< GLSL::SpecularBrdfLightingModel >( p_writer.createLightingModel( GLSL::SpecularBrdfLightingModel::Name
					, p_shadows ) );
			}

			std::shared_ptr< GLSL::SpecularBrdfLightingModel > createLightingModel( GLSL::GlslWriter & p_writer
				, LightType p_light
				, GLSL::ShadowType p_shadows )
			{
				std::shared_ptr< GLSL::LightingModel > result;

				switch ( p_light )
				{
				case LightType::eDirectional:
				{
					result = p_writer.createDirectionalLightingModel( GLSL::SpecularBrdfLightingModel::Name
						, p_shadows );
					auto light = p_writer.declUniform< GLSL::DirectionalLight >( cuT( "light" ) );
				}
				break;

				case LightType::ePoint:
				{
					result = p_writer.createPointLightingModel( GLSL::SpecularBrdfLightingModel::Name
						, p_shadows );
					auto light = p_writer.declUniform< GLSL::PointLight >( cuT( "light" ) );
				}
				break;

				case LightType::eSpot:
				{
					result = p_writer.createSpotLightingModel( GLSL::SpecularBrdfLightingModel::Name
						, p_shadows );
					auto light = p_writer.declUniform< GLSL::SpotLight >( cuT( "light" ) );
				}
				break;

				default:
					FAILURE( "Invalid light type" );
					break;
				}

				return std::static_pointer_cast< GLSL::SpecularBrdfLightingModel >( result );
			}
		}
	}

	ParallaxShadowFunction declareParallaxShadowFunc( GLSL::GlslWriter & p_writer
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags )
	{
		ParallaxShadowFunction result;

		if ( checkFlag( textureFlags, TextureChannel::eHeight )
			&& checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			using namespace GLSL;
			auto c3d_mapHeight( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapHeight ) );
			auto c3d_heightScale( p_writer.getBuiltin< Float >( cuT( "c3d_heightScale" ) ) );

			result = p_writer.implementFunction< Float >( cuT( "ParallaxSoftShadowMultiplier" )
				, [&]( Vec3 const & p_lightDir
				, Vec2 const p_initialTexCoord
				, Float p_initialHeight )
				{
					auto shadowMultiplier = p_writer.declLocale( cuT( "shadowMultiplier" ), 1.0_f );
					auto minLayers = p_writer.declLocale( cuT( "minLayers" ), 10.0_f );
					auto maxLayers = p_writer.declLocale( cuT( "maxLayers" ), 20.0_f );

					// calculate lighting only for surface oriented to the light source
					IF( p_writer, dot( vec3( 0.0_f, 0, 1 ), p_lightDir ) > 0.0_f )
					{
						// calculate initial parameters
						auto numSamplesUnderSurface = p_writer.declLocale( cuT( "numSamplesUnderSurface" ), 0.0_f );
						shadowMultiplier = 0;
						auto numLayers = p_writer.declLocale( cuT( "numLayers" )
							, mix( maxLayers
								, minLayers
								, GLSL::abs( dot( vec3( 0.0_f, 0.0, 1.0 ), p_lightDir ) ) ) );
						auto layerHeight = p_writer.declLocale( cuT( "layerHeight" ), p_initialHeight / numLayers );
						auto texStep = p_writer.declLocale( cuT( "deltaTexCoords" ), p_writer.paren( p_lightDir.xy() * c3d_heightScale ) / p_lightDir.z() / numLayers );

						// current parameters
						auto currentLayerHeight = p_writer.declLocale( cuT( "currentLayerHeight" ), p_initialHeight - layerHeight );
						auto currentTextureCoords = p_writer.declLocale( cuT( "currentTextureCoords" ), p_initialTexCoord + texStep );
						auto heightFromTexture = p_writer.declLocale( cuT( "heightFromTexture" ), texture( c3d_mapHeight, currentTextureCoords ).r() );
						auto stepIndex = p_writer.declLocale( cuT( "stepIndex" ), 1_i );

						// while point is below depth 0.0 )
						WHILE( p_writer, currentLayerHeight > 0.0_f )
						{
							// if point is under the surface
							IF( p_writer, heightFromTexture < currentLayerHeight )
							{
								// calculate partial shadowing factor
								numSamplesUnderSurface += 1;
								auto newShadowMultiplier = p_writer.declLocale( cuT( "newShadowMultiplier" )
									, p_writer.paren( currentLayerHeight - heightFromTexture )
										* p_writer.paren( 1.0_f - stepIndex / numLayers ) );
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
						IF( p_writer, numSamplesUnderSurface < 1.0_f )
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

					p_writer.returnStmt( shadowMultiplier );
				}, InVec3{ &p_writer, cuT( "p_lightDir" ) }
				, InVec2{ &p_writer, cuT( "p_initialTexCoord" ) }
				, InFloat{ &p_writer, cuT( "p_initialHeight" ) } );
		}

		return result;
	}

	ParallaxFunction declareParallaxMappingFunc( GLSL::GlslWriter & p_writer
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags )
	{
		using namespace GLSL;
		ParallaxFunction result;

		if ( checkFlag( textureFlags, TextureChannel::eHeight )
			&& checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			auto c3d_mapHeight( p_writer.getBuiltin< Sampler2D >( ShaderProgram::MapHeight ) );
			auto c3d_heightScale( p_writer.getBuiltin< Float >( cuT( "c3d_heightScale" ) ) );

			result = p_writer.implementFunction< Vec2 >( cuT( "ParallaxMapping" ),
				[&]( Vec2 const & p_texCoords, Vec3 const & p_viewDir )
				{
					// number of depth layers
					auto minLayers = p_writer.declLocale( cuT( "minLayers" ), 10.0_f );
					auto maxLayers = p_writer.declLocale( cuT( "maxLayers" ), 20.0_f );
					auto numLayers = p_writer.declLocale( cuT( "numLayers" )
						, mix( maxLayers
							, minLayers
							, GLSL::abs( dot( vec3( 0.0_f, 0.0, 1.0 ), p_viewDir ) ) ) );
					// calculate the size of each layer
					auto layerDepth = p_writer.declLocale( cuT( "layerDepth" ), Float( 1.0f / numLayers ) );
					// depth of current layer
					auto currentLayerDepth = p_writer.declLocale( cuT( "currentLayerDepth" ), 0.0_f );
					// the amount to shift the texture coordinates per layer (from vector P)
					auto p = p_writer.declLocale( cuT( "p" ), p_viewDir.xy() * c3d_heightScale );
					auto deltaTexCoords = p_writer.declLocale( cuT( "deltaTexCoords" ), p / numLayers );

					auto currentTexCoords = p_writer.declLocale( cuT( "currentTexCoords" ), p_texCoords );
					auto currentDepthMapValue = p_writer.declLocale( cuT( "currentDepthMapValue" ), texture( c3d_mapHeight, currentTexCoords ).r() );

					WHILE( p_writer, currentLayerDepth < currentDepthMapValue )
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
					auto prevTexCoords = p_writer.declLocale( cuT( "prevTexCoords" ), currentTexCoords + deltaTexCoords );

					// get depth after and before collision for linear interpolation
					auto afterDepth = p_writer.declLocale( cuT( "afterDepth" ), currentDepthMapValue - currentLayerDepth );
					auto beforeDepth = p_writer.declLocale( cuT( "beforeDepth" ), texture( c3d_mapHeight, prevTexCoords ).r() - currentLayerDepth + layerDepth );

					// interpolation of texture coordinates
					auto weight = p_writer.declLocale( cuT( "weight" ), afterDepth / p_writer.paren( afterDepth - beforeDepth ) );
					auto finalTexCoords = p_writer.declLocale( cuT( "finalTexCoords" ), prevTexCoords * weight + currentTexCoords * p_writer.paren( 1.0_f - weight ) );

					p_writer.returnStmt( finalTexCoords );
				}, InVec2{ &p_writer, cuT( "p_texCoords" ) }
				, InVec3{ &p_writer, cuT( "p_viewDir" ) } );
		}

		return result;
	}

	bool isShadowMapProgram( ProgramFlags const & p_flags )
	{
		return checkFlag( p_flags, ProgramFlag::eShadowMapDirectional )
			|| checkFlag( p_flags, ProgramFlag::eShadowMapSpot )
			|| checkFlag( p_flags, ProgramFlag::eShadowMapPoint );
	}

	GLSL::ShadowType getShadowType( SceneFlags const & p_flags )
	{
		auto shadow = SceneFlag( uint16_t( p_flags ) & uint16_t( SceneFlag::eShadowFilterStratifiedPoisson ) );

		switch ( shadow )
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

	GLSL::FogType getFogType( SceneFlags const & p_flags )
	{
		auto fog = SceneFlag( uint16_t( p_flags ) & uint16_t( SceneFlag::eFogSquaredExponential ) );

		switch ( fog )
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
