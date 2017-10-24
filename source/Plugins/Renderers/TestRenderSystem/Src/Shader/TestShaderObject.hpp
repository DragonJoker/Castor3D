/* See LICENSE file in root folder */
#ifndef ___TRS_SHADER_OBJECT_H___
#define ___TRS_SHADER_OBJECT_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Shader/ShaderObject.hpp>

namespace TestRender
{
	class TestShaderObject
		: public castor3d::ShaderObject
	{
		friend class TestShaderProgram;

		using UIntStrMap = std::map< castor::String, uint32_t >;

	public:
		/**
		 * Constructor
		 */
		TestShaderObject( TestShaderProgram * p_parent, castor3d::ShaderType p_type );
		/**
		 * Destructor
		 */
		~TestShaderObject();
		/**
		 *\copydoc		castor3d::ShaderObject::create
		 */
		bool create()override;
		/**
		 *\copydoc		castor3d::ShaderObject::Destroy
		 */
		void destroy()override;
		/**
		 *\copydoc		castor3d::ShaderObject::Compile
		 */
		bool compile()override;
		/**
		 *\copydoc		castor3d::ShaderObject::Detach
		 */
		void detach()override;
		/**
		 *\copydoc		castor3d::ShaderObject::attachTo
		 */
		void attachTo( castor3d::ShaderProgram & p_program )override;

	private:
		/**
		 *\copydoc		castor3d::ShaderObject::doRetrieveCompilerLog
		 */
		castor::String doRetrieveCompilerLog()override;
		/**
		 *\copydoc		castor3d::ShaderObject::doCreateUniform
		 */
		std::shared_ptr< castor3d::PushUniform > doCreateUniform( castor3d::UniformType p_type, int p_occurences )override;
	};
}

#endif
