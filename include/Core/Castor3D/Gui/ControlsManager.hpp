/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ControlsManager_H___
#define ___C3D_ControlsManager_H___

#include "Castor3D/Gui/GuiModule.hpp"
#include "Castor3D/Gui/Theme/Theme.hpp"

#include "Castor3D/Event/UserInput/UserInputListener.hpp"
#include "Castor3D/Event/UserInput/EventHandler.hpp"

#include <CastorUtils/Graphics/Position.hpp>

namespace castor3d
{
	class ControlsManager
		: public std::enable_shared_from_this< ControlsManager >
		, public UserInputListener
		, public StylesHolder
	{
		friend class Control;
		friend class ButtonCtrl;
		friend class ComboBoxCtrl;
		friend class EditCtrl;
		friend class ExpandablePanelCtrl;
		friend class FrameCtrl;
		friend class ListBoxCtrl;
		friend class PanelCtrl;
		friend class SliderCtrl;

	public:
		using StylesHolder::setDefaultFont;
		/**@name General */
		//@{

		/** Constructor
		*\param[in] engine
		*	The engine
		*/
		C3D_API explicit ControlsManager( Engine & engine );

		//@}
		/**@name Style management */
		//@{

		/** Creates a theme if it doesn't exist, returns the existing one if it exists
		*\param[in] name
		*	The theme name
		*\return
		*	The theme.
		*/
		C3D_API ThemeRPtr createTheme( castor::String const & name
			, Scene * scene );

		/** \return
		*	The theme for given name (\p nullptr if it doesn't exist).
		*/
		C3D_API ThemeRPtr getTheme( castor::String const & name )const;
		
		auto & getThemes()const
		{
			return m_themes;
		}

		/** \return
		*	The button style with given name (\p nullptr if it doesn't exist).
		*/
		C3D_API ButtonStyleRPtr getButtonStyle( castor::String const & name )const override;

		/** \return
		*	The combo box style with given name (\p nullptr if it doesn't exist).
		*/
		C3D_API ComboBoxStyleRPtr getComboBoxStyle( castor::String const & name )const override;

		/** \return
		*	The edit style with given name (\p nullptr if it doesn't exist).
		*/
		C3D_API EditStyleRPtr getEditStyle( castor::String const & name )const override;

		/** \return
		*	The expandable panel style with given name (\p nullptr if it doesn't exist).
		*/
		C3D_API ExpandablePanelStyleRPtr getExpandablePanelStyle( castor::String const & name )const override;

		/** \return
		*	The frame style with given name (\p nullptr if it doesn't exist).
		*/
		C3D_API FrameStyleRPtr getFrameStyle( castor::String const & name )const override;

		/** \return
		*	The list box style with given name (\p nullptr if it doesn't exist).
		*/
		C3D_API ListBoxStyleRPtr getListBoxStyle( castor::String const & name )const override;

		/** \return
		*	The panel style with given name (\p nullptr if it doesn't exist).
		*/
		C3D_API PanelStyleRPtr getPanelStyle( castor::String const & name )const override;

		/** \return
		*	The slider style with given name (\p nullptr if it doesn't exist).
		*/
		C3D_API SliderStyleRPtr getSliderStyle( castor::String const & name )const override;

		/** \return
		*	The static style with given name (\p nullptr if it doesn't exist).
		*/
		C3D_API StaticStyleRPtr getStaticStyle( castor::String const & name )const override;

		//@}
		/**@name Controls management */
		//@{

		/** Creates the given control's overlays and binds its callbacks to appropriate events
		*\param[in] control
		*	The control
		*/
		C3D_API void create( ControlSPtr control );

		/** Destroys the given control's overlays and unbinds its callbacks from appropriate events.
		*\param[in] control
		*	The control.
		*/
		C3D_API void destroy( ControlSPtr control );

		/** Adds a control that can an event target
		*\param[in] control
		*	The control
		*/
		C3D_API void addControl( ControlSPtr control );

		/** Removes a control
		*\param[in] id
		*	The control ID
		*/
		C3D_API void removeControl( ControlID id );

		/** Retrieves a control.
		*\param[in] id
		*	The control ID.
		*\return
		*	The control.
		*/
		C3D_API ControlSPtr getControl( ControlID id )const;

		/** Retrieves a control.
		*\param[in] name
		*	The control name.
		*\return
		*	The control.
		*/
		C3D_API ControlSPtr findControl( castor::String const & name )const;

		/** \return
		*	The root controls.
		*/
		C3D_API std::vector< Control * > getRootControls()const;

		//@}

		C3D_API static castor::AttributeParsers createParsers( castor3d::Engine & engine );
		C3D_API static castor::StrUInt32Map createSections();
		C3D_API static void * createContext( castor::FileParserContext & context );

	private:
		/** Sets the control that is currently moved (only one at a time is allowed).
		*/
		bool setMovedControl( ControlRPtr control
			, MouseEvent const & event );

		/** Sets the control that is currently resized (only one at a time is allowed).
		*/
		bool setResizedControl( ControlRPtr control
			, MouseEvent const & event );

		/** Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( ButtonCtrl & control );

		/** Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( ComboBoxCtrl & control );

		/** Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( EditCtrl & control );

		/** Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( ExpandablePanelCtrl & control );

		/** Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( ListBoxCtrl & control );

		/** Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( SliderCtrl & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ButtonCtrl & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ComboBoxCtrl & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( EditCtrl & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ExpandablePanelCtrl & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ListBoxCtrl & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( SliderCtrl & control );

	private:
		/** \copydoc		UserInputListener::doInitialise
		 */
		bool doInitialise()override;

		/** \copydoc		UserInputListener::doCleanup
		 */
		void doCleanup()override;

		/** Retrieves the first handler which can be targeted by mouse, at its position
		*\param[in] position
		*	The mouse position
		*/
		EventHandler * doGetMouseTargetableHandler( castor::Position const & position )const override;

		/** Updates the z-index ordered controls array
		*/
		void doUpdate();

		/** \return
		*	The controls by z-index.
		*/
		std::vector< Control * > doGetControlsByZIndex()const;

		/** Sets the controls by z-index.
		*/
		void doSetControlsByZIndex( std::vector< Control * > v );

		/** \return
		*	The controls by ID.
		*/
		std::map< ControlID, ControlWPtr > doGetControlsById()const;

		/** Marks the manager as to be updated.
		*/
		void doMarkDirty();

	public:
		C3D_API static castor::String Name;

	private:
		mutable std::mutex m_mutexControlsById;
		std::map< ControlID, ControlWPtr > m_controlsById;
		std::vector< Control * > m_rootControls;
		mutable std::mutex m_mutexControlsByZIndex;
		mutable std::vector< Control * > m_controlsByZIndex;
		mutable std::atomic< CpuFrameEvent * > m_event{};
		std::map< castor::String, ThemeUPtr > m_themes;
		std::map< Control const *, OnButtonEventConnection > m_onButtonClicks;
		std::map< Control const *, OnComboEventConnection > m_onComboSelects;
		std::map< Control const *, OnEditEventConnection > m_onEditUpdates;
		std::map< Control const *, OnListEventConnection > m_onListSelects;
		std::map< Control const *, OnSliderEventConnection > m_onSliderTracks;
		std::map< Control const *, OnSliderEventConnection > m_onSliderReleases;
		std::map< Control const *, OnExpandablePanelEventConnection > m_onPanelExpands;
		std::map< Control const *, OnExpandablePanelEventConnection > m_onPanelRetracts;
		ControlRPtr m_movedControl{};
		ControlRPtr m_resizedControl{};
	};
}

#endif
