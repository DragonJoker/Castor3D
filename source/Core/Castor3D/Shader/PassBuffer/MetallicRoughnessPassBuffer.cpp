#include "Castor3D/Shader/PassBuffer/MetallicRoughnessPassBuffer.hpp"

#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
#if C3D_MaterialsStructOfArrays

		MetallicRoughnessPassBuffer::PassesData doBindData( uint8_t * data
			, uint32_t count )
		{
			auto albRough = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto metDiv = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
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
				albRough,
				metDiv,
				common,
				reflRefr,
				{
					sssInfo,
					transmittance,
				},
			};
		}

#else

		MetallicRoughnessPassBuffer::PassesData doBindData( uint8_t * data
			, uint32_t count )
		{
			return makeArrayView( reinterpret_cast< MetallicRoughnessPassBuffer::PassData * >( data )
				, reinterpret_cast< MetallicRoughnessPassBuffer::PassData * >( data ) + count );
		}

#endif
	}

	//*********************************************************************************************

	MetallicRoughnessPassBuffer::MetallicRoughnessPassBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: PassBuffer{ engine, device, count, DataSize }
		, m_data{ doBindData( m_buffer.getPtr(), count ) }
	{
	}

	MetallicRoughnessPassBuffer::~MetallicRoughnessPassBuffer()
	{
	}

	void MetallicRoughnessPassBuffer::visit( PhongPass const & pass )
	{
		CU_Require( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if C3D_MaterialsStructOfArrays

		auto & diffAmb = m_data.diffAmb[index];
		auto & albRough = m_data.albRough[index];
		auto & metDiv = m_data.metDiv[index];
		auto & common = data.common[index];
		auto & reflRefr = m_data.reflRefr[index];
		auto & extended = m_data.extended;

#else

		auto & data = m_data[index];
		auto & albRough = data.albRough;
		auto & metDiv = data.metDiv;
		auto & common = data.common;
		auto & reflRefr = data.reflRefr;
		auto & extended = data.extended;

#endif

		albRough.r = pass.getDiffuse().red();
		albRough.g = pass.getDiffuse().green();
		albRough.b = pass.getDiffuse().blue();
		albRough.a = ( 255.0f - pass.getShininess() ) / 255.0f;
		metDiv.r = float( point::length( toRGBFloat( pass.getSpecular() ) ) / point::length( castor::Point3f{ 1, 1, 1 } ) );
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

	void MetallicRoughnessPassBuffer::visit( MetallicRoughnessPbrPass const & pass )
	{
		CU_Require( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if C3D_MaterialsStructOfArrays

		auto & diffAmb = m_data.diffAmb[index];
		auto & albRough = m_data.albRough[index];
		auto & metDiv = m_data.metDiv[index];
		auto & reflRefr = m_data.reflRefr[index];
		auto & extended = m_data.extended;

#else

		auto & data = m_data[index];
		auto & albRough = data.albRough;
		auto & metDiv = data.metDiv;
		auto & common = data.common;
		auto & reflRefr = data.reflRefr;
		auto & extended = data.extended;

#endif

		albRough.r = pass.getAlbedo().red();
		albRough.g = pass.getAlbedo().green();
		albRough.b = pass.getAlbedo().blue();
		albRough.a = pass.getRoughness();
		metDiv.r = pass.getMetallic();
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

	//*********************************************************************************************
}
