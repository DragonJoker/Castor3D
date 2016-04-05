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
#ifndef ___GUICOMMON_SHADER_EDITOR_PAGE_H___
#define ___GUICOMMON_SHADER_EDITOR_PAGE_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/frame.h>
#include <wx/aui/auibook.h>

namespace GuiCommon
{
	class ShaderEditorPage
		: public wxPanel
	{
	public:
		ShaderEditorPage( bool p_bCanEdit, StcContext & p_stcContext, Castor3D::ShaderProgramSPtr p_shader, Castor3D::eSHADER_TYPE p_type, wxWindow * p_parent, wxPoint const & p_position = wxDefaultPosition, const wxSize p_size = wxSize( 800, 600 ) );
		~ShaderEditorPage();

		bool LoadFile( wxString const & p_file );
		void SaveFile( bool p_createIfNone );

		wxString const & GetShaderFile()const
		{
			return m_shaderFile;
		}

		wxString const & GetShaderSource()const
		{
			return m_shaderSource;
		}

		Castor3D::eSHADER_MODEL GetShaderModel()const
		{
			return m_shaderModel;
		}

	private:
		void DoInitialiseShaderLanguage();
		void DoInitialiseLayout();
		void DoLoadPage();
		void DoCleanup();

		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & p_event );

	protected:
		wxAuiManager m_auiManager;
		StcContext & m_stcContext;
		StcTextEditor * m_editor;
		FrameVariablesList * m_frameVariablesList;
		PropertiesHolder * m_frameVariablesProperties;
		wxString m_shaderFile;
		wxString m_shaderSource;
		Castor3D::ShaderProgramWPtr m_shaderProgram;
		Castor3D::eSHADER_TYPE m_shaderType;
		Castor3D::eSHADER_MODEL m_shaderModel;
		bool m_canEdit;
	};
}

#endif
