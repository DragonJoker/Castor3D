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
#ifndef ___C3D_DIRECT_RENDER_TECHNIQUE_H___
#define ___C3D_DIRECT_RENDER_TECHNIQUE_H___

#include <Technique/RenderTechnique.hpp>

namespace Direct
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Basic render technique class
	\~french
	\brief		Classe de technique de rendu basique
	*/
	class RenderTechnique
		: public Castor3D::RenderTechnique
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderTarget	The render target for this technique
		 *\param[in]	p_renderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderTarget	La render target pour cette technique
		 *\param[in]	p_renderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 */
		RenderTechnique( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem & p_renderSystem, Castor3D::Parameters const & p_params );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~RenderTechnique();
		/**
		 *\~english
		 *\brief		Instantiation function, used by the factory to create objects of a wanted type
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_renderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\return		A clone of this object
		 *\~french
		 *\brief		Fonction d'instanciation, utilisée par la fabrique pour créer des objets d'un type donné
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_renderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		Un clône de cet objet
		 */
		static Castor3D::RenderTechniqueSPtr CreateInstance( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem & p_renderSystem, Castor3D::Parameters const & p_params );

	protected:
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoCreate
		 */
		bool DoCreate()override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoDestroy
		 */
		void DoDestroy()override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoInitialise
		 */
		bool DoInitialise( uint32_t & p_index )override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoCleanup
		 */
		void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoBeginRender
		 */
		void DoBeginRender()override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoBeginOpaqueRendering
		 */
		void DoBeginOpaqueRendering()override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoEndOpaqueRendering
		 */
		void DoEndOpaqueRendering()override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoBeginTransparentRendering
		 */
		void DoBeginTransparentRendering()override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoEndTransparentRendering
		 */
		void DoEndTransparentRendering()override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoEndRender
		 */
		void DoEndRender()override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoWriteInto
		 */
		bool DoWriteInto( Castor::TextFile & p_file )override;
	};
}

#endif
