/*
See LICENSE file in root folder
*/
#ifndef ___GL_SHADER_OBJECT_H___
#define ___GL_SHADER_OBJECT_H___

#include "Common/GlObject.hpp"

#include <Shader/ShaderObject.hpp>

namespace GlRender
{
	class GlShaderObject
		: public castor3d::ShaderObject
		, public Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >
	{
		friend class GlShaderProgram;

		using ObjectType = Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >;
		using UIntStrMap = std::map< castor::String, uint32_t >;

	public:
		/**
		 * Constructor
		 */
		GlShaderObject( OpenGl & p_gl, GlShaderProgram & p_parent, castor3d::ShaderType p_type );
		/**
		 * Destructor
		 */
		virtual ~GlShaderObject();
		/**
		 *\copydoc		castor3d::ShaderObject::create
		 */
		virtual bool create();
		/**
		 *\copydoc		castor3d::ShaderObject::Destroy
		 */
		virtual void destroy();
		/**
		 *\copydoc		castor3d::ShaderObject::Compile
		 */
		virtual bool compile();
		/**
		 *\copydoc		castor3d::ShaderObject::Detach
		 */
		virtual void detach();
		/**
		 *\copydoc		castor3d::ShaderObject::attachTo
		 */
		virtual void attachTo( castor3d::ShaderProgram & p_program );

	private:
		/**
		 *\copydoc		castor3d::ShaderObject::doRetrieveCompilerLog
		 */
		virtual castor::String doRetrieveCompilerLog();
		/**
		 *\copydoc		castor3d::ShaderProgram::doCreateVariable
		 */
		std::shared_ptr< castor3d::PushUniform > doCreateUniform( castor3d::UniformType type
			, int occurences )override;

	protected:
		GlShaderProgram * m_shaderProgram;
		UIntStrMap m_mapParamsByName;
	};
}

#endif
