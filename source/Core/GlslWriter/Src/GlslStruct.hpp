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
#ifndef ___GLSL_Struct_H___
#define ___GLSL_Struct_H___

#include "GlslWriterPrerequisites.hpp"

namespace glsl
{
	class Struct
	{
	public:
		GlslWriter_API Struct( GlslWriter & p_writer
			, castor::String const & p_name );
		GlslWriter_API Struct( GlslWriter & p_writer
			, castor::String const & p_name
			, castor::String const & p_instName );
		GlslWriter_API void end();

		template< typename T >
		inline void declMember( castor::String const & p_name );
		template< typename T >
		inline void declMember( castor::String const & p_name, uint32_t p_dimension );
		template< typename T >
		inline void declMemberArray( castor::String const & p_name );
		template< typename T >
		inline T getMember( castor::String const & p_name );
		template< typename T >
		inline Array< T > getMember( castor::String const & p_name, uint32_t p_dimension );
		template< typename T >
		inline Array< T > getMemberArray( castor::String const & p_name );

	private:
		IndentBlock * m_block;
		GlslWriter & m_writer;
		castor::String m_name;
		castor::String m_instName;
	};
}

#include "GlslStruct.inl"

#endif
