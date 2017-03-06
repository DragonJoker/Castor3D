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
#ifndef ___TRS_TRANSFORM_FEEDBACK_H___
#define ___TRS_TRANSFORM_FEEDBACK_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Miscellaneous/TransformFeedback.hpp>

namespace TestRender
{
	/*!
	\author 	Sylvain DOREMUS
	\version 	0.9.0
	\date 		19/10/2016
	\~english
	\brief		Transform feedback implementation.
	\~french
	\brief		Implémentation du Transform feedback.
	*/
	class TestTransformFeedback
		: public Castor3D::TransformFeedback
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderSystem	The RenderSystem
		 *\param[in]	m_declaration	The buffer elements declaration.
		 *\param[in]	m_program		The shader program.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	m_declaration	La déclaration des éléments du tampon.
		 *\param[in]	m_program		Le programm shader.
		 */
		TestTransformFeedback( TestRenderSystem & p_renderSystem, Castor3D::BufferDeclaration const & p_computed, Castor3D::Topology p_topology, Castor3D::ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~TestTransformFeedback();

	private:
		/**
		 *\copydoc		Castor3D::TransformFeedback::Initialise
		 */
		bool DoInitialise()override;
		/**
		 *\copydoc		Castor3D::TransformFeedback::Cleanup
		 */
		void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::TransformFeedback::Bind
		 */
		void DoBind()const override;
		/**
		 *\copydoc		Castor3D::TransformFeedback::Unbind
		 */
		void DoUnbind()const override;
		/**
		 *\copydoc		Castor3D::TransformFeedback::DoBegin
		 */
		void DoBegin()const override;
		/**
		 *\copydoc		Castor3D::TransformFeedback::DoEnd
		 */
		void DoEnd()const override;
	};
}

#endif
