#include "vortex.hpp"

HorseShoe::Vortex::Vortex(const Vector3d& _x1, const Vector3d& _x2):x1_(_x1),x2_(_x2){}

Vector3d HorseShoe::Vortex::getInducedVec(const Vector3d& xp) const{
	Vector3d r0=x2_-x1_, r1=xp-x1_, r2=xp-x2_;
	Vector3d cross=r1.cross(r2);
	return r0.dot(r1.normalized()-r2.normalized())*cross/(cross.dot(cross)*4*M_PI);
}

Vector3d HorseShoe::Vortex::getAxis() const{return (x2_-x1_);}

double HorseShoe::Vortex::width() const{return getAxis().norm();}

HorseShoe::HorseShoe(const Vector3d& xA, const Vector3d& xB, const Vector3d& xC, const Vector3d& xD):vort1_(xA,xB), vort2_(xB,xC), vort3_(xC,xD){}

Vector3d HorseShoe::getTrailingInducedVec(const Vector3d& xp) const{
	return vort1_.getInducedVec(xp)+vort3_.getInducedVec(xp);
}

Vector3d HorseShoe::getBoundedInducedVec(const Vector3d& xp) const{
	return vort2_.getInducedVec(xp);
}

Vector3d HorseShoe::getInducedVec(const Vector3d& xp) const{
	return this->getTrailingInducedVec(xp)+this->getBoundedInducedVec(xp);
}

double HorseShoe::width() const{
	return fabs(vort2_.getAxis()[1]);
}
