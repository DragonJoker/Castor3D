/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextScene_H___
#define ___CSE_TextScene_H___

#include <Castor3D/Scene/Scene.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Scene >
		: public TextWriterT< castor3d::Scene >
	{
	public:
		struct Options
		{
			Options( Path prootFolder = {}
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
				, float pscale = 1.0f )
				: rootFolder{ castor::move( prootFolder ) }
				, sceneTexturesFile{ castor::move( psceneTexturesFile ) }
				, sceneSamplersFile{ castor::move( psceneSamplersFile ) }
				, globalSamplersFile{ castor::move( pglobalSamplersFile ) }
				, sceneMaterialsFile{ castor::move( psceneMaterialsFile ) }
				, globalMaterialsFile{ castor::move( pglobalMaterialsFile ) }
				, skeletonsFile{ castor::move( pskeletonsFile ) }
				, meshesFile{ castor::move( pmeshesFile ) }
				, nodesFile{ castor::move( pnodesFile ) }
				, objectsFile{ castor::move( pobjectsFile ) }
				, lightsFile{ castor::move( plightsFile ) }
				, sceneFontsFile{ castor::move( psceneFontsFile ) }
				, globalFontsFile{ castor::move( pglobalFontsFile ) }
				, sceneThemesFile{ castor::move( psceneThemesFile ) }
				, globalThemesFile{ castor::move( pglobalThemesFile ) }
				, sceneStylesFile{ castor::move( psceneStylesFile ) }
				, globalStylesFile{ castor::move( pglobalStylesFile ) }
				, sceneControlsFile{ castor::move( psceneControlsFile ) }
				, globalControlsFile{ castor::move( pglobalControlsFile ) }
				, subfolder{ castor::move( psubfolder ) }
				, scale{ pscale }
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
		};

		explicit TextWriter( String const & tabs
			, Options options = {} );
		bool operator()( castor3d::Scene const & scene
			, castor::StringStream & file )override;

	private:
		Options m_options;
	};
}

#endif
