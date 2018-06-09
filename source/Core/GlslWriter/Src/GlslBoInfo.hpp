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
		BoInfo( LayoutType layout
			, uint32_t bind
			, uint32_t set )
			: m_layout{ layout }
			, m_bind{ bind }
			, m_set{ set }
		{
		}

		inline void registerMember( castor::String const & name, TypeName type )
		{
			m_members.emplace( name, type );
		}
		
		inline TypeName getMemberType( castor::String const & name )const
		{
			return m_members.at( name );
		}

		inline LayoutType getLayout()const
		{
			return m_layout;
		}

		inline uint32_t getBindingPoint()const
		{
			return m_bind;
		}

		inline uint32_t getSetPoint()const
		{
			return m_set;
		}

	private:
		LayoutType m_layout;
		uint32_t m_bind;
		uint32_t m_set;
		std::map< castor::String, TypeName > m_members;
	};
}

#endif
