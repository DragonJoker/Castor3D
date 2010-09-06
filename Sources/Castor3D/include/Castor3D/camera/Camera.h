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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Camera___
#define ___C3D_Camera___

namespace Castor3D
{
	//! Class which represents a Camera
	/*!
	Class to represent a Camera, giving its position, orientation, viewport...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Camera
	{
	private:
		friend class Scene;
		friend class CameraRenderer;		
		String m_name;					//! The camera name
		Quaternion m_orientation;		//! The camera orientation, relative to its scene node
		Vector3f m_position;			//! The camera position, relative to its scene node
		float * m_matrix;				//! The transformation matrix
		Viewport * m_viewport;			//! The viewport of this camera
		int m_windowWidth;				//! Display window width
		int m_windowHeight;				//! Display window height
		CameraRenderer * m_renderer;	//! The camera renderer

		/**
		* Constructor, needs the camera renderer, the name, window size and projection type. Creates a viewport renderer and a viewport
		* Not to be used by the user, use Scene::CreateCamera instead
		*@param p_renderer : a camera renderer, OpenGL or Direct3D
		*@param p_name : the camera name
		*@param p_ww, p_wh : display window size
		*@param p_type : Projection type
		*/
		Camera( CameraRenderer * p_renderer, const String & p_name, 
			int p_ww, int p_wh, ProjectionType p_type);

	public:
		/**
		 * Destructor
		 */
		~Camera();
		/**
		 * Applies the viewport, the rotation, and renders all
		 */
		void Apply();
		/**
		 * Removes the transformations
		 */
		void Remove();
		/**
		 * Resizes the viewport
		 */
		void Resize( unsigned int p_width, unsigned int p_height);
		/**
		 * Rotation around Y axis
		 * @param p_angle : [in] The rotation angle
		 */
		void Yaw( float p_angle);//Rotation autour de l'axe Y
		/**
		 * Rotation around X axis
		 * @param p_angle : [in] The rotation angle
		 */
		void Pitch( float p_angle);
		/**
		 * Rotation around Z axis
		 * @param p_angle : [in] The rotation angle
		 */
		void Roll( float p_angle);
		/**
		 * Rotates the camera from a quaternion
		 *@param p_quat : [in] orientation to add to this
		 */
		void Rotate( const Quaternion & p_quat);
		/**
		 * Translates the camera from a Vector3f
		 *@param p_vector : [in] The translation value
		 */
		void Translate( const Vector3f & p_vector);
		/**
		 * Translates the camera from three floats
		 *@param x, y, z : [in] The translation value
		 */
		void Translate( float x, float y, float z);
		/**
		 * Sets the orientation to 0
		 */
		void ResetOrientation();
		/**
		 * Sets the position to 0
		 */
		void ResetPosition();
		/**
		 * Computes and returns the rotation matrix
		 *@return The rotation matrix
		 */
		float * GetRotationMatrix();
		/**
		 * Writes the camera in a file
		 *@param p_file : [in] The file to write in
		 *@return true if successful, false if not
		 */
		bool Write( General::Utils::FileIO * p_file)const;
		/**
		* Returns the first object at mouse coords x and y
		*@param p_scene : [in] The scene used for the selection
		*@param p_mode : [in] The selection mode (vertex, face, submesh, geometry)
		*@param p_found : [out] The found object (vertex, face, submesh, geometry)
		*@param x : [in] The x mouse coordinate
		*@param y : [in] The y mouse coordinate
		*@return true if something was found, false if not
		*/
		bool Select( Scene * p_scene, SelectionMode p_mode, void ** p_found, int x, int y);

	public:
		inline String				GetName			()const {return m_name;}
		inline const Vector3f *		GetPosition		()const {return & m_position;}
		inline Viewport *			GetViewport		()const {return m_viewport;}
		inline const Quaternion *	GetOrientation	()const {return & m_orientation;}
	};
}

#endif
