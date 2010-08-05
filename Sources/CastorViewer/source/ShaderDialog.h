#ifndef ___ShaderDialog___
#define ___ShaderDialog___

#include "UniformVariableDialog.h"

namespace CastorViewer
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
		wxListBox * m_pListUniformVariables;

		wxButton * m_pButtonOk;
		wxButton * m_pButtonCancel;

		wxString m_strVertexShader;
		wxString m_strFragmentShader;
		wxString m_strGeometryShader;

		Castor3D::ShaderProgram * m_shaderProgram;
		Castor3D::Pass * m_pPass;
		bool m_bCompiled;
		bool m_bOwnShader;

		std::map <int, Castor3D::UniformVariable *> m_mapUniformVariables;

	public:
		ShaderDialog( wxWindow * p_pParent, Castor3D::Pass * p_pPass, const wxPoint & p_ptPosition=wxPoint( 0, 0), const wxSize p_ptSize=wxSize( 800, 600));
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