#ifndef ___CA_ShaderDialog___
#define ___CA_ShaderDialog___

#include "FrameVariableDialog.h"

namespace CastorArchitect
{
	class TextPanel;

	class ShaderDialog : public wxDialog
	{
	protected:
		enum eIDs
		{
			eHasGeometryShader,
			eVertex,
			eFragment,
			eGeometry,
			eLoadShader,
			eEditors,
			eGrid,
			eOK,
			eCancel
		};

	protected:
		wxButton * m_vertexFolder;
		wxButton * m_fragmentFolder;
		wxButton * m_geometryFolder;
		wxButton * m_loadShader;
		wxNotebook * m_pEditors;
		TextPanel * m_pVertexEditor;
		TextPanel * m_pFragmentEditor;
		TextPanel * m_pGeometryEditor;
		wxListBox * m_pListFrameVariables;

		wxButton * m_pButtonOk;
		wxButton * m_pButtonCancel;

		wxString m_strVertexShader;
		wxString m_strFragmentShader;
		wxString m_strGeometryShader;

		Castor3D::ShaderProgramPtr m_shaderProgram;
		Castor3D::PassPtr m_pPass;
		bool m_bCompiled;
		bool m_bOwnShader;

		C3DMap( int, Castor3D::FrameVariablePtr) m_mapFrameVariables;

	public:
		ShaderDialog( wxWindow * p_pParent, Castor3D::PassPtr p_pPass, const wxPoint & p_ptPosition=wxPoint( 0, 0), const wxSize p_ptSize=wxSize( 800, 600));
		~ShaderDialog();

	private:
		void _cleanup();
		void _loadShader();
		void _openShaderFile( );

		DECLARE_EVENT_TABLE()
		void _onVertexFolder	( wxCommandEvent & event);
		void _onFragmentFolder	( wxCommandEvent & event);
		void _onGeometryFolder	( wxCommandEvent & event);
		void _onLoadShader		( wxCommandEvent & event);
		void _onClose			( wxCloseEvent & event);
		void _onOk				( wxCommandEvent & event);
		void _onCancel			( wxCommandEvent & event);
		void _onGridCellChange	( wxCommandEvent & event);
	};
}

#endif