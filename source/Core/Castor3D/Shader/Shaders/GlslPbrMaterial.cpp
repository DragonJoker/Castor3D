#include "Castor3D/Shader/Shaders/GlslPbrMaterial.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	PbrLightMaterial::PbrLightMaterial( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, albedo{ getMember< sdw::Vec3 >( "albedo" ) }
		, specular{ getMember< sdw::Vec3 >( "specular" ) }
		, metalness{ getMember< sdw::Float >( "metalness" ) }
		, roughness{ getMember< sdw::Float >( "roughness" ) }
	{
	}

	PbrLightMaterial & PbrLightMaterial::operator=( PbrLightMaterial const & rhs )
	{
		StructInstance::operator=( rhs );
		return *this;
	}

	void PbrLightMaterial::create( sdw::Vec3 const & albedo
		, sdw::Vec4 const & data3
		, sdw::Vec4 const & data2
		, sdw::Float const & ambient )
	{
		this->albedo = albedo;
		this->specular = data3.rgb();
		this->metalness = data3.a();
		this->roughness = data2.a();
	}

	void PbrLightMaterial::create( Material const & material )
	{
		albedo = material.colourDiv.rgb();
		roughness = material.colourDiv.a();
		specular = material.specDiv.rgb();
		metalness = material.specDiv.a();
	}

	void PbrLightMaterial::output( sdw::Vec4 & outData2, sdw::Vec4 & outData3 )
	{
		outData2 = vec4( albedo, roughness );
		outData3 = vec4( specular, metalness );
	}

	sdw::Vec3 PbrLightMaterial::getAmbient( sdw::Vec3 const & ambientLight )
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

	ast::type::StructPtr PbrLightMaterial::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, "PbrLightMaterial" );

		if ( result->empty() )
		{
			result->declMember( "albedo", ast::type::Kind::eVec3F );
			result->declMember( "specular", ast::type::Kind::eVec3F );
			result->declMember( "metalness", ast::type::Kind::eFloat );
			result->declMember( "roughness", ast::type::Kind::eFloat );
		}

		return result;
	}
}
