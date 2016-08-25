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
		TestShaderObject( TestShaderProgram * p_parent, Castor3D::ShaderType p_type );
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
