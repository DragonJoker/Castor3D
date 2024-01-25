/*
See LICENSE file in root folder
*/
namespace castor3d
{
	template< typename StyleT >
	StyleT * StylesHolder::createControlStyle( castor::String name
		, Scene * scene
		, castor::StringMap< castor::UniquePtr< StyleT > > & controls )
	{
		auto fullName{ name };

		if ( !m_holderName.empty() )
		{
			fullName = m_holderName + cuT( "/" ) + fullName;
		}

		auto it = controls.find( name );

		if ( it != controls.end() )
		{
			log::warn << cuT( "StylesHolder: Duplicate style [" ) << name << cuT( "]" ) << std::endl;
		}
		else
		{
			if constexpr ( std::is_same_v< StyleT, PanelStyle > )
			{
				it = controls.emplace( name
					, castor::makeUnique< StyleT >( fullName
						, scene
						, m_engine ) ).first;
			}
			else
			{
				it = controls.emplace( name
					, castor::makeUnique< StyleT >( fullName
						, scene
						, m_engine
						, getDefaultFont()->getName() ) ).first;
			}
		}

		return it->second.get();
	}

	template< typename StyleT >
	StyleT * StylesHolder::getControlStyle( castor::String name
		, castor::StringMap< castor::UniquePtr< StyleT > > const & controls )const
	{
		auto it = controls.find( name );

		if ( it != controls.end() )
		{
			return it->second.get();
		}

		StyleT * style{};
		auto panelIt = std::find_if( m_panelStyles.begin()
			, m_panelStyles.end()
			, [&name, &style]( auto const & lookup )
			{
				if ( name.find( lookup.first + cuT( "/" ) ) == 0u )
				{
					style = lookup.second->template getStyle< StyleT >( name.substr( lookup.first.size() + 1u ) );
				}

				return style != nullptr;
			} );

		if ( panelIt == m_panelStyles.end() )
		{
			style = nullptr;
		}

		if ( style == nullptr )
		{
			auto expandPanelIt = std::find_if( m_expandablePanelStyles.begin()
				, m_expandablePanelStyles.end()
				, [&name, &style]( auto const & lookup )
				{
					auto header = lookup.first + cuT( "/Header/" );
					auto panel = lookup.first + cuT( "/Content/" );

					if ( name.find( header ) == 0u )
					{
						style = lookup.second->getHeaderStyle().template getStyle< StyleT >( name.substr( header.size() ) );
					}
					else if ( name.find( panel ) == 0u )
					{
						style = lookup.second->getContentStyle().template getStyle< StyleT >( name.substr( panel.size() ) );
					}

					return style != nullptr;
				} );

			if ( expandPanelIt == m_expandablePanelStyles.end() )
			{
				style = nullptr;
			}
		}

		if ( style == nullptr )
		{
			CU_SrcException( "StylesHolder", "Couldn't find style" );
		}

		return style;
	}
}
