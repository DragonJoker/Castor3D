#include "DrawEdgesPostEffect/DrawEdgesUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>

#include <CastorUtils/Graphics/Size.hpp>

namespace draw_edges
{
	//*********************************************************************************************

	DrawEdgesData::DrawEdgesData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, normalDepthWidth{ getMember< sdw::Float >( "normalDepthWidth" ) }
		, objectWidth{ getMember< sdw::Float >( "objectWidth" ) }
	{
	}

	ast::type::StructPtr DrawEdgesData::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
			, "C3D_DrawEdgesData" );

		if ( result->empty() )
		{
			result->declMember( "normalDepthWidth", ast::type::Kind::eFloat );
			result->declMember( "objectWidth", ast::type::Kind::eFloat );
		}

		return result;
	}

	//*********************************************************************************************

	castor::String const DrawEdgesUbo::Buffer = cuT( "DrawEdges" );
	castor::String const DrawEdgesUbo::Data = cuT( "c3d_drawEdgesData" );

	DrawEdgesUbo::DrawEdgesUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPools->getBuffer< Configuration >( 0u ) }
	{
		auto & data = m_ubo.getData();
		data.normalDepthWidth = 1.0f;
		data.objectWidth = 1.0f;
	}

	DrawEdgesUbo::~DrawEdgesUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void DrawEdgesUbo::cpuUpdate( float normalDepthWidth
		, float objectWidth )
	{
		auto & data = m_ubo.getData();
		data.normalDepthWidth = normalDepthWidth;
		data.objectWidth = objectWidth;
	}

	//************************************************************************************************
}
