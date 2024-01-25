#include "Castor3D/Miscellaneous/DebugCallbacks.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Core/Instance.hpp>
#include <ashespp/Core/RendererList.hpp>

namespace castor3d
{
	namespace dbg
	{
		static castor::String formatMessage( castor::StringView prefix
			, castor::MbStringView message )
		{
			auto mbPrefix = castor::toUtf8( prefix );

			if ( message.find_first_of( "|\n" ) == message.find( "\n" ) )
			{
				auto split = castor::string::split( castor::MbStringView{ message.data() }, "\n", ~0u, false );
				castor::MbStringStream stream;

				for ( auto const & str : split )
				{
					stream << "\n" << mbPrefix << str;
				}

				return castor::makeString( stream.str() );
			}

			auto split = castor::string::split( castor::MbStringView{ message.data() }, "|", ~0u, false );
			castor::MbStringStream stream;

			if ( !split.empty() )
			{
				auto first = split[0];
				auto end = first.find( "]" );
				stream << "\n" << mbPrefix << first.substr( 0, end );

				if ( split.size() > 1u )
				{
					for ( auto & str : castor::makeArrayView( split.begin() + 1u, split.end() ) )
					{
						stream << "\n" << mbPrefix << castor::string::trim( str );
					}
				}
			}

			return castor::makeString( stream.str() );
		}

#if VK_EXT_debug_utils

		static castor::OutputStream & operator<<( castor::OutputStream & stream, VkDebugUtilsObjectNameInfoEXT const & value )
		{
			stream << cuT( "(" ) << std::hex << value.objectHandle << cuT( ") " ) << castor::makeString( ashes::getName( value.objectType ) );

			if ( value.pObjectName )
			{
				stream << cuT( " " ) << value.pObjectName;
			}

			return stream;
		}

		static castor::OutputStream & operator<<( castor::OutputStream & stream, VkDebugUtilsLabelEXT const & value )
		{
			stream << cuT( "(" ) << value.color[0]
				<< cuT( ", " ) << value.color[1]
				<< cuT( ", " ) << value.color[2]
				<< cuT( ", " ) << value.color[3] << cuT( ")" );

			if ( value.pLabelName )
			{
				stream << " " << value.pLabelName;
			}

			return stream;
		}

		template< typename ObjectT >
		static void print( castor::OutputStream & stream
			, castor::String const & name
			, uint32_t count
			, ObjectT const * objects
			, castor::String const & lineEnd
			, castor::String const & lineBegin )
		{
			stream << lineBegin << name << cuT( ": " ) << count << lineEnd;

			for ( uint32_t i = 0u; i < count; ++i, ++objects )
			{
				stream << lineBegin << lineBegin << *objects << lineEnd;
			}
		}

		static VkBool32 VKAPI_PTR debugMessageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
			, VkDebugUtilsMessageTypeFlagsEXT messageTypes
			, const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData
			, void * /*pUserData*/ )
		{
			VkBool32 result = VK_FALSE;

			if ( !pCallbackData->messageIdNumber )
			{
				return result;
			}

			// Select prefix depending on flags passed to the callback
			// Note that multiple flags may be set for a single validation message
			auto stream = castor::makeStringStream();
			stream << cuT( "Vulkan " );
			castor::String lineEnd;
			castor::String lineBegin = cuT( ", " );

			// Error that may result in undefined behaviour
			switch ( messageSeverity )
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				stream << cuT( "Error" );
				lineEnd = cuT( "\n" );
				lineBegin = cuT( "    " );
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				stream << cuT( "Warning" );
				lineEnd = cuT( "\n" );
				lineBegin = cuT( "    " );
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				stream << cuT( "Info" );
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				stream << cuT( "Verbose" );
				break;
			default:
				break;
			}

			if ( ashes::checkFlag( messageTypes, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ) )
			{
				stream << cuT( " - General" );
			}
			if ( ashes::checkFlag( messageTypes, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT ) )
			{
				stream << cuT( " - Validation" );
			}
			if ( ashes::checkFlag( messageTypes, VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT ) )
			{
				stream << cuT( " - Performance" );
			}

			stream << lineEnd;

			if ( pCallbackData->pMessageIdName )
			{
				stream << lineBegin << cuT( "Message ID: " ) << pCallbackData->pMessageIdName << lineEnd;
			}

			stream << lineBegin << cuT( "Code: 0x" ) << std::hex << pCallbackData->messageIdNumber << lineEnd;
			stream << lineBegin << cuT( "Message: " ) << formatMessage( lineBegin + cuT( "  " ), pCallbackData->pMessage ) << lineEnd;
			print( stream, cuT( "Objects" ), pCallbackData->objectCount, pCallbackData->pObjects, lineEnd, lineBegin );
			print( stream, cuT( "Queue Labels" ), pCallbackData->queueLabelCount, pCallbackData->pQueueLabels, lineEnd, lineBegin );
			print( stream, cuT( "CommmandBuffer Labels" ), pCallbackData->cmdBufLabelCount, pCallbackData->pCmdBufLabels, lineEnd, lineBegin );

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

		static VkDebugUtilsMessengerEXT createDebugUtilsMessenger( ashes::Instance const & instance
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
			auto createInfo = makeVkStruct< VkDebugUtilsMessengerCreateInfoEXT >( 0u
				, severityFlags
				, typeFlags
				, debugMessageCallback
				, userData );
			return instance.createDebugUtilsMessenger( createInfo );
		}

#endif
#if VK_EXT_debug_report

		static VkBool32 VKAPI_PTR debugReportCallback( VkDebugReportFlagsEXT flags
			, VkDebugReportObjectTypeEXT objectType
			, uint64_t object
			, size_t /*location*/
			, int32_t messageCode
			, char const * pLayerPrefix
			, char const * pMessage
			, void * /*pUserData*/ )
		{
			// Select prefix depending on flags passed to the callback
			// Note that multiple flags may be set for a single validation message
			auto stream = castor::makeStringStream();
			stream << cuT( "Vulkan " );
			castor::String lineEnd;
			castor::String lineBegin = cuT( ", " );

			// Error that may result in undefined behaviour
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_ERROR_BIT_EXT ) )
			{
				lineEnd = cuT( "\n" );
				lineBegin = cuT( "    " );
				stream << cuT( "Error:" ) << lineEnd << lineBegin;
			}
			// Warnings may hint at unexpected / non-spec API usage
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_WARNING_BIT_EXT ) )
			{
				lineEnd = cuT( "\n" );
				lineBegin = cuT( "    " );
				stream << cuT( "Warning:" ) << lineEnd << lineBegin;
			}
			// May indicate sub-optimal usage of the API
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) )
			{
				lineEnd = cuT( "\n" );
				lineBegin = cuT( "    " );
				stream << cuT( "Performance:" ) << lineEnd << lineBegin;
			}
			// Informal messages that may become handy during debugging
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) )
			{
				stream << cuT( "Info:" ) << lineEnd;
			}
			// Diagnostic info from the Vulkan loader and layers
			// Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_DEBUG_BIT_EXT ) )
			{
				stream << cuT( "Debug:" ) << lineEnd;
			}

			// Display message to default output (console/logcat)
			stream << cuT( "Layer: " ) << pLayerPrefix << lineEnd;
			stream << lineBegin << cuT( "Code: 0x" ) << std::hex << messageCode << lineEnd;
			stream << lineBegin << cuT( "Object: (" ) << std::hex << object << cuT( ") " ) << castor::makeString( ashes::getName( objectType ) ) << lineEnd;
			stream << lineBegin << cuT( "Message: " ) << formatMessage( lineBegin + cuT( "  " ), pMessage );

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

		static VkDebugReportCallbackEXT createDebugReportCallback( ashes::Instance const & instance
			, void * userData )
		{
			VkDebugReportFlagsEXT debugReportFlags = 0u
				| VK_DEBUG_REPORT_DEBUG_BIT_EXT
				//| VK_DEBUG_REPORT_INFORMATION_BIT_EXT
				| VK_DEBUG_REPORT_WARNING_BIT_EXT
				| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
				| VK_DEBUG_REPORT_ERROR_BIT_EXT;
			auto createInfo = makeVkStruct< VkDebugReportCallbackCreateInfoEXT >( debugReportFlags
				, debugReportCallback
				, userData );
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

		m_messenger = dbg::createDebugUtilsMessenger( instance, userData );

		if ( m_messenger == VK_NULL_HANDLE )
#endif
		{

#if VK_EXT_debug_report

			m_callback = dbg::createDebugReportCallback( instance, userData );

#endif
		}
	}

	DebugCallbacks::~DebugCallbacks()noexcept
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
