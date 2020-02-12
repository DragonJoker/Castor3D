#include "Castor3D/Shader/PassBuffer/PhongPassBuffer.hpp"

#include "Castor3D/Material/Pass/PhongPass.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
#if C3D_MaterialsStructOfArrays

		PhongPassBuffer::PassesData doBindData( uint8_t * buffer
			, uint32_t count )
		{
			auto diffAmb = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto specShin = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto common = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto reflRefr = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto sssInfo = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto transmittance = makeArrayView( reinterpret_cast< std::array< PassBuffer::RgbaColour, 10u > * >( buffer )
				, reinterpret_cast< std::array< PassBuffer::RgbaColour, 10u > * >( buffer ) + count );
			return
			{
				diffAmb,
				specShin,
				common,
				reflRefr,
				{
					sssInfo,
					transmittance,
				},
			};
		}

#else

		PhongPassBuffer::PassesData doBindData( uint8_t * buffer
			, uint32_t count )
		{
			return makeArrayView( reinterpret_cast< PhongPassBuffer::PassData * >( buffer )
				, reinterpret_cast< PhongPassBuffer::PassData * >( buffer ) + count );
		}

#endif
	}

	//*********************************************************************************************

	PhongPassBuffer::PhongPassBuffer( Engine & engine
		, uint32_t count )
		: PassBuffer{ engine, count, DataSize }
		, m_data{ doBindData( m_buffer.getPtr(), count ) }
	{
	}

	PhongPassBuffer::~PhongPassBuffer()
	{
	}

	void PhongPassBuffer::visit( PhongPass const & pass )
	{
		CU_Require( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if C3D_MaterialsStructOfArrays

		auto & diffAmb = m_data.diffAmb[index];
		auto & specShin = m_data.specShin[index];
		auto & common = m_data.common[index];
		auto & reflRefr = m_data.reflRefr[index];
		auto & extended = m_data.extended[index];

#else

		auto & data = m_data[index];
		auto & diffAmb = data.diffAmb;
		auto & specShin = data.specShin;
		auto & common = data.common;
		auto & reflRefr = data.reflRefr;
		auto & extended = data.extended;

#endif

		diffAmb.r = pass.getDiffuse().red();
		diffAmb.g = pass.getDiffuse().green();
		diffAmb.b = pass.getDiffuse().blue();
		diffAmb.a = pass.getAmbient();
		specShin.r = pass.getSpecular().red();
		specShin.g = pass.getSpecular().green();
		specShin.b = pass.getSpecular().blue();
		specShin.a = pass.getShininess();
		common.r = pass.getOpacity();
		common.g = pass.getEmissive();
		common.b = pass.getAlphaValue();
		common.a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		reflRefr.r = pass.getRefractionRatio();
		reflRefr.g = checkFlag( pass.getTextures(), TextureFlag::eRefraction ) ? 1.0f : 0.0f;
		reflRefr.b = checkFlag( pass.getTextures(), TextureFlag::eReflection ) ? 1.0f : 0.0f;
		reflRefr.a = float( pass.getBWAccumulationOperator() );
		doVisitExtended( pass, extended );
	}
}
