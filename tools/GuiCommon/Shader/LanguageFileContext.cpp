#include "GuiCommon/Shader/LanguageFileContext.hpp"

#include "GuiCommon/Shader/StcContext.hpp"
#include "GuiCommon/Shader/StyleInfo.hpp"
#include "GuiCommon/Shader/LanguageInfo.hpp"

namespace GuiCommon
{
	LanguageFileContext::LanguageFileContext()
	{

		mapFoldFlags[cuT( "comment" )] = eSTC_FOLD_COMMENT;
		mapFoldFlags[cuT( "compact" )] = eSTC_FOLD_COMPACT;
		mapFoldFlags[cuT( "preprocessor" )] = eSTC_FOLD_PREPROC;
	}
}
