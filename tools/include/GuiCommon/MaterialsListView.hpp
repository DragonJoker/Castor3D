#ifndef ___GC_MaterialsListViewFrame___
#define ___GC_MaterialsListViewFrame___

namespace GuiCommon
{
	class wxMaterialsListView : public wxListView
	{
	private:
		wxImageList *				m_pListImages;
		wxListItem *				m_pListItems;
		uint32_t					m_uiNbItems;
		std::vector< wxImage * >	m_arrayImages;
		int							m_iImgWidth;
		Castor3D::Engine *			m_pEngine;

	public:
		wxMaterialsListView( Castor3D::Engine * p_pEngine, wxWindow * p_pParent, wxWindowID p_id = wxID_ANY, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~wxMaterialsListView();

		wxImage * CreateMaterialImage( Castor3D::MaterialSPtr p_pMaterial, uint32_t p_uiWidth, uint32_t p_uiHeight );
		wxImage * CreatePassImage( Castor3D::PassSPtr p_pPass, uint32_t p_uiWidth, uint32_t p_uiHeight );
		wxImage * CreateTextureUnitImage( Castor3D::TextureUnitSPtr p_pUnit, uint32_t p_uiWidth, uint32_t p_uiHeight );
		void CreateList();
		void AddItem( Castor::String const & p_strMaterialName );
	};
}

#endif
