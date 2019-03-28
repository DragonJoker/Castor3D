#include "Castor3D/Shader/PassBuffer/SpecularGlossinessPassBuffer.hpp"

#include "Castor3D/Material/PhongPass.hpp"
#include "Castor3D/Material/SpecularGlossinessPbrPass.hpp"

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
		, uint32_t count )
		: PassBuffer{ engine, count, DataSize }
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

		m_data.diffDiv[index].r = pass.getDiffuse().red();
		m_data.diffDiv[index].g = pass.getDiffuse().green();
		m_data.diffDiv[index].b = pass.getDiffuse().blue();
		m_data.specGloss[index].r = pass.getSpecular().red();
		m_data.specGloss[index].g = pass.getSpecular().green();
		m_data.specGloss[index].b = pass.getSpecular().blue();
		m_data.specGloss[index].a = pass.getShininess();
		m_data.common[index].r = pass.getOpacity();
		m_data.common[index].g = pass.getEmissive();
		m_data.common[index].b = pass.getAlphaValue();
		m_data.common[index].a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		m_data.reflRefr[index].r = pass.getRefractionRatio();
		m_data.reflRefr[index].g = checkFlag( pass.getTextures(), TextureFlag::eRefraction ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].b = checkFlag( pass.getTextures(), TextureFlag::eReflection ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].a = 1.0f;
		doVisitExtended( pass, m_data.extended );

#else

		m_data[index].diffDiv.r = pass.getDiffuse().red();
		m_data[index].diffDiv.g = pass.getDiffuse().green();
		m_data[index].diffDiv.b = pass.getDiffuse().blue();
		m_data[index].specGloss.r = pass.getSpecular().red();
		m_data[index].specGloss.g = pass.getSpecular().green();
		m_data[index].specGloss.b = pass.getSpecular().blue();
		m_data[index].specGloss.a = pass.getShininess();
		m_data[index].common.r = pass.getOpacity();
		m_data[index].common.g = pass.getEmissive();
		m_data[index].common.b = pass.getAlphaValue();
		m_data[index].common.a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		m_data[index].reflRefr.r = pass.getRefractionRatio();
		m_data[index].reflRefr.g = checkFlag( pass.getTextures(), TextureFlag::eRefraction ) ? 1.0f : 0.0f;
		m_data[index].reflRefr.b = checkFlag( pass.getTextures(), TextureFlag::eReflection ) ? 1.0f : 0.0f;
		m_data[index].reflRefr.a = 1.0f;
		doVisitExtended( pass, m_data[index].extended );

#endif
	}

	void SpecularGlossinessPassBuffer::visit( SpecularGlossinessPbrPass const & pass )
	{
		CU_Require( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if C3D_MaterialsStructOfArrays

		m_data.diffDiv[index].r = pass.getDiffuse().red();
		m_data.diffDiv[index].g = pass.getDiffuse().green();
		m_data.diffDiv[index].b = pass.getDiffuse().blue();
		m_data.specGloss[index].r = pass.getSpecular().red();
		m_data.specGloss[index].g = pass.getSpecular().green();
		m_data.specGloss[index].b = pass.getSpecular().blue();
		m_data.specGloss[index].a = pass.getGlossiness();
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

		m_data[index].diffDiv.r = pass.getDiffuse().red();
		m_data[index].diffDiv.g = pass.getDiffuse().green();
		m_data[index].diffDiv.b = pass.getDiffuse().blue();
		m_data[index].specGloss.r = pass.getSpecular().red();
		m_data[index].specGloss.g = pass.getSpecular().green();
		m_data[index].specGloss.b = pass.getSpecular().blue();
		m_data[index].specGloss.a = pass.getGlossiness();
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
