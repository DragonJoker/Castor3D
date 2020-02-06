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
#ifndef ___GUICOMMON_SUBMESH_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_SUBMESH_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Submesh helper class to communicate between Scene objects or Materials lists and PropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesHolder, pour les maillages
	*/
	class SubmeshTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_geometry	The parent geometry
		 *\param[in]	p_submesh	The target submesh
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_geometry	La géométrie parente
		 *\param[in]	p_submesh	Le maillage cible
		 */
		SubmeshTreeItemProperty( bool p_editable, Castor3D::Geometry & p_geometry, Castor3D::Submesh & p_submesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~SubmeshTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the submesh
		 *\return		The value
		 *\~french
		 *\brief		Récupère le maillage
		 *\return		La valeur
		 */
		inline Castor3D::Submesh & GetSubmesh()
		{
			return m_submesh;
		}
		/**
		 *\~english
		 *\brief		Retrieves the geometry
		 *\return		The value
		 *\~french
		 *\brief		Récupère la géométrie
		 *\return		La valeur
		 */
		inline Castor3D::Geometry & GetGeometry()
		{
			return m_geometry;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::DoCreateProperties
		 */
		virtual void DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::DoPropertyChange
		 */
		virtual void DoPropertyChange( wxPropertyGridEvent & p_event );

	private:
		void OnMaterialChange( Castor::String const & p_name );
		void OnTopologyChange( Castor3D::Topology p_value );

	private:
		Castor3D::Geometry & m_geometry;
		Castor3D::Submesh & m_submesh;
	};
}

#endif
