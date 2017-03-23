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
#ifndef ___CUTOpenCLBenchmark___
#define ___CUTOpenCLBenchmark___

#if defined( CASTOR_USE_OCL )

#include "CastorUtilsTestPrerequisites.hpp"

#include <Math/TransformationMatrix.hpp>

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#include "cl.hpp"

namespace Testing
{
	class OpenCLBench
		:	public BenchCase
	{
	public:
		OpenCLBench();
		virtual ~OpenCLBench();
		virtual void Execute();

	private:
		void MtxMultCastor();
		void MtxMultCL();

		Castor::Matrix4x4f m_mtx4x4CuA;
		Castor::Matrix4x4f m_mtx4x4CuB;
		Castor::Matrix4x4f m_mtx4x4CuC;
		float m_bufferMtx4x4A[16];
		float m_bufferMtx4x4B[16];
		float m_bufferMtx4x4C[16];
		bool m_bClInitialised;
		cl::Context m_context;
		std::vector< cl::Device > m_arrayDevices;
		std::vector< cl::Platform > m_arrayPlatforms;
		cl::Platform m_platform;
		cl::Buffer m_clBufferMtx4x4A;
		cl::Buffer m_clBufferMtx4x4B;
		cl::Buffer m_clBufferMtx4x4C;
		cl::Program m_program;
		cl::Kernel m_kernel;
		cl::CommandQueue m_queue;
		cl::Event m_event;
	};
}

#endif

#endif
