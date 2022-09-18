#include "Castor3D/Material/Pass/PBR/Shaders/GlslPbrMaterial.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	PbrLightMaterial::PbrLightMaterial( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: LightMaterial{ writer, std::move( expr ), enabled }
		, roughness{ albDiv }
		, metalness{ spcDiv }
	{
	}

	void PbrLightMaterial::create( sdw::Vec3 const & newAlbedo
		, sdw::Vec4 const & spcMtl
		, sdw::Vec4 const & colRgh
		, Materials const & materials
		, Material const & material )
	{
		if ( isEnabled() )
		{
			create( newAlbedo
				, spcMtl
				, colRgh
				, 0.0_f );
		}
	}

	void PbrLightMaterial::create( sdw::Vec3 const & newAlbedo
		, sdw::Vec4 const & spcMtl
		, sdw::Vec4 const & colRgh
		, sdw::Float const & ambient )
	{
		if ( isEnabled() )
		{
			albedo = newAlbedo;
			specular = spcMtl.rgb();
			metalness = spcMtl.a();
			roughness = colRgh.a();
		}
	}

	void PbrLightMaterial::create( sdw::Vec3 const & vtxColour
		, Materials const & materials
		, Material const & material )
	{
		if ( vtxColour.isEnabled() )
		{
			create( material.colour() * vtxColour
				, material.specDiv()
				, material.colourDiv()
				, materials
				, material );
		}
		else
		{
			create( material.colour()
				, material.specDiv()
				, material.colourDiv()
				, materials
				, material );
		}
	}

	void PbrLightMaterial::output( sdw::Vec4 & outColRgh, sdw::Vec4 & outSpcMtl )const
	{
		outColRgh = vec4( albedo, roughness );
		outSpcMtl = vec4( specular, metalness );
	}

	sdw::Vec3 PbrLightMaterial::getAmbient( sdw::Vec3 const & ambientLight )const
	{
		return ambientLight;
	}

	void PbrLightMaterial::adjustDirectSpecular( sdw::Vec3 & directSpecular )const
	{
	}

	sdw::Vec3 PbrLightMaterial::getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const
	{
		return indirectAmbient;
	}

	sdw::Float PbrLightMaterial::getMetalness()const
	{
		return metalness;
	}

	sdw::Float PbrLightMaterial::getRoughness()const
	{
		return roughness;
	}
}
