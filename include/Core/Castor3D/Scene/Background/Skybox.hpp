/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkyboxBackground_H___
#define ___C3D_SkyboxBackground_H___

#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/PBR/IblTextures.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"

namespace castor3d
{
	class SkyboxBackground
		: public SceneBackground
	{
	public:
		/**
		*\~english
		*\brief
		*	Skybox loader.
		*\~english
		*\brief
		*	Loader de Skybox.
		*/
		class TextWriter
			: public castor::TextWriter< SkyboxBackground >
		{
		public:
			/**
			*\~english
			*\brief
			*	Constructor.
			*\~french
			*\brief
			*	Constructeur.
			*/
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			*\~english
			*\brief
			*	Writes a Skybox into a text file.
			*\param[in] obj
			*	The Skybox to save.
			*\param[in] file
			*	The file to write the Skybox in.
			*\~french
			*\brief
			*	Ecrit une Skybox dans un fichier texte.
			*\param[in] obj
			*	La Skybox.
			*\param[in] file
			*	Le fichier.
			*/
			C3D_API bool operator()( SkyboxBackground const & obj
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
		C3D_API explicit SkyboxBackground( Engine & engine
			, Scene & scene );
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
		C3D_API void accept( BackgroundVisitor & visitor )const override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline castor::Path const & getEquiTexturePath()const
		{
			return m_equiTexturePath;
		}

		inline castor::Size const & getEquiSize()const
		{
			return m_equiSize;
		}

		inline castor::Path const & getCrossTexturePath()const
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
		inline void setTexture( TextureLayoutSPtr texture )
		{
			m_texture = std::move( texture );
		}
		/**@}*/

	private:
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
		bool doInitialiseTexture();
		void doInitialiseEquiTexture();
		void doInitialiseCrossTexture();

	private:
		TextureLayoutSPtr m_equiTexture;
		castor::Path m_equiTexturePath;
		castor::Size m_equiSize;
		TextureLayoutSPtr m_crossTexture;
		castor::Path m_crossTexturePath;
		Viewport m_viewport;
	};
}

#endif
