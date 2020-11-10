/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Logger_H___
#define ___C3D_Logger_H___

#include "MiscellaneousModule.hpp"

#include <CastorUtils/Log/LoggerStream.hpp>

namespace castor3d
{
	class log
	{
		friend class Engine;

	private:
		static castor::LoggerInstance * initialise( castor::LoggerInstance & logger );
		static void cleanup();

	private:
		static castor::LoggerInstance * m_logger;

	public:
		C3D_API static castor::LoggerStreamT< char, castor::TraceLoggerStreambufT > trace;
		C3D_API static castor::LoggerStreamT< char, castor::DebugLoggerStreambufT > debug;
		C3D_API static castor::LoggerStreamT< char, castor::InfoLoggerStreambufT > info;
		C3D_API static castor::LoggerStreamT< char, castor::WarningLoggerStreambufT > warn;
		C3D_API static castor::LoggerStreamT< char, castor::ErrorLoggerStreambufT > error;
	};
}

#endif
