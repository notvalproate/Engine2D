#pragma once

class MathModule {
public:
	template <typename T>
	static inline constexpr int Sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	static double MoveTowards(double current, double target, double maxDelta);
};