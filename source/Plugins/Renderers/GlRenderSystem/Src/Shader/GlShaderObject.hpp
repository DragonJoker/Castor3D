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
#ifndef ___GL_SHADER_OBJECT_H___
#define ___GL_SHADER_OBJECT_H___

#include "Common/GlObject.hpp"

#include <Shader/ShaderObject.hpp>

namespace GlRender
{
	class GlShaderObject
		: public Castor3D::ShaderObject
		, public Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >
	{
		friend class GlShaderProgram;

		using ObjectType = Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >;
		using UIntStrMap = std::map< Castor::String, uint32_t >;

	public:
		/**
		 * Constructor
		 */
		GlShaderObject( OpenGl & p_gl, GlShaderProgram * p_parent, Castor3D::ShaderType p_type );
		/**
		 * Destructor
		 */
		virtual ~GlShaderObject();
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
		/**
		 *\~english
		 *\brief		Tells if the compiled shader has the given parameter
		 *\param[in]	p_name	The parameter name
		 *\return		\p true if the parameter was found
		 *\~french
		 *\brief		Dit si le shader compil� a le param�tre donn�
		 *\param[in]	p_name	Le nom du param�tre
		 *\return		\p true si le param�tre a �t� trouv�
		 */
		virtual bool HasParameter( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Gives the wanted parameter from compiled shader
		 *\param[in]	p_name	The parameter name
		 *\return		\p nullptr if not found or if shader isn't compiled
		 *\~french
		 *\brief		R�cup�re le param�tre � partir du shader compil�
		 *\param[in]	p_name	Le nom du param�tre
		 *\return		\p nullptr si le param�tre n'a pas �t� trouv� ou si le shader n'est pas compil�
		 */
		uint32_t GetParameter( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Defines the given parameter value
		 *\param[in]	p_name	The parameter name
		 *\param[in]	p_value	The parameter value
		 *\~french
		 *\brief		D�finit la valeur du param�tre
		 *\param[in]	p_name	Le nom du param�tre
		 *\param[in]	p_value	La valeur du param�tre
		 */
		virtual void SetParameter( Castor::String const & p_name, Castor::Matrix4x4r const & p_value );
		/**
		 *\~english
		 *\brief		Defines the given parameter value
		 *\param[in]	p_name	The parameter name
		 *\param[in]	p_value	The parameter value
		 *\~french
		 *\brief		D�finit la valeur du param�tre
		 *\param[in]	p_name	Le nom du param�tre
		 *\param[in]	p_value	La valeur du param�tre
		 */
		virtual void SetParameter( Castor::String const & p_name, Castor::Matrix3x3r const & p_value );

	private:
		/**
		 *\copydoc		Castor3D::ShaderObject::DoRetrieveCompilerLog
		 */
		virtual Castor::String DoRetrieveCompilerLog();

	protected:
		GlShaderProgram * m_shaderProgram;
		UIntStrMap m_mapParamsByName;
	};
}

#endif
