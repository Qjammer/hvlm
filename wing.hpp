#pragma once
#include <iostream>
#include "vortex.hpp"

class WingSection {
private:
	HorseShoe horseshoe_;
	Vector3d quarter_pos_, normal_vec_, control_point_;
	double chord1, chord2;
public:
	WingSection(const HorseShoe& hs,const Vector3d& quart_p,const Vector3d& contr_p,
				const Vector3d& normal, double ch1, double ch2);
	const Vector3d& getQuarterPoint() const;
	const Vector3d& getControlPoint() const;
	Vector3d getInducedVec(const Vector3d& xd) const;
	const Vector3d& getNormalVec() const;
	double getWidth() const;
	double getSurface() const;
	const HorseShoe& getHorseshoe() const;
};

class Wing{
	friend int main(int argc, char** argv);
private:
	double wingspan, ar, tr, sweep_angle;
	double root_zero_lift, tip_zero_lift;
	double root_twist, tip_twist;
	double flap_x, flap_y_start, flap_y_end, flap_angle;
	double flap_zero_lift_delta;
	unsigned int subdivisions;

	Vector3d airspeed_;
	double aoa_;

	std::vector<WingSection> sections_;

	MatrixXd system_matrix_;
	VectorXd system_vec_;

	VectorXd width_diffs_, surf_diffs_, zero_lift_angle_;
	VectorXd quarter_x_pos_, quarter_y_pos_;

	VectorXd circulation_;
	VectorXd local_lift_coeff_;
	double lift_coeff_, moment_coeff_, drag_coeff_;
	VectorXd gamma_deltay_;

	MatrixXd induced_drag_matrix_;
public:
	Wing(double wingspan, double ar, double tr, double sweep_angle,
		double root_zero, double tip_zero, double root_eps, double tip_eps,
		double flap_x, double flap_y_start, double flap_y_end, double flap_angle,
		unsigned int subdiv, Vector3d& airspeed_);

	double getZeroLift(double frac) const;
	double getChord(double frac) const;
	double getMeanAeroChord() const;
	double getSurface() const;

	Vector3d normalToZeroLift(double frac) const;
	Vector3d quarterLinePos(double frac) const;//From -1 to 1
	void generateSections();
	void assembleMatrix();
	void assembleVector();
	void generateCirculation();
	void generateCoefficients();//Lift and moment
	void assembleDragMatrix();
	void generateDragCoefficient();
};
