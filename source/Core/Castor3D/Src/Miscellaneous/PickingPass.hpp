/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_PICKING_PASS_H___
#define ___C3D_PICKING_PASS_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Render/RenderPass.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Geometry.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Picking pass, using FBO.
	\~french
	\brief		Passe de picking, utilisant les FBO.
	*/
	class PickingPass
		: public RenderPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API PickingPass( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~PickingPass();
		/**
		 *\~english
		 *\brief		Initialises the pipeline, FBO and program.
		 *\param		p_size	The FBO dimensions.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Initialise le pipeline, le FBO et le programme.
		 *\param		p_size	Les dimensions du FBO.
		 *\return		\p true if ok.
		 */
		C3D_API bool Initialise( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Picks a geometry at given mouse position.
		 *\param[in]	p_position		The position in the pass.
		 *\param[in]	p_camera		The viewing camera.
		 *\return		The picked geometry, nullptr if none.
		 *\~french
		 *\brief		Sélectionne la géométrie à la position de souris donnée.
		 *\param[in]	p_position		La position dans la passe.
		 *\param[in]	p_camera		La caméra regardant la scène.
		 *\return		La géométrie sélectionnée, nullptr si aucune.
		 */
		C3D_API GeometrySPtr Pick( Castor::Position const & p_position, Camera const & p_camera );

	private:
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_flags	A combination of TextureChannel
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés
		 *\param[in]	p_flags	Une combinaison de TextureChannel
		 */
		C3D_API Castor::String DoGetPixelShaderSource( uint32_t p_flags )const override;

	private:
		//!\~english	The scenes, and cameras used to render them.
		//!\~french		Les scènes, et les caméras utilisées pour les dessiner.
		std::map< Castor::String, GeometryWPtr > m_pickable;
		//!\~english	The pipeline used to render opaque nodes.
		//!\~french		Le pipeline de rendu utilisé pour dessiner les noeuds opaques.
		PipelineSPtr m_pipeline;
		//!\~english	The texture receiving the color render.
		//!\~french		La texture recevant le rendu couleur.
		TextureLayoutSPtr m_colourTexture;
		//!\~english	The buffer receiving the depth render.
		//!\~french		Le tampon recevant le rendu profondeur.
		DepthStencilRenderBufferSPtr m_depthBuffer;
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The attach between texture and main frame buffer.
		//!\~french		L'attache entre la texture et le tampon principal.
		TextureAttachmentSPtr m_colourAttach;
		//!\~english	The attach between depth buffer and main frame buffer.
		//!\~french		L'attache entre le tampon profondeur et le tampon principal.
		RenderBufferAttachmentSPtr m_depthAttach;
		//!\~english	The render queue.
		//!\~french		La file de rendu.
		RenderQueue m_renderQueue;
	};
}

#endif
