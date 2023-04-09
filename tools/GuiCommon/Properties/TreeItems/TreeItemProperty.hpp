/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___
#define ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include "GuiCommon/Properties/Math/MatrixProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/Math/PositionProperties.hpp"
#include "GuiCommon/Properties/Math/QuaternionProperties.hpp"
#include "GuiCommon/Properties/Math/RectangleProperties.hpp"
#include "GuiCommon/Properties/Math/SizeProperties.hpp"

#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Material/Pass/PassVisitor.hpp>
#include <Castor3D/Miscellaneous/PipelineVisitor.hpp>

#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <wx/treectrl.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace GuiCommon
{
	/**
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
		bool IsEditable()const
		{
			return m_editable;
		}

		void setPrefix( castor::String const & prefix )
		{
			m_prefix = prefix;
		}

	protected:
		/**
		 *\return		The materials names.
		 */
		wxArrayString getMaterialsList()const;
		/**
		 *\return		The fonts names.
		 */
		wxArrayString getFontsList()const;
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
		template< typename ParentT, typename MyValueT, typename ControlT >
		wxPGProperty * createProperty( ParentT * parent
			, wxString const & name
			, MyValueT && value
			, PropertyChangeHandler handler
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls );
		template< typename ParentT, typename EnumT, typename FuncT, typename ControlT = bool >
		wxPGProperty * addPropertyE( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, FuncT func
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename EnumT, typename FuncT, typename ControlT = bool >
		wxPGProperty * addPropertyE( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, EnumT selected
			, FuncT func
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
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
		template< typename ParentT, typename ValueT, typename ControlT = bool >
		wxPGProperty * addProperty( ParentT * parent
			, wxString const & name
			, ValueT const & value
			, PropertyChangeHandler handler
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ValueT, typename ControlT = bool >
		wxPGProperty * addProperty( ParentT * parent
			, wxString const & name
			, ValueT const & value
			, castor::Range< ValueT > const & range
			, PropertyChangeHandler handler
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ValueT, typename ControlT = bool >
		wxPGProperty * addProperty( ParentT * parent
			, wxString const & name
			, ValueT const & value
			, ValueT const & step
			, PropertyChangeHandler handler
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
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
		wxPGProperty * addProperty( wxPropertyGrid * parent
			, wxString const & name
			, wxArrayString const & choices
			, wxString const & selected
			, PropertyChangeHandler handler
			, std::atomic_bool * control );
		wxPGProperty * addProperty( wxPGProperty * parent
			, wxString const & name
			, wxArrayString const & choices
			, wxString const & selected
			, PropertyChangeHandler handler
			, std::atomic_bool * control );
		wxPGProperty * addProperty( wxPropertyGrid * parent
			, wxString const & name
			, wxPGEditor * editor
			, PropertyChangeHandler handler
			, std::atomic_bool * control );
		wxPGProperty * addProperty( wxPGProperty * parent
			, wxString const & name
			, wxPGEditor * editor
			, PropertyChangeHandler handler
			, std::atomic_bool * control );
		wxPGProperty * addMaterial( wxPropertyGrid * parent
			, castor3d::Engine & engine
			, wxString const & name
			, wxArrayString const & choices
			, castor3d::MaterialObs selected
			, std::function< void( castor3d::MaterialObs ) > setter );

		template< typename ParentT, typename ValueT, typename ControlT = bool >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, castor::RangedValue< ValueT > * value
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ValueT, typename ControlT = bool >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT * value
			, castor::Range< ValueT > const & range
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ValueT, typename ControlT = bool >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, castor::ChangeTracked< castor::RangedValue< ValueT > > * value
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ValueT, typename ControlT = bool >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT * value
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ValueT, typename ControlT = bool >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT * value
			, ValueT step
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT = bool >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT value
			, ObjectT * object
			, ValueSetterT< ObjectU, ValueT > setter
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT = bool >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT  value
			, ValueT step
			, ObjectT * object
			, ValueSetterT< ObjectU, ValueT > setter
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT = bool >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT const & value
			, ObjectT * object
			, ValueRefSetterT< ObjectU, ValueT > setter
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT = bool >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, castor::RangedValue< ValueT > const & value
			, ObjectT * object
			, ValueSetterT< ObjectU, ValueT > setter
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT = bool >
		wxPGProperty * addPropertyET( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, ObjectT * object
			, ValueSetterT< ObjectU, EnumT > setter
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT = bool >
		wxPGProperty * addPropertyET( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, EnumT selected
			, ObjectT * object
			, ValueSetterT< ObjectU, EnumT > setter
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );
		template< typename ParentT, typename EnumT, typename ControlT = bool >
		wxPGProperty * addPropertyET( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, EnumT * value
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{}
			, castor3d::PipelineVisitor::OnEnumValueChangeT< EnumT > onChange = []( EnumT, EnumT ) {} );
		template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT = bool >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, wxString const & selected
			, ObjectT * object
			, ValueSetterT< ObjectU, EnumT > setter
			, castor3d::PassVisitorBase::ControlsListT< ControlT > controls = castor3d::PassVisitorBase::ControlsListT< ControlT >{} );

		template< typename ParentT, typename MyValueT, typename ControlT >
		wxPGProperty * createProperty( ParentT * parent
			, wxString const & name
			, MyValueT && value
			, PropertyChangeHandler handler
			, ControlT * control )
		{
			return createProperty( parent
				, name
				, std::forward< MyValueT >( value )
				, handler
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename EnumT, typename FuncT, typename ControlT >
		wxPGProperty * addPropertyE( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, FuncT func
			, ControlT * control )
		{
			return addPropertyE( parent
				, name
				, choices
				, func
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename EnumT, typename FuncT, typename ControlT >
		wxPGProperty * addPropertyE( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, EnumT selected
			, FuncT func
			, ControlT * control )
		{
			return addPropertyE( parent
				, name
				, choices
				, selected
				, func
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ValueT, typename ControlT >
		wxPGProperty * addProperty( ParentT * parent
			, wxString const & name
			, ValueT const & value
			, PropertyChangeHandler handler
			, ControlT * control )
		{
			return addProperty( parent
				, name
				, value
				, handler
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ValueT, typename ControlT >
		wxPGProperty * addProperty( ParentT * parent
			, wxString const & name
			, ValueT const & value
			, castor::Range< ValueT > const & range
			, PropertyChangeHandler handler
			, ControlT * control )
		{
			return addProperty( parent
				, name
				, value
				, range
				, handler
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ValueT, typename ControlT >
		wxPGProperty * addProperty( ParentT * parent
			, wxString const & name
			, ValueT const & value
			, ValueT const & step
			, PropertyChangeHandler handler
			, ControlT * control )
		{
			return addProperty( parent
				, name
				, value
				, step
				, handler
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}


		template< typename ParentT, typename ValueT, typename ControlT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, castor::RangedValue< ValueT > * value
			, ControlT * control )
		{
			return addPropertyT( parent
				, name
				, value
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ValueT, typename ControlT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT * value
			, castor::Range< ValueT > const & range
			, ControlT * control )
		{
			return addPropertyT( parent
				, name
				, value
				, range
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ValueT, typename ControlT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, castor::ChangeTracked< castor::RangedValue< ValueT > > * value
			, ControlT * control )
		{
			return addPropertyT( parent
				, name
				, value
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ValueT, typename ControlT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT * value
			, ControlT * control )
		{
			return addPropertyT( parent
				, name
				, value
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ValueT, typename ControlT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT * value
			, ValueT step
			, ControlT * control )
		{
			return addPropertyT( parent
				, name
				, value
				, step
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT value
			, ObjectT * object
			, ValueSetterT< ObjectU, ValueT > setter
			, ControlT * control )
		{
			return addPropertyT( parent
				, name
				, value
				, object
				, setter
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT value
			, ValueT step
			, ObjectT * object
			, ValueSetterT< ObjectU, ValueT > setter
			, ControlT * control )
		{
			return addPropertyT( parent
				, name
				, value
				, step
				, object
				, setter
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, ValueT const & value
			, ObjectT * object
			, ValueRefSetterT< ObjectU, ValueT > setter
			, ControlT * control )
		{
			return addPropertyT( parent
				, name
				, value
				, object
				, setter
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, castor::RangedValue< ValueT > const & value
			, ObjectT * object
			, ValueSetterT< ObjectU, ValueT > setter
			, ControlT * control )
		{
			return addPropertyT( parent
				, name
				, value
				, object
				, setter
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT >
		wxPGProperty * addPropertyET( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, ObjectT * object
			, ValueSetterT< ObjectU, EnumT > setter
			, ControlT * control )
		{
			return addPropertyET( parent
				, name
				, choices
				, object
				, setter
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT >
		wxPGProperty * addPropertyET( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, EnumT selected
			, ObjectT * object
			, ValueSetterT< ObjectU, EnumT > setter
			, ControlT * control )
		{
			return addPropertyET( parent
				, name
				, choices
				, selected
				, object
				, setter
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

		template< typename ParentT, typename EnumT, typename ControlT >
		wxPGProperty * addPropertyET( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, EnumT * value
			, ControlT * control
			, castor3d::PipelineVisitor::OnEnumValueChangeT< EnumT > onChange = []( EnumT, EnumT ) {} )
		{
			return addPropertyET( parent
				, name
				, choices
				, value
				, castor3d::PassVisitorBase::makeControlsList( control )
				, onChange);
		}

		template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT >
		wxPGProperty * addPropertyT( ParentT * parent
			, wxString const & name
			, wxArrayString const & choices
			, wxString const & selected
			, ObjectT * object
			, ValueSetterT< ObjectU, EnumT > setter
			, ControlT * control )
		{
			return addPropertyT( parent
				, name
				, choices
				, selected
				, object
				, setter
				, castor3d::PassVisitorBase::makeControlsList( control ) );
		}

	private:
		PropertyChangeHandler doGetHandler( PropertyChangeHandler handler
			, castor3d::PassVisitorBase::ControlsList controls );
		PropertyChangeHandler doGetHandler( PropertyChangeHandler handler
			, castor3d::PassVisitorBase::AtomicControlsList controls );

	protected:
		wxMenu * m_menu;

	private:
		bool m_editable;
		castor3d::Engine * m_engine;
		std::map< wxString, PropertyChangeHandler > m_handlers;
		castor::String m_prefix;
	};
}

#include "TreeItemProperty.inl"

#endif
