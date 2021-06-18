/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkyboxBackground_H___
#define ___C3D_SkyboxBackground_H___

#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Render/PBR/IblTextures.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	class SkyboxBackground
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
		*\param name
		*	The background name.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] engine
		*	Le moteur.
		*\param[in] scene
		*	La scène parente.
		*\param name
		*	Le nom du fond.
		*/
		C3D_API explicit SkyboxBackground( Engine & engine
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
		C3D_API virtual ~SkyboxBackground();
		/**
		*\~english
		*\brief
		*	Sets the skybox's left face texture.
		*\param[in] folder
		*	The image folder.
		*\param[in] relative
		*	The image file path, relative to \p folder.
		*\~french
		*\brief
		*	Définit la texture de la face gauche de la skybox.
		*\param[in] folder
		*	Le dossier de l'image.
		*\param[in] relative
		*	Le chemin d'accès à l'image, relatif à \p folder.
		*/
		C3D_API void loadLeftImage( castor::Path const & folder
			, castor::Path const & relative );
		/**
		*\~english
		*\brief
		*	Sets the skybox's right face texture.
		*\param[in] folder
		*	The image folder.
		*\param[in] relative
		*	The image file path, relative to \p folder.
		*\~french
		*\brief
		*	Définit la texture de la face droite de la skybox.
		*\param[in] folder
		*	Le dossier de l'image.
		*\param[in] relative
		*	Le chemin d'accès à l'image, relatif à \p folder.
		*/
		C3D_API void loadRightImage( castor::Path const & folder
			, castor::Path const & relative );
		/**
		*\~english
		*\brief
		*	Sets the skybox's top face texture.
		*\param[in] folder
		*	The image folder.
		*\param[in] relative
		*	The image file path, relative to \p folder.
		*\~french
		*\brief
		*	Définit la texture de la face du haut de la skybox.
		*\param[in] folder
		*	Le dossier de l'image.
		*\param[in] relative
		*	Le chemin d'accès à l'image, relatif à \p folder.
		*/
		C3D_API void loadTopImage( castor::Path const & folder
			, castor::Path const & relative );
		/**
		*\~english
		*\brief
		*	Sets the skybox's bottom face texture.
		*\param[in] folder
		*	The image folder.
		*\param[in] relative
		*	The image file path, relative to \p folder.
		*\~french
		*\brief
		*	Définit la texture de la face du bas de la skybox.
		*\param[in] folder
		*	Le dossier de l'image.
		*\param[in] relative
		*	Le chemin d'accès à l'image, relatif à \p folder.
		*/
		C3D_API void loadBottomImage( castor::Path const & folder
			, castor::Path const & relative );
		/**
		*\~english
		*\brief
		*	Sets the skybox's front face texture.
		*\param[in] folder
		*	The image folder.
		*\param[in] relative
		*	The image file path, relative to \p folder.
		*\~french
		*\brief
		*	Définit la texture de la face avant de la skybox.
		*\param[in] folder
		*	Le dossier de l'image.
		*\param[in] relative
		*	Le chemin d'accès à l'image, relatif à \p folder.
		*/
		C3D_API void loadFrontImage( castor::Path const & folder
			, castor::Path const & relative );
		/**
		*\~english
		*\brief
		*	Sets the skybox's back face texture.
		*\param[in] folder
		*	The image folder.
		*\param[in] relative
		*	The image file path, relative to \p folder.
		*\~french
		*\brief
		*	Définit la texture de la face arrière de la skybox.
		*\param[in] folder
		*	Le dossier de l'image.
		*\param[in] relative
		*	Le chemin d'accès à l'image, relatif à \p folder.
		*/
		C3D_API void loadBackImage( castor::Path const & folder
			, castor::Path const & relative );
		/**
		*\~english
		*\brief
		*	Sets a skybox's face texture.
		*\param[in] folder
		*	The image folder.
		*\param[in] relative
		*	The image file path, relative to \p folder.
		*\param[in] face
		*	The face into which the image will be loaded.
		*\~french
		*\brief
		*	Définit la texture d'une face de la skybox.
		*\param[in] folder
		*	Le dossier de l'image.
		*\param[in] relative
		*	Le chemin d'accès à l'image, relatif à \p folder.
		*\param[in] face
		*	La face dans laquelle l'image sera chargée.
		*/
		C3D_API void loadFaceImage( castor::Path const & folder
			, castor::Path const & relative
			, CubeMapFace face );
		/**
		*\~english
		*\brief
		*	Sets a skybox's face texture.
		*\param[in] folder
		*	The image folder.
		*\param[in] relative
		*	The image file path, relative to \p folder.
		*\param[in] face
		*	The face into which the image will be loaded.
		*\~french
		*\brief
		*	Définit la texture d'une face de la skybox.
		*\param[in] folder
		*	Le dossier de l'image.
		*\param[in] relative
		*	Le chemin d'accès à l'image, relatif à \p folder.
		*\param[in] face
		*	La face dans laquelle l'image sera chargée.
		*/
		C3D_API void setFaceTexture( castor::Path const & folder
			, castor::Path const & relative
			, CubeMapFace face );
		/**
		*\~english
		*\brief
		*	Sets the skybox's equirectangular texture.
		*\param[in] texture
		*	The texture.
		*\param[in] size
		*	The wanted skybox dimensions.
		*\~french
		*\brief
		*	Définit la texture équirectangulaire de la skybox.
		*\param[in] texture
		*	La texture.
		*\param[in] size
		*	Les dimensions voulues pour la skybox.
		*/
		C3D_API void setEquiTexture( TextureLayoutSPtr texture
			, uint32_t size );
		/**
		*\~english
		*\brief
		*	Sets the skybox's equirectangular texture.
		*\param[in] folder
		*	The image folder.
		*\param[in] relative
		*	The image file path, relative to \p folder.
		*\param[in] size
		*	The wanted skybox dimensions.
		*\~french
		*\brief
		*	Définit la texture équirectangulaire de la skybox.
		*\param[in] folder
		*	Le dossier de l'image.
		*\param[in] relative
		*	Le chemin d'accès à l'image, relatif à \p folder.
		*\param[in] size
		*	Les dimensions voulues pour la skybox.
		*/
		C3D_API void loadEquiTexture( castor::Path const & folder
			, castor::Path const & relative
			, uint32_t size );
		/**
		*\~english
		*\brief
		*	Sets the size of the skybox generated from the equirectangular texture.
		*\param[in] size
		*	The wanted skybox dimensions.
		*\~french
		*\brief
		*	Définit les dimensions de la skybox extraite depuis la texture équirectangulaire de la skybox.
		*\param[in] size
		*	Les dimensions voulues pour la skybox.
		*/
		C3D_API void setEquiSize( uint32_t size );
		/**
		*\~english
		*\brief
		*	Sets the skybox's cross texture.
		*\param[in] folder
		*	The image folder.
		*\param[in] relative
		*	The image file path, relative to \p folder.
		*\~french
		*\brief
		*	Définit la texture croix de la skybox.
		*\param[in] folder
		*	Le dossier de l'image.
		*\param[in] relative
		*	Le chemin d'accès à l'image, relatif à \p folder.
		*/
		C3D_API void loadCrossTexture( castor::Path const & folder
			, castor::Path const & relative );
		/**
		*\~english
		*\brief
		*	Sets the skybox's cross texture.
		*\param[in] texture
		*	The texture.
		*\~french
		*\brief
		*	Définit la texture croix de la skybox.
		*\param[in] texture
		*	La texture.
		*/
		C3D_API void setCrossTexture( TextureLayoutSPtr texture );
		/**
		*\copydoc	castor3d::SceneBackground::accept
		*/
		C3D_API void accept( BackgroundVisitor & visitor )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		castor::Path const & getEquiTexturePath()const
		{
			return m_equiTexturePath;
		}

		castor::Size const & getEquiSize()const
		{
			return m_equiSize;
		}

		castor::Path const & getCrossTexturePath()const
		{
			return m_crossTexturePath;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void setTexture( TextureLayoutSPtr texture )
		{
			m_texture = std::move( texture );
		}
		/**@}*/

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup()override;
		void doCpuUpdate( CpuUpdater & updater )override;
		void doGpuUpdate( GpuUpdater & updater )override;
		bool doInitialiseTexture( RenderDevice const & device );
		void doInitialiseLayerTexture( RenderDevice const & device );
		void doInitialiseEquiTexture( RenderDevice const & device );
		void doInitialiseCrossTexture( RenderDevice const & device );

	private:
		std::array< TextureLayoutSPtr, 6u > m_layerTexture;
		std::array< castor::Path, 6u > m_layerTexturePath;
		TextureLayoutSPtr m_equiTexture;
		castor::Path m_equiTexturePath;
		castor::Size m_equiSize;
		TextureLayoutSPtr m_crossTexture;
		castor::Path m_crossTexturePath;
		Viewport m_viewport;
	};
}

#endif
