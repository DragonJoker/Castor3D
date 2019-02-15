/* See LICENSE file in root folder */
#ifndef ___CUTOpenCLBenchmark___
#define ___CUTOpenCLBenchmark___

#if defined( CASTOR_USE_OCL )

#include "CastorUtilsTestPrerequisites.hpp"

#include <CastorUtils/Math/TransformationMatrix.hpp>

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

		castor::Matrix4x4f m_mtx4x4CuA;
		castor::Matrix4x4f m_mtx4x4CuB;
		castor::Matrix4x4f m_mtx4x4CuC;
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
