/* See LICENSE file in root folder */
#ifndef ___PrecompiledHeader___
#define ___PrecompiledHeader___

#ifdef _WIN32
#   define __WXMSW__
#   undef __WXGTK__
#endif

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4311 )
#	pragma warning( disable:4312 )
#endif
#include <wx/app.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/checkbox.h>
#include <wx/checklst.h>
#include <wx/choicdlg.h>
#include <wx/colordlg.h>
#include <wx/combobox.h>
#include <wx/dcclient.h>
#include <wx/dialog.h>
#include <wx/file.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/frame.h>
#include <wx/grid.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/listbox.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/radiobox.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/stdpaths.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/toolbar.h>
#include <wx/treectrl.h>
#if defined( _MSC_VER )
#	pragma warning( pop )
#endif

#endif
