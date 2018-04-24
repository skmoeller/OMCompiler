#include<stdio.h>
#include "Lapack.h"
#include "TotalPivot_Interface.h"
//#include "Newton.h"
//#include "newtonIteration.h"
#include "../../../../OMSI/include/omsi.h"
#include "../../../../OMSI/include/omsi_eqns_system.h"

#include "../omsi_math/omsi_math.h"
#include "../omsi_math/omsi_vector.h"


int get_x(sim_data_t* data, omsi_vector_t* vector){
    vector->data[0]=data->real_vars[6];
    vector->data[1]=data->real_vars[7];
    return 1;
}

int set_x(sim_data_t* data, omsi_vector_t* vector){
	data->real_vars[6]=vector->data[0];
	data->real_vars[7]=vector->data[1];
	return 1;
}

int get_a_matrix(sim_data_t* data, omsi_matrix_t* matrix){
	matrix->data[0]=data->real_vars[0];
	matrix->data[1]=data->real_vars[1];
	matrix->data[2]=data->real_vars[2];
	matrix->data[3]=data->real_vars[3];
	return 1;
}

int get_b_vector(sim_data_t* data, omsi_vector_t* vector){
	vector->data[0]=data->real_vars[4];
	vector->data[1]=data->real_vars[5];
	return 1;
}

void fill_system_with_values(omsi_linear_system_t* linearSystem, omsi_t* omsiData){
   linearSystem->n_system = 2;
   //Matrix A
   omsiData->sim_data.real_vars[0]= 2.0;
   omsiData->sim_data.real_vars[1]= 3.0;
   omsiData->sim_data.real_vars[2]= 3.0;
   omsiData->sim_data.real_vars[3]= 0.5;
   //Vector B
   omsiData->sim_data.real_vars[4]= 8.0;
   omsiData->sim_data.real_vars[5]= 4.0;
   //Vector X
   omsiData->sim_data.real_vars[6]= 0.0;
   omsiData->sim_data.real_vars[7]= 0.0;

   linearSystem->equation_index=1;
}

int eval_residual(sim_data_t* data,  omsi_vector_t* x,  omsi_vector_t* f, int iflag)
{
  // f = A*x - b
  double* xd = x->data;
  double* fd = f->data;

  fd[0] = data->real_vars[0]*xd[0] + data->real_vars[1]*xd[1] - data->real_vars[4];
  fd[1] = data->real_vars[2]*xd[0] + data->real_vars[3]*xd[1] - data->real_vars[5];

  return iflag;
}

void assign_function_pointers(omsi_linear_system_t *linearSystem){
	linearSystem->get_x = &get_x;
	linearSystem->set_x = &set_x;
	linearSystem->get_a_matrix = &get_a_matrix;
	linearSystem->get_b_vector = &get_b_vector;
	linearSystem->eval_residual = &eval_residual;
}

void test_lapack(){
	omsi_linear_system_t* linearSystem = malloc(sizeof(omsi_linear_system_t));
	omsi_t *omsiData = malloc(sizeof(omsi_t));
	omsiData->sim_data.real_vars = (double*) malloc(8*sizeof(double));
	fill_system_with_values(linearSystem, omsiData);
	assign_function_pointers(linearSystem);
	omsi_vector_t* x = _omsi_allocateVectorData(2);

	void *lapackData;
	allocateLapackData(linearSystem->n_system, &lapackData);

	int i;
	if (!solveLapack(lapackData, omsiData, linearSystem))
	{
	 linearSystem->get_x(&omsiData->sim_data, x);
	 printf("Yeah, first loop solved successful with Lapack!\n");
	 printf("Solution:\n");
	 for(i=0; i<linearSystem->n_system; ++i)
	   printf("x[%d] = %f\n", i+1, x->data[i]);
	}

	freeLapackData(&lapackData);
	free(linearSystem);
	free(omsiData);
}

void test_totalpivot(){
	omsi_linear_system_t* linearSystem = malloc(sizeof(omsi_linear_system_t));
	omsi_t* omsiData = malloc(sizeof(omsi_t));
	omsiData->sim_data.real_vars = (double*) malloc(8*sizeof(double));
	fill_system_with_values(linearSystem, omsiData);
	assign_function_pointers(linearSystem);
	omsi_vector_t* x = _omsi_allocateVectorData(2);

	void *totalPivotData;
	allocateTotalPivotData(linearSystem->n_system, &totalPivotData);

	int i;
	if (!solveTotalPivot(totalPivotData, omsiData, linearSystem))
	{
	 linearSystem->get_x(&omsiData->sim_data, x);
	 printf("Yeah, first loop solved successful with TotalPivot!\n");
	 printf("Solution:\n");
	 for(i=0; i<linearSystem->n_system; ++i)
	   printf("x[%d] = %f\n", i+1, x->data[i]);
	}

	freeTotalPivotData(&totalPivotData);
	free(linearSystem);
	free(omsiData);
}

//void test_newton(){
//	struct omsi_nonlinear_system_t* nonlinearSystem = malloc(sizeof(struct omsi_nonlinear_system_t));
//	struct omsi_t* omsiData = malloc(sizeof(struct omsi_t));
//	omsiData->sim_data.real_vars = (double*) malloc(8*sizeof(double));
//	fill_system_with_values(nonlinearSystem, omsiData);
//	assign_function_pointers(nonlinearSystem);
//	omsi_vector_t* x = _omsi_allocateVectorData(2);
//
//	void *newtonData;
//	allocateNewtonData(nonlinearSystem->n_system, &newtonData);
//
//	int i;
//	if (!solveNewton(newtonData, omsiData, nonlinearSystem))
//	{
//	 nonlinearSystem->get_x(&omsiData->sim_data, x);
//	 printf("Yeah, first loop solved successful with Newton!\n");
//	 printf("Solution:\n");
//	 for(i=0; i<nonlinearSystem->n_system; ++i)
//	   printf("x[%d] = %f\n", i+1, x->data[i]);
//	}
//
//	freeNewtonData(&newtonData);
//	free(nonlinearSystem);
//	free(omsiData);
//}

int main() {
	test_lapack();
	//test_totalpivot();
	//test_newton();
}



