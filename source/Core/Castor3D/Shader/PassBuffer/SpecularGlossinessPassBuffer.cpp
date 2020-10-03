#include "Castor3D/Shader/PassBuffer/SpecularGlossinessPassBuffer.hpp"

#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
#if C3D_MaterialsStructOfArrays

		SpecularGlossinessPassBuffer::PassesData doBindData( uint8_t * data
			, uint32_t count )
		{
			auto diffDiv = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto specGloss = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto common = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto reflRefr = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto sssInfo = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto transmittance = makeArrayView( reinterpret_cast< std::array< PassBuffer::RgbaColour, 10u > * >( data )
				, reinterpret_cast< std::array< PassBuffer::RgbaColour, 10u > * >( data ) + count );
			return
			{
				diffDiv,
				specGloss,
				common,
				reflRefr,
				{
					sssInfo,
					transmittance,
				},
			};
		}

#else

		SpecularGlossinessPassBuffer::PassesData doBindData( uint8_t * data
			, uint32_t count )
		{
			return makeArrayView( reinterpret_cast< SpecularGlossinessPassBuffer::PassData * >( data )
				, reinterpret_cast< SpecularGlossinessPassBuffer::PassData * >( data ) + count );
		}

#endif
	}

	SpecularGlossinessPassBuffer::SpecularGlossinessPassBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: PassBuffer{ engine, device, count, DataSize }
		, m_data{ doBindData( m_buffer.getPtr(), count ) }
	{
	}

	SpecularGlossinessPassBuffer::~SpecularGlossinessPassBuffer()
	{
	}

	void SpecularGlossinessPassBuffer::visit( PhongPass const & pass )
	{
		CU_Require( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if C3D_MaterialsStructOfArrays

		auto & diffDiv = m_data.diffDiv[index];
		auto & specGloss = m_data.specGloss[index];
		auto & metDiv = m_data.metDiv[index];
		auto & reflRefr = m_data.reflRefr[index];
		auto & extended = m_data.extended;

#else

		auto & data = m_data[index];
		auto & diffDiv = data.diffDiv;
		auto & specGloss = data.specGloss;
		auto & common = data.common;
		auto & reflRefr = data.reflRefr;
		auto & extended = data.extended;

#endif

		diffDiv.r = pass.getDiffuse().red();
		diffDiv.g = pass.getDiffuse().green();
		diffDiv.b = pass.getDiffuse().blue();
		specGloss.r = pass.getSpecular().red();
		specGloss.g = pass.getSpecular().green();
		specGloss.b = pass.getSpecular().blue();
		specGloss.a = pass.getShininess().value();
		common.r = pass.getOpacity();
		common.g = pass.getEmissive();
		common.b = pass.getAlphaValue();
		common.a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		reflRefr.r = pass.getRefractionRatio();
		reflRefr.g = pass.hasRefraction() ? 1.0f : 0.0f;
		reflRefr.b = pass.hasReflections() ? 1.0f : 0.0f;
		reflRefr.a = 1.0f;
		doVisitExtended( pass, extended );
	}

	void SpecularGlossinessPassBuffer::visit( SpecularGlossinessPbrPass const & pass )
	{
		CU_Require( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if C3D_MaterialsStructOfArrays

		auto & diffDiv = m_data.diffDiv[index];
		auto & specGloss = m_data.specGloss[index];
		auto & metDiv = m_data.metDiv[index];
		auto & reflRefr = m_data.reflRefr[index];
		auto & extended = m_data.extended;

#else

		auto & data = m_data[index];
		auto & diffDiv = data.diffDiv;
		auto & specGloss = data.specGloss;
		auto & common = data.common;
		auto & reflRefr = data.reflRefr;
		auto & extended = data.extended;

#endif

		diffDiv.r = pass.getDiffuse().red();
		diffDiv.g = pass.getDiffuse().green();
		diffDiv.b = pass.getDiffuse().blue();
		specGloss.r = pass.getSpecular().red();
		specGloss.g = pass.getSpecular().green();
		specGloss.b = pass.getSpecular().blue();
		specGloss.a = pass.getGlossiness();
		common.r = pass.getOpacity();
		common.g = pass.getEmissive();
		common.b = pass.getAlphaValue();
		common.a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		reflRefr.r = pass.getRefractionRatio();
		reflRefr.g = pass.hasRefraction() ? 1.0f : 0.0f;
		reflRefr.b = pass.hasReflections() ? 1.0f : 0.0f;
		reflRefr.a = float( pass.getBWAccumulationOperator() );
		doVisitExtended( pass, extended );
	}
}
