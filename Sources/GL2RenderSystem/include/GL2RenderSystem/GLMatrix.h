#ifndef ___CSGL_Matrix___
#define ___CSGL_Matrix___

#include "Module_GLRender.h"

namespace Castor3D
{
	class Pipeline : public Theory::AutoSingleton<Pipeline>
	{
		friend class Theory::AutoSingleton<Pipeline>;

	public:
		typedef enum
		{
			eNone		=	0,
			eModelView	=	GL_MODELVIEW,
			eProjection	=	GL_PROJECTION,
			eTexture	=	GL_TEXTURE,
			eNbModes	=	3,
		}
		eMATRIX_MODE;

	private:
		typedef std::stack<glm::mat4> glMatrix;

	protected:
		glMatrix m_matrix[eNbModes];
		eMATRIX_MODE m_eCurrentMode;
		C3DMap( size_t, size_t) m_mapIdByProgram[eNbModes];

	protected:
		Pipeline();
		~Pipeline();

	public:
		static void MatrixMode( eMATRIX_MODE p_eMode);
		static void LoadIdentity();
		static void PushMatrix();
		static void PopMatrix();
		static void Translate( const Point3r & p_translate);
		static void Rotate( const Quaternion & p_rotate);
		static void Scale( const Point3r & p_translate);
		static void MultMatrix( const Matrix4x4r & p_matrix);
		static void MultMatrix( const real * p_matrix);
		static void Apply( GLShaderProgramPtr p_pProgram);
		static void Perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar);
		static void Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
	};

	void operator *=( glm::mat4 & p_matA, const Matrix4x4r & p_matB);
}

#endif