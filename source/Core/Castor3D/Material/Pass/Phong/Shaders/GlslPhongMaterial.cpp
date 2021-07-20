#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongMaterial.hpp"

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

	void PhongLightMaterial::create( sdw::Vec3 const & albedo
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, Material const & material )
	{
		create( albedo
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
	}

	void PhongLightMaterial::create( sdw::Vec3 const & albedo
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, sdw::Float const & ambient )
	{
		this->albedo = albedo;
		this->specular = data3.rgb();
		this->ambient = ambient;
		this->shininess = data2.a();
	}

	void PhongLightMaterial::create( Material const & material )
	{
		create( pow( max( material.colourDiv.rgb(), vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( material.gamma ) )
			, material.specDiv
			, material.specDiv
			, material );
	}

	void PhongLightMaterial::output( sdw::Vec4 & outData2, sdw::Vec4 & outData3 )const
	{
		outData2 = vec4( albedo, shininess );
		outData3 = vec4( specular, 0.0_f );
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
		return shininess / PhongPass::MaxShininess;
	}

	sdw::Float PhongLightMaterial::computeShininess( sdw::Float const & glossiness )
	{
		return glossiness * PhongPass::MaxShininess;
	}

	//*********************************************************************************************
}
