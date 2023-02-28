/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlaysModule_H___
#define ___C3D_OverlaysModule_H___

#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"

#include <CastorUtils/Math/MathModule.hpp>

#include <ashespp/AshesPPPrerequisites.hpp>

namespace castor3d
{
	/**
	*\~english
	*\brief
	*	The descriptor binding indices for overlay rendering.
	*\~french
	*\brief
	*	Les indices de binding des descripteurs pour le rendu des overlays.
	*/
	enum class OverlayBindingId : uint32_t
	{
		eMaterials,
		eTexConfigs,
		eTexAnims,
		eCamera,
		eOverlays,
	};
	/**
	*\~english
	*\brief
	*	The overlays renderer.
	*\~french
	*\brief
	*	Le renderer d'overlays.
	*/
	class OverlayRenderer;
	/**
	*\~english
	*\brief
	*	Prepares the overlays for render.
	*\~french
	*\brief
	*	Prépare les overlays pour le rendu.
	*/
	class OverlayPreparer;
	/**
	*\~english
	*\brief
	*	Pool for the overlays vertices.
	*\~french
	*\brief
	*	Pool pour les sommets des overlays.
	*/
	template< typename VertexT, uint32_t CountT >
	struct OverlayVertexBufferPoolT;
	/**
	*\~english
	*\brief
	*	An overlay vertices index in the pool.
	*\~french
	*\brief
	*	L'index dans le pool des sommets d'un overlay.
	*/
	template< typename VertexT, uint32_t CountT >
	struct OverlayVertexBufferIndexT;

	template< typename VertexT, uint32_t CountT >
	using OverlayVertexBufferPoolPtrT = std::unique_ptr< OverlayVertexBufferPoolT< VertexT, CountT > >;

	struct OverlayPipeline
	{
		ashes::PipelineLayoutPtr pipelineLayout;
		ashes::GraphicsPipelinePtr pipeline;
	};

	struct OverlayRenderNode
	{
		OverlayPipeline & pipeline;
		Pass const & pass;
	};

	struct OverlayGeometryBuffers
	{
		GpuBufferBase * buffer;
		VkDeviceSize offset;
		VkDeviceSize range;
		uint32_t count;
	};

	CU_DeclareCUSmartPtr( castor3d, OverlayRenderer, C3D_API );
}

#endif
