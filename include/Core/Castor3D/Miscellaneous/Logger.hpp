/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Logger_H___
#define ___C3D_Logger_H___

#include "MiscellaneousModule.hpp"

#include <CastorUtils/Log/LoggerStream.hpp>

namespace c3d
{
	class log
	{
		friend class Engine;

	private:
		static LoggerInstance * initialise( LoggerInstance & logger );
		static void cleanup();

	private:
		static LoggerInstance * m_logger;

	public:
		C3D_API static LoggerStreamT< xchar, TraceLoggerStreambufT > trace;
		C3D_API static LoggerStreamT< xchar, DebugLoggerStreambufT > debug;
		C3D_API static LoggerStreamT< xchar, InfoLoggerStreambufT > info;
		C3D_API static LoggerStreamT< xchar, WarningLoggerStreambufT > warn;
		C3D_API static LoggerStreamT< xchar, ErrorLoggerStreambufT > error;
	};
}

#endif
