
#ifndef ___C3D_GeometryRenderer___
#define ___C3D_GeometryRenderer___

#include "../geometry/Module_Geometry.h"
#include "Module_Render.h"
#include "Renderer.h"

namespace Castor3D
{
	//! The Geometry renderer
	/*!
	Renders a geometry, initialises it's buffers, draws it, ...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API GeometryRenderer : public Renderer<Geometry>
	{
	protected:
		friend class RenderSystem;
		float * m_matrix;	//!< A pointer over the geometry's rotation matrix
		/**
		 * Constructor, only RenderSystem can use it
		 */
		GeometryRenderer( RenderSystem * p_rs)
			:	Renderer<Geometry>( p_rs)
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~GeometryRenderer(){ Cleanup(); }
		/**
		 * Cleans up this renderer
		 */
		virtual void Cleanup(){}
		/**
		 * Initialises the geometry's buffers
		 */
		virtual void Initialise() = 0;
		/**
		 * Draws the geometry via it's buffers
		 */
		virtual void Draw( DrawType p_displayMode) = 0;
		/**
		 * Draws the combo box of the geometry
		 */
		virtual void DrawKillBox( Vector3f * p_min, Vector3f * p_max) = 0;
		/**
		 * Applies the geometry's transformations (position, ...)
		 */
		virtual void ApplyTransformations( const Vector3f & p_position,
										   float * p_orientation) = 0;
		/**
		 * Removes the geometry's transformations
		 */
		virtual void RemoveTransformations() = 0;
		/**
		 * Sets the rotation matrix pointer
		 */
		inline void SetMatrix( float * p_matrix) { m_matrix = p_matrix; }
	};
}

#endif
