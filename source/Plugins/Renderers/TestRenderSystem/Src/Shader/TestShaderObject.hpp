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
#ifndef ___TRS_SHADER_OBJECT_H___
#define ___TRS_SHADER_OBJECT_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Shader/ShaderObject.hpp>

namespace TestRender
{
	class TestShaderObject
		: public Castor3D::ShaderObject
	{
		friend class TestShaderProgram;

		using UIntStrMap = std::map< Castor::String, uint32_t >;

	public:
		/**
		 * Constructor
		 */
		TestShaderObject( TestShaderProgram * p_parent, Castor3D::eSHADER_TYPE p_type );
		/**
		 * Destructor
		 */
		virtual ~TestShaderObject();
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
		 *\copydoc		Castor3D::ShaderObject::HasParameter
		 */
		virtual bool HasParameter( Castor::String const & p_name );
		/**
		 *\copydoc		Castor3D::ShaderObject::SetParameter
		 */
		virtual void SetParameter( Castor::String const & p_name, Castor::Matrix4x4r const & p_value );
		/**
		 *\copydoc		Castor3D::ShaderObject::SetParameter
		 */
		virtual void SetParameter( Castor::String const & p_name, Castor::Matrix3x3r const & p_value );

	private:
		/**
		 *\copydoc		Castor3D::ShaderObject::DoRetrieveCompilerLog
		 */
		virtual Castor::String DoRetrieveCompilerLog();
	};
}

#endif
