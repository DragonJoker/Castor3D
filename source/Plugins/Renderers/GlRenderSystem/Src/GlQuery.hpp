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

#include <GpuQuery.hpp>

#include "GlObject.hpp"
#include "OpenGl.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\brief		glQuery implementation
	*/
	class GlQuery
		: public Castor3D::GpuQuery
		, public Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >
	{
		using ObjectType = Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_invertFinal	Tells if the final render is to be inverted.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	p_invertFinal	Dit si on inverse l'image du rendu final.
		 */
		GlQuery( GlRenderSystem & p_renderSystem, Castor3D::eQUERY_TYPE p_query );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlQuery();

	private:
		/**
		 *\copydoc		Castor3D::GpuQuery::Create
		 */
		virtual bool DoCreate();
		/**
		 *\copydoc		Castor3D::GpuQuery::Destroy
		 */
		virtual void DoDestroy();
		/**
		 *\copydoc		Castor3D::GpuQuery::Begin
		 */
		virtual bool DoBegin()const;
		/**
		 *\copydoc		Castor3D::GpuQuery::End
		 */
		virtual void DoEnd()const;
		/**
		 *\copydoc		Castor3D::GpuQuery::DoGetInfos
		 */
		virtual bool DoGetInfos( Castor3D::eQUERY_INFO p_infos, int32_t & p_param )const;
		/**
		 *\copydoc		Castor3D::GpuQuery::DoGetInfos
		 */
		virtual bool DoGetInfos( Castor3D::eQUERY_INFO p_infos, uint32_t & p_param )const;
		/**
		 *\copydoc		Castor3D::GpuQuery::DoGetInfos
		 */
		virtual bool DoGetInfos( Castor3D::eQUERY_INFO p_infos, int64_t & p_param )const;
		/**
		 *\copydoc		Castor3D::GpuQuery::DoGetInfos
		 */
		virtual bool DoGetInfos( Castor3D::eQUERY_INFO p_infos, uint64_t & p_param )const;
	};
}

#endif
