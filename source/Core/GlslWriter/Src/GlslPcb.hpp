/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_Pcb_H___
#define ___GLSL_Pcb_H___

#include "GlslBoInfo.hpp"
#include "GlslIndentBlock.hpp"

namespace glsl
{
	class Pcb
	{
	public:
		enum class Layout
		{
			eStd430,
		};

		GlslWriter_API Pcb( GlslWriter & writer
			, castor::String const & name
			, castor::String const & instance );
		GlslWriter_API void end();

		template< typename T >
		inline T declMember( castor::String const & name
			, uint32_t location );
		template< typename T >
		inline Array< T > declMember( castor::String const & name
			, uint32_t dimension
			, uint32_t location );
		template< typename T >
		inline Array< T > declMemberArray( castor::String const & name
			, uint32_t location );
		template< typename T >
		inline Optional< T > declMember( castor::String const & name
			, uint32_t location
			, bool enabled );
		template< typename T >
		inline Optional< Array< T > > declMember( castor::String const & name
			, uint32_t dimension
			, uint32_t location
			, bool enabled );
		template< typename T >
		inline Optional< Array< T > > declMemberArray( castor::String const & name
			, uint32_t location
			, bool enabled );
		template< typename T >
		inline T getMember( castor::String const & name
			, uint32_t location );
		template< typename T >
		inline Array< T > getMember( castor::String const & name
			, uint32_t location
			, uint32_t dimension );
		template< typename T >
		inline Array< T > getMemberArray( castor::String const & name
			, uint32_t location );
		template< typename T >
		inline Optional< T > getMember( castor::String const & name
			, uint32_t location
			, bool enabled );
		template< typename T >
		inline Optional< Array< T > > getMember( castor::String const & name
			, uint32_t dimension
			, uint32_t location
			, bool enabled );
		template< typename T >
		inline Optional< Array< T > > getMemberArray( castor::String const & name
			, uint32_t location
			, bool enabled );

	private:
		GlslWriter & m_writer;
		castor::StringStream m_stream;
		castor::String m_name;
		castor::String m_instance;
		uint32_t m_count{ 0u };
	};
}

#include "GlslPcb.inl"

#endif
