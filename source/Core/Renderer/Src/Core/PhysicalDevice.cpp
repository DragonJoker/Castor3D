/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "PhysicalDevice.hpp"

#include "Renderer.hpp"
#include "Device.hpp"

namespace renderer
{
	PhysicalDevice::PhysicalDevice( Renderer & renderer )
		: m_renderer{ renderer }
	{
	}

	bool PhysicalDevice::deduceMemoryType( uint32_t typeBits
		, MemoryPropertyFlags requirements
		, uint32_t & typeIndex )const
	{
		bool result{ false };

		// Recherche parmi les types de mémoire de la première ayant les propriétés voulues.
		uint32_t i{ 0 };

		while ( i < m_memoryProperties.memoryTypes.size() && !result )
		{
			if ( ( typeBits & 1 ) == 1 )
			{
				// Le type de mémoire est disponible, a-t-il les propriétés demandées?
				if ( ( m_memoryProperties.memoryTypes[i].propertyFlags & requirements ) == requirements )
				{
					typeIndex = i;
					result = true;
				}
			}

			typeBits >>= 1;
			++i;
		}

		return result;
	}
}
