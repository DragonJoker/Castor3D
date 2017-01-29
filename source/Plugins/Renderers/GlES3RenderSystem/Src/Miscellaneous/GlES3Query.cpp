#include "Miscellaneous/GlES3Query.hpp"

#include "Render/GlES3RenderSystem.hpp"
#include "Common/OpenGlES3.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3Query::GlES3Query( GlES3RenderSystem & p_renderSystem, QueryType p_query )
		: GpuQuery( p_renderSystem, p_query )
		, ObjectType( p_renderSystem.GetOpenGlES3(),
					  "GlES3Query",
					  std::bind( &OpenGlES3::GenQueries, std::ref( p_renderSystem.GetOpenGlES3() ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::DeleteQueries, std::ref( p_renderSystem.GetOpenGlES3() ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::IsQuery, std::ref( p_renderSystem.GetOpenGlES3() ), std::placeholders::_1 )
					)
	{
	}

	GlES3Query::~GlES3Query()
	{
	}

	bool GlES3Query::DoInitialise()
	{
		return ObjectType::Create();
	}

	void GlES3Query::DoCleanup()
	{
		return ObjectType::Destroy();
	}

	void GlES3Query::DoBegin()const
	{
		ENSURE( GetGlES3Name() != GlES3InvalidIndex );
		GetOpenGlES3().BeginQuery( GetOpenGlES3().Get( m_type ), GetGlES3Name() );
	}

	void GlES3Query::DoEnd()const
	{
		GetOpenGlES3().EndQuery( GetOpenGlES3().Get( m_type ) );
	}

	void GlES3Query::DoGetInfos( Castor3D::QueryInfo p_infos, int32_t & p_param )const
	{
		GetOpenGlES3().GetQueryObjectInfos( GetGlES3Name(), GetOpenGlES3().Get( p_infos ), &p_param );
	}

	void GlES3Query::DoGetInfos( Castor3D::QueryInfo p_infos, uint32_t & p_param )const
	{
		GetOpenGlES3().GetQueryObjectInfos( GetGlES3Name(), GetOpenGlES3().Get( p_infos ), &p_param );
	}

	void GlES3Query::DoGetInfos( Castor3D::QueryInfo p_infos, int64_t & p_param )const
	{
		GetOpenGlES3().GetQueryObjectInfos( GetGlES3Name(), GetOpenGlES3().Get( p_infos ), &p_param );
	}

	void GlES3Query::DoGetInfos( Castor3D::QueryInfo p_infos, uint64_t & p_param )const
	{
		GetOpenGlES3().GetQueryObjectInfos( GetGlES3Name(), GetOpenGlES3().Get( p_infos ), &p_param );
	}
}
