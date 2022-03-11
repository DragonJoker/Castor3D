#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArraySsbo.hpp>

namespace castor3d::shader
{
	//*****************************************************************************************

	Material::Material( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, colourDiv{ getMember< sdw::Vec4 >( "colourDiv" ) }
		, specDiv{ getMember< sdw::Vec4 >( "specDiv" ) }
		, edgeFactors{ getMember< sdw::Vec4 >( "edgeFactors" ) }
		, edgeColour{ getMember< sdw::Vec4 >( "edgeColour" ) }
		, specific{ getMember< sdw::Vec4 >( "specific" ) }
		, m_common{ getMember< sdw::Vec4 >( "common" ) }
		, m_opacityTransmission{ getMember< sdw::Vec4 >( "opacityTransmission" ) }
		, m_reflRefr{ getMember< sdw::Vec4 >( "reflRefr" ) }
		, opacity{ m_opacityTransmission.w() }
		, transmission{ m_opacityTransmission.xyz() }
		, emissive{ m_common.y() }
		, alphaRef{ m_common.z() }
		, sssProfileIndex{ writer.cast< sdw::UInt >( m_common.w() ) }
		, refractionRatio{ m_reflRefr.x() }
		, hasRefraction{ writer.cast< sdw::Int >( m_reflRefr.y() ) }
		, hasReflection{ writer.cast< sdw::Int >( m_reflRefr.z() ) }
		, bwAccumulationOperator{ m_reflRefr.w() }
		, edgeWidth{ edgeFactors.x() }
		, depthFactor{ edgeFactors.y() }
		, normalFactor{ edgeFactors.z() }
		, objectFactor{ edgeFactors.w() }
	{
	}

	sdw::Vec3 Material::colour()const
	{
		return colourDiv.rgb();
	}

	ast::type::BaseStructPtr Material::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "C3D_Material" );

		if ( result->empty() )
		{
			result->declMember( "colourDiv", ast::type::Kind::eVec4F );
			result->declMember( "specDiv", ast::type::Kind::eVec4F );
			result->declMember( "edgeFactors", ast::type::Kind::eVec4F );
			result->declMember( "edgeColour", ast::type::Kind::eVec4F );
			result->declMember( "specific", ast::type::Kind::eVec4F );
			result->declMember( "common", ast::type::Kind::eVec4F );
			result->declMember( "opacityTransmission", ast::type::Kind::eVec4F );
			result->declMember( "reflRefr", ast::type::Kind::eVec4F );
		}

		return result;
	}

	//*********************************************************************************************

	Materials::Materials( sdw::ShaderWriter & writer )
		: m_writer{ writer }
	{
	}

	Materials::Materials( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: Materials{ writer }
	{
		if ( enable )
		{
			declare( binding, set );
		}
	}

	void Materials::declare( uint32_t binding
		, uint32_t set )
	{
		m_ssbo = std::make_unique< sdw::ArraySsboT< Material > >( m_writer
			, PassBufferName
			, binding
			, set
			, true );
	}

	Material Materials::getMaterial( sdw::UInt const & index )const
	{
		return ( *m_ssbo )[index - 1_u];
	}

	//*****************************************************************************************

	SssProfile::SssProfile( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, m_sssInfo{ getMember< sdw::Vec4 >( "sssInfo" ) }
		, transmittanceProfile{ getMemberArray< sdw::Vec4 >( "transmittanceProfile" ) }
		, transmittanceProfileSize{ writer.cast< sdw::Int >( m_sssInfo.x() ) }
		, gaussianWidth{ m_sssInfo.y() }
		, subsurfaceScatteringStrength{ m_sssInfo.z() }
	{
	}

	ast::type::BaseStructPtr SssProfile::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "C3D_SssProfile" );

		if ( result->empty() )
		{
			result->declMember( "sssInfo", ast::type::Kind::eVec4F );
			result->declMember( "transmittanceProfile", ast::type::Kind::eVec4F, 10u );
		}

		return result;
	}

	//*********************************************************************************************

	SssProfiles::SssProfiles( sdw::ShaderWriter & writer )
		: m_writer{ writer }
	{
	}

	SssProfiles::SssProfiles( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: SssProfiles{ writer }
	{
		if ( enable )
		{
			declare( binding, set );
		}
	}

	void SssProfiles::declare( uint32_t binding
		, uint32_t set )
	{
		castor::String const SssProfilesBufferName = cuT( "SssProfilesBuffer" );
		m_ssbo = std::make_unique< sdw::ArraySsboT< SssProfile > >( m_writer
			, SssProfilesBufferName
			, binding
			, set
			, true );
	}

	SssProfile SssProfiles::getProfile( sdw::UInt const & index )const
	{
		return ( *m_ssbo )[index - 1_u];
	}

	//*********************************************************************************************
}
