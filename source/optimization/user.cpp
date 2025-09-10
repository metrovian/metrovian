#include "optimization/user.h"
#include "predefined.h"

std::string optimization_user::objective_function() {
	return export_function();
}