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
		: public Castor::OwnedBy< RenderTarget >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderTarget	The target for this pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderTarget	La cible de la passe.
		 */
		C3D_API PickingPass( RenderTarget & p_renderTarget );
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
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Initialise le pipeline, le FBO et le programme.
		 *\return		\p true if ok.
		 */
		C3D_API bool Initialise();
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
		 *\param[in]	p_position		The position in the window.
		 *\param[in]	m_windowSize	The window dimensions.
		 *\param[in]	p_camera		The viewing camera.
		 *\return		The picked geometry, nullptr if none.
		 *\~french
		 *\brief		Sélectionne la géométrie à la position de souris donnée.
		 *\param[in]	p_position		La position dans la fenêtre.
		 *\param[in]	m_windowSize	Les dimensions de la fenêtre.
		 *\param[in]	p_camera		La caméra regardant la scène.
		 *\return		La géométrie sélectionnée, nullptr si aucune.
		 */
		C3D_API GeometrySPtr Pick( Castor::Position const & p_position, Castor::Size const & m_windowSize, Camera const & p_camera );
		/**
		 *\~english
		 *\brief		Adds a geometry that can be picked.
		 *\~french
		 *\brief		Ajoute une géométrie qui peut être sélectionnée.
		 */
		inline void AddPickable( GeometrySPtr p_geometry )
		{
			m_pickable.insert( { p_geometry->GetName(), p_geometry } );
		}
		/**
		 *\~english
		 *\brief		Removes a geometry from the list.
		 *\~french
		 *\brief		Supprime une géométrie de la liste.
		 */
		inline void RemovePickable( Castor::String const & p_name )
		{
			auto l_it = m_pickable.find( p_name );

			if ( l_it != m_pickable.end() )
			{
				m_pickable.erase( l_it );
			}
		}

	protected:
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
		//!\~english	The parent render target.
		//!\~french		La render target parente.
		RenderTarget & m_renderTarget;
		//!\~english	The scenes, and cameras used to render them.
		//!\~french		Les scènes, et les caméras utilisées pour les dessiner.
		std::map< Castor::String, GeometryWPtr > m_pickable;
		//!\~english	The pipeline used to render opaque nodes.
		//!\~french		Le pipeline de rendu utilisé pour dessiner les noeuds opaques.
		PipelineSPtr m_pipeline;
		//!\~english	The program used for picking.
		//!\~french		Le programme utilisé pour le picking.
		ShaderProgramSPtr m_program;
		//!\~english	The variable holding the geometry colour, in the program.
		//!\~french		La variable shader contenant la couleur de la géométrie.
		Point4fFrameVariableSPtr m_color;
	};
}

#endif
