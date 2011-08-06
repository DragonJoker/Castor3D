/*
This source file is part of ProceduralGenerator (https://sourceforge.net/projects/proceduralgene/)

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
#ifndef ___ProceduralGenerator___
#define ___ProceduralGenerator___

namespace Castor3D
{
	class C3D_API Generator
	{
	protected:
		class C3D_API Thread : public MultiThreading::Thread
		{
		private:
			bool			m_bEnded;
			bool			m_bLaunched;
			bool			m_bStopped;

		protected:
			Generator	*	m_pParent;
			int				m_iWidth;
			int				m_iTop;
			int				m_iBottom;
			int				m_iHeight;
			UbPixel			m_pxColour;
			size_t			m_uiIndex;

			mutable MultiThreading::RecursiveMutex	m_mutex;

		public:
			Thread( Generator * p_pParent, size_t p_uiIndex, int iWidth, int iTop, int iBottom, int iTotalHeight, UbPixel const & p_pxColour);
			virtual ~Thread();

			void Run();
			void Wait();
			virtual int Entry();

			virtual void Step()=0;

			inline bool	IsEnded		()const	{ return m_bEnded; }
			inline bool IsStopped	()const	{ CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();return m_bStopped; }

			inline void Relaunch	()						{ m_bLaunched = true; }
			inline void Stop		()						{ CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();m_bStopped = true; }
			inline void SetRed		( unsigned char val)	{ m_pxColour[0] = val; }
			inline void SetGreen	( unsigned char val)	{ m_pxColour[1] = val; }
			inline void SetBlue		( unsigned char val)	{ m_pxColour[2] = val; }

		private:
			static unsigned int StEntry( void * p_pThis);
		};

	protected:
		int							m_iWidth;
		int							m_iHeight;
		unsigned long long			m_ullStep;
		PixelBuffer					m_frontBuffer;
		PixelBuffer					m_backBuffer;
		UbPixel						m_pxColour;
		size_t						m_uiThreadCount;
		bool						m_bEnded;
		std::vector <Thread *>		m_arraySlaveThreads;
		bool						m_bInitialised;
		TextureUnit				*	m_pTexture;

	public:
		Generator( int p_width, int p_height);
		virtual ~Generator();

		virtual bool Step();
		virtual void SetRed( unsigned char val);
		virtual void SetGreen( unsigned char val);
		virtual void SetBlue( unsigned char val);
		virtual void SwapBuffers();
		virtual void InitialiseStep();
		virtual void ClearAllThreads();

		bool AllEnded();
		void Suspend();
		void SetSize( int p_iWidth, int p_iHeight);
		void SetSize( Point2i const & p_size);
		void SaveFrame();

		template <class ThreadClass>
		ThreadClass * CreateThread( int iWidth, int iTop, int iBottom, int iTotalHeight, UbPixel const & p_pxColour)
		{
			ThreadClass * l_pThread = new ThreadClass( this, m_arraySlaveThreads.size(), m_iWidth, iTop, iBottom, iTotalHeight, p_pxColour);
			m_arraySlaveThreads.push_back( l_pThread);
			return l_pThread;
		}

		inline unsigned char		GetRed				()const { return m_pxColour[0]; }
		inline unsigned char		GetGreen			()const { return m_pxColour[1]; }
		inline unsigned char		GetBlue				()const { return m_pxColour[2]; }
		inline bool					IsInitialised		()const { return m_bInitialised; }
		inline const PixelBuffer *	GetPixelsBuffer		()const { return & m_frontBuffer; }
		inline PixelBuffer *		GetPixelsBuffer		()		{ return & m_frontBuffer; }
		inline int					GetWidth			()const { return m_iWidth; }
		inline int					GetHeight			()const { return m_iHeight; }

	protected:
		void _cleanup();
		Point2i _loadImage( String const & p_strImagePath, Image & p_pImage);
		void _subRender();
		void _saveFrame();
	};

	extern int GetCPUCount();
}

#endif
