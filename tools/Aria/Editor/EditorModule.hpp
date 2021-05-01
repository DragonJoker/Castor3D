/*
See LICENSE file in root folder
*/
#ifndef ___CTP_EditorModule_HPP___
#define ___CTP_EditorModule_HPP___

#include <map>
#include <memory>

namespace aria
{
	class LanguageFileContext;
	class LanguageFileParser;
	class LanguageInfo;
	class SceneFileDialog;
	class SceneFileEditor;
	class StcContext;
	class StcTextEditor;
	class StyleInfo;

	using LanguageInfoPtr = std::shared_ptr< LanguageInfo >;
	using LanguageFileContextPtr = std::shared_ptr< LanguageFileContext >;
	using StyleInfoMap = std::map< int, StyleInfo >;
}

#endif
