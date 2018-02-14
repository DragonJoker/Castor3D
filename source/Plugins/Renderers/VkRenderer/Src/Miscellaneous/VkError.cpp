/*
This file belongs to VkLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

#include <map>
#include <atomic>
#include <thread>

namespace vk_renderer
{
	namespace
	{
		/**\brief
		*	Classe permettant d'avoir une variable accessible depuis plusieurs threads.
		*	Cette variable aura une valeur spécifique pour chaque thread qui l'utilise.
		*\remarks
		*	Le paramètre template doit être default initialisable et copiable.
		*/
		template< typename Type >
		class ThreadUniqueValue
		{
		public:
			/** Constructeur
			*\param[in] p_value
			*	La valeur par défaut de la variable
			*/
			explicit ThreadUniqueValue( Type const & value = Type{} )
				: m_initialValue{ value }
			{
			}

			/**
			*\return
			*	La valeur de la variable pour le thread appelant
			*/
			operator Type & ()
			{
				doWaitUnlock();
				return doGetValue();
			}

			/**
			*\return
			*	La valeur de la variable pour le thread appelant
			*/
			operator Type const & ()const
			{
				doWaitUnlock();
				return doGetValue();
			}

		private:
			/** Attend que la variable ne soit plus lockée.
			*/
			void doWaitUnlock()const
			{
				while ( m_locked )
				{
					std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
				}
			}

			/** Locke la variable, récupère sa valeur pour le thread appelant, puis la délocke.
			*\return
			*	La valeur de la variable pour le thread appelant.
			*/
			Type & doGetValue()const
			{
				m_locked = true;
				Type & result = m_values.emplace( std::this_thread::get_id(), m_initialValue ).first->second;
				m_locked = false;
				return result;
			}

		private:
			mutable std::atomic_bool m_locked{ false };
			mutable std::map< std::thread::id, Type > m_values;
			Type const m_initialValue;
		};

		/**
		*\return
		*	La dernière erreur survenue pour le thread appelant.
		*/
		std::string & doGetError()
		{
			static ThreadUniqueValue< std::string > error;
			return error;
		}
	}

	bool checkError( VkResult errCode )
	{
		bool result = errCode == VK_SUCCESS;

		if ( !result )
		{
			switch ( errCode )
			{
			case VK_NOT_READY:
				doGetError() = "A fence or query has not yet completed.";
				result = true;
				break;

			case VK_TIMEOUT:
				doGetError() = "A wait operation has not completed in the specified time.";
				result = true;
				break;

			case VK_EVENT_SET:
				doGetError() = " An event is signaled.";
				result = true;
				break;

			case VK_EVENT_RESET:
				doGetError() = "An event is unsignaled.";
				result = true;
				break;

			case VK_INCOMPLETE:
				doGetError() = " A return array was too small for the result.";
				result = true;
				break;

			case VK_ERROR_OUT_OF_HOST_MEMORY:
				doGetError() = "A host memory allocation has failed.";
				assert( false );
				break;

			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				doGetError() = "A device memory allocation has failed.";
				assert( false );
				break;

			case VK_ERROR_INITIALIZATION_FAILED:
				doGetError() = "Initialization of an object could not be completed for implementation-specific reasons.";
				assert( false );
				break;

			case VK_ERROR_DEVICE_LOST:
				doGetError() = "The logical or physical device has been lost.";
				//assert( false );
				break;

			case VK_ERROR_MEMORY_MAP_FAILED:
				doGetError() = "Mapping of a memory object has failed.";
				assert( false );
				break;

			case VK_ERROR_LAYER_NOT_PRESENT:
				doGetError() = "A requested layer is not present or could not be loaded.";
				assert( false );
				break;

			case VK_ERROR_EXTENSION_NOT_PRESENT:
				doGetError() = "A requested extension is not supported.";
				assert( false );
				break;

			case VK_ERROR_FEATURE_NOT_PRESENT:
				doGetError() = "A requested feature is not supported.";
				assert( false );
				break;

			case VK_ERROR_INCOMPATIBLE_DRIVER:
				doGetError() = "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons.";
				assert( false );
				break;

			case VK_ERROR_TOO_MANY_OBJECTS:
				doGetError() = "Too many objects of the type have already been created.";
				assert( false );
				break;

			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				doGetError() = " A requested format is not supported on this device.";
				assert( false );
				break;

			case VK_ERROR_SURFACE_LOST_KHR:
				doGetError() = "A surface is no longer available.";
				assert( false );
				break;

			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				doGetError() = "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API.";
				assert( false );
				break;

			case VK_SUBOPTIMAL_KHR:
				doGetError() = "An image became available, and the swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.";
				assert( false );
				break;

			case VK_ERROR_OUT_OF_DATE_KHR:
				doGetError() = "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail.";
				assert( false );
				break;

			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
				doGetError() = "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image.";
				assert( false );
				break;

			case VK_ERROR_VALIDATION_FAILED_EXT:
				doGetError() = "Validation failed.";
				assert( false );
				break;

			case VK_ERROR_INVALID_SHADER_NV:
				doGetError() = "One or more shaders failed to compile or link.";
				assert( false );
				break;

			default:
				doGetError() = "Unknown error";
				assert( false );
				break;
			}

			std::cerr << doGetError() << "\n";
		}

		return result;
	}

	std::string const & getLastError()
	{
		return doGetError();
	}
}
