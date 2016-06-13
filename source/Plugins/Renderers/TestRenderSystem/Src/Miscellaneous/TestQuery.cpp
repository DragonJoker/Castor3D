#include "Miscellaneous/TestQuery.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestQuery::TestQuery( TestRenderSystem & p_renderSystem, eQUERY_TYPE p_query )
		: GpuQuery( p_renderSystem, p_query )
	{
	}

	TestQuery::~TestQuery()
	{
	}

	bool TestQuery::DoCreate()
	{
		return true;
	}

	void TestQuery::DoDestroy()
	{
	}

	bool TestQuery::DoBegin()const
	{
		return true;
	}

	void TestQuery::DoEnd()const
	{
	}

	bool TestQuery::DoGetInfos( Castor3D::eQUERY_INFO p_infos, int32_t & p_param )const
	{
		return true;
	}

	bool TestQuery::DoGetInfos( Castor3D::eQUERY_INFO p_infos, uint32_t & p_param )const
	{
		return true;
	}

	bool TestQuery::DoGetInfos( Castor3D::eQUERY_INFO p_infos, int64_t & p_param )const
	{
		return true;
	}

	bool TestQuery::DoGetInfos( Castor3D::eQUERY_INFO p_infos, uint64_t & p_param )const
	{
		return true;
	}
}
