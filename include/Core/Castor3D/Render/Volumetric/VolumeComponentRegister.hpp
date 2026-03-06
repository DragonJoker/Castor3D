/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VolumeComponentRegister_H___
#define ___C3D_VolumeComponentRegister_H___

#include "Castor3D/Render/Volumetric/VolumetricModule.hpp"

#include <map>

namespace c3d
{
	namespace shader
	{
		class VolumeShaders;
	}

	class VolumeComponentRegister
		: public OwnedBy< Engine >
	{
	public:
		/**
		 *\name
		 *	Construction / Destruction.
		 */
		/**@{*/
		VolumeComponentRegister( VolumeComponentRegister const & ) = delete;
		VolumeComponentRegister( VolumeComponentRegister && )noexcept = delete;
		VolumeComponentRegister & operator=( VolumeComponentRegister const & ) = delete;
		VolumeComponentRegister & operator=( VolumeComponentRegister && )noexcept = delete;
		C3D_API explicit VolumeComponentRegister( Engine & engine );
		C3D_API ~VolumeComponentRegister()noexcept;
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Components registration.
		 *\~french
		 *\name
		 *	Enregistrement des composants.
		 */
		/**@{*/
		C3D_API void registerComponent( String const & componentType
			, VolumeComponentPluginUPtr componentPlugin );
		C3D_API void unregisterComponent( String const & componentType );
		C3D_API uint32_t getNameId( String const & componentType )const;
		C3D_API VolumeComponentPlugin const & getPlugin( uint32_t componentId )const;
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Shader struct initialisation.
		 *\~french
		 *\name
		 *	Initialisation de structure shader.
		 */
		/**@{*/
		C3D_API Vector< shader::VolumeComponentShaderPtr > createShaders( sdw::ShaderWriter & writer
			, shader::VolumeShaders const & volumeShaders
			, c3d::Extent2D const & targetExtent
			, bool hasDepth
			, uint32_t enabledPlugins
			, uint32_t & bindingId )const;
		/**@}*/

		auto begin()const noexcept
		{
			return m_registered.begin();
		}

		auto end()const noexcept
		{
			return m_registered.end();
		}
		/**@}*/

	private:
		struct Component
		{
			Component()noexcept = default;
			explicit Component( uint32_t id )noexcept
				: id{ id }
			{
			}

			uint32_t id{};
			String name{};
			VolumeComponentPluginUPtr plugin{};
		};
		using Components = Vector< Component >;

	private:
		Component & getNextId();
		void registerComponent( Component & componentDesc
			, String const & componentType
			, VolumeComponentPluginUPtr componentPlugin );
		void unregisterComponent( uint32_t id );

	private:
		Components m_registered;
	};
}

#endif
