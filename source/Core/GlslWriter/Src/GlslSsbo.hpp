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
#ifndef ___GLSL_Ssbo_H___
#define ___GLSL_Ssbo_H___

#include "GlslBoInfo.hpp"
#include "GlslIndentBlock.hpp"

namespace GLSL
{
	class Ssbo
	{
	public:
		enum class Layout
		{
			eStd140,
			eStd430,
			ePacked,
			eShared
		};

		using Info = BoInfo< Ssbo::Layout >;

		GlslWriter_API Ssbo( GlslWriter & p_writer
			, castor::String const & p_name
			, uint32_t p_bind
			, Layout p_layout = Layout::eStd430 );
		GlslWriter_API void end();

		template< typename T >
		inline T declMember( castor::String const & p_name );
		template< typename T >
		inline Array< T > declMember( castor::String const & p_name
			, uint32_t p_dimension );
		template< typename T >
		inline Array< T > declMemberArray( castor::String const & p_name );
		template< typename T >
		inline Optional< T > declMember( castor::String const & p_name
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > declMember( castor::String const & p_name
			, uint32_t p_dimension
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > declMemberArray( castor::String const & p_name
			, bool p_enabled );
		template< typename T >
		inline T getMember( castor::String const & p_name );
		template< typename T >
		inline Array< T > getMember( castor::String const & p_name
			, uint32_t p_dimension );
		template< typename T >
		inline Array< T > getMemberArray( castor::String const & p_name );
		template< typename T >
		inline Optional< T > getMember( castor::String const & p_name
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > getMember( castor::String const & p_name
			, uint32_t p_dimension
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > getMemberArray( castor::String const & p_name
			, bool p_enabled );

	private:
		GlslWriter & m_writer;
		castor::StringStream m_stream;
		std::unique_ptr< IndentBlock > m_block;
		castor::String m_name;
		uint32_t m_count{ 0u };
		Info m_info;
	};
}

#include "GlslSsbo.inl"

#endif
