/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.*/
#ifndef ___GL_Pipeline___
#define ___GL_Pipeline___

#include "Module_GLRender.h"

namespace Castor3D
{
	class GLPipeline : public Pipeline
	{
	protected:
		static C3DMap( size_t, size_t) sm_mapIdByProgram[eNbModes];
		static C3DMap( size_t, size_t) sm_normalMatrixIDsByProgram;
		static C3DMap( size_t, size_t) sm_pmvMatrixIDsByProgram;

	public:
		GLPipeline();
		~GLPipeline();

		static void InitFunctions();

	private:
		static bool _matrixMode					( eMATRIX_MODE p_eMode);
		static bool _loadIdentity				();
		static bool _pushMatrix					();
		static bool _popMatrix					();
		static bool _translate					( const Point3r & p_translate);
		static bool _rotate						( const Quaternion & p_rotate);
		static bool _scale						( const Point3r & p_translate);
		static bool _multMatrixMtx				( const Matrix4x4r & p_matrix);
		static bool _multMatrixPtr				( const real * p_matrix);
		static bool _perspective				( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar);
		static bool _ortho						( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		static bool	_project					( const Point3r & p_ptObj, const Point4r & p_ptViewport, Point3r & p_ptResult);
		static bool	_unProject					( const Point3i & p_ptWin, const Point4r & p_ptViewport, Point3r & p_ptResult);
		static void _applyProjection			();
		static void _applyModelView				();
		static void _applyProjectionModelView	();
		static void _applyModelNormal			();
		static void _applyViewport				( int p_iWindowWidth, int p_iWindowHeight);

	private:
		static void _applyMatrix( eMATRIX_MODE p_eMatrix, const char * p_szName, GLShaderProgram * p_pProgram);
	};
}

#endif