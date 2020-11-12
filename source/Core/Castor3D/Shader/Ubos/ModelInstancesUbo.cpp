#include "Castor3D/Shader/Ubos/ModelInstancesUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		ModelInstancesData::ModelInstancesData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_instances{ getMemberArray< sdw::UVec4 >( "instances" ) }
			, m_instanceCount{ getMember< sdw::UInt >( "instanceCount" ) }
		{
		}

		ModelInstancesData & ModelInstancesData::operator=( ModelInstancesData const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr ModelInstancesData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "ModelInstancesData" );

			if ( result->empty() )
			{
				result->declMember( "instances", ast::type::Kind::eVec4U, 7u );
				result->declMember( "instanceCount", ast::type::Kind::eUInt );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > ModelInstancesData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::UInt ModelInstancesData::getTileIndex( sdw::InVertex const & in )const
		{
			return min( m_instanceCount - 1_u
				, m_instances[in.instanceIndex / 4][in.instanceIndex % 4] );
		}
	}

	//*********************************************************************************************

	castor::String const ModelInstancesUbo::BufferModelInstances = cuT( "ModelInstances" );
	castor::String const ModelInstancesUbo::ModelInstancesData = cuT( "c3d_modelInstancesData" );

	ModelInstancesUbo::ModelInstancesUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
	}

	ModelInstancesUbo::~ModelInstancesUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void ModelInstancesUbo::cpuUpdate( UInt32Array const & instances )
	{
		castor3d::cpuUpdate( m_ubo, instances );
	}

	void cpuUpdate( UniformBufferOffsetT< ModelInstancesUboConfiguration > & buffer
		, UInt32Array const & instances )
	{
		CU_Require( buffer );
		auto & configuration = buffer.getData();
		uint32_t index = 0u;

		for ( uint32_t i = 0u; i < instances.size(); ++i )
		{
			auto & instance = configuration.instances[index++];

			for ( uint32_t j = 0u; j < 4u && i < instances.size(); ++i, ++j )
			{
				instance[j] = instances[i];
			}

			--i;
		}

		configuration.instanceCount = uint32_t( instances.size() );
	}
}
