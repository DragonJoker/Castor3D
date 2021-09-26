/* See LICENSE file in root folder */
#ifndef ___C3DT_SCENE_EXPORT_TEST_H___
#define ___C3DT_SCENE_EXPORT_TEST_H___

#include "Castor3DTestPrerequisites.hpp"

#include <cstring>

namespace Testing
{
	class SceneExportTest
		: public C3DTestCase
	{
	public:
		explicit SceneExportTest( castor3d::Engine & engine );

	private:
		void doRegisterTests()override;

	private:
		void SimpleScene();
		void InstancedScene();
		void AlphaScene();
		void AnimatedScene();

	private:
		castor3d::SceneSPtr doParseScene( castor::Path const & p_path );
		void doTestScene( castor::String const & p_name );
	};
}

#endif
