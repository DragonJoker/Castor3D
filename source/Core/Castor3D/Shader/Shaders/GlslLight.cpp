#include "Castor3D/Shader/Shaders/GlslLight.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		//*********************************************************************************************

		Light::Light( Shader * shader
			, ast::expr::ExprPtr expr )
			: StructInstance{ shader, std::move( expr ) }
			, m_colourIndex{ getMember< Vec4 >( "m_colourIndex" ) }
			, m_intensityFarPlane{ getMember< Vec4 >( "m_intensityFarPlane" ) }
			, m_volumetric{ getMember< Vec4 >( "m_volumetric" ) }
			, m_shadowsOffsets{ getMember< Vec4 >( "m_shadowsOffsets" ) }
			, m_shadowsVariances{ getMember< Vec4 >( "m_shadowsVariances" ) }
			, m_colour{ getShader()
				, makeSwizzle( makeExpr( m_colourIndex )
					, ast::expr::SwizzleKind::e012 ) }
			, m_intensity{ getShader()
				, makeSwizzle( makeExpr( m_intensityFarPlane )
					, ast::expr::SwizzleKind::e01 ) }
			, m_farPlane{ getShader()
				, makeSwizzle( makeExpr( m_intensityFarPlane )
					, ast::expr::SwizzleKind::e2 ) }
			, m_shadowType{ getShader()
				, makeCast( getShader()->getTypesCache().getInt()
					, makeSwizzle( makeExpr( m_intensityFarPlane )
						, ast::expr::SwizzleKind::e3 ) ) }
			, m_index{ getShader()
				, makeCast( getShader()->getTypesCache().getInt()
					, makeSwizzle( makeExpr( m_colourIndex )
						, ast::expr::SwizzleKind::e3 ) ) }
			, m_volumetricSteps{ getShader()
				, makeCast( getShader()->getTypesCache().getUInt()
					, makeSwizzle( makeExpr( m_volumetric )
						, ast::expr::SwizzleKind::e0 ) ) }
			, m_volumetricScattering{ getShader()
				, makeSwizzle( makeExpr( m_volumetric )
					, ast::expr::SwizzleKind::e1 ) }
			, m_rawShadowOffsets{ getShader()
				, makeSwizzle( makeExpr( m_shadowsOffsets )
					, ast::expr::SwizzleKind::e01 ) }
			, m_pcfShadowOffsets{ getShader()
				, makeSwizzle( makeExpr( m_shadowsOffsets )
					, ast::expr::SwizzleKind::e23 ) }
			, m_vsmShadowVariance{ getShader()
				, makeSwizzle( makeExpr( m_shadowsVariances )
					, ast::expr::SwizzleKind::e01 ) }
		{
		}

		Light & Light::operator=( Light const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr Light::makeType( ast::type::TypesCache & cache )
		{
			auto result = std::make_unique< ast::type::Struct >( cache
				, ast::type::MemoryLayout::eStd140
				, "Light" );

			if ( result->empty() )
			{
				result->declMember( "m_colourIndex", ast::type::Kind::eVec4F );
				result->declMember( "m_intensityFarPlane", ast::type::Kind::eVec4F );
				result->declMember( "m_volumetric", ast::type::Kind::eVec4F );
				result->declMember( "m_shadowsOffsets", ast::type::Kind::eVec4F );
				result->declMember( "m_shadowsVariances", ast::type::Kind::eVec4F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > Light::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		//*********************************************************************************************

		DirectionalLight::DirectionalLight( Shader * shader
			, ast::expr::ExprPtr expr )
			: StructInstance{ shader, std::move( expr ) }
			, m_lightBase{ getMember< Light >( "m_lightBase" ) }
			, m_directionCount{ getMember< Vec4 >( "m_directionCount" ) }
			, m_transforms{ getMemberArray< Mat4 >( "m_transforms" ) }
			, m_splitDepths{ getMember< Vec4 >( "m_splitDepths" ) }
			, m_splitScales{ getMember< Vec4 >( "m_splitScales" ) }
			, m_direction{ getShader()
				, makeSwizzle( makeExpr( m_directionCount )
					, ast::expr::SwizzleKind::e012 ) }
			, m_cascadeCount{ getShader()
				, makeCast( getShader()->getTypesCache().getUInt()
					, makeSwizzle( makeExpr( m_directionCount )
						, ast::expr::SwizzleKind::e3 ) ) }
		{
		}

		ast::type::StructPtr DirectionalLight::makeType( ast::type::TypesCache & cache )
		{
			auto result = std::make_unique< ast::type::Struct >( cache
				, ast::type::MemoryLayout::eStd140
				, "DirectionalLight" );

			if ( result->empty() )
			{
				result->declMember( "m_lightBase", Light::makeType( cache ) );
				result->declMember( "m_directionCount", ast::type::Kind::eVec4F );
				result->declMember( "m_splitDepths", ast::type::Kind::eVec4F );
				result->declMember( "m_splitScales", ast::type::Kind::eVec4F );
				result->declMember( "m_transforms", ast::type::Kind::eMat4x4F, DirectionalMaxCascadesCount );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > DirectionalLight::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		//*********************************************************************************************

		PointLight::PointLight( Shader * shader
			, ast::expr::ExprPtr expr )
			: StructInstance( shader, std::move( expr ) )
			, m_lightBase{ getMember< Light >( "m_lightBase" ) }
			, m_position4{ getMember< Vec4 >( "m_position" ) }
			, m_attenuation4{ getMember< Vec4 >( "m_attenuation" ) }
			, m_position{ getShader()
				, makeSwizzle( makeExpr( m_position4 )
					, ast::expr::SwizzleKind::e012 ) }
			, m_attenuation{ getShader()
				, makeSwizzle( makeExpr( m_attenuation4 )
					, ast::expr::SwizzleKind::e012 ) }
		{
		}

		ast::type::StructPtr PointLight::makeType( ast::type::TypesCache & cache )
		{
			auto result = std::make_unique< ast::type::Struct >( cache
				, ast::type::MemoryLayout::eStd140
				, "PointLight" );

			if ( result->empty() )
			{
				result->declMember( "m_lightBase", Light::makeType( cache ) );
				result->declMember( "m_position", ast::type::Kind::eVec4F );
				result->declMember( "m_attenuation", ast::type::Kind::eVec4F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > PointLight::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		//*********************************************************************************************

		SpotLight::SpotLight( Shader * shader
			, ast::expr::ExprPtr expr )
			: StructInstance( shader, std::move( expr ) )
			, m_lightBase{ getMember< Light >( "m_lightBase" ) }
			, m_position4{ getMember< Vec4 >( "m_position" ) }
			, m_attenuation4{ getMember< Vec4 >( "m_attenuation" ) }
			, m_direction4{ getMember< Vec4 >( "m_direction" ) }
			, m_exponentCutOff{ getMember< Vec4 >( "m_exponentCutOff" ) }
			, m_transform{ getMember< Mat4 >( "m_transform" ) }
			, m_position{ getShader()
				, makeSwizzle( makeExpr( m_position4 )
					, ast::expr::SwizzleKind::e012 ) }
			, m_attenuation{ getShader()
				, makeSwizzle( makeExpr( m_attenuation4 )
					, ast::expr::SwizzleKind::e012 ) }
			, m_direction{ getShader()
				, makeSwizzle( makeExpr( m_direction4 )
					, ast::expr::SwizzleKind::e012 ) }
			, m_exponent{ getShader()
				, makeSwizzle( makeExpr( m_exponentCutOff )
					, ast::expr::SwizzleKind::e0 ) }
			, m_cutOff{ getShader()
				, makeSwizzle( makeExpr( m_exponentCutOff )
					, ast::expr::SwizzleKind::e1 ) }
		{
		}

		ast::type::StructPtr SpotLight::makeType( ast::type::TypesCache & cache )
		{
			auto result = std::make_unique< ast::type::Struct >( cache
				, ast::type::MemoryLayout::eStd140
				, "SpotLight" );
			result->declMember( "m_lightBase", Light::makeType( cache ) );
			result->declMember( "m_position", ast::type::Kind::eVec4F );
			result->declMember( "m_attenuation", ast::type::Kind::eVec4F );
			result->declMember( "m_direction", ast::type::Kind::eVec4F );
			result->declMember( "m_exponentCutOff", ast::type::Kind::eVec4F );
			result->declMember( "m_transform", ast::type::Kind::eMat4x4F );
			return result;
		}

		std::unique_ptr< sdw::Struct > SpotLight::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		//*********************************************************************************************
	}
}
