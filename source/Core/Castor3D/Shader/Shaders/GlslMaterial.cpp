#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArraySsbo.hpp>

namespace castor3d::shader
{
	//*****************************************************************************************

	sdw::Vec3 Material::colour()const
	{
		return colourDiv().rgb();
	}

	void Material::getPassMultipliers( SubmeshFlags submeshFlags
		, sdw::UInt const & passCount
		, sdw::UVec4 const & passMasks
		, sdw::Array< sdw::Vec4 > & passMultipliers )const
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks )
			&& passMasks.isEnabled() )
		{
			FOR( *m_writer, sdw::UInt, passIdx, 0_u, passIdx < passCount, ++passIdx )
			{
				auto mask32 = passMasks[passIdx / 4_u];
				auto mask8 = ( mask32 >> ( ( passIdx % 4_u ) * 8_u ) ) & 0xFF_u;
				passMultipliers[passIdx / 4_u][passIdx % 4_u] = m_writer->cast< sdw::Float >( mask8 ) / 255.0_f;
			}
			ROF;
		}
	}

	void Material::applyAlphaFunc( VkCompareOp alphaFunc
		, sdw::Float & alpha
		, sdw::Float const & passMultiplier
		, bool opaque )
	{
		applyAlphaFunc( alphaFunc
			, alpha
			, alphaRef()
			, passMultiplier
			, opaque );
	}

	void Material::applyAlphaFunc( VkCompareOp alphaFunc
		, sdw::Float & alpha
		, sdw::Float const & palphaRef
		, sdw::Float const & passMultiplier
		, bool opaque )
	{
		applyAlphaFunc( *m_writer
			, alphaFunc
			, alpha
			, alphaRef()
			, passMultiplier
			, opaque );
	}

	void Material::applyAlphaFunc( sdw::ShaderWriter & writer
		, VkCompareOp alphaFunc
		, sdw::Float & alpha
		, sdw::Float const & palphaRef
		, sdw::Float const & passMultiplier
		, bool opaque )
	{
		alpha *= passMultiplier;

		if ( alphaFunc != VK_COMPARE_OP_ALWAYS )
		{
			switch ( alphaFunc )
			{
			case VK_COMPARE_OP_NEVER:
				writer.demote();
				break;
			case VK_COMPARE_OP_LESS:
				IF( writer, alpha >= palphaRef )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_EQUAL:
				IF( writer, alpha != palphaRef )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_LESS_OR_EQUAL:
				IF( writer, alpha > palphaRef )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_GREATER:
				IF( writer, alpha <= palphaRef )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_NOT_EQUAL:
				IF( writer, alpha == palphaRef )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_GREATER_OR_EQUAL:
				IF( writer, alpha < palphaRef )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_ALWAYS:
				break;

			default:
				break;
			}
		}
	}

	sdw::UInt Material::getTexture( uint32_t idx )const
	{
		return ( idx < 4u
			? textures0()[idx]
			: ( idx < 8u
				? textures1()[idx - 4u]
				: 0_u ) );
	}

	//*********************************************************************************************

	Materials::Materials( sdw::ShaderWriter & writer )
		: m_writer{ writer }
	{
	}

	Materials::Materials( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: Materials{ writer }
	{
		if ( enable )
		{
			declare( binding, set );
		}
	}

	void Materials::declare( uint32_t binding
		, uint32_t set )
	{
		m_ssbo = std::make_unique< sdw::ArraySsboT< Material > >( m_writer
			, PassBufferName
			, binding
			, set
			, true );
	}

	Material Materials::getMaterial( sdw::UInt const & index )const
	{
		return ( m_ssbo
			? ( *m_ssbo )[index - 1_u]
			: m_writer.declLocale< Material >( "mat", false ) );
	}

	void Materials::blendMaterials( Utils & utils
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, SubmeshFlags const & submeshFlags
		, TextureFlagsArray const & textures
		, bool hasTextures
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::UInt const & passCount
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, OpacityBlendComponents & output )const
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) )
		{
			auto resOpacity = m_writer.declLocale( "resOpacity"
				, 0.0_f
				, output.opacity.isEnabled() );

			FOR( m_writer, sdw::UInt, passIdx, 0_u, passIdx < passCount, ++passIdx )
			{
				auto passMultiplier = m_writer.declLocale( "passMultiplier"
					, passMultipliers[passIdx / 4_u][passIdx % 4_u] );

				IF( m_writer, passMultiplier != 0.0_f )
				{
					OpacityBlendComponents components{ m_writer.declLocale( "passTexCoord0", output.texCoord0 )
						, m_writer.declLocale( "passOpacity", output.opacity ) };
					applyMaterial( utils
						, alphaFunc
						, passFlags
						, textures
						, hasTextures
						, textureConfigs
						, textureAnims
						, maps
						, materialId + passIdx
						, components );
					resOpacity += components.opacity * passMultiplier;
				}
				FI;
			}
			ROF;

			output.opacity = resOpacity;
			Material::applyAlphaFunc( m_writer
				, alphaFunc
				, output.opacity
				, 0.0_f
				, 1.0_f );
		}
		else
		{
			auto material = applyMaterial( utils
				, alphaFunc
				, passFlags
				, textures
				, hasTextures
				, textureConfigs
				, textureAnims
				, maps
				, materialId
				, output );
			material.applyAlphaFunc( alphaFunc
				, output.opacity
				, 1.0_f );
		}
	}

	void Materials::blendMaterials( Utils & utils
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, SubmeshFlags const & submeshFlags
		, TextureFlagsArray const & textures
		, bool hasTextures
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::UInt const & passCount
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, GeometryBlendComponents & output )const
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) )
		{
			auto resOpacity = m_writer.declLocale( "resOpacity"
				, 0.0_f
				, output.opacity.isEnabled() );
			auto resNormal = m_writer.declLocale( "resNormal"
				, vec3( 0.0_f )
				, output.normal.isEnabled() );
			auto resTangent = m_writer.declLocale( "resTangent"
				, vec3( 0.0_f )
				, output.tangent.isEnabled() );
			auto resBitangent = m_writer.declLocale( "resBitangent"
				, vec3( 0.0_f )
				, output.bitangent.isEnabled() );
			auto resTangentSpaceViewPosition = m_writer.declLocale( "resTangentSpaceViewPosition"
				, vec3( 0.0_f )
				, output.tangentSpaceViewPosition.isEnabled() );
			auto resTangentSpaceFragPosition = m_writer.declLocale( "resTangentSpaceFragPosition"
				, vec3( 0.0_f )
				, output.tangentSpaceFragPosition.isEnabled() );

			FOR( m_writer, sdw::UInt, passIdx, 0_u, passIdx < passCount, ++passIdx )
			{
				auto passMultiplier = m_writer.declLocale( "passMultiplier"
					, passMultipliers[passIdx / 4_u][passIdx % 4_u] );

				IF( m_writer, passMultiplier != 0.0_f )
				{
					GeometryBlendComponents components{ m_writer.declLocale( "passTexCoord0", output.texCoord0 )
						, m_writer.declLocale( "passTexCoord1", output.texCoord1 )
						, m_writer.declLocale( "passTexCoord2", output.texCoord2 )
						, m_writer.declLocale( "passTexCoord3", output.texCoord3 )
						, m_writer.declLocale( "passOpacity", output.opacity )
						, m_writer.declLocale( "passNormal", output.normal )
						, m_writer.declLocale( "passTangent", output.tangent )
						, m_writer.declLocale( "passBitangent", output.bitangent )
						, m_writer.declLocale( "passTangentSpaceViewPosition", output.tangentSpaceViewPosition )
						, m_writer.declLocale( "passTangentSpaceFragPosition", output.tangentSpaceFragPosition ) };
					applyMaterial( utils
						, alphaFunc
						, passFlags
						, textures
						, hasTextures
						, textureConfigs
						, textureAnims
						, maps
						, materialId + passIdx
						, components );
					resOpacity += components.opacity * passMultiplier;
					resNormal += components.normal * passMultiplier;
					resTangent += components.tangent * passMultiplier;
					resBitangent += components.bitangent * passMultiplier;
					resTangentSpaceViewPosition += components.tangentSpaceViewPosition * passMultiplier;
					resTangentSpaceFragPosition += components.tangentSpaceFragPosition * passMultiplier;
				}
				FI;
			}
			ROF;

			output.opacity = resOpacity;
			output.normal = normalize( resNormal );
			output.tangent = normalize( resTangent );
			output.bitangent = normalize( resBitangent );
			output.tangentSpaceViewPosition = resTangentSpaceViewPosition;
			output.tangentSpaceFragPosition = resTangentSpaceFragPosition;
			Material::applyAlphaFunc( m_writer
				, alphaFunc
				, output.opacity
				, 0.0_f
				, 1.0_f );
		}
		else
		{
			auto material = applyMaterial( utils
				, alphaFunc
				, passFlags
				, textures
				, hasTextures
				, textureConfigs
				, textureAnims
				, maps
				, materialId
				, output );
			material.applyAlphaFunc( alphaFunc
				, output.opacity
				, 1.0_f );
		}
	}

	std::unique_ptr< LightMaterial > Materials::blendMaterials( Utils & utils
		, bool needsRsm
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, SubmeshFlags const & submeshFlags
		, TextureFlagsArray const & textures
		, bool hasTextures
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::UInt const & passCount
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, sdw::Vec3 const & vertexColour
		, OpaqueBlendComponents & output )const
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) )
		{
			auto resOpacity = m_writer.declLocale( "resOpacity"
				, 0.0_f
				, output.opacity.isEnabled() );
			auto resOcclusion = m_writer.declLocale( "resOcclusion"
				, 0.0_f
				, output.occlusion.isEnabled() );
			auto resTransmittance = m_writer.declLocale( "resTransmittance"
				, 0.0_f
				, output.transmittance.isEnabled() );
			auto resEmissive = m_writer.declLocale( "resEmissive"
				, vec3( 0.0_f )
				, output.emissive.isEnabled() );
			auto resNormal = m_writer.declLocale( "resNormal"
				, vec3( 0.0_f )
				, output.normal.isEnabled() );
			auto resTangent = m_writer.declLocale( "resTangent"
				, vec3( 0.0_f )
				, output.tangent.isEnabled() );
			auto resBitangent = m_writer.declLocale( "resBitangent"
				, vec3( 0.0_f )
				, output.bitangent.isEnabled() );
			auto resTangentSpaceViewPosition = m_writer.declLocale( "resTangentSpaceViewPosition"
				, vec3( 0.0_f )
				, output.tangentSpaceViewPosition.isEnabled() );
			auto resTangentSpaceFragPosition = m_writer.declLocale( "resTangentSpaceFragPosition"
				, vec3( 0.0_f )
				, output.tangentSpaceFragPosition.isEnabled() );
			auto result = lightingModel.declMaterial( "lightMat" );
			result->create( vec3( 0.0_f )
				, vec4( 0.0_f )
				, vec4( 0.0_f ) );
			result->depthFactor = 0.0_f;
			result->normalFactor = 0.0_f;
			result->objectFactor = 0.0_f;
			result->edgeColour = vec4( 0.0_f );
			result->specific = vec4( 0.0_f );

			FOR( m_writer, sdw::UInt, passIdx, 0_u, passIdx < passCount, ++passIdx )
			{
				auto passMultiplier = m_writer.declLocale( "passMultiplier"
					, passMultipliers[passIdx / 4_u][passIdx % 4_u] );

				IF( m_writer, passMultiplier != 0.0_f )
				{
					OpaqueBlendComponents components{ m_writer.declLocale( "passTexCoord0", output.texCoord0 )
						, m_writer.declLocale( "passTexCoord1", output.texCoord1 )
						, m_writer.declLocale( "passTexCoord2", output.texCoord2 )
						, m_writer.declLocale( "passTexCoord3", output.texCoord3 )
						, m_writer.declLocale( "passOpacity", output.opacity )
						, m_writer.declLocale( "passOcclusion", output.occlusion )
						, m_writer.declLocale( "passTransmittance", output.transmittance )
						, m_writer.declLocale( "passEmissive", output.emissive )
						, m_writer.declLocale( "passNormal", output.normal )
						, m_writer.declLocale( "passTangent", output.tangent )
						, m_writer.declLocale( "passBitangent", output.bitangent )
						, m_writer.declLocale( "passTangentSpaceViewPosition", output.tangentSpaceViewPosition )
						, m_writer.declLocale( "passTangentSpaceFragPosition", output.tangentSpaceFragPosition ) };
					auto [material, lightMat] = applyMaterial( utils
						, needsRsm
						, alphaFunc
						, passFlags
						, textures
						, hasTextures
						, textureConfigs
						, textureAnims
						, lightingModel
						, maps
						, materialId + passIdx
						, vertexColour
						, components );
					result->blendWith( *lightMat, passMultiplier );

					resOpacity += components.opacity * passMultiplier;
					resOcclusion += components.occlusion * passMultiplier;
					resTransmittance += components.transmittance * passMultiplier;
					resEmissive += components.emissive * passMultiplier;
					resNormal += components.normal * passMultiplier;
					resTangent += components.tangent * passMultiplier;
					resBitangent += components.bitangent * passMultiplier;
					resTangentSpaceViewPosition += components.tangentSpaceViewPosition * passMultiplier;
					resTangentSpaceFragPosition += components.tangentSpaceFragPosition * passMultiplier;
				}
				FI;
			}
			ROF;

			output.opacity = resOpacity;
			output.occlusion = resOpacity;
			output.transmittance = resTransmittance;
			output.emissive = resEmissive;
			output.normal = normalize( resNormal );
			output.tangent = normalize( resTangent );
			output.bitangent = normalize( resBitangent );
			output.tangentSpaceViewPosition = resTangentSpaceViewPosition;
			output.tangentSpaceFragPosition = resTangentSpaceFragPosition;
			Material::applyAlphaFunc( m_writer
				, alphaFunc
				, output.opacity
				, 0.0_f
				, 1.0_f );
			result->albedo *= vertexColour;
			return result;
		}

		auto [material, lightMat] = applyMaterial( utils
			, needsRsm
			, alphaFunc
			, passFlags
			, textures
			, hasTextures
			, textureConfigs
			, textureAnims
			, lightingModel
			, maps
			, materialId
			, vertexColour
			, output );
		material.applyAlphaFunc( alphaFunc
			, output.opacity
			, 1.0_f );
		return std::move( lightMat );
	}

	std::unique_ptr< LightMaterial > Materials::blendMaterials( Utils & utils
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, SubmeshFlags const & submeshFlags
		, TextureFlagsArray const & textures
		, bool hasTextures
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::UInt const & passCount
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, sdw::Vec3 const & vertexColour
		, OpaqueBlendComponents & output )const
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) )
		{
			auto resOpacity = m_writer.declLocale( "resOpacity"
				, 0.0_f
				, output.opacity.isEnabled() );
			auto resOcclusion = m_writer.declLocale( "resOcclusion"
				, 0.0_f
				, output.occlusion.isEnabled() );
			auto resTransmittance = m_writer.declLocale( "resTransmittance"
				, 0.0_f
				, output.transmittance.isEnabled() );
			auto resEmissive = m_writer.declLocale( "resEmissive"
				, vec3( 0.0_f )
				, output.emissive.isEnabled() );
			auto resNormal = m_writer.declLocale( "resNormal"
				, vec3( 0.0_f )
				, output.normal.isEnabled() );
			auto resTangent = m_writer.declLocale( "resTangent"
				, vec3( 0.0_f )
				, output.tangent.isEnabled() );
			auto resBitangent = m_writer.declLocale( "resBitangent"
				, vec3( 0.0_f )
				, output.bitangent.isEnabled() );
			auto resTangentSpaceViewPosition = m_writer.declLocale( "resTangentSpaceViewPosition"
				, vec3( 0.0_f )
				, output.tangentSpaceViewPosition.isEnabled() );
			auto resTangentSpaceFragPosition = m_writer.declLocale( "resTangentSpaceFragPosition"
				, vec3( 0.0_f )
				, output.tangentSpaceFragPosition.isEnabled() );
			auto result = lightingModel.declMaterial( "lightMat" );
			result->create( vec3( 0.0_f )
				, vec4( 0.0_f )
				, vec4( 0.0_f ) );
			result->depthFactor = 0.0_f;
			result->normalFactor = 0.0_f;
			result->objectFactor = 0.0_f;
			result->edgeColour = vec4( 0.0_f );
			result->specific = vec4( 0.0_f );

			FOR( m_writer, sdw::UInt, passIdx, 0_u, passIdx < passCount, ++passIdx )
			{
				auto passMultiplier = m_writer.declLocale( "passMultiplier"
					, passMultipliers[passIdx / 4_u][passIdx % 4_u] );

				IF( m_writer, passMultiplier != 0.0_f )
				{
					OpaqueBlendComponents components{ m_writer.declLocale( "passTexCoord0", output.texCoord0 )
						, m_writer.declLocale( "passTexCoord1", output.texCoord1 )
						, m_writer.declLocale( "passTexCoord2", output.texCoord2 )
						, m_writer.declLocale( "passTexCoord3", output.texCoord3 )
						, m_writer.declLocale( "passOpacity", output.opacity )
						, m_writer.declLocale( "passOcclusion", output.occlusion )
						, m_writer.declLocale( "passTransmittance", output.transmittance )
						, m_writer.declLocale( "passEmissive", output.emissive )
						, m_writer.declLocale( "passNormal", output.normal )
						, m_writer.declLocale( "passTangent", output.tangent )
						, m_writer.declLocale( "passBitangent", output.bitangent )
						, m_writer.declLocale( "passTangentSpaceViewPosition", output.tangentSpaceViewPosition )
						, m_writer.declLocale( "passTangentSpaceFragPosition", output.tangentSpaceFragPosition ) };
					auto [material, lightMat] = applyMaterial( utils
						, alphaFunc
						, passFlags
						, textures
						, hasTextures
						, textureConfigs
						, textureAnims
						, lightingModel
						, maps
						, materialId + passIdx
						, vertexColour
						, components );
					result->blendWith( *lightMat, passMultiplier );

					resOpacity += components.opacity * passMultiplier;
					resOcclusion += components.occlusion * passMultiplier;
					resTransmittance += components.transmittance * passMultiplier;
					resEmissive += components.emissive * passMultiplier;
					resNormal += components.normal * passMultiplier;
					resTangent += components.tangent * passMultiplier;
					resBitangent += components.bitangent * passMultiplier;
					resTangentSpaceViewPosition += components.tangentSpaceViewPosition * passMultiplier;
					resTangentSpaceFragPosition += components.tangentSpaceFragPosition * passMultiplier;
				}
				FI;
			}
			ROF;

			output.opacity = resOpacity;
			output.occlusion = resOpacity;
			output.transmittance = resTransmittance;
			output.emissive = resEmissive;
			output.normal = normalize( resNormal );
			output.tangent = normalize( resTangent );
			output.bitangent = normalize( resBitangent );
			output.tangentSpaceViewPosition = resTangentSpaceViewPosition;
			output.tangentSpaceFragPosition = resTangentSpaceFragPosition;
			Material::applyAlphaFunc( m_writer
				, alphaFunc
				, output.opacity
				, 0.0_f
				, 1.0_f );
			result->albedo *= vertexColour;
			return result;
		}

		auto [material, lightMat] = applyMaterial( utils
			, alphaFunc
			, passFlags
			, textures
			, hasTextures
			, textureConfigs
			, textureAnims
			, lightingModel
			, maps
			, materialId
			, vertexColour
			, output );
		material.applyAlphaFunc( alphaFunc
			, output.opacity
			, 1.0_f );
		return std::move( lightMat );
	}

	std::unique_ptr< LightMaterial > Materials::blendMaterials( Utils & utils
		, bool opaque
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, SubmeshFlags const & submeshFlags
		, TextureFlagsArray const & textures
		, bool hasTextures
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::UInt const & passCount
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, sdw::Vec3 const & vertexColour
		, LightingBlendComponents & output )const
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) )
		{
			auto resOpacity = m_writer.declLocale( "resOpacity"
				, 0.0_f
				, output.opacity.isEnabled() );
			auto resOcclusion = m_writer.declLocale( "resOcclusion"
				, 0.0_f
				, output.occlusion.isEnabled() );
			auto resTransmittance = m_writer.declLocale( "resTransmittance"
				, 0.0_f
				, output.transmittance.isEnabled() );
			auto resTransmission = m_writer.declLocale( "resTransmission"
				, vec3( 0.0_f )
				, output.transmission.isEnabled() );
			auto resEmissive = m_writer.declLocale( "resEmissive"
				, vec3( 0.0_f )
				, output.emissive.isEnabled() );
			auto resRefractionRatio = m_writer.declLocale( "resRefractionRatio"
				, 0.0_f
				, output.refractionRatio.isEnabled() );
			auto resHasRefraction = m_writer.declLocale( "resHasRefraction"
				, 0_i
				, output.hasRefraction.isEnabled() );
			auto resHasReflection = m_writer.declLocale( "resHasReflection"
				, 0_i
				, output.hasReflection.isEnabled() );
			auto resBwAccumulationOperator = m_writer.declLocale( "resBwAccumulationOperator"
				, 0.0_f
				, output.bwAccumulationOperator.isEnabled() );
			auto resNormal = m_writer.declLocale( "resNormal"
				, vec3( 0.0_f )
				, output.normal.isEnabled() );
			auto resTangent = m_writer.declLocale( "resTangent"
				, vec3( 0.0_f )
				, output.tangent.isEnabled() );
			auto resBitangent = m_writer.declLocale( "resBitangent"
				, vec3( 0.0_f )
				, output.bitangent.isEnabled() );
			auto resTangentSpaceViewPosition = m_writer.declLocale( "resTangentSpaceViewPosition"
				, vec3( 0.0_f )
				, output.tangentSpaceViewPosition.isEnabled() );
			auto resTangentSpaceFragPosition = m_writer.declLocale( "resTangentSpaceFragPosition"
				, vec3( 0.0_f )
				, output.tangentSpaceFragPosition.isEnabled() );
			auto result = lightingModel.declMaterial( "lightMat" );
			result->create( vec3( 0.0_f )
				, vec4( 0.0_f )
				, vec4( 0.0_f ) );
			result->depthFactor = 0.0_f;
			result->normalFactor = 0.0_f;
			result->objectFactor = 0.0_f;
			result->edgeColour = vec4( 0.0_f );
			result->specific = vec4( 0.0_f );

			FOR( m_writer, sdw::UInt, passIdx, 0_u, passIdx < passCount, ++passIdx )
			{
				auto passMultiplier = m_writer.declLocale( "passMultiplier"
					, passMultipliers[passIdx / 4_u][passIdx % 4_u] );

				IF( m_writer, passMultiplier != 0.0_f )
				{
					LightingBlendComponents components{ m_writer.declLocale( "passTexCoord0", output.texCoord0 )
						, m_writer.declLocale( "passTexCoord1", output.texCoord1 )
						, m_writer.declLocale( "passTexCoord2", output.texCoord2 )
						, m_writer.declLocale( "passTexCoord3", output.texCoord3 )
						, m_writer.declLocale( "passOpacity", output.opacity )
						, m_writer.declLocale( "passOcclusion", output.occlusion )
						, m_writer.declLocale( "passTransmittance", output.transmittance )
						, m_writer.declLocale( "passTransmission", output.transmission )
						, m_writer.declLocale( "passEmissive", output.emissive )
						, m_writer.declLocale( "passRefractionRatio", output.refractionRatio )
						, m_writer.declLocale( "passHasRefraction", output.hasRefraction )
						, m_writer.declLocale( "passHasReflection", output.hasReflection )
						, m_writer.declLocale( "passBwAccumulationOperator", output.bwAccumulationOperator )
						, m_writer.declLocale( "passNormal", output.normal )
						, m_writer.declLocale( "passTangent", output.tangent )
						, m_writer.declLocale( "passBitangent", output.bitangent )
						, m_writer.declLocale( "passTangentSpaceViewPosition", output.tangentSpaceViewPosition )
						, m_writer.declLocale( "passTangentSpaceFragPosition", output.tangentSpaceFragPosition ) };
					auto [material, lightMat] = applyMaterial( utils
						, alphaFunc
						, passFlags
						, textures
						, hasTextures
						, textureConfigs
						, textureAnims
						, lightingModel
						, maps
						, materialId + passIdx
						, vertexColour
						, components );
					result->blendWith( *lightMat, passMultiplier );

					resOpacity += components.opacity * passMultiplier;
					resOcclusion += components.occlusion * passMultiplier;
					resTransmittance += components.transmittance * passMultiplier;
					resTransmission += components.transmission * passMultiplier;
					resEmissive += components.emissive * passMultiplier;
					resRefractionRatio += components.refractionRatio * passMultiplier;
					resHasRefraction += components.hasRefraction;
					resHasReflection += components.hasReflection;
					resBwAccumulationOperator = max( resBwAccumulationOperator, components.bwAccumulationOperator );
					resNormal += components.normal * passMultiplier;
					resTangent += components.tangent * passMultiplier;
					resBitangent += components.bitangent * passMultiplier;
					resTangentSpaceViewPosition += components.tangentSpaceViewPosition * passMultiplier;
					resTangentSpaceFragPosition += components.tangentSpaceFragPosition * passMultiplier;
				}
				FI;
			}
			ROF;

			output.opacity = resOpacity;
			output.occlusion = resOcclusion;
			output.transmittance = resTransmittance;
			output.transmission = resTransmission;
			output.emissive = resEmissive;
			output.refractionRatio = resRefractionRatio;
			output.hasRefraction = min( 1_i, resHasRefraction );
			output.hasReflection = min( 1_i, resHasReflection );
			output.bwAccumulationOperator = resBwAccumulationOperator;
			output.normal = normalize( resNormal );
			output.tangent = normalize( resTangent );
			output.bitangent = normalize( resBitangent );
			output.tangentSpaceViewPosition = resTangentSpaceViewPosition;
			output.tangentSpaceFragPosition = resTangentSpaceFragPosition;
			Material::applyAlphaFunc( m_writer
				, alphaFunc
				, output.opacity
				, 0.0_f
				, 1.0_f
				, opaque );
			result->albedo *= vertexColour;
			return result;
		}

		auto [material, lightMat] = applyMaterial( utils
			, alphaFunc
			, passFlags
			, textures
			, hasTextures
			, textureConfigs
			, textureAnims
			, lightingModel
			, maps
			, materialId
			, vertexColour
			, output );
		material.applyAlphaFunc( alphaFunc
			, output.opacity
			, 1.0_f
			, opaque );
		return std::move( lightMat );
	}

	Material Materials::applyMaterial( Utils & utils
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, TextureFlagsArray const & textures
		, bool hasTextures
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, OpacityBlendComponents & output )const
	{
		auto material = m_writer.declLocale( "passMaterial"
			, getMaterial( materialId ) );
		output.opacity = material.opacity();
		auto textureFlags = merge( textures );

		if ( hasTextures
			&& textureConfigs.isEnabled()
			&& checkFlag( textureFlags, TextureFlag::eOpacity ) )
		{
			for ( uint32_t index = 0u; index < textures.size(); ++index )
			{
				auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
				auto id = m_writer.declLocale( "id" + name
					, material.getTexture( index ) );

				IF( m_writer, id > 0_u )
				{
					auto config = m_writer.declLocale( "config" + name
						, textureConfigs.getTextureConfiguration( id ) );

					IF( m_writer, config.isOpacity() )
					{
						auto anim = m_writer.declLocale( "anim" + name
							, textureAnims.getTextureAnimation( id ) );
						auto texCoord = m_writer.declLocale( "texCoord" + name
							, output.texCoord0.xy() );
						config.transformUV( anim, texCoord );
						auto sampledOpacity = m_writer.declLocale( "sampled" + name
							, utils.sampleMap( passFlags, maps[id - 1_u], texCoord ) );
						output.opacity = config.getOpacity( sampledOpacity, output.opacity );
					}
					FI;
				}
				FI;
			}
		}

		return material;
	}

	Material Materials::applyMaterial( Utils & utils
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, TextureFlagsArray const & textures
		, bool hasTextures
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, GeometryBlendComponents & output )const
	{
		auto material = m_writer.declLocale( "passMaterial"
			, getMaterial( materialId ) );
		output.opacity = material.opacity();

		if ( hasTextures && textureConfigs.isEnabled() )
		{
			auto textureFlags = merge( textures );

			if ( ( textureFlags & TextureFlag::eGeometry ) != 0 )
			{
				for ( uint32_t index = 0u; index < textures.size(); ++index )
				{
					auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
					auto id = m_writer.declLocale( "c3d_id" + name
						, material.getTexture( index ) );

					IF( m_writer, id > 0_u )
					{
						auto config = m_writer.declLocale( "config" + name
							, textureConfigs.getTextureConfiguration( id ) );

						IF( m_writer, config.isGeometry() )
						{
							auto anim = m_writer.declLocale( "anim" + name
								, textureAnims.getTextureAnimation( id ) );
							auto texcoord = m_writer.declLocale( "tex" + name
								, textureConfigs.getTexcoord( config
									, output.texCoord0
									, output.texCoord1
									, output.texCoord2
									, output.texCoord3 ) );
							config.computeGeometryMapContribution( utils
								, passFlags
								, textureFlags
								, name
								, anim
								, maps[id - 1_u]
								, texcoord
								, output.opacity
								, output.normal
								, output.tangent
								, output.bitangent
								, output.tangentSpaceViewPosition
								, output.tangentSpaceFragPosition );
							textureConfigs.setTexcoord( config
								, texcoord
								, output.texCoord0
								, output.texCoord1
								, output.texCoord2
								, output.texCoord3 );
						}
						FI;
					}
					FI;
				}
			}
		}

		return material;
	}

	std::pair< Material, std::unique_ptr< LightMaterial > > Materials::applyMaterial( Utils & utils
		, bool needsRsm
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, TextureFlagsArray const & textures
		, bool hasTextures
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::Vec3 const & vertexColour
		, OpaqueBlendComponents & output )const
	{
		if ( needsRsm )
		{
			return applyMaterial( utils
				, alphaFunc
				, passFlags
				, textures
				, hasTextures
				, textureConfigs
				, textureAnims
				, lightingModel
				, maps
				, materialId
				, vertexColour
				, output );
		}

		auto material = m_writer.declLocale( "passMaterial"
			, getMaterial( materialId ) );
		output.emissive = vec3( material.emissive() );
		output.opacity = material.opacity();
		auto lightMat = lightingModel.declMaterial( "passLightMat"
			, needsRsm );
		lightMat->create( vertexColour
			, material );

		if ( hasTextures )
		{
			textureConfigs.computeGeometryMapContributions( utils
				, passFlags
				, textures
				, textureAnims
				, maps
				, material
				, output.texCoord0
				, output.texCoord1
				, output.texCoord2
				, output.texCoord3
				, output.opacity
				, output.tangentSpaceViewPosition
				, output.tangentSpaceFragPosition );
		}

		return std::make_pair( material, std::move( lightMat ) );
	}

	std::pair< Material, std::unique_ptr< LightMaterial > > Materials::applyMaterial( Utils & utils
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, TextureFlagsArray const & textures
		, bool hasTextures
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::Vec3 const & vertexColour
		, OpaqueBlendComponents & output )const
	{
		auto material = m_writer.declLocale( "passMaterial"
			, getMaterial( materialId ) );
		output.emissive = vec3( material.emissive() );
		output.opacity = material.opacity();
		auto lightMat = lightingModel.declMaterial( "passLightMat" );
		lightMat->create( vertexColour
			, material );

		if ( hasTextures )
		{
			lightingModel.computeMapContributions( passFlags
				, textures
				, textureConfigs
				, textureAnims
				, maps
				, material
				, output.texCoord0
				, output.texCoord1
				, output.texCoord2
				, output.texCoord3
				, output.normal
				, output.tangent
				, output.bitangent
				, output.emissive
				, output.opacity
				, output.occlusion
				, output.transmittance
				, *lightMat
				, output.tangentSpaceViewPosition
				, output.tangentSpaceFragPosition );
		}

		return std::make_pair( material, std::move( lightMat ) );
	}

	std::pair< Material, std::unique_ptr< LightMaterial > > Materials::applyMaterial( Utils & utils
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, TextureFlagsArray const & textures
		, bool hasTextures
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::Vec3 const & vertexColour
		, LightingBlendComponents & output )const
	{
		auto material = m_writer.declLocale( "passMaterial"
			, getMaterial( materialId ) );
		auto lightMat = lightingModel.declMaterial( "passLightMat" );
		lightMat->create( vertexColour
			, material );
		output.emissive = lightMat->albedo * material.emissive();
		output.transmission = material.transmission();
		output.refractionRatio = material.refractionRatio();
		output.hasRefraction = material.hasRefraction();
		output.hasReflection = material.hasReflection();
		output.bwAccumulationOperator = material.bwAccumulationOperator();
		output.opacity = material.opacity();

		lightingModel.computeMapContributions( passFlags
			, textures
			, textureConfigs
			, textureAnims
			, maps
			, material
			, output.texCoord0
			, output.texCoord1
			, output.texCoord2
			, output.texCoord3
			, output.normal
			, output.tangent
			, output.bitangent
			, output.emissive
			, output.opacity
			, output.occlusion
			, output.transmittance
			, *lightMat
			, output.tangentSpaceViewPosition
			, output.tangentSpaceFragPosition );
		return std::make_pair( material, std::move( lightMat ) );
	}

	//*****************************************************************************************

	SssProfile::SssProfile( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, transmittanceProfileSize{ getMember< sdw::Int >( "transmittanceProfileSize" ) }
		, gaussianWidth{ getMember< sdw::Float >( "gaussianWidth" ) }
		, subsurfaceScatteringStrength{ getMember< sdw::Float >( "subsurfaceScatteringStrength" ) }
		, pad{ getMember< sdw::Float >( "pad" ) }
		, transmittanceProfile{ getMemberArray< sdw::Vec4 >( "transmittanceProfile" ) }
	{
	}

	ast::type::BaseStructPtr SssProfile::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "C3D_SssProfile" );

		if ( result->empty() )
		{
			result->declMember( "transmittanceProfileSize", ast::type::Kind::eInt );
			result->declMember( "gaussianWidth", ast::type::Kind::eFloat );
			result->declMember( "subsurfaceScatteringStrength", ast::type::Kind::eFloat );
			result->declMember( "pad", ast::type::Kind::eFloat );
			result->declMember( "transmittanceProfile", ast::type::Kind::eVec4F, TransmittanceProfileSize );
		}

		return result;
	}

	//*********************************************************************************************

	SssProfiles::SssProfiles( sdw::ShaderWriter & writer )
		: m_writer{ writer }
	{
	}

	SssProfiles::SssProfiles( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: SssProfiles{ writer }
	{
		if ( enable )
		{
			declare( binding, set );
		}
	}

	void SssProfiles::declare( uint32_t binding
		, uint32_t set )
	{
		castor::String const SssProfilesBufferName = cuT( "SssProfilesBuffer" );
		m_ssbo = std::make_unique< sdw::ArraySsboT< SssProfile > >( m_writer
			, SssProfilesBufferName
			, binding
			, set
			, true );
	}

	SssProfile SssProfiles::getProfile( sdw::UInt const & index )const
	{
		return ( *m_ssbo )[index - 1_u];
	}

	//*********************************************************************************************
}
