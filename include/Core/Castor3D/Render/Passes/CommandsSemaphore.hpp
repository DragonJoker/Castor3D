/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CommandsSemaphore_H___
#define ___C3D_CommandsSemaphore_H___

#include "PassesModule.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	struct CommandsSemaphore
	{
		C3D_API CommandsSemaphore( RenderDevice const & device
			, castor::String const & name );
		C3D_API CommandsSemaphore( ashes::CommandBufferPtr commandBuffer = nullptr
			, ashes::SemaphorePtr semaphore = nullptr );

		C3D_API void submit( ashes::Queue const & queue )const;
		C3D_API ashes::Semaphore const & submit( ashes::Queue const & queue
			, ashes::Semaphore const & toWait )const;

		ashes::CommandBufferPtr commandBuffer;
		ashes::SemaphorePtr semaphore;
	};
}

#endif
