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
#ifndef ___C3D_Buffer___
#define ___C3D_Buffer___

#include "../material/Module_Material.h"

namespace Castor3D
{
	//! Buffer management class
	/*!
	Class which manages the buffers used in 3D (texture, VBO ...)
	Not to be used to manage usual buffers
	*/
	class CS3D_API Buffer3D : public General::Resource::Buffer <float>
	{
	protected:
		bool m_bToDelete;
		bool m_bAssigned;

	public:
		Buffer3D();
		/**
		* Initialisation function, used by VBOs
		*/
		virtual void Initialise(){}
		/**
		* Activation function, to activate this buffer and show it's content in 3D
		*/
		virtual void Activate() = 0;
		/**
		* De-activation function, to deactivate this buffer and hide it's content in 3D
		*/
		virtual void Deactivate() = 0;

		void Negate( unsigned int p_index);

		inline bool IsToDelete()const { return m_bToDelete; }
		inline bool IsAssigned()const { return m_bAssigned; }
	};

	//! Vertex buffer representation
	/*!
	Holds the vertex coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API VertexBuffer : public Buffer3D
	{
	public:
		/**
		 * Activation function, to activate this buffer and show the vertex in it
		 */
		virtual void Activate(){}
		/**
		 * De-activation function, to deactivate this buffer and hide it's content
		 */
		virtual void Deactivate(){}
	};

	//! Attribs buffer representation
	/*!
	Holds the vertex coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API VertexAttribsBuffer : public Buffer3D
	{
	protected:
		int m_iAttribLocation;

	public:
		/**
		 * Activation function, to activate this buffer and show the vertex in it
		 */
		virtual void Activate(){}
		/**
		 * De-activation function, to deactivate this buffer and hide it's content
		 */
		virtual void Deactivate(){}

		inline void SetAttribLocation( int p_iAttribLocation) { m_iAttribLocation = p_iAttribLocation; }
	};

	//! Normals buffer representation
	/*!
	Holds the normals coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API NormalsBuffer : public Buffer3D
	{
	public:
		/**
		 * Activation function, to activate this buffer and apply the normals in it
		 */
		virtual void Activate(){}
		/**
		 * De-activation function, to deactivate this buffer and hide it's content
		 */
		virtual void Deactivate(){}
	};

	//! Texture buffer representation
	/*!
	Holds the texture coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API TextureBuffer : public Buffer3D
	{
	public:
		/**
		 * Activation function, void, use next
		 */
		virtual void Activate(){}
		/**
		 * Activation function, to activate this buffer and apply the textures in it
		 */
		virtual void Activate( Pass * p_pass)=0;
		/**
		 * De-activation function, to deactivate this buffer and hide it's content
		 */
		virtual void Deactivate()=0;
	};


	//! 3D Buffer manager
	/*!
	Holds the buffers used by all the meshes and other things.
	Manages the deletion of the buffers during the Update() function, in order to make it during a render loop (needed for OpenGL's VBO)
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API BufferManager : public General::Theory::AutoSingleton <BufferManager>
	{
		friend class General::Theory::AutoSingleton <BufferManager>;
	protected:
		std::vector <Buffer3D *> m_arrayBuffersToDelete;
		std::vector <Buffer3D *> m_arrayBuffers;
		std::map <size_t, Buffer3D *> m_mapBuffers;
		std::map <Buffer3D *, size_t> m_mapBuffersIndexes;

	protected:
		BufferManager(){}
		~BufferManager(){} // cleanup must be made with the update function and by all owners of buffers

	public:
		void Cleanup();
		bool AddBuffer( Buffer3D * p_pBuffer);
		bool AssignBuffer( size_t p_uiID, Buffer3D * p_pBuffer);
		bool DeleteBuffer( Buffer3D * p_pBuffer);
		void Update();
	};
}

#endif
