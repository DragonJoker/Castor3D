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
		CommandsSemaphore( ashes::CommandBufferPtr && commandBuffer
			, ashes::SemaphorePtr && semaphore )
			: commandBuffer{ std::move( commandBuffer ) }
			, semaphore{ std::move( semaphore ) }
		{
		}

		ashes::CommandBufferPtr commandBuffer;
		ashes::SemaphorePtr semaphore;
	};
}

#endif
