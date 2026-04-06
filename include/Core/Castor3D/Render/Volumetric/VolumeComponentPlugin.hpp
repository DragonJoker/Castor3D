/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VolumeComponentPlugin_H___
#define ___C3D_VolumeComponentPlugin_H___

#include "Castor3D/Render/Volumetric/VolumetricModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace c3d
{
	class Camera;

	namespace shader
	{
		class VolumeComponentShader
		{
		private:
			VolumeComponentShader( VolumeComponentShader const & ) = delete;
			VolumeComponentShader & operator=( VolumeComponentShader const & ) = delete;

		public:
			C3D_INL_API VolumeComponentShader( VolumeComponentShader && ) = default;
			C3D_INL_API VolumeComponentShader & operator=( VolumeComponentShader && ) = default;
			C3D_INL_API virtual ~VolumeComponentShader() = default;

			VolumeComponentShader( sdw::ShaderWriter & writer
				, VolumeShaders const & volumeShaders )
				: m_writer{ writer }
				, m_volumeShaders{ volumeShaders }
			{
			}

			virtual void fillType( sdw::type::BaseStruct & type )const = 0;
			virtual void fillInit( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const = 0;
			virtual void registerVolumes( Volumes & volumes ) = 0;
			virtual void initialise( sdw::Vec2 const & pixelCoord, Ray & ray
				, Volumes & volumes, VolumesTraversalResult & result ) = 0;
			virtual void finalise( Ray const & ray, VolumesTraversalResult & result ) = 0;

		protected:
			sdw::ShaderWriter & m_writer;
			VolumeShaders const & m_volumeShaders;
		};
	}

	class VolumeComponentPlugin
	{
	private:
		VolumeComponentPlugin( VolumeComponentPlugin const & ) = delete;
		VolumeComponentPlugin & operator=( VolumeComponentPlugin const & ) = delete;
		VolumeComponentPlugin & operator=( VolumeComponentPlugin && rhs )noexcept = delete;

	public:
		/**
		*\name
		*	Construction / Desctruction.
		*/
		/**@{*/
		C3D_API virtual ~VolumeComponentPlugin();
		C3D_API VolumeComponentPlugin( VolumeComponentPlugin && rhs )noexcept;
		/**
		 *\~english
		 *\param[in]	volumeComponents		The components registrar.
		 *\~french
		 *\param[in]	volumeComponents		Le registre de composants.
		 */
		explicit VolumeComponentPlugin( VolumeComponentRegister const & volumeComponents )
			: m_volumeComponents{ volumeComponents }
		{
		}
		/**
		*\~english
		*\brief
		*	Creates the component's shaders.
		*\~french
		*\brief
		*	Crée les shaders du composant.
		*/
		C3D_API virtual shader::VolumeComponentShaderPtr createComponentsShader( sdw::ShaderWriter & writer
			, shader::VolumeShaders const & volumeShaders
			, Extent2D const & targetExtent
			, bool hasDepth
			, uint32_t & bindingId )const = 0;
		/**
		 *\~english
		 *\name
		 *	Pass bindings.
		 *\~french
		 *\name
		 *	Attaches de passe.
		 */
		/**@{*/
		C3D_API virtual void registerScenePasses( crg::ResourcesCache & resources
			, crg::FramePassGroup & graph
			, c3d::Scene const & scene )const = 0;
		C3D_API virtual void registerCameraPasses( crg::ResourcesCache & resources
			, crg::FramePassGroup & graph
			, c3d::Camera const & camera )const = 0;
		C3D_API virtual void registerBindings( crg::FramePass & pass
			, Camera const & camera
			, uint32_t & bindingId )const = 0;
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Plugin specific data.
		 *\~french
		 *\name
		 *	Données spécifiques de plugin.
		 */
		/**@{*/
		C3D_API virtual void registerCamera( Camera const & camera
			, c3d::Texture const * depthObj ) = 0;
		/**@}*/
		/**
		*\name
		*	Getters.
		*/
		/**@{*/
		uint32_t getId()const noexcept
		{
			return m_id;
		}

		bool isEnabled()const noexcept
		{
			return m_enabled;
		}

		VolumeComponentRegister const & getRegister()const noexcept
		{
			return m_volumeComponents;
		}
		/**@}*/

	protected:
		void setEnabled( bool v )
		{
			m_enabled = v;
		}

	private:
		friend class VolumeComponentRegister;

		void setId( uint32_t id )
		{
			m_id = id;
		}

	private:
		uint32_t m_id{};
		bool m_enabled{};
		VolumeComponentRegister const & m_volumeComponents;
	};
}

#endif
