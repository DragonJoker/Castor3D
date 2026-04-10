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

	class Camera;
	class Scene;
	struct Texture;

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
		C3D_API VolumeComponentPlugin & getPlugin( String const & componentType )const;
		C3D_API VolumeComponentPlugin & getPlugin( uint32_t componentId )const;
		C3D_API void registerCamera( Camera const & camera
			, Texture const * depthObj )const;
		C3D_API bool hasAnyEnabled()const noexcept;
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
			, Extent2D const & targetExtent
			, bool hasDepth
			, uint32_t enabledPlugins
			, uint32_t & bindingId )const;
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Pass bindings.
		 *\~french
		 *\name
		 *	Attaches de passe.
		 */
		/**@{*/
		C3D_API void registerPasses( crg::ResourcesCache & resources
			, crg::FramePassGroup & graph
			, c3d::Camera const & camera )const;
		C3D_API void registerBindings( crg::FramePass & pass
			, uint32_t enabledPlugins
			, Camera const & camera
			, uint32_t & bindingId )const;
		/**@}*/

		VolumeComponentPlugin * operator[]( uint32_t pluginId )
		{
			if ( pluginId > 0 && pluginId <= m_registered.size() )
				return m_registered[pluginId - 1u].plugin.get();
			return nullptr;
		}

		auto begin()const noexcept
		{
			return m_registered.begin();
		}

		auto end()const noexcept
		{
			return m_registered.end();
		}

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
