#include "vortex.hpp"
#include <cmath>

HorseShoe::Vortex::Vortex(const Vector3d& _x1, const Vector3d& _x2):x1_(_x1),x2_(_x2){}

Vector3d HorseShoe::Vortex::getInducedVec(const Vector3d& xp) const{
	Vector3d r0=x2_-x1_, r1=xp-x1_, r2=xp-x2_;
	Vector3d cross=r1.cross(r2);
	//return r0.dot(r1.normalized()-r2.normalized())*cross/(cross.dot(cross)*4*M_PI);
	auto v=r0.dot(r1.normalized()-r2.normalized())*cross/(cross.dot(cross)*4*M_PI);
	if(std::isnan(double(v[0]))){return Vector3d{0,0,0};}
	return v;
}

HorseShoe::HorseShoe(const std::vector<Vector3d>& pts){
	for(auto it=pts.begin();it!=pts.end()-1;it++){
		this->vortices_.emplace_back(*it,*(it+1));
	}
}

Vector3d HorseShoe::getInducedVec(const Vector3d& xp) const{
	Vector3d r(0.,0.,0.);
	for(auto it=this->vortices_.begin();it!=this->vortices_.end();it++){
		r+=it->getInducedVec(xp);
	}
	return r;
}

