__kernel void primos(__global int *vet_primos) {
    
    // Get the index of the current element
    int num = get_global_id(0);
    int i = 0;
    int flag = 0;

    for(i=0; i<256*256; i++)
    {
    	if(num%i == 0)
    	{
    		flag =1;
    		break;
    	}
    }
    // Do the operation
    //C[i] = A[i] + B[i];

}