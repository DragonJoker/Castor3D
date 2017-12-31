#include "Miscellaneous/GlQuery.hpp"

#include "Render/GlRenderSystem.hpp"
#include "Common/OpenGl.hpp"

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlQuery::GlQuery( GlRenderSystem & renderSystem, QueryType p_query )
		: GpuQuery( renderSystem, p_query )
		, ObjectType( renderSystem.getOpenGl(),
					  "GlQuery",
					  std::bind( &OpenGl::GenQueries, std::ref( renderSystem.getOpenGl() ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteQueries, std::ref( renderSystem.getOpenGl() ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsQuery, std::ref( renderSystem.getOpenGl() ), std::placeholders::_1 )
					)
	{
	}

	GlQuery::~GlQuery()
	{
	}

	bool GlQuery::doInitialise()
	{
		return ObjectType::create();
	}

	void GlQuery::doCleanup()
	{
		return ObjectType::destroy();
	}

	void GlQuery::doBegin()const
	{
		ENSURE( getGlName() != GlInvalidIndex );
		getOpenGl().BeginQuery( getOpenGl().get( m_type ), getGlName() );
	}

	void GlQuery::doEnd()const
	{
		getOpenGl().EndQuery( getOpenGl().get( m_type ) );
	}

	void GlQuery::doGetInfos( castor3d::QueryInfo p_infos, int32_t & p_param )const
	{
		getOpenGl().GetQueryObjectInfos( getGlName(), getOpenGl().get( p_infos ), &p_param );
	}

	void GlQuery::doGetInfos( castor3d::QueryInfo p_infos, uint32_t & p_param )const
	{
		getOpenGl().GetQueryObjectInfos( getGlName(), getOpenGl().get( p_infos ), &p_param );
	}

	void GlQuery::doGetInfos( castor3d::QueryInfo p_infos, int64_t & p_param )const
	{
		getOpenGl().GetQueryObjectInfos( getGlName(), getOpenGl().get( p_infos ), &p_param );
	}

	void GlQuery::doGetInfos( castor3d::QueryInfo p_infos, uint64_t & p_param )const
	{
		getOpenGl().GetQueryObjectInfos( getGlName(), getOpenGl().get( p_infos ), &p_param );
	}
}
