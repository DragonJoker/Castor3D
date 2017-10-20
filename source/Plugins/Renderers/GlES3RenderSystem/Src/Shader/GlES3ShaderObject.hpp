/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_SHADER_OBJECT_H___
#define ___C3DGLES3_SHADER_OBJECT_H___

#include "Common/GlES3Object.hpp"

#include <Shader/ShaderObject.hpp>

namespace GlES3Render
{
	class GlES3ShaderObject
		: public Castor3D::ShaderObject
		, public Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >
	{
		friend class GlES3ShaderProgram;

		using ObjectType = Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >;
		using UIntStrMap = std::map< Castor::String, uint32_t >;

	public:
		/**
		 * Constructor
		 */
		GlES3ShaderObject( OpenGlES3 & p_gl, GlES3ShaderProgram * p_parent, Castor3D::ShaderType p_type );
		/**
		 * Destructor
		 */
		virtual ~GlES3ShaderObject();
		/**
		 *\copydoc		Castor3D::ShaderObject::Create
		 */
		virtual bool Create();
		/**
		 *\copydoc		Castor3D::ShaderObject::Destroy
		 */
		virtual void Destroy();
		/**
		 *\copydoc		Castor3D::ShaderObject::Compile
		 */
		virtual bool Compile();
		/**
		 *\copydoc		Castor3D::ShaderObject::Detach
		 */
		virtual void Detach();
		/**
		 *\copydoc		Castor3D::ShaderObject::AttachTo
		 */
		virtual void AttachTo( Castor3D::ShaderProgram & p_program );

	private:
		/**
		 *\copydoc		Castor3D::ShaderObject::DoRetrieveCompilerLog
		 */
		virtual Castor::String DoRetrieveCompilerLog();

	protected:
		GlES3ShaderProgram * m_shaderProgram;
		UIntStrMap m_mapParamsByName;
	};
}

#endif
