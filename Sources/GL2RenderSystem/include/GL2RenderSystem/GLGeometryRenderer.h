//******************************************************************************
#ifndef ___GLGeometryContext___
#define ___GLGeometryContext___
//******************************************************************************
#include <Castor3D/geometry/Module_Geometry.h>
#include <Castor3D/render_system/GeometryRenderer.h>
#include "Module_GLRender.h"
//******************************************************************************
namespace Castor3D
{
	class GLGeometryRenderer : public GeometryRenderer
	{
	protected:
		bool m_listCreated;
		unsigned int m_nbUnits;
		
		GLRenderSystem * m_glRenderSystem;

	public :
		GLGeometryRenderer( GLRenderSystem * p_rs);
		CS3D_GL_API virtual ~GLGeometryRenderer();
		
		CS3D_GL_API virtual void Cleanup();
		CS3D_GL_API virtual void Initialise();
		CS3D_GL_API virtual void Draw( DrawType p_displayMode);
		CS3D_GL_API virtual void DrawKillBox( Vector3f * p_min, Vector3f * p_max);
		CS3D_GL_API virtual void ApplyTransformations( const Vector3f & p_position,
													   float * p_orientation);
		CS3D_GL_API virtual void RemoveTransformations();

	private:
		void _initialiseVBO( unsigned int & p_index,
							 unsigned int p_nbCoords,
							 float * p_triangles);
		void _drawVBO( DrawType p_displayMode);
		void _drawVB( DrawType p_displayMode);
	};
}
//******************************************************************************
#endif
//******************************************************************************