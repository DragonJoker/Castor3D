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
		void LoadSceneThenAnother();

	private:
		castor3d::SceneRPtr doParseScene( castor::Path const & path, bool initialise = false );
		void doTestScene( castor::String const & name );
	};
}

#endif
