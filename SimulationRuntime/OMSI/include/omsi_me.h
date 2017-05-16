#ifndef OIS_ME_H
#define OIS_ME_H


#ifdef __cplusplus
extern "C" {
#endif

struct  omsi_event_info_t
{
	//has event iteration converged
	bool iteration_converged;
    //is simulation stopped
	bool terminate_simulation;
    // has next time
	bool next_event_time_defined;
    // next time event
	double  next_event_time;
	// change dynamic state selection
	bool state_value_references_changed;
};



#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif


/*
//called from fmi2_instantiate
int omsi_me_init(omsi_t* osi, const char* GUID, const char* resource_location);
//call from fmi2_setup_expriment
void omsi_setup_experiment(omsi_t* osi, bool tolerance_defined,
                          double relative_tolerance);
//called from fmi2_enter_intialization_mode
int omsi_initialize(omsi_t* osi);
//called from fmi2_set_real
int omsi_set_real(omsi_t* osi, const int vr[], size_t nvr, const double value[]);
//called from fmi2_set_integer
int omsi_set_integer(omsi_t* osi, const int vr[], size_t nvr, const int value[]);
//called from fmi2_set_boolean
int omsi_set_boolean(omsi_t* osi, const int vr[], size_t nvr, const bool value[]);
//called from fmi2_set_string
int omsi_set_string(omsi_t* osi, const int vr[], size_t nvr, const const char* value[]);
//called from fmi2_get_real
int omsi_get_real(omsi_t* osi, const int vr[], size_t nvr, double value[]);
//called from fmi2_get_integer
int omsi_get_integer(omsi_t* osi, const int vr[], size_t nvr, int value[]);
//called from fmi2_get_boolean
int omsi_get_boolean(omsi_t* osi, const int vr[], size_t nvr, bool value[]);
//called from fmi2_get_string
int omsi_get_string(omsi_t* osi, const int vr[], size_t nvr, const char*  value[]);
//called from fmi2_get_directional_derivative
int omsi_get_directional_derivative(omsi_t* osi,
                const int vUnknown_ref[], size_t nUnknown,
                const int vKnown_ref[],   size_t nKnown,
                const double dvKnown[], double dvUnknown[]);

//called from fmi2_get_derivatives
int omsi_get_derivatives(omsi_t* osi, double derivatives[] , size_t nx);
//called from fmi2_get_event_indicators
int omsi_get_event_indicators(omsi_t* osi, double eventIndicators[], size_t ni);
//called from fmi2_get_nominals_of_continuous_states
int omsi_get_nominal_continuous_states(omsi_t* osi, double x_nominal[], size_t nx);
//called from fmi2_completed_integrator_step
int omsi_completed_integrator_step(omsi_t* osi, double* triggered_event);
//called from fmi2_set_time
int omsi_set_time(omsi_t* osi, double time);
//called from  fmi2_set_continuous_states
int omsi_set_continuous_states(omsi_t* osi, const double x[], size_t nx);
//called from fmi2_terminate
int omsi_terminate(omsi_t* osi);
//called from fmi2_terminate
int omsi_terminate(omsi_t* osi);


int omsi_next_time_event(omsi_t* osi);
*/