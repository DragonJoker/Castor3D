/* See LICENSE file in root folder */
#ifndef ___TRS_SHADER_PROGRAM_H___
#define ___TRS_SHADER_PROGRAM_H___

#include "Shader/TestProgramInputLayout.hpp"

#include <Shader/ShaderProgram.hpp>

namespace TestRender
{
	class TestShaderProgram
		: public castor3d::ShaderProgram
	{
	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 */
		explicit TestShaderProgram( TestRenderSystem & p_renderSystem );
		/**
		 *\brief		Destructor.
		 */
		virtual ~TestShaderProgram();
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

	private:
		/**
		 *\copydoc		castor3d::ShaderProgram::doCreateObject
		 */
		castor3d::ShaderObjectSPtr doCreateObject( castor3d::ShaderType p_type )override;

	private:
		TestProgramInputLayout m_layout;
	};
}

#endif
