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

namespace c3d
{
	struct OverlayTextBuffer
	{
		using MyBufferIndex = OverlayTextBufferIndex;

		C3D_API OverlayTextBuffer( Engine & engine
			, String const & debugName
			, RenderDevice const & device );
		C3D_API ~OverlayTextBuffer()noexcept;

		C3D_API OverlayTextBufferIndex fill( uint32_t overlayIndex
			, TextOverlay const & overlay )noexcept;
		C3D_API void fillDescriptorSet( ashes::DescriptorSetLayout const & descriptorLayout
			, ashes::DescriptorSet & descriptorSet )const;
		C3D_API void upload( UploadData & uploader );

		Engine & engine;
		RenderDevice const & device;
		String name;

		template< typename DataT >
		struct DataBufferT
		{
			BufferUPtrT< DataT > buffer;
			ArrayView< DataT > data;
			uint32_t allocated{};
		};
		DataBufferT< TextChar > charsBuffer;
		DataBufferT< TextWord > wordsBuffer;
		DataBufferT< TextLine > linesBuffer;
	};
	using OverlayTextBufferPtr = RawUniquePtr< OverlayTextBuffer >;

	struct OverlayTextBufferPool
	{
		using MyBufferIndex = OverlayTextBufferIndex;

		C3D_API OverlayTextBufferPool( Engine & engine
			, String const & debugName
			, RenderDevice const & device );

		C3D_API OverlayTextBufferIndex fill( uint32_t overlayIndex
			, FontTexture const * fontTexture
			, TextOverlay const & overlay )noexcept;
		C3D_API void fillDescriptorSet( FontTexture const * fontTexture
			, ashes::DescriptorSetLayout const & descriptorLayout
			, ashes::DescriptorSet & descriptorSet );
		C3D_API void upload( UploadData & uploader );
		C3D_API OverlayTextBuffer const * get( FontTexture const & fontTexture );

	private:
		Engine & m_engine;
		RenderDevice const & m_device;
		String m_name;
		HashMap< FontTexture const *, OverlayTextBufferPtr > m_buffers;
	};
}

#endif
