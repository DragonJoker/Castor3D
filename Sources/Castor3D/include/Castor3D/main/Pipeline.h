#ifndef ___CSGL_Matrix___
#define ___CSGL_Matrix___

#include <stack>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_projection.hpp>

namespace Castor3D
{
	class CS3D_API Pipeline
	{
	public:
		typedef enum
		{
			eModelView	=	0,
			eProjection	=	1,
			eTexture	=	2,
			eNbModes	=	3,
		}
		eMATRIX_MODE;

	private:
		typedef std::stack<glm::mat4> glMatrix;

	protected:
		static Pipeline * sm_singleton;
		glMatrix m_matrix[eNbModes];
		eMATRIX_MODE m_eCurrentMode;
		glm::mat3 m_matrixNormal;

	protected:
		Pipeline();
		~Pipeline();

	public:
		template <typename T>
		static inline T * GetSingletonPtr()
		{
			return (T *)sm_singleton;
		}

		static void MatrixMode( eMATRIX_MODE p_eMode);
		static bool LoadIdentity();
		static bool PushMatrix();
		static bool PopMatrix();
		static bool Translate( const Point3r & p_translate);
		static bool Rotate( const Quaternion & p_rotate);
		static bool Scale( const Point3r & p_translate);
		static bool MultMatrix( const Matrix4x4r & p_matrix);
		static bool MultMatrix( const real * p_matrix);
		static bool Perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar);
		static bool Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		static void Apply( ShaderProgramPtr p_pProgram);

	protected:
		virtual void _matrixMode( eMATRIX_MODE p_eMode);
		virtual bool _loadIdentity();
		virtual bool _pushMatrix();
		virtual bool _popMatrix();
		virtual bool _translate( const Point3r & p_translate);
		virtual bool _rotate( const Quaternion & p_rotate);
		virtual bool _scale( const Point3r & p_translate);
		virtual bool _multMatrix( const Matrix4x4r & p_matrix);
		virtual bool _multMatrix( const real * p_matrix);
		virtual bool _perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar);
		virtual bool _ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		virtual void _apply( ShaderProgramPtr p_pProgram){}
	};

	void operator *=( glm::mat4 & p_matA, const Matrix4x4r & p_matB);
}

#endif