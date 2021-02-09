#include "Castor3D/Shader/Shaders/GlslLight.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		//*********************************************************************************************

		Light::Light( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_colourIndex{ getMember< Vec4 >( "m_colourIndex" ) }
			, m_intensityFarPlane{ getMember< Vec4 >( "m_intensityFarPlane" ) }
			, m_volumetric{ getMember< Vec4 >( "m_volumetric" ) }
			, m_shadowsOffsets{ getMember< Vec4 >( "m_shadowsOffsets" ) }
			, m_shadowsVariances{ getMember< Vec4 >( "m_shadowsVariances" ) }
			, m_colour{ m_colourIndex.xyz() }
			, m_intensity{ m_intensityFarPlane.xy() }
			, m_farPlane{ m_intensityFarPlane.z() }
			, m_shadowType{ writer.cast< Int >( m_intensityFarPlane.w() ) }
			, m_index{ writer.cast< Int >( m_colourIndex.w() ) }
			, m_volumetricSteps{ writer.cast< UInt >( m_volumetric.x() ) }
			, m_volumetricScattering{ m_volumetric.y() }
			, m_rawShadowOffsets{ m_shadowsOffsets.xy() }
			, m_pcfShadowOffsets{ m_shadowsOffsets.zw() }
			, m_vsmShadowVariance{ m_shadowsVariances.xy() }
		{
		}

		Light & Light::operator=( Light const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr Light::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
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

		DirectionalLight::DirectionalLight( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_lightBase{ getMember< Light >( "m_lightBase" ) }
			, m_directionCount{ getMember< Vec4 >( "m_directionCount" ) }
			, m_transforms{ getMemberArray< Mat4 >( "m_transforms" ) }
			, m_splitDepths{ getMember< Vec4 >( "m_splitDepths" ) }
			, m_splitScales{ getMember< Vec4 >( "m_splitScales" ) }
			, m_direction{ m_directionCount.xyz() }
			, m_cascadeCount{ writer.cast< UInt >( m_directionCount.w() ) }
		{
		}

		ast::type::StructPtr DirectionalLight::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
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

		PointLight::PointLight( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_lightBase{ getMember< Light >( "m_lightBase" ) }
			, m_position4{ getMember< Vec4 >( "m_position" ) }
			, m_attenuation4{ getMember< Vec4 >( "m_attenuation" ) }
			, m_position{ m_position4.xyz() }
			, m_attenuation{ m_attenuation4.xyz() }
		{
		}

		ast::type::StructPtr PointLight::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
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

		SpotLight::SpotLight( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_lightBase{ getMember< Light >( "m_lightBase" ) }
			, m_position4{ getMember< Vec4 >( "m_position" ) }
			, m_attenuation4{ getMember< Vec4 >( "m_attenuation" ) }
			, m_direction4{ getMember< Vec4 >( "m_direction" ) }
			, m_exponentCutOff{ getMember< Vec4 >( "m_exponentCutOff" ) }
			, m_transform{ getMember< Mat4 >( "m_transform" ) }
			, m_position{ m_position4.xyz() }
			, m_attenuation{ m_attenuation4.xyz() }
			, m_direction{ m_direction4.xyz() }
			, m_exponent{ m_exponentCutOff.x() }
			, m_cutOff{ m_exponentCutOff.y() }
		{
		}

		ast::type::StructPtr SpotLight::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "SpotLight" );

			if ( result->empty() )
			{
				result->declMember( "m_lightBase", Light::makeType( cache ) );
				result->declMember( "m_position", ast::type::Kind::eVec4F );
				result->declMember( "m_attenuation", ast::type::Kind::eVec4F );
				result->declMember( "m_direction", ast::type::Kind::eVec4F );
				result->declMember( "m_exponentCutOff", ast::type::Kind::eVec4F );
				result->declMember( "m_transform", ast::type::Kind::eMat4x4F );
			}

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
