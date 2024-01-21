#include "Castor3D/Gui/Layout/LayoutBox.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlLayoutControl.hpp"

namespace castor3d
{
	namespace boxlayt
	{
		static uint32_t getBorderDim( castor::Point4ui const & borderSize
			, uint32_t component )
		{
			return borderSize[component] + borderSize[component + 2u];
		}
	}

	LayoutBox::LayoutBox( ControlsManager & manager )
		: Layout{ "c3d.layout.box", manager }
	{
	}

	LayoutBox::LayoutBox( LayoutControl & container )
		: Layout{ "c3d.layout.box", container }
	{
	}

	void LayoutBox::doUpdate()
	{
		uint32_t advanceComp;
		uint32_t fixedComp;

		if ( m_horizontal )
		{
			advanceComp = 0u;
			fixedComp = 1u;
		}
		else
		{
			advanceComp = 1u;
			fixedComp = 0u;
		}

		auto scrollPosition = m_container
			? m_container->getScrollPosition()
			: castor::Position{};
		uint32_t controlsSep{ doComputeSeparator( advanceComp ) };
		auto borders = m_container
			? m_container->getBorderSize()
			: castor::Point4ui{};
		auto containerSize = m_container
			? m_container->getSize()
			: m_manager->getSize();
		auto advance = int32_t( borders[advanceComp] );
		uint32_t containerFixedCompLimit = containerSize[fixedComp] - boxlayt::getBorderDim( borders, fixedComp );

		for ( auto const & item : m_items )
		{
			if ( item.isSpacer() )
			{
				if ( item.spacer()->isDynamic() )
				{
					advance += int32_t( controlsSep );
				}
				else
				{
					advance += int32_t( item.spacer()->getSize() );
				}
			}
			else
			{
				auto control = item.control();
				uint32_t controlSizeAdvance = control->getSize()[advanceComp];

				if ( control->isVisible() )
				{
					auto [fixed, controlSizeFixed] = doGetFixedPosSize( item, containerFixedCompLimit, fixedComp );
					fixed += int32_t( borders[fixedComp] );

					castor::Position position;
					position[fixedComp] = fixed + int32_t( item.padding( fixedComp ) ) + scrollPosition.x();
					position[advanceComp] = advance + int32_t( item.padding( advanceComp ) ) + scrollPosition.y();
					control->setPosition( position );

					castor::Size size;
					size[fixedComp] = controlSizeFixed;
					size[advanceComp] = controlSizeAdvance;
					control->setSize( size );

					advance += int32_t( controlSizeAdvance )
						+ int32_t( item.paddingSize( advanceComp ) );
				}
				else if ( item.reserveSpaceIfHidden() )
				{
					advance += int32_t( controlSizeAdvance )
						+ int32_t( item.paddingSize( advanceComp ) );
				}
			}
		}
	}

	uint32_t LayoutBox::doComputeSeparator( uint32_t component )const
	{
		auto borders = m_container
			? m_container->getBorderSize()
			: castor::Point4ui{};
		auto containerSize = m_container
			? m_container->getSize()
			: m_manager->getSize();
		uint32_t count{};
		uint32_t maxComponentValue = containerSize[component] - boxlayt::getBorderDim( borders, component );
		uint32_t accum{ std::accumulate( m_items.begin()
			, m_items.end()
			, uint32_t{}
			, [component, &count]( uint32_t acc, Layout::Item const & lookup )
			{
				if ( lookup.isSpacer() )
				{
					if ( !lookup.spacer()->isDynamic() )
					{
						acc = acc + lookup.spacer()->getSize();
					}
					else
					{
						++count;
					}
				}
				else
				{
					if ( lookup.control()->isVisible()
						|| lookup.reserveSpaceIfHidden() )
					{
						acc = acc + lookup.getPaddedSize()[component];
					}
				}

				return acc;
			} ) };

		return ( accum > maxComponentValue || !count )
			? 0u
			: ( maxComponentValue - accum ) / count;
	}

	std::pair< int32_t, uint32_t > LayoutBox::doGetFixedPosSize( Item const & item
		, uint32_t limit
		, uint32_t component )const
	{
		limit -= item.paddingSize( component );
		auto control = item.control();

		if ( !control )
		{
			return { 0, limit };
		}

		auto borderPosition = control->getBorderPosition();
		auto borderBegin = control->getBorderSize()[component];
		auto borderEnd = control->getBorderSize()[component + 2u];

		if ( borderPosition == BorderPosition::eMiddle )
		{
			borderBegin /= 2;
			borderEnd /= 2;
		}
		else if ( borderPosition == BorderPosition::eInternal )
		{
			borderBegin = 0;
			borderEnd = 0;
		}

		auto borderSize = borderEnd + borderBegin;

		if ( item.expand() )
		{
			return { borderBegin
				, std::max( 0, int32_t( limit ) - int32_t( borderSize ) ) };
		}

		auto controlSize = control->getSize()[component];
		auto controlPosition = int32_t( borderBegin );

		if ( component == 0u )
		{
			if ( item.hAlign() != HAlign::eLeft )
			{
				controlPosition = int32_t( limit ) - int32_t( controlSize );

				if ( item.hAlign() == HAlign::eCenter )
				{
					controlPosition /= 2;
				}
			}
		}
		else if ( item.vAlign() != VAlign::eTop )
		{
			controlPosition = int32_t( limit ) - int32_t( controlSize );

			if ( item.vAlign() == VAlign::eCenter )
			{
				controlPosition /= 2;
			}
		}

		return { controlPosition, controlSize };
	}
}
