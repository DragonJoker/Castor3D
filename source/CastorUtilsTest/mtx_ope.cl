
__kernel void mult_4x4( __global float * p_mtxC, __global float * p_mtxA, __global float * p_mtxB )
{
	int l_itx = get_global_id( 0 ); 
	int l_ity = get_global_id( 1 );
 
	// value stores the element that is 
	// computed by the thread
	float l_fValue = 0;
	for( int k = 0 ; k < 4 ; ++k )
	{
		float l_fElementA = p_mtxA[l_ity * 4 + k];
		float l_fElementB = p_mtxB[k * 4 + l_itx];
		l_fValue += l_fElementA * l_fElementB;
	}
 
	// Write the matrix to device memory each 
	// thread writes one element
	p_mtxC[l_ity * 4 + l_itx] = l_fValue;
}

__kernel void mult_3x3( __global float * p_mtxC, __global float * p_mtxA, __global float * p_mtxB )
{
	int l_itx = get_global_id( 0 ); 
	int l_ity = get_global_id( 1 );
 
	// value stores the element that is 
	// computed by the thread
	float l_fValue = 0;
	for( int k = 0 ; k < 3 ; ++k )
	{
		float l_fElementA = p_mtxA[l_ity * 3 + k];
		float l_fElementB = p_mtxB[k * 3 + l_itx];
		l_fValue += l_fElementA * l_fElementB;
	}
 
	// Write the matrix to device memory each 
	// thread writes one element
	p_mtxC[l_ity * 3 + l_itx] = l_fValue;
}

__kernel void mult_2x2( __global float * p_mtxC, __global float * p_mtxA, __global float * p_mtxB )
{
	int l_itx = get_global_id( 0 ); 
	int l_ity = get_global_id( 1 );
 
	// value stores the element that is 
	// computed by the thread
	float l_fValue = 0;
	for( int k = 0 ; k < 2 ; ++k )
	{
		float l_fElementA = p_mtxA[l_ity * 2 + k];
		float l_fElementB = p_mtxB[k * 2 + l_itx];
		l_fValue += l_fElementA * l_fElementB;
	}
 
	// Write the matrix to device memory each 
	// thread writes one element
	p_mtxC[l_ity * 2 + l_itx] = l_fValue;
}