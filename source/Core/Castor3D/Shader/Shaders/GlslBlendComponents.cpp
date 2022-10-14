#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	BlendComponents::BlendComponents( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, normal{ getMember( "normal", vec3( 0.0_f ) ) }
		, colour{ getMember( "colour", vec3( 0.0_f ) ) }
		, emissive{ getMember( "emissive", vec3( 0.0_f ) ) }
		, specular{ getMember( "specular", vec3( 0.0_f ) ) }
		, transmission{ getMember( "transmission", vec3( 1.0_f ) ) }
		, opacity{ getMember( "opacity", 1.0_f ) }
		, bwAccumulationOperator{ getMember( "bwAccumulationOperator", 0_u ) }
		, alphaRef{ getMember( "alphaRef", 0.95_f ) }
		, occlusion{ getMember( "occlusion", 1.0_f ) }
		, transmittance{ getMember( "transmittance", 1.0_f ) }
		, refractionRatio{ getMember( "refractionRatio", 0.0_f ) }
		, hasReflection{ getMember( "hasReflection", 0_u ) }
		, hasRefraction{ getMember( "hasRefraction", 0_u ) }
		, metalness{ getMember( "metalness", 0.0_f ) }
		, roughness{ getMember( "roughness", 1.0_f ) }
		, shininess{ computeShininessFromRoughness( roughness ) }
	{
	}
	
	BlendComponents::BlendComponents( Materials const & materials
		, Material const & material
		, SurfaceBase const & surface )
		: BlendComponents{ *materials.getWriter()
			, makeInit( materials, material, surface )
			, true }
	{
	}

	BlendComponents::BlendComponents( Materials const & materials
		, bool zeroInit )
		: BlendComponents{ *materials.getWriter()
			, makeInit( materials, zeroInit )
			, true }
	{
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
		, sdw::expr::ExprList & inits )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_BlendComponents" );

		if ( result->empty() )
		{
			BlendComponents::fillType( *result, materials, material, surface, inits );
		}

		return result;
	}

	ast::type::BaseStructPtr BlendComponents::makeType( ast::type::TypesCache & cache
		, BlendComponents const & rhs )
	{
		return std::static_pointer_cast< ast::type::BaseStruct >( rhs.getType() );
	}

	sdw::Vec3 BlendComponents::computeF0( sdw::Vec3 const & albedo
		, sdw::Float const & metalness )
	{
		return mix( vec3( 0.04_f ), albedo, vec3( metalness ) );
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
		materials.getPassShaders().fillComponents( type, materials, inits );
	}

	void BlendComponents::fillType( ast::type::BaseStruct & type
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::expr::ExprList & inits )
	{
		materials.getPassShaders().fillComponents( type, materials, material, surface, inits );
	}

	void BlendComponents::fillInit( sdw::type::BaseStruct const & components
		, Materials const & materials
		, sdw::expr::ExprList & inits )
	{
		materials.getPassShaders().fillComponentsInits( components, materials, inits );
	}

	void BlendComponents::fillInit( sdw::type::BaseStruct const & components
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::expr::ExprList & inits )
	{
		materials.getPassShaders().fillComponentsInits( components, materials, material, surface, inits );
	}

	sdw::expr::ExprPtr BlendComponents::makeInit( Materials const & materials
		, bool zeroInit )
	{
		auto & writer = *materials.getWriter();
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

		return sdw::makeAggrInit( type, std::move( initializers ) );
	}

	sdw::expr::ExprPtr BlendComponents::makeInit( Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface )
	{
		auto & writer = *materials.getWriter();
		sdw::expr::ExprList initializers;
		auto type = BlendComponents::makeType( writer.getTypesCache(), materials, material, surface, initializers );

		if ( initializers.empty() )
		{
			BlendComponents::fillInit( *type, materials, material, surface, initializers );
		}

		return sdw::makeAggrInit( type, std::move( initializers ) );
	}

	//*********************************************************************************************
}
