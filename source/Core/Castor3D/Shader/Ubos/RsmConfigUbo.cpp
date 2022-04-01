#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		RsmConfigData::RsmConfigData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, castor::move( expr ), enabled }
			, intensity{ getMember< sdw::Float >( "intensity" ) }
			, maxRadius{ getMember< sdw::Float >( "maxRadius" ) }
			, sampleCount{ getMember< sdw::UInt >( "sampleCount" ) }
			, index{ getMember< sdw::Int >( "index" ) }
		{
		}

		ast::type::BaseStructPtr RsmConfigData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_RsmData" );

			if ( result->empty() )
			{
				result->declMember( "intensity", ast::type::Kind::eFloat );
				result->declMember( "maxRadius", ast::type::Kind::eFloat );
				result->declMember( "sampleCount", ast::type::Kind::eUInt32 );
				result->declMember( "index", ast::type::Kind::eInt32 );
			}

			return result;
		}

		castor::RawUniquePtr< sdw::Struct > RsmConfigData::declare( sdw::ShaderWriter & writer )
		{
			return castor::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}
	}

	//*********************************************************************************************

	RsmConfigUbo::RsmConfigUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}

	RsmConfigUbo::~RsmConfigUbo()noexcept
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void RsmConfigUbo::cpuUpdate( RsmConfig const & rsmConfig
		, uint32_t index )
	{
		CU_Require( m_ubo );
		auto & rsmData = m_ubo.getData();
		rsmData.intensity = *rsmConfig.intensity;
		rsmData.maxRadius = *rsmConfig.maxRadius;
		rsmData.sampleCount = rsmConfig.sampleCount.value().value();
		rsmData.index = int32_t( index );
	}

	//*********************************************************************************************
}
