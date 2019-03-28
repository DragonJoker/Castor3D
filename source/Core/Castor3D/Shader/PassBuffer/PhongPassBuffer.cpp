#include "Castor3D/Shader/PassBuffer/PhongPassBuffer.hpp"

#include "Castor3D/Material/PhongPass.hpp"

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

		m_data.diffAmb[index].r = pass.getDiffuse().red();
		m_data.diffAmb[index].g = pass.getDiffuse().green();
		m_data.diffAmb[index].b = pass.getDiffuse().blue();
		m_data.diffAmb[index].a = pass.getAmbient();
		m_data.specShin[index].r = pass.getSpecular().red();
		m_data.specShin[index].g = pass.getSpecular().green();
		m_data.specShin[index].b = pass.getSpecular().blue();
		m_data.specShin[index].a = pass.getShininess();
		m_data.common[index].r = pass.getOpacity();
		m_data.common[index].g = pass.getEmissive();
		m_data.common[index].b = pass.getAlphaValue();
		m_data.common[index].a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		m_data.reflRefr[index].r = pass.getRefractionRatio();
		m_data.reflRefr[index].g = checkFlag( pass.getTextures(), TextureFlag::eRefraction ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].b = checkFlag( pass.getTextures(), TextureFlag::eReflection ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].a = float( pass.getBWAccumulationOperator() );
		doVisitExtended( pass, m_data.extended );

#else

		m_data[index].diffAmb.r = pass.getDiffuse().red();
		m_data[index].diffAmb.g = pass.getDiffuse().green();
		m_data[index].diffAmb.b = pass.getDiffuse().blue();
		m_data[index].diffAmb.a = pass.getAmbient();
		m_data[index].specShin.r = pass.getSpecular().red();
		m_data[index].specShin.g = pass.getSpecular().green();
		m_data[index].specShin.b = pass.getSpecular().blue();
		m_data[index].specShin.a = pass.getShininess();
		m_data[index].common.r = pass.getOpacity();
		m_data[index].common.g = pass.getEmissive();
		m_data[index].common.b = pass.getAlphaValue();
		m_data[index].common.a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		m_data[index].reflRefr.r = pass.getRefractionRatio();
		m_data[index].reflRefr.g = checkFlag( pass.getTextures(), TextureFlag::eRefraction ) ? 1.0f : 0.0f;
		m_data[index].reflRefr.b = checkFlag( pass.getTextures(), TextureFlag::eReflection ) ? 1.0f : 0.0f;
		m_data[index].reflRefr.a = float( pass.getBWAccumulationOperator() );
		doVisitExtended( pass, m_data[index].extended );

#endif
	}
}
