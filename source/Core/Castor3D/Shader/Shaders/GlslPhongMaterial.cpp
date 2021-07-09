#include "Castor3D/Shader/Shaders/GlslPhongMaterial.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//***********************************************************************************************

	PhongLightMaterial::PhongLightMaterial( sdw::ShaderWriter & writer )
		: PhongLightMaterial{ { writer, "specular" }, { writer, "shininess" } }
	{
	}

	PhongLightMaterial::PhongLightMaterial( sdw::InVec3 const & specular
		, sdw::InFloat const & shininess )
		: specular{ specular }
		, shininess{ shininess }
		, m_expr{ sdw::expr::makeComma( makeExpr( this->specular ), makeExpr( this->shininess ) ) }
	{
	}

	ast::expr::Expr * PhongLightMaterial::getExpr()const
	{
		return m_expr.get();
	}

	sdw::ShaderWriter * PhongLightMaterial::getWriter()const
	{
		return sdw::findWriter( specular, shininess );
	}

	void PhongLightMaterial::setVar( ast::var::VariableList::const_iterator & var )
	{
		specular.setVar( var );
		shininess.setVar( var );
	}

	//***********************************************************************************************

	ast::expr::ExprList makeFnArg( sdw::ShaderWriter & writer
		, PhongLightMaterial const & value )
	{
		ast::expr::ExprList result;
		auto args = sdw::makeFnArg( writer, value.specular );

		for ( auto & expr : args )
		{
			result.emplace_back( std::move( expr ) );
		}

		args = sdw::makeFnArg( writer, value.shininess );

		for ( auto & expr : args )
		{
			result.emplace_back( std::move( expr ) );
		}

		return result;
	}

	//*****************************************************************************************

	PhongMaterial::PhongMaterial( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: BaseMaterial{ writer, std::move( expr ), enabled }
		, m_diffAmb{ getMember< sdw::Vec4 >( "diffAmb" ) }
		, m_specShin{ getMember< sdw::Vec4 >( "specShin" ) }
		, diffuse{ m_diffAmb.xyz() }
		, ambient{ m_diffAmb.w() }
		, specular{ m_specShin.xyz() }
		, shininess{ m_specShin.w() }
	{
	}

	sdw::Vec3 PhongMaterial::colour()const
	{
		return diffuse;
	}

	PhongLightMaterial PhongMaterial::getLightMaterial()const
	{
		return getLightMaterial( m_specShin.xyz()
			, m_specShin.w() );
	}

	PhongLightMaterial PhongMaterial::getLightMaterial( sdw::Vec3 specular
		, sdw::Float shininess )
	{
		return { std::move( specular )
			, std::move( shininess ) };
	}

	std::unique_ptr< sdw::Struct > PhongMaterial::declare( sdw::ShaderWriter & writer )
	{
		return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
	}

	ast::type::StructPtr PhongMaterial::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "LegacyMaterial" );

		if ( result->empty() )
		{
			result->declMember( "diffAmb", ast::type::Kind::eVec4F );
			result->declMember( "specShin", ast::type::Kind::eVec4F );
			result->declMember( "common", ast::type::Kind::eVec4F );
			result->declMember( "opacity", ast::type::Kind::eVec4F );
			result->declMember( "reflRefr", ast::type::Kind::eVec4F );
			result->declMember( "sssInfo", ast::type::Kind::eVec4F );
			result->declMember( "transmittanceProfile", ast::type::Kind::eVec4F, MaxTransmittanceProfileSize );
		}

		return result;
	}

	//*********************************************************************************************

	PhongMaterials::PhongMaterials( sdw::ShaderWriter & writer )
		: Materials{ writer }
	{
	}

	void PhongMaterials::declare( bool hasSsbo
		, uint32_t binding
		, uint32_t set )
	{
		m_type = PhongMaterial::declare( m_writer );

		if ( hasSsbo )
		{
			m_ssbo = std::make_unique< sdw::ArraySsboT< PhongMaterial > >( m_writer
				, PassBufferName
				, m_type->getType()
				, binding
				, set
				, true );
		}
		else
		{
			auto c3d_materials = m_writer.declSampledImage< FImgBufferRgba32 >( "c3d_materials"
				, binding
				, set );
			m_getMaterial = m_writer.implementFunction< PhongMaterial >( "getMaterial"
				, [this, &c3d_materials]( sdw::UInt const & index )
				{
					auto result = m_writer.declLocale< PhongMaterial >( "result"
						, *m_type );
					auto offset = m_writer.declLocale( "offset"
						, m_writer.cast< sdw::Int >( index ) * sdw::Int( MaxMaterialComponentsCount ) );
					result.m_diffAmb = c3d_materials.fetch( sdw::Int{ offset++ } );
					result.m_specShin = c3d_materials.fetch( sdw::Int{ offset++ } );
					doFetch( result, c3d_materials, offset );
					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "index" } );
		}
	}

	PhongMaterial PhongMaterials::getMaterial( sdw::UInt const & index )const
	{
		if ( m_ssbo )
		{
			return ( *m_ssbo )[index - 1_u];
		}
		else
		{
			return m_getMaterial( index - 1_u );
		}
	}

	BaseMaterialUPtr PhongMaterials::getBaseMaterial( sdw::UInt const & index )const
	{
		auto material = m_writer.declLocale( "material"
			, getMaterial( index ) );
		return std::make_unique< PhongMaterial >( material );
	}

	//*********************************************************************************************
}
