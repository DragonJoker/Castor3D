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
#ifndef ___GL_QUERY_H___
#define ___GL_QUERY_H___

#include <Miscellaneous/GpuQuery.hpp>

#include "Common/GlObject.hpp"
#include "Common/OpenGl.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\brief		glQuery implementation
	*/
	class GlQuery
		: public Castor3D::GpuQuery
		, public Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >
	{
		using ObjectType = Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	p_invertFinal	Tells if the final render is to be inverted.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	p_invertFinal	Dit si on inverse l'image du rendu final.
		 */
		GlQuery( GlRenderSystem & renderSystem, Castor3D::QueryType p_query );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlQuery();

	private:
		/**
		 *\copydoc		Castor3D::GpuQuery::DoInitialise
		 */
		virtual bool DoInitialise();
		/**
		 *\copydoc		Castor3D::GpuQuery::DoCleanup
		 */
		virtual void DoCleanup();
		/**
		 *\copydoc		Castor3D::GpuQuery::Begin
		 */
		virtual void DoBegin()const;
		/**
		 *\copydoc		Castor3D::GpuQuery::End
		 */
		virtual void DoEnd()const;
		/**
		 *\copydoc		Castor3D::GpuQuery::DoGetInfos
		 */
		virtual void DoGetInfos( Castor3D::QueryInfo p_infos, int32_t & p_param )const;
		/**
		 *\copydoc		Castor3D::GpuQuery::DoGetInfos
		 */
		virtual void DoGetInfos( Castor3D::QueryInfo p_infos, uint32_t & p_param )const;
		/**
		 *\copydoc		Castor3D::GpuQuery::DoGetInfos
		 */
		virtual void DoGetInfos( Castor3D::QueryInfo p_infos, int64_t & p_param )const;
		/**
		 *\copydoc		Castor3D::GpuQuery::DoGetInfos
		 */
		virtual void DoGetInfos( Castor3D::QueryInfo p_infos, uint64_t & p_param )const;
	};
}

#endif
