#include "core/reconstruction/optimization/user.h"
#include "core/predefined.h"

std::string optimization_user::objective_function() {
	return export_function();
}