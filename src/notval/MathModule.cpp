#include "MathModule.hpp"
#include <cmath>

double MathModule::MoveTowards(double current, double target, double maxDelta) {
	if (std::abs(target - current) <= maxDelta) {
		return target;
	}
	return current + (Sign(target - current) * maxDelta);
}