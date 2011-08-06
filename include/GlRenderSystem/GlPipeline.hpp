/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___Gl_Pipeline___
#define ___Gl_Pipeline___

#include "Module_GlRender.hpp"

namespace Castor3D
{
	class GlPipeline : public Pipeline
	{
	protected:
		typedef KeyedContainer<size_t, size_t>::Map UIntUIntMap;
		typedef KeyedContainer<CGprogram, CGparameter>::Map ParameterProgramMap;

		static UIntUIntMap sm_mapIdByProgram[eMODE_COUNT];
		static UIntUIntMap sm_normalMatrixIDsByProgram;
		static UIntUIntMap sm_pmvMatrixIDsByProgram;

		static ParameterProgramMap sm_mapIdByCgProgram[eMODE_COUNT];
		static ParameterProgramMap sm_normalMatrixIDsByCgProgram;
		static ParameterProgramMap sm_pmvMatrixIDsByCgProgram;

	public:
		GlPipeline();
		~GlPipeline();

		static void InitFunctions();

	private:
		static bool _matrixMode					( eMODE p_eMode);
		static bool _loadIdentity				();
		static bool _pushMatrix					();
		static bool _popMatrix					();
		static bool _translate					( Point3r const & p_translate);
		static bool _rotate						( Quaternion const & p_rotate);
		static bool _scale						( Point3r const & p_scale);
		static bool _multMatrixMtx				( Matrix4x4r const & p_matrix);
		static bool _multMatrixPtr				( real const * p_matrix);
		static bool _perspective				( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar);
		static bool _frustum					( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		static bool _ortho						( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		static bool	_project					( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_ptResult);
		static bool	_unProject					( const Point3i & p_ptWin, Point4r const & p_ptViewport, Point3r & p_ptResult);
		static void _applyProjection			();
		static void _applyModelView				();
		static void _applyProjectionModelView	();
		static void _applyModelNormal			();
		static void _cgApplyProjection			();
		static void _cgApplyModelView			();
		static void _cgApplyProjectionModelView	();
		static void _cgApplyModelNormal			();
		static void _applyViewport				( int p_iWindowWidth, int p_iWindowHeight);
		static void _applyMatrix( eMODE p_eMatrix, char const * p_szName, GlShaderProgram * p_pProgram);
		static void _cgApplyMatrix( eMODE p_eMatrix, char const * p_szName, CgShaderProgram * p_pProgram);
	};
}

#endif