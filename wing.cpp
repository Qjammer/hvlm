#include "wing.hpp"

WingSection::WingSection(const Vector3d& xA, const Vector3d& xB,
                         const Vector3d& xC, const Vector3d& xD,
                         const Vector3d& ias, double vortL):
                	xA(xA),xB(xB),xC(xC),xD(xD),
                	n_(0.5*(xA-xB-xC+xD).cross(-xA-xB+xC+xD)),//Norm of n_ is the surface of the section
                	horseshoe_(std::vector<Vector3d>{xA+vortL*ias.normalized(),xA,xB,xC,xD,xD+vortL*ias.normalized()})
{
}

Vector3d WingSection::getQuarterPoint() const{
	return 0.5*(this->xB+this->xC);
}

Vector3d WingSection::getCP() const{
	return 0.5*(this->xA+this->xD);
}

Vector3d WingSection::getInducedVec(const Vector3d& _xd) const{
	return this->horseshoe_.getInducedVecAlt(_xd);
}

const Vector3d& WingSection::getNormalVec() const{
	return this->n_;
}

double WingSection::getWidth() const{
	return fabs((this->xC-this->xB)[1]);
}

double WingSection::getSurface() const{
	return this->n_.norm();
}

const HorseShoe& WingSection::getHorseshoe() const{
	return this->horseshoe_;
}

/****Wing****/

Wing::Wing(Params p):subdivisions(p.subdiv),airspeed_(p.airspeed),quarter_pos_(p.quarterLine),twist_(p.twist),zll_(p.zero_lift),chord_(p.chord){
	std::cout<<"genSections"<<std::endl;
	this->generateSectionsAlt();
	std::cout<<"assMatr"<<std::endl;
	this->assembleMatrix();
	std::cout<<"assVec"<<std::endl;
	this->assembleVector();
	std::cout<<"solveSys"<<std::endl;
	this->generateCirculation();
	std::cout<<"genCoeffs"<<std::endl;
	this->generateCoefficients();
	std::cout<<"assDrag"<<std::endl;
	this->assembleDragMatrix();
	std::cout<<"genDrag"<<std::endl;
	this->generateDragCoefficient();
}

double Wing::getTwistAlt(double x) const{
	return interpVector(this->twist_,fabs(x),0)(1);
}
double Wing::getChordAlt(double x) const{
	return interpVector(this->chord_,fabs(x),0)(1);

}

double Wing::getZeroLiftAlt(double y) const{
	double abs_y=fabs(y);
	double twist=this->getTwistAlt(y);
	double zll=interpVector(this->zll_,abs_y,0)(1);
	double flap_delta=0;
	if(flap_y_start<abs_y&&abs_y<flap_y_end){
		flap_delta=flap_zero_lift_delta;
	}
	return twist+zll+flap_delta;//TODO: Add flaps
}

double Wing::getWingspan() const{
	return this->quarter_pos_.bottomRows<1>()(1);
}

void Wing::generateSectionsAlt(){
	std::vector<WingSection>section_vector;
	unsigned long n=this->subdivisions;
	this->width_diffs_=VectorXd(n);
	this->surf_diffs_=VectorXd(n);
	this->quarter_x_pos_=VectorXd(n);
	this->quarter_y_pos_=VectorXd(n);

	for(unsigned long i=0;i<n;++i){
		double fracn=double(i)/n; //From 0 to 1
		double fracns=double(i+1)/n; //From 0 to 1
		double fracn2=fracn*2-1;//From -1 to 1
		double fracns2=fracns*2-1;//From -1 to 1

		double x1=this->getWingspan()*fracn2;
		double x2=this->getWingspan()*fracns2;

		Vector3d xB=this->getQuarterPos(x1);
		Vector3d xC=this->getQuarterPos(x2);

		double zll1=this->getZeroLiftAlt(x1);
		double zll2=this->getZeroLiftAlt(x2);
		
		double ch1=this->getChordAlt(x1);
		double ch2=this->getChordAlt(x2);

		Vector3d xA=xB+0.5*ch1*Vector3d(cos(zll1),0,sin(zll1));
		Vector3d xD=xC+0.5*ch2*Vector3d(cos(zll2),0,sin(zll2));

		WingSection sec=WingSection(xA,xB,xC,xD,this->airspeed_,20*this->getWingspan());

		this->width_diffs_(i)=sec.getWidth();
		this->quarter_x_pos_(i)=sec.getQuarterPoint()(0);
		this->quarter_y_pos_(i)=sec.getQuarterPoint()(1);
		this->surf_diffs_(i)=sec.getSurface();

		section_vector.push_back(sec);
	}
	this->sections_=section_vector;
}

Vector3d Wing::getQuarterPos(double y) const{
	Vector3d vec=interpVector(this->quarter_pos_,fabs(y),1);
	if(y<0) vec(1)*=-1;
	return vec;
}

void Wing::assembleMatrix(){
	auto& invec_=this->sections_;
	unsigned int n=invec_.size();
	MatrixXd mat=MatrixXd(n,n);
	for(auto it1=invec_.begin();it1!=invec_.end();++it1){//For each control point
		VectorXd vec=VectorXd(n);
		const Vector3d& cp=it1->getCP();
		const Vector3d& normal=it1->getNormalVec();
		for(auto it2=invec_.begin();it2<invec_.end();++it2){//For each vortex
			vec[it2-invec_.begin()]=it2->getInducedVec(cp).dot(normal);
		}
		mat.row(it1-invec_.begin())=vec;
	}
	this->system_matrix_=mat;
}

void Wing::assembleVector(){
	auto vec_=this->sections_;
	airspeed_=this->airspeed_;
	unsigned int n=vec_.size();
	VectorXd vec=VectorXd(n);
	for(auto it1=vec_.begin();it1!=vec_.end();++it1){//For each control point
		vec[it1-vec_.begin()]=airspeed_.dot(it1->getNormalVec());
	}
	this->system_vec_=-vec;
}

void Wing::generateCirculation(){
	this->circulation_=this->system_matrix_.colPivHouseholderQr().solve(system_vec_);
}

double Wing::getMeanAeroChord() const{
	double s=0;
	for(int i=0;i<this->chord_.rows()-1;i++){
		double yn=this->chord_(i,0);
		double yn1=this->chord_(i+1,0);
		double dy=yn1-yn;
		double cn=this->chord_(i,1);
		double cn1=this->chord_(i+1,1);
		s+=dy*(cn1*cn1-cn1*cn+cn*cn);
	}
	return s/this->getSurface();
}

void Wing::generateCoefficients(){
	this->gamma_deltay_=this->circulation_.cwiseProduct(this->width_diffs_);
	this->local_lift_coeff_=2*this->gamma_deltay_.cwiseProduct(this->surf_diffs_.cwiseInverse())/this->airspeed_.norm();
	this->lift_coeff_=2*this->gamma_deltay_.sum()/(this->airspeed_.norm()*2*this->getSurface());
	double k=-2.0/(this->airspeed_.norm()*2*this->getSurface()*this->getMeanAeroChord());
	this->moment_coeff_=k*this->gamma_deltay_.cwiseProduct(this->quarter_x_pos_).sum();
}

void Wing::assembleDragMatrix(){
	//TODO: Fix the drag computation
	auto invec_=this->sections_;
	unsigned int n=invec_.size();
	MatrixXd mat=MatrixXd(n,n);
	for(auto it1=invec_.begin();it1!=invec_.end();++it1){//For each control point
		VectorXd vec=VectorXd(n);
		for(auto it2=invec_.begin();it2!=invec_.end();++it2){//For each vortex
			vec[it2-invec_.begin()]=it1->getHorseshoe().getInducedVecAlt(it2->getQuarterPoint())[2];
		}
		mat.row(it1-invec_.begin())=vec;
	}
	this->induced_drag_matrix_=mat;
}

void Wing::generateDragCoefficient(){
	double ias=this->airspeed_.norm();
	this->drag_coeff_=(-2.0/(ias*ias*2*getSurface()))*(this->gamma_deltay_.cwiseProduct(this->induced_drag_matrix_*this->circulation_)).sum();
}

double Wing::getSurface() const{
	double s=0;
	for(int i=0;i<this->chord_.rows()-1;++i){
		s+=(this->chord_(i+1,0)-this->chord_(i,0))*0.5*(this->chord_(i+1,1)+this->chord_(i,1));
	}
	return s;
}
