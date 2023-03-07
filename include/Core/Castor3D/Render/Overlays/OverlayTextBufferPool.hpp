/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayTextBufferPool_H___
#define ___C3D_OverlayTextBufferPool_H___

#include "Castor3D/Render/Overlays/OverlaysModule.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

namespace castor3d
{
	struct OverlayTextBuffer
	{
		using MyBufferIndex = OverlayTextBufferIndex;

		C3D_API OverlayTextBuffer( Engine & engine
			, std::string const & debugName
			, RenderDevice const & device );

		C3D_API OverlayTextBufferIndex fill( uint32_t overlayIndex
			, castor::Size const & renderSize
			, TextOverlay const & overlay
			, OverlayRenderNode const & node
			, bool secondary );
		C3D_API void fillDescriptorSet( ashes::DescriptorSetLayout const & descriptorLayout
			, ashes::DescriptorSet & descriptorSet );
		C3D_API void upload( ashes::CommandBuffer const & cb );

		Engine & engine;
		RenderDevice const & device;
		std::string name;

		template< typename DataT >
		struct DataBufferT
		{
			ashes::BufferPtr< DataT > buffer;
			castor::ArrayView< DataT > data;
			uint32_t allocated{};
		};
		DataBufferT< TextChar > charsBuffer;
		DataBufferT< TextWord > wordsBuffer;
		DataBufferT< TextLine > linesBuffer;
	};
	using OverlayTextBufferPtr = std::unique_ptr< OverlayTextBuffer >;

	struct OverlayTextBufferPool
	{
		using MyBufferIndex = OverlayTextBufferIndex;

		C3D_API OverlayTextBufferPool( Engine & engine
			, std::string const & debugName
			, RenderDevice const & device );

		C3D_API OverlayTextBufferIndex fill( uint32_t overlayIndex
			, FontTexture const * fontTexture
			, castor::Size const & renderSize
			, TextOverlay const & overlay
			, OverlayRenderNode const & node
			, bool secondary );
		C3D_API void fillDescriptorSet( FontTexture const * fontTexture
			, ashes::DescriptorSetLayout const & descriptorLayout
			, ashes::DescriptorSet & descriptorSet );
		C3D_API void upload( ashes::CommandBuffer const & cb );
		C3D_API OverlayTextBuffer const * get( FontTexture const & fontTexture );

	private:
		Engine & m_engine;
		RenderDevice const & m_device;
		std::string m_name;
		std::map< FontTexture const *, OverlayTextBufferPtr > m_buffers;
	};
}

#endif
