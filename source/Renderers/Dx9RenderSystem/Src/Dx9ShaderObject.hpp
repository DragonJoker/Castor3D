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
#ifndef ___DX9_SHADER_OBJECT_H___
#define ___DX9_SHADER_OBJECT_H___

#include "Dx9RenderSystemPrerequisites.hpp"

#include <ShaderObject.hpp>

namespace Dx9Render
{
	class DxShaderObject
		:	public Castor3D::ShaderObjectBase
	{
		friend class DxShaderProgram;
		typedef std::map< Castor::String, D3DXHANDLE >	HandleStrMap;

	public:
		/**
		 * Constructor
		 */
		DxShaderObject( DxShaderProgram * p_pParent, Castor3D::eSHADER_TYPE p_eType );
		/**
		 * Destructor
		 */
		virtual ~DxShaderObject();
		/**
		 * Compiles program
		 */
		virtual bool Compile();
		/**
		 * Get compiler messages
		 */
		void RetrieveCompilerLog( Castor::String & p_strCompilerLog );
		/**
		 *\~english
		 *\brief		Activates the shader
		 *\~french
		 *\brief		Active le shader
		 */
		virtual void Bind() = 0;
		/**
		 *\~english
		 *\brief		Deactivates the shader
		 *\~french
		 *\brief		Désactive le shader
		 */
		virtual void Unbind() = 0;
		/**
		 *\~english
		 *\brief		Destroys the program on GPU
		 *\~french
		 *\brief		Détruit le programme sur le GPU
		 */
		virtual void DestroyProgram();
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
		D3DXHANDLE GetParameter( Castor::String const & p_strName );
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

		virtual	void SetEntryPoint( Castor::String const & p_strName )
		{
			m_strEntryPoint = p_strName;
		}
		virtual	Castor::String GetEntryPoint()const
		{
			return m_strEntryPoint;
		}

	protected:
		Castor::String m_strEntryPoint;
		Castor::String m_compilerLog;
		DxShaderProgram * m_pShaderProgram;
		IDirect3DVertexDeclaration9 * m_pInputVariables;
		Castor::String m_strProfile;
		HandleStrMap m_mapParamsByName;
	};
}

#endif