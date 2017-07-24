/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_GENERATOR_H___
#define ___C3D_GENERATOR_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/Pixel.hpp>
#include <Graphics/PixelBuffer.hpp>

namespace Castor3D
{
	class Generator
	{
	protected:
		class Thread
		{
		private:
			bool m_bEnded{ true };
			bool m_bLaunched{ false };
			bool m_bStopped{ false };

		protected:
			Generator * m_parent{ nullptr };
			int m_iWidth{ 0 };
			int m_iTop{ 0 };
			int m_iBottom{ 0 };
			int m_iHeight{ 0 };
			Castor::UbPixel m_pxColour;
			uint32_t m_index{ 0u };
			std::shared_ptr< std::thread > m_pThread;
			mutable std::recursive_mutex m_mutex;

		public:
			C3D_API Thread( Generator * p_parent, uint32_t p_index, int iWidth, int iTop, int iBottom, int iTotalHeight, Castor::UbPixel const & p_pxColour );
			C3D_API virtual ~Thread();

			C3D_API void Run();
			C3D_API void Wait();
			C3D_API virtual int Entry();

			C3D_API virtual void Step() = 0;

			inline bool	IsEnded()const
			{
				return m_bEnded;
			}
			inline bool IsStopped()const
			{
				auto lock = Castor::make_unique_lock( m_mutex );
				return m_bStopped;
			}

			inline void Relaunch()
			{
				m_bLaunched = true;
			}
			inline void Stop()
			{
				auto lock = Castor::make_unique_lock( m_mutex );
				m_bStopped = true;
			}
			inline void SetRed( uint8_t val )
			{
				m_pxColour[0] = val;
			}
			inline void SetGreen( uint8_t val )
			{
				m_pxColour[1] = val;
			}
			inline void SetBlue( uint8_t val )
			{
				m_pxColour[2] = val;
			}

		private:
			static uint32_t StEntry( Generator::Thread * p_pThis );
		};

	protected:
		int m_iWidth{ 0 };
		int m_iHeight{ 0 };
		unsigned long long m_ullStep{ 0u };
		Castor::PixelBuffer m_frontBuffer;
		Castor::PixelBuffer m_backBuffer;
		Castor::UbPixel m_pxColour;
		uint32_t m_uiThreadCount{ 0u };
		bool m_bEnded{ true };
		std::vector <Thread *> m_arraySlaveThreads;
		bool m_initialised{ false };
		TextureUnit * m_pTexture{ nullptr };
		Engine * m_engine{ nullptr };

	public:
		C3D_API Generator( Engine * engine, int p_width, int p_height );
		C3D_API virtual ~Generator();

		C3D_API virtual bool Step();
		C3D_API virtual void SetRed( uint8_t val );
		C3D_API virtual void SetGreen( uint8_t val );
		C3D_API virtual void SetBlue( uint8_t val );
		C3D_API virtual void SwapBuffers();
		C3D_API virtual void InitialiseStep();
		C3D_API virtual void ClearAllThreads();

		C3D_API bool AllEnded();
		C3D_API void Suspend();
		C3D_API void SetSize( int p_iWidth, int p_iHeight );
		C3D_API void SetSize( Castor::Point2i const & p_size );
		C3D_API void SaveFrame();

		template <class ThreadClass>
		ThreadClass * CreateThread( int iWidth, int iTop, int iBottom, int iTotalHeight, Castor::UbPixel const & p_pxColour )
		{
			ThreadClass * pThread = new ThreadClass( this, m_arraySlaveThreads.size(), m_iWidth, iTop, iBottom, iTotalHeight, p_pxColour );
			m_arraySlaveThreads.push_back( pThread );
			return pThread;
		}

		inline uint8_t GetRed()const
		{
			return m_pxColour[0];
		}
		inline uint8_t GetGreen()const
		{
			return m_pxColour[1];
		}
		inline uint8_t GetBlue()const
		{
			return m_pxColour[2];
		}
		inline bool IsInitialised()const
		{
			return m_initialised;
		}
		inline Castor::PixelBuffer const * GetPixelsBuffer()const
		{
			return & m_frontBuffer;
		}
		inline Castor::PixelBuffer * GetPixelsBuffer()
		{
			return & m_frontBuffer;
		}
		inline int GetWidth()const
		{
			return m_iWidth;
		}
		inline int GetHeight()const
		{
			return m_iHeight;
		}

	protected:
		C3D_API void DoCleanup();
		uint32_t DoGetThreadsCount()
		{
			return uint32_t( m_arraySlaveThreads.size() );
		}
		C3D_API Castor::Point2i _loadImage( Castor::String const & p_strImagePath, Castor::Image & p_pImage );
		C3D_API void _subRender();
		C3D_API void _saveFrame();
	};

	extern int GetCPUCount();
}

#endif
