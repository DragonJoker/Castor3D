/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ControlsManager_H___
#define ___C3D_ControlsManager_H___

#include "Castor3D/Gui/GuiModule.hpp"
#include "Castor3D/Gui/Layout/LayoutItemFlags.hpp"
#include "Castor3D/Gui/Theme/Theme.hpp"

#include "Castor3D/Event/UserInput/UserInputListener.hpp"
#include "Castor3D/Event/UserInput/EventHandler.hpp"

#include <CastorUtils/Graphics/Position.hpp>

#include <stack>

namespace c3d
{
	class ControlsManager
		: public UserInputListener
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
		friend class ScrollBarCtrl;
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

		/** Sets the size.
		 *\param[in]	value	The new value.
		 */
		C3D_API void setSize( Size const & value );

		/** Sets the layout for the elements contained in this one.
		 *\param[in]	layout	The new value
		 */
		C3D_API void setLayout( LayoutUPtr layout );

		Size const & getSize()const noexcept
		{
			return m_size;
		}

		//@}
		/**@name Style management */
		//@{

		/** Creates a theme if it doesn't exist, returns the existing one if it exists
		*\param[in] name
		*	The theme name
		*\param[in] scene
		*	The parent scene
		*\return
		*	The theme.
		*/
		C3D_API ThemeRPtr createTheme( String const & name
			, Scene * scene );

		/** \return
		*	The theme for given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API ThemeRPtr getTheme( String const & name )const;
		
		auto & getThemes()const
		{
			return m_themes;
		}

		/** \return
		*	The button style with given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API ButtonStyleRPtr getButtonStyle( String const & name )const override;

		/** \return
		*	The combo box style with given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API ComboBoxStyleRPtr getComboBoxStyle( String const & name )const override;

		/** \return
		*	The edit style with given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API EditStyleRPtr getEditStyle( String const & name )const override;

		/** \return
		*	The expandable panel style with given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API ExpandablePanelStyleRPtr getExpandablePanelStyle( String const & name )const override;

		/** \return
		*	The frame style with given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API FrameStyleRPtr getFrameStyle( String const & name )const override;

		/** \return
		*	The list box style with given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API ListBoxStyleRPtr getListBoxStyle( String const & name )const override;

		/** \return
		*	The panel style with given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API PanelStyleRPtr getPanelStyle( String const & name )const override;

		/** \return
		*	The progress style with given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API ProgressStyleRPtr getProgressStyle( String const & name )const override;

		/** \return
		*	The scrollbar style with given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API ScrollBarStyleRPtr getScrollBarStyle( String const & name )const override;

		/** \return
		*	The slider style with given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API SliderStyleRPtr getSliderStyle( String const & name )const override;

		/** \return
		*	The static style with given name (\p nullptr if it doesn't exist).
		*\param[in] name
		*	The name
		*/
		C3D_API StaticStyleRPtr getStaticStyle( String const & name )const override;

		//@}
		/**@name Controls management */
		//@{

		/** Adds a control that still needs to be created.
		*\param[in] control
		*	The control
		*/
		C3D_API ControlRPtr registerControl( ControlUPtr control );

		template< typename ControlT >
		ControlT * registerControlT( UniquePtr< ControlT > control )
		{
			return &static_cast< ControlT & >( *registerControl( ptrRefCast< Control >( control ) ) );
		}

		/** Removes a control that has been destroyed.
		*\param[in] control
		*	The control
		*/
		C3D_API void unregisterControl( Control const & control );

		/** Creates the given control's overlays and binds its callbacks to appropriate events
		*\param[in] control
		*	The control
		*/
		C3D_API void create( ControlRPtr control );

		/** Destroys the given control's overlays and unbinds its callbacks from appropriate events.
		*\param[in] control
		*	The control.
		*/
		C3D_API void destroy( ControlRPtr control );

		/** Destroys the controls related to given scene.
		*\param[in] scene
		*	The controls scene.
		*/
		C3D_API void destroyControls( Scene const & scene );

		/** Adds a control that can an event target
		*\param[in] control
		*	The control
		*/
		C3D_API void addControl( ControlRPtr control );

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
		C3D_API ControlRPtr getControl( ControlID id )const;

		/** Retrieves a control.
		*\param[in] name
		*	The control name.
		*\param[in] scene
		*	The control scene.
		*\return
		*	The control.
		*/
		C3D_API ControlRPtr findControl( String const & name, SceneRPtr scene )const;

		/** \return
		*	The root controls.
		*/
		C3D_API Vector< ControlRPtr > getRootControls()const;

		//@}

		C3D_API static AttributeParsers createParsers();
		C3D_API static StrSectionIdMap createSections();

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
		void connectEvents( ScrollBarCtrl & control );

		/** Connects the manager to given control's events.
		*\param[in] control
		*	The control.
		*/
		void connectEvents( SliderCtrl & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ButtonCtrl const & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ComboBoxCtrl const & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( EditCtrl const & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ExpandablePanelCtrl const & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ListBoxCtrl const & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( ScrollBarCtrl const & control );

		/** Disconnects the manager from given control's events.
		*\param[in] control
		*	The control.
		*/
		void disconnectEvents( SliderCtrl const & control );

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
		EventHandlerRPtr doGetMouseTargetableHandler( Position const & position )const override;

		/** Updates the z-index ordered controls array
		*/
		void doUpdate();

		/** \return
		*	The controls by z-index.
		*/
		Vector< ControlRPtr > doGetControlsByZIndex()const;

		/** Sets the controls by z-index.
		*/
		void doSetControlsByZIndex( Vector< ControlRPtr > v );

		/** \return
		*	The controls by ID.
		*/
		Map< ControlID, ControlRPtr > doGetControlsById()const;

		/** Marks the manager as to be updated.
		*/
		void doMarkDirty();

		void doRemoveControlNL( ControlID id, bool rootControl = true );
		void doDestroyControlsRec( ControlRPtr control );

	public:
		C3D_API static String Name;

	private:
		mutable Mutex m_mutexControlsById;
		Map< ControlID, ControlRPtr > m_controlsById;
		Vector< ControlRPtr > m_rootControls;
		mutable Mutex m_mutexControlsByZIndex;
		mutable Vector< ControlRPtr > m_controlsByZIndex;
		mutable std::atomic< CpuFrameEvent * > m_event{};
		StringMap< ThemeUPtr > m_themes;
		Map< Control const *, OnButtonEventConnection > m_onButtonClicks;
		Map< Control const *, OnComboEventConnection > m_onComboSelects;
		Map< Control const *, OnEditEventConnection > m_onEditUpdates;
		Map< Control const *, OnListEventConnection > m_onListSelects;
		Map< Control const *, OnScrollBarEventConnection > m_onScrollTracks;
		Map< Control const *, OnScrollBarEventConnection > m_onScrollReleases;
		Map< Control const *, OnSliderEventConnection > m_onSliderTracks;
		Map< Control const *, OnSliderEventConnection > m_onSliderReleases;
		Map< Control const *, OnExpandablePanelEventConnection > m_onPanelExpands;
		Map< Control const *, OnExpandablePanelEventConnection > m_onPanelRetracts;
		ControlRPtr m_movedControl{};
		ControlRPtr m_resizedControl{};
		Size m_size;
		LayoutUPtr m_layout;
	};

	struct SceneContext;

	struct GuiContext
	{
		RootContext * root{};
		SceneContext * scene{};
		ControlsManager * controls{};
		Stack< ControlRPtr > parents{};
		Stack< ControlStyleRPtr > styles{};
		Stack< StylesHolderRPtr > stylesHolder{};
		String controlName{};
		ButtonCtrlRPtr button{};
		ComboBoxCtrlRPtr combo{};
		EditCtrlRPtr edit{};
		ListBoxCtrlRPtr listbox{};
		SliderCtrlRPtr slider{};
		StaticCtrlRPtr staticTxt{};
		PanelCtrlRPtr panel{};
		ProgressCtrlRPtr progress{};
		ExpandablePanelCtrlRPtr expandablePanel{};
		FrameCtrlRPtr frame{};
		ScrollableCtrlRPtr scrollable{};
		ThemeRPtr theme{};
		ButtonStyleRPtr buttonStyle{};
		ComboBoxStyleRPtr comboStyle{};
		EditStyleRPtr editStyle{};
		ListBoxStyleRPtr listboxStyle{};
		SliderStyleRPtr sliderStyle{};
		StaticStyleRPtr staticStyle{};
		PanelStyleRPtr panelStyle{};
		ProgressStyleRPtr progressStyle{};
		ExpandablePanelStyleRPtr expandablePanelStyle{};
		FrameStyleRPtr frameStyle{};
		ScrollBarStyleRPtr scrollBarStyle{};
		ScrollableStyleRPtr scrollableStyle{};
		LayoutUPtr layout{};
		LayoutItemFlags layoutCtrlFlags{};

		C3D_API ControlRPtr getTopControl()const;
		C3D_API void popControl();
		C3D_API void pushStylesHolder( StylesHolder * style );
		C3D_API void popStylesHolder( StylesHolder const * style );
		C3D_API ControlStyleRPtr getTopStyle()const;
		C3D_API void popStyle();

		template< typename StyleT >
		void pushStyle( StyleT * style
			, StyleT *& result )
		{
			styles.push( style );
			result = style;

			if constexpr ( std::is_base_of_v< StylesHolder, StyleT > )
			{
				pushStylesHolder( style );
			}

			if constexpr ( std::is_base_of_v< ScrollableStyle, StyleT > )
			{
				scrollableStyle = style;
			}
		}
	};

	C3D_API String getPrefix( GuiContext const & context );
	C3D_API Engine * getEngine( GuiContext const & context );
}

#endif
