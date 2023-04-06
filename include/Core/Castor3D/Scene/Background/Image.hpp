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
			, castor::String const & name = castor::String{} );
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
		/**
		*\copydoc	castor3d::SceneBackground::accept
		*/
		C3D_API void accept( PipelineVisitor & visitor )override;
		/**
		*\copydoc	castor3d::SceneBackground::write
		*/
		C3D_API bool write( castor::String const & tabs
			, castor::Path const & folder
			, castor::StringStream & stream )const override;
		/**
		*\copydoc	castor3d::SceneBackground::getModelName
		*/
		castor::String const & getModelName()const override;

		castor::Path const & getImagePath()const noexcept
		{
			return m_2dTexturePath;
		}

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup()override;
		void doCpuUpdate( CpuUpdater & updater )const override;
		void doGpuUpdate( GpuUpdater & updater )const override;
		void doAddPassBindings( crg::FramePass & pass
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const override;
		void doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const override;
		void doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const override;
		void doInitialise2DTexture( RenderDevice const & device
			, QueueData const & queueData );

	private:
		TextureLayoutUPtr m_2dTexture;
		castor::Path m_2dTexturePath;
		float m_ratio;
	};
}

#endif
