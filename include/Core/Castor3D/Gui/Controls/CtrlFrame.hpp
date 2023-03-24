/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrameCtrl_H___
#define ___C3D_FrameCtrl_H___

#include "Castor3D/Gui/Controls/CtrlPanel.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"
#include "Castor3D/Gui/Theme/StyleFrame.hpp"

namespace castor3d
{
	class FrameCtrl
		: public Control
	{
	public:
		/** Constructor
		*\param[in]	name	The control name
		*\param[in]	style	The control style
		*\param[in]	parent	The parent control, if any
		*\param[in]	id		The control ID
		*/
		C3D_API FrameCtrl( SceneRPtr scene
			, castor::String const & name
			, FrameStyle * style
			, ControlRPtr parent );

		/** Constructor
		*\param[in]	name		The control name
		*\param[in]	style		The control style
		*\param[in]	parent		The parent control, if any
		*\param[in]	values		The list value
		*\param[in]	selected	The selected value
		*\param[in]	id			The control ID
		*\param[in]	position	The position
		*\param[in]	size		The size
		*\param[in]	flags		The configuration flags
		*\param[in]	visible		Initial visibility status
		*/
		C3D_API FrameCtrl( SceneRPtr scene
			, castor::String const & name
			, FrameStyle * style
			, ControlRPtr parent
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t headerHeight
			, bool expanded
			, ControlFlagType flags = 0
			, bool visible = true );

		/** \return	The frame style.
		*/
		FrameStyle const & getStyle()const noexcept
		{
			return static_cast< FrameStyle const & >( getBaseStyle() );
		}

		castor::U32String const & getHeaderCaption()const noexcept
		{
			return m_header->getCaption();
		}

		HAlign getHeaderHAlign()const noexcept
		{
			return m_header->getHAlign();
		}

		VAlign getHeaderVAlign()const noexcept
		{
			return m_header->getVAlign();
		}

		castor::Size const & getMinSize()const noexcept
		{
			return m_minSize;
		}

		void setMinSize( castor::Size const & size )noexcept
		{
			m_minSize = size;
		}

		void setHeaderCaption( castor::U32String const & v )noexcept
		{
			setCaption( v );
		}

		void setHeaderHAlign( HAlign v)noexcept
		{
			m_header->setHAlign( v );
		}

		void setHeaderVAlign( VAlign v )noexcept
		{
			m_header->setVAlign( v );
		}

		auto getContent()const noexcept
		{
			return m_content;
		}

		C3D_API static ControlType constexpr Type{ ControlType::eFrame };

	private:
		auto getHeader()const noexcept
		{
			return m_header;
		}

		FrameStyle & getStyle()
		{
			return static_cast< FrameStyle & >( getBaseStyle() );
		}

		/** @copydoc Control::doCreate
		*/
		void doCreate()override;

		/** @copydoc Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc Control::doUpdatePosition
		*/
		castor::Position doUpdatePosition( castor::Position const & value )const noexcept override;

		/** @copydoc Control::doUpdateSize
		*/
		castor::Size doUpdateSize( castor::Size const & value )const noexcept override;

		/** @copydoc Control::doSetPosition
		*/
		void doSetPosition( castor::Position const & value )override;

		/** @copydoc Control::doSetSize
		*/
		void doSetSize( castor::Size const & value )override;

		/** @copydoc Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc Control::doCatchesMouseEvents
		*/
		bool doCatchesMouseEvents()const override;

		/** @copydoc Control::doSetVisible
		*/
		void doSetVisible( bool visible )override;

		/** @copydoc Control::doSetCaption
		*/
		void doSetCaption( castor::U32String const & caption )override
		{
			m_header->setCaption( caption );
		}

		/** Update position of children.
		*/
		void doUpdatePositions();

		/** Update size of children.
		*/
		void doUpdateSizes();

	private:
		uint32_t m_headerHeight;
		StaticCtrlSPtr m_header;
		PanelCtrlSPtr m_content;
		castor::Size m_minSize;
	};
}

#endif
