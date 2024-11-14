#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"

#include "Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp"

#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderAST/Type/TypeStruct.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	namespace blendcomp
	{
		void fillType( ast::type::BaseStruct & type )
		{
			type.declMember( "dielectricF0", sdw::type::Kind::eVec3F );
			type.declMember( "alphaRoughness", sdw::type::Kind::eFloat );
			type.declMember( "f90", sdw::type::Kind::eVec3F );
			type.declMember( "dielectricF90", sdw::type::Kind::eVec3F );
			type.declMember( "specularWeight", sdw::type::Kind::eFloat );
		}

		void fillInit( sdw::expr::ExprList & inits )
		{
			inits.emplace_back( sdw::makeExpr( vec3( 0.04_f ) ) );
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
			inits.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
		}
	}

	//*********************************************************************************************

	BlendComponents::BlendComponents( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, castor::move( expr ), enabled }
		, ior{ getMember( "ior", sdw::Float{ RefractionComponent::Default } ) }
		, perceptualRoughness{ getMember( "roughness", 1.0_f ) }
		, dielectricF0{ getMember( "dielectricF0", vec3( 0.04_f ) ) }
		, alphaRoughness{ getMember( "alphaRoughness", 1.0_f ) }
		, f90{ getMember( "f90", vec3( 1.0_f ) ) }
		, dielectricF90{ getMember( "dielectricF90", vec3( 1.0_f ) ) }
		, metalness{ getMember( "metalness", 0.0_f ) }
		, baseColour{ getMember( "baseColour", vec3( 0.0_f ) ) }
		, sheenColour{ getMember( "sheenColour", vec3( 0.0_f ) ) }
		, sheenRoughness{ getMember( "sheenRoughness", 0.0_f ) }
		, clearcoatF0{ getMember( "clearcoatF0", vec3( 0.0_f ) ) }
		, clearcoatF90{ getMember( "clearcoatF90", vec3( 0.0_f ) ) }
		, clearcoatFactor{ getMember( "clearcoatFactor", 0.0_f ) }
		, clearcoatNormal{ getMember( "clearcoatNormal", vec3( 0.0_f ) ) }
		, clearcoatRoughness{ getMember( "clearcoatRoughness", 0.0_f ) }
		, specularWeight{ getMember( "specularWeight", 1.0_f ) }
		, transmissionFactor{ getMember( "transmissionFactor", 0.0_f ) }
		, thicknessFactor{ getMember( "thicknessFactor", 0.0_f ) }
		, attenuationColour{ getMember( "attenuationColour", vec3( 0.0_f ) ) }
		, attenuationDistance{ getMember( "attenuationDistance", 0.0_f ) }
		, iridescenceFactor{ getMember( "iridescenceFactor", 0.0_f ) }
		, iridescenceThickness{ getMember( "iridescenceThickness", 0.0_f ) }
		, iridescenceIor{ getMember( "iridescenceIor", 0.0_f ) }
		, diffuseTransmissionColour{ getMember( "diffuseTransmissionColour", vec3( 1.0_f ) ) }
		, diffuseTransmissionFactor{ getMember( "diffuseTransmissionFactor", 0.0_f ) }
		, dispersion{ getMember( "dispersion", 0.0_f ) }
		, emissiveColour{ getMember( "emissiveColour", vec3( 0.0_f ) ) }
		, emissiveFactor{ getMember( "emissiveFactor", 0.0_f ) }

		//, ambientColour{ getMember( "ambientColour", vec3( 1.0_f ) ) }
		//, ambientFactor{ getMember( "ambientFactor", 1.0_f ) }
		//, hasTransmission{ getMember( "hasTransmission", 0_u ) }
		//, iridescenceFresnel{ getMember( "iridescenceFresnel", vec3( 0.0_f ) ) }
		//, iridescenceF0{ getMember( "iridescenceF0", vec3( 0.0_f ) ) }
		//, shininess{ computeShininessFromRoughness( roughness ) }
		//, specular{ getMember( "specular", vec3( 0.0_f ) ) }

		, opacity{ getMember( "opacity", 1.0_f ) }
		, bwAccumulationOperator{ getMember( "bwAccumulationOperator", 0_u ) }
		, alphaRef{ getMember( "alphaRef", 0.95_f ) }
		, occlusion{ getMember( "occlusion", 1.0_f ) }
		, transmittance{ getMember( "transmittance", 1.0_f ) }
		, hasReflection{ getMember( "hasReflection", 0_u ) }
	{
		auto & structType = static_cast< sdw::type::Struct const & >( *sdw::StructInstance::getType() );

		if ( auto index = structType.findMember( "normal" );
			index != ast::type::Struct::NotFound )
		{
			m_derivativeValues = ast::type::isStructType( structType.getMember( index ).type );
		}
	}

	BlendComponents::BlendComponents( Materials const & materials
		, Material const & material
		, SurfaceBase const & surface )
		: BlendComponents{ *materials.getWriter()
			, makeInit( materials, material, surface, nullptr )
			, true }
	{
	}

	BlendComponents::BlendComponents( Materials const & materials
		, Material const & material
		, DerivSurfaceBase const & surface )
		: BlendComponents{ *materials.getWriter()
			, makeInit( materials, material, surface, nullptr )
			, true }
	{
	}

	BlendComponents::BlendComponents( Materials const & materials
		, Material const & material
		, SurfaceBase const & surface
		, sdw::Vec4 const & clrCot )
		: BlendComponents{ *materials.getWriter()
			, makeInit( materials, material, surface, &clrCot )
			, true }
	{
	}

	BlendComponents::BlendComponents( Materials const & materials
		, Material const & material
		, DerivSurfaceBase const & surface
		, sdw::Vec4 const & clrCot )
		: BlendComponents{ *materials.getWriter()
			, makeInit( materials, material, surface, &clrCot )
			, true }
	{}

	BlendComponents::BlendComponents( Materials const & materials
		, bool zeroInit )
		: BlendComponents{ *materials.getWriter()
			, makeInit( materials, zeroInit )
			, true }
	{
	}

	void BlendComponents::finish( PassShaders const & passShaders
		, DerivSurfaceBase const & surface
		, CameraData const & camera
		, ModelData const & model
		, Utils & utils )
	{
		passShaders.finishComponents( surface, camera, model, utils, *this );
	}

	void BlendComponents::registerDebug( DebugOutputCategory const & debugOutput )const
	{
		debugOutput.registerOutput( cuT( "Base Colour" ), baseColour.rgb() );
		debugOutput.registerOutput( cuT( "Metalness" ), metalness );
		debugOutput.registerOutput( cuT( "Roughness" ), perceptualRoughness );
		debugOutput.registerOutput( cuT( "IOR" ), ior );
		debugOutput.registerOutput( cuT( "Dielectric F0" ), dielectricF0 );
		debugOutput.registerOutput( cuT( "F90" ), f90 );
		debugOutput.registerOutput( cuT( "Dielectric F90" ), dielectricF90 );
		debugOutput.registerOutput( cuT( "Sheen Colour" ), sheenColour );
		debugOutput.registerOutput( cuT( "Sheen Roughness" ), sheenRoughness );
		debugOutput.registerOutput( cuT( "Clearcoat F0" ), clearcoatF0 );
		debugOutput.registerOutput( cuT( "Clearcoat F90" ), clearcoatF90 );
		debugOutput.registerOutput( cuT( "Clearcoat Factor" ), clearcoatFactor );
		debugOutput.registerOutput( cuT( "Clearcoat Normal" ), clearcoatNormal );
		debugOutput.registerOutput( cuT( "Clearcoat Roughness" ), clearcoatRoughness );
		debugOutput.registerOutput( cuT( "Specular Weight" ), specularWeight );
		debugOutput.registerOutput( cuT( "Transmission Factor" ), transmissionFactor );
		debugOutput.registerOutput( cuT( "Thickness Factor" ), thicknessFactor );
		debugOutput.registerOutput( cuT( "Attenuation Colour" ), attenuationColour );
		debugOutput.registerOutput( cuT( "Attenuation Distance" ), attenuationDistance );
		debugOutput.registerOutput( cuT( "Iridescence Factor" ), iridescenceFactor );
		debugOutput.registerOutput( cuT( "Iridescence IOR" ), iridescenceIor );
		debugOutput.registerOutput( cuT( "Diffuse Transmission Colour" ), diffuseTransmissionColour );
		debugOutput.registerOutput( cuT( "Diffuse Transmission Factor" ), diffuseTransmissionFactor );
		debugOutput.registerOutput( cuT( "Dispersion" ), dispersion );
		debugOutput.registerOutput( cuT( "Emissive Colour" ), emissiveColour );
		debugOutput.registerOutput( cuT( "Emissive Factor" ), emissiveFactor );
		debugOutput.registerOutput( cuT( "Opacity" ), opacity );
		debugOutput.registerOutput( cuT( "Occlusion" ), occlusion );
		debugOutput.registerOutput( cuT( "Transmittance" ), transmittance );
	}

	void BlendComponents::setNormal( sdw::Vec3 const v )
	{
		if ( usesDerivativeValues() )
		{
			getMember< shader::DerivVec3 >( "normal" ).value() = v;
		}
		else
		{
			getMember< sdw::Vec3 >( "normal" ) = v;
		}
	}

	void BlendComponents::normalizeNormal()
	{
		if ( usesDerivativeValues() )
		{
			getMember< shader::DerivVec3 >( "normal" ) = normalize( getMember< shader::DerivVec3 >( "normal" ) );
		}
		else
		{
			getMember< sdw::Vec3 >( "normal" ) = normalize( getMember< sdw::Vec3 >( "normal" ) );
		}
	}

	sdw::Vec3 BlendComponents::getRawNormal()const
	{
		return usesDerivativeValues()
			? getMember< shader::DerivVec3 >( "normal" ).value()
			: getMember< sdw::Vec3 >( "normal" );
	}

	sdw::Vec4 BlendComponents::getRawTangent()const
	{
		return usesDerivativeValues()
			? getMember< shader::DerivVec4 >( "tangent" ).value()
			: getMember< sdw::Vec4 >( "tangent" );
	}

	sdw::Vec3 BlendComponents::getRawBitangent()const
	{
		return usesDerivativeValues()
			? getMember< shader::DerivVec3 >( "bitangent" ).value()
			: getMember< sdw::Vec3 >( "bitangent" );
	}

	shader::DerivVec3 BlendComponents::getDerivNormal()const
	{
		return usesDerivativeValues()
			? getMember< shader::DerivVec3 >( "normal" )
			: shader::DerivVec3{ normalize( getMember< sdw::Vec3 >( "normal" ) )
				, dFdx( getMember< sdw::Vec3 >( "normal" ) )
				, dFdy( getMember< sdw::Vec3 >( "normal" ) ) };
	}

	shader::DerivVec4 BlendComponents::getDerivTangent()const
	{
		return usesDerivativeValues()
			? getMember< shader::DerivVec4 >( "tangent" )
			: shader::DerivVec4{ normalize( getMember< sdw::Vec4 >( "tangent" ) )
				, dFdx( getMember< sdw::Vec4 >( "tangent" ) )
				, dFdy( getMember< sdw::Vec4 >( "tangent" ) ) };
	}

	shader::DerivVec3 BlendComponents::getDerivBitangent()const
	{
		return usesDerivativeValues()
			? getMember< shader::DerivVec3 >( "bitangent" )
			: shader::DerivVec3{ normalize( getMember< sdw::Vec3 >( "bitangent" ) )
				, dFdx( getMember< sdw::Vec3 >( "bitangent" ) )
				, dFdy( getMember< sdw::Vec3 >( "bitangent" ) ) };
	}

	sdw::type::BaseStructPtr BlendComponents::makeType( ast::type::TypesCache & cache
		, Materials const & materials
		, bool zeroInit
		, sdw::expr::ExprList & inits )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_BlendComponents" );

		if ( result->empty() )
		{
			BlendComponents::fillType( *result, materials, inits );
		}

		return result;
	}

	sdw::type::BaseStructPtr BlendComponents::makeType( ast::type::TypesCache & cache
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_BlendComponents" );

		if ( result->empty() )
		{
			BlendComponents::fillType( *result, materials, material, surface, clrCot, inits );
		}

		return result;
	}

	ast::type::BaseStructPtr BlendComponents::makeType( ast::type::TypesCache & cache
		, BlendComponents const & rhs )
	{
		return std::static_pointer_cast< ast::type::BaseStruct >( rhs.getType() );
	}

	sdw::Float BlendComponents::computeRoughnessFromGlossiness( sdw::Float const & glossiness )
	{
		return 1.0_f - glossiness;
	}

	sdw::Float BlendComponents::computeGlossinessFromRoughness( sdw::Float const & roughness )
	{
		return 1.0_f - roughness;
	}

	sdw::Float BlendComponents::computeGlossinessFromShininess( sdw::Float const & shininess )
	{
		return shininess / MaxPhongShininess;
	}

	sdw::Float BlendComponents::computeShininessFromGlossiness( sdw::Float const & glossiness )
	{
		return glossiness * MaxPhongShininess;
	}

	void BlendComponents::fillType( ast::type::BaseStruct & type
		, Materials const & materials
		, sdw::expr::ExprList & inits )
	{
		blendcomp::fillType( type );
		blendcomp::fillInit( inits );
		materials.getPassShaders().fillComponents( type, materials, inits );
	}

	void BlendComponents::fillType( ast::type::BaseStruct & type
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )
	{
		blendcomp::fillType( type );
		blendcomp::fillInit( inits );
		materials.getPassShaders().fillComponents( type, materials, material, surface, clrCot, inits );
	}

	void BlendComponents::fillInit( sdw::type::BaseStruct const & components
		, Materials const & materials
		, sdw::expr::ExprList & inits )
	{
		blendcomp::fillInit( inits );
		materials.getPassShaders().fillComponentsInits( components, materials, inits );
	}

	void BlendComponents::fillInit( sdw::type::BaseStruct const & components
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )
	{
		blendcomp::fillInit( inits );
		materials.getPassShaders().fillComponentsInits( components, materials, material, surface, clrCot, inits );
	}

	sdw::expr::ExprPtr BlendComponents::makeInit( Materials const & materials
		, bool zeroInit )
	{
		auto const & writer = *materials.getWriter();
		sdw::expr::ExprList initializers;
		auto type = BlendComponents::makeType( writer.getTypesCache(), materials, zeroInit, initializers );

		if ( initializers.empty() )
		{
			if ( zeroInit )
			{
				return StructInstance::makeInitExpr( type, nullptr );
			}

			BlendComponents::fillInit( *type, materials, initializers );
		}

		return sdw::makeAggrInit( type, castor::move( initializers ) );
	}

	sdw::expr::ExprPtr BlendComponents::makeInit( Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::Vec4 const * clrCot )
	{
		auto const & writer = *materials.getWriter();
		sdw::expr::ExprList initializers;
		auto type = BlendComponents::makeType( writer.getTypesCache(), materials, material, surface, clrCot, initializers );

		if ( initializers.empty() )
		{
			BlendComponents::fillInit( *type, materials, material, surface, clrCot, initializers );
		}

		return sdw::makeAggrInit( type, castor::move( initializers ) );
	}

	//*********************************************************************************************
}
