/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderClusteredModule_H___
#define ___C3D_RenderClusteredModule_H___

#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Design/DesignModule.hpp>

namespace c3d
{
	/**@name Render */
	//@{
	/**@name Clustered */
	//@{

	enum class ClusterSplitScheme
	{
		eExponentialBase = 0,
		eLinear = 1,
		eExponentialLinearHybrid = 2,
		CU_ScopedEnumBounds( eExponentialBase, eExponentialLinearHybrid )
	};
	C3D_API String getName( ClusterSplitScheme value );
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
	*	Clusters configuration.
	*\~french
	*\brief
	*	Configuration des clusters.
	*/
	struct ClustersConfig;
	/**
	*\~english
	*\brief
	*	The buffer containing the clusters.
	*\~french
	*\brief
	*	Le buffer contenant les clusters.
	*/
	class FrustumClusters;

	using ClustersBuffersChangedFunction = Function< void( FrustumClusters const & ) >;
	using OnClustersBuffersChanged = SignalT< ClustersBuffersChangedFunction >;
	using OnClustersBuffersChangedConnection = ConnectionT< ClustersBuffersChangedFunction >;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, ClustersConfig, C3D_API );
	CU_DeclareSmartPtr( c3d, FrustumClusters, C3D_API );
	/** @endcond */

	C3D_API u32 getLightsMortonCodeChunkCount( u32 lightCount );

	//@}
	//@}
}

#endif
