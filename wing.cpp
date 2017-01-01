#include "wing.hpp"

WingSection::WingSection(const HorseShoe& _vx, const Vector3d& _qp,
						const Vector3d& _cop, const Vector3d& _normal,
						double ch1,double ch2
					):horseshoe_(_vx), quarter_pos_(_qp), normal_vec_(_normal),
					control_point_(_cop), chord1(ch1), chord2(ch2){}

const Vector3d& WingSection::getQuarterPoint() const{
	return this->quarter_pos_;
}

const Vector3d& WingSection::getControlPoint() const{
	return this->control_point_;
}

Vector3d WingSection::getInducedVec(const Vector3d& _xd) const{
	return this->horseshoe_.getInducedVec(_xd);
}

const Vector3d& WingSection::getNormalVec() const{
	return this->normal_vec_;
}

double WingSection::getWidth() const{
	return this->horseshoe_.width();
}

double WingSection::getSurface() const{
	return this->horseshoe_.width()*0.5*(this->chord1+this->chord2);
}

const HorseShoe& WingSection::getHorseshoe() const{
	return this->horseshoe_;
}

/****Wing****/

Wing::Wing(double _wingspan, double _ar, double _tr, double _sweep_angle,
			double _root_zero, double _tip_zero, double _root_eps, double _tip_eps,
			double _flap_x, double _flap_y_start, double _flap_y_end, double _flap_angle,
			unsigned int _subdiv, Vector3d& _airspeed
		):wingspan(_wingspan), ar(_ar), tr(_tr), sweep_angle(_sweep_angle),
		root_zero_lift(_root_zero), tip_zero_lift(_tip_zero), root_twist(_root_eps),
		tip_twist(_tip_eps), flap_x(_flap_x), flap_y_start(_flap_y_start),
		flap_y_end(_flap_y_end), flap_angle(_flap_angle),
		flap_zero_lift_delta(-(M_PI-acos(2*flap_x-1)+2*sqrt(flap_x*(1-flap_x)))*flap_angle/M_PI),
		subdivisions(2*_subdiv), airspeed_(_airspeed), aoa_(atan2(_airspeed[2],_airspeed[0])){
	flap_zero_lift_delta*=0.9-0.01*180*flap_angle/M_PI;//Approximating correction factor through line
	std::cout<<"Correction factor:"<<0.9-0.01*180*flap_angle/M_PI<<std::endl;
	this->generateSections();
	this->assembleMatrix();
	this->assembleVector();
	this->generateCirculation();
	this->generateCoefficients();
	this->assembleDragMatrix();
	this->generateDragCoefficient();
}

double Wing::getZeroLift(double frac) const{//From -1 to 1
	double abs_frac=fabs(frac);
	double flap_delta=0;
	if(abs_frac>flap_y_start&&abs_frac<flap_y_end){
		flap_delta=flap_zero_lift_delta;
	}
	return root_twist-(root_zero_lift+flap_delta)+fabs(frac)*((tip_twist-root_twist)-(tip_zero_lift-root_zero_lift));
}

double Wing::getChord(double frac) const{
	return 2*wingspan/(this->ar*(1+this->tr))*(1+(this->tr-1)*fabs(frac));
}

Vector3d Wing::normalToZeroLift(double frac) const{//TODO:Needs to account for flaps
	double angle=getZeroLift(frac);
	return Vector3d({sin(angle),0,cos(angle)});
}

Vector3d Wing::quarterLinePos(double frac) const{//From -1 to 1
	double y_pos=frac*wingspan/2;
	double x_pos=0.25*this->getChord(0)+fabs(y_pos)*tan(sweep_angle);
	double z_pos=0;
	return Vector3d({x_pos,y_pos,z_pos});
}

void Wing::generateSections(){
	unsigned long total_subdiv= this->subdivisions;
	std::vector<WingSection> section_vector;

	unsigned long half=total_subdiv/2;
	this->width_diffs_=VectorXd(half);
	this->surf_diffs_=VectorXd(half);
	this->zero_lift_angle_=VectorXd(half);
	this->quarter_x_pos_=VectorXd(half);
	this->quarter_y_pos_=VectorXd(half);

	Vector3d delta=Vector3d(0,this->wingspan/(2.0*total_subdiv),0);//TODO:We may have to stick this into the loop if we want variable widths
	double dist=1.0*this->wingspan;

	for(unsigned long i=0;i<total_subdiv;++i){
		double x=((2.0*i+1)/total_subdiv)-1.0;//Fraction within wingspan from -1 to
		Vector3d quarter_pos=this->quarterLinePos(x);
		// Vector3d xB=quarter_pos-delta;//Method as described in the PDFs
		// Vector3d xC=quarter_pos+delta;
		Vector3d xB=this->quarterLinePos(x-1.0/total_subdiv);//Method as described in example
		Vector3d xC=this->quarterLinePos(x+1.0/total_subdiv);

		// Vector3d vort_length1=Vector3d(dist,0,0);//Method A
		// Vector3d vort_length2=vort_length1;

		// Vector3d vort_length1=Vector3d(dist,0,dist*tan(this->aoa_));//Method B
		// Vector3d vort_length2=vort_length1;
		// What the actual fuck? Did Method B actually break it? Yep, looks like this breaks the program. It starts oscillating uncontrollably.
		// I guess we wont be using it then. Turns out Method A also makes it oscillate, but to a higher tolerance.

		Vector3d vort_length1=Vector3d(dist,0,dist*tan(this->getZeroLift(x-1.0/total_subdiv)));//Method C. Handcrafted. Trailing vortices follow the zero lifting line
		Vector3d vort_length2=Vector3d(dist,0,dist*tan(this->getZeroLift(x+1.0/total_subdiv)));
		//Each Vortex is parallel to the zero lifting line of each wing segment, instead of to the plane of the wing

		//Ok so each Method above has its own shit going on. I dont know which one to use anymore
		Vector3d xA=xB+vort_length1;
		Vector3d xD=xC+vort_length2;
		HorseShoe vs=HorseShoe(xA, xB, xC, xD);
		Vector3d control_point=quarter_pos+0.5*this->getChord(x)*Vector3d(cos(this->getZeroLift(x)),0,sin(this->getZeroLift(x)));


		Vector3d normal=this->normalToZeroLift(x);
		double chord1=this->getChord(x+1.0/total_subdiv);
		double chord2=this->getChord(x-1.0/total_subdiv);
		WingSection sec=WingSection(vs,quarter_pos,control_point,normal,chord1,chord2);
		if(i<half){
			this->width_diffs_[i]=sec.getWidth();
			this->zero_lift_angle_[i]=this->getZeroLift(x);
			this->quarter_x_pos_[i]=sec.getQuarterPoint()[0];
			this->quarter_y_pos_[i]=sec.getQuarterPoint()[1];
			this->surf_diffs_[i]=sec.getSurface();
		}
		section_vector.push_back(sec);
	}
	this->sections_=section_vector;
}

void Wing::assembleMatrix(){
	auto& invec_=this->sections_;
	unsigned int half=invec_.size()/2;
	MatrixXd mat=MatrixXd(half,half);
	for(auto it1=invec_.begin();(it1-invec_.begin())<half;++it1){//Cada punt de control_point
		VectorXd vec=VectorXd(half);
		const Vector3d& cont_p=it1->getControlPoint();
		const Vector3d& normal=it1->getNormalVec();
		auto it2a=invec_.begin();
		auto it2b=invec_.end()-1;
		for(;it2a<it2b;++it2a,--it2b){//Cada Vortex
			vec[it2a-invec_.begin()]=it2a->getInducedVec(cont_p).dot(normal)+it2b->getInducedVec(cont_p).dot(normal);
		}
		mat.row(it1-invec_.begin())=vec;
	}
	this->system_matrix_=mat;
}

void Wing::assembleVector(){
	auto vec_=this->sections_;
	airspeed_=this->airspeed_;
	unsigned int half=vec_.size()/2;
	VectorXd vec=VectorXd(half);
	for(auto it1=vec_.begin();(it1-vec_.begin())<half;++it1){//Cada vector normal
		vec[it1-vec_.begin()]=airspeed_.dot(it1->getNormalVec());
	}
	this->system_vec_=-vec;
}

void Wing::generateCirculation(){
	this->circulation_=this->system_matrix_.colPivHouseholderQr().solve(system_vec_);

}

double Wing::getMeanAeroChord() const{
	return (2.0/3.0)*getChord(0)*(this->tr*this->tr/(1+this->tr)+1);
}

void Wing::generateCoefficients(){
	this->gamma_deltay_=this->circulation_.cwiseProduct(this->width_diffs_);
	auto p1=2.0*this->gamma_deltay_/this->airspeed_.norm();
	this->local_lift_coeff_=p1.cwiseProduct(this->surf_diffs_.cwiseInverse());
	this->lift_coeff_= 2.0*p1.sum()/this->getSurface();
	this->moment_coeff_=-2.0*p1.cwiseProduct(quarter_x_pos_).sum()/(getSurface()*getMeanAeroChord());
}

void Wing::assembleDragMatrix(){
	auto invec_=this->sections_;
	unsigned int half=invec_.size()/2;
	MatrixXd mat=MatrixXd(half,half);
	for(auto it1=invec_.begin();(it1-invec_.begin())<half;++it1){//Cada punt de control_point
		VectorXd vec=VectorXd(invec_.size()/2);
		auto it2a=invec_.begin();
		auto it2b=--invec_.end();
		for(;it2a<it2b;++it2a,--it2b){//Cada Vortex
			vec[it2a-invec_.begin()]=it1->getHorseshoe().getTrailingInducedVec(it2a->getQuarterPoint())[2];
		}
		mat.row(it1-invec_.begin())=vec;
	}
	this->induced_drag_matrix_=mat;
}

void Wing::generateDragCoefficient(){
	double norm=this->airspeed_.norm();
	this->drag_coeff_=(-2.0/(norm*norm*getSurface()))*(this->gamma_deltay_.cwiseProduct(this->induced_drag_matrix_*this->circulation_)).sum();
}

double Wing::getSurface() const{
	return this->wingspan*this->wingspan/this->ar;
}
