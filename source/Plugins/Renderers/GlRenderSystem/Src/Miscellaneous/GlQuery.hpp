/*
See LICENSE file in root folder
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
		: public castor3d::GpuQuery
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
		GlQuery( GlRenderSystem & renderSystem, castor3d::QueryType p_query );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlQuery();

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
		 *\copydoc		castor3d::GpuQuery::Begin
		 */
		void doBegin()const override;
		/**
		 *\copydoc		castor3d::GpuQuery::End
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
