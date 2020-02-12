/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___
#define ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

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
	 *\brief		Construit un wxArrayString à partir d'un array de wxString.
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
	 *\brief		Construit un make_wxArrayInt à partir d'un array d'int.
	*\param[in]		p_values	L'array.
	 *\return		Le make_wxArrayInt.
	 */
	template< size_t Count >
	wxArrayInt make_wxArrayInt( std::array< int, Count > p_values )
	{
		wxArrayInt result{ Count };
		std::memcpy( &result[0], p_values.data(), Count * sizeof( int ) );
		return result;
	}
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		The supported object types, for display, and properties
	\~french
	\brief		Les type d'objets supportés, dans les propriétés, et à l'affichage
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
		ePROPERTY_DATA_TYPE_POST_EFFECT,
		ePROPERTY_DATA_TYPE_TONE_MAPPING,
		ePROPERTY_DATA_TYPE_BONE,
		ePROPERTY_DATA_TYPE_SKELETON,
		ePROPERTY_DATA_TYPE_SKELETON_ANIMATION,
		ePROPERTY_DATA_TYPE_BACKGROUND,
	}	ePROPERTY_DATA_TYPE;
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer
	*/
	class TreeItemProperty
		: public wxTreeItemData
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The engine, to post events to.
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_type		The object type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur, auquel on va poster les évènements.
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_type		Le type d'objet
		 */
		TreeItemProperty( castor3d::Engine * engine, bool p_editable, ePROPERTY_DATA_TYPE p_type );
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
		 *\brief		Affiche le menu de l'objet du wxTree, aux coordonnàes donnàes
		*\param[in]		p_window	Le wxWindow qui affiche le menu
		 *\param[in]	x, y		Les coordonnàes
		 */
		void DisplayTreeItemMenu( wxWindow * p_window, wxCoord x, wxCoord y );
		/**
		 *\~english
		 *\brief		Creates and fills the item properties, in the given wxPropertyGrid
		 *\param[in]	p_editor	The button editor, for properties that need it
		 *\param[in]	p_grid		The target wxPropertyGrid
		 *\~french
		 *\brief		Construit et remplit les propriétés de l'objet, dans la wxPropertyGrid donnàe
		 *\param[in]	p_editor	L'àditeur bouton, pour les propriétés en ayant besoin
		 *\param[in]	p_grid		La wxPropertyGrid cible
		 */
		void CreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\~english
		 *\brief		Call when a property grid property is changed
		 *\param[in]	p_event	The event
		 *\~french
		 *\brief		Appelàe lorsqu'une propriété est changàe
		 *\param[in]	p_event	L'évènement
		 */
		void onPropertyChange( wxPropertyGridEvent & p_event );
		/**
		 *\~english
		 *\brief		Retrieves the object type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type d'objet
		 *\return		La valeur
		 */
		inline ePROPERTY_DATA_TYPE getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Retrieves the editable status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de modifiabilità
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
		 *\param[in]	engine	The engine, to retrieve the materials
		 *\return		The created property
		 *\~french
		 *\brief		Cràe une propriété de sàlection de matàriau
		 *\param[in]	p_name		Le nom de la propriété
		 *\param[in]	engine	Le moteur, pour ràcupàrer les matériaux
		 *\return		La propriété crààe.
		 */
		wxEnumProperty * doCreateMaterialProperty( wxString const & p_name );
		/**
		 *\~english
		 *\brief		Posts an functor event to the engine.
		 *\param[in]	engine	The engine, to retrieve the materials
		 *\param[in]	p_functor	The function to execute
		 *\~french
		 *\brief		Poste un évènement functeur au moteur
		 *\param[in]	engine	Le moteur, pour ràcupàrer les matériaux
		 *\param[in]	p_functor	La fonction à exàcuter
		 */
		void doApplyChange( std::function< void() > p_functor );
		/**
		 *\~english
		 *\brief		Creates the menu displayed for the wxTree item, available if m_editable is true
		 *\~french
		 *\brief		Cràe le menu affichà pour l'objet du wxTree, disponible si m_editable est à true
		 */
		void CreateTreeItemMenu();
		/**
		 *\~english
		 *\brief		Creates the menu displayed for the wxTree item, available if m_editable is true
		 *\~french
		 *\brief		Cràe le menu affichà pour l'objet du wxTree, disponible si m_editable est à true
		 */
		virtual void doCreateTreeItemMenu();
		/**
		 *\~english
		 *\brief		Creates and fills the overlay properties, in the given wxPropertyGrid
		 *\param[in]	p_editor	The button editor, for properties that need it
		 *\param[in]	p_grid		The target wxPropertyGrid
		 *\~french
		 *\brief		Construit et remplit les propriétés de l'incrustation, dans la wxPropertyGrid donnàe
		 *\param[in]	p_grid	La wxPropertyGrid cible
		 */
		virtual void doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid ) = 0;
		/**
		 *\~english
		 *\brief		Call when a property grid property is changed
		 *\param[in]	p_event	The event
		 *\~french
		 *\brief		Appelàe lorsqu'une propriété est changàe
		 *\param[in]	p_event	L'évènement
		 */
		virtual void doPropertyChange( wxPropertyGridEvent & p_event ) = 0;

	protected:
		wxMenu * m_menu;

	private:
		ePROPERTY_DATA_TYPE m_type;
		bool m_editable;
		castor3d::Engine * m_engine;
	};
}

#endif
