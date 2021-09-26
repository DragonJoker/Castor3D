#include "ToonMaterial/Shaders/GlslToonMaterial.hpp"

#include "ToonMaterial/ToonPass.hpp"

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
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, c3d::Material const & material )
	{
		create( palbedo
			, data3
			, data2
			, material.colourDiv.a() );
		edgeFactors = material.edgeFactors;
		edgeColour = material.edgeColour;
		specific = material.specific;
		edgeWidth = material.edgeWidth;
		depthFactor = material.depthFactor;
		normalFactor = material.normalFactor;
		objectFactor = material.objectFactor;
		smoothBand = material.specific.r();
	}

	void ToonPhongLightMaterial::create( sdw::Vec3 const & palbedo
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, sdw::Float const & pambient )
	{
		albedo = palbedo;
		specular = data3.rgb();
		ambient = pambient;
		shininess = data2.a();
	}

	void ToonPhongLightMaterial::create( c3d::Material const & material )
	{
		create( pow( max( material.colourDiv.rgb(), vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( material.gamma ) )
			, material.specDiv
			, material.specDiv
			, material );
	}

	void ToonPhongLightMaterial::output( sdw::Vec4 & outData2, sdw::Vec4 & outData3 )const
	{
		outData2 = vec4( albedo, shininess );
		outData3 = vec4( specular, 0.0_f );
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
		return shininess / ToonPhongPass::MaxShininess;
	}

	sdw::Float ToonPhongLightMaterial::computeShininess( sdw::Float const & glossiness )
	{
		return glossiness * ToonPhongPass::MaxShininess;
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
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, c3d::Material const & material )
	{
		create( palbedo
			, data3
			, data2
			, 0.0_f );
		edgeFactors = material.edgeFactors;
		edgeColour = material.edgeColour;
		specific = material.specific;
		edgeWidth = material.edgeWidth;
		depthFactor = material.depthFactor;
		normalFactor = material.normalFactor;
		objectFactor = material.objectFactor;
		smoothBand = material.specific.r();
	}

	void ToonPbrLightMaterial::create( sdw::Vec3 const & palbedo
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, sdw::Float const & ambient )
	{
		albedo = palbedo;
		specular = data3.rgb();
		metalness = data3.a();
		roughness = data2.a();
	}

	void ToonPbrLightMaterial::create( c3d::Material const & material )
	{
		create( material.colourDiv.rgb()
			, material.specDiv
			, material.colourDiv
			, material );
	}

	void ToonPbrLightMaterial::output( sdw::Vec4 & outData2, sdw::Vec4 & outData3 )const
	{
		outData2 = vec4( albedo, roughness );
		outData3 = vec4( specular, metalness );
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

	//*********************************************************************************************
}
