#include "Miscellaneous/GlQuery.hpp"

#include "Render/GlRenderSystem.hpp"
#include "Common/OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlQuery::GlQuery( GlRenderSystem & p_renderSystem, QueryType p_query )
		: GpuQuery( p_renderSystem, p_query )
		, ObjectType( p_renderSystem.GetOpenGl(),
					  "GlQuery",
					  std::bind( &OpenGl::GenQueries, std::ref( p_renderSystem.GetOpenGl() ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteQueries, std::ref( p_renderSystem.GetOpenGl() ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsQuery, std::ref( p_renderSystem.GetOpenGl() ), std::placeholders::_1 )
					)
	{
	}

	GlQuery::~GlQuery()
	{
	}

	bool GlQuery::DoInitialise()
	{
		return ObjectType::Create();
	}

	void GlQuery::DoCleanup()
	{
		return ObjectType::Destroy();
	}

	bool GlQuery::DoBegin()const
	{
		ENSURE( GetGlName() != GlInvalidIndex );
		return GetOpenGl().BeginQuery( GetOpenGl().Get( m_type ), GetGlName() );
	}

	void GlQuery::DoEnd()const
	{
		GetOpenGl().EndQuery( GetOpenGl().Get( m_type ) );
	}

	bool GlQuery::DoGetInfos( Castor3D::QueryInfo p_infos, int32_t & p_param )const
	{
		return GetOpenGl().GetQueryObjectInfos( GetGlName(), GetOpenGl().Get( p_infos ), &p_param );
	}

	bool GlQuery::DoGetInfos( Castor3D::QueryInfo p_infos, uint32_t & p_param )const
	{
		return GetOpenGl().GetQueryObjectInfos( GetGlName(), GetOpenGl().Get( p_infos ), &p_param );
	}

	bool GlQuery::DoGetInfos( Castor3D::QueryInfo p_infos, int64_t & p_param )const
	{
		return GetOpenGl().GetQueryObjectInfos( GetGlName(), GetOpenGl().Get( p_infos ), &p_param );
	}

	bool GlQuery::DoGetInfos( Castor3D::QueryInfo p_infos, uint64_t & p_param )const
	{
		return GetOpenGl().GetQueryObjectInfos( GetGlName(), GetOpenGl().Get( p_infos ), &p_param );
	}
}
