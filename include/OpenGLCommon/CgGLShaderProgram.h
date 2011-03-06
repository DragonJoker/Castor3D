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
#ifndef ___Gl_CgShaderProgram___
#define ___Gl_CgShaderProgram___

#include "Module_GlRender.h"

#include <Castor3D/shader/ShaderProgram.h>

namespace Castor3D
{
	class CgGlShaderProgram : public CgShaderProgram
	{
	private:
		shared_ptr < CgGlPointFrameVariable<float, 4>		>	m_pAmbients;
		shared_ptr < CgGlPointFrameVariable<float, 4>		>	m_pDiffuses;
		shared_ptr < CgGlPointFrameVariable<float, 4>		>	m_pSpeculars;
		shared_ptr < CgGlPointFrameVariable<float, 4>		>	m_pPositions;
		shared_ptr < CgGlPointFrameVariable<float, 3>		>	m_pAttenuations;
		shared_ptr < CgGlMatrixFrameVariable<float, 4, 4>	>	m_pOrientations;
		shared_ptr < CgGlOneFrameVariable<float> 			>	m_pExponents;
		shared_ptr < CgGlOneFrameVariable<float> 			>	m_pCutOffs;

		std::set <int> m_setFreeLights;

		shared_ptr < CgGlPointFrameVariable<float, 4>	> 	m_pAmbient;
		shared_ptr < CgGlPointFrameVariable<float, 4>	> 	m_pDiffuse;
		shared_ptr < CgGlPointFrameVariable<float, 4>	> 	m_pEmissive;
		shared_ptr < CgGlPointFrameVariable<float, 4>	> 	m_pSpecular;
		shared_ptr < CgGlOneFrameVariable<float>		> 	m_pShininess;
		bool m_bMatChanged;

		shared_ptr < CgGlPointFrameVariable<float, 4>	> 	m_pAmbientLight;

	protected:
		String		m_linkerLog;
		CgGlFrameVariablePtrStrMap m_mapCgGlFrameVariables;

	public:
		CgGlShaderProgram();
		CgGlShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual ~CgGlShaderProgram();
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

		CgGlFrameVariablePtr GetCgGlFrameVariable( const String & p_strName);

		virtual void AddFrameVariable( CgFrameVariablePtr p_pVariable, CgShaderObjectPtr p_pObject);

		template <typename T>
		void AddFrameVariable( shared_ptr< CgOneFrameVariable<T> > p_pVariable, CgShaderObjectPtr p_pObject)
		{
			CgShaderProgram::AddFrameVariable( p_pVariable, p_pObject);

			if (p_pVariable != NULL)
			{
				if (m_mapCgGlFrameVariables.find( p_pVariable->GetName()) == m_mapCgGlFrameVariables.end())
				{
					shared_ptr< CgGlOneFrameVariable<T> > l_pGLVariable( new CgGlOneFrameVariable<T>( p_pVariable.get()));
					m_mapCgGlFrameVariables.insert( CgGlFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<CgGlFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

		template <typename T, size_t Count>
		void AddFrameVariable( shared_ptr< CgPointFrameVariable<T, Count> > p_pVariable, CgShaderObjectPtr p_pObject)
		{
			CgShaderProgram::AddFrameVariable( p_pVariable, p_pObject);

			if (p_pVariable != NULL)
			{
				if (m_mapCgGlFrameVariables.find( p_pVariable->GetName()) == m_mapCgGlFrameVariables.end())
				{
					shared_ptr< CgGlPointFrameVariable<T, Count> > l_pGLVariable( new CgGlPointFrameVariable<T, Count>( p_pVariable.get()));
					m_mapCgGlFrameVariables.insert( CgGlFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<CgGlFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

		template <typename T, size_t Rows, size_t Columns>
		void AddFrameVariable( shared_ptr< CgMatrixFrameVariable<T, Rows, Columns> > p_pVariable, CgShaderObjectPtr p_pObject)
		{
			CgShaderProgram::AddFrameVariable( p_pVariable, p_pObject);

			if (p_pVariable != NULL)
			{
				if (m_mapCgGlFrameVariables.find( p_pVariable->GetName()) == m_mapCgGlFrameVariables.end())
				{
					 shared_ptr< CgGlMatrixFrameVariable<T, Rows, Columns> > l_pGLVariable( new CgGlMatrixFrameVariable<T, Rows, Columns>( p_pVariable.get()));
					m_mapCgGlFrameVariables.insert( CgGlFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<CgGlFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

	public:
		inline const CgGlFrameVariablePtrStrMap &	GetCgGlFrameVariables	()const { return m_mapCgGlFrameVariables; }
	};
}

#endif
