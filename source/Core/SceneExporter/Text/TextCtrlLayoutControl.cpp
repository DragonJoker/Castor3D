#include "TextCtrlLayoutControl.hpp"

#include "TextCtrlButton.hpp"
#include "TextCtrlControl.hpp"
#include "TextCtrlComboBox.hpp"
#include "TextCtrlEdit.hpp"
#include "TextCtrlExpandablePanel.hpp"
#include "TextCtrlFrame.hpp"
#include "TextCtrlListBox.hpp"
#include "TextCtrlPanel.hpp"
#include "TextCtrlSlider.hpp"
#include "TextCtrlStatic.hpp"
#include "TextLayoutBox.hpp"

#include <Castor3D/Gui/Controls/CtrlButton.hpp>
#include <Castor3D/Gui/Controls/CtrlComboBox.hpp>
#include <Castor3D/Gui/Controls/CtrlEdit.hpp>
#include <Castor3D/Gui/Controls/CtrlExpandablePanel.hpp>
#include <Castor3D/Gui/Controls/CtrlFrame.hpp>
#include <Castor3D/Gui/Controls/CtrlListBox.hpp>
#include <Castor3D/Gui/Controls/CtrlPanel.hpp>
#include <Castor3D/Gui/Controls/CtrlSlider.hpp>
#include <Castor3D/Gui/Controls/CtrlStatic.hpp>
#include <Castor3D/Gui/Layout/LayoutBox.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< LayoutControl >::TextWriter( String const & tabs
		, bool customStyle )
		: TextWriterT< LayoutControl >{ tabs }
		, m_customStyle{ customStyle }
	{
	}

	bool TextWriter< LayoutControl >::operator()( LayoutControl const & control
		, StringStream & file )
	{
		auto result = TextWriter< Control >{ tabs(), m_customStyle }( control, file );

		for ( auto ctrl : control.getChildren() )
		{
			if ( result )
			{
				result = false;

				if ( ctrl )
				{
					result = writeControl( *this, *ctrl, file, control.getName() + cuT( "/" ) );
				}
			}
		}

		if ( result )
		{
			if ( auto layout = control.getLayout() )
			{
				if ( layout->getName() == cuT( "c3d.layout.box" ) )
				{
					if ( auto block{ beginBlock( file, cuT( "box_layout" ) ) } )
					{
						result = writeSub( file, static_cast< LayoutBox const & >( *control.getLayout() ) );
					}
				}
			}
		}

		return result;
	}

	bool writeControl( TextWriterBase & writer
		, Control const & control
		, castor::StringStream & file
		, String const & prefix )
	{
		bool result{};
		auto name = control.getName();

		if ( !prefix.empty()
			&& name.find( prefix ) == 0 )
		{
			name = name.substr( prefix.size() );
		}

		switch ( control.getType() )
		{
		case ControlType::ePanel:
			if ( auto block{ writer.beginBlock( file, cuT( "panel" ), name ) } )
			{
				result = writer.writeSub( file, static_cast< PanelCtrl const & >( control ) );
			}
			break;
		case ControlType::eStatic:
			if ( auto block{ writer.beginBlock( file, cuT( "static" ), name ) } )
			{
				result = writer.writeSub( file, static_cast< StaticCtrl const & >( control ) );
			}
			break;
		case ControlType::eEdit:
			if ( auto block{ writer.beginBlock( file, cuT( "edit" ), name ) } )
			{
				result = writer.writeSub( file, static_cast< EditCtrl const & >( control ) );
			}
			break;
		case ControlType::eSlider:
			if ( auto block{ writer.beginBlock( file, cuT( "slider" ), name ) } )
			{
				result = writer.writeSub( file, static_cast< SliderCtrl const & >( control ) );
			}
			break;
		case ControlType::eComboBox:
			if ( auto block{ writer.beginBlock( file, cuT( "combobox" ), name ) } )
			{
				result = writer.writeSub( file, static_cast< ComboBoxCtrl const & >( control ) );
			}
			break;
		case ControlType::eListBox:
			if ( auto block{ writer.beginBlock( file, cuT( "listbox" ), name ) } )
			{
				result = writer.writeSub( file, static_cast< ListBoxCtrl const & >( control ) );
			}
			break;
		case ControlType::eButton:
			if ( auto block{ writer.beginBlock( file, cuT( "button" ), name ) } )
			{
				result = writer.writeSub( file, static_cast< ButtonCtrl const & >( control ) );
			}
			break;
		case ControlType::eExpandablePanel:
			if ( auto block{ writer.beginBlock( file, cuT( "expandable_panel" ), name ) } )
			{
				result = writer.writeSub( file, static_cast< ExpandablePanelCtrl const & >( control ) );
			}
			break;
		case ControlType::eFrame:
			if ( auto block{ writer.beginBlock( file, cuT( "frame" ), name ) } )
			{
				result = writer.writeSub( file, static_cast< FrameCtrl const & >( control ) );
			}
			break;
		default:
			log::error << "Unsupported Control type" << std::endl;
			break;
		}

		return result;
	}
}
