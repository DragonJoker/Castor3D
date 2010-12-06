/*****************************************************************************

	Author : Marc BILLON
	Compagny : FDSSoftMedia - Copyright 2007

*****************************************************************************/

#ifndef ___EMUSE_CONTEXT_H___
#define ___EMUSE_CONTEXT_H___

#include "Module_Script.h"

namespace CastorShape
{
	struct ScriptContext
	{
		ScriptEngine			* scriptEngine;
		ScriptTimerManager		* timerManager;
		String					  lastSceneFile;
		String					  instanceNum;
		String					  lastDataRep;

		real					  frameTime;

		bool					  m_continue;
		bool					  m_breakOne;
		bool					  m_return;

		ScriptNode				* m_currentFunction;

		String 					groupName;
		String 					filename;
		String 					fullLine;
		String 					baseDirectory;
		String 					imageDir;
		String 					general_string_a;
		String 					general_string_b;
		String 					general_string_c;

		unsigned int			lineNo;
		int						general_int;

		real 					currentReal;

		ScriptContext()
			:	m_continue			(false),
				m_breakOne			(false),
				m_return			(false),
				m_currentFunction	(NULL)
		{
		}
	};
}

#endif
