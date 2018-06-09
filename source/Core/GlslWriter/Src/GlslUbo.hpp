/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_Ubo_H___
#define ___GLSL_Ubo_H___

#include "GlslBoInfo.hpp"
#include "GlslIndentBlock.hpp"

namespace glsl
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

		GlslWriter_API Ubo( GlslWriter & writer
			, castor::String const & name
			, uint32_t bind
			, uint32_t set = 0u
			, Layout layout = Layout::eStd140 );
		GlslWriter_API void end();

		template< typename T >
		inline T declMember( castor::String const & name );
		template< typename T >
		inline Array< T > declMember( castor::String const & name
			, uint32_t p_dimension );
		template< typename T >
		inline Array< T > declMemberArray( castor::String const & name );
		template< typename T >
		inline Optional< T > declMember( castor::String const & name
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > declMember( castor::String const & name
			, uint32_t p_dimension
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > declMemberArray( castor::String const & name
			, bool p_enabled );
		template< typename T >
		inline T getMember( castor::String const & name );
		template< typename T >
		inline Array< T > getMember( castor::String const & name
			, uint32_t p_dimension );
		template< typename T >
		inline Array< T > getMemberArray( castor::String const & name );
		template< typename T >
		inline Optional< T > getMember( castor::String const & name
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > getMember( castor::String const & name
			, uint32_t p_dimension
			, bool p_enabled );
		template< typename T >
		inline Optional< Array< T > > getMemberArray( castor::String const & name
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

#include "GlslUbo.inl"

#endif
