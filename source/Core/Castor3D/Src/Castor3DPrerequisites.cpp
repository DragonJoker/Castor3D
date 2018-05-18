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

namespace glsl
{
	String const TypeTraits< castor3d::shader::Light >::Name = cuT( "Light" );
	String const TypeTraits< castor3d::shader::DirectionalLight >::Name = cuT( "DirectionalLight" );
	String const TypeTraits< castor3d::shader::PointLight >::Name = cuT( "PointLight" );
	String const TypeTraits< castor3d::shader::SpotLight >::Name = cuT( "SpotLight" );
	String const TypeTraits< castor3d::shader::BaseMaterial >::Name = cuT( "BaseMaterial" );
	String const TypeTraits< castor3d::shader::LegacyMaterial >::Name = cuT( "LegacyMaterial" );
	String const TypeTraits< castor3d::shader::MetallicRoughnessMaterial >::Name = cuT( "MetallicRoughnessMaterial" );
	String const TypeTraits< castor3d::shader::SpecularGlossinessMaterial >::Name = cuT( "SpecularGlossinessMaterial" );
}

using namespace glsl;

namespace castor3d
{
	renderer::Format convert( castor::PixelFormat format )
	{
		switch ( format )
		{
		case castor::PixelFormat::eL8:
			return renderer::Format::eR8_UNORM;
		case castor::PixelFormat::eL16F:
			return renderer::Format::eR16_SFLOAT;
		case castor::PixelFormat::eL32F:
			return renderer::Format::eR32_SFLOAT;
		case castor::PixelFormat::eA8L8:
			return renderer::Format::eR8G8_UNORM;
		case castor::PixelFormat::eAL16F:
			return renderer::Format::eR16G16_SFLOAT;
		case castor::PixelFormat::eAL32F:
			return renderer::Format::eR32G32_SFLOAT;
		case castor::PixelFormat::eA1R5G5B5:
			return renderer::Format::eA1R5G5B5_UNORM_PACK16;
		case castor::PixelFormat::eR5G6B5:
			return renderer::Format::eR5G5B5A1_UNORM_PACK16;
		case castor::PixelFormat::eR8G8B8:
			return renderer::Format::eR8G8B8_UNORM;
		case castor::PixelFormat::eB8G8R8:
			return renderer::Format::eB8G8R8_UNORM;
		case castor::PixelFormat::eR8G8B8_SRGB:
			return renderer::Format::eR8G8B8_SRGB;
		case castor::PixelFormat::eB8G8R8_SRGB:
			return renderer::Format::eB8G8R8_SRGB;
		case castor::PixelFormat::eA8R8G8B8:
			return renderer::Format::eR8G8B8A8_UNORM;
		case castor::PixelFormat::eA8B8G8R8:
			return renderer::Format::eA8B8G8R8_UNORM_PACK32;
		case castor::PixelFormat::eA8R8G8B8_SRGB:
			return renderer::Format::eR8G8B8A8_SRGB;
		case castor::PixelFormat::eA8B8G8R8_SRGB:
			return renderer::Format::eA8B8G8R8_SRGB_PACK32;
		case castor::PixelFormat::eRGB16F:
			return renderer::Format::eR16G16B16_SFLOAT;
		case castor::PixelFormat::eRGBA16F:
			return renderer::Format::eR16G16B16A16_SFLOAT;
		case castor::PixelFormat::eRGB32F:
			return renderer::Format::eR32G32B32_SFLOAT;
		case castor::PixelFormat::eRGBA32F:
			return renderer::Format::eR32G32B32A32_SFLOAT;
		case castor::PixelFormat::eDXTC1:
			return renderer::Format::eBC1_RGB_UNORM_BLOCK;
		case castor::PixelFormat::eDXTC3:
			return renderer::Format::eBC2_UNORM_BLOCK;
		case castor::PixelFormat::eDXTC5:
			return renderer::Format::eBC3_UNORM_BLOCK;
		case castor::PixelFormat::eD16:
			return renderer::Format::eD16_UNORM;
		case castor::PixelFormat::eD24S8:
			return renderer::Format::eD24_UNORM_S8_UINT;
		case castor::PixelFormat::eD32:
			return renderer::Format::eX8_D24_UNORM_PACK32;
		case castor::PixelFormat::eD32F:
			return renderer::Format::eD32_SFLOAT;
		case castor::PixelFormat::eD32FS8:
			return renderer::Format::eD32_SFLOAT_S8_UINT;
		case castor::PixelFormat::eS8:
			return renderer::Format::eS8_UINT;
		default:
			assert( false && "Unsupported castor::PixelFormat" );
			return renderer::Format::eR8G8B8A8_UNORM;
		}
	}

	castor::PixelFormat convert( renderer::Format format )
	{
		switch ( format )
		{
		case renderer::Format::eR8_UNORM:
			return castor::PixelFormat::eL8;
		case renderer::Format::eR16_SFLOAT:
			return castor::PixelFormat::eL16F;
		case renderer::Format::eR32_SFLOAT:
			return castor::PixelFormat::eL32F;
		case renderer::Format::eR8G8_UNORM:
			return castor::PixelFormat::eA8L8;
		case renderer::Format::eR16G16_SFLOAT:
			return castor::PixelFormat::eAL16F;
		case renderer::Format::eR32G32_SFLOAT:
			return castor::PixelFormat::eAL32F;
		case renderer::Format::eA1R5G5B5_UNORM_PACK16:
			return castor::PixelFormat::eA1R5G5B5;
		case renderer::Format::eR5G5B5A1_UNORM_PACK16:
			return castor::PixelFormat::eR5G6B5;
		case renderer::Format::eR8G8B8_UNORM:
			return castor::PixelFormat::eR8G8B8;
		case renderer::Format::eB8G8R8_UNORM:
			return castor::PixelFormat::eB8G8R8;
		case renderer::Format::eR8G8B8_SRGB:
			return castor::PixelFormat::eR8G8B8_SRGB;
		case renderer::Format::eB8G8R8_SRGB:
			return castor::PixelFormat::eB8G8R8_SRGB;
		case renderer::Format::eR8G8B8A8_UNORM:
			return castor::PixelFormat::eA8R8G8B8;
		case renderer::Format::eA8B8G8R8_UNORM_PACK32:
			return castor::PixelFormat::eA8B8G8R8;
		case renderer::Format::eR8G8B8A8_SRGB:
			return castor::PixelFormat::eA8R8G8B8_SRGB;
		case renderer::Format::eA8B8G8R8_SRGB_PACK32:
			return castor::PixelFormat::eA8B8G8R8_SRGB;
		case renderer::Format::eR16G16B16_SFLOAT:
			return castor::PixelFormat::eRGB16F;
		case renderer::Format::eR16G16B16A16_SFLOAT:
			return castor::PixelFormat::eRGBA16F;
		case renderer::Format::eR32G32B32_SFLOAT:
			return castor::PixelFormat::eRGB32F;
		case renderer::Format::eR32G32B32A32_SFLOAT:
			return castor::PixelFormat::eRGBA32F;
		case renderer::Format::eBC1_RGB_UNORM_BLOCK:
			return castor::PixelFormat::eDXTC1;
		case renderer::Format::eBC2_UNORM_BLOCK:
			return castor::PixelFormat::eDXTC3;
		case renderer::Format::eBC3_UNORM_BLOCK:
			return castor::PixelFormat::eDXTC5;
		case renderer::Format::eD16_UNORM:
			return castor::PixelFormat::eD16;
		case renderer::Format::eD24_UNORM_S8_UINT:
			return castor::PixelFormat::eD24S8;
		case renderer::Format::eX8_D24_UNORM_PACK32:
			return castor::PixelFormat::eD32;
		case renderer::Format::eD32_SFLOAT:
			return castor::PixelFormat::eD32F;
		case renderer::Format::eD32_SFLOAT_S8_UINT:
			return castor::PixelFormat::eD32FS8;
		case renderer::Format::eS8_UINT:
			return castor::PixelFormat::eS8;
		default:
			assert( false && "Unsupported renderer::Format" );
			return castor::PixelFormat::eA8R8G8B8;
		}
	}

	castor::Matrix4x4f convert( renderer::Mat4 const & value )
	{
		castor::Matrix4x4f result;
		std::memcpy( result.ptr(), &value[0][0], 16 * sizeof( float ) );
		return result;
	}

	renderer::ClearColorValue convert( castor::RgbaColour const & value )
	{
		renderer::ClearColorValue result;
		std::memcpy( result.float32.data(), value.constPtr(), 4 * sizeof( float ) );
		return result;
	}

	castor::RgbaColour convert( renderer::ClearColorValue const & value )
	{
		castor::RgbaColour result;
		std::memcpy( result.ptr(), value.float32.data(), 4 * sizeof( float ) );
		return result;
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
				, SceneFlags const & sceneFlags
				, PassFlags const & passFlags )
			{
				using namespace glsl;
				auto texCoord( writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

				if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
				{
					auto vtx_normal( writer.getBuiltin< Vec3 >( cuT( "vtx_normal" ) ) );
					auto vtx_tangent( writer.getBuiltin< Vec3 >( cuT( "vtx_tangent" ) ) );
					auto vtx_bitangent( writer.getBuiltin< Vec3 >( cuT( "vtx_bitangent" ) ) );
					auto c3d_mapNormal( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapNormal" ) ) );

					auto v3MapNormal = writer.declLocale( cuT( "v3MapNormal" )
						, texture( c3d_mapNormal, texCoord.xy() ).xyz() );
					v3MapNormal = normalize( glsl::fma( v3MapNormal
						, vec3( 2.0_f )
						, vec3( -1.0_f ) ) );

					if ( checkFlag( textureFlags, TextureChannel::eHeight )
						&& !checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
					{
						auto c3d_mapHeight( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapHeight" ) ) );
						v3MapNormal = mix( vec3( 0.0_f, 0.0_f, 1.0_f )
							, v3MapNormal
							, texture( c3d_mapHeight, texCoord.xy() ).r() );
					}

					auto tbn = writer.declLocale( cuT( "tbn" )
						, mat3( normalize( vtx_tangent )
							, normalize( vtx_bitangent )
							, normal ) );
					normal = normalize( tbn * v3MapNormal );
				}

				if ( checkFlag( textureFlags, TextureChannel::eGloss ) )
				{
					auto c3d_mapGloss( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapGloss" ) ) );
					shininess = texture( c3d_mapGloss, texCoord.xy() ).r() * 128.0_f;
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
					auto c3d_mapDiffuse( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapDiffuse" ) ) );
					diffuse *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
						, gamma
						, texture( c3d_mapDiffuse, texCoord.xy() ).xyz() );
				}

				if ( checkFlag( textureFlags, TextureChannel::eSpecular ) )
				{
					auto c3d_mapSpecular( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapSpecular" ) ) );
					specular *= texture( c3d_mapSpecular, texCoord.xy() ).xyz();
				}

				emissive *= diffuse;

				if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
				{
					auto c3d_mapEmissive( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapEmissive" ) ) );
					emissive *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
						, gamma
						, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
				}
			}

			std::shared_ptr< PhongLightingModel > createLightingModel( glsl::GlslWriter & writer
				, uint32_t & index )
			{
				auto result = std::make_shared< PhongLightingModel >( writer );
				result->declareModel( index );
				return result;
			}

			std::shared_ptr< PhongLightingModel > createLightingModel( glsl::GlslWriter & writer
				, LightType lightType
				, ShadowType shadows
				, uint32_t & index )
			{
				auto result = std::make_shared< PhongLightingModel >( writer );

				switch ( lightType )
				{
				case LightType::eDirectional:
					result->declareDirectionalModel( shadows, index );
					break;

				case LightType::ePoint:
					result->declarePointModel( shadows, index );
					break;

				case LightType::eSpot:
					result->declareSpotModel( shadows, index );
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
					, glsl::Vec3 & normal
					, glsl::Float & metallic
					, glsl::Float & roughness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags
					, PassFlags const & passFlags )
				{
					using namespace glsl;
					auto texCoord( writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

					if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
					{
						auto vtx_normal( writer.getBuiltin< Vec3 >( cuT( "vtx_normal" ) ) );
						auto vtx_tangent( writer.getBuiltin< Vec3 >( cuT( "vtx_tangent" ) ) );
						auto vtx_bitangent( writer.getBuiltin< Vec3 >( cuT( "vtx_bitangent" ) ) );
						auto c3d_mapNormal( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapNormal" ) ) );

						auto v3MapNormal = writer.declLocale( cuT( "v3MapNormal" )
							, texture( c3d_mapNormal, texCoord.xy() ).xyz() );
						v3MapNormal = normalize( glsl::fma( v3MapNormal
							, vec3( 2.0_f )
							, vec3( -1.0_f ) ) );

						if ( checkFlag( textureFlags, TextureChannel::eHeight )
							&& !checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
						{
							auto c3d_mapHeight( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapHeight" ) ) );
							v3MapNormal = mix( vec3( 0.0_f, 0.0_f, 1.0_f )
								, v3MapNormal
								, texture( c3d_mapHeight, texCoord.xy() ).r() );
						}

						auto tbn = writer.declLocale( cuT( "tbn" )
							, mat3( normalize( vtx_tangent )
								, normalize( vtx_bitangent )
								, normal ) );
						normal = normalize( tbn * v3MapNormal );
					}

					if ( checkFlag( textureFlags, TextureChannel::eMetallic ) )
					{
						auto c3d_mapMetallic( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapMetallic" ) ) );
						metallic = texture( c3d_mapMetallic, texCoord.xy() ).r();
					}

					if ( checkFlag( textureFlags, TextureChannel::eRoughness ) )
					{
						auto c3d_mapRoughness( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapRoughness" ) ) );
						roughness = texture( c3d_mapRoughness, texCoord.xy() ).r();
					}
				}

				void computePostLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & albedo
					, glsl::Vec3 & emissive
					, glsl::Float const & gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags )
				{
					using namespace glsl;
					auto texCoord( writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

					if ( checkFlag( textureFlags, TextureChannel::eAlbedo ) )
					{
						auto c3d_mapAlbedo( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapAlbedo" ) ) );
						albedo *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
							, gamma
							, texture( c3d_mapAlbedo, texCoord.xy() ).xyz() );
					}

					emissive *= albedo;

					if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
					{
						auto c3d_mapEmissive( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapEmissive" ) ) );
						emissive *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
							, gamma
							, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
					}
				}

				std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, uint32_t & index )
				{
					auto result = std::make_shared< MetallicBrdfLightingModel >( writer );
					result->declareModel( index );
					return result;
				}

				std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, LightType lightType
					, ShadowType shadows
					, uint32_t & index )
				{
					auto result = std::make_shared< MetallicBrdfLightingModel >( writer );

					switch ( lightType )
					{
					case LightType::eDirectional:
						result->declareDirectionalModel( shadows, index );
						break;

					case LightType::ePoint:
						result->declarePointModel( shadows, index );
						break;

					case LightType::eSpot:
						result->declareSpotModel( shadows, index );
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
					, glsl::Vec3 & normal
					, glsl::Vec3 & specular
					, glsl::Float & glossiness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags
					, PassFlags const & passFlags )
				{
					using namespace glsl;
					auto texCoord( writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

					if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
					{
						auto vtx_normal( writer.getBuiltin< Vec3 >( cuT( "vtx_normal" ) ) );
						auto vtx_tangent( writer.getBuiltin< Vec3 >( cuT( "vtx_tangent" ) ) );
						auto vtx_bitangent( writer.getBuiltin< Vec3 >( cuT( "vtx_bitangent" ) ) );
						auto c3d_mapNormal( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapNormal" ) ) );

						auto v3MapNormal = writer.declLocale( cuT( "v3MapNormal" )
							, texture( c3d_mapNormal, texCoord.xy() ).xyz() );
						v3MapNormal = normalize( glsl::fma( v3MapNormal
							, vec3( 2.0_f )
							, vec3( -1.0_f ) ) );

						if ( checkFlag( textureFlags, TextureChannel::eHeight )
							&& !checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
						{
							auto c3d_mapHeight( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapHeight" ) ) );
							v3MapNormal = mix( vec3( 0.0_f, 0.0_f, 1.0_f )
								, v3MapNormal
								, texture( c3d_mapHeight, texCoord.xy() ).r() );
						}

						auto tbn = writer.declLocale( cuT( "tbn" )
							, mat3( normalize( vtx_tangent )
								, normalize( vtx_bitangent )
								, normal ) );
						normal = normalize( tbn * v3MapNormal );
					}

					if ( checkFlag( textureFlags, TextureChannel::eSpecular ) )
					{
						auto c3d_mapSpecular( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapSpecular" ) ) );
						specular *= texture( c3d_mapSpecular, texCoord.xy() ).rgb();
					}

					if ( checkFlag( textureFlags, TextureChannel::eGloss ) )
					{
						auto c3d_mapGloss( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapGloss" ) ) );
						glossiness *= texture( c3d_mapGloss, texCoord.xy() ).r();
					}
				}

				void computePostLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & diffuse
					, glsl::Vec3 & emissive
					, glsl::Float const & gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags )
				{
					using namespace glsl;
					auto texCoord( writer.getBuiltin< Vec3 >( cuT( "texCoord" ) ) );

					if ( checkFlag( textureFlags, TextureChannel::eAlbedo ) )
					{
						auto c3d_mapDiffuse( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapDiffuse" ) ) );
						diffuse *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
							, gamma
							, texture( c3d_mapDiffuse, texCoord.xy() ).xyz() );
					}

					emissive *= diffuse;

					if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
					{
						auto c3d_mapEmissive( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapEmissive" ) ) );
						emissive *= writeFunctionCall< Vec3 >( &writer, cuT( "removeGamma" )
							, gamma
							, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
					}
				}

				std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, uint32_t & index )
				{
					auto result = std::make_shared< SpecularBrdfLightingModel >( writer );
					result->declareModel( index );
					return result;
				}

				std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, LightType lightType
					, ShadowType shadows
					, uint32_t & index )
				{
					auto result = std::make_shared< SpecularBrdfLightingModel >( writer );

					switch ( lightType )
					{
					case LightType::eDirectional:
						result->declareDirectionalModel( shadows, index );
						break;

					case LightType::ePoint:
						result->declarePointModel( shadows, index );
						break;

					case LightType::eSpot:
						result->declareSpotModel( shadows, index );
						break;

					default:
						FAILURE( "Invalid light type" );
						break;
					}

					return result;
				}
			}
		}

		void applyAlphaFunc( glsl::GlslWriter & writer
			, renderer::CompareOp alphaFunc
			, glsl::Float const & alpha
			, glsl::Float const & alphaRef )
		{
			using namespace glsl;

			switch ( alphaFunc )
			{
			case renderer::CompareOp::eLess:
				IF( writer, alpha >= alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			case renderer::CompareOp::eLessEqual:
				IF( writer, alpha > alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			case renderer::CompareOp::eEqual:
				IF( writer, alpha != alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			case renderer::CompareOp::eNotEqual:
				IF( writer, alpha == alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			case renderer::CompareOp::eGreaterEqual:
				IF( writer, alpha < alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			case renderer::CompareOp::eGreater:
				IF( writer, alpha <= alphaRef )
				{
					writer.discard();
				}
				FI;
				break;

			default:
				IF( writer, alpha <= 0.2 )
				{
					writer.discard();
				}
				FI;
				break;
			}
		}

		std::unique_ptr< Materials > createMaterials( glsl::GlslWriter & writer
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

		ParallaxShadowFunction declareParallaxShadowFunc( glsl::GlslWriter & writer
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags )
		{
			ParallaxShadowFunction result;

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				using namespace glsl;
				auto c3d_mapHeight( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapHeight" ) ) );
				auto c3d_heightScale( writer.getBuiltin< Float >( cuT( "c3d_heightScale" ) ) );

				result = writer.implementFunction< Float >( cuT( "ParallaxSoftShadowMultiplier" )
					, [&]( Vec3 const & lightDir
					, Vec2 const initialTexCoord
					, Float initialHeight )
					{
						auto shadowMultiplier = writer.declLocale( cuT( "shadowMultiplier" )
							, 1.0_f );
						auto minLayers = writer.declLocale( cuT( "minLayers" )
							, 10.0_f );
						auto maxLayers = writer.declLocale( cuT( "maxLayers" )
							, 20.0_f );

						// calculate lighting only for surface oriented to the light source
						IF( writer, dot( vec3( 0.0_f, 0, 1 ), lightDir ) > 0.0_f )
						{
							// calculate initial parameters
							auto numSamplesUnderSurface = writer.declLocale( cuT( "numSamplesUnderSurface" )
								, 0.0_f );
							shadowMultiplier = 0;
							auto numLayers = writer.declLocale( cuT( "numLayers" )
								, mix( maxLayers
									, minLayers
									, glsl::abs( dot( vec3( 0.0_f, 0.0, 1.0 ), lightDir ) ) ) );
							auto layerHeight = writer.declLocale( cuT( "layerHeight" )
								, initialHeight / numLayers );
							auto texStep = writer.declLocale( cuT( "deltaTexCoords" )
								, writer.paren( lightDir.xy() * c3d_heightScale ) / lightDir.z() / numLayers );

							// current parameters
							auto currentLayerHeight = writer.declLocale( cuT( "currentLayerHeight" )
								, initialHeight - layerHeight );
							auto currentTextureCoords = writer.declLocale( cuT( "currentTextureCoords" )
								, initialTexCoord + texStep );
							auto heightFromTexture = writer.declLocale( cuT( "heightFromTexture" )
								, texture( c3d_mapHeight, currentTextureCoords ).r() );
							auto stepIndex = writer.declLocale( cuT( "stepIndex" )
								, 1_i );

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
					}, InVec3{ &writer, cuT( "lightDir" ) }
					, InVec2{ &writer, cuT( "initialTexCoord" ) }
					, InFloat{ &writer, cuT( "initialHeight" ) } );
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
				auto c3d_mapHeight( writer.getBuiltin< Sampler2D >( cuT( "c3d_mapHeight" ) ) );
				auto c3d_heightScale( writer.getBuiltin< Float >( cuT( "c3d_heightScale" ) ) );

				result = writer.implementFunction< Vec2 >( cuT( "ParallaxMapping" ),
					[&]( Vec2 const & texCoords, Vec3 const & viewDir )
					{
						// number of depth layers
						auto minLayers = writer.declLocale( cuT( "minLayers" )
							, 10.0_f );
						auto maxLayers = writer.declLocale( cuT( "maxLayers" )
							, 20.0_f );
						auto numLayers = writer.declLocale( cuT( "numLayers" )
							, mix( maxLayers
								, minLayers
								, glsl::abs( dot( vec3( 0.0_f, 0.0, 1.0 ), viewDir ) ) ) );
						// calculate the size of each layer
						auto layerDepth = writer.declLocale( cuT( "layerDepth" )
							, Float( 1.0f / numLayers ) );
						// depth of current layer
						auto currentLayerDepth = writer.declLocale( cuT( "currentLayerDepth" )
							, 0.0_f );
						// the amount to shift the texture coordinates per layer (from vector P)
						auto p = writer.declLocale( cuT( "p" )
							, viewDir.xy() * c3d_heightScale );
						auto deltaTexCoords = writer.declLocale( cuT( "deltaTexCoords" )
							, p / numLayers );

						auto currentTexCoords = writer.declLocale( cuT( "currentTexCoords" )
							, texCoords );
						auto currentDepthMapValue = writer.declLocale( cuT( "currentDepthMapValue" )
							, texture( c3d_mapHeight, currentTexCoords ).r() );

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
						auto prevTexCoords = writer.declLocale( cuT( "prevTexCoords" )
							, currentTexCoords + deltaTexCoords );

						// get depth after and before collision for linear interpolation
						auto afterDepth = writer.declLocale( cuT( "afterDepth" )
							, currentDepthMapValue - currentLayerDepth );
						auto beforeDepth = writer.declLocale( cuT( "beforeDepth" )
							, texture( c3d_mapHeight, prevTexCoords ).r() - currentLayerDepth + layerDepth );

						// interpolation of texture coordinates
						auto weight = writer.declLocale( cuT( "weight" )
							, vec2( afterDepth / writer.paren( afterDepth - beforeDepth ) ) );
						auto finalTexCoords = writer.declLocale( cuT( "finalTexCoords" )
							, glsl::fma( prevTexCoords
								, weight
								, currentTexCoords * writer.paren( vec2( 1.0_f ) - weight ) ) );

						writer.returnStmt( finalTexCoords );
					}, InVec2{ &writer, cuT( "texCoords" ) }
					, InVec3{ &writer, cuT( "viewDir" ) } );
			}

			return result;
		}
	}

	bool isShadowMapProgram( ProgramFlags const & flags )
	{
		return checkFlag( flags, ProgramFlag::eShadowMapDirectional )
			|| checkFlag( flags, ProgramFlag::eShadowMapSpot )
			|| checkFlag( flags, ProgramFlag::eShadowMapPoint );
	}

	FogType getFogType( SceneFlags const & flags )
	{
		FogType result = FogType::eDisabled;

		if ( checkFlag( flags, SceneFlag::eFogLinear) )
		{
			result = FogType::eLinear;
		}
		else if ( checkFlag( flags, SceneFlag::eFogExponential) )
		{
			result = FogType::eExponential;
		}
		else if ( checkFlag( flags, SceneFlag::eFogSquaredExponential ) )
		{
			result = FogType::eSquaredExponential;
		}

		return result;
	}
}
