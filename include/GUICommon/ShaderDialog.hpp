#ifndef ___GC_ShaderDialog___
#define ___GC_ShaderDialog___

#include "FrameVariableDialog.hpp"

namespace GuiCommon
{
	class wxTextPanel;

	class wxShaderDialog : public wxDialog
	{
	protected:
		typedef enum
		{	eID_BUTTON_VERTEX
		,	eID_BUTTON_FRAGMENT
		,	eID_BUTTON_GEOMETRY
		,	eID_BUTTON_HULL
		,	eID_BUTTON_DOMAIN
		,	eID_BUTTON_LOAD
		,	eID_BUTTON_OK
		,	eID_BUTTON_CANCEL
		,	eID_NOTEBOOK_EDITORS
		,	eID_GRID_VERTEX
		,	eID_GRID_FRAGMENT
		,	eID_GRID_GEOMETRY
		,	eID_GRID_HULL
		,	eID_GRID_DOMAIN
		}	eID;

	protected:
		wxButton * m_pFolders[Castor3D::eSHADER_TYPE_COUNT];
		wxButton * m_loadShader;
		wxNotebook * m_pEditorsBook;
		wxTextPanel * m_pEditors[Castor3D::eSHADER_TYPE_COUNT];
		wxListBox * m_pListsFrameVariables[Castor3D::eSHADER_TYPE_COUNT];

		wxButton * m_pButtonOk;
		wxButton * m_pButtonCancel;

		wxString m_strShaders[Castor3D::eSHADER_TYPE_COUNT];

		Castor3D::ShaderProgramPtr m_shaderProgram;
		Castor3D::ShaderObjectPtr m_shaderObject;
		Castor3D::PassPtr m_pPass;
		bool m_bCompiled;
		bool m_bOwnShader;

		KeyedContainer< int, Castor3D::FrameVariablePtr>::Map m_mapFrameVariables[Castor3D::eSHADER_TYPE_COUNT];

	public:
		wxShaderDialog( wxWindow * p_pParent, Castor3D::PassPtr p_pPass, const wxPoint & p_ptPosition=wxPoint( 0, 0), const wxSize p_ptSize=wxSize( 800, 600));
		~wxShaderDialog();

	private:
		void _cleanup();
		void _loadShader();
		void _openShaderFile();
		void _onFolder( Castor3D::eSHADER_TYPE p_eType);
		void _onGridCellChange( Castor3D::eSHADER_TYPE p_eType, int p_iRow);

		DECLARE_EVENT_TABLE()
		void _onVertexFolder			( wxCommandEvent & event);
		void _onFragmentFolder			( wxCommandEvent & event);
		void _onGeometryFolder			( wxCommandEvent & event);
		void _onHullFolder				( wxCommandEvent & event);
		void _onDomainFolder			( wxCommandEvent & event);
		void _onLoadShader				( wxCommandEvent & event);
		void _onClose					( wxCloseEvent & event);
		void _onOk						( wxCommandEvent & event);
		void _onCancel					( wxCommandEvent & event);
		void _onGridVertexCellChange	( wxCommandEvent & event);
		void _onGridFragmentCellChange	( wxCommandEvent & event);
		void _onGridGeometryCellChange	( wxCommandEvent & event);
		void _onGridHullCellChange		( wxCommandEvent & event);
		void _onGridDomainCellChange	( wxCommandEvent & event);
	};
}

#endif
