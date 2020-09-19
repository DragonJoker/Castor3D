/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ColourBackground_H___
#define ___C3D_ColourBackground_H___

#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Graphics/HdrColourComponent.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

#include <ashespp/Buffer/Buffer.hpp>

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
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] engine
		*	Le moteur.
		*\param[in] scene
		*	La scène.
		*/
		C3D_API ColourBackground( Engine & engine
			, Scene & scene );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		C3D_API ~ColourBackground();
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
		/**
		*\copydoc	castor3d::SceneBackground::doInitialise
		*/
		bool doInitialise( ashes::RenderPass const & renderPass )override;
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
		void doUpdateColour();

	private:
		castor::ChangeTracked< castor::HdrRgbColour > m_colour;
		ashes::StagingTexturePtr m_stagingTexture;
		ashes::CommandBufferPtr m_cmdCopy;
		Viewport m_viewport;
	};
}

#endif
