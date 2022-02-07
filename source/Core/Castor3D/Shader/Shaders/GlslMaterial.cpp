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
		, m_sssInfo{ getMember< sdw::Vec4 >( "sssInfo" ) }
		, transmittanceProfile{ getMemberArray< sdw::Vec4 >( "transmittanceProfile" ) }
		, opacity{ m_opacityTransmission.w() }
		, transmission{ m_opacityTransmission.xyz() }
		, emissive{ m_common.y() }
		, alphaRef{ m_common.z() }
		, refractionRatio{ m_reflRefr.x() }
		, hasRefraction{ writer.cast< sdw::Int >( m_reflRefr.y() ) }
		, hasReflection{ writer.cast< sdw::Int >( m_reflRefr.z() ) }
		, bwAccumulationOperator{ m_reflRefr.w() }
		, subsurfaceScatteringEnabled{ writer.cast< sdw::Int >( m_sssInfo.x() ) }
		, gaussianWidth{ m_sssInfo.y() }
		, subsurfaceScatteringStrength{ m_sssInfo.z() }
		, transmittanceProfileSize{ writer.cast< sdw::Int >( m_sssInfo.w() ) }
		, edgeWidth{ edgeFactors.x() }
		, depthFactor{ edgeFactors.y() }
		, normalFactor{ edgeFactors.z() }
		, objectFactor{ edgeFactors.w() }
	{
	}

	void Material::create( sdw::CombinedImageT< FImgBufferRgba32 > & materials
		, sdw::Int & offset )
	{
		colourDiv = materials.fetch( sdw::Int{ offset++ } );
		specDiv = materials.fetch( sdw::Int{ offset++ } );
		edgeFactors = materials.fetch( sdw::Int{ offset++ } );
		edgeColour = materials.fetch( sdw::Int{ offset++ } );
		specific = materials.fetch( sdw::Int{ offset++ } );
		m_common = materials.fetch( sdw::Int{ offset++ } );
		m_opacityTransmission = materials.fetch( sdw::Int{ offset++ } );
		m_reflRefr = materials.fetch( sdw::Int{ offset++ } );
		m_sssInfo = materials.fetch( sdw::Int{ offset++ } );

		for ( uint32_t i = 0; i < MaxTransmittanceProfileSize; ++i )
		{
			transmittanceProfile[i] = materials.fetch( sdw::Int{ offset++ } );
		}
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
			result->declMember( "sssInfo", ast::type::Kind::eVec4F );
			result->declMember( "transmittanceProfile", ast::type::Kind::eVec4F, MaxTransmittanceProfileSize );
		}

		return result;
	}

	std::unique_ptr< sdw::Struct > Material::declare( sdw::ShaderWriter & writer )
	{
		return std::make_unique< sdw::Struct >( writer, makeType( writer.getTypesCache() ) );
	}

	//*********************************************************************************************

	Materials::Materials( sdw::ShaderWriter & writer )
		: m_writer{ writer }
	{
	}

	void Materials::declare( uint32_t binding
		, uint32_t set )
	{
		auto type = Material::declare( m_writer );
		m_ssbo = std::make_unique< sdw::ArraySsboT< Material > >( m_writer
			, PassBufferName
			, type->getType()
			, binding
			, set
			, true );
	}

	Material Materials::getMaterial( sdw::UInt const & index )const
	{
		return ( *m_ssbo )[index - 1_u];
	}

	//*********************************************************************************************
}
