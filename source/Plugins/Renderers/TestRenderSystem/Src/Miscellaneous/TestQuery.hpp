/* See LICENSE file in root folder */
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
