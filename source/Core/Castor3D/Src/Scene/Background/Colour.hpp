/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ColourBackground_H___
#define ___C3D_ColourBackground_H___

#include "Scene/Background/Background.hpp"

#include <Buffer/Buffer.hpp>
#include <Design/ChangeTracked.hpp>

namespace castor3d
{
	/**
	*\version
	*	0.9.0
	*\~english
	*\brief
	*	A skybox with no image but a colour.
	*\~french
	*\brief
	*	Une skybox sans image mais avec une couleur.
	*/
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
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] engine
		*	Le moteur.
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

		inline ashes::Texture const & getImage()const
		{
			CU_Require( m_texture );
			return m_texture->getTexture();
		}

		inline ashes::TextureView const & getView()const
		{
			CU_Require( m_texture );
			return m_texture->getDefaultView();
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
		*\copydoc	castor3d::SceneBackground::doUpdate
		*/
		void doUpdate( Camera const & camera )override;

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
