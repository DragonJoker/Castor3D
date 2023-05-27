/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderClusteredModule_H___
#define ___C3D_RenderClusteredModule_H___

#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Design/DesignModule.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Clustered */
	//@{

	/**
	*\~english
	*\brief
	*	Pass used to assign lights to the cluster cells they affect.
	*\~french
	*\brief
	*	Pass utilisée pour placer les sources lumineuses dans les cellules qu'elles affectent.
	*/
	class AssignLightsToClusters;
	/**
	*\~english
	*\brief
	*	The buffer containing the clusters.
	*\~french
	*\brief
	*	Le buffer contenant les clusters.
	*/
	class FrustumClusters;

	CU_DeclareSmartPtr( castor3d, FrustumClusters, C3D_API );

	using ClustersBuffersChangedFunction = std::function< void( FrustumClusters const & ) >;
	using OnClustersBuffersChanged = castor::SignalT< ClustersBuffersChangedFunction >;
	using OnClustersBuffersChangedConnection = castor::ConnectionT< ClustersBuffersChangedFunction >;

	//@}
	//@}
}

#endif
