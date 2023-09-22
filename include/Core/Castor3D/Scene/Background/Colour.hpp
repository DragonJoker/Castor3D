/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ColourBackground_H___
#define ___C3D_ColourBackground_H___

#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Graphics/HdrColourComponent.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/StagingTexture.hpp>

namespace castor3d
{
	class ColourBackground
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
		*	The scene.
		*\param name
		*	The background name.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] engine
		*	Le moteur.
		*\param[in] scene
		*	La scène.
		*\param name
		*	Le nom du fond.
		*/
		C3D_API ColourBackground( Engine & engine
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
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline castor::HdrRgbColour const & getColour()const
		{
			return m_colour.value();
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
			, VkShaderStageFlags shaderStages
			, uint32_t & index )const override;
		void doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const override;

	private:
		mutable castor::GroupChangeTracked< castor::HdrRgbColour > m_colour;
		castor::PxBufferBaseUPtr m_buffer;
	};
}

#endif
