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
		explicit SceneExportTest( c3d::Engine & engine );

	private:
		void doRegisterTests()override;

	private:
		void SimpleScene();
		void InstancedScene();
		void AlphaScene();
		void AnimatedScene();
		void LoadSceneThenAnother();
		void LoadCleanReload();

	private:
		c3d::SceneRPtr doParseScene( c3d::Path const & path, bool initialise = false );
		void doTestScene( c3d::String const & name );
		void doTestCleanReloadScene( c3d::String const & name );
	};
}

#endif
