/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderDepthQuad_H___
#define ___C3D_RenderDepthQuad_H___

#include "RenderToTexture/RenderQuad.hpp"
#include "Shader/Program.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/03/2017
	\version	0.9.0
	\~english
	\brief		Class used to render depth textures.
	\~french
	\brief		Classe utilisée pour rendre les textures profondeur.
	*/
	class RenderDepthQuad
		: public RenderQuad
	{
	public:
		/**
		 *\~english
		 *\brief
		 *	Constructor.
		 *\param[in] renderSystem
		 *	The RenderSystem.
		 *\~french
		 *\brief
		 *	Constructeur.
		 *\param[in] renderSystem
		 *	Le RenderSystem.
		 */
		C3D_API explicit RenderDepthQuad( RenderSystem & renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderDepthQuad();
		/**
		 *\~english
		 *\brief		Initialises the program.
		 *\~french
		 *\brief		Initialise le programme.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Renders the given depth texture to the given frame buffer.
		 *\param[in]	renderPass	The render pass.
		 *\param[in]	frameBuffer	The frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\~french
		 *\brief		Rend la texture profondeur donnée dans le tampon d'image donné.
		 *\param[in]	renderPass	La passe de rendu.
		 *\param[in]	frameBuffer	Le frame buffer.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 */
		C3D_API void render( renderer::RenderPass const & renderPass
			, renderer::FrameBuffer const & frameBuffer
			, castor::Position const & position
			, castor::Size const & size
			, TextureLayout const & texture );

	private:
		ShaderProgram m_program;
	};
}

#endif
