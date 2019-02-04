#include "Castor3DPrerequisites.hpp"

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslPhongLighting.hpp"
#include "Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	//*************************************************************************

	ShaderModule::ShaderModule( ShaderModule && rhs )
		: stage{ rhs.stage }
		, name{ std::move( rhs.name ) }
		, source{ std::move( rhs.source ) }
		, shader{ std::move( rhs.shader ) }
	{
	}

	ShaderModule & ShaderModule::operator=( ShaderModule && rhs )
	{
		stage = rhs.stage;
		name = std::move( rhs.name );
		source = std::move( rhs.source );
		shader = std::move( rhs.shader );

		return *this;
	}

	ShaderModule::ShaderModule( ashes::ShaderStageFlag stage
		, std::string const & name )
		: stage{ stage }
		, name{ name }
	{
	}

	ShaderModule::ShaderModule( ashes::ShaderStageFlag stage
		, std::string const & name
		, std::string source )
		: stage{ stage }
		, name{ name }
		, source{ source }
	{
	}

	ShaderModule::ShaderModule( ashes::ShaderStageFlag stage
		, std::string const & name
		, ShaderPtr shader )
		: stage{ stage }
		, name{ name }
		, shader{ std::move( shader ) }
	{
	}

	namespace shader
	{
		//*********************************************************************

		namespace legacy
		{
			void computePreLightingMapContributions( sdw::ShaderWriter & writer
				, shader::Utils const & utils
				, sdw::Vec3 & normal
				, sdw::Float & shininess
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags
				, PassFlags const & passFlags )
			{
				using namespace sdw;
				auto texCoord( writer.getVariable< Vec3 >( "texCoord" ) );

				if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
				{
					auto vtx_normal( writer.getVariable< Vec3 >( "vtx_normal" ) );
					auto vtx_tangent( writer.getVariable< Vec3 >( "vtx_tangent" ) );
					auto vtx_bitangent( writer.getVariable< Vec3 >( "vtx_bitangent" ) );
					auto c3d_mapNormal( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapNormal" ) );

					auto v3MapNormal = writer.declLocale( "v3MapNormal"
						, utils.invertNormal( texture( c3d_mapNormal, texCoord.xy() ).xyz() ) );
					v3MapNormal = normalize( sdw::fma( v3MapNormal
						, vec3( 2.0_f )
						, vec3( -1.0_f ) ) );

					if ( checkFlag( textureFlags, TextureChannel::eHeight )
						&& !checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
					{
						auto c3d_mapHeight( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapHeight" ) );
						auto mapHeight = writer.declLocale( "mapHeight"
							, texture( c3d_mapHeight, texCoord.xy() ).r() );
						v3MapNormal = mix( vec3( 0.0_f, 0.0_f, 1.0_f )
							, v3MapNormal
							, vec3( mapHeight ) );
					}

					auto tbn = writer.declLocale( "tbn"
						, mat3( normalize( vtx_tangent )
							, normalize( vtx_bitangent )
							, normal ) );
					normal = normalize( tbn * v3MapNormal );
				}

				if ( checkFlag( textureFlags, TextureChannel::eGloss ) )
				{
					auto c3d_mapGloss( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapGloss" ) );
					shininess = texture( c3d_mapGloss, texCoord.xy() ).r() * 255.0_f;
				}
			}

			void computePostLightingMapContributions( sdw::ShaderWriter & writer
				, shader::Utils const & utils
				, sdw::Vec3 & diffuse
				, sdw::Vec3 & specular
				, sdw::Vec3 & emissive
				, sdw::Float const & gamma
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags )
			{
				using namespace sdw;
				auto texCoord( writer.getVariable< Vec3 >( "texCoord" ) );

				if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
				{
					auto c3d_mapDiffuse( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapDiffuse" ) );
					diffuse *= utils.removeGamma( gamma
						, texture( c3d_mapDiffuse, texCoord.xy() ).xyz() );
				}

				if ( checkFlag( textureFlags, TextureChannel::eSpecular ) )
				{
					auto c3d_mapSpecular( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapSpecular" ) );
					specular *= texture( c3d_mapSpecular, texCoord.xy() ).xyz();
				}

				emissive *= diffuse;

				if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
				{
					auto c3d_mapEmissive( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapEmissive" ) );
					emissive *= utils.removeGamma( gamma
						, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
				}
			}

			std::shared_ptr< PhongLightingModel > createLightingModel( sdw::ShaderWriter & writer
				, uint32_t & index
				, uint32_t maxCascades )
			{
				auto result = std::make_shared< PhongLightingModel >( writer );
				result->declareModel( index, maxCascades );
				return result;
			}

			std::shared_ptr< PhongLightingModel > createLightingModel( sdw::ShaderWriter & writer
				, ShadowType shadows
				, bool volumetric
				, uint32_t & index
				, uint32_t maxCascades )
			{
				auto result = std::make_shared< PhongLightingModel >( writer );
				result->declareDirectionalModel( shadows
					, volumetric
					, index
					, maxCascades );
				return result;
			}

			std::shared_ptr< PhongLightingModel > createLightingModel( sdw::ShaderWriter & writer
				, LightType lightType
				, ShadowType shadows
				, bool volumetric
				, uint32_t & index )
			{
				auto result = std::make_shared< PhongLightingModel >( writer );

				switch ( lightType )
				{
				case LightType::eDirectional:
					CU_Failure( "Directional light model should use the other overload" );
					break;

				case LightType::ePoint:
					result->declarePointModel( shadows, volumetric, index );
					break;

				case LightType::eSpot:
					result->declareSpotModel( shadows, volumetric, index );
					break;

				default:
					CU_Failure( "Invalid light type" );
					break;
				}

				return result;
			}
		}

		namespace pbr
		{
			//*****************************************************************

			namespace mr
			{
				void computePreLightingMapContributions( sdw::ShaderWriter & writer
					, shader::Utils const & utils
					, sdw::Vec3 & normal
					, sdw::Float & metallic
					, sdw::Float & roughness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags
					, PassFlags const & passFlags )
				{
					using namespace sdw;
					auto texCoord( writer.getVariable< Vec3 >( "texCoord" ) );

					if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
					{
						auto vtx_normal( writer.getVariable< Vec3 >( "vtx_normal" ) );
						auto vtx_tangent( writer.getVariable< Vec3 >( "vtx_tangent" ) );
						auto vtx_bitangent( writer.getVariable< Vec3 >( "vtx_bitangent" ) );
						auto c3d_mapNormal( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapNormal" ) );

						auto v3MapNormal = writer.declLocale( "v3MapNormal"
							, utils.invertNormal( texture( c3d_mapNormal, texCoord.xy() ).xyz() ) );
						v3MapNormal = normalize( sdw::fma( v3MapNormal
							, vec3( 2.0_f )
							, vec3( -1.0_f ) ) );

						if ( checkFlag( textureFlags, TextureChannel::eHeight )
							&& !checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
						{
							auto c3d_mapHeight( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapHeight" ) );
							auto mapHeight = writer.declLocale( "mapHeight"
								, texture( c3d_mapHeight, texCoord.xy() ).r() );
							v3MapNormal = mix( vec3( 0.0_f, 0.0_f, 1.0_f )
								, v3MapNormal
								, vec3( mapHeight ) );
						}

						auto tbn = writer.declLocale( "tbn"
							, mat3( normalize( vtx_tangent )
								, normalize( vtx_bitangent )
								, normal ) );
						normal = normalize( tbn * v3MapNormal );
					}

					if ( checkFlag( textureFlags, TextureChannel::eMetallic ) )
					{
						auto c3d_mapMetallic( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapMetallic" ) );
						metallic = texture( c3d_mapMetallic, texCoord.xy() ).r();
					}

					if ( checkFlag( textureFlags, TextureChannel::eRoughness ) )
					{
						auto c3d_mapRoughness( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapRoughness" ) );
						roughness = texture( c3d_mapRoughness, texCoord.xy() ).r();
					}
				}

				void computePostLightingMapContributions( sdw::ShaderWriter & writer
					, shader::Utils const & utils
					, sdw::Vec3 & albedo
					, sdw::Vec3 & emissive
					, sdw::Float const & gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags )
				{
					using namespace sdw;
					auto texCoord( writer.getVariable< Vec3 >( "texCoord" ) );

					if ( checkFlag( textureFlags, TextureChannel::eAlbedo ) )
					{
						auto c3d_mapAlbedo( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapAlbedo" ) );
						albedo *= utils.removeGamma( gamma
							, texture( c3d_mapAlbedo, texCoord.xy() ).xyz() );
					}

					emissive *= albedo;

					if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
					{
						auto c3d_mapEmissive( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapEmissive" ) );
						emissive *= utils.removeGamma( gamma
							, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
					}
				}

				std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
					, uint32_t & index
					, uint32_t maxCascades )
				{
					auto result = std::make_shared< MetallicBrdfLightingModel >( writer );
					result->declareModel( index, maxCascades );
					return result;
				}

				std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
					, ShadowType shadows
					, bool volumetric
					, uint32_t & index
					, uint32_t maxCascades )
				{
					auto result = std::make_shared< MetallicBrdfLightingModel >( writer );
					result->declareDirectionalModel( shadows
						, volumetric
						, index
						, maxCascades );
					return result;
				}

				std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
					, LightType lightType
					, ShadowType shadows
					, bool volumetric
					, uint32_t & index )
				{
					auto result = std::make_shared< MetallicBrdfLightingModel >( writer );

					switch ( lightType )
					{
					case LightType::eDirectional:
						CU_Failure( "Directional light model should use the other overload" );
						break;

					case LightType::ePoint:
						result->declarePointModel( shadows, volumetric, index );
						break;

					case LightType::eSpot:
						result->declareSpotModel( shadows, volumetric, index );
						break;

					default:
						CU_Failure( "Invalid light type" );
						break;
					}

					return result;
				}
			}

			//*****************************************************************

			namespace sg
			{
				void computePreLightingMapContributions( sdw::ShaderWriter & writer
					, shader::Utils const & utils
					, sdw::Vec3 & normal
					, sdw::Vec3 & specular
					, sdw::Float & glossiness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags
					, PassFlags const & passFlags )
				{
					using namespace sdw;
					auto texCoord( writer.getVariable< Vec3 >( "texCoord" ) );

					if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
					{
						auto vtx_normal( writer.getVariable< Vec3 >( "vtx_normal" ) );
						auto vtx_tangent( writer.getVariable< Vec3 >( "vtx_tangent" ) );
						auto vtx_bitangent( writer.getVariable< Vec3 >( "vtx_bitangent" ) );
						auto c3d_mapNormal( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapNormal" ) );

						auto v3MapNormal = writer.declLocale( "v3MapNormal"
							, utils.invertNormal( texture( c3d_mapNormal, texCoord.xy() ).xyz() ) );
						v3MapNormal = normalize( sdw::fma( v3MapNormal
							, vec3( 2.0_f )
							, vec3( -1.0_f ) ) );

						if ( checkFlag( textureFlags, TextureChannel::eHeight )
							&& !checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
						{
							auto c3d_mapHeight( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapHeight" ) );
							auto mapHeight = writer.declLocale( "mapHeight"
								, texture( c3d_mapHeight, texCoord.xy() ).r() );
							v3MapNormal = mix( vec3( 0.0_f, 0.0_f, 1.0_f )
								, v3MapNormal
								, vec3( mapHeight ) );
						}

						auto tbn = writer.declLocale( "tbn"
							, mat3( normalize( vtx_tangent )
								, normalize( vtx_bitangent )
								, normal ) );
						normal = normalize( tbn * v3MapNormal );
					}

					if ( checkFlag( textureFlags, TextureChannel::eSpecular ) )
					{
						auto c3d_mapSpecular( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapSpecular" ) );
						specular *= texture( c3d_mapSpecular, texCoord.xy() ).rgb();
					}

					if ( checkFlag( textureFlags, TextureChannel::eGloss ) )
					{
						auto c3d_mapGloss( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapGloss" ) );
						glossiness *= texture( c3d_mapGloss, texCoord.xy() ).r();
					}
				}

				void computePostLightingMapContributions( sdw::ShaderWriter & writer
					, shader::Utils const & utils
					, sdw::Vec3 & diffuse
					, sdw::Vec3 & emissive
					, sdw::Float const & gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags )
				{
					using namespace sdw;
					auto texCoord( writer.getVariable< Vec3 >( "texCoord" ) );

					if ( checkFlag( textureFlags, TextureChannel::eAlbedo ) )
					{
						auto c3d_mapDiffuse( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapDiffuse" ) );
						diffuse *= utils.removeGamma( gamma
							, texture( c3d_mapDiffuse, texCoord.xy() ).xyz() );
					}

					emissive *= diffuse;

					if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
					{
						auto c3d_mapEmissive( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapEmissive" ) );
						emissive *= utils.removeGamma( gamma
							, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
					}
				}

				std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
					, uint32_t & index
					, uint32_t maxCascades )
				{
					auto result = std::make_shared< SpecularBrdfLightingModel >( writer );
					result->declareModel( index, maxCascades );
					return result;
				}

				std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
					, ShadowType shadows
					, bool volumetric
					, uint32_t & index
					, uint32_t maxCascades )
				{
					auto result = std::make_shared< SpecularBrdfLightingModel >( writer );
					result->declareDirectionalModel( shadows
						, volumetric
						, index
						, maxCascades );
					return result;
				}

				std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
					, LightType lightType
					, ShadowType shadows
					, bool volumetric
					, uint32_t & index )
				{
					auto result = std::make_shared< SpecularBrdfLightingModel >( writer );

					switch ( lightType )
					{
					case LightType::eDirectional:
						CU_Failure( "Directional light model should use the other overload" );
						break;

					case LightType::ePoint:
						result->declarePointModel( shadows, volumetric, index );
						break;

					case LightType::eSpot:
						result->declareSpotModel( shadows, volumetric, index );
						break;

					default:
						CU_Failure( "Invalid light type" );
						break;
					}

					return result;
				}
			}
		}

		//*********************************************************************

		void applyAlphaFunc( sdw::ShaderWriter & writer
			, ashes::CompareOp alphaFunc
			, sdw::Float const & alpha
			, sdw::Float const & alphaRef )
		{
			using namespace sdw;

			switch ( alphaFunc )
			{
			case ashes::CompareOp::eLess:
				IF( writer, alpha >= alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			case ashes::CompareOp::eLessEqual:
				IF( writer, alpha > alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			case ashes::CompareOp::eEqual:
				IF( writer, alpha != alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			case ashes::CompareOp::eNotEqual:
				IF( writer, alpha == alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			case ashes::CompareOp::eGreaterEqual:
				IF( writer, alpha < alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			case ashes::CompareOp::eGreater:
				IF( writer, alpha <= alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			default:
				IF( writer, alpha <= 0.2_f )
				{
					writer.discard();
				}
				FI;
				break;
			}
		}

		std::unique_ptr< Materials > createMaterials( sdw::ShaderWriter & writer
			, PassFlags const & passFlags )
		{
			std::unique_ptr< Materials > result;

			if ( checkFlag( passFlags, PassFlag::ePbrMetallicRoughness ) )
			{
				result = std::make_unique< PbrMRMaterials >( writer );
			}
			else if ( checkFlag( passFlags, PassFlag::ePbrSpecularGlossiness ) )
			{
				result = std::make_unique< PbrSGMaterials >( writer );
			}
			else
			{
				result = std::make_unique< LegacyMaterials >( writer );
			}

			return result;
		}

		ParallaxShadowFunction declareParallaxShadowFunc( sdw::ShaderWriter & writer
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags )
		{
			ParallaxShadowFunction result;

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				using namespace sdw;
				auto c3d_mapHeight( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapHeight" ) );
				auto c3d_heightScale( writer.getVariable< Float >( "c3d_heightScale" ) );

				result = writer.implementFunction< Float >( "ParallaxSoftShadowMultiplier"
					, [&]( Vec3 const & lightDir
						, Vec2 const initialTexCoord
						, Float initialHeight )
					{
						auto shadowMultiplier = writer.declLocale( "shadowMultiplier"
							, 1.0_f );
						auto minLayers = writer.declLocale( "minLayers"
							, 10.0_f );
						auto maxLayers = writer.declLocale( "maxLayers"
							, 20.0_f );

						// calculate lighting only for surface oriented to the light source
						IF( writer, dot( vec3( 0.0_f, 0.0_f, 1.0_f ), lightDir ) > 0.0_f )
						{
							// calculate initial parameters
							auto numSamplesUnderSurface = writer.declLocale( "numSamplesUnderSurface"
								, 0.0_f );
							shadowMultiplier = 0.0_f;
							auto numLayers = writer.declLocale( "numLayers"
								, mix( maxLayers
									, minLayers
									, sdw::abs( dot( vec3( 0.0_f, 0.0, 1.0 ), lightDir ) ) ) );
							auto layerHeight = writer.declLocale( "layerHeight"
								, initialHeight / numLayers );
							auto texStep = writer.declLocale( "deltaTexCoords"
								, writer.paren( lightDir.xy() * c3d_heightScale ) / lightDir.z() / numLayers );

							// current parameters
							auto currentLayerHeight = writer.declLocale( "currentLayerHeight"
								, initialHeight - layerHeight );
							auto currentTextureCoords = writer.declLocale( "currentTextureCoords"
								, initialTexCoord + texStep );
							auto heightFromTexture = writer.declLocale( "heightFromTexture"
								, texture( c3d_mapHeight, currentTextureCoords ).r() );
							auto stepIndex = writer.declLocale( "stepIndex"
								, 1_i );

							// while point is below depth 0.0 )
							WHILE( writer, currentLayerHeight > 0.0_f )
							{
								// if point is under the surface
								IF( writer, heightFromTexture < currentLayerHeight )
								{
									// calculate partial shadowing factor
									numSamplesUnderSurface += 1.0_f;
									auto newShadowMultiplier = writer.declLocale( "newShadowMultiplier"
										, writer.paren( currentLayerHeight - heightFromTexture )
										* writer.paren( 1.0_f - writer.cast< Float >( stepIndex ) / numLayers ) );
									shadowMultiplier = max( shadowMultiplier, newShadowMultiplier );
								}
								FI;

								// offset to the next layer
								stepIndex += 1_i;
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
					}
					, InVec3{ writer, "lightDir" }
						, InVec2{ writer, "initialTexCoord" }
					, InFloat{ writer, "initialHeight" } );
			}

			return result;
		}

		ParallaxFunction declareParallaxMappingFunc( sdw::ShaderWriter & writer
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags )
		{
			using namespace sdw;
			ParallaxFunction result;

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto c3d_mapHeight( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapHeight" ) );
				auto c3d_heightScale( writer.getVariable< Float >( "c3d_heightScale" ) );

				result = writer.implementFunction< Vec2 >( "ParallaxMapping",
					[&]( Vec2 const & texCoords, Vec3 const & viewDir )
					{
						// number of depth layers
						auto minLayers = writer.declLocale( "minLayers"
							, 10.0_f );
						auto maxLayers = writer.declLocale( "maxLayers"
							, 20.0_f );
						auto numLayers = writer.declLocale( "numLayers"
							, mix( maxLayers
								, minLayers
								, sdw::abs( dot( vec3( 0.0_f, 0.0_f, 1.0_f ), viewDir ) ) ) );
						// calculate the size of each layer
						auto layerDepth = writer.declLocale( "layerDepth"
							, 1.0_f / numLayers );
						// depth of current layer
						auto currentLayerDepth = writer.declLocale( "currentLayerDepth"
							, 0.0_f );
						// the amount to shift the texture coordinates per layer (from vector P)
						auto p = writer.declLocale( "p"
							, viewDir.xy() * c3d_heightScale );
						auto deltaTexCoords = writer.declLocale( "deltaTexCoords"
							, p / numLayers );

						auto currentTexCoords = writer.declLocale( "currentTexCoords"
							, texCoords );
						auto currentDepthMapValue = writer.declLocale( "currentDepthMapValue"
							, texture( c3d_mapHeight, currentTexCoords ).r() );

						WHILE( writer, currentLayerDepth < currentDepthMapValue )
						{
							// shift texture coordinates along direction of P
							currentTexCoords -= deltaTexCoords;
							// get depthmap value at current texture coordinates
							currentDepthMapValue = textureLod( c3d_mapHeight, currentTexCoords, 0.0_f ).r();
							// get depth of next layer
							currentLayerDepth += layerDepth;
						}
						ELIHW;

						// get texture coordinates before collision (reverse operations)
						auto prevTexCoords = writer.declLocale( "prevTexCoords"
							, currentTexCoords + deltaTexCoords );

						// get depth after and before collision for linear interpolation
						auto afterDepth = writer.declLocale( "afterDepth"
							, currentDepthMapValue - currentLayerDepth );
						auto beforeDepth = writer.declLocale( "beforeDepth"
							, texture( c3d_mapHeight, prevTexCoords ).r() - currentLayerDepth + layerDepth );

						// interpolation of texture coordinates
						auto weight = writer.declLocale( "weight"
							, vec2( afterDepth / writer.paren( afterDepth - beforeDepth ) ) );
						auto finalTexCoords = writer.declLocale( "finalTexCoords"
							, sdw::fma( prevTexCoords
								, weight
								, currentTexCoords * writer.paren( vec2( 1.0_f ) - weight ) ) );

						writer.returnStmt( finalTexCoords );
					}
					, InVec2{ writer, "texCoords" }
					, InVec3{ writer, "viewDir" } );
			}

			return result;
		}
	}

	//*************************************************************************
}
