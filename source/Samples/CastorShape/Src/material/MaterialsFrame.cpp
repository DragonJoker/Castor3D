#include "../material/MaterialsFrame.hpp"

using namespace Castor3D;
using namespace CastorShape;
using namespace Castor;

wxMaterialsFrame::wxMaterialsFrame( Castor3D::Engine * p_pEngine, wxWindow * parent, wxString const & title, wxPoint const & pos, wxSize const & size )
	:	GuiCommon::wxMaterialsFrame( p_pEngine, true, parent, title, pos, size )
{
}

wxMaterialsFrame::~wxMaterialsFrame()
{
}

void wxMaterialsFrame::Initialise()
{
	wxSizer * l_pSizer = GuiCommon::wxMaterialsFrame::DoBaseInit();
	wxSize l_size = GetClientSize();
	m_newMaterial		= new wxButton(	this, mlfNewMaterial,		_( "New"	),	wxDefaultPosition, wxSize( 80, 25 ), wxBORDER_SIMPLE	);
	m_deleteMaterial	= new wxButton(	this, mlfDeleteMaterial,	_( "Delete" ),	wxDefaultPosition, wxSize( 80, 25 ), wxBORDER_SIMPLE	);
	m_newMaterialName	= new wxTextCtrl(	this, mlfNewMaterialName,	wxEmptyString,	wxDefaultPosition, wxSize( 100, 25 ), wxTE_PROCESS_ENTER | wxBORDER_SIMPLE	);
	wxBoxSizer *	l_pSizerAll	= new wxBoxSizer( wxVERTICAL	);
	wxBoxSizer *	l_pSizerBtn	= new wxBoxSizer( wxHORIZONTAL	);
	l_pSizerBtn->Add(	m_newMaterial,		wxSizerFlags( 0 )	);
	l_pSizerBtn->Add(	m_newMaterialName,	wxSizerFlags( 1 )	);
	l_pSizerBtn->Add(	m_deleteMaterial,	wxSizerFlags( 0 )	);
	l_pSizerAll->Add(	l_pSizerBtn,		wxSizerFlags( 0 ).Expand().Border( wxALL, 5 )	);
	l_pSizerAll->Add(	l_pSizer,			wxSizerFlags( 1 ).Expand()	);
	GuiCommon::wxMaterialsFrame::DoEndInit( l_pSizerAll );
	m_deleteMaterial->Hide();
	m_newMaterialName->Hide();
}

void wxMaterialsFrame::SetMaterialName( String const & p_materialName )
{
	GuiCommon::wxMaterialsFrame::SetMaterialName( p_materialName );
	m_deleteMaterial->Show();
}

BEGIN_EVENT_TABLE( wxMaterialsFrame, GuiCommon::wxMaterialsFrame )
	EVT_LIST_ITEM_SELECTED(	mlfMaterialsList, 	wxMaterialsFrame::OnSelected	)
	EVT_LIST_ITEM_DESELECTED(	mlfMaterialsList, 	wxMaterialsFrame::OnDeselected	)
	EVT_BUTTON(	mlfNewMaterial,						wxMaterialsFrame::OnNewMaterial	)
	EVT_BUTTON(	mlfDeleteMaterial,					wxMaterialsFrame::OnDeleteMaterial	)
	EVT_TEXT_ENTER( mlfNewMaterialName,				wxMaterialsFrame::OnNewMaterialName	)
END_EVENT_TABLE()

void wxMaterialsFrame::OnNewMaterial( wxCommandEvent & WXUNUSED( p_event ) )
{
	m_newMaterialName->Show();
	m_newMaterialName->SetFocus();
}

void wxMaterialsFrame::OnNewMaterialName( wxCommandEvent & WXUNUSED( p_event ) )
{
	String l_strName = ( wxChar const * )m_newMaterialName->GetValue().c_str();
	MaterialManager & l_mtlManager = m_pEngine->GetMaterialManager();

	if ( !l_mtlManager.has( l_strName ) )
	{
		MaterialSPtr l_pMaterial = std::make_shared< Material >( m_pEngine, l_strName );
		l_pMaterial->CreatePass();
		m_pSelectedMaterial = l_pMaterial;
		l_mtlManager.insert( l_strName, l_pMaterial );
		m_pMaterialsList->AddItem( l_strName );
		SetMaterialName( l_strName );
		m_newMaterialName->Hide();
		m_newMaterialName->SetValue( cuEmptyString );
	}
	else
	{
		wxMessageBox( _( "A material with this name already exists" ), _( "Error" ) );
	}
}

void wxMaterialsFrame::OnDeleteMaterial( wxCommandEvent & WXUNUSED( p_event ) )
{
	SceneSPtr l_scene = m_pEngine->GetSceneManager().find( cuT( "MainScene" ) );
	GeometryPtrStrMap::iterator l_it = l_scene->GeometriesBegin();
	GeometrySPtr l_geometry;
	MeshSPtr l_mesh;
	SubmeshSPtr l_submesh;

	while ( l_it != l_scene->GeometriesEnd() )
	{
		l_geometry = l_it->second;
		l_mesh = l_geometry->GetMesh();
		std::for_each( l_mesh->Begin(), l_mesh->End(), [&]( SubmeshSPtr p_pSubmesh )
		{
			if ( l_geometry->GetMaterial( p_pSubmesh )->GetName() == m_pSelectedMaterial.lock()->GetName() )
			{
				l_geometry->SetMaterial( p_pSubmesh, m_pEngine->GetMaterialManager().GetDefaultMaterial() );
			}
		} );
	}

	m_pEngine->GetMaterialManager().erase( m_pSelectedMaterial.lock()->GetName() );
	m_pSelectedMaterial.reset();
	SetMaterialName( cuEmptyString );
	m_pMaterialsList->CreateList();
}
