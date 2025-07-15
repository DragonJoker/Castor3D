/* See LICENSE file in root folder */
#ifndef ___C3DT_BINARY_EXPORT_TEST_H___
#define ___C3DT_BINARY_EXPORT_TEST_H___

#include "Castor3DTestPrerequisites.hpp"

#include <cstring>

namespace Testing
{
	class BinaryExportTest
		: public C3DTestCase
	{
	public:
		explicit BinaryExportTest( c3d::Engine & engine );

	private:
		void doRegisterTests() override;

	private:
		void SimpleMesh();
		void ImportExport();
		void AnimatedMesh();
		void doTestMeshFile( c3d::String const & name );
		void doTestMesh( c3d::Mesh & src );
	};
}

#endif
