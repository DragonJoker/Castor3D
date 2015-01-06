if ( WIN32 )
	option( PROJECTS_PACKAGE_WIX "Build WiX setup" TRUE )
else()
	option( PROJECTS_PACKAGE_DEB "Build Debian package" TRUE )
	option( PROJECTS_PACKAGE_RPM "Build RPM package" FALSE )
endif()

include( CPackComponent )