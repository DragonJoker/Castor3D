/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LayoutControl_H___
#define ___C3D_LayoutControl_H___

#include "Castor3D/Gui/Controls/CtrlControl.hpp"

namespace castor3d
{
	class LayoutControl
		: public Control
	{
	public:
		/** Constructor.
		 *\param[in]	name		The control name.
		 *\param[in]	type		The type.
		 *\param[in]	style		The control's style.
		 *\param[in]	parent		The parent control, if any.
		 *\param[in]	id			The control ID.
		 *\param[in]	position	The position.
		 *\param[in]	size		The size.
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status.
		 */
		C3D_API LayoutControl( ControlType type
			, SceneRPtr scene
			, castor::String const & name
			, ControlStyleRPtr style
			, ControlRPtr parent
			, castor::Position const & position
			, castor::Size const & size
			, uint64_t flags = 0
			, bool visible = true );

		/** Sets the layout for the elements contained in this one.
		 *\param[in]	value	The new value
		 */
		C3D_API void setLayout( LayoutUPtr layout );

	private:
		/** Sets the background borders size.
		 *\param[in]	value		The new value.
		 */
		void doSetBackgroundBorders( castor::Point4ui const & value )final override;
		/** Sets the position
		*\param[in]	value		The new value
		*/
		void doSetPosition( castor::Position const & value )final override;

		/** Sets the size
		*\param[in]	value	The new value
		*/
		void doSetSize( castor::Size const & value )final override;

		/** Sets the caption.
		*\param[in]	caption	The new value
		*/
		void doSetCaption( castor::String const & caption )final override;

		/** Sets the visibility
		 *\remarks		Used for derived control specific behavious
		 *\param[in]	value		The new value
		 */
		void doSetVisible( bool visible )final override;

		/** Sets the background borders size.
		 *\param[in]	value		The new value.
		 */
		virtual void doSubSetBackgroundBorders( castor::Point4ui const & value ) {}
		/** Sets the position
		*\param[in]	value		The new value
		*/
		virtual void doSubSetPosition( castor::Position const & value ) {}

		/** Sets the size
		*\param[in]	value	The new value
		*/
		virtual void doSubSetSize( castor::Size const & value ) {}

		/** Sets the caption.
		*\param[in]	caption	The new value
		*/
		virtual void doSubSetCaption( castor::String const & caption ) {}

		/** Sets the visibility
		 *\remarks		Used for derived control specific behavious
		 *\param[in]	value		The new value
		 */
		virtual void doSubSetVisible( bool visible ) {}

	private:
		LayoutUPtr m_layout;
	};
}

#endif
