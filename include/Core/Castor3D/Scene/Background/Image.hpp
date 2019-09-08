/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ImageBackground_H___
#define ___C3D_ImageBackground_H___

#include "Castor3D/Scene/Background/Background.hpp"

namespace castor3d
{
	/**
	*\version
	*	0.11.0
	*\~english
	*\brief
	*	Simple image background.
	*\~french
	*\brief
	*	Simple image de fond.
	*/
	class ImageBackground
		: public SceneBackground
	{
	public:
		/**
		*\~english
		*\brief
		*	ImageBackground loader.
		*\~english
		*\brief
		*	Loader de ImageBackground.
		*/
		class TextWriter
			: public castor::TextWriter< ImageBackground >
		{
		public:
			/**
			*\~english
			*\brief
			*	Constructor.
			*\param[in] tabs
			*	The current indentation.
			*\~french
			*\brief
			*	Constructeur.
			*\param[in] tabs
			*	L'indentation actuelle.
			*/
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			*\~english
			*\brief
			*	Writes a Skybox into a text file.
			*\param[in] obj
			*	The Skybox to save.
			*\param[in] file
			*	The file to write the ImageBackground in.
			*\~french
			*\brief
			*	Ecrit une ImageBackground dans un fichier texte.
			*\param[in] obj
			*	La Skybox.
			*\param[in] file
			*	Le fichier.
			*/
			C3D_API bool operator()( ImageBackground const & obj
				, castor::TextFile & file )override;
		};

	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] engine
		*	The engine.
		*\param[in] scene
		*	The parent scene.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] engine
		*	Le moteur.
		*\param[in] scene
		*	La scène parente.
		*/
		C3D_API explicit ImageBackground( Engine & engine
			, Scene & scene );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		C3D_API virtual ~ImageBackground();
		/**
		*\~english
		*\return
		*	Sets the texture image.
		*\param[in] folder
		*	The image folder.
		*\param[in] relative
		*	The image file path, relative to \p folder.
		*\~french
		*\return
		*	Définit l'image de la texture.
		*\param[in] folder
		*	Le dossier de l'image.
		*\param[in] relative
		*	Le chemin d'accès à l'image, relatif à \p folder.
		*/
		C3D_API bool loadImage( castor::Path const & folder
			, castor::Path const & relative );
		/**
		*\copydoc	castor3d::SceneBackground::accept
		*/
		C3D_API void accept( BackgroundVisitor & visitor )const override;

	private:
		/**
		*\copydoc	castor3d::SceneBackground::doInitialiseShader
		*/
		ashes::PipelineShaderStageCreateInfoArray doInitialiseShader()override;
		/**
		*\copydoc	castor3d::SceneBackground::doInitialise
		*/
		bool doInitialise( ashes::RenderPass const & renderPass )override;
		/**
		*\copydoc	castor3d::SceneBackground::doCleanup
		*/
		void doCleanup()override;
		/**
		*\copydoc	castor3d::SceneBackground::doUpdate
		*/
		void doUpdate( Camera const & camera )override;

	private:
		void doInitialise2DTexture();

	private:
		TextureLayoutSPtr m_2dTexture;
		castor::Path m_2dTexturePath;
		Viewport m_viewport;
		float m_ratio;
	};
}

#endif
