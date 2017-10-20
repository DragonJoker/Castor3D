/* See LICENSE file in root folder */
#ifndef ___C3DGL_PROGRAM_INPUT_LAYOUT_H___
#define ___C3DGL_PROGRAM_INPUT_LAYOUT_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Shader/ProgramInputLayout.hpp>

namespace TestRender
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		03/02/2016
	\brief		ProgramInputLayout class.
	\remark		Holds the vertex buffer declaration, initialised by the shader program when compiled.
	*/
	class TestProgramInputLayout
		: public castor3d::ProgramInputLayout
	{
	public:
		/**
		 *\brief		Constructor
		 *\param[in]	p_gl	The OpenGL APIs.
		 */
		TestProgramInputLayout( castor3d::RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 */
		~TestProgramInputLayout();
		/**
		 *\copydoc		castor3d::ProgramInputLayout::Initialise
		 */
		virtual bool initialise( castor3d::ShaderProgram const & p_program );
		/**
		 *\copydoc		castor3d::ProgramInputLayout::Cleanup
		 */
		virtual void cleanup();
	};
}

#endif
