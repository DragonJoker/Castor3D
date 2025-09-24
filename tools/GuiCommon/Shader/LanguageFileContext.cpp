#include "GuiCommon/Shader/LanguageFileContext.hpp"

#include "GuiCommon/Shader/StcContext.hpp"
#include "GuiCommon/Shader/StyleInfo.hpp"
#include "GuiCommon/Shader/LanguageInfo.hpp"

namespace GuiCommon
{
	LanguageFileContext::LanguageFileContext()
	{
		mapFoldFlags[cuT( "comment" )] = int( eSTC_FOLD::eCOMMENT );
		mapFoldFlags[cuT( "compact" )] = int( eSTC_FOLD::eCOMPACT );
		mapFoldFlags[cuT( "preprocessor" )] = int( eSTC_FOLD::ePREPROC );
	}
}
