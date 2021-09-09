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
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, Material const & material )
	{
		create( newAlbedo
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
	}

	void PbrLightMaterial::create( sdw::Vec3 const & newAlbedo
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, sdw::Float const & ambient )
	{
		albedo = newAlbedo;
		specular = data3.rgb();
		metalness = data3.a();
		roughness = data2.a();
	}

	void PbrLightMaterial::create( Material const & material )
	{
		create( material.colourDiv.rgb()
			, material.specDiv
			, material.colourDiv
			, material );
	}

	void PbrLightMaterial::output( sdw::Vec4 & outData2, sdw::Vec4 & outData3 )const
	{
		outData2 = vec4( albedo, roughness );
		outData3 = vec4( specular, metalness );
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
