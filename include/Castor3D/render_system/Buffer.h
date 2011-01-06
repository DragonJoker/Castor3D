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
#ifndef ___C3D_Buffer___
#define ___C3D_Buffer___

#include "../geometry/basic/Vertex.h"

namespace Castor3D
{
	//! 3D Buffer management class
	/*!
	Class which holds the buffers used in 3D (texture, VBO ...)
	Not to be used to manage usual buffers of data, use Castor::Resource::Buffer for that
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <typename T>
	class C3D_API Buffer3D : public Castor::Resources::Buffer<T>
	{
	protected:
		bool m_bToDelete;
		bool m_bAssigned;

	public:
		Buffer3D()
			:	m_bAssigned( false),
				m_bToDelete( false)
		{}
		virtual ~Buffer3D(){}
		/**
		* Initialisation function, used by VBOs
		*/
		virtual void Initialise(){}
		/**
		* Activation function, to activate the buffer and show it's content in 3D
		*/
		virtual void Activate() = 0;
		/**
		* De-activation function, to deactivate the buffer and hide it's content in 3D
		*/
		virtual void Deactivate() = 0;

	public:
		/**@name Accessors */
		//@{
		virtual bool IsToDelete	()const	{ return m_bToDelete; }
		virtual bool IsAssigned	()const	{ return m_bAssigned; }
		//@}
	};

	//! Vertex buffer representation
	/*!
	Holds the vertex coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API IndicesBuffer : public Buffer3D<unsigned int>
	{
	public:
		/**
		 * Activation function, to activate the buffer and show the vertex in it
		 */
		virtual void Activate();
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate();
	};

	//! Vertex buffer representation
	/*!
	Holds the vertex coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API VertexBuffer : public Buffer3D<real>
	{
	public:
		/**
		 * Activation function, to activate the buffer and show the vertex in it
		 */
		virtual void Activate();
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate();
	};

	//! Attribs buffer representation
	/*!
	Holds vertex attributes such as normals, texture coordinates, ...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <typename T>
	class C3D_API VertexAttribsBuffer : public Buffer3D<T>
	{
	public:
		VertexAttribsBuffer( const String & p_strName)
		{
		}
		virtual ~VertexAttribsBuffer()
		{
		}
		/**
		 * Activation function, to activate the buffer and show the vertex in it
		 */
		virtual void Activate(){}
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate(){}

		virtual void SetShaderProgram( ShaderProgramPtr p_pProgram){}
	};

	//! Normals buffer representation
	/*!
	Holds the normals coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API NormalsBuffer : public Buffer3D<real>
	{
	public:
		/**
		 * Activation function, to activate the buffer and apply the normals in it
		 */
		virtual void Activate();
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate();
	};

	//! Texture buffer representation, holds texture coordinates
	/*!
	Holds the texture coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API TextureBuffer : public Buffer3D<real>
	{
	public:
		/**
		 * Activation function, void, use next
		 */
		virtual void Activate();
		/**
		 * Activation function, to activate the buffer and apply the textures in it
		 */
		virtual void Activate( PassPtr p_pass)=0;
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate()=0;
	};

	//! Vertex infos buffer representation
	/*!
	Holds the vertex, normals, texture and tangent coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API VertexInfosBuffer : public Buffer3D<real>
	{
	public:
		/**
		 * Activation function, to activate the buffer and apply the normals in it
		 */
		virtual void Activate();
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate();

		virtual void AddVertex( Vertex & p_pVertex, bool p_bIncrease);
	};

	//! Texture buffer representation, holds texture data
	/*!
	Holds the texture coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API TextureBufferObject : public Buffer3D<unsigned char>
	{
	protected:
		Castor::Resources::PixelFormat  m_pixelFormat;
		size_t m_uiSize;
		const unsigned char * m_pBytes;

	public:
		TextureBufferObject();
		~TextureBufferObject();
		/**
		 * Activation function, void, use next
		 */
		virtual void Activate();
		/**
		 * Activation function, to activate the buffer and apply the textures in it
		 */
		virtual void Activate( PassPtr p_pass);
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate();
		/**
		* Initialisation function
		*/
		virtual void Initialise( const Castor::Resources::PixelFormat & p_format, size_t p_uiSize, const unsigned char * p_pBytes);
		/**
		* Cleanup function
		*/
		virtual void Cleanup();
	};

	//! 3D Buffer manager
	/*!
	Holds the buffers used by all the meshes and other things.
	Manages the deletion of the buffers during the Update() function, in order to make it during a render loop (needed for OpenGL's VBO)
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API BufferManager : public Castor::Theory::AutoSingleton <BufferManager>
	{
		friend class Castor::Theory::AutoSingleton <BufferManager>;

	private:
		template <typename T>
		class TBuffers
		{
			friend class BufferManager;

		private:
			typedef typename SmartPtr< Buffer3D<T> >::Shared	Buffer3DPtr;
			typedef typename KeyedContainer<	size_t,			Buffer3D<T> *	>::Map		Buffer3DPtrUIntMap;
			typedef typename KeyedContainer<	Buffer3D<T> *,	size_t			>::Map		UIntBuffer3DPtrMap;
			typedef typename Container<		Buffer3DPtr							>::Vector	Buffer3DPtrArray;
			typedef bool (__thiscall * BufferFunction)( Buffer3DPtr p_pBuffer, size_t p_uiID);
			typedef BufferFunction PBufferFunction;

		public:
			Buffer3DPtrArray		m_arrayBuffersToDelete;
			Buffer3DPtrArray		m_arrayBuffers;
			Buffer3DPtrUIntMap		m_mapBuffers;
			UIntBuffer3DPtrMap		m_mapBuffersIndexes;

		public:
			void Cleanup();
			void Update();
			bool AddBuffer( Buffer3DPtr p_pBuffer, size_t=0);
			bool AssignBuffer( Buffer3D<T> * p_pBuffer, size_t p_uiID);
			bool UnassignBuffer( Buffer3D<T> * p_pBuffer, size_t p_uiID);
			bool DeleteBuffer( Buffer3DPtr p_pBuffer, size_t=0);
		};

	private:
		TBuffers<float> m_stFloatBuffers;
		TBuffers<double> m_stDoubleBuffers;
		TBuffers<int> m_stIntBuffers;
		TBuffers<size_t> m_stUIntBuffers;
		TBuffers<bool> m_stBoolBuffers;
		TBuffers<char> m_stCharBuffers;
		TBuffers<unsigned char> m_stUCharBuffers;
		TBuffers<long> m_stLongBuffers;
		TBuffers<unsigned long> m_stULongBuffers;
		TBuffers<long long> m_stLongLongBuffers;
		TBuffers<unsigned long long> m_stULongLongBuffers;

	private:
		BufferManager(){}
		~BufferManager(){} // cleanup must be made with the update function and by all owners of buffers

	public:
		/**
		 * Cleans up the manager
		 */
		static void Cleanup();
		/**
		 * Updates the manager, id est deletes unused buffers
		 */
		static void Update();
		/**
		 * Creates a buffer and adds it to the manager
		 *@return The newly created buffer
		 */
		template <typename T, typename _Ty>
		static typename SmartPtr<_Ty>::Shared CreateBuffer();
		/**
		 * Creates a buffer to be linked in a shader and adds it to the manager
		 *@param p_strArg : [in] The buffer's name (as it appears in the shader)
		 *@return The newly created buffer
		 */
		template <typename T, typename _Ty>
		static typename SmartPtr<_Ty>::Shared CreateBuffer( const String & p_strArg);
		/**
		 * Assigns the buffer
		 *@param p_uiID : the ID of the assigned buffer
		 *@param p_pBuffer : pointer to the buffer
		 *@return true if successful, false if not
		 */
		template <typename T>
		static bool AssignBuffer( size_t p_uiID, Buffer3D<T> * p_pBuffer);
		/**
		 * Unassigns the buffer
		 *@param p_uiID : the ID of the assigned buffer
		 *@param p_pBuffer : pointer to the buffer
		 *@return true if successful, false if not
		 */
		template <typename T>
		static bool UnassignBuffer( size_t p_uiID, Buffer3D<T> * p_pBuffer);
		/**
		 * Deletes the buffer, id est mark it as unused
		 *@param p_pBuffer : pointer to the buffer
		 *@return true if successful, false if not
		 */
		template <typename T>
		static bool DeleteBuffer( typename SmartPtr< Buffer3D<T> >::Shared p_pBuffer);

	private:
		template <typename T>
		static bool _applyFunction( typename SmartPtr< Buffer3D<T> >::Shared p_pBuffer, size_t p_uiID,
									bool( TBuffers<T>::* p_function)( typename SmartPtr< Buffer3D<T> >::Shared, size_t));
		template <typename T>
		static bool _applyFunctionPtr( Buffer3D<T> * p_pBuffer, size_t p_uiID, bool( TBuffers<T>::* p_function)( Buffer3D<T> *, size_t));

		// Déclaration template de la fonction d'application de la fonction du buffer :P
		template <typename T> bool _apply( typename SmartPtr< Buffer3D<T> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<T>::* p_function)( typename SmartPtr< Buffer3D<T> >::Shared, size_t));
		// Déclaration template de la fonction d'application de la fonction du buffer :P
		template <typename T> bool _applyPtr( Buffer3D<T> * p_pBuffer, size_t p_uiID, bool( TBuffers<T>::* p_function)( Buffer3D<T> *, size_t));



		// Spécialisation pour les float
		template <> bool _apply( SmartPtr< Buffer3D<float> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<float>::* p_function)( SmartPtr< Buffer3D<float> >::Shared, size_t));
		// Spécialisation pour les double
		template <> bool _apply( SmartPtr< Buffer3D<double> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<double>::* p_function)( SmartPtr< Buffer3D<double> >::Shared, size_t));
		// Spécialisation pour les int
		template <> bool _apply( SmartPtr< Buffer3D<int> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<int>::* p_function)( SmartPtr< Buffer3D<int> >::Shared, size_t));
		// Spécialisation pour les unsigned int
		template <> bool _apply( SmartPtr< Buffer3D<size_t> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<size_t>::* p_function)( SmartPtr< Buffer3D<size_t> >::Shared, size_t));
		// Spécialisation pour les bool
		template <> bool _apply( SmartPtr< Buffer3D<bool> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<bool>::* p_function)( SmartPtr< Buffer3D<bool> >::Shared, size_t));
		// Spécialisation pour les char
		template <> bool _apply( SmartPtr< Buffer3D<char> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<char>::* p_function)( SmartPtr< Buffer3D<char> >::Shared, size_t));
		// Spécialisation pour les unsigned char
		template <> bool _apply( SmartPtr< Buffer3D<unsigned char> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned char>::* p_function)( SmartPtr< Buffer3D<unsigned char> >::Shared, size_t));
		// Spécialisation pour les long
		template <> bool _apply( SmartPtr< Buffer3D<long> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<long>::* p_function)( SmartPtr< Buffer3D<long> >::Shared, size_t));
		// Spécialisation pour les unsigned long
		template <> bool _apply( SmartPtr< Buffer3D<unsigned long> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long>::* p_function)( SmartPtr< Buffer3D<unsigned long> >::Shared, size_t));
		// Spécialisation pour les long long
		template <> bool _apply( SmartPtr< Buffer3D<long long> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<long long>::* p_function)( SmartPtr< Buffer3D<long long> >::Shared, size_t));
		// Spécialisation pour les unsigned long long
		template <> bool _apply( SmartPtr< Buffer3D<unsigned long long> >::Shared p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long long>::* p_function)( SmartPtr< Buffer3D<unsigned long long> >::Shared, size_t));




		// Spécialisation pour les float
		template <> bool _applyPtr( Buffer3D<float> * p_pBuffer, size_t p_uiID, bool( TBuffers<float>::* p_function)( Buffer3D<float> *, size_t));
		// Spécialisation pour les double
		template <> bool _applyPtr( Buffer3D<double> * p_pBuffer, size_t p_uiID, bool( TBuffers<double>::* p_function)( Buffer3D<double> *, size_t));
		// Spécialisation pour les int
		template <> bool _applyPtr( Buffer3D<int> * p_pBuffer, size_t p_uiID, bool( TBuffers<int>::* p_function)( Buffer3D<int> *, size_t));
		// Spécialisation pour les unsigned int
		template <> bool _applyPtr( Buffer3D<size_t> * p_pBuffer, size_t p_uiID, bool( TBuffers<size_t>::* p_function)( Buffer3D<size_t> *, size_t));
		// Spécialisation pour les bool
		template <> bool _applyPtr( Buffer3D<bool> * p_pBuffer, size_t p_uiID, bool( TBuffers<bool>::* p_function)( Buffer3D<bool> *, size_t));
		// Spécialisation pour les char
		template <> bool _applyPtr( Buffer3D<char> * p_pBuffer, size_t p_uiID, bool( TBuffers<char>::* p_function)( Buffer3D<char> *, size_t));
		// Spécialisation pour les unsigned char
		template <> bool _applyPtr( Buffer3D<unsigned char> * p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned char>::* p_function)( Buffer3D<unsigned char> *, size_t));
		// Spécialisation pour les long
		template <> bool _applyPtr( Buffer3D<long> * p_pBuffer, size_t p_uiID, bool( TBuffers<long>::* p_function)( Buffer3D<long> *, size_t));
		// Spécialisation pour les unsigned long
		template <> bool _applyPtr( Buffer3D<unsigned long> * p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long>::* p_function)( Buffer3D<unsigned long> *, size_t));
		// Spécialisation pour les long long
		template <> bool _applyPtr( Buffer3D<long long> * p_pBuffer, size_t p_uiID, bool( TBuffers<long long>::* p_function)( Buffer3D<long long> *, size_t));
		// Spécialisation pour les unsigned long long
		template <> bool _applyPtr( Buffer3D<unsigned long long> * p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long long>::* p_function)( Buffer3D<unsigned long long> *, size_t));
	};

#include "Buffer.inl"
}

#endif
