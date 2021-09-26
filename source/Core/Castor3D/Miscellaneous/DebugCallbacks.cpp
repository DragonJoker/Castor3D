#include "Castor3D/Miscellaneous/DebugCallbacks.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Core/Instance.hpp>
#include <ashespp/Core/RendererList.hpp>

namespace castor3d
{
	namespace
	{
		std::string formatMessage( std::string_view prefix
			, std::string_view message )
		{
			if ( message.find_first_of( "|\n" ) == message.find( "\n" ) )
			{
				auto split = castor::string::split( message.data(), "\n", ~( 0u ), false );
				std::stringstream stream;

				for ( auto & str : split )
				{
					stream << "\n" << prefix << str;
				}

				return stream.str();
			}

			auto split = castor::string::split( message.data(), "|", ~( 0u ), false );
			std::stringstream stream;

			if ( !split.empty() )
			{
				auto first = split[0];
				auto end = first.find( "]" );
				stream << "\n" << prefix << first.substr( 0, end );

				if ( split.size() > 1u )
				{
					for ( auto & str : castor::makeArrayView( split.begin() + 1u, split.end() ) )
					{
						stream << "\n" << prefix << castor::string::trim( str );
					}
				}
			}

			return stream.str();
		}

#if VK_EXT_debug_utils

		std::ostream & operator<<( std::ostream & stream, VkDebugUtilsObjectNameInfoEXT const & value )
		{
			stream << "(" << std::hex << value.objectHandle << ") " << ashes::getName( value.objectType );

			if ( value.pObjectName )
			{
				stream << " " << value.pObjectName;
			}

			return stream;
		}

		std::ostream & operator<<( std::ostream & stream, VkDebugUtilsLabelEXT const & value )
		{
			stream << "(" << value.color[0]
				<< ", " << value.color[1]
				<< ", " << value.color[2]
				<< ", " << value.color[3] << ")";

			if ( value.pLabelName )
			{
				stream << " " << value.pLabelName;
			}

			return stream;
		}

		template< typename ObjectT >
		void print( std::ostream & stream
			, std::string const & name
			, uint32_t count
			, ObjectT const * objects
			, std::string const & lineEnd
			, std::string const & lineBegin )
		{
			stream << lineBegin << name << ": " << count << lineEnd;

			for ( uint32_t i = 0u; i < count; ++i, ++objects )
			{
				stream << lineBegin << lineBegin << *objects << lineEnd;
			}
		}

		VkBool32 VKAPI_PTR debugMessageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
			, VkDebugUtilsMessageTypeFlagsEXT messageTypes
			, const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData
			, void * pUserData )
		{
			VkBool32 result = VK_FALSE;

			if ( !pCallbackData->messageIdNumber )
			{
				return result;
			}

			// Select prefix depending on flags passed to the callback
			// Note that multiple flags may be set for a single validation message
			std::locale loc{ "C" };
			std::stringstream stream;
			stream.imbue( loc );
			stream << "Vulkan ";
			std::string lineEnd;
			std::string lineBegin = ", ";

			// Error that may result in undefined behaviour
			switch ( messageSeverity )
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				stream << "Error";
				lineEnd = "\n";
				lineBegin = "    ";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				stream << "Warning";
				lineEnd = "\n";
				lineBegin = "    ";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				stream << "Info";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				stream << "Verbose";
				break;
			default:
				break;
			}

			if ( ashes::checkFlag( messageTypes, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ) )
			{
				stream << " - General";
			}
			if ( ashes::checkFlag( messageTypes, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT ) )
			{
				stream << " - Validation";
			}
			if ( ashes::checkFlag( messageTypes, VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT ) )
			{
				stream << " - Performance";
			}

			stream << lineEnd;

			if ( pCallbackData->pMessageIdName )
			{
				stream << lineBegin << "Message ID: " << pCallbackData->pMessageIdName << lineEnd;
			}

			stream << lineBegin << "Code: 0x" << std::hex << pCallbackData->messageIdNumber << lineEnd;
			stream << lineBegin << "Message: " << formatMessage( lineBegin + "  ", pCallbackData->pMessage ) << lineEnd;
			print( stream, "Objects", pCallbackData->objectCount, pCallbackData->pObjects, lineEnd, lineBegin );
			print( stream, "Queue Labels", pCallbackData->queueLabelCount, pCallbackData->pQueueLabels, lineEnd, lineBegin );
			print( stream, "CommmandBuffer Labels", pCallbackData->cmdBufLabelCount, pCallbackData->pCmdBufLabels, lineEnd, lineBegin );

			switch ( messageSeverity )
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				log::error << stream.str() << std::endl;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				log::warn << stream.str() << std::endl;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				log::trace << stream.str() <<std::endl;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				log::debug << stream.str() << std::endl;
				break;
			default:
				break;
			}

			// The return value of this callback controls wether the Vulkan call that caused
			// the validation message will be aborted or not
			// Return VK_FALSE if we DON'T want Vulkan calls that cause a validation message 
			// (and return a VkResult) to abort
			// Return VK_TRUE if you instead want to have calls abort, and the function will 
			// return VK_ERROR_VALIDATION_FAILED_EXT 
			return result;
		}

		VkDebugUtilsMessengerEXT createDebugUtilsMessenger( ashes::Instance const & instance
			, void * userData )
		{
			VkDebugUtilsMessageSeverityFlagsEXT severityFlags = 0u
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			VkDebugUtilsMessageTypeFlagsEXT typeFlags = 0u
				| VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			VkDebugUtilsMessengerCreateInfoEXT createInfo
			{
				VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
				nullptr,
				0u,
				severityFlags,
				typeFlags,
				debugMessageCallback,
				userData
			};
			return instance.createDebugUtilsMessenger( createInfo );
		}

#endif
#if VK_EXT_debug_report

		VkBool32 VKAPI_PTR debugReportCallback( VkDebugReportFlagsEXT flags
			, VkDebugReportObjectTypeEXT objectType
			, uint64_t object
			, size_t location
			, int32_t messageCode
			, const char * pLayerPrefix
			, const char * pMessage
			, void * pUserData )
		{
			// Select prefix depending on flags passed to the callback
			// Note that multiple flags may be set for a single validation message
			std::locale loc{ "C" };
			std::stringstream stream;
			stream.imbue( loc );
			stream << "Vulkan ";
			std::string lineEnd;
			std::string lineBegin = ", ";

			// Error that may result in undefined behaviour
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_ERROR_BIT_EXT ) )
			{
				lineEnd = "\n";
				lineBegin = "    ";
				stream << "Error:" << lineEnd << lineBegin;
			};
			// Warnings may hint at unexpected / non-spec API usage
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_WARNING_BIT_EXT ) )
			{
				lineEnd = "\n";
				lineBegin = "    ";
				stream << "Warning:" << lineEnd << lineBegin;
			};
			// May indicate sub-optimal usage of the API
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) )
			{
				lineEnd = "\n";
				lineBegin = "    ";
				stream << "Performance:" << lineEnd << lineBegin;
			};
			// Informal messages that may become handy during debugging
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) )
			{
				stream << "Info:" << lineEnd;
			}
			// Diagnostic info from the Vulkan loader and layers
			// Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_DEBUG_BIT_EXT ) )
			{
				stream << "Debug:" << lineEnd;
			}

			// Display message to default output (console/logcat)
			stream << "Layer: " << pLayerPrefix << lineEnd;
			stream << lineBegin << "Code: 0x" << std::hex << messageCode << lineEnd;
			stream << lineBegin << "Object: (" << std::hex << object << ") " << ashes::getName( objectType ) << lineEnd;
			stream << lineBegin << "Message: " << formatMessage( lineBegin + "  ", pMessage );

			VkBool32 result = VK_FALSE;

			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_ERROR_BIT_EXT ) )
			{
				log::error << stream.str() << std::endl;
			}
			else if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_WARNING_BIT_EXT )
				|| ashes::checkFlag( flags, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) )
			{
				log::warn << stream.str() << std::endl;
			}
			else if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) )
			{
				log::trace << stream.str() << std::endl;
			}
			else
			{
				log::debug << stream.str() << std::endl;
			}

			// The return value of this callback controls wether the Vulkan call that caused
			// the validation message will be aborted or not
			// Return VK_FALSE if we DON'T want Vulkan calls that cause a validation message 
			// (and return a VkResult) to abort
			// Return VK_TRUE if you instead want to have calls abort, and the function will 
			// return VK_ERROR_VALIDATION_FAILED_EXT 
			return result;
		}

		VkDebugReportCallbackEXT createDebugReportCallback( ashes::Instance const & instance
			, void * userData )
		{
			VkDebugReportFlagsEXT debugReportFlags = 0u
				| VK_DEBUG_REPORT_DEBUG_BIT_EXT
				//| VK_DEBUG_REPORT_INFORMATION_BIT_EXT
				| VK_DEBUG_REPORT_WARNING_BIT_EXT
				| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
				| VK_DEBUG_REPORT_ERROR_BIT_EXT;
			VkDebugReportCallbackCreateInfoEXT createInfo
			{
				VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
				nullptr,
				debugReportFlags,
				debugReportCallback,
				userData
			};
			return instance.createDebugReportCallback( createInfo );
		}

#endif
	}

	//*************************************************************************

	DebugCallbacks::DebugCallbacks( ashes::Instance const & instance
		, void * userData )
		: m_instance{ instance }
	{
#if VK_EXT_debug_utils

		m_messenger = createDebugUtilsMessenger( instance, userData );

		if ( m_messenger == VK_NULL_HANDLE )
#endif
		{

#if VK_EXT_debug_report

			m_callback = createDebugReportCallback( instance, userData );

#endif
		}
	}

	DebugCallbacks::~DebugCallbacks()
	{
#if VK_EXT_debug_report

		if ( m_callback != VK_NULL_HANDLE )
		{
			m_instance.vkDestroyDebugReportCallbackEXT( m_instance
				, m_callback
				, m_instance.getAllocationCallbacks() );
		}

#endif
#if VK_EXT_debug_utils

		if ( m_messenger != VK_NULL_HANDLE )
		{
			m_instance.vkDestroyDebugUtilsMessengerEXT( m_instance
				, m_messenger
				, m_instance.getAllocationCallbacks() );
		}

#endif
	}

	//*************************************************************************
}
