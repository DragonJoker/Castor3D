#ifndef ___CS_MaterialsFrame___
#define ___CS_MaterialsFrame___

#include "Module_Material.h"

namespace CastorShape
{
	class MaterialsFrame : public GuiCommon::MaterialsFrame
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
		GuiCommon::MaterialPanel * m_materialPanel;
		wxButton * m_newMaterial;
		wxButton * m_deleteMaterial;
		wxTextCtrl * m_newMaterialName;

	public:
		MaterialsFrame( Castor3D::MaterialManager * p_pManager, wxWindow * parent, const wxString & title,
						  const wxPoint & pos = wxDefaultPosition,
						  const wxSize & size = wxSize( 500, 500));
		~MaterialsFrame();

		virtual void CreateMaterialPanel( const String & p_materialName);

	private:
		DECLARE_EVENT_TABLE()
		void _onNewMaterial			( wxCommandEvent & event);
		void _onNewMaterialName		( wxCommandEvent & event);
		void _onDeleteMaterial		( wxCommandEvent & event);
	};
}

#endif
