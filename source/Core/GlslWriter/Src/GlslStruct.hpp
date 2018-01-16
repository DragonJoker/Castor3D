/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_Struct_H___
#define ___GLSL_Struct_H___

#include "GlslWriterPrerequisites.hpp"

namespace glsl
{
	class Struct
	{
	public:
		GlslWriter_API Struct( GlslWriter & writer
			, castor::String const & p_name );
		GlslWriter_API Struct( GlslWriter & writer
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
