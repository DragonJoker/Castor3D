/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GL_SHADER_OBJECT_H___
#define ___GL_SHADER_OBJECT_H___

#include "Common/GlObject.hpp"

#include <Shader/ShaderObject.hpp>

namespace GlRender
{
	class GlShaderObject
		: public Castor3D::ShaderObject
		, public Object< std::function< uint32_t() >, std::function< bool( uint32_t ) > >
	{
		friend class GlShaderProgram;

		using ObjectType = Object< std::function< uint32_t() >, std::function< bool( uint32_t ) > >;
		using UIntStrMap = std::map< Castor::String, uint32_t >;

	public:
		/**
		 * Constructor
		 */
		GlShaderObject( OpenGl & p_gl, GlShaderProgram * p_parent, Castor3D::eSHADER_TYPE p_type );
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
		 *\brief		Dit si le shader compilé a le paramètre donné
		 *\param[in]	p_name	Le nom du paramètre
		 *\return		\p true si le paramètre a été trouvé
		 */
		virtual bool HasParameter( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Gives the wanted parameter from compiled shader
		 *\param[in]	p_name	The parameter name
		 *\return		\p nullptr if not found or if shader isn't compiled
		 *\~french
		 *\brief		Récupère le paramètre à partir du shader compilé
		 *\param[in]	p_name	Le nom du paramètre
		 *\return		\p nullptr si le paramètre n'a pas été trouvé ou si le shader n'est pas compilé
		 */
		uint32_t GetParameter( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Defines the given parameter value
		 *\param[in]	p_name	The parameter name
		 *\param[in]	p_value	The parameter value
		 *\~french
		 *\brief		Définit la valeur du paramètre
		 *\param[in]	p_name	Le nom du paramètre
		 *\param[in]	p_value	La valeur du paramètre
		 */
		virtual void SetParameter( Castor::String const & p_name, Castor::Matrix4x4r const & p_value );
		/**
		 *\~english
		 *\brief		Defines the given parameter value
		 *\param[in]	p_name	The parameter name
		 *\param[in]	p_value	The parameter value
		 *\~french
		 *\brief		Définit la valeur du paramètre
		 *\param[in]	p_name	Le nom du paramètre
		 *\param[in]	p_value	La valeur du paramètre
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
