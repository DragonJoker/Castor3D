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
#ifndef ___GLSL_Ubo_H___
#define ___GLSL_Ubo_H___

#include "GlslBoInfo.hpp"
#include "GlslIndentBlock.hpp"

namespace GLSL
{
	class Ubo
	{
	public:
		enum class Layout
		{
			eStd140,
			ePacked,
			eShared
		};

		using Info = BoInfo< Ubo::Layout >;

		GlslWriter_API Ubo( GlslWriter & p_writer
			, Castor::String const & p_name
			, uint32_t p_bind
			, Layout p_layout = Layout::eStd140 );
		GlslWriter_API void End();

		template< typename T >
		inline T DeclMember( Castor::String const & p_name );
		template< typename T >
		inline Array< T > DeclMember( Castor::String const & p_name
			, uint32_t p_dimension );
		template< typename T >
		inline Array< T > DeclMemberArray( Castor::String const & p_name );
		template< typename T >
		inline Optional< T > DeclMember( Castor::String const & p_name
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > DeclMember( Castor::String const & p_name
			, uint32_t p_dimension
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > DeclMemberArray( Castor::String const & p_name
			, bool p_enabled );
		template< typename T >
		inline T GetMember( Castor::String const & p_name );
		template< typename T >
		inline Array< T > GetMember( Castor::String const & p_name
			, uint32_t p_dimension );
		template< typename T >
		inline Array< T > GetMemberArray( Castor::String const & p_name );
		template< typename T >
		inline Optional< T > GetMember( Castor::String const & p_name
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > GetMember( Castor::String const & p_name
			, uint32_t p_dimension
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > GetMemberArray( Castor::String const & p_name
			, bool p_enabled );

	private:
		GlslWriter & m_writer;
		Castor::StringStream m_stream;
		std::unique_ptr< IndentBlock > m_block;
		Castor::String m_name;
		uint32_t m_count{ 0u };
		Info m_info;
	};
}

#include "GlslUbo.inl"

#endif
