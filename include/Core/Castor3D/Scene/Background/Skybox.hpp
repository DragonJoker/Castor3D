/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkyboxBackground_H___
#define ___C3D_SkyboxBackground_H___

#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
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
		*\copydoc	castor3d::SceneBackground::accept
		*/
		C3D_API void accept( BackgroundVisitor & visitor )override;
		/**
		*\copydoc	castor3d::SceneBackground::accept
		*/
		C3D_API void accept( ConfigurationVisitorBase & visitor )override;
		/**
		*\copydoc	castor3d::SceneBackground::write
		*/
		C3D_API bool write( castor::String const & tabs
			, castor::Path const & folder
			, castor::StringStream & stream )const override;
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
		C3D_API void setLeftImage( castor::Path const & folder
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
		C3D_API void setRightImage( castor::Path const & folder
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
		C3D_API void setTopImage( castor::Path const & folder
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
		C3D_API void setBottomImage( castor::Path const & folder
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
		C3D_API void setFrontImage( castor::Path const & folder
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
		C3D_API void setBackImage( castor::Path const & folder
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
		C3D_API void setFaceTexture( castor::Path const & folder
			, castor::Path const & relative
			, CubeMapFace face );
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
		C3D_API void setEquiTexture( castor::Path const & folder
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
		C3D_API void setCrossTexture( castor::Path const & folder
			, castor::Path const & relative );
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

		std::array< castor::Path, 6u > const & getLayerTexturePath()const
		{
			return m_layerTexturePath;
		}
		/**@}*/

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup()override;
		void doCpuUpdate( CpuUpdater & updater )const override;
		void doGpuUpdate( GpuUpdater & updater )const override;
		void doUpload( UploadData & uploader )override;
		void doAddPassBindings( crg::FramePass & pass
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const override;
		void doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const override;
		void doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const override;
		bool doInitialiseTexture( RenderDevice const & device );
		void doInitialiseLayerTexture( RenderDevice const & device );
		void doInitialiseEquiTexture( RenderDevice const & device );

	private:
		std::array< castor::ImageUPtr, 6u > m_layerTexture;
		std::array< castor::Path, 6u > m_layerTexturePath;
		TextureLayoutUPtr m_equiTexture;
		castor::Path m_equiTexturePath;
		castor::Size m_equiSize;
		castor::Path m_crossTexturePath;

		static castor::ImageUPtr copyCrossImageFace( Engine & engine
			, castor::StringView faceName
			, castor::Image const & lines
			, uint32_t index );
		static std::array< castor::ImageUPtr, 6u > splitCrossImageBuffer( Engine & engine
			, castor::Image const & cross );
	};
}

#endif
