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
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Dx9_CgShaderProgram___
#define ___Dx9_CgShaderProgram___

#include "Module_Dx9Render.h"

#include <Castor3D/shader/ShaderProgram.h>

namespace Castor3D
{
	class C3D_Dx9_API CgDx9ShaderProgram : public CgShaderProgram
	{
	private:
		shared_ptr < CgDx9PointFrameVariable<float, 4>		>	m_pAmbients;
		shared_ptr < CgDx9PointFrameVariable<float, 4>		>	m_pDiffuses;
		shared_ptr < CgDx9PointFrameVariable<float, 4>		>	m_pSpeculars;
		shared_ptr < CgDx9PointFrameVariable<float, 4>		>	m_pPositions;
		shared_ptr < CgDx9PointFrameVariable<float, 3>		>	m_pAttenuations;
		shared_ptr < CgDx9MatrixFrameVariable<float, 4, 4>	>	m_pOrientations;
		shared_ptr < CgDx9OneFrameVariable<float> 			>	m_pExponents;
		shared_ptr < CgDx9OneFrameVariable<float> 			>	m_pCutOffs;

		std::set <int> m_setFreeLights;

		shared_ptr < CgDx9PointFrameVariable<float, 4>	> 	m_pAmbient;
		shared_ptr < CgDx9PointFrameVariable<float, 4>	> 	m_pDiffuse;
		shared_ptr < CgDx9PointFrameVariable<float, 4>	> 	m_pEmissive;
		shared_ptr < CgDx9PointFrameVariable<float, 4>	> 	m_pSpecular;
		shared_ptr < CgDx9OneFrameVariable<float>		> 	m_pShininess;
		bool m_bMatChanged;

		shared_ptr < CgDx9PointFrameVariable<float, 4>	> 	m_pAmbientLight;

	protected:
		String		m_linkerLog;
		CgDx9FrameVariablePtrStrMap m_mapCgDx9FrameVariables;

	public:
		CgDx9ShaderProgram();
		CgDx9ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual ~CgDx9ShaderProgram();
		virtual void Cleanup();
		virtual void Initialise();
		/**
		 * Link all Shaders
		 */
		virtual bool Link();
		/**
		 * Use Shader. OpenGL calls will go through vertex, geometry and/or fragment shaders.
		 */
		virtual void Begin();
		/**
		 * Send uniform variables to the shader
		 */
		virtual void ApplyAllVariables();
		/**
		 * Stop using this shader. OpenGL calls will go through regular pipeline.
		 */
		virtual void End();

		int AssignLight();
		void FreeLight( int p_iIndex);

		void SetAmbientLight( const Point4f & p_crColour);
		void SetLightAmbient( int p_iIndex, const Point4f & p_crColour);
		void SetLightDiffuse( int p_iIndex, const Point4f & p_crColour);
		void SetLightSpecular( int p_iIndex, const Point4f & p_crColour);
		void SetLightPosition( int p_iIndex, const Point4f & p_ptPosition);
		void SetLightOrientation( int p_iIndex, const Matrix4x4r & p_mtxOrientation);
		void SetLightAttenuation( int p_iIndex, const Point3f & p_fAtt);
		void SetLightExponent( int p_iIndex, float p_fExp);
		void SetLightCutOff( int p_iIndex, float p_fCut);
		void SetMaterialAmbient( const Point4f & p_crColour);
		void SetMaterialDiffuse( const Point4f & p_crColour);
		void SetMaterialSpecular( const Point4f & p_crColour);
		void SetMaterialEmissive( const Point4f & p_crColour);
		void SetMaterialShininess( float p_fShine);

		CgDx9FrameVariablePtr GetCgGLFrameVariable( const String & p_strName);

		virtual void AddFrameVariable( CgFrameVariablePtr p_pVariable, CgShaderObjectPtr p_pObject);

		template <typename T>
		void AddFrameVariable( shared_ptr< CgOneFrameVariable<T> > p_pVariable, CgShaderObjectPtr p_pObject)
		{
			CgShaderProgram::AddFrameVariable( p_pVariable, p_pObject);

			if (p_pVariable != NULL)
			{
				if (m_mapCgDx9FrameVariables.find( p_pVariable->GetName()) == m_mapCgDx9FrameVariables.end())
				{
					shared_ptr< CgDx9OneFrameVariable<T> > l_pGLVariable( new CgDx9OneFrameVariable<T>( p_pVariable.get()));
					m_mapCgDx9FrameVariables.insert( CgDx9FrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<CgDx9FrameVariableBase>( l_pGLVariable)));
				}
			}
		}

		template <typename T, size_t Count>
		void AddFrameVariable( shared_ptr< CgPointFrameVariable<T, Count> > p_pVariable, CgShaderObjectPtr p_pObject)
		{
			CgShaderProgram::AddFrameVariable( p_pVariable, p_pObject);

			if (p_pVariable != NULL)
			{
				if (m_mapCgDx9FrameVariables.find( p_pVariable->GetName()) == m_mapCgDx9FrameVariables.end())
				{
					shared_ptr< CgDx9PointFrameVariable<T, Count> > l_pGLVariable( new CgDx9PointFrameVariable<T, Count>( p_pVariable.get()));
					m_mapCgDx9FrameVariables.insert( CgDx9FrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<CgDx9FrameVariableBase>( l_pGLVariable)));
				}
			}
		}

		template <typename T, size_t Rows, size_t Columns>
		void AddFrameVariable( shared_ptr< CgMatrixFrameVariable<T, Rows, Columns> > p_pVariable, CgShaderObjectPtr p_pObject)
		{
			CgShaderProgram::AddFrameVariable( p_pVariable, p_pObject);

			if (p_pVariable != NULL)
			{
				if (m_mapCgDx9FrameVariables.find( p_pVariable->GetName()) == m_mapCgDx9FrameVariables.end())
				{
					 shared_ptr< CgDx9MatrixFrameVariable<T, Rows, Columns> > l_pGLVariable( new CgDx9MatrixFrameVariable<T, Rows, Columns>( p_pVariable.get()));
					m_mapCgDx9FrameVariables.insert( CgDx9FrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<CgDx9FrameVariableBase>( l_pGLVariable)));
				}
			}
		}

	public:
		inline const CgDx9FrameVariablePtrStrMap &	GetCgDx9FrameVariables	()const { return m_mapCgDx9FrameVariables; }
	};
}

#endif
