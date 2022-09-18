#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongMaterial.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/Phong/PhongPass.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//***********************************************************************************************

	PhongLightMaterial::PhongLightMaterial( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: LightMaterial{ writer, std::move( expr ), enabled }
		, ambient{ albDiv }
		, shininess{ spcDiv }
	{
	}

	void PhongLightMaterial::create( sdw::Vec3 const & newAlbedo
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
				, material.colourDiv().a() );
		}
	}

	void PhongLightMaterial::create( sdw::Vec3 const & newAlbedo
		, sdw::Vec4 const & spcMtl
		, sdw::Vec4 const & colRgh
		, sdw::Float const & newAmbient )
	{
		if ( isEnabled() )
		{
			albedo = newAlbedo;
			specular = spcMtl.rgb();
			ambient = newAmbient;
			shininess = colRgh.a();
		}
	}

	void PhongLightMaterial::create( sdw::Vec3 const & vtxColour
		, Materials const & materials
		, Material const & material )
	{
		if ( vtxColour.isEnabled() )
		{
			create( material.colour() * vtxColour
				, material.specDiv()
				, material.specDiv()
				, materials
				, material );
		}
		else
		{
			create( material.colour()
				, material.specDiv()
				, material.specDiv()
				, materials
				, material );
		}
	}

	void PhongLightMaterial::output( sdw::Vec4 & outColRgh, sdw::Vec4 & outSpcMtl )const
	{
		outColRgh = vec4( albedo, shininess );
		outSpcMtl = vec4( specular, 0.0_f );
	}

	sdw::Vec3 PhongLightMaterial::getAmbient( sdw::Vec3 const & ambientLight )const
	{
		return ambientLight * albedo;
	}

	void PhongLightMaterial::adjustDirectSpecular( sdw::Vec3 & directSpecular )const
	{
		directSpecular *= specular;
	}

	sdw::Vec3 PhongLightMaterial::getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const
	{
		return indirectAmbient * ambient;
	}

	sdw::Float PhongLightMaterial::getMetalness()const
	{
		return LightMaterial::computeMetalness( albedo, specular );
	}

	sdw::Float PhongLightMaterial::getRoughness()const
	{
		return LightMaterial::computeRoughness( PhongLightMaterial::computeGlossiness( shininess ) );
	}

	sdw::Float PhongLightMaterial::computeGlossiness( sdw::Float const & shininess )
	{
		return shininess / MaxPhongShininess;
	}

	sdw::Float PhongLightMaterial::computeShininess( sdw::Float const & glossiness )
	{
		return glossiness * MaxPhongShininess;
	}

	//*********************************************************************************************
}
