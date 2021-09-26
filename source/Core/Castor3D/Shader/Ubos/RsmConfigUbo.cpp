#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		RsmConfigData::RsmConfigData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, rsmIntensity{ getMember< sdw::Float >( "rsmIntensity" ) }
			, rsmRMax{ getMember< sdw::Float >( "rsmRMax" ) }
			, rsmSampleCount{ getMember< sdw::UInt >( "rsmSampleCount" ) }
			, rsmIndex{ getMember< sdw::UInt >( "rsmIndex" ) }
		{
		}

		ast::type::StructPtr RsmConfigData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_RsmConfigData" );

			if ( result->empty() )
			{
				result->declMember( "rsmIntensity", ast::type::Kind::eFloat );
				result->declMember( "rsmRMax", ast::type::Kind::eFloat );
				result->declMember( "rsmSampleCount", ast::type::Kind::eUInt );
				result->declMember( "rsmIndex", ast::type::Kind::eUInt );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > RsmConfigData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}
	}

	//*********************************************************************************************

	std::string const RsmConfigUbo::BufferRsmConfig = "RsmConfig";
	std::string const RsmConfigUbo::RsmConfigData = "c3d_rsmConfigData";

	RsmConfigUbo::RsmConfigUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
	}

	RsmConfigUbo::~RsmConfigUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void RsmConfigUbo::update( Light const & light )
	{
		CU_Require( m_ubo );
		auto & config = light.getRsmConfig();
		auto & data = m_ubo.getData();
		data.intensity = config.intensity;
		data.maxRadius = config.maxRadius;
		data.sampleCount = config.sampleCount.value().value();
		data.index = light.getShadowMapIndex();
	}

	//*********************************************************************************************
}
