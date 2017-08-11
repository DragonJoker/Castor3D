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
#ifndef ___GLSL_BoInfo_H___
#define ___GLSL_BoInfo_H___

#include "GlslWriterPrerequisites.hpp"

namespace glsl
{
	template< typename LayoutType >
	struct BoInfo
	{
		BoInfo( LayoutType p_layout, uint32_t p_bind )
			: m_layout{ p_layout }
			, m_bind{ p_bind }
		{
		}

		inline void registerMember( castor::String const & p_name, TypeName p_type )
		{
			m_members.emplace( p_name, p_type );
		}
		
		inline TypeName getMemberType( castor::String const & p_name )const
		{
			return m_members.at( p_name );
		}

		inline LayoutType getLayout()const
		{
			return m_layout;
		}

		inline uint32_t getBindingPoint()const
		{
			return m_bind;
		}

	private:
		LayoutType m_layout;
		uint32_t m_bind;
		std::map< castor::String, TypeName > m_members;
	};
}

#endif
