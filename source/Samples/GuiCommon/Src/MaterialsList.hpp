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
#ifndef ___GUICOMMON_MATERIALS_LIST_VIEW_H___
#define ___GUICOMMON_MATERIALS_LIST_VIEW_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/treectrl.h>

namespace GuiCommon
{
	class MaterialsList
		: public wxTreeCtrl
	{
	public:
		MaterialsList( PropertiesHolder * p_propertiesHolder
			, wxWindow * p_parent
			, wxPoint const & p_ptPos = wxDefaultPosition
			, wxSize const & p_size = wxDefaultSize );
		~MaterialsList();

		void LoadMaterials( Castor3D::Engine * engine
			, Castor3D::Scene & p_scene );
		void UnloadMaterials();

	private:
		void DoAddMaterial( wxTreeItemId p_id
			, Castor3D::MaterialSPtr p_material );
		void DoAddPass( wxTreeItemId p_id
			, uint32_t p_index
			, Castor3D::PassSPtr p_pass );
		void DoAddTexture( wxTreeItemId p_id
			, uint32_t p_index
			, Castor3D::TextureUnitSPtr p_texture
			, Castor3D::MaterialType p_type );

		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & p_event );
		void OnSelectItem( wxTreeEvent & p_event );
		void OnMouseRButtonUp( wxTreeEvent & p_event );

	private:
		Castor3D::Engine * m_engine;
		Castor3D::Scene * m_scene;
		PropertiesHolder * m_propertiesHolder;
	};
}

#endif
