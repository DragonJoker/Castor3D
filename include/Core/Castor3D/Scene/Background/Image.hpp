/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ImageBackground_H___
#define ___C3D_ImageBackground_H___

#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	class ImageBackground
		: public SceneBackground
	{
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
			, Scene & scene
			, castor::String const & name = castor::String{} );
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
		C3D_API void accept( BackgroundVisitor & visitor )override;

	private:
		/**
		*\copydoc	castor3d::SceneBackground::doInitialiseShader
		*/
		ashes::PipelineShaderStageCreateInfoArray doInitialiseShader( RenderDevice const & device )override;
		/**
		*\copydoc	castor3d::SceneBackground::doInitialise
		*/
		bool doInitialise( RenderDevice const & device
			, ashes::RenderPass const & renderPass )override;
		/**
		*\copydoc	castor3d::SceneBackground::doCleanup
		*/
		void doCleanup()override;
		/**
		*\copydoc	castor3d::SceneBackground::doCpuUpdate
		*/
		void doCpuUpdate( CpuUpdater & updater )override;
		/**
		*\copydoc	castor3d::SceneBackground::doGpuUpdate
		*/
		void doGpuUpdate( GpuUpdater & updater )override;

	private:
		void doInitialise2DTexture( RenderDevice const & device );

	private:
		TextureLayoutSPtr m_2dTexture;
		castor::Path m_2dTexturePath;
		Viewport m_viewport;
		float m_ratio;
	};
}

#endif
