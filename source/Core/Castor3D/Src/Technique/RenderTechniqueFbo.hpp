/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniqueFbo_H___
#define ___C3D_RenderTechniqueFbo_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0
	\date		19/12/2012
	\~english
	\brief		Internal struct holding a complete frame buffer.
	\~french
	\brief		Structure interne contenant un tampon d'image complet.
	*/
	struct RenderTechniqueFbo
	{
	public:
		explicit RenderTechniqueFbo( RenderTechnique & technique );
		bool initialise( castor::Size size );
		void cleanup();

		//!\~english	The texture receiving the color render.
		//!\~french		La texture recevant le rendu couleur.
		TextureLayoutSPtr m_colourTexture;
		//!\~english	The buffer receiving the depth render.
		//!\~french		Le tampon recevant le rendu profondeur.
		TextureLayoutSPtr m_depthBuffer;
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		renderer::FrameBufferPtr m_frameBuffer;

	private:
		RenderTechnique & m_technique;
	};
}

#endif
