#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArraySsbo.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d::shader
{
	//*****************************************************************************************

	BaseMaterial::BaseMaterial( ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, m_common{ getMember< Vec4 >( "common" ) }
		, m_opacityTransmission{ getMember< Vec4 >( "opacity" ) }
		, m_reflRefr{ getMember< Vec4 >( "reflRefr" ) }
		, m_sssInfo{ getMember< Vec4 >( "sssInfo" ) }
		, transmittanceProfile{ getMemberArray< Vec4 >( "transmittanceProfile" ) }
		, opacity{ m_opacityTransmission.w() }
		, transmission{ m_opacityTransmission.xyz() }
		, emissive{ m_common.y() }
		, alphaRef{ m_common.z() }
		, gamma{ m_common.w() }
		, refractionRatio{ m_reflRefr.x() }
		, hasRefraction{ writer.cast< Int >( m_reflRefr.y() ) }
		, hasReflection{ writer.cast< Int >( m_reflRefr.z() ) }
		, bwAccumulationOperator{ m_reflRefr.w() }
		, subsurfaceScatteringEnabled{ writer.cast< Int >( m_sssInfo.x() ) }
		, gaussianWidth{ m_sssInfo.y() }
		, subsurfaceScatteringStrength{ m_sssInfo.z() }
		, transmittanceProfileSize{ writer.cast< Int >( m_sssInfo.w() ) }
	{
	}

	//*********************************************************************************************

	Materials::Materials( ShaderWriter & writer )
		: m_writer{ writer }
	{
	}

	void Materials::doFetch( BaseMaterial & result
		, sdw::SampledImageT< FImgBufferRgba32 > & c3d_materials
		, sdw::Int & offset )
	{
		result.m_common = c3d_materials.fetch( Int{ offset++ } );
		result.m_opacityTransmission = c3d_materials.fetch( Int{ offset++ } );
		result.m_reflRefr = c3d_materials.fetch( Int{ offset++ } );
		result.m_sssInfo = c3d_materials.fetch( Int{ offset++ } );

		for ( uint32_t i = 0; i < MaxTransmittanceProfileSize; ++i )
		{
			result.transmittanceProfile[i] = c3d_materials.fetch( Int{ offset++ } );
		}
	}

	//*********************************************************************************************
}
