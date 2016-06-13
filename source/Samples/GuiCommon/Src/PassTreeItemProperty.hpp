/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GUICOMMON_PASS_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_PASS_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Geometry helper class to communicate between Scene objects or Materials lists and PropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de sc�ne, ou la liste de mat�riaux, et PropertiesHolder, pour les g�om�tries
	*/
	class PassTreeItemProperty
		: public TreeItemProperty
		, private wxEvtHandler
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_editable	Tells if the properties are modifiable.
		 *\param[in]	p_pass		The target pass.
		 *\param[in]	p_scene		The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_editable	Dit si les propri�t�s sont modifiables.
		 *\param[in]	p_pass		La passe cible.
		 *\param[in]	p_scene		La sc�ne.
		 */
		PassTreeItemProperty( bool p_editable, Castor3D::PassSPtr p_pass, Castor3D::Scene & p_scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~PassTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the geometry
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re la g�om�trie
		 *\return		La valeur
		 */
		inline Castor3D::PassSPtr GetPass()
		{
			return m_pass.lock();
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
		void OnAmbientColourChange( Castor::Colour const & p_value );
		void OnDiffuseColourChange( Castor::Colour const & p_value );
		void OnSpecularColourChange( Castor::Colour const & p_value );
		void OnEmissiveColourChange( Castor::Colour const & p_value );
		void OnExponentChange( double p_value );
		void OnTwoSidedChange( bool p_value );
		void OnOpacityChange( double p_value );
		bool OnEditShader( wxPGProperty * p_property );

	private:
		Castor3D::PassWPtr m_pass;
		Castor3D::Scene & m_scene;
	};
}

#endif
