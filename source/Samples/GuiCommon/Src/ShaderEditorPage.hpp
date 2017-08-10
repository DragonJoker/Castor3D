/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
		ShaderEditorPage( bool p_bCanEdit
			, StcContext & p_stcContext
			, castor3d::ShaderProgramSPtr p_shader
			, castor3d::ShaderType p_type
			, castor3d::Pass const & p_pass
			, castor3d::Scene const & p_scene
			, wxWindow * p_parent
			, wxPoint const & p_position = wxDefaultPosition
			, const wxSize p_size = wxSize( 800, 600 ) );
		~ShaderEditorPage();

		bool LoadFile( wxString const & p_file );
		void SaveFile( bool p_createIfNone );

		wxString const & getShaderFile()const
		{
			return m_shaderFile;
		}

		wxString const & getShaderSource()const
		{
			return m_shaderSource;
		}

		castor3d::ShaderModel getShaderModel()const
		{
			return m_shaderModel;
		}

	private:
		void doInitialiseShaderLanguage();
		void doInitialiseLayout();
		void doLoadPage( castor3d::RenderPipeline & p_pipeline );
		void doCleanup();

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
		castor3d::ShaderProgramWPtr m_shaderProgram;
		castor3d::ShaderType m_shaderType;
		castor3d::ShaderModel m_shaderModel;
		bool m_canEdit;
	};
}

#endif
