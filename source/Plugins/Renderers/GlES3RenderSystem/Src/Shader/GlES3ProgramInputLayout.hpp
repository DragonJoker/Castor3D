/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_PROGRAM_INPUT_LAYOUT_H___
#define ___C3DGLES3_PROGRAM_INPUT_LAYOUT_H___

#include "GlES3RenderSystemPrerequisites.hpp"

#include "Common/GlES3Holder.hpp"

#include <Shader/ProgramInputLayout.hpp>

namespace GlES3Render
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		03/02/2016
	\brief		ProgramInputLayout class.
	\remark		Holds the vertex buffer declaration, initialised by the shader program when compiled.
	*/
	class GlES3ProgramInputLayout
		: public Castor3D::ProgramInputLayout
		, public Holder
	{
	public:
		/**
		 *\brief		Constructor
		 *\param[in]	p_gl	The OpenGL APIs.
		 */
		GlES3ProgramInputLayout( OpenGlES3 & p_gl, Castor3D::RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 */
		~GlES3ProgramInputLayout();
		/**
		 *\copydoc		Castor3D::ProgramInputLayout::Initialise
		 */
		virtual bool Initialise( Castor3D::ShaderProgram const & p_program );
		/**
		 *\copydoc		Castor3D::ProgramInputLayout::Cleanup
		 */
		virtual void Cleanup();

	private:
		std::multimap< int, Castor3D::BufferElementDeclaration > DoListAttributes( Castor3D::ShaderProgram const & p_program );
		void DoListOther( Castor3D::ShaderProgram const & p_program );
	};
}

#endif
