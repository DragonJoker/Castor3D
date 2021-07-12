#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp"
#include "Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
#if C3D_MaterialsStructOfArrays

		PassBuffer::PassesData doBindData( uint8_t * buffer
			, uint32_t count )
		{
			auto colourDiv = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto specDiv = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto common = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto opacity = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
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
				colourDiv,
				specDiv,
				common,
				opacity,
				reflRefr,
				sssInfo,
				transmittance,
			};
		}

#else

		PassBuffer::PassesData doBindData( uint8_t * buffer
			, uint32_t count )
		{
			return castor::makeArrayView( reinterpret_cast< PassBuffer::PassData * >( buffer )
				, reinterpret_cast< PassBuffer::PassData * >( buffer ) + count );
		}

#endif
	}

	//*********************************************************************************************

	PassBuffer::PassBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * DataSize, cuT( "PassBuffer" ) }
		, m_passCount{ count }
		, m_data{ doBindData( m_buffer.getPtr(), count ) }
	{
	}

	uint32_t PassBuffer::addPass( Pass & pass )
	{
		CU_Require( pass.getId() == 0u );
		CU_Require( m_passes.size() < shader::MaxMaterialsCount );
		m_passes.emplace_back( &pass );
		pass.setId( m_passID++ );
		m_connections.emplace_back( pass.onChanged.connect( [this]( Pass const & pass )
		{
			m_dirty.emplace_back( &pass );
		} ) );
		m_dirty.emplace_back( &pass );
		return pass.getId();
	}

	void PassBuffer::removePass( Pass & pass )
	{
		auto id = pass.getId() - 1u;
		CU_Require( id < m_passes.size() );
		CU_Require( &pass == m_passes[id] );
		auto it = m_passes.erase( m_passes.begin() + id );
		m_connections.erase( m_connections.begin() + id );
		++id;

		while ( it != m_passes.end() )
		{
			( *it )->setId( id );
			++it;
			++id;
		}

		pass.setId( 0u );
		m_passID--;
	}

	void PassBuffer::update()
	{
		if ( !m_dirty.empty() )
		{
			std::vector< Pass const * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			std::for_each( dirty.begin(), end, [this]( Pass const * p_pass )
			{
				p_pass->accept( *this );
			} );

			m_buffer.update();
		}
	}

	VkDescriptorSetLayoutBinding PassBuffer::createLayoutBinding( uint32_t binding )const
	{
		return m_buffer.createLayoutBinding( binding );
	}

	void PassBuffer::createPassBinding( crg::FramePass & pass, uint32_t binding )const
	{
		return m_buffer.createPassBinding( pass, cuT( "Materials" ), binding );
	}

	void PassBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}

	ashes::WriteDescriptorSet PassBuffer::getBinding( uint32_t binding )const
	{
		return m_buffer.getBinding( binding );
	}

	void PassBuffer::visit( PhongPass const & pass )
	{
		CU_Require( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if C3D_MaterialsStructOfArrays

		auto & colourDiv = m_data.colourDiv[index];
		auto & specDiv = m_data.specDiv[index];

#else

		auto & data = m_data[index];
		auto & colourDiv = data.colourDiv;
		auto & specDiv = data.specDiv;

#endif

		colourDiv.r = pass.getDiffuse().red();
		colourDiv.g = pass.getDiffuse().green();
		colourDiv.b = pass.getDiffuse().blue();
		colourDiv.a = pass.getAmbient();
		specDiv.r = pass.getSpecular().red();
		specDiv.g = pass.getSpecular().green();
		specDiv.b = pass.getSpecular().blue();
		specDiv.a = pass.getShininess().value();
		doVisitCommon( pass );
	}

	void PassBuffer::visit( MetallicRoughnessPbrPass const & pass )
	{
		CU_Require( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if C3D_MaterialsStructOfArrays

		auto & colourDiv = m_data.colourDiv[index];
		auto & specDiv = m_data.specDiv[index];

#else

		auto & data = m_data[index];
		auto & colourDiv = data.colourDiv;
		auto & specDiv = data.specDiv;

#endif

		colourDiv.r = pass.getAlbedo().red();
		colourDiv.g = pass.getAlbedo().green();
		colourDiv.b = pass.getAlbedo().blue();
		colourDiv.a = pass.getRoughness();
		auto f0 = castor::RgbColour{ 0.04f, 0.04f, 0.04f } * ( 1.0f - pass.getMetallic() ) + pass.getAlbedo() * pass.getMetallic();
		specDiv.r = f0.red();
		specDiv.g = f0.green();
		specDiv.b = f0.blue();
		specDiv.a = pass.getMetallic();
		doVisitCommon( pass );
	}

	void PassBuffer::visit( SpecularGlossinessPbrPass const & pass )
	{
		CU_Require( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if C3D_MaterialsStructOfArrays

		auto & colourDiv = m_data.colourDiv[index];
		auto & specDiv = m_data.specDiv[index];

#else

		auto & data = m_data[index];
		auto & colourDiv = data.colourDiv;
		auto & specDiv = data.specDiv;

#endif

		colourDiv.r = pass.getDiffuse().red();
		colourDiv.g = pass.getDiffuse().green();
		colourDiv.b = pass.getDiffuse().blue();
		colourDiv.a = 1.0f - pass.getGlossiness();
		specDiv.r = pass.getSpecular().red();
		specDiv.g = pass.getSpecular().green();
		specDiv.b = pass.getSpecular().blue();
		specDiv.a = castor::point::length( castor::Point3f{ pass.getSpecular().constPtr() } );
		doVisitCommon( pass );
	}

	void PassBuffer::doVisitCommon( Pass const & pass )
	{
		auto index = pass.getId() - 1;

#if C3D_MaterialsStructOfArrays

		auto & common = data.common[index];
		auto & opacity = data.opacity[index];
		auto & reflRefr = m_data.reflRefr[index];
		auto & extended = m_data.extended;

		if ( pass.hasSubsurfaceScattering() )
		{
			doVisit( pass.getSubsurfaceScattering()
				, index
				, extended );
		}
		else
		{
			extended.sssInfo[index].r = 0.0f;
			extended.sssInfo[index].g = 0.0f;
			extended.sssInfo[index].b = 0.0f;
			extended.sssInfo[index].a = 0.0f;
		}

#else

		auto & data = m_data[index];
		auto & common = data.common;
		auto & opacity = data.opacity;
		auto & reflRefr = data.reflRefr;
		auto & extended = data.extended;

		if ( pass.hasSubsurfaceScattering() )
		{
			doVisit( pass.getSubsurfaceScattering()
				, index
				, extended );
		}
		else
		{
			extended.sssInfo.r = 0.0f;
			extended.sssInfo.g = 0.0f;
			extended.sssInfo.b = 0.0f;
			extended.sssInfo.a = 0.0f;
		}

#endif

		common.r = pass.getOpacity();
		common.g = pass.getEmissive();
		common.b = pass.getAlphaValue();
		common.a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		opacity.r = pass.getTransmission()->x;
		opacity.g = pass.getTransmission()->y;
		opacity.b = pass.getTransmission()->z;
		opacity.a = pass.getOpacity();
		reflRefr.r = pass.getRefractionRatio();
		reflRefr.g = pass.hasRefraction() ? 1.0f : 0.0f;
		reflRefr.b = pass.hasReflections() ? 1.0f : 0.0f;
		reflRefr.a = float( pass.getBWAccumulationOperator() );
	}

	void PassBuffer::doVisit( SubsurfaceScattering const & subsurfaceScattering
		, uint32_t index
		, ExtendedData & data )
	{
#if C3D_MaterialsStructOfArrays

		data.sssInfo[index].r = 1.0f;
		data.sssInfo[index].g = subsurfaceScattering.getGaussianWidth();
		data.sssInfo[index].b = subsurfaceScattering.getStrength();
		data.sssInfo[index].a = float( subsurfaceScattering.getProfileSize() );

		auto i = 0u;

		for ( auto & factor : subsurfaceScattering )
		{
			data.transmittanceProfile[index][i].r = factor[0];
			data.transmittanceProfile[index][i].g = factor[1];
			data.transmittanceProfile[index][i].b = factor[2];
			data.transmittanceProfile[index][i].a = factor[3];
			++i;
		}

#else

		data.sssInfo.r = 1.0f;
		data.sssInfo.g = subsurfaceScattering.getGaussianWidth();
		data.sssInfo.b = subsurfaceScattering.getStrength();
		data.sssInfo.a = float( subsurfaceScattering.getProfileSize() );

		auto i = 0u;

		for ( auto & factor : subsurfaceScattering )
		{
			data.transmittanceProfile[i].r = factor[0];
			data.transmittanceProfile[i].g = factor[1];
			data.transmittanceProfile[i].b = factor[2];
			data.transmittanceProfile[i].a = factor[3];
			++i;
		}

#endif
	}
}
