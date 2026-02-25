/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ImageBackground_H___
#define ___C3D_ImageBackground_H___

#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace c3d
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
		C3D_API explicit ImageBackground( Engine & engine
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
		C3D_API bool setImage( Path const & folder
			, Path const & relative );
		/**
		*\copydoc	SceneBackground::getModelName
		*/
		String const & getModelName()const override;

		static void addParsers( AttributeParsers & result );

		Path const & getImagePath()const noexcept
		{
			return m_2dTexturePath;
		}

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup()override;
		void doCpuUpdate( CpuUpdater & updater )const override;
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
		void doInitialise2DTexture( RenderDevice const & device );

	private:
		TextureLayoutUPtr m_2dTexture;
		Path m_2dTexturePath;
		float m_ratio;
	};
}

#endif
