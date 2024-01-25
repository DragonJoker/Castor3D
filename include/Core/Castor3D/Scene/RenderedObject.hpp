/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderedObject_H___
#define ___C3D_RenderedObject_H___

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	class RenderedObject
	{
	public:
		C3D_API void fillEntry( uint32_t nodeId
			, Pass const & pass
			, SceneNode const & sceneNode
			, uint32_t meshletCount
			, ModelBufferConfiguration & modelData );
		C3D_API void fillEntryOffsets( uint32_t nodeId
			, VkDeviceSize vertexOffset
			, VkDeviceSize indexOffset
			, VkDeviceSize meshletOffset );

		void setVisible( bool value )noexcept
		{
			m_visible = value;
		}

		void setShadowCaster( bool value )noexcept
		{
			m_castsShadows = value;
		}

		void setShadowReceiver( bool value )noexcept
		{
			m_receivesShadows = value;
		}

		void setCullable( bool value )noexcept
		{
			m_cullable = value;
		}

		bool isVisible()const noexcept
		{
			return m_visible;
		}

		bool isShadowCaster()const noexcept
		{
			return m_castsShadows;
		}

		bool isShadowReceiver()const noexcept
		{
			return m_receivesShadows;
		}

		bool isCullable()const noexcept
		{
			return m_cullable;
		}

	private:
		struct Offsets
		{
			Offsets() = default;

			VkDeviceSize vertexOffset{};
			VkDeviceSize indexOffset{};
			VkDeviceSize meshletOffset{};
		};

	private:
		bool m_visible{ true };
		bool m_castsShadows{ true };
		bool m_receivesShadows{ true };
		bool m_cullable{ true };
		uint32_t m_firstUpdate{ 5u };
		castor::UnorderedMap< uint32_t, castor::Pair< ModelBufferConfiguration *, Offsets > > m_modelsDataOffsets{};
	};
}

#endif
