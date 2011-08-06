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
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Dx9_ShaderProgram___
#define ___Dx9_ShaderProgram___

#include "Module_Dx9Render.hpp"

namespace Castor3D
{
	class Dx9ShaderProgram : public HlslShaderProgram
	{
	protected:
		String		m_linkerLog;

	public:
		Dx9ShaderProgram();
		Dx9ShaderProgram( const StringArray & p_shaderFiles);
		virtual ~Dx9ShaderProgram();
		/**
		 * Get Linker Messages
		 */
		virtual void RetrieveLinkerLog( String & strLog);

		int GetAttributeLocation( String const & p_strName)const{ return 0; }

	private:
		ShaderObjectPtr _createObject(eSHADER_TYPE p_eType);
		virtual shared_ptr<OneIntFrameVariable>			_create1IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point2iFrameVariable>		_create2IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point3iFrameVariable>		_create3IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point4iFrameVariable>		_create4IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<OneUIntFrameVariable>		_create1UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point2uiFrameVariable>		_create2UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point3uiFrameVariable>		_create3UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point4uiFrameVariable>		_create4UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<OneFloatFrameVariable>		_create1FloatVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point2fFrameVariable>		_create2FloatVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point3fFrameVariable>		_create3FloatVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point4fFrameVariable>		_create4FloatVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<OneDoubleFrameVariable>		_create1DoubleVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point2dFrameVariable>		_create2DoubleVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point3dFrameVariable>		_create3DoubleVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Point4dFrameVariable>		_create4DoubleVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x2iFrameVariable>		_create2x2IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x3iFrameVariable>		_create2x3IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x4iFrameVariable>		_create2x4IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x2iFrameVariable>		_create3x2IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x3iFrameVariable>		_create3x3IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x4iFrameVariable>		_create3x4IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x2iFrameVariable>		_create4x2IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x3iFrameVariable>		_create4x3IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x4iFrameVariable>		_create4x4IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x2uiFrameVariable>	_create2x2UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x3uiFrameVariable>	_create2x3UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x4uiFrameVariable>	_create2x4UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x2uiFrameVariable>	_create3x2UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x3uiFrameVariable>	_create3x3UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x4uiFrameVariable>	_create3x4UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x2uiFrameVariable>	_create4x2UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x3uiFrameVariable>	_create4x3UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x4uiFrameVariable>	_create4x4UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x2fFrameVariable>		_create2x2FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x3fFrameVariable>		_create2x3FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x4fFrameVariable>		_create2x4FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x2fFrameVariable>		_create3x2FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x3fFrameVariable>		_create3x3FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x4fFrameVariable>		_create3x4FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x2fFrameVariable>		_create4x2FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x3fFrameVariable>		_create4x3FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x4fFrameVariable>		_create4x4FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x2dFrameVariable>		_create2x2DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x3dFrameVariable>		_create2x3DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix2x4dFrameVariable>		_create2x4DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x2dFrameVariable>		_create3x2DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x3dFrameVariable>		_create3x3DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix3x4dFrameVariable>		_create3x4DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x2dFrameVariable>		_create4x2DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x3dFrameVariable>		_create4x3DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
		virtual shared_ptr<Matrix4x4dFrameVariable>		_create4x4DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject);
	};
}

#endif
