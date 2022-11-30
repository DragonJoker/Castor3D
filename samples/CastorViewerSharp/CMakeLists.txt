if ( MSVC AND ${CASTOR_BUILD_INTEROP_COM} )
	set( CVS_VERSION_MAJOR 0 )
	set( CVS_VERSION_MINOR 5 )
	set( CVS_VERSION_BUILD 0 )
	set( PROJECT_VERSION "${CVS_VERSION_MAJOR}.${CVS_VERSION_MINOR}.${CVS_VERSION_BUILD}" )

	project( CastorViewerSharp VERSION ${PROJECT_VERSION} LANGUAGES CSharp )

	include( CSharpUtilities )

	# configure_file(
		# ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.csproj.in
		# ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.csproj
		# @ONLY
	# )
	add_executable( ${PROJECT_NAME}
		App.xaml
		App.xaml.cs
		MainWindow.xaml
		MainWindow.xaml.cs

		Properties/AssemblyInfo.cs
		Properties/Resources.Designer.cs
		Properties/Resources.resx
		Properties/Settings.Designer.cs
		Properties/Settings.settings
	)
	csharp_set_designer_cs_properties(
		Properties/AssemblyInfo.cs
		Properties/Resources.Designer.cs
		Properties/Resources.resx
		Properties/Settings.Designer.cs
		Properties/Settings.settings
	)
	csharp_set_xaml_cs_properties(
		App.xaml
		App.xaml.cs
		MainWindow.xaml
		MainWindow.xaml.cs
	)
	set_property( SOURCE App.xaml
		PROPERTY
			VS_XAML_TYPE "ApplicationDefinition"
	)
	set( ${PROJECT_NAME}_REFERENCES
		"Castor3D"
		"System"
		"System.Core"
		"System.Data"
		"System.Data.DataSetExtensions"
		"System.Xaml"
		"System.Xml"
		"System.Xml.Linq"
		"Microsoft.CSharp"
		"WindowsBase"
		"PresentationCore"
		"PresentationFramework"
	)
	set_target_properties( ${PROJECT_NAME}
		PROPERTIES
			VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.8.1"
			WIN32_EXECUTABLE TRUE
			FOLDER "Samples"
			VS_DOTNET_REFERENCES "${${PROJECT_NAME}_REFERENCES}"
	)
	add_dependencies( ${PROJECT_NAME}
		ComCastor3D
	)
	set( Build "yes (version ${CVS_VERSION_MAJOR}.${CVS_VERSION_MINOR}.${CVS_VERSION_BUILD})" PARENT_SCOPE )
endif ()