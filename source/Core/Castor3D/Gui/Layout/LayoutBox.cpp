#include "Castor3D/Gui/Layout/LayoutBox.hpp"

#include "Castor3D/Gui/Controls/CtrlLayoutControl.hpp"

namespace castor3d
{
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

		uint32_t controlsSep{ doComputeSeparator( advanceComp ) };
		auto & borders = m_container.getBackgroundBorders();
		int32_t advance = int32_t( borders[advanceComp] );
		uint32_t containerFixedCompLimit = m_container.getSize()[fixedComp] - ( borders[fixedComp] + borders[fixedComp + 2u] );

		for ( auto & item : m_items )
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
				auto controlSize = control->getSize();
				uint32_t controlSizeAdvance = controlSize[advanceComp];

				if ( control->isVisible() )
				{
					auto [fixed, controlSizeFixed] = doGetPosSize( item, containerFixedCompLimit, fixedComp );
					fixed += int32_t( borders[fixedComp] );

					castor::Position position;
					position[fixedComp] = fixed;
					position[advanceComp] = advance;
					control->setPosition( position );

					castor::Size size;
					size[fixedComp] = controlSizeFixed;
					size[advanceComp] = controlSizeAdvance;
					control->setSize( size );

					advance += int32_t( controlSizeAdvance );
				}
				else if ( item.flags().m_reserveSpaceIfHidden )
				{
					advance += int32_t( controlSizeAdvance );
				}
			}
		}
	}

	uint32_t LayoutBox::doComputeSeparator( uint32_t component )
	{
		auto & borders = m_container.getBackgroundBorders();
		uint32_t count{};
		uint32_t maxComponentValue = m_container.getSize()[component] - ( borders[component] + borders[component + 2u] );
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
						|| lookup.flags().m_reserveSpaceIfHidden )
					{
						acc = acc + lookup.control()->getSize()[component];
					}
				}

				return acc;
			} ) };

		return ( accum > maxComponentValue || !count )
			? 0u
			: ( maxComponentValue - accum ) / count;
	}

	std::pair< int32_t, uint32_t > LayoutBox::doGetPosSize( Item const & item
		, uint32_t limit
		, uint32_t component )
	{
		if ( item.flags().m_expand )
		{
			return { 0u, limit };
		}

		auto & controlSize = item.control()->getSize();
		auto sizeFixed = controlSize[component];
		int32_t posFixed{};

		if ( component == 0u )
		{
			if ( item.flags().m_hAlign != HAlign::eLeft )
			{
				posFixed = int32_t( limit ) - int32_t( sizeFixed );

				if ( item.flags().m_hAlign == HAlign::eCenter )
				{
					posFixed /= 2;
				}
			}
		}
		else if ( item.flags().m_vAlign != VAlign::eTop )
		{
			posFixed = int32_t( limit ) - int32_t( sizeFixed );

			if ( item.flags().m_vAlign == VAlign::eCenter )
			{
				posFixed /= 2;
			}
		}

		return { posFixed, sizeFixed };
	}
}
