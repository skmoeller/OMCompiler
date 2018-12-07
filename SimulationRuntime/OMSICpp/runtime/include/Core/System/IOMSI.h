#pragma once

class OMSICallBackWrapper {
public:
	static omsi_status evaluate(struct omsi_function_t*    this_function,
		const omsi_values*         read_only_vars_and_params,
		void*                      data)
	{
		return omsi_ok;
	};
	static omsi_status initialize(struct omsi_function_t*    this_function,
		const omsi_values*         read_only_vars_and_params,
		void*                      data)
	{
		return omsi_ok;
	};
	static void setSystemObj(IContinuous& obj) { _system = &obj; }
private:
	static IContinuous* _system;
};
