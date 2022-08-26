#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArrayStorageBuffer.hpp>

namespace castor3d::shader
{
	//*****************************************************************************************

	sdw::Vec3 Material::colour()const
	{
		return colourDiv().rgb();
	}

	void Material::getPassMultipliers( PipelineFlags const & flags
		, sdw::UVec4 const & passMasks
		, sdw::Array< sdw::Vec4 > & passMultipliers )const
	{
		if ( flags.enablePassMasks()
			&& passMasks.isEnabled() )
		{
			FOR( *m_writer, sdw::UInt, passIdx, 0_u, passIdx < passCount() && passIdx < MaxPassLayers, ++passIdx )
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
		return textures()[idx / 4u][idx % 4u];
	}

	sdw::UInt Material::getTexture( sdw::UInt const & idx )const
	{
		return textures()[idx / 4u][idx % 4u];
	}

	//*********************************************************************************************

	OpacityBlendComponents::OpacityBlendComponents( sdw::ShaderWriter & writer
		, std::string const & prefix
		, BlendComponentT< sdw::Vec3 > t0
		, BlendComponentT< sdw::Float > opa )
		: m_texCoord0{ writer.declLocale( prefix + "Texcoord0", std::move( t0.value ), t0.enabled ) }
		, m_opacity{ writer.declLocale( prefix + "Opacity", std::move( opa.value ), opa.enabled ) }
	{
	}

	OpacityBlendComponents::OpacityBlendComponents( sdw::ShaderWriter & writer
		, std::string const & prefix
		, OpacityBlendComponents const & rhs )
		: OpacityBlendComponents{ writer
			, prefix
			, { rhs.texCoord0(), rhs.texCoord0().isEnabled() }
			, { rhs.opacity(), rhs.opacity().isEnabled() } }
	{
	}

	OpaResult OpacityBlendComponents::createResult( sdw::ShaderWriter & writer
		, OpacityBlendComponents const & rhs )const
	{
		return OpaResult{ writer.declLocale( "rOpa", 0.0_f, rhs.opacity().isEnabled() ) };
	}

	void OpacityBlendComponents::apply( sdw::Float const & passMultiplier
		, OpaResult & res )const
	{
		res.opa += opacity() * passMultiplier;
	}

	void OpacityBlendComponents::set( OpaResult const & rhs )
	{
		opacity() = rhs.opa;
	}

	//*********************************************************************************************

	GeometryBlendComponents::GeometryBlendComponents( sdw::ShaderWriter & writer
		, std::string const & prefix
		, BlendComponentT< sdw::Vec3 > t0
		, BlendComponentT< sdw::Vec3 > t1
		, BlendComponentT< sdw::Vec3 > t2
		, BlendComponentT< sdw::Vec3 > t3
		, BlendComponentT< sdw::Float > opa
		, BlendComponentT< sdw::Vec3 > nml
		, BlendComponentT< sdw::Vec3 > tan
		, BlendComponentT< sdw::Vec3 > bit
		, BlendComponentT< sdw::Vec3 > tvp
		, BlendComponentT< sdw::Vec3 > tfp
		, BlendComponentT< sdw::Float > occ )
		: OpacityBlendComponents{ writer, prefix
			, std::move( t0 )
			, std::move( opa ) }
		, m_texCoord1{ writer.declLocale( prefix + "TexCoord1", std::move( t1.value ), t1.enabled ) }
		, m_texCoord2{ writer.declLocale( prefix + "TexCoord2", std::move( t2.value ), t2.enabled ) }
		, m_texCoord3{ writer.declLocale( prefix + "TexCoord3", std::move( t3.value ), t3.enabled ) }
		, m_normal{ writer.declLocale( prefix + "Normal", std::move( nml.value ), nml.enabled ) }
		, m_tangent{ writer.declLocale( prefix + "Tangent", std::move( tan.value ), tan.enabled ) }
		, m_bitangent{ writer.declLocale( prefix + "Bitangent", std::move( bit.value ), bit.enabled ) }
		, m_tangentSpaceViewPosition{ writer.declLocale( prefix + "TangentSpaceViewPosition", std::move( tvp.value ), tvp.enabled ) }
		, m_tangentSpaceFragPosition{ writer.declLocale( prefix + "TangentSpaceFragPosition", std::move( tfp.value ), tfp.enabled ) }
		, m_occlusion{ writer.declLocale( prefix + "Occlusion", std::move( occ.value ), occ.enabled ) }
	{
	}

	GeometryBlendComponents::GeometryBlendComponents( sdw::ShaderWriter & writer
		, std::string const & prefix
		, GeometryBlendComponents const & rhs )
		: GeometryBlendComponents{ writer
			, prefix
			, { rhs.texCoord0(), rhs.texCoord0().isEnabled() }
			, { rhs.texCoord1(), rhs.texCoord1().isEnabled() }
			, { rhs.texCoord2(), rhs.texCoord2().isEnabled() }
			, { rhs.texCoord3(), rhs.texCoord3().isEnabled() }
			, { rhs.opacity(), rhs.opacity().isEnabled() }
			, { rhs.normal(), rhs.normal().isEnabled() }
			, { rhs.tangent(), rhs.tangent().isEnabled() }
			, { rhs.bitangent(), rhs.bitangent().isEnabled() }
			, { rhs.tangentSpaceViewPosition(), rhs.tangentSpaceViewPosition().isEnabled() }
			, { rhs.tangentSpaceFragPosition(), rhs.tangentSpaceFragPosition().isEnabled() }
			, { rhs.occlusion(), rhs.occlusion().isEnabled() } }
	{
	}

	GeoResult GeometryBlendComponents::createResult( sdw::ShaderWriter & writer
		, GeometryBlendComponents const & rhs )const
	{
		return GeoResult{ OpacityBlendComponents::createResult( writer, rhs )
			, writer.declLocale( "rNml", vec3( 0.0_f ), rhs.normal().isEnabled() )
			, writer.declLocale( "rTan", vec3( 0.0_f ), rhs.tangent().isEnabled() )
			, writer.declLocale( "rBit", vec3( 0.0_f ), rhs.bitangent().isEnabled() )
			, writer.declLocale( "rTvp", vec3( 0.0_f ), rhs.tangentSpaceViewPosition().isEnabled() )
			, writer.declLocale( "rTfp", vec3( 0.0_f ), rhs.tangentSpaceFragPosition().isEnabled() )
			, writer.declLocale( "rOcc", 0.0_f, rhs.occlusion().isEnabled() ) };
	}

	void GeometryBlendComponents::apply( sdw::Float const & passMultiplier
		, GeoResult & res )const
	{
		OpacityBlendComponents::apply( passMultiplier, res.opa );
		res.nml += normal() * passMultiplier;
		res.tan += tangent() * passMultiplier;
		res.bit += bitangent() * passMultiplier;
		res.tvp += tangentSpaceViewPosition() * passMultiplier;
		res.tfp += tangentSpaceFragPosition() * passMultiplier;
		res.occ += occlusion() * passMultiplier;
	}

	void GeometryBlendComponents::set( GeoResult const & rhs )
	{
		OpacityBlendComponents::set( rhs.opa );
		normal() = normalize( rhs.nml );
		tangent() = normalize( rhs.tan );
		bitangent() = normalize( rhs.bit );
		tangentSpaceViewPosition() = rhs.tvp;
		tangentSpaceFragPosition() = rhs.tfp;
		occlusion() = rhs.occ;
	}

	//*********************************************************************************************

	OpaqueBlendComponents::OpaqueBlendComponents( sdw::ShaderWriter & writer
		, std::string const & prefix
		, BlendComponentT< sdw::Vec3 > t0
		, BlendComponentT< sdw::Vec3 > t1
		, BlendComponentT< sdw::Vec3 > t2
		, BlendComponentT< sdw::Vec3 > t3
		, BlendComponentT< sdw::Float > opa
		, BlendComponentT< sdw::Vec3 > nml
		, BlendComponentT< sdw::Vec3 > tan
		, BlendComponentT< sdw::Vec3 > bit
		, BlendComponentT< sdw::Vec3 > tvp
		, BlendComponentT< sdw::Vec3 > tfp
		, BlendComponentT< sdw::Float > occ
		, BlendComponentT< sdw::Float > trn
		, BlendComponentT< sdw::Vec3 > ems )
		: GeometryBlendComponents{ writer, prefix
			, std::move( t0 ), std::move( t1 ), std::move( t2 ), std::move( t3 )
			, std::move( opa )
			, std::move( nml ), std::move( tan ), std::move( bit )
			, std::move( tvp ), std::move( tfp ), std::move( occ ) }
		, m_transmittance{ writer.declLocale( prefix + "Transmittance", std::move( trn.value ), trn.enabled ) }
		, m_emissive{ writer.declLocale( prefix + "Emissive", std::move( ems.value ), ems.enabled ) }
	{
	}

	OpaqueBlendComponents::OpaqueBlendComponents( sdw::ShaderWriter & writer
		, std::string const & prefix
		, OpaqueBlendComponents const & rhs )
		: OpaqueBlendComponents{ writer
			, prefix
			, { rhs.texCoord0(), rhs.texCoord0().isEnabled() }
			, { rhs.texCoord1(), rhs.texCoord1().isEnabled() }
			, { rhs.texCoord2(), rhs.texCoord2().isEnabled() }
			, { rhs.texCoord3(), rhs.texCoord3().isEnabled() }
			, { rhs.opacity(), rhs.opacity().isEnabled() }
			, { rhs.normal(), rhs.normal().isEnabled() }
			, { rhs.tangent(), rhs.tangent().isEnabled() }
			, { rhs.bitangent(), rhs.bitangent().isEnabled() }
			, { rhs.tangentSpaceViewPosition(), rhs.tangentSpaceViewPosition().isEnabled() }
			, { rhs.tangentSpaceFragPosition(), rhs.tangentSpaceFragPosition().isEnabled() }
			, { rhs.occlusion(), rhs.occlusion().isEnabled() }
			, { rhs.transmittance(), rhs.transmittance().isEnabled() }
			, { rhs.emissive(), rhs.emissive().isEnabled() } }
	{
	}

	OpqResult OpaqueBlendComponents::createResult( sdw::ShaderWriter & writer
		, OpaqueBlendComponents const & rhs )const
	{
		return OpqResult{ GeometryBlendComponents::createResult( writer, rhs )
			, writer.declLocale( "rTrn", 0.0_f, rhs.transmittance().isEnabled() )
			, writer.declLocale( "eRms", vec3( 0.0_f ), rhs.emissive().isEnabled() ) };
	}

	void OpaqueBlendComponents::apply( sdw::Float const & passMultiplier
		, OpqResult & res )const
	{
		GeometryBlendComponents::apply( passMultiplier, res.geo );
		res.trn += transmittance() * passMultiplier;
		res.ems += emissive() * passMultiplier;
	}

	void OpaqueBlendComponents::set( OpqResult const & rhs )
	{
		GeometryBlendComponents::set( rhs.geo );
		transmittance() = rhs.trn;
		emissive() = rhs.ems;
	}

	//*********************************************************************************************

	VisibilityBlendComponents::VisibilityBlendComponents( sdw::ShaderWriter & writer
		, std::string const & prefix
		, BlendComponentT< DerivTex > t0
		, BlendComponentT< DerivTex > t1
		, BlendComponentT< DerivTex > t2
		, BlendComponentT< DerivTex > t3
		, BlendComponentT< sdw::Float > opa
		, BlendComponentT< sdw::Vec3 > nml
		, BlendComponentT< sdw::Vec3 > tan
		, BlendComponentT< sdw::Vec3 > bit
		, BlendComponentT< sdw::Vec3 > tvp
		, BlendComponentT< sdw::Vec3 > tfp
		, BlendComponentT< sdw::Float > occ
		, BlendComponentT< sdw::Float > trn
		, BlendComponentT< sdw::Vec3 > ems )
		: m_texCoord0{ writer.declLocale( prefix + "TexCoord0", std::move( t0.value ), t0.enabled ) }
		, m_texCoord1{ writer.declLocale( prefix + "TexCoord1", std::move( t1.value ), t1.enabled ) }
		, m_texCoord2{ writer.declLocale( prefix + "TexCoord2", std::move( t2.value ), t2.enabled ) }
		, m_texCoord3{ writer.declLocale( prefix + "TexCoord3", std::move( t3.value ), t3.enabled ) }
		, m_opacity{ writer.declLocale( prefix + "Opacity", std::move( opa.value ), opa.enabled ) }
		, m_normal{ writer.declLocale( prefix + "Normal", std::move( nml.value ), nml.enabled ) }
		, m_tangent{ writer.declLocale( prefix + "Tangent", std::move( tan.value ), tan.enabled ) }
		, m_bitangent{ writer.declLocale( prefix + "Bitangent", std::move( bit.value ), bit.enabled ) }
		, m_tangentSpaceViewPosition{ writer.declLocale( prefix + "TangentSpaceViewPosition", std::move( tvp.value ), tvp.enabled ) }
		, m_tangentSpaceFragPosition{ writer.declLocale( prefix + "TangentSpaceFragPosition", std::move( tfp.value ), tfp.enabled ) }
		, m_occlusion{ writer.declLocale( prefix + "Occlusion", std::move( occ.value ), occ.enabled ) }
		, m_transmittance{ writer.declLocale( prefix + "Transmittance", std::move( trn.value ), trn.enabled ) }
		, m_emissive{ writer.declLocale( prefix + "Emissive", std::move( ems.value ), ems.enabled ) }
	{
	}

	VisibilityBlendComponents::VisibilityBlendComponents( sdw::ShaderWriter & writer
		, std::string const & prefix
		, VisibilityBlendComponents const & rhs )
		: VisibilityBlendComponents{ writer
			, prefix
			, { rhs.texCoord0(), rhs.texCoord0().isEnabled() }
			, { rhs.texCoord1(), rhs.texCoord1().isEnabled() }
			, { rhs.texCoord2(), rhs.texCoord2().isEnabled() }
			, { rhs.texCoord3(), rhs.texCoord3().isEnabled() }
			, { rhs.opacity(), rhs.opacity().isEnabled() }
			, { rhs.normal(), rhs.normal().isEnabled() }
			, { rhs.tangent(), rhs.tangent().isEnabled() }
			, { rhs.bitangent(), rhs.bitangent().isEnabled() }
			, { rhs.tangentSpaceViewPosition(), rhs.tangentSpaceViewPosition().isEnabled() }
			, { rhs.tangentSpaceFragPosition(), rhs.tangentSpaceFragPosition().isEnabled() }
			, { rhs.occlusion(), rhs.occlusion().isEnabled() }
			, { rhs.transmittance(), rhs.transmittance().isEnabled() }
			, { rhs.emissive(), rhs.emissive().isEnabled() } }
	{
	}

	VisResult VisibilityBlendComponents::createResult( sdw::ShaderWriter & writer
		, VisibilityBlendComponents const & rhs )const
	{
		return VisResult{ writer.declLocale( "rOpa", 0.0_f, rhs.opacity().isEnabled() )
			, writer.declLocale( "rNml", vec3( 0.0_f ), rhs.normal().isEnabled() )
			, writer.declLocale( "rTan", vec3( 0.0_f ), rhs.tangent().isEnabled() )
			, writer.declLocale( "rBit", vec3( 0.0_f ), rhs.bitangent().isEnabled() )
			, writer.declLocale( "rTvp", vec3( 0.0_f ), rhs.tangentSpaceViewPosition().isEnabled() )
			, writer.declLocale( "rTfp", vec3( 0.0_f ), rhs.tangentSpaceFragPosition().isEnabled() )
			, writer.declLocale( "rOcc", 0.0_f, rhs.occlusion().isEnabled() )
			, writer.declLocale( "rTrn", 0.0_f, rhs.transmittance().isEnabled() )
			, writer.declLocale( "eRms", vec3( 0.0_f ), rhs.emissive().isEnabled() ) };
	}

	void VisibilityBlendComponents::apply( sdw::Float const & passMultiplier
		, VisResult & res )const
	{
		res.opa += opacity() * passMultiplier;
		res.nml += normal() * passMultiplier;
		res.tan += tangent() * passMultiplier;
		res.bit += bitangent() * passMultiplier;
		res.tvp += tangentSpaceViewPosition() * passMultiplier;
		res.tfp += tangentSpaceFragPosition() * passMultiplier;
		res.occ += occlusion() * passMultiplier;
		res.trn += transmittance() * passMultiplier;
		res.ems += emissive() * passMultiplier;
	}

	void VisibilityBlendComponents::set( VisResult const & rhs )
	{
		opacity() = rhs.opa;
		normal() = normalize( rhs.nml );
		tangent() = normalize( rhs.tan );
		bitangent() = normalize( rhs.bit );
		tangentSpaceViewPosition() = rhs.tvp;
		tangentSpaceFragPosition() = rhs.tfp;
		occlusion() = rhs.occ;
		transmittance() = rhs.trn;
		emissive() = rhs.ems;
	}

	//*********************************************************************************************

	LightingBlendComponents::LightingBlendComponents( sdw::ShaderWriter & writer
		, std::string const & prefix
		, BlendComponentT< sdw::Vec3 > t0
		, BlendComponentT< sdw::Vec3 > t1
		, BlendComponentT< sdw::Vec3 > t2
		, BlendComponentT< sdw::Vec3 > t3
		, BlendComponentT< sdw::Float > opa
		, BlendComponentT< sdw::Vec3 > nml
		, BlendComponentT< sdw::Vec3 > tan
		, BlendComponentT< sdw::Vec3 > bit
		, BlendComponentT< sdw::Vec3 > tvp
		, BlendComponentT< sdw::Vec3 > tfp
		, BlendComponentT< sdw::Float > occ
		, BlendComponentT< sdw::Float > trn
		, BlendComponentT< sdw::Vec3 > ems
		, BlendComponentT< sdw::Vec3 > trs
		, BlendComponentT< sdw::Float > rfr
		, BlendComponentT< sdw::UInt > hrr
		, BlendComponentT< sdw::UInt > hrl
		, BlendComponentT< sdw::Float > acc )
		: OpaqueBlendComponents{ writer, prefix
			, std::move( t0 ), std::move( t1 ), std::move( t2 ), std::move( t3 )
			, std::move( opa )
			, std::move( nml ), std::move( tan ), std::move( bit )
			, std::move( tvp ), std::move( tfp )
			, std::move( occ ), std::move( trn ), std::move( ems ) }
		, m_transmission{ writer.declLocale( prefix + "Transmission", std::move( trs.value ), trs.enabled ) }
		, m_refractionRatio{ writer.declLocale( prefix + "RefractionRatio", std::move( rfr.value ), rfr.enabled ) }
		, m_hasRefraction{ writer.declLocale( prefix + "HasRefraction", std::move( hrr.value ), hrr.enabled ) }
		, m_hasReflection{ writer.declLocale( prefix + "HasReflection", std::move( hrl.value ), hrl.enabled ) }
		, m_bwAccumulationOperator{ writer.declLocale( prefix + "BwAccumulationOperator", std::move( acc.value ), acc.enabled ) }
	{
	}

	LightingBlendComponents::LightingBlendComponents( sdw::ShaderWriter & writer
		, std::string const & prefix
		, LightingBlendComponents const & rhs )
		: LightingBlendComponents{ writer
			, prefix
			, { rhs.texCoord0(), rhs.texCoord0().isEnabled() }
			, { rhs.texCoord1(), rhs.texCoord1().isEnabled() }
			, { rhs.texCoord2(), rhs.texCoord2().isEnabled() }
			, { rhs.texCoord3(), rhs.texCoord3().isEnabled() }
			, { rhs.opacity(), rhs.opacity().isEnabled() }
			, { rhs.normal(), rhs.normal().isEnabled() }
			, { rhs.tangent(), rhs.tangent().isEnabled() }
			, { rhs.bitangent(), rhs.bitangent().isEnabled() }
			, { rhs.tangentSpaceViewPosition(), rhs.tangentSpaceViewPosition().isEnabled() }
			, { rhs.tangentSpaceFragPosition(), rhs.tangentSpaceFragPosition().isEnabled() }
			, { rhs.occlusion(), rhs.occlusion().isEnabled() }
			, { rhs.transmittance(), rhs.transmittance().isEnabled() }
			, { rhs.emissive(), rhs.emissive().isEnabled() }
			, { rhs.transmission(), rhs.transmission().isEnabled() }
			, { rhs.refractionRatio(), rhs.refractionRatio().isEnabled() }
			, { rhs.hasRefraction(), rhs.hasRefraction().isEnabled() }
			, { rhs.hasReflection(), rhs.hasReflection().isEnabled() }
			, { rhs.bwAccumulationOperator(), rhs.bwAccumulationOperator().isEnabled() } }
	{
	}

	LgtResult LightingBlendComponents::createResult( sdw::ShaderWriter & writer
		, LightingBlendComponents const & rhs )const
	{
		return LgtResult{ OpaqueBlendComponents::createResult( writer, rhs )
			, writer.declLocale( "rTrs", vec3( 0.0_f ), rhs.transmission().isEnabled() )
			, writer.declLocale( "rRfr", 0.0_f, rhs.refractionRatio().isEnabled() )
			, writer.declLocale( "rHrr", 0_u, rhs.hasRefraction().isEnabled() )
			, writer.declLocale( "rHrl", 0_u, rhs.hasReflection().isEnabled() )
			, writer.declLocale( "rAcc", 0.0_f, rhs.bwAccumulationOperator().isEnabled() ) };
	}

	void LightingBlendComponents::apply( sdw::Float const & passMultiplier
		, LgtResult & res )const
	{
		OpaqueBlendComponents::apply( passMultiplier, res.opq );
		res.trs += transmission() * passMultiplier;
		res.rfr += refractionRatio() * passMultiplier;
		res.hrr += hasRefraction();
		res.hrl += hasReflection();
		res.acc = max( res.acc, bwAccumulationOperator() );
	}

	void LightingBlendComponents::set( LgtResult const & rhs )
	{
		OpaqueBlendComponents::set( rhs.opq );
		transmission() = rhs.trs;
		refractionRatio() = rhs.rfr;
		hasRefraction() = min( 1_u, rhs.hrr );
		hasReflection() = min( 1_u, rhs.hrl );
		bwAccumulationOperator() = rhs.acc;
	}

	//*****************************************************************************************

	namespace mats
	{
		template< typename ComponentT >
		static void blendMaterialsT( sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, PipelineFlags const & flags
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, ComponentT & output )
		{
			if ( flags.enablePassMasks() )
			{
				ComponentT firstComponents{ writer, "first", output };
				auto result = firstComponents.createResult( writer, output );
				auto material = materials.applyMaterial( "firstMaterial"
					, flags
					, textureConfigs, textureAnims, maps
					, materialId
					, firstComponents
					, utils );
				auto passMultiplier = writer.declLocale( "passMultiplier"
					, passMultipliers[0_u][0_u] );

				IF( writer, passMultiplier != 0.0_f )
				{
					firstComponents.apply( passMultiplier, result );
				}
				FI;

				FOR( writer, sdw::UInt, passIdx, 1_u, passIdx < material.passCount() && passIdx < MaxPassLayers, ++passIdx )
				{
					passMultiplier = passMultipliers[passIdx / 4_u][passIdx % 4_u];

					IF( writer, passMultiplier != 0.0_f )
					{
						ComponentT passComponents{ writer, "pass", output };
						materials.applyMaterial( "passMaterial"
							, flags
							, textureConfigs, textureAnims, maps
							, materialId + passIdx
							, passComponents
							, utils );
						passComponents.apply( passMultiplier, result );
					}
					FI;
				}
				ROF;

				output.set( result );
				Material::applyAlphaFunc( writer
					, flags.alphaFunc
					, output.opacity()
					, 0.0_f
					, 1.0_f );
			}
			else
			{
				auto material = materials.applyMaterial( "passMaterial"
					, flags
					, textureConfigs, textureAnims, maps
					, materialId
					, output
					, utils );
				material.applyAlphaFunc( flags.alphaFunc
					, output.opacity()
					, 1.0_f );
			}
		}

		template< typename ComponentT, typename ... ParamsT >
		std::pair< Material, std::unique_ptr< LightMaterial > > blendMaterialsT( sdw::ShaderWriter & writer
			, Materials const & materials
			, bool opaque
			, PipelineFlags const & flags
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, sdw::Vec3 const & vertexColour
			, ComponentT & output
			, ParamsT && ... params )
		{
			if ( flags.enablePassMasks() )
			{
				auto lightMat = lightingModel.declMaterial( "lightMat" );
				lightMat->create( vec3( 0.0_f )
					, vec4( 0.0_f )
					, vec4( 0.0_f ) );
				lightMat->depthFactor = 0.0_f;
				lightMat->normalFactor = 0.0_f;
				lightMat->objectFactor = 0.0_f;
				lightMat->edgeColour = vec4( 0.0_f );
				lightMat->specific = vec4( 0.0_f );
				ComponentT firstComponents{ writer, "first", output };
				auto result = firstComponents.createResult( writer, output );
				auto firstMats = materials.applyMaterial( "firstMaterial", "firstLightMat"
					, flags
					, textureConfigs, textureAnims, lightingModel, maps
					, materialId, vertexColour
					, firstComponents
					, std::forward< ParamsT >( params )... );
				auto material = std::move( firstMats.first );
				auto firstLightMat = std::move( firstMats.second );
				auto passMultiplier = writer.declLocale( "passMultiplier"
					, passMultipliers[0_u][0_u] );

				IF( writer, passMultiplier != 0.0_f )
				{
					lightMat->blendWith( *firstLightMat, passMultiplier );
					firstComponents.apply( passMultiplier, result );
				}
				FI;

				FOR( writer, sdw::UInt, passIdx, 1_u, passIdx < material.passCount() && passIdx < MaxPassLayers, ++passIdx )
				{
					passMultiplier = passMultipliers[passIdx / 4_u][passIdx % 4_u];

					IF( writer, passMultiplier != 0.0_f )
					{
						ComponentT passComponents{ writer, "pass", output };
						auto curMats = materials.applyMaterial( "passMaterial", "passLightMat"
							, flags
							, textureConfigs, textureAnims, lightingModel, maps
							, materialId + passIdx, vertexColour
							, passComponents
							, std::forward< ParamsT >( params )... );
						auto curMaterial = std::move( curMats.first );
						auto curLightMat = std::move( curMats.second );

						lightMat->blendWith( *curLightMat, passMultiplier );
						material.lighting() += curMaterial.lighting();
						passComponents.apply( passMultiplier, result );
					}
					FI;
				}
				ROF;

				output.set( result );
				Material::applyAlphaFunc( writer
					, flags.alphaFunc
					, output.opacity()
					, 0.0_f
					, 1.0_f
					, opaque );
				lightMat->albedo *= vertexColour;
				return { material, std::move( lightMat ) };
			}

			auto mats = materials.applyMaterial( "passMaterial", "passLightMat"
				, flags
				, textureConfigs, textureAnims, lightingModel, maps
				, materialId, vertexColour
				, output
				, std::forward< ParamsT >( params )... );
			auto material = std::move( mats.first );
			auto result = std::move( mats.second );
			material.applyAlphaFunc( flags.alphaFunc
				, output.opacity()
				, 1.0_f
				, opaque );
			return { material, std::move( result ) };
		}
	}

	//*********************************************************************************************

	Materials::Materials( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: BufferT< Material >{ PassBufferName
			, "materials"
			, writer
			, binding
			, set
			, enable }
	{
	}

	void Materials::blendMaterials( Utils & utils
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, OpacityBlendComponents & output )const
	{
		mats::blendMaterialsT( m_writer, *this, utils
			, flags
			, textureConfigs, textureAnims, maps
			, materialId, passMultipliers
			, output );
	}

	void Materials::blendMaterials( Utils & utils
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, GeometryBlendComponents & output )const
	{
		mats::blendMaterialsT( m_writer, *this, utils
			, flags
			, textureConfigs, textureAnims, maps
			, materialId, passMultipliers
			, output );
	}

	std::unique_ptr< LightMaterial > Materials::blendMaterials( Utils & utils
		, bool needsRsm
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, sdw::Vec3 const & vertexColour
		, OpaqueBlendComponents & output )const
	{
		return mats::blendMaterialsT( m_writer, *this
			, true
			, flags
			, textureConfigs, textureAnims, lightingModel, maps
			, materialId, passMultipliers, vertexColour
			, output
			, utils, needsRsm ).second;
	}

	std::unique_ptr< LightMaterial > Materials::blendMaterials( Utils & utils
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, Material & material
		, sdw::UInt const & materialId
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, sdw::Vec3 const & vertexColour
		, VisibilityBlendComponents & output )const
	{
		if ( flags.enablePassMasks() )
		{
			auto lightMat = lightingModel.declMaterial( "lightMat" );
			lightMat->create( vec3( 0.0_f )
				, vec4( 0.0_f )
				, vec4( 0.0_f ) );
			lightMat->depthFactor = 0.0_f;
			lightMat->normalFactor = 0.0_f;
			lightMat->objectFactor = 0.0_f;
			lightMat->edgeColour = vec4( 0.0_f );
			lightMat->specific = vec4( 0.0_f );
			VisibilityBlendComponents firstComponents{ m_writer, "first", output };
			auto result = firstComponents.createResult( m_writer, output );
			auto firstLightMat = applyMaterial( "firstLightMat"
				, flags
				, textureConfigs, textureAnims, lightingModel, maps
				, material, vertexColour
				, firstComponents );
			auto passMultiplier = m_writer.declLocale( "passMultiplier"
				, passMultipliers[0_u][0_u] );

			IF( m_writer, passMultiplier != 0.0_f )
			{
				lightMat->blendWith( *firstLightMat, passMultiplier );
				firstComponents.apply( passMultiplier, result );
			}
			FI;

			FOR( m_writer, sdw::UInt, passIdx, 1_u, passIdx < material.passCount() && passIdx < MaxPassLayers, ++passIdx )
			{
				passMultiplier = passMultipliers[passIdx / 4_u][passIdx % 4_u];

				IF( m_writer, passMultiplier != 0.0_f )
				{
					VisibilityBlendComponents passComponents{ m_writer, "pass", output };
					auto curMaterial = m_writer.declLocale( "passMaterial"
						, getMaterial( materialId + passIdx ) );
					auto curLightMat = applyMaterial( "passLightMat"
						, flags
						, textureConfigs, textureAnims, lightingModel, maps
						, curMaterial, vertexColour
						, passComponents );

					lightMat->blendWith( *curLightMat, passMultiplier );
					material.lighting() += curMaterial.lighting();
					passComponents.apply( passMultiplier, result );
				}
				FI;
			}
			ROF;

			output.set( result );
			return lightMat;
		}

		return applyMaterial( "passLightMat"
			, flags
			, textureConfigs, textureAnims, lightingModel, maps
			, material, vertexColour
			, output );
	}

	std::unique_ptr< LightMaterial > Materials::blendMaterials( Utils & utils
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, sdw::Vec3 const & vertexColour
		, OpaqueBlendComponents & output )const
	{
		return mats::blendMaterialsT( m_writer, *this
			, true
			, flags
			, textureConfigs, textureAnims, lightingModel, maps
			, materialId, passMultipliers, vertexColour
			, output ).second;
	}

	std::pair< Material, std::unique_ptr< LightMaterial > > Materials::blendMaterials( Utils & utils
		, bool opaque
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, sdw::Vec3 const & vertexColour
		, LightingBlendComponents & output )const
	{
		return mats::blendMaterialsT( m_writer, *this
			, opaque
			, flags
			, textureConfigs, textureAnims, lightingModel, maps
			, materialId, passMultipliers, vertexColour
			, output );
	}

	Material Materials::applyMaterial( std::string const & matName
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, OpacityBlendComponents & output
		, Utils & utils )const
	{
		auto material = m_writer.declLocale( matName
			, getMaterial( materialId ) );
		output.opacity() = material.opacity();

		if ( maps.isEnabled()
			&& textureConfigs.isEnabled()
			&& flags.enableOpacity() )
		{
			FOR( m_writer, sdw::UInt, index, 0u, index < material.texturesCount() && index < MaxPassTextures, ++index )
			{
				auto id = m_writer.declLocale( "c3d_id"
					, material.getTexture( index ) );

				IF( m_writer, id > 0_u )
				{
					auto config = m_writer.declLocale( "c3d_config"
						, textureConfigs.getTextureConfiguration( id ) );

					IF( m_writer, config.isOpacity() )
					{
						auto anim = m_writer.declLocale( "c3d_anim"
							, textureAnims.getTextureAnimation( id ) );
						auto texCoord = m_writer.declLocale( "c3d_texCoord"
							, output.texCoord0().xy() );
						texCoord = utils.transformUV( config, anim, texCoord );
						auto sampledOpacity = m_writer.declLocale( "c3d_sampled"
							, utils.sampleMap( flags, maps[id - 1_u], texCoord ) );
						output.opacity() = config.getOpacity( sampledOpacity, output.opacity() );
					}
					FI;
				}
				FI;
			}
			ROF;
		}

		return material;
	}

	Material Materials::applyMaterial( std::string const & matName
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, GeometryBlendComponents & output
		, Utils & utils )const
	{
		auto material = m_writer.declLocale( matName
			, getMaterial( materialId ) );
		output.opacity() = material.opacity();

		if ( maps.isEnabled()
			&& textureConfigs.isEnabled()
			&& flags.hasGeometryMaps() )
		{
			FOR( m_writer, sdw::UInt, index, 0u, index < material.texturesCount() && index < MaxPassTextures, ++index )
			{
				auto id = m_writer.declLocale( "c3d_id"
					, material.getTexture( index ) );

				IF( m_writer, id > 0_u )
				{
					auto config = m_writer.declLocale( "c3d_config"
						, textureConfigs.getTextureConfiguration( id ) );

					IF( m_writer, config.isGeometry() )
					{
						auto anim = m_writer.declLocale( "c3d_anim"
							, textureAnims.getTextureAnimation( id ) );
						auto texcoord = m_writer.declLocale( "c3d_tex"
							, textureConfigs.getTexcoord( config
								, output.texCoord0()
								, output.texCoord1()
								, output.texCoord2()
								, output.texCoord3() ) );
						config.computeGeometryMapContribution( utils
							, flags
							, anim
							, maps[id - 1_u]
							, texcoord
							, output.opacity()
							, output.normal()
							, output.tangent()
							, output.bitangent()
							, output.tangentSpaceViewPosition()
							, output.tangentSpaceFragPosition() );
						textureConfigs.setTexcoord( config
							, texcoord
							, output.texCoord0()
							, output.texCoord1()
							, output.texCoord2()
							, output.texCoord3() );
					}
					FI;
				}
				FI;
			}
			ROF;
		}

		return material;
	}

	std::pair< Material, std::unique_ptr< LightMaterial > > Materials::applyMaterial( std::string const & matName
		, std::string const & lgtMatName
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::Vec3 const & vertexColour
		, OpaqueBlendComponents & output
		, Utils & utils
		, bool needsRsm )const
	{
		if ( needsRsm )
		{
			return applyMaterial( matName
				, lgtMatName
				, flags
				, textureConfigs
				, textureAnims
				, lightingModel
				, maps
				, materialId
				, vertexColour
				, output );
		}

		auto material = m_writer.declLocale( matName
			, getMaterial( materialId ) );
		output.emissive() = vec3( material.emissive() );
		output.opacity() = material.opacity();
		auto lightMat = lightingModel.declMaterial( lgtMatName
			, needsRsm );
		lightMat->create( vertexColour
			, material );

		if ( maps.isEnabled()
			&& textureConfigs.isEnabled() )
		{
			textureConfigs.computeGeometryMapContributions( utils
				, flags
				, textureAnims
				, maps
				, material
				, output.texCoord0()
				, output.texCoord1()
				, output.texCoord2()
				, output.texCoord3()
				, output.opacity()
				, output.tangentSpaceViewPosition()
				, output.tangentSpaceFragPosition() );
		}

		return std::make_pair( material, std::move( lightMat ) );
	}

	std::unique_ptr< LightMaterial > Materials::applyMaterial( std::string const & lgtMatName
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, Material const & material
		, sdw::Vec3 const & vertexColour
		, VisibilityBlendComponents & output )const
	{
		auto lightMat = lightingModel.declMaterial( lgtMatName );
		lightMat->create( vertexColour
			, material );
		output.emissive() = vec3( material.emissive() );
		output.opacity() = material.opacity();

		if ( maps.isEnabled() )
		{
			lightingModel.computeMapContributions( flags
				, textureConfigs
				, textureAnims
				, maps
				, material
				, output.texCoord0()
				, output.texCoord1()
				, output.texCoord2()
				, output.texCoord3()
				, output.normal()
				, output.tangent()
				, output.bitangent()
				, output.emissive()
				, output.opacity()
				, output.occlusion()
				, output.transmittance()
				, *lightMat
				, output.tangentSpaceViewPosition()
				, output.tangentSpaceFragPosition() );
		}

		return lightMat;
	}

	std::pair< Material, std::unique_ptr< LightMaterial > > Materials::applyMaterial( std::string const & matName
		, std::string const & lgtMatName
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::Vec3 const & vertexColour
		, OpaqueBlendComponents & output )const
	{
		auto material = m_writer.declLocale( matName
			, getMaterial( materialId ) );
		auto lightMat = lightingModel.declMaterial( lgtMatName );
		lightMat->create( vertexColour
			, material );
		output.emissive() = vec3( material.emissive() );
		output.opacity() = material.opacity();

		if ( maps.isEnabled() )
		{
			lightingModel.computeMapContributions( flags
				, textureConfigs
				, textureAnims
				, maps
				, material
				, output.texCoord0()
				, output.texCoord1()
				, output.texCoord2()
				, output.texCoord3()
				, output.normal()
				, output.tangent()
				, output.bitangent()
				, output.emissive()
				, output.opacity()
				, output.occlusion()
				, output.transmittance()
				, *lightMat
				, output.tangentSpaceViewPosition()
				, output.tangentSpaceFragPosition() );
		}

		return std::make_pair( material, std::move( lightMat ) );
	}

	std::pair< Material, std::unique_ptr< LightMaterial > > Materials::applyMaterial( std::string const & matName
		, std::string const & lgtMatName
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, shader::LightingModel & lightingModel
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UInt const & materialId
		, sdw::Vec3 const & vertexColour
		, LightingBlendComponents & output )const
	{
		auto material = m_writer.declLocale( matName
			, getMaterial( materialId ) );
		auto lightMat = lightingModel.declMaterial( lgtMatName );
		lightMat->create( vertexColour
			, material );
		output.emissive() = lightMat->albedo * material.emissive();
		output.opacity() = material.opacity();
		output.transmission() = material.transmission();
		output.refractionRatio() = material.refractionRatio();
		output.hasRefraction() = material.hasRefraction();
		output.hasReflection() = material.hasReflection();
		output.bwAccumulationOperator() = material.bwAccumulationOperator();

		if ( maps.isEnabled() )
		{
			lightingModel.computeMapContributions( flags
				, textureConfigs
				, textureAnims
				, maps
				, material
				, output.texCoord0()
				, output.texCoord1()
				, output.texCoord2()
				, output.texCoord3()
				, output.normal()
				, output.tangent()
				, output.bitangent()
				, output.emissive()
				, output.opacity()
				, output.occlusion()
				, output.transmittance()
				, *lightMat
				, output.tangentSpaceViewPosition()
				, output.tangentSpaceFragPosition() );
		}

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

	SssProfiles::SssProfiles( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: BufferT< SssProfile >{ "SssProfilesBuffer"
			, "profiles"
			, writer
			, binding
			, set
			, enable }
	{
	}

	//*********************************************************************************************
}
