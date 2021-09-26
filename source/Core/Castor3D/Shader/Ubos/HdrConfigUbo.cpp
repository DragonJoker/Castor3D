#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		HdrConfigData::HdrConfigData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_exposure{ getMember< sdw::Float >( "exposure" ) }
			, m_gamma{ getMember< sdw::Float >( "gamma" ) }
		{
		}

		ast::type::StructPtr HdrConfigData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_HdrConfigData" );

			if ( result->empty() )
			{
				result->declMember( "exposure", ast::type::Kind::eFloat );
				result->declMember( "gamma", ast::type::Kind::eFloat );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > HdrConfigData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::Vec3 HdrConfigData::removeGamma( sdw::Vec3 const & sRGB )const
		{
			return pow( max( sRGB, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( m_gamma ) );
		}

		sdw::Vec3 HdrConfigData::applyGamma( sdw::Vec3 const & hdr )const
		{
			return pow( max( hdr, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( 1.0_f / m_gamma ) );
		}
	}

	//*********************************************************************************************

	castor::String const HdrConfigUbo::BufferHdrConfig = cuT( "HdrConfig" );
	castor::String const HdrConfigUbo::HdrConfigData = cuT( "c3d_hdrConfigData" );

	HdrConfigUbo::HdrConfigUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
	}

	HdrConfigUbo::~HdrConfigUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void HdrConfigUbo::cpuUpdate( HdrConfig const & config )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();
		data.exposure = config.exposure;
		data.gamma = config.gamma;
	}
}
