/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderCullingModule_H___
#define ___C3D_RenderCullingModule_H___

#include "Castor3D/Limits.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace c3d
{
	/**@name Render */
	//@{
	/**@name Culling */
	//@{

	/**
	*\~english
	*\brief
	*	No culling.
	*\~french
	*\brief
	*	Aucun culling.
	*/
	class DummyCuller;
	/**
	*\~english
	*\brief
	*	Culls nodes against a frustum.
	*\~french
	*\brief
	*	Elimine les noeuds par rapport à un frustum.
	*/
	class FrustumCuller;
	/**
	*\~english
	*\brief
	*	Base class to cull nodes, before adding them to the render queue.
	*\~french
	*\brief
	*	Classe de base pour éliminer les noeuds, avant de les ajouter à la file de rendu.
	*/
	class SceneCuller;

	using SceneCullerSignalFunction = Function< void( SceneCuller const & ) >;
	using SceneCullerSignal = SignalT< SceneCullerSignalFunction >;
	using SceneCullerSignalConnection = ConnectionT< SceneCullerSignal >;

	using SceneCullerSubmeshSignalFunction = Function< void( SceneCuller const &, CulledNodeT< SubmeshRenderNode > const &, bool ) >;
	using SceneCullerSubmeshSignal = SignalT< SceneCullerSubmeshSignalFunction >;
	using SceneCullerSubmeshSignalConnection = ConnectionT< SceneCullerSubmeshSignal >;

	using SceneCullerBillboardSignalFunction = Function< void( SceneCuller const &, CulledNodeT< BillboardRenderNode > const &, bool ) >;
	using SceneCullerBillboardSignal = SignalT< SceneCullerBillboardSignalFunction >;
	using SceneCullerBillboardSignalConnection = ConnectionT< SceneCullerBillboardSignal >;

	using PipelineNodes = Array< uint32_t, MaxNodesPerPipeline >;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, SceneCuller, C3D_API );
	/** @endcond */

	//@}
	//@}
}

#endif
