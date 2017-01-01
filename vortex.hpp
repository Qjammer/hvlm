#pragma once
#include <eigen3/Eigen/Dense>
#include <iostream>

using namespace Eigen;

class HorseShoe{
	private:
		class Vortex{
			private:
				Vector3d x1_,x2_;
			public:
				Vortex(const Vector3d& _x1,const Vector3d& _x2);
				Vector3d getInducedVec(const Vector3d& xp) const;
				Vector3d getAxis() const;
				double width() const;
		};
		Vortex vort1_,vort2_,vort3_;
	public:
		HorseShoe(const Vector3d& xA, const Vector3d& xB, const Vector3d& xC, const Vector3d& xD);
		Vector3d getTrailingInducedVec(const Vector3d& xp) const;
		Vector3d getBoundedInducedVec(const Vector3d& xp) const;
		Vector3d getInducedVec(const Vector3d& xp) const;
		double width() const;
};
