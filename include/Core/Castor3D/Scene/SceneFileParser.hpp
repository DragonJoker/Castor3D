/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneFileParser_H___
#define ___C3D_SceneFileParser_H___

#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

#include <stack>

namespace c3d
{
	class SceneFileParser
		: public OwnedBy< Engine >
		, public DataHolderT< RootContext >
		, public FileParser
	{
	public:
		/**
		 *\~english
		 *\brief			Constructor.
		 *\param[in]		engine		The engine.
		 *\param[in,out]	progress	Optional progress bar.
		 *\~french
		 *\brief			Constructeur.
		 *\param[in]		engine		Le moteur.
		 *\param[in,out]	progress	Barre de progression optionnelle.
		 */
		C3D_API explicit SceneFileParser( Engine & engine
			, ProgressBar * progress = nullptr );
		C3D_API FileParserContextUPtr initialiseParser( Path const & path );
		/**
		 *\~english
		 *\return		The render window defined by the scene.
		 *\~french
		 *\return		La fenêtre de rendu définie par la scène.
		 */
		RenderWindowDesc getRenderWindow()
		{
			return getData().window;
		}

		ScenePtrStrMap::iterator scenesBegin()
		{
			return getData().mapScenes.begin();
		}

		ScenePtrStrMap::const_iterator scenesBegin()const
		{
			return getData().mapScenes.begin();
		}

		ScenePtrStrMap::const_iterator scenesEnd()const
		{
			return getData().mapScenes.end();
		}

	private:
		C3D_API FileParserContextUPtr doInitialiseParser( Path const & path )override;
		C3D_API void doCleanupParser( PreprocessedFile & preprocessed )override;
		C3D_API void doValidate( PreprocessedFile & preprocessed )override;
		C3D_API String doGetSectionName( SectionId section )const override;
		C3D_API RawUniquePtr< FileParser > doCreateParser()const override;
	};
}

#endif
