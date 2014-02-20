#ifndef ___CS_MaterialsFrame___
#define ___CS_MaterialsFrame___

#include "Module_Material.hpp"

namespace CastorShape
{
	class wxMaterialsFrame : public GuiCommon::wxMaterialsFrame
	{
	public:
		enum eIDs
		{
			mlfNewMaterial,
			mlfNewMaterialName,
			mlfDeleteMaterial,
			mlfMaterialsList,
		};

	private:
		GuiCommon::wxMaterialPanel *	m_materialPanel;
		wxButton *						m_newMaterial;
		wxButton *						m_deleteMaterial;
		wxTextCtrl *					m_newMaterialName;

	public:
		wxMaterialsFrame( Castor3D::Engine * p_pEngine, wxWindow * parent, wxString const & title, wxPoint const & pos = wxDefaultPosition, wxSize const & size = wxSize( 500, 500 ) );
		~wxMaterialsFrame();

		virtual void SetMaterialName( Castor::String const & p_materialName);
		virtual void Initialise();

	private:
		DECLARE_EVENT_TABLE()
		void OnNewMaterial			( wxCommandEvent & p_event);
		void OnNewMaterialName		( wxCommandEvent & p_event);
		void OnDeleteMaterial		( wxCommandEvent & p_event);
	};
}

#endif
