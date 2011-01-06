#ifndef ___GC_ShaderDialog___
#define ___GC_ShaderDialog___

#include "FrameVariableDialog.h"

namespace GUICommon
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
			eGridVertex,
			eGridFragment,
			eGridGeometry,
			eOK,
			eCancel
		};

	protected:
		wxButton * m_pFolders[Castor3D::eNbShaderTypes];
		wxButton * m_loadShader;
		wxNotebook * m_pEditorsBook;
		TextPanel * m_pEditors[Castor3D::eNbShaderTypes];
		wxListBox * m_pListsFrameVariables[Castor3D::eNbShaderTypes];

		wxButton * m_pButtonOk;
		wxButton * m_pButtonCancel;

		wxString m_strShaders[Castor3D::eNbShaderTypes];

		Castor3D::ShaderProgramPtr m_shaderProgram;
		Castor3D::PassPtr m_pPass;
		bool m_bCompiled;
		bool m_bOwnShader;
		Castor3D::ShaderManager * m_pManager;

		KeyedContainer< int, Castor3D::FrameVariablePtr>::Map m_mapFrameVariables[Castor3D::eNbShaderTypes];

	public:
		ShaderDialog( wxWindow * p_pParent, Castor3D::ShaderManager * p_pManager, Castor3D::PassPtr p_pPass, const wxPoint & p_ptPosition=wxPoint( 0, 0), const wxSize p_ptSize=wxSize( 800, 600));
		~ShaderDialog();

	private:
		void _cleanup();
		void _loadShader();
		void _openShaderFile();
		void _onFolder( Castor3D::eSHADER_PROGRAM_TYPE p_eType);
		void _onGridCellChange( Castor3D::eSHADER_PROGRAM_TYPE p_eType, int p_iRow);

		DECLARE_EVENT_TABLE()
		void _onVertexFolder			( wxCommandEvent & event);
		void _onFragmentFolder			( wxCommandEvent & event);
		void _onGeometryFolder			( wxCommandEvent & event);
		void _onLoadShader				( wxCommandEvent & event);
		void _onClose					( wxCloseEvent & event);
		void _onOk						( wxCommandEvent & event);
		void _onCancel					( wxCommandEvent & event);
		void _onGridVertexCellChange	( wxCommandEvent & event);
		void _onGridFragmentCellChange	( wxCommandEvent & event);
		void _onGridGeometryCellChange	( wxCommandEvent & event);
	};
}

#endif