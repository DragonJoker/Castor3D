/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlaysModule_H___
#define ___C3D_OverlaysModule_H___

#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Overlay/FontTexture.hpp"

#include <CastorUtils/Math/MathModule.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>

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
		eOverlaysIDs,
		eOverlaysSurfaces,
	};
	/**
	*\~english
	*\brief
	*	Overlays render pass.
	*\~french
	*\brief
	*	Passe de rendu des overlays.
	*/
	class OverlayPass;
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

	template< typename VertexT, uint32_t CountT >
	using OverlayVertexBufferPoolPtrT = std::unique_ptr< OverlayVertexBufferPoolT< VertexT, CountT > >;

	struct OverlayDrawPipeline
	{
		ashes::PipelineLayoutPtr pipelineLayout;
		ashes::GraphicsPipelinePtr pipeline;
	};

	struct OverlayPipelineData
	{
		struct DescriptorSets
		{
			ashes::DescriptorSetPtr draw{};
			ashes::DescriptorSetCRefArray all{};
		};
		using DescriptorSetsPtr = std::unique_ptr< DescriptorSets >;

		OverlayPipelineData( OverlayPipelineData const & ) = delete;
		OverlayPipelineData & operator=( OverlayPipelineData const & ) = delete;
		OverlayPipelineData( OverlayPipelineData && ) = default;
		OverlayPipelineData & operator=( OverlayPipelineData && ) = default;
		OverlayPipelineData() = default;
		~OverlayPipelineData() = default;

		OverlayPipelineData( ashes::BufferPtr< uint32_t > iDsBuffer,
			castor::ArrayView< uint32_t > ids,
			DescriptorSetsPtr descs,
			uint32_t c )
			: overlaysIDsBuffer{ std::move( iDsBuffer ) }
			, overlaysIDs{ std::move( ids ) }
			, descriptorSets{ std::move( descs ) }
			, count{ c }
		{
		}

		ashes::BufferPtr< uint32_t > overlaysIDsBuffer{};
		castor::ArrayView< uint32_t > overlaysIDs{};
		DescriptorSetsPtr descriptorSets{};
		uint32_t count{};
	};

	struct OverlayDrawNode
	{
		OverlayDrawPipeline & pipeline;
		Pass const & pass;
	};

	struct OverlayTextBufferIndex
	{
		OverlayTextBuffer * pool{};
		uint32_t word{};
		uint32_t line{};
		float top{};
	};

	struct OverlayDrawData
	{
		Overlay const * overlay{};
		OverlayDrawNode const * node{};
		OverlayPipelineData const * pipelineData{};
		uint32_t vertexOffset{};
		uint32_t vertexCount{};
		uint32_t overlayIndex{};
		uint32_t pipelineIndex{};
		OverlayTextBufferIndex textBuffer{};
		bool secondary{};
	};

	CU_DeclareCUSmartPtr( castor3d, OverlayRenderer, C3D_API );
	CU_DeclareSmartPtr( OverlayTextBufferPool );
}

#endif
