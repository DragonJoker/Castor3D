## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

INCLUDES = \
	$(WX_CXXFLAGS)\
	 -I/home/dragonjoker/Projects/GeneralLib/src -I/home/dragonjoker/Projects/CastorShape/Castor3D/include -I/home/dragonjoker/Projects/GLRenderSystem/src

AM_CXXFLAGS =\
	 -D_UNICODE\
	`wx-config --cppflags`\
	 -Wall\
	 -g

bin_PROGRAMS = castorshape

castorshape_SOURCES = \
	CastorShape/geometry/GeometriesListFrame.cpp\
	CastorShape/geometry/NewConeDialog.cpp\
	CastorShape/geometry/NewCubeDialog.cpp\
	CastorShape/geometry/NewCylinderDialog.cpp\
	CastorShape/geometry/NewGeometryDialog.cpp\
	CastorShape/geometry/NewIcosaedronDialog.cpp\
	CastorShape/geometry/NewPlaneDialog.cpp\
	CastorShape/geometry/NewSphereDialog.cpp\
	CastorShape/geometry/NewTorusDialog.cpp\
	CastorShape/main/CastorShape.cpp\
	CastorShape/main/MainFrame.cpp\
	CastorShape/main/RenderPanel.cpp\
	CastorShape/material/EnvironmentFrame.cpp\
	CastorShape/material/MaterialPanel.cpp\
	CastorShape/material/MaterialsFrame.cpp\
	CastorShape/material/MaterialsListView.cpp\
	CastorShape/material/NewMaterialDialog.cpp\
	CastorShape/precompiled_headers/PrecompiledHeaders.cpp

castorshape_LDFLAGS = \
	`wx-config --libs`

castorshape_LDADD =  \
	$(WX_LIBS)\
	 -lcastor3d -lgenerallib -lboost_thread -lfreeimageplus

