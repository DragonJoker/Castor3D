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
		, edgeWidth{ getMember< sdw::Float >( "edgeWidth" ) }
		, depthFactor{ getMember< sdw::Float >( "depthFactor" ) }
		, normalFactor{ getMember< sdw::Float >( "normalFactor" ) }
		, objectFactor{ getMember< sdw::Float >( "objectFactor" ) }
		, edgeColour{ getMember< sdw::Vec4 >( "edgeColour" ) }
		, specific{ getMember< sdw::Vec4 >( "specific" ) }
		, index{ getMember< sdw::UInt >( "index" ) }
		, emissive{ getMember< sdw::Float >( "emissive" ) }
		, alphaRef{ getMember< sdw::Float >( "alphaRef" ) }
		, sssProfileIndex{ getMember< sdw::UInt >( "sssProfileIndex" ) }
		, transmission{ getMember< sdw::Vec3 >( "transmission" ) }
		, opacity{ getMember< sdw::Float >( "opacity" ) }
		, refractionRatio{ getMember< sdw::Float >( "refractionRatio" ) }
		, hasRefraction{ getMember< sdw::UInt >( "hasRefraction" ) }
		, hasReflection{ getMember< sdw::UInt >( "hasReflection" ) }
		, bwAccumulationOperator{ getMember< sdw::UInt >( "bwAccumulationOperator" ) }
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

			result->declMember( "edgeWidth", ast::type::Kind::eFloat );
			result->declMember( "depthFactor", ast::type::Kind::eFloat );
			result->declMember( "normalFactor", ast::type::Kind::eFloat );
			result->declMember( "objectFactor", ast::type::Kind::eFloat );

			result->declMember( "edgeColour", ast::type::Kind::eVec4F );
			result->declMember( "specific", ast::type::Kind::eVec4F );

			result->declMember( "index", ast::type::Kind::eUInt );
			result->declMember( "emissive", ast::type::Kind::eFloat );
			result->declMember( "alphaRef", ast::type::Kind::eFloat );
			result->declMember( "sssProfileIndex", ast::type::Kind::eUInt );

			result->declMember( "transmission", ast::type::Kind::eVec3F );
			result->declMember( "opacity", ast::type::Kind::eFloat );

			result->declMember( "refractionRatio", ast::type::Kind::eFloat );
			result->declMember( "hasRefraction", ast::type::Kind::eInt );
			result->declMember( "hasReflection", ast::type::Kind::eInt );
			result->declMember( "bwAccumulationOperator", ast::type::Kind::eFloat );
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
		return ( m_ssbo
			? ( *m_ssbo )[index - 1_u]
			: m_writer.declLocale< Material >( "mat", false ) );
	}

	//*****************************************************************************************

	SssProfile::SssProfile( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, transmittanceProfileSize{ getMember< sdw::Int >( "transmittanceProfileSize" ) }
		, gaussianWidth{ getMember< sdw::Float >( "gaussianWidth" ) }
		, subsurfaceScatteringStrength{ getMember< sdw::Float >( "subsurfaceScatteringStrength" ) }
		, pad{ getMember< sdw::Float >( "pad" ) }
		, transmittanceProfile{ getMemberArray< sdw::Vec4 >( "transmittanceProfile" ) }
	{
	}

	ast::type::BaseStructPtr SssProfile::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140, "C3D_SssProfile" );

		if ( result->empty() )
		{
			result->declMember( "transmittanceProfileSize", ast::type::Kind::eInt );
			result->declMember( "gaussianWidth", ast::type::Kind::eFloat );
			result->declMember( "subsurfaceScatteringStrength", ast::type::Kind::eFloat );
			result->declMember( "pad", ast::type::Kind::eFloat );
			result->declMember( "transmittanceProfile", ast::type::Kind::eVec4F, TransmittanceProfileSize );
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
