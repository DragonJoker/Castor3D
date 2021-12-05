#include "Castor3D/Shader/Shaders/GlslLight.hpp"

#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		//*********************************************************************************************

		LightData::LightData( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, data{ getMemberArray< Vec4 >( "data" ) }
		{
		}

		ast::type::BaseStructPtr LightData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_LightData" );

			if ( result->empty() )
			{
				result->declMember( "data", ast::type::Kind::eVec4F, getMaxLightComponentsCount() );
			}

			return result;
		}

		//*********************************************************************************************

		Light::Light( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_colourIndex{ getMember< Vec4 >( "colourIndex" ) }
			, m_intensityFarPlane{ getMember< Vec4 >( "intensityFarPlane" ) }
			, m_volumetric{ getMember< Vec4 >( "volumetric" ) }
			, m_shadowsOffsets{ getMember< Vec4 >( "shadowsOffsets" ) }
			, m_shadowsVariances{ getMember< Vec4 >( "shadowsVariances" ) }
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

		ast::type::BaseStructPtr Light::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_Light" );

			if ( result->empty() )
			{
				result->declMember( "colourIndex", ast::type::Kind::eVec4F );
				result->declMember( "intensityFarPlane", ast::type::Kind::eVec4F );
				result->declMember( "volumetric", ast::type::Kind::eVec4F );
				result->declMember( "shadowsOffsets", ast::type::Kind::eVec4F );
				result->declMember( "shadowsVariances", ast::type::Kind::eVec4F );
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
			, m_lightBase{ getMember< Light >( "lightBase" ) }
			, m_directionCount{ getMember< Vec4 >( "directionCount" ) }
			, m_splitDepths{ getMember< Vec4 >( "splitDepths" ) }
			, m_splitScales{ getMember< Vec4 >( "splitScales" ) }
			, m_transforms{ getMemberArray< Mat4 >( "transforms" ) }
			, m_direction{ m_directionCount.xyz() }
			, m_cascadeCount{ writer.cast< UInt >( m_directionCount.w() ) }
		{
		}

		ast::type::BaseStructPtr DirectionalLight::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_DirectionalLight" );

			if ( result->empty() )
			{
				result->declMember( "lightBase", Light::makeType( cache ) );
				result->declMember( "directionCount", ast::type::Kind::eVec4F );
				result->declMember( "splitDepths", ast::type::Kind::eVec4F );
				result->declMember( "splitScales", ast::type::Kind::eVec4F );
				result->declMember( "transforms", ast::type::Kind::eMat4x4F, DirectionalMaxCascadesCount );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > DirectionalLight::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		//*********************************************************************************************

		TiledDirectionalLight::TiledDirectionalLight( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_lightBase{ getMember< Light >( "lightBase" ) }
			, m_directionCount{ getMember< Vec4 >( "directionCount" ) }
			, m_tiles{ getMember< Vec4 >( "tiles" ) }
			, m_splitDepths{ getMemberArray< Vec4 >( "splitDepths" ) }
			, m_splitScales{ getMemberArray< Vec4 >( "splitScales" ) }
			, m_transforms{ getMemberArray< Mat4 >( "transforms" ) }
			, m_direction{ m_directionCount.xyz() }
			, m_cascadeCount{ writer.cast< UInt >( m_directionCount.w() ) }
		{
		}

		ast::type::BaseStructPtr TiledDirectionalLight::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_TiledDirectionalLight" );

			if ( result->empty() )
			{
				result->declMember( "lightBase", Light::makeType( cache ) );
				result->declMember( "directionCount", ast::type::Kind::eVec4F );
				result->declMember( "tiles", ast::type::Kind::eVec4F );
				result->declMember( "splitDepths", ast::type::Kind::eVec4F, 2u );
				result->declMember( "splitScales", ast::type::Kind::eVec4F, 2u );
				result->declMember( "transforms", ast::type::Kind::eMat4x4F, DirectionalMaxCascadesCount );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > TiledDirectionalLight::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		//*********************************************************************************************

		PointLight::PointLight( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_lightBase{ getMember< Light >( "lightBase" ) }
			, m_position4{ getMember< Vec4 >( "position" ) }
			, m_attenuation4{ getMember< Vec4 >( "attenuation" ) }
			, m_position{ m_position4.xyz() }
			, m_attenuation{ m_attenuation4.xyz() }
		{
		}

		ast::type::BaseStructPtr PointLight::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_PointLight" );

			if ( result->empty() )
			{
				result->declMember( "lightBase", Light::makeType( cache ) );
				result->declMember( "position", ast::type::Kind::eVec4F );
				result->declMember( "attenuation", ast::type::Kind::eVec4F );
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
			, m_lightBase{ getMember< Light >( "lightBase" ) }
			, m_position4{ getMember< Vec4 >( "position" ) }
			, m_attenuation4{ getMember< Vec4 >( "attenuation" ) }
			, m_direction4{ getMember< Vec4 >( "direction" ) }
			, m_exponentCutOff{ getMember< Vec4 >( "exponentCutOff" ) }
			, m_transform{ getMember< Mat4 >( "transform" ) }
			, m_position{ m_position4.xyz() }
			, m_attenuation{ m_attenuation4.xyz() }
			, m_direction{ m_direction4.xyz() }
			, m_exponent{ m_exponentCutOff.x() }
			, m_cutOff{ m_exponentCutOff.y() }
		{
		}

		ast::type::BaseStructPtr SpotLight::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_SpotLight" );

			if ( result->empty() )
			{
				result->declMember( "lightBase", Light::makeType( cache ) );
				result->declMember( "position", ast::type::Kind::eVec4F );
				result->declMember( "attenuation", ast::type::Kind::eVec4F );
				result->declMember( "direction", ast::type::Kind::eVec4F );
				result->declMember( "exponentCutOff", ast::type::Kind::eVec4F );
				result->declMember( "transform", ast::type::Kind::eMat4x4F );
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
