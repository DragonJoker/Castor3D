#ifndef ___CSGL_Pipeline___
#define ___CSGL_Pipeline___

namespace Castor3D
{
	class GLPipeline : public Pipeline
	{
	protected:
		C3DMap( size_t, size_t) m_mapIdByProgram[eNbModes];
		C3DMap( size_t, size_t) m_normalMatrixIDsByProgram;

	public:
		GLPipeline();
		~GLPipeline();

		inline static GLPipeline * GetSingletonPtr()
		{
			return Pipeline::GetSingletonPtr<GLPipeline>();
		}

	private:
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
		virtual void _apply( ShaderProgramPtr p_pProgram);
	};
}

#endif