/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneFileParser_H___
#define ___C3D_SceneFileParser_H___

#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

#include <stack>

namespace castor3d
{
	class SceneFileParser
		: public castor::OwnedBy< Engine >
		, public castor::DataHolderT< RootContext >
		, public castor::FileParser
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit SceneFileParser( Engine & engine
			, ProgressBar * progress = nullptr );
		C3D_API castor::FileParserContextUPtr initialiseParser( castor::Path const & path );
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
		C3D_API castor::FileParserContextUPtr doInitialiseParser( castor::Path const & path )override;
		C3D_API void doCleanupParser( castor::PreprocessedFile & preprocessed )override;
		C3D_API void doValidate( castor::PreprocessedFile & preprocessed )override;
		C3D_API castor::String doGetSectionName( castor::SectionId section )const override;
		C3D_API castor::RawUniquePtr< FileParser > doCreateParser()const override;
	};
}

#endif
