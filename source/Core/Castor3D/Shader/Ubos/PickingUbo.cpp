#include "Castor3D/Shader/Ubos/PickingUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		PickingData::PickingData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_drawIndex{ getMember< sdw::Int >( "drawIndex" ) }
			, m_nodeIndex{ getMember< sdw::Int >( "nodeIndex" ) }
		{
		}

		PickingData & PickingData::operator=( PickingData const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr PickingData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "PickingData" );

			if ( result->empty() )
			{
				result->declMember( "drawIndex", ast::type::Kind::eInt );
				result->declMember( "nodeIndex", ast::type::Kind::eInt );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > PickingData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::Vec4 PickingData::getIndex( sdw::UInt const & instance )const
		{
			return vec4( m_drawIndex, m_nodeIndex, instance, 0.0_f );
		}
	}

	//*********************************************************************************************

	castor::String const PickingUbo::BufferPicking = cuT( "Picking" );
	castor::String const PickingUbo::PickingData = cuT( "c3d_pickingData" );

	PickingUbo::PickingUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise( engine.getRenderSystem()->getCurrentRenderDevice() );
		}
	}

	void PickingUbo::initialise( RenderDevice const & device )
	{
		if ( !m_ubo )
		{
			m_ubo = device.uboPools->getBuffer< Configuration >( 0u );
		}
	}

	void PickingUbo::cleanup( RenderDevice const & device )
	{
		if ( m_ubo )
		{
			device.uboPools->putBuffer( m_ubo );
		}
	}

	void PickingUbo::update( Configuration & configuration
		, uint32_t drawIndex
		, uint32_t nodeIndex )
	{
		configuration.drawIndex = drawIndex;
		configuration.nodeIndex = nodeIndex;
	}

	//*********************************************************************************************
}
