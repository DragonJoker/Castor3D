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
	*	Pool for the overlays texts using a specific FontTexture.
	*\~french
	*\brief
	*	Pool pour les textes des overlays utilisant une FontTexture spécifique.
	*/
	struct OverlayTextBuffer;
	/**
	*\~english
	*\brief
	*	Pool for the overlays texts.
	*\~french
	*\brief
	*	Pool pour les textes des overlays.
	*/
	struct OverlayTextBufferPool;
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
		GpuBufferBase * buffer{};
		VkDeviceSize offset{};
		VkDeviceSize range{};
		uint32_t count{};
	};

	struct OverlayTextBufferIndex
	{
		OverlayTextBuffer * pool{};
		uint32_t word{};
		uint32_t line{};
		float top{};
	};

	CU_DeclareCUSmartPtr( castor3d, OverlayRenderer, C3D_API );
	CU_DeclareSmartPtr( OverlayTextBufferPool );
}

#endif
