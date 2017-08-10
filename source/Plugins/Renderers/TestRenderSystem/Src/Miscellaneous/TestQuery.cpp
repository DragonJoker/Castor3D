#include "Miscellaneous/TestQuery.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestQuery::TestQuery( TestRenderSystem & p_renderSystem, QueryType p_query )
		: GpuQuery( p_renderSystem, p_query )
	{
	}

	TestQuery::~TestQuery()
	{
	}

	bool TestQuery::doInitialise()
	{
		return true;
	}

	void TestQuery::doCleanup()
	{
	}

	void TestQuery::doBegin()const
	{
	}

	void TestQuery::doEnd()const
	{
	}

	void TestQuery::doGetInfos( castor3d::QueryInfo p_infos, int32_t & p_param )const
	{
	}

	void TestQuery::doGetInfos( castor3d::QueryInfo p_infos, uint32_t & p_param )const
	{
	}

	void TestQuery::doGetInfos( castor3d::QueryInfo p_infos, int64_t & p_param )const
	{
	}

	void TestQuery::doGetInfos( castor3d::QueryInfo p_infos, uint64_t & p_param )const
	{
	}
}
