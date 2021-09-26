/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___
#define ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#pragma warning( push )
#pragma warning( disable: 4365 )
#include "GuiCommon/Properties/Math/MatrixProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/Math/PositionProperties.hpp"
#include "GuiCommon/Properties/Math/QuaternionProperties.hpp"
#include "GuiCommon/Properties/Math/RectangleProperties.hpp"
#include "GuiCommon/Properties/Math/SizeProperties.hpp"

#include <Castor3D/Render/RenderModule.hpp>

#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <wx/treectrl.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#pragma warning( pop )

namespace GuiCommon
{
	/**
	\version	0.8.0
	\brief		Helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	*/
	class TreeItemProperty
		: public wxTreeItemData
	{
	public:
		/**
		 *\~english
		 *\param[in]	engine	The engine, to post events to.
		 *\param[in]	editable	Tells if the properties are modifiable
		 *\param[in]	type		The object type
		 */
		TreeItemProperty( castor3d::Engine * engine
			, bool editable );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~TreeItemProperty()override;
		/**
		 *\brief		Displays the wxTree item menu, at given coordinates
		 *\param[in]	window	The wxWindow that displays the menu
		 *\param[in]	x, y	The coordinates
		 */
		void DisplayTreeItemMenu( wxWindow * window, wxCoord x, wxCoord y );
		/**
		 *\~english
		 *\brief		Creates and fills the item properties, in the given wxPropertyGrid
		 *\param[in]	editor	The button editor, for properties that need it
		 *\param[in]	grid	The target wxPropertyGrid
		 */
		void CreateProperties( wxPGEditor * editor, wxPropertyGrid * grid );
		/**
		 *\brief		Call when a property grid property is changed
		 *\param[in]	event	The event
		 */
		void onPropertyChange( wxPropertyGridEvent & event );
		/**
		 *\brief		Retrieves the editable status
		 *\return		The value
		 */
		inline bool IsEditable()const
		{
			return m_editable;
		}

	protected:
		/**
		 *\return		The materials names.
		 */
		wxArrayString getMaterialsList();
		/**
		 *\brief		Creates the menu displayed for the wxTree item, available if m_editable is true
		 */
		void CreateTreeItemMenu();
		/**
		 *\brief		Creates the menu displayed for the wxTree item, available if m_editable is true
		 */
		virtual void doCreateTreeItemMenu();
		/**
		 *\brief		Creates and fills the overlay properties, in the given wxPropertyGrid
		 *\param[in]	editor	The button editor, for properties that need it
		 *\param[in]	grid	The target wxPropertyGrid
		 */
		virtual void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid ) = 0;

	public:
		using PropertyChangeHandler = std::function< void ( wxVariant const & ) >;
		static PropertyChangeHandler const EmptyHandler;

		template< typename ObjectT, typename ValueT >
		using ValueRefSetterT = void ( ObjectT::* )( ValueT const & );

		template< typename ObjectT, typename ValueT >
		using ValueSetterT = void ( ObjectT::* )( ValueT );
		wxPGProperty * addProperty( wxPropertyGrid * parent
			, wxString const & name );
		wxPGProperty * addProperty( wxPGProperty * parent
			, wxString const & name );
		template< typename ParentT, typename MyValueT >
		wxPGProperty * createProperty( ParentT * parent
			, wxString const & name
			, MyValueT && value
			, PropertyChangeHandler handler
			, bool * control = nullptr );
		template< typename ParentT, typename EnumT, typename FuncT >
		wxPGProperty * addPropertyE( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, FuncT func
			, bool * control = nullptr );
		template< typename ParentT, typename EnumT, typename FuncT >
		wxPGProperty * addPropertyE( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, EnumT selected
			, FuncT func
			, bool * control = nullptr );
		wxPGProperty * addProperty( wxPropertyGrid * parent
			, wxString const & name
			, wxArrayString const & choices
			, wxString const & selected
			, PropertyChangeHandler handler
			, bool * control = nullptr );
		wxPGProperty * addProperty( wxPGProperty * parent
			, wxString const & name
			, wxArrayString const & choices
			, wxString const & selected
			, PropertyChangeHandler handler
			, bool * control = nullptr );
		template< typename ParentT, typename ValueT >
		wxPGProperty * addProperty( ParentT * parent
			, wxString const & name
			, ValueT const & value
			, PropertyChangeHandler handler
			, bool * control = nullptr );
		template< typename ParentT, typename ValueT >
		wxPGProperty * addProperty( ParentT * parent
			, wxString const & name
			, ValueT const & value
			, ValueT const & step
			, PropertyChangeHandler handler
			, bool * control = nullptr );
		wxPGProperty * addProperty( wxPropertyGrid * parent
			, wxString const & name
			, wxPGEditor * editor
			, PropertyChangeHandler handler
			, bool * control = nullptr );
		wxPGProperty * addProperty( wxPGProperty * parent
			, wxString const & name
			, wxPGEditor * editor
			, PropertyChangeHandler handler
			, bool * control = nullptr );

		template< typename ParentT, typename ValueT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, castor::RangedValue< ValueT > * value
			, bool * control = nullptr );
		template< typename ParentT, typename ValueT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, castor::ChangeTracked< castor::RangedValue< ValueT > > * value
			, bool * control = nullptr );
		template< typename ParentT, typename ValueT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT * value
			, bool * control = nullptr );
		template< typename ParentT, typename ValueT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT * value
			, ValueT step
			, bool * control = nullptr );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT value
			, ObjectT * object
			, ValueSetterT< ObjectU, ValueT > setter
			, bool * control = nullptr );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT  value
			, ValueT step
			, ObjectT * object
			, ValueSetterT< ObjectU, ValueT > setter
			, bool * control = nullptr );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT const & value
			, ObjectT * object
			, ValueRefSetterT< ObjectU, ValueT > setter
			, bool * control = nullptr );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, castor::RangedValue< ValueT > const & value
			, ObjectT * object
			, ValueSetterT< ObjectU, ValueT > setter
			, bool * control = nullptr );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT >
		wxPGProperty * addPropertyET( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, ObjectT * object
			, ValueSetterT< ObjectU, EnumT > setter
			, bool * control = nullptr );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT >
		wxPGProperty * addPropertyET( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, EnumT selected
			, ObjectT * object
			, ValueSetterT< ObjectU, EnumT > setter
			, bool * control = nullptr );
		template< typename ParentT, typename EnumT >
		wxPGProperty * addPropertyET( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, EnumT * value
			, bool * control = nullptr );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, wxString const & selected
			, ObjectT * object
			, ValueSetterT< ObjectU, EnumT > setter
			, bool * control = nullptr );

	private:
		PropertyChangeHandler doGetHandler( PropertyChangeHandler handler
			, bool * control );

	protected:
		wxMenu * m_menu;

	private:
		bool m_editable;
		castor3d::Engine * m_engine;
		std::map< wxString, PropertyChangeHandler > m_handlers;
	};
}

#include "TreeItemProperty.inl"

#endif
