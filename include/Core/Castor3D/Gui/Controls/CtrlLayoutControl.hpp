/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LayoutControl_H___
#define ___C3D_LayoutControl_H___

#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Controls/CtrlScrollable.hpp"

namespace c3d
{
	C3D_API bool isLayoutControl( ControlType type );
	C3D_API bool isLayoutControl( Control const & control );

	class LayoutControl
		: public Control
		, public ScrollableCtrl
	{
	public:
		/** Constructor.
		 *\param[in]	scene				The parent scene (nullptr for global).
		 *\param[in]	name				The control name.
		 *\param[in]	type				The type.
		 *\param[in]	controlStyle		The control's style.
		 *\param[in]	scrollableStyle		The scrollable's style.
		 *\param[in]	parent				The parent control, if any.
		 *\param[in]	position			The position.
		 *\param[in]	size				The size.
		 *\param[in]	flags				The configuration flags
		 *\param[in]	visible				Initial visibility status.
		 */
		C3D_API LayoutControl( ControlType type
			, SceneRPtr scene
			, String const & name
			, ControlStyleRPtr controlStyle
			, ScrollableStyleRPtr scrollableStyle
			, ControlRPtr parent
			, Position const & position
			, Size const & size
			, ControlFlagType flags = 0
			, bool visible = true );

		/** Sets the layout for the elements contained in this one.
		 *\param[in]	layout	The new value
		 */
		C3D_API void setLayout( LayoutUPtr layout );

		auto getLayout()const
		{
			return m_layout.get();
		}

	private:
		/** @copydoc Control::doCreate
		*/
		void doCreate()override;

		/** @copydoc Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc Control::doAddChild
		 */
		void doAddChild( ControlRPtr control )override;

		/** @copydoc Control::doRemoveChild
		 */
		void doRemoveChild( ControlRPtr control )override;

		/** @copydoc Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

		/** @copydoc Control::doUpdateZIndex
		*/
		void doUpdateZIndex( uint32_t & index )override;

		/** @copydoc Control::doAdjustZIndex
		*/
		void doAdjustZIndex( uint32_t offset )override;

		/** @copydoc Control::doUpdateClientRect
		*/
		Point4ui doUpdateClientRect( Point4ui const & clientRect )final;

		/** Sets the background borders size.
		 *\param[in]	value		The new value.
		 */
		void doSetBorderSize( Point4ui const & value )final;

		/** Sets the position
		*\param[in]	value		The new value
		*/
		void doSetPosition( Position const & value )final;

		/** Sets the size
		*\param[in]	value	The new value
		*/
		void doSetSize( Size const & value )final;

		/** Sets the caption.
		*\param[in]	caption	The new value
		*/
		void doSetCaption( U32String const & caption )final;

		/** Sets the visibility
		 *\remarks		Used for derived control specific behavious
		 *\param[in]	visible		The new value
		 */
		void doSetVisible( bool visible )final;

		/** @copydoc Control::doUpdateClientRect
		*/
		virtual Point4ui doSubUpdateClientRect( Point4ui const & clientRect )
		{
			return clientRect;
		}

		/** Sets the background borders size.
		 *\param[in]	value		The new value.
		 */
		virtual void doSubSetBorderSize( Point4ui const & value ) {}
		/** Sets the position
		*\param[in]	value		The new value
		*/
		virtual void doSubSetPosition( Position const & value ) {}

		/** Sets the size
		*\param[in]	value	The new value
		*/
		virtual void doSubSetSize( Size const & value ) {}

		/** Sets the caption.
		*\param[in]	caption	The new value
		*/
		virtual void doSubSetCaption( U32String const & caption ) {}

		/** Sets the visibility
		 *\remarks		Used for derived control specific behavious
		 *\param[in]	visible		The new value
		 */
		virtual void doSubSetVisible( bool visible ) {}

	private:
		LayoutUPtr m_layout;
	};
}

#endif
