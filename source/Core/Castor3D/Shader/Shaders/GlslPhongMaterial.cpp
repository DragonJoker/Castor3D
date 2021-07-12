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

	void PhongLightMaterial::create( Material const & material )
	{
		albedo = pow( max( material.colourDiv.rgb(), vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( material.gamma ) );
		specular = material.specDiv.rgb();
		ambient = material.colourDiv.a();
		shininess = material.specDiv.a();
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
