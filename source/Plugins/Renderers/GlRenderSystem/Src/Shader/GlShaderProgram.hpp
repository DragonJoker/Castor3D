/*
See LICENSE file in root folder
*/
#ifndef ___GL_SHADER_PROGRAM_H___
#define ___GL_SHADER_PROGRAM_H___

#include "Common/GlObject.hpp"
#include "Shader/GlProgramInputLayout.hpp"

#include <Shader/ShaderProgram.hpp>

namespace GlRender
{
	class GlShaderProgram
		: public castor3d::ShaderProgram
		, public Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >
	{
		using ObjectType = Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >;

	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	renderSystem	The RenderSystem.
		 */
		GlShaderProgram( OpenGl & p_gl, GlRenderSystem & renderSystem );
		/**
		 *\brief		Destructor.
		 */
		virtual ~GlShaderProgram();
		/**
		 *\copydoc		castor3d::ShaderProgram::initialise
		 */
		bool initialise()override;
		/**
		 *\copydoc		castor3d::ShaderProgram::cleanup
		 */
		void cleanup()override;
		/**
		 *\copydoc		castor3d::ShaderProgram::bind
		 */
		void bind()const override;
		/**
		 *\copydoc		castor3d::ShaderProgram::unbind
		 */
		void unbind()const override;
		/**
		 *\copydoc		castor3d::ShaderProgram::link
		 */
		bool link()override;
		/**
		 *\copydoc		castor3d::ShaderProgram::getLayout
		 */
		castor3d::ProgramInputLayout const & getLayout()const override
		{
			return m_layout;
		}
		/**
		 *\copydoc		castor3d::ShaderProgram::getLayout
		 */
		castor3d::ProgramInputLayout & getLayout()override
		{
			return m_layout;
		}
		/**
		 *\param[in]	p_name	The attribute name.
		 *\return		The given attribute location in program.
		 */
		int getAttributeLocation( castor::String const & p_name )const;

	private:
		/**
		 *\copydoc		castor3d::ShaderProgram::doCreateObject
		 */
		castor3d::ShaderObjectSPtr doCreateObject( castor3d::ShaderType p_type )override;
		/**
		 *\copydoc		castor3d::ShaderProgram::doRetrieveLinkerLog
		 */
		castor::String doRetrieveLinkerLog();

	private:
		void doBindTransformLayout();

	private:
		castor::String m_linkerLog;
		GlProgramInputLayout m_layout;
	};
}

#endif
