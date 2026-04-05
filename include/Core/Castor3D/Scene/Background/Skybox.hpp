/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkyboxBackground_H___
#define ___C3D_SkyboxBackground_H___

#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/PBR/IblTextures.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace c3d
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
			, String const & name = String{} );
		/**
		*\copydoc	SceneBackground::accept
		*/
		C3D_API void accept( BackgroundVisitor & visitor )override;
		/**
		*\copydoc	SceneBackground::accept
		*/
		C3D_API void accept( ConfigurationVisitorBase & visitor )override;
		/**
		*\copydoc	SceneBackground::write
		*/
		C3D_API bool write( String const & tabs
			, Path const & folder
			, StringStream & stream )const override;
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
		C3D_API void setLeftImage( Path const & folder
			, Path const & relative );
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
		C3D_API void setRightImage( Path const & folder
			, Path const & relative );
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
		C3D_API void setTopImage( Path const & folder
			, Path const & relative );
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
		C3D_API void setBottomImage( Path const & folder
			, Path const & relative );
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
		C3D_API void setFrontImage( Path const & folder
			, Path const & relative );
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
		C3D_API void setBackImage( Path const & folder
			, Path const & relative );
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
		C3D_API void setFaceTexture( Path const & folder
			, Path const & relative
			, SkyboxFace face );
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
		C3D_API void setEquiTexture( Path const & folder
			, Path const & relative
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
		C3D_API void setCrossTexture( Path const & folder
			, Path const & relative );

		C3D_API static void addParsers( AttributeParsers & result );
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/

		void setOrientation( Quaternion orientation )noexcept
		{
			m_orientation = c3d::move( orientation );
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Path const & getEquiTexturePath()const
		{
			return m_equiTexturePath;
		}

		Size const & getEquiSize()const
		{
			return m_equiSize;
		}

		Path const & getCrossTexturePath()const
		{
			return m_crossTexturePath;
		}

		Array< Path, 6u > const & getLayerTexturePath()const
		{
			return m_layerTexturePath;
		}

		Quaternion const & getOrientation()const
		{
			return m_orientation;
		}
		/**@}*/

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup()override;
		void doCpuUpdate( CpuUpdater & updater )override;
		void doGpuUpdate( GpuUpdater & updater )const override;
		void doUpload( UploadData & uploader )override;
		void doAddPassBindings( crg::FramePass & pass
			, Texture * targetImage
			, uint32_t & index )const override;
		void doAddLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index )const override;
		void doAddDescriptorWrites( ashes::WriteDescriptorSetArray & descriptorWrites
			, Texture * targetImage
			, uint32_t & index )const override;
		bool doInitialiseTexture( RenderDevice const & device );
		void doInitialiseLayerTexture( RenderDevice const & device );
		void doInitialiseEquiTexture( RenderDevice const & device );

	private:
		Array< ImageUPtr, 6u > m_layerTexture;
		Array< Path, 6u > m_layerTexturePath;
		TextureLayoutUPtr m_equiTexture;
		Path m_equiTexturePath;
		Size m_equiSize;
		Path m_crossTexturePath;
		Quaternion m_orientation{ Quaternion::identity() };

		static ImageUPtr copyCrossImageFace( StringView faceName
			, Image const & lines
			, uint32_t index );
		static Array< ImageUPtr, 6u > splitCrossImageBuffer( Image const & cross );
	};

	struct SkyboxContext
	{
		SkyboxBackgroundUPtr skybox{};
	};
}

#endif
