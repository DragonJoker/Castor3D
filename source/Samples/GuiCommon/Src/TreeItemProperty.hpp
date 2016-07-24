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
#ifndef ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___
#define ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/treectrl.h>
#include <wx/propgrid/propgrid.h>

namespace GuiCommon
{
	/**
	 *\~english
	 *\brief		Builds a wxArrayString from a an array of wxString.
	 *\param[in]	p_values	The array.
	 *\return		The wxArrayString.
	 *\~french
	 *\brief		Construit un wxArrayString � partir d'un array de wxString.
	*\param[in]		p_values	L'array.
	 *\return		Le wxArrayString.
	 */
	template< size_t Count >
	wxArrayString make_wxArrayString( std::array< wxString, Count > p_values )
	{
		return wxArrayString{ Count, p_values.data() };
	}
	/**
	 *\~english
	 *\brief		Builds a make_wxArrayInt from a an array of int.
	 *\param[in]	p_values	The array.
	 *\return		The make_wxArrayInt.
	 *\~french
	 *\brief		Construit un make_wxArrayInt � partir d'un array d'int.
	*\param[in]		p_values	L'array.
	 *\return		Le make_wxArrayInt.
	 */
	template< size_t Count >
	wxArrayInt make_wxArrayInt( std::array< int, Count > p_values )
	{
		wxArrayInt l_return{ Count };
		std::memcpy( &l_return[0], p_values.data(), Count * sizeof( int ) );
		return l_return;
	}
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
		 *\param[in]	p_window	The wxWindow that displays the menu
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
