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
		, edgeWidth{ specific.g() }
		, stepsCount{ specific.b() }
	{
	}

	void ToonPhongLightMaterial::create( sdw::Vec3 const & albedo
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, c3d::Material const & material )
	{
		create( albedo
			, data3
			, data2
			, material.colourDiv.a() );
		smoothBand = material.specific.r();
		edgeWidth = material.specific.g();
		stepsCount = material.specific.b();
	}

	void ToonPhongLightMaterial::create( sdw::Vec3 const & albedo
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, sdw::Float const & ambient )
	{
		this->albedo = albedo;
		this->specular = data3.rgb();
		this->ambient = ambient;
		this->shininess = data2.a();
	}

	void ToonPhongLightMaterial::create( c3d::Material const & material )
	{
		albedo = pow( max( material.colourDiv.rgb(), vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( material.gamma ) );
		ambient = material.colourDiv.a();
		specular = material.specDiv.rgb();
		shininess = material.specDiv.a();
		smoothBand = material.specific.r();
		edgeWidth = material.specific.g();
		stepsCount = material.specific.b();
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

	sdw::Float ToonPhongLightMaterial::transform( sdw::Float const & value )const
	{
		return floor( value * stepsCount ) / stepsCount;
	}

	sdw::Vec3 ToonPhongLightMaterial::transform( sdw::Vec3 const & value )const
	{
		return vec3( transform( value.x() )
			, transform( value.y() ) 
			, transform( value.z() ) );
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
		, edgeWidth{ specific.g() }
		, stepsCount{ specific.b() }
	{
	}

	void ToonPbrLightMaterial::create( sdw::Vec3 const & albedo
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, c3d::Material const & material )
	{
		create( albedo
			, data3
			, data2
			, material.colourDiv.a() );
		smoothBand = material.specific.r();
		edgeWidth = material.specific.g();
		stepsCount = material.specific.b();
	}

	void ToonPbrLightMaterial::create( sdw::Vec3 const & albedo
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, sdw::Float const & ambient )
	{
		this->albedo = albedo;
		this->specular = data3.rgb();
		this->metalness = data3.a();
		this->roughness = data2.a();
	}

	void ToonPbrLightMaterial::create( c3d::Material const & material )
	{
		specific = material.specific;
		albedo = material.colourDiv.rgb();
		roughness = material.colourDiv.a();
		specular = material.specDiv.rgb();
		metalness = material.specDiv.a();
		smoothBand = material.specific.r();
		edgeWidth = material.specific.g();
		stepsCount = material.specific.b();
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

	sdw::Float ToonPbrLightMaterial::transform( sdw::Float const & value )const
	{
		return floor( value * stepsCount ) / stepsCount;
	}

	sdw::Vec3 ToonPbrLightMaterial::transform( sdw::Vec3 const & value )const
	{
		return vec3( transform( value.x() )
			, transform( value.y() )
			, transform( value.z() ) );
	}

	//*********************************************************************************************
}
