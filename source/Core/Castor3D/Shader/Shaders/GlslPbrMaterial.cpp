#include "Castor3D/Shader/Shaders/GlslPbrMaterial.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
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

	PbrLightMaterial::PbrLightMaterial( sdw::ShaderWriter & writer )
		: PbrLightMaterial{ { writer, "specular" }, { writer, "metalness" }, { writer, "roughness" } }
	{
	}

	PbrLightMaterial::PbrLightMaterial( sdw::InOutVec3 const & specular
		, sdw::InOutFloat const & metalness
		, sdw::InOutFloat const & roughness )
		: specular{ specular }
		, metalness{ metalness }
		, roughness{ roughness }
		, m_expr{ sdw::expr::makeComma( makeExpr( this->specular )
			, sdw::expr::makeComma( makeExpr( this->metalness )
				, makeExpr( this->roughness ) ) ) }
	{
	}

	ast::expr::Expr * PbrLightMaterial::getExpr()const
	{
		return m_expr.get();
	}

	sdw::ShaderWriter * PbrLightMaterial::getWriter()const
	{
		return sdw::findWriter( specular, metalness, roughness );
	}

	void PbrLightMaterial::setVar( ast::var::VariableList::const_iterator & var )
	{
		specular.setVar( var );
		metalness.setVar( var );
		roughness.setVar( var );
	}

	//***********************************************************************************************

	ast::expr::ExprList makeFnArg( sdw::ShaderWriter & writer
		, PbrLightMaterial const & value )
	{
		ast::expr::ExprList result;
		auto args = sdw::makeFnArg( writer, value.specular );

		for ( auto & expr : args )
		{
			result.emplace_back( std::move( expr ) );
		}

		args = sdw::makeFnArg( writer, value.metalness );

		for ( auto & expr : args )
		{
			result.emplace_back( std::move( expr ) );
		}

		args = sdw::makeFnArg( writer, value.roughness );

		for ( auto & expr : args )
		{
			result.emplace_back( std::move( expr ) );
		}

		return result;
	}

	//*****************************************************************************************

	MetallicRoughnessMaterial::MetallicRoughnessMaterial( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: BaseMaterial{ writer, std::move( expr ), enabled }
		, m_albRough{ getMember< sdw::Vec4 >( "albRough" ) }
		, m_metDiv{ getMember< sdw::Vec4 >( "metDiv" ) }
		, albedo{ m_albRough.xyz() }
		, roughness{ m_albRough.w() }
		, metalness{ m_metDiv.x() }
	{
	}

	sdw::Vec3 MetallicRoughnessMaterial::colour()const
	{
		return m_albRough.rgb();
	}

	std::unique_ptr< sdw::Struct > MetallicRoughnessMaterial::declare( sdw::ShaderWriter & writer )
	{
		return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
	}

	PbrLightMaterial MetallicRoughnessMaterial::getLightMaterial()const
	{
		return { LightingModel::computeF0( albedo, metalness )
			, std::move( metalness )
			, std::move( roughness ) };
	}

	PbrLightMaterial MetallicRoughnessMaterial::getLightMaterial( sdw::Vec3 albedo
		, sdw::Float metalness
		, sdw::Float roughness )
	{
		auto & writer = *sdw::findWriter( albedo, metalness, roughness );
		auto specular = writer.declLocale( "specular"
			, LightingModel::computeF0( albedo, metalness ) );
		return { std::move( specular )
			, std::move( metalness )
			, std::move( roughness ) };
	}

	ast::type::StructPtr MetallicRoughnessMaterial::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "MetallicRoughnessMaterial" );

		if ( result->empty() )
		{
			result->declMember( "albRough", ast::type::Kind::eVec4F );
			result->declMember( "metDiv", ast::type::Kind::eVec4F );
			result->declMember( "common", ast::type::Kind::eVec4F );
			result->declMember( "opacity", ast::type::Kind::eVec4F );
			result->declMember( "reflRefr", ast::type::Kind::eVec4F );
			result->declMember( "sssInfo", ast::type::Kind::eVec4F );
			result->declMember( "transmittanceProfile", ast::type::Kind::eVec4F, MaxTransmittanceProfileSize );
		}

		return result;
	}

	//*****************************************************************************************

	SpecularGlossinessMaterial::SpecularGlossinessMaterial( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: BaseMaterial{ writer, std::move( expr ), enabled }
		, m_diffDiv{ getMember< sdw::Vec4 >( "diffDiv" ) }
		, m_specGloss{ getMember< sdw::Vec4 >( "specGloss" ) }
		, albedo{ m_diffDiv.xyz() }
		, specular{ m_specGloss.xyz() }
		, glossiness{ m_specGloss.w() }
	{
	}

	sdw::Vec3 SpecularGlossinessMaterial::colour()const
	{
		return albedo;
	}

	std::unique_ptr< sdw::Struct > SpecularGlossinessMaterial::declare( sdw::ShaderWriter & writer )
	{
		return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
	}

	PbrLightMaterial SpecularGlossinessMaterial::getLightMaterial()const
	{
		return { std::move( specular )
			, LightingModel::computeMetalness( albedo, specular )
			, LightingModel::computeRoughness( glossiness ) };
	}

	PbrLightMaterial SpecularGlossinessMaterial::getLightMaterial( sdw::Vec3 albedo
		, sdw::Vec3 specular
		, sdw::Float glossiness )
	{
		auto & writer = *sdw::findWriter( albedo, specular, glossiness );
		auto roughness = writer.declLocale( "roughness"
			, 1.0_f - glossiness );
		auto metalness = writer.declLocale( "metalness"
			, LightingModel::computeMetalness( albedo, specular ) );
		return { std::move( specular )
			, LightingModel::computeMetalness( albedo, specular )
			, LightingModel::computeRoughness( glossiness ) };
	}

	ast::type::StructPtr SpecularGlossinessMaterial::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "SpecularGlossinessMaterial" );

		if ( result->empty() )
		{
			result->declMember( "diffDiv", ast::type::Kind::eVec4F );
			result->declMember( "specGloss", ast::type::Kind::eVec4F );
			result->declMember( "common", ast::type::Kind::eVec4F );
			result->declMember( "opacity", ast::type::Kind::eVec4F );
			result->declMember( "reflRefr", ast::type::Kind::eVec4F );
			result->declMember( "sssInfo", ast::type::Kind::eVec4F );
			result->declMember( "transmittanceProfile", ast::type::Kind::eVec4F, MaxTransmittanceProfileSize );
		}

		return result;
	}

	//*********************************************************************************************

	PbrMRMaterials::PbrMRMaterials( sdw::ShaderWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrMRMaterials::declare( bool hasSsbo
		, uint32_t binding
		, uint32_t set )
	{
		m_type = MetallicRoughnessMaterial::declare( m_writer );

		if ( hasSsbo )
		{
			m_ssbo = std::make_unique< sdw::ArraySsboT< MetallicRoughnessMaterial > >( m_writer
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
			m_getMaterial = m_writer.implementFunction< MetallicRoughnessMaterial >( "getMaterial"
				, [this, &c3d_materials]( sdw::UInt const & index )
				{
					auto result = m_writer.declLocale< MetallicRoughnessMaterial >( "result"
						, *m_type );
					auto offset = m_writer.declLocale( "offset"
						, m_writer.cast< sdw::Int >( index ) * sdw::Int( MaxMaterialComponentsCount ) );
					result.m_albRough = c3d_materials.fetch( sdw::Int{ offset++ } );
					result.m_metDiv = c3d_materials.fetch( sdw::Int{ offset++ } );
					doFetch( result, c3d_materials, offset );
					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "index" } );
		}
	}

	MetallicRoughnessMaterial PbrMRMaterials::getMaterial( sdw::UInt const & index )const
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

	BaseMaterialUPtr PbrMRMaterials::getBaseMaterial( sdw::UInt const & index )const
	{
		auto material = m_writer.declLocale( "material"
			, getMaterial( index ) );
		return std::make_unique< MetallicRoughnessMaterial >( material );
	}

	//*********************************************************************************************

	PbrSGMaterials::PbrSGMaterials( sdw::ShaderWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrSGMaterials::declare( bool hasSsbo
		, uint32_t binding
		, uint32_t set )
	{
		m_type = SpecularGlossinessMaterial::declare( m_writer );

		if ( hasSsbo )
		{
			m_ssbo = std::make_unique< sdw::ArraySsboT< SpecularGlossinessMaterial > >( m_writer
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
			m_getMaterial = m_writer.implementFunction< SpecularGlossinessMaterial >( "getMaterial"
				, [this, &c3d_materials]( sdw::UInt const & index )
				{
					auto result = m_writer.declLocale< SpecularGlossinessMaterial >( "result"
						, *m_type );
					auto offset = m_writer.declLocale( "offset"
						, m_writer.cast< sdw::Int >( index ) * sdw::Int( MaxMaterialComponentsCount ) );
					result.m_diffDiv = c3d_materials.fetch( sdw::Int{ offset++ } );
					result.m_specGloss = c3d_materials.fetch( sdw::Int{ offset++ } );
					doFetch( result, c3d_materials, offset );
					m_writer.returnStmt( result );
				}
				, sdw::InUInt{ m_writer, "index" } );
		}
	}

	SpecularGlossinessMaterial PbrSGMaterials::getMaterial( sdw::UInt const & index )const
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

	BaseMaterialUPtr PbrSGMaterials::getBaseMaterial( sdw::UInt const & index )const
	{
		auto material = m_writer.declLocale( "material"
			, getMaterial( index ) );
		return std::make_unique< SpecularGlossinessMaterial >( material );
	}

	//*****************************************************************************************
}
