/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GUICOMMON_FRAME_VARIABLE_DIALOG_H___
#define ___GUICOMMON_FRAME_VARIABLE_DIALOG_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/dialog.h>

namespace GuiCommon
{
	class FrameVariableDialog
		: public wxDialog
	{
	private:
		typedef enum eID
		{
			eID_BUTTON_OK,
			eID_BUTTON_CANCEL,
			eID_COMBO_TYPE,
		}	eID;

	protected:
		Castor3D::FrameVariableWPtr m_pFrameVariable;
		Castor3D::ShaderProgramBaseWPtr m_pProgram;
		bool m_bOwn;
		wxComboBox * m_pComboType;
		wxTextCtrl * m_pEditName;
		wxTextCtrl * m_pEditValue;
		Castor3D::eSHADER_TYPE m_eTargetStage;

	public:
		FrameVariableDialog( wxWindow * p_pParent, Castor3D::ShaderProgramBaseSPtr p_pProgram, Castor3D::eSHADER_TYPE p_eTargetStage, Castor3D::FrameVariableSPtr p_pFrameVariable = nullptr );
		~FrameVariableDialog();

		inline Castor3D::FrameVariableSPtr GetFrameVariable()
		{
			return m_pFrameVariable.lock();
		}

	private:
		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent &	p_event );
		void OnSelectType( wxCommandEvent &	p_event );
		void OnOk( wxCommandEvent &	p_event );
		void OnCancel( wxCommandEvent &	p_event );
	};
}

#endif
