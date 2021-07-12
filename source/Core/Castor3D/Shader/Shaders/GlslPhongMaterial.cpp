#include "Castor3D/Shader/Shaders/GlslPhongMaterial.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//***********************************************************************************************

	PhongLightMaterial::PhongLightMaterial( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, albedo{ getMember< sdw::Vec3 >( "albedo" ) }
		, ambient{ getMember< sdw::Float >( "ambient" ) }
		, specular{ getMember< sdw::Vec3 >( "specular" ) }
		, shininess{ getMember< sdw::Float >( "shininess" ) }
	{
	}

	PhongLightMaterial & PhongLightMaterial::operator=( PhongLightMaterial const & rhs )
	{
		StructInstance::operator=( rhs );
		return *this;
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
		albedo = pow( max( material.colourDiv.rgb(), vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( material.gamma ) );
		specular = material.specDiv.rgb();
		ambient = material.colourDiv.a();
		shininess = material.specDiv.a();
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
		return LightingModel::computeMetalness( albedo, specular );
	}

	sdw::Float PhongLightMaterial::getRoughness()const
	{
		return LightingModel::computeRoughness( LightingModel::computeGlossiness( shininess ) );
	}

	ast::type::StructPtr PhongLightMaterial::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, "PhongLightMaterial" );

		if ( result->empty() )
		{
			result->declMember( "albedo", ast::type::Kind::eVec3F );
			result->declMember( "specular", ast::type::Kind::eVec3F );
			result->declMember( "ambient", ast::type::Kind::eFloat );
			result->declMember( "shininess", ast::type::Kind::eFloat );
		}

		return result;
	}

	//*********************************************************************************************
}
