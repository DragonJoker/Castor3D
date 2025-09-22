/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextScene_H___
#define ___CSE_TextScene_H___

#include <Castor3D/Scene/Scene.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Scene >
		: public TextWriterT< Scene >
	{
	public:
		struct Options
		{
			explicit Options( Path prootFolder = {}
				, Path psceneTexturesFile = {}
				, Path psceneSamplersFile = {}
				, Path pglobalSamplersFile = {}
				, Path psceneMaterialsFile = {}
				, Path pglobalMaterialsFile = {}
				, Path pskeletonsFile = {}
				, Path pmeshesFile = {}
				, Path pnodesFile = {}
				, Path pobjectsFile = {}
				, Path plightsFile = {}
				, Path psceneFontsFile = {}
				, Path pglobalFontsFile = {}
				, Path psceneThemesFile = {}
				, Path psceneStylesFile = {}
				, Path pglobalThemesFile = {}
				, Path pglobalStylesFile = {}
				, Path psceneControlsFile = {}
				, Path pglobalControlsFile = {}
				, String psubfolder = {}
				, float pscale = 1.0f
				, bool pforceText = false )
				: rootFolder{ c3d::move( prootFolder ) }
				, sceneTexturesFile{ c3d::move( psceneTexturesFile ) }
				, sceneSamplersFile{ c3d::move( psceneSamplersFile ) }
				, globalSamplersFile{ c3d::move( pglobalSamplersFile ) }
				, sceneMaterialsFile{ c3d::move( psceneMaterialsFile ) }
				, globalMaterialsFile{ c3d::move( pglobalMaterialsFile ) }
				, skeletonsFile{ c3d::move( pskeletonsFile ) }
				, meshesFile{ c3d::move( pmeshesFile ) }
				, nodesFile{ c3d::move( pnodesFile ) }
				, objectsFile{ c3d::move( pobjectsFile ) }
				, lightsFile{ c3d::move( plightsFile ) }
				, sceneFontsFile{ c3d::move( psceneFontsFile ) }
				, globalFontsFile{ c3d::move( pglobalFontsFile ) }
				, sceneThemesFile{ c3d::move( psceneThemesFile ) }
				, globalThemesFile{ c3d::move( pglobalThemesFile ) }
				, sceneStylesFile{ c3d::move( psceneStylesFile ) }
				, globalStylesFile{ c3d::move( pglobalStylesFile ) }
				, sceneControlsFile{ c3d::move( psceneControlsFile ) }
				, globalControlsFile{ c3d::move( pglobalControlsFile ) }
				, subfolder{ c3d::move( psubfolder ) }
				, scale{ pscale }
				, forceText{ pforceText }
			{
			}

			Path rootFolder;
			Path sceneTexturesFile;
			Path sceneSamplersFile;
			Path globalSamplersFile;
			Path sceneMaterialsFile;
			Path globalMaterialsFile;
			Path skeletonsFile;
			Path meshesFile;
			Path nodesFile;
			Path objectsFile;
			Path lightsFile;
			Path sceneFontsFile;
			Path globalFontsFile;
			Path sceneThemesFile;
			Path globalThemesFile;
			Path sceneStylesFile;
			Path globalStylesFile;
			Path sceneControlsFile;
			Path globalControlsFile;
			String subfolder;
			float scale;
			bool forceText;
		};

		explicit TextWriter( String const & tabs
			, Options options = Options{} );
		bool operator()( Scene const & scene
			, StringStream & file )override;

	private:
		Options m_options;
	};
}

#endif
