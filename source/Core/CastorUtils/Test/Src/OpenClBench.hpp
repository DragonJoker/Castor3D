/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___CUTOpenCLBenchmark___
#define ___CUTOpenCLBenchmark___

#if defined( CASTOR_USE_OCL )

#include "Benchmark.hpp"

#include <TransformationMatrix.hpp>

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