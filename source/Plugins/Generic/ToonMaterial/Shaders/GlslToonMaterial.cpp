#include "ToonMaterial/Shaders/GlslToonMaterial.hpp"

#include "ToonMaterial/ToonPass.hpp"

#include <Castor3D/Limits.hpp>

#include <ShaderWriter/Source.hpp>

namespace toon::shader
{
	//***********************************************************************************************

	ToonPhongLightMaterial::ToonPhongLightMaterial( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: c3d::LightMaterial{ writer, std::move( expr ), enabled }
		, ambient{ albDiv }
		, shininess{ spcDiv }
		, smoothBand{ specific.r() }
	{
	}

	void ToonPhongLightMaterial::create( sdw::Vec3 const & palbedo
		, sdw::Vec4 const & spcMtl
		, sdw::Vec4 const & colRgh
		, c3d::Material const & material )
	{
		if ( isEnabled() )
		{
			create( palbedo
				, spcMtl
				, colRgh
				, material.colourDiv().a() );
			edgeWidth = material.edgeWidth();
			depthFactor = material.depthFactor();
			normalFactor = material.normalFactor();
			objectFactor = material.objectFactor();
			edgeColour = material.edgeColour();
			specific = material.specific();
			smoothBand = specific.r();
		}
	}

	void ToonPhongLightMaterial::create( sdw::Vec3 const & palbedo
		, sdw::Vec4 const & spcMtl
		, sdw::Vec4 const & colRgh
		, sdw::Float const & pambient )
	{
		if ( isEnabled() )
		{
			albedo = palbedo;
			specular = spcMtl.rgb();
			ambient = pambient;
			shininess = colRgh.a();
		}
	}

	void ToonPhongLightMaterial::create( sdw::Vec3 const & vtxColour
		, c3d::Materials const & materials
		, c3d::Material const & material )
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

	void ToonPhongLightMaterial::output( sdw::Vec4 & outColRgh, sdw::Vec4 & outSpcMtl )const
	{
		outColRgh = vec4( albedo, shininess );
		outSpcMtl = vec4( specular, 0.0_f );
	}

	sdw::Vec3 ToonPhongLightMaterial::getAmbient( sdw::Vec3 const & ambientLight )const
	{
		return ambientLight * albedo;
	}

	void ToonPhongLightMaterial::adjustDirectSpecular( sdw::Vec3 & directSpecular )const
	{
		directSpecular *= specular;
	}

	sdw::Vec3 ToonPhongLightMaterial::getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const
	{
		return indirectAmbient * ambient;
	}

	sdw::Float ToonPhongLightMaterial::getMetalness()const
	{
		return c3d::LightMaterial::computeMetalness( albedo, specular );
	}

	sdw::Float ToonPhongLightMaterial::getRoughness()const
	{
		return c3d::LightMaterial::computeRoughness( ToonPhongLightMaterial::computeGlossiness( shininess ) );
	}

	sdw::Float ToonPhongLightMaterial::computeGlossiness( sdw::Float const & shininess )
	{
		return shininess / castor3d::MaxPhongShininess;
	}

	sdw::Float ToonPhongLightMaterial::computeShininess( sdw::Float const & glossiness )
	{
		return glossiness * castor3d::MaxPhongShininess;
	}

	void ToonPhongLightMaterial::doBlendWith( c3d::LightMaterial const & material
		, sdw::Float const & weight )
	{
		auto & toonMaterial = static_cast< ToonPhongLightMaterial const & >( material );
		edgeWidth = lighting::interpolate( edgeWidth, toonMaterial.edgeWidth, weight );
		depthFactor = lighting::interpolate( depthFactor, toonMaterial.depthFactor, weight );
		normalFactor = lighting::interpolate( normalFactor, toonMaterial.normalFactor, weight );
		objectFactor = lighting::interpolate( objectFactor, toonMaterial.objectFactor, weight );
		edgeColour = lighting::interpolate( edgeColour, toonMaterial.edgeColour, weight );
		smoothBand = lighting::interpolate( smoothBand, toonMaterial.smoothBand, weight );
	}

	//*********************************************************************************************

	ToonPbrLightMaterial::ToonPbrLightMaterial( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: LightMaterial{ writer, std::move( expr ), enabled }
		, roughness{ albDiv }
		, metalness{ spcDiv }
		, smoothBand{ specific.r() }
	{
	}

	void ToonPbrLightMaterial::create( sdw::Vec3 const & palbedo
		, sdw::Vec4 const & spcMtl
		, sdw::Vec4 const & colRgh
		, c3d::Material const & material )
	{
		if ( isEnabled() )
		{
			create( palbedo
				, spcMtl
				, colRgh
				, 0.0_f );
			edgeWidth = material.edgeWidth();
			depthFactor = material.depthFactor();
			normalFactor = material.normalFactor();
			objectFactor = material.objectFactor();
			edgeColour = material.edgeColour();
			specific = material.specific();
			smoothBand = specific.r();
		}
	}

	void ToonPbrLightMaterial::create( sdw::Vec3 const & palbedo
		, sdw::Vec4 const & spcMtl
		, sdw::Vec4 const & colRgh
		, sdw::Float const & ambient )
	{
		if ( isEnabled() )
		{
			albedo = palbedo;
			specular = spcMtl.rgb();
			metalness = spcMtl.a();
			roughness = colRgh.a();
		}
	}

	void ToonPbrLightMaterial::create( sdw::Vec3 const & vtxColour
		, c3d::Materials const & materials
		, c3d::Material const & material )
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

	void ToonPbrLightMaterial::output( sdw::Vec4 & outColRgh, sdw::Vec4 & outSpcMtl )const
	{
		outColRgh = vec4( albedo, roughness );
		outSpcMtl = vec4( specular, metalness );
	}

	sdw::Vec3 ToonPbrLightMaterial::getAmbient( sdw::Vec3 const & ambientLight )const
	{
		return ambientLight;
	}

	void ToonPbrLightMaterial::adjustDirectSpecular( sdw::Vec3 & directSpecular )const
	{
	}

	sdw::Vec3 ToonPbrLightMaterial::getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const
	{
		return indirectAmbient;
	}

	sdw::Float ToonPbrLightMaterial::getMetalness()const
	{
		return metalness;
	}

	sdw::Float ToonPbrLightMaterial::getRoughness()const
	{
		return roughness;
	}

	void ToonPbrLightMaterial::doBlendWith( c3d::LightMaterial const & material
		, sdw::Float const & weight )
	{
		auto & toonMaterial = static_cast< ToonPbrLightMaterial const & >( material );
		edgeWidth = lighting::interpolate( edgeWidth, toonMaterial.edgeWidth, weight );
		depthFactor = lighting::interpolate( depthFactor, toonMaterial.depthFactor, weight );
		normalFactor = lighting::interpolate( normalFactor, toonMaterial.normalFactor, weight );
		objectFactor = lighting::interpolate( objectFactor, toonMaterial.objectFactor, weight );
		edgeColour = lighting::interpolate( edgeColour, toonMaterial.edgeColour, weight );
		smoothBand = lighting::interpolate( smoothBand, toonMaterial.smoothBand, weight );
	}

	//*********************************************************************************************
}
