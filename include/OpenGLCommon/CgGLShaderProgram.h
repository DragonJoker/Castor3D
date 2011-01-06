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
#ifndef ___GL_CgShaderProgram___
#define ___GL_CgShaderProgram___

#include "Module_GLRender.h"

#include <Castor3D/shader/Cg/CgShaderProgram.h>

namespace Castor3D
{
	class CgGLShaderProgram : public CgShaderProgram
	{
	private:
		SmartPtr < CgGLPointFrameVariable<float, 4> >::Shared		m_pAmbients;
		SmartPtr < CgGLPointFrameVariable<float, 4> >::Shared		m_pDiffuses;
		SmartPtr < CgGLPointFrameVariable<float, 4> >::Shared		m_pSpeculars;
		SmartPtr < CgGLPointFrameVariable<float, 4> >::Shared		m_pPositions;
		SmartPtr < CgGLPointFrameVariable<float, 3> >::Shared		m_pAttenuations;
		SmartPtr < CgGLMatrixFrameVariable<float, 4, 4> >::Shared	m_pOrientations;
		SmartPtr < CgGLOneFrameVariable<float> >::Shared			m_pExponents;
		SmartPtr < CgGLOneFrameVariable<float> >::Shared			m_pCutOffs;

		std::set <int> m_setFreeLights;

		SmartPtr < CgGLPointFrameVariable<float, 4> >::Shared 	m_pAmbient;
		SmartPtr < CgGLPointFrameVariable<float, 4> >::Shared 	m_pDiffuse;
		SmartPtr < CgGLPointFrameVariable<float, 4> >::Shared 	m_pEmissive;
		SmartPtr < CgGLPointFrameVariable<float, 4> >::Shared 	m_pSpecular;
		SmartPtr < CgGLOneFrameVariable<float> >::Shared 			m_pShininess;
		bool m_bMatChanged;

		SmartPtr < CgGLPointFrameVariable<float, 4> >::Shared 	m_pAmbientLight;

	protected:
		String		m_linkerLog;
		CgGLFrameVariablePtrStrMap m_mapCgGLFrameVariables;

	public:
		CgGLShaderProgram();
		CgGLShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual ~CgGLShaderProgram();
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

		CgGLFrameVariablePtr GetCgGLFrameVariable( const String & p_strName);

		virtual void AddFrameVariable( CgFrameVariablePtr p_pVariable, CgShaderObjectPtr p_pObject);

		template <typename T>
		void AddFrameVariable( typename SmartPtr< CgOneFrameVariable<T> >::Shared p_pVariable, CgShaderObjectPtr p_pObject)
		{
			CgShaderProgram::AddFrameVariable( p_pVariable, p_pObject);

			if ( ! p_pVariable == NULL)
			{
				if (m_mapCgGLFrameVariables.find( p_pVariable->GetName()) == m_mapCgGLFrameVariables.end())
				{
					typename SmartPtr< CgGLOneFrameVariable<T> >::Shared l_pGLVariable( new CgGLOneFrameVariable<T>( p_pVariable.get()));
					m_mapCgGLFrameVariables.insert( CgGLFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<CgGLFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

		template <typename T, size_t Count>
		void AddFrameVariable( typename SmartPtr< CgPointFrameVariable<T, Count> >::Shared p_pVariable, CgShaderObjectPtr p_pObject)
		{
			CgShaderProgram::AddFrameVariable( p_pVariable, p_pObject);

			if ( ! p_pVariable == NULL)
			{
				if (m_mapCgGLFrameVariables.find( p_pVariable->GetName()) == m_mapCgGLFrameVariables.end())
				{
					typename SmartPtr< CgGLPointFrameVariable<T, Count> >::Shared l_pGLVariable( new CgGLPointFrameVariable<T, Count>( p_pVariable.get()));
					m_mapCgGLFrameVariables.insert( CgGLFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<CgGLFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

		template <typename T, size_t Rows, size_t Columns>
		void AddFrameVariable( typename SmartPtr< CgMatrixFrameVariable<T, Rows, Columns> >::Shared p_pVariable, CgShaderObjectPtr p_pObject)
		{
			CgShaderProgram::AddFrameVariable( p_pVariable, p_pObject);

			if ( ! p_pVariable == NULL)
			{
				if (m_mapCgGLFrameVariables.find( p_pVariable->GetName()) == m_mapCgGLFrameVariables.end())
				{
					typename SmartPtr< CgGLMatrixFrameVariable<T, Rows, Columns> >::Shared l_pGLVariable( new CgGLMatrixFrameVariable<T, Rows, Columns>( p_pVariable.get()));
					m_mapCgGLFrameVariables.insert( CgGLFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<CgGLFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

	public:
		inline const CgGLFrameVariablePtrStrMap &	GetCgGLFrameVariables	()const { return m_mapCgGLFrameVariables; }
	};
}

#endif