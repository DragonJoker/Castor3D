/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___
#define ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/treectrl.h>
#include <wx/propgrid/propgrid.h>

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		The supported object types, for display, and properties
	\~french
	\brief		Les type d'objets support�s, dans les propri�t�s, et � l'affichage
	*/
	typedef enum ePROPERTY_DATA_TYPE
	{
		ePROPERTY_DATA_TYPE_SCENE,
		ePROPERTY_DATA_TYPE_RENDER_TARGET,
		ePROPERTY_DATA_TYPE_VIEWPORT,
		ePROPERTY_DATA_TYPE_RENDER_WINDOW,
		ePROPERTY_DATA_TYPE_BILLBOARD,
		ePROPERTY_DATA_TYPE_CAMERA,
		ePROPERTY_DATA_TYPE_GEOMETRY,
		ePROPERTY_DATA_TYPE_SUBMESH,
		ePROPERTY_DATA_TYPE_LIGHT,
		ePROPERTY_DATA_TYPE_NODE,
		ePROPERTY_DATA_TYPE_OVERLAY,
		ePROPERTY_DATA_TYPE_MATERIAL,
		ePROPERTY_DATA_TYPE_PASS,
		ePROPERTY_DATA_TYPE_TEXTURE,
		ePROPERTY_DATA_TYPE_ANIMATED_OBJECT_GROUP,
		ePROPERTY_DATA_TYPE_ANIMATED_OBJECT,
		ePROPERTY_DATA_TYPE_ANIMATION,
	}	ePROPERTY_DATA_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Helper class to communicate between Scene objects or Materials lists and PropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de sc�ne, ou la liste de mat�riaux, et PropertiesHolder
	*/
	class TreeItemProperty
		: public wxTreeItemData
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine	The engine, to post events to.
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_type		The object type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur, auquel on va poster les �v�nements.
		 *\param[in]	p_editable	Dit si les propri�t�s sont modifiables
		 *\param[in]	p_type		Le type d'objet
		 */
		TreeItemProperty( Castor3D::Engine * p_engine, bool p_editable, ePROPERTY_DATA_TYPE p_type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TreeItemProperty();
		/**
		 *\~english
		 *\brief		Displays the wxTree item menu, at given coordinates
		 *\param[in]	p_window	The wxWindow thqt displays the menu
		 *\param[in]	x, y		The coordinates
		 *\~french
		 *\brief		Affiche le menu de l'objet du wxTree, aux coordonn�es donn�es
		*\param[in]		p_window	Le wxWindow qui affiche le menu
		 *\param[in]	x, y		Les coordonn�es
		 */
		void DisplayTreeItemMenu( wxWindow * p_window, wxCoord x, wxCoord y );
		/**
		 *\~english
		 *\brief		Creates and fills the item properties, in the given wxPropertyGrid
		 *\param[in]	p_editor	The button editor, for properties that need it
		 *\param[in]	p_grid		The target wxPropertyGrid
		 *\~french
		 *\brief		Construit et remplit les propri�t�s de l'objet, dans la wxPropertyGrid donn�e
		 *\param[in]	p_editor	L'�diteur bouton, pour les propri�t�s en ayant besoin
		 *\param[in]	p_grid		La wxPropertyGrid cible
		 */
		void CreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\~english
		 *\brief		Call when a property grid property is changed
		 *\param[in]	p_event	The event
		 *\~french
		 *\brief		Appel�e lorsqu'une propri�t� est chang�e
		 *\param[in]	p_event	L'�v�nement
		 */
		void OnPropertyChange( wxPropertyGridEvent & p_event );
		/**
		 *\~english
		 *\brief		Retrieves the object type
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le type d'objet
		 *\return		La valeur
		 */
		inline ePROPERTY_DATA_TYPE GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Retrieves the editable status
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le statut de modifiabilit�
		 *\return		La valeur
		 */
		inline bool IsEditable()const
		{
			return m_editable;
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates a material selector property
		 *\param[in]	p_name		The property name
		 *\param[in]	p_engine	The engine, to retrieve the materials
		 *\return		The created property
		 *\~french
		 *\brief		Cr�e une propri�t� de s�lection de mat�riau
		 *\param[in]	p_name		Le nom de la propri�t�
		 *\param[in]	p_engine	Le moteur, pour r�cup�rer les mat�riaux
		 *\return		La propri�t� cr��e.
		 */
		wxEnumProperty * DoCreateMaterialProperty( wxString const & p_name );
		/**
		 *\~english
		 *\brief		Posts an functor event to the engine.
		 *\param[in]	p_engine	The engine, to retrieve the materials
		 *\param[in]	p_functor	The function to execute
		 *\~french
		 *\brief		Poste un �v�nement functeur au moteur
		 *\param[in]	p_engine	Le moteur, pour r�cup�rer les mat�riaux
		 *\param[in]	p_functor	La fonction � ex�cuter
		 */
		void DoApplyChange( std::function< void() > p_functor );
		/**
		 *\~english
		 *\brief		Creates the menu displayed for the wxTree item, available if m_editable is true
		 *\~french
		 *\brief		Cr�e le menu affich� pour l'objet du wxTree, disponible si m_editable est � true
		 */
		void CreateTreeItemMenu();
		/**
		 *\~english
		 *\brief		Creates the menu displayed for the wxTree item, available if m_editable is true
		 *\~french
		 *\brief		Cr�e le menu affich� pour l'objet du wxTree, disponible si m_editable est � true
		 */
		virtual void DoCreateTreeItemMenu();
		/**
		 *\~english
		 *\brief		Creates and fills the overlay properties, in the given wxPropertyGrid
		 *\param[in]	p_editor	The button editor, for properties that need it
		 *\param[in]	p_grid		The target wxPropertyGrid
		 *\~french
		 *\brief		Construit et remplit les propri�t�s de l'incrustation, dans la wxPropertyGrid donn�e
		 *\param[in]	p_grid	La wxPropertyGrid cible
		 */
		virtual void DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid ) = 0;
		/**
		 *\~english
		 *\brief		Call when a property grid property is changed
		 *\param[in]	p_event	The event
		 *\~french
		 *\brief		Appel�e lorsqu'une propri�t� est chang�e
		 *\param[in]	p_event	L'�v�nement
		 */
		virtual void DoPropertyChange( wxPropertyGridEvent & p_event ) = 0;

	protected:
		wxMenu * m_menu;

	private:
		ePROPERTY_DATA_TYPE m_type;
		bool m_editable;
		Castor3D::Engine * m_engine;
	};
}

#endif
