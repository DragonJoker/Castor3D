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
			REQUIRE( m_texture );
			return *m_texture;
		}

		inline TextureLayout & getTexture()
		{
			REQUIRE( m_texture );
			return *m_texture;
		}

		inline renderer::Texture const & getImage()const
		{
			REQUIRE( m_texture );
			return m_texture->getTexture();
		}

		inline renderer::TextureView const & getView()const
		{
			REQUIRE( m_texture );
			return m_texture->getDefaultView();
		}
		/**@}*/

	private:
		/**
		*\copydoc	castor3d::SceneBackground::doInitialise
		*/
		bool doInitialise( renderer::RenderPass const & renderPass )override;
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
		renderer::BufferPtr< castor::Point4f > m_stagingBuffer;
		renderer::BufferImageCopyArray m_copyRegions;
		renderer::CommandBufferPtr m_cmdCopy;
		Viewport m_viewport;
	};
}

#endif
