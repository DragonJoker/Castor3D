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
		C3D_API void accept( PipelineVisitor & visitor )override;
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
		inline TextureLayout const & getTexture()const
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		inline TextureLayout & getTexture()
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		inline ashes::Image const & getImage()const
		{
			CU_Require( m_texture );
			return m_texture->getTexture();
		}

		inline ashes::ImageView const & getView()const
		{
			CU_Require( m_texture );
			return m_texture->getDefaultView().getSampledView();
		}

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
		void doAddPassBindings( crg::FramePass & pass
			, crg::ImageData const & targetImage
			, uint32_t & index )const override;
		void doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const override;
		void doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageData const & targetImage
			, uint32_t & index )const override;
		void doUpdateColour( QueueData const & queueData
			, RenderDevice const & device )const;

	private:
		mutable castor::ChangeTracked< castor::HdrRgbColour > m_colour;
		ashes::StagingTexturePtr m_stagingTexture;
		ashes::CommandBufferPtr m_cmdCopy;
	};
}

#endif
