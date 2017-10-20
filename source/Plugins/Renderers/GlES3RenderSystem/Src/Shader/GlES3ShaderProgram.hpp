/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_SHADER_PROGRAM_H___
#define ___C3DGLES3_SHADER_PROGRAM_H___

#include "Common/GlES3Object.hpp"
#include "Shader/GlES3ProgramInputLayout.hpp"

#include <Shader/ShaderProgram.hpp>

namespace GlES3Render
{
	class GlES3ShaderProgram
		: public Castor3D::ShaderProgram
		, public Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >
	{
		using ObjectType = Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >;

	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 */
		GlES3ShaderProgram( OpenGlES3 & p_gl, GlES3RenderSystem & p_renderSystem );
		/**
		 *\brief		Destructor.
		 */
		virtual ~GlES3ShaderProgram();
		/**
		 *\copydoc		Castor3D::ShaderProgram::Initialise
		 */
		bool Initialise()override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::Cleanup
		 */
		void Cleanup()override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::Bind
		 */
		void Bind()const override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::Unbind
		 */
		void Unbind()const override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::Link
		 */
		bool Link()override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::GetLayout
		 */
		Castor3D::ProgramInputLayout const & GetLayout()const override
		{
			return m_layout;
		}
		/**
		 *\copydoc		Castor3D::ShaderProgram::GetLayout
		 */
		Castor3D::ProgramInputLayout & GetLayout()override
		{
			return m_layout;
		}
		/**
		 *\param[in]	p_name	The attribute name.
		 *\return		The given attribute location in program.
		 */
		int GetAttributeLocation( Castor::String const & p_name )const;

	private:
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateObject
		 */
		Castor3D::ShaderObjectSPtr DoCreateObject( Castor3D::ShaderType p_type )override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateVariable
		 */
		std::shared_ptr< Castor3D::PushUniform > DoCreateUniform( Castor3D::UniformType p_type, int p_occurences )override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoGetVertexShaderSource
		 */
		Castor::String DoRetrieveLinkerLog();

	private:
		void DoBindTransformLayout();

	private:
		Castor::String m_linkerLog;
		GlES3ProgramInputLayout m_layout;
	};
}

#endif
