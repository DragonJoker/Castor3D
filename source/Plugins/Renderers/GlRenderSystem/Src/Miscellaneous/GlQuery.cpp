#include "Miscellaneous/GlQuery.hpp"

#include "Render/GlRenderSystem.hpp"
#include "Common/OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlQuery::GlQuery( GlRenderSystem & renderSystem, QueryType p_query )
		: GpuQuery( renderSystem, p_query )
		, ObjectType( renderSystem.GetOpenGl(),
					  "GlQuery",
					  std::bind( &OpenGl::GenQueries, std::ref( renderSystem.GetOpenGl() ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteQueries, std::ref( renderSystem.GetOpenGl() ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsQuery, std::ref( renderSystem.GetOpenGl() ), std::placeholders::_1 )
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

	void GlQuery::DoBegin()const
	{
		ENSURE( GetGlName() != GlInvalidIndex );
		GetOpenGl().BeginQuery( GetOpenGl().Get( m_type ), GetGlName() );
	}

	void GlQuery::DoEnd()const
	{
		GetOpenGl().EndQuery( GetOpenGl().Get( m_type ) );
	}

	void GlQuery::DoGetInfos( Castor3D::QueryInfo p_infos, int32_t & p_param )const
	{
		GetOpenGl().GetQueryObjectInfos( GetGlName(), GetOpenGl().Get( p_infos ), &p_param );
	}

	void GlQuery::DoGetInfos( Castor3D::QueryInfo p_infos, uint32_t & p_param )const
	{
		GetOpenGl().GetQueryObjectInfos( GetGlName(), GetOpenGl().Get( p_infos ), &p_param );
	}

	void GlQuery::DoGetInfos( Castor3D::QueryInfo p_infos, int64_t & p_param )const
	{
		GetOpenGl().GetQueryObjectInfos( GetGlName(), GetOpenGl().Get( p_infos ), &p_param );
	}

	void GlQuery::DoGetInfos( Castor3D::QueryInfo p_infos, uint64_t & p_param )const
	{
		GetOpenGl().GetQueryObjectInfos( GetGlName(), GetOpenGl().Get( p_infos ), &p_param );
	}
}
