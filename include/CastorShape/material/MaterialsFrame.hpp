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
		GuiCommon::wxMaterialPanel * m_materialPanel;
		wxButton * m_newMaterial;
		wxButton * m_deleteMaterial;
		wxTextCtrl * m_newMaterialName;

	public:
		wxMaterialsFrame( wxWindow * parent, const wxString & title,
						  const wxPoint & pos = wxDefaultPosition,
						  const wxSize & size = wxSize( 500, 500));
		~wxMaterialsFrame();

		virtual void CreateMaterialPanel( const String & p_materialName);

	private:
		DECLARE_EVENT_TABLE()
		void _onNewMaterial			( wxCommandEvent & event);
		void _onNewMaterialName		( wxCommandEvent & event);
		void _onDeleteMaterial		( wxCommandEvent & event);
	};
}

#endif
