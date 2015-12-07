/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GL_QUERY_H___
#define ___GL_QUERY_H___

#include "GlObject.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\brief		glQuery implementation
	*/
	class GlQuery
		: public Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >
	{
		using ObjectType = Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >;

	public:
		GlQuery( OpenGl & p_gl, eGL_QUERY p_query );
		virtual ~GlQuery();

		bool Begin();
		void End();

		template< typename T >
		inline bool GetInfos( eGL_QUERY_INFO p_infos, T & p_param )
		{
			return GetOpenGl().GetQueryObjectInfos( GetGlName(), p_infos, &p_param );
		}

	private:
		eGL_QUERY m_target;
	};
}

#endif
