/*
See LICENSE file in root folder
*/
#ifndef ___C3DGL_PROGRAM_INPUT_LAYOUT_H___
#define ___C3DGL_PROGRAM_INPUT_LAYOUT_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "Common/GlHolder.hpp"

#include <Shader/ProgramInputLayout.hpp>

namespace GlRender
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		03/02/2016
	\brief		ProgramInputLayout class.
	\remark		Holds the vertex buffer declaration, initialised by the shader program when compiled.
	*/
	class GlProgramInputLayout
		: public castor3d::ProgramInputLayout
		, public Holder
	{
	public:
		/**
		 *\brief		Constructor
		 *\param[in]	p_gl	The OpenGL APIs.
		 */
		GlProgramInputLayout( OpenGl & p_gl, castor3d::RenderSystem & renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 */
		~GlProgramInputLayout();
		/**
		 *\copydoc		castor3d::ProgramInputLayout::Initialise
		 */
		virtual bool initialise( castor3d::ShaderProgram const & p_program );
		/**
		 *\copydoc		castor3d::ProgramInputLayout::Cleanup
		 */
		virtual void cleanup();

	private:
		std::multimap< int, castor3d::BufferElementDeclaration > doListAttributes( castor3d::ShaderProgram const & p_program );
		void doListOther( castor3d::ShaderProgram const & p_program );
	};
}

#endif
