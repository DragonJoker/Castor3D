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
#ifndef ___TRS_QUERY_H___
#define ___TRS_QUERY_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Miscellaneous/GpuQuery.hpp>

namespace TestRender
{
	/*!
	\author		Sylvain DOREMUS
	\brief		glQuery implementation
	*/
	class TestQuery
		: public castor3d::GpuQuery
	{
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
		TestQuery( TestRenderSystem & p_renderSystem, castor3d::QueryType p_query );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~TestQuery();

	private:
		/**
		 *\copydoc		castor3d::GpuQuery::doInitialise
		 */
		bool doInitialise()override;
		/**
		 *\copydoc		castor3d::GpuQuery::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::GpuQuery::doBegin
		 */
		void doBegin()const override;
		/**
		 *\copydoc		castor3d::GpuQuery::doEnd
		 */
		void doEnd()const override;
		/**
		 *\copydoc		castor3d::GpuQuery::doGetInfos
		 */
		void doGetInfos( castor3d::QueryInfo p_infos, int32_t & p_param )const override;
		/**
		 *\copydoc		castor3d::GpuQuery::doGetInfos
		 */
		void doGetInfos( castor3d::QueryInfo p_infos, uint32_t & p_param )const override;
		/**
		 *\copydoc		castor3d::GpuQuery::doGetInfos
		 */
		void doGetInfos( castor3d::QueryInfo p_infos, int64_t & p_param )const override;
		/**
		 *\copydoc		castor3d::GpuQuery::doGetInfos
		 */
		void doGetInfos( castor3d::QueryInfo p_infos, uint64_t & p_param )const override;
	};
}

#endif
