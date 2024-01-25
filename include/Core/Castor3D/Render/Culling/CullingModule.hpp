/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderCullingModule_H___
#define ___C3D_RenderCullingModule_H___

#include "Castor3D/Limits.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
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

	CU_DeclareSmartPtr( castor3d, SceneCuller, C3D_API );

	using SceneCullerSignalFunction = castor::Function< void( SceneCuller const & ) >;
	using SceneCullerSignal = castor::SignalT< SceneCullerSignalFunction >;
	using SceneCullerSignalConnection = castor::ConnectionT< SceneCullerSignal >;

	using SceneCullerSubmeshSignalFunction = castor::Function< void( SceneCuller const &, CulledNodeT< SubmeshRenderNode > const &, bool ) >;
	using SceneCullerSubmeshSignal = castor::SignalT< SceneCullerSubmeshSignalFunction >;
	using SceneCullerSubmeshSignalConnection = castor::ConnectionT< SceneCullerSubmeshSignal >;

	using SceneCullerBillboardSignalFunction = castor::Function< void( SceneCuller const &, CulledNodeT< BillboardRenderNode > const &, bool ) >;
	using SceneCullerBillboardSignal = castor::SignalT< SceneCullerBillboardSignalFunction >;
	using SceneCullerBillboardSignalConnection = castor::ConnectionT< SceneCullerBillboardSignal >;

	using PipelineNodes = castor::Array< uint32_t, MaxNodesPerPipeline >;

	//@}
	//@}
}

#endif
