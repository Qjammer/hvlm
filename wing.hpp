#pragma once
#include <vector>
#include <iostream>
#include "vortex.hpp"

VectorXd interpVector(const MatrixXd& m,double pos,int axis); // First dimension must be strictly increasing. Other dimensions are values interpolated

struct Params{
	MatrixXd quarterLine;
	MatrixXd twist;
	MatrixXd zero_lift;
	MatrixXd chord;
	int subdiv;
	Vector3d airspeed;
};

class WingSection {
private:
	Vector3d xA, xB, xC, xD;
	Vector3d  n_;
	HorseShoe horseshoe_;
public:
	WingSection(const Vector3d& xA, const Vector3d& xB,
	            const Vector3d& xC, const Vector3d& xD,
	            const Vector3d& ias, double vortLength);

	Vector3d getQuarterPoint() const;
	Vector3d getCP() const;
	Vector3d getInducedVec(const Vector3d& xd) const;
	const Vector3d& getNormalVec() const;
	double getWidth() const;
	double getSurface() const;
	const HorseShoe& getHorseshoe() const;
};

class Wing{
	friend int main(int argc, char** argv);
private:
	//double root_zero_lift, tip_zero_lift;
	//double root_twist, tip_twist;
	double flap_x, flap_y_start, flap_y_end, flap_angle;
	double flap_zero_lift_delta=0;
	unsigned int subdivisions;

	Vector3d airspeed_;

	std::vector<WingSection> sections_;

	MatrixXd system_matrix_;
	VectorXd system_vec_;

	VectorXd width_diffs_, surf_diffs_;
	VectorXd quarter_x_pos_, quarter_y_pos_;

	VectorXd circulation_;
	VectorXd local_lift_coeff_;
	double lift_coeff_, moment_coeff_, drag_coeff_;
	VectorXd gamma_deltay_;

	MatrixXd induced_drag_matrix_;


	MatrixXd quarter_pos_;
	MatrixXd twist_;
	MatrixXd zll_;
	MatrixXd chord_;

public:
	Wing(double wingspan, double ar, double tr, double sweep_angle,
		double root_zero, double tip_zero, double root_eps, double tip_eps,
		double flap_x, double flap_y_start, double flap_y_end, double flap_angle,
		unsigned int subdiv, Vector3d& airspeed_);
	Wing(Params p);

	double getMeanAeroChord() const;
	double getSurface() const;

	void generateSections();
	void assembleMatrix();
	void assembleVector();
	void generateCirculation();
	void generateCoefficients();//Lift and moment
	void assembleDragMatrix();
	void generateDragCoefficient();

	double getWingspan() const;
	Vector3d getQuarterPos(double x) const;
	double getTwist(double x) const;
	double getZeroLift(double x) const;
	Vector3d getNormalToSection(const Vector3d& quarterVector,const Vector3d& cpVector) const;
	double getChord(double x) const;
};
