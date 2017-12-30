/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_QUERY_H___
#define ___C3DGLES3_QUERY_H___

#include <Miscellaneous/GpuQuery.hpp>

#include "Common/GlES3Object.hpp"
#include "Common/OpenGlES3.hpp"

namespace GlES3Render
{
	/*!
	\author		Sylvain DOREMUS
	\brief		glQuery implementation
	*/
	class GlES3Query
		: public Castor3D::GpuQuery
		, public Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >
	{
		using ObjectType = Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >;

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
		GlES3Query( GlES3RenderSystem & p_renderSystem, Castor3D::QueryType p_query );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlES3Query();

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
