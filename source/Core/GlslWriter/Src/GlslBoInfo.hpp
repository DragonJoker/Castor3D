/*
See LICENSE file in root folder
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
