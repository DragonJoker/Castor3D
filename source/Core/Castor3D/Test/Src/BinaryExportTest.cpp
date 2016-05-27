#include "BinaryExportTest.hpp"

#include <Unique.hpp>

using namespace Castor;
namespace Castor3D
{
	namespace Testing
	{
		//*********************************************************************************************

		namespace
		{
		}

		//*********************************************************************************************

		BinaryExportTest::BinaryExportTest( Engine & p_engine )
			: TestCase{ "BinaryExportTest", p_engine }
		{
		}

		BinaryExportTest::~BinaryExportTest()
		{
		}

		void BinaryExportTest::DoRegisterTests()
		{
			DoRegisterTest( "SimpleMesh", std::bind( &BinaryExportTest::SimpleMesh, this ) );
		}

		void BinaryExportTest::SimpleMesh()
		{
		}

		//*********************************************************************************************
	}
}
