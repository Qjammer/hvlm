#pragma once
#include <iostream>
#include <vector>
#include <eigen3/Eigen/Dense>

using namespace Eigen;

class HorseShoe{
	private:
		class Vortex{
			private:
				Vector3d x1_,x2_;
			public:
				Vortex(const Vector3d& _x1={0,0,0},const Vector3d& _x2={0,0,0});
				Vector3d getInducedVec(const Vector3d& xp) const;
		};
		std::vector<Vortex> vortices_;
	public:
		HorseShoe(const std::vector<Vector3d>& pts);
		Vector3d getInducedVec(const Vector3d& xp) const;
		double width() const;
};
