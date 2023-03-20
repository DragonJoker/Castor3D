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
				: rootFolder{ std::move( prootFolder ) }
				, sceneSamplersFile{ std::move( psceneMaterialsFile ) }
				, globalSamplersFile{ std::move( pglobalSamplersFile ) }
				, sceneMaterialsFile{ std::move( psceneMaterialsFile ) }
				, globalMaterialsFile{ std::move( pglobalMaterialsFile ) }
				, skeletonsFile{ std::move( pskeletonsFile ) }
				, meshesFile{ std::move( pmeshesFile ) }
				, nodesFile{ std::move( pnodesFile ) }
				, objectsFile{ std::move( pobjectsFile ) }
				, lightsFile{ std::move( plightsFile ) }
				, sceneFontsFile{ std::move( psceneFontsFile ) }
				, globalFontsFile{ std::move( pglobalFontsFile ) }
				, sceneThemesFile{ std::move( psceneThemesFile ) }
				, globalThemesFile{ std::move( pglobalThemesFile ) }
				, sceneStylesFile{ std::move( psceneStylesFile ) }
				, globalStylesFile{ std::move( pglobalStylesFile ) }
				, sceneControlsFile{ std::move( psceneControlsFile ) }
				, globalControlsFile{ std::move( pglobalControlsFile ) }
				, subfolder{ std::move( psubfolder ) }
				, scale{ std::move( pscale ) }
			{
			}

			Path rootFolder;
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
