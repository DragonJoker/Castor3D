/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___Cg_ShaderObject___
#define ___Cg_ShaderObject___

#include "CgShaderProgram.hpp"
#include <Castor3D/ShaderObject.hpp>

namespace CgShader
{
	//! Shader object
	/*!
	Cg Shader object representation, can be vertex, fragment or geometry shader
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CgShaderObject : public Castor3D::ShaderObjectBase
	{
		friend class CgShaderProgram;
		typedef std::map< Castor::String, CGparameter > ParamStrMap;

	protected:
		CGprofile			m_cgProfile;		//!< Cg Shader profile
		CGprogram			m_cgProgram;		//!< Cg Shader program
		Castor::String		m_compilerLog;		//!< Compiler log
		Castor::String		m_strEntryPoint;
		ParamStrMap			m_mapParamsByName;

	public:
		/**
		 * Constructor
		 */
		CgShaderObject( CgShaderProgram * p_pParent, Castor3D::eSHADER_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~CgShaderObject();
		/**
		 *\~english
		 *\brief		Tells if the compiled shader has the given parameter
		 *\param[in]	p_strName	The parameter name
		 *\return		\p true if the parameter was found
		 *\~french
		 *\brief		Dit si le shader compilé a le paramètre donné
		 *\param[in]	p_strName	Le nom du paramètre
		 *\return		\p true si le paramètre a été trouvé
		 */
		virtual bool HasParameter( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Gives the wanted parameter from compiled shader
		 *\param[in]	p_strName	The parameter name
		 *\return		\p NULL if not found or if shader isn't compiled
		 *\~french
		 *\brief		Récupère le paramètre à partir du shader compilé
		 *\param[in]	p_strName	Le nom du paramètre
		 *\return		\p NULL si le paramètre n'a pas été trouvé ou si le shader n'est pas compilé
		 */
		CGparameter GetParameter( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Defines the given parameter value
		 *\param[in]	p_strName	The parameter name
		 *\param[in]	p_mtxValue	The parameter value
		 *\~french
		 *\brief		Définit la valeur du paramètre
		 *\param[in]	p_strName	Le nom du paramètre
		 *\param[in]	p_mtxValue	La valeur du paramètre
		 */
		virtual void SetParameter( Castor::String const & p_strName, Castor::Matrix4x4r const & p_mtxValue );
		/**
		 *\~english
		 *\brief		Defines the given parameter value
		 *\param[in]	p_strName	The parameter name
		 *\param[in]	p_mtxValue	The parameter value
		 *\~french
		 *\brief		Définit la valeur du paramètre
		 *\param[in]	p_strName	Le nom du paramètre
		 *\param[in]	p_mtxValue	La valeur du paramètre
		 */
		virtual void SetParameter( Castor::String const & p_strName, Castor::Matrix3x3r const & p_mtxValue );

		virtual	void				SetEntryPoint		( Castor::String const & p_strName )	{ m_strEntryPoint = p_strName;	}
		inline	CGprofile			GetProfile			()const									{ return m_cgProfile;			}
		inline	CGprogram			GetProgram			()const									{ return m_cgProgram;			}
		virtual	Castor::String		GetEntryPoint		()const									{ return m_strEntryPoint;		}
	};
}

#endif
