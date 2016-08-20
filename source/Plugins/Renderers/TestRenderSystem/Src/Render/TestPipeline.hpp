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
#ifndef ___TRS_PIPELINE_H___
#define ___TRS_PIPELINE_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Render/Pipeline.hpp>

namespace TestRender
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Implementation of the rendering pipeline.
	\~french
	\brief		Impl�mentation du pipeline de rendu.
	*/
	class TestPipeline
		: public Castor3D::Pipeline
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The parent RenderSystem.
		 *\param[in]	p_rsState		The rateriser state.
		 *\param[in]	p_bdState		The blend state.
		 *\param[in]	p_msState		The multisample state.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le RenderSystem parent.
		 *\param[in]	p_rsState		L'état de rastériseur.
		 *\param[in]	p_bdState		L'état de mélange.
		 *\param[in]	p_msState		L'état de multi-échantillonnage.
		 */
		TestPipeline( TestRenderSystem & p_renderSystem
					  , Castor3D::RasteriserStateUPtr && p_rsState
					  , Castor3D::BlendStateUPtr && p_bdState
					  , Castor3D::MultisampleStateUPtr && p_msState );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~TestPipeline();
		/**
		 *\~english
		 *\brief		Applies the pipeline.
		 *\~french
		 *\brief		Applique le pipeline.
		 */
		void Apply()const override;
		/**
		 *\copydoc		Castor3D::Context::ApplyViewport
		 */
		virtual void ApplyViewport( int p_windowWidth, int p_windowHeight );
	};
}

#endif
