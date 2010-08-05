/*****************************************************************************

	Author : Marc BILLON
	Compagny : FDSSoftMedia - Copyright 2007

*****************************************************************************/

#ifndef ___EMUSE_CONTEXT_H___
#define ___EMUSE_CONTEXT_H___

#include "Module_Script.h"

namespace CastorShape
{
	struct Context
	{
		ScriptEngine			* scriptEngine;
		ScriptTimerManager		* timerManager;
		String					  lastSceneFile;
		String					  instanceNum;
		String					  lastDataRep;

		float					  frameTime;

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

		String					autoPhysicsPreName;
		String					autoPhysicsPostName;

		unsigned int			lineNo;
		int						general_int;

		float 					currentReal;

		bool 					usingShadow;
		bool 					forceShadows;
		bool					autoPhysics;

		Context()
			:	m_continue			(false),
				m_breakOne			(false),
				m_return			(false),
				m_currentFunction	(NULL)
		{
		}
	};
}

#endif
