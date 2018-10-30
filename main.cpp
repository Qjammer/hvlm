#include <eigen3/Eigen/Dense>
#include <iostream>
#include <fstream>
#include <iomanip>//setprecision()
#include <vector>
#include <limits>
#include "wing.hpp"

using namespace Eigen;
using std::cout;
using std::endl;
constexpr int PARAMS=15;

VectorXd parseVector(std::string txt){
	std::vector<float> nums;
	while(txt.size()>0){
		int pos=txt.find(" ");
		if(pos==-1){
			pos=txt.size();
		}
		std::string numstr=txt.substr(0,pos);
		float num=atof(numstr.c_str());
		nums.push_back(num);
		txt.erase(0,pos+1);
	}
	VectorXd v(nums.size());
	for(unsigned long i=0;i<nums.size();i++){
		v(i)=nums[i];
	}
	return v;
}

VectorXd interpVector(const MatrixXd& m,double pos,int axis){ // First dimension must be strictly increasing. Other dimensions are values interpolated
	if(pos<m(0,axis)){
		return m.row(0);
	}
	for(int i=0;i<m.rows()-1;++i){
		if (m(i,axis)<=pos&&pos<m(i+1,axis)){
			auto a=m.row(i);
			auto b=m.row(i+1);
			auto r=a+(b-a)*(pos-a(axis))/(b(axis)-a(axis));
			return r;
		}
	}
	return m.row(m.rows()-1);
}

Params parseStream(std::istream&& str){
	Params p;
	if(!str.good()){
		std::cout<<"Could not open file."<<endl;
		return p;
	}
	std::string s;
	std::cout<<"Parsing parameters"<<std::endl;

	// Fourth Line
	getline(str,s,'\n');
	VectorXd quarterx=parseVector(s);
	getline(str,s,'\n');
	VectorXd quartery=parseVector(s);
	getline(str,s,'\n');
	VectorXd quarterz=parseVector(s);
	p.quarterLine.resize(quarterx.size(),3);
	p.quarterLine<<quarterx,quartery,quarterz;

	// Twist
	getline(str,s,'\n');
	VectorXd twistx=parseVector(s);
	getline(str,s,'\n');
	VectorXd twistv=parseVector(s);
	p.twist.resize(twistx.size(),2);
	p.twist<<twistx,twistv;

	// Zero_Lift
	getline(str,s,'\n');
	VectorXd zllx=parseVector(s);
	getline(str,s,'\n');
	VectorXd zllv=parseVector(s);
	p.zero_lift.resize(zllx.size(),2);
	p.zero_lift<<zllx,zllv;

	// Chord
	getline(str,s,'\n');
	VectorXd cx=parseVector(s);
	getline(str,s,'\n');
	VectorXd cv=parseVector(s);
	p.chord.resize(cx.size(),2);
	p.chord<<cx,cv;

	// Subdiv
	getline(str,s,'\n');
	double sdv=atof(s.c_str());
	if(sdv!=0){p.subdiv=sdv;}

	//Airspeed
	getline(str,s,'\n');
	VectorXd v=parseVector(s);
	p.airspeed<<v;
	
	return p;
}

int main(int argc, char** argv){
	cout<<std::setprecision(10);

	/**** Help Message Setup ****/
	std::stringstream help_str;
	help_str<<"Usage: hvm [OPTIONS...]"<<endl<<//Help message
	"\t-i file,\tuse this file as input parameters, separated by \\n. Use - as file for stdin input"<<endl<<
	"\t-o file,\tuse this file as ouput"<<endl<<
	"\t-d,\t\tuse degrees instead of radians for angle input"<<endl;

	/****Getting inputs****/
	Params params;
	int curr=1;
	bool fileOutput=false;
	bool paramsLoaded=false;
	std::string outputFile;
	bool deg=false;
	if(argc>1){
		while(curr<argc){
			if(argv[curr][0]=='-'){
				if(argv[curr][1]=='i'){
					if(paramsLoaded){
						std::cerr<<"Input File already loaded. Only one source for the parameters must be defined. Terminating."<<endl;
						return 1;
					} else {
						if(argv[curr+1][0]=='-'){
							params=parseStream(std::move(std::cin));
						} else {
							params=parseStream(std::ifstream(argv[curr+1]));
						}
						paramsLoaded=true;
						curr+=2;
					}
				}else if(argv[curr][1]=='o'){
					if(fileOutput){
						std::cerr<<"Output File already set. Only one ouput file is supported. Terminating."<<endl;
						return 1;
					} else {
						std::cerr<<"Output File detected"<<endl;
						fileOutput=true;
						outputFile=argv[curr+1];
						curr+=2;
					}
				} else if(argv[curr][1]=='h'){
					std::cerr<<help_str.str();
					return 0;
				} else if(argv[curr][1]=='d'){
					deg=true;
					++curr;
				}
			} else {
				cout<<"Parameter not recognized. Type -h for help."<<endl;
				return 1;
			}
		}
	} else {
		std::cerr<<help_str.str();
		return 0;
	}
	
	/**** Unpacking parameters ****/
	if(deg){
		constexpr double k=M_PI/180.0;
		params.twist.col(1)*=k;
		params.zero_lift.col(1)*=k;
	}
	if(params.subdiv<50){
		std::cerr<<"Too few subdivisions. Results are not guaranteed to be accurate."<<endl;
	}

	/**** Actual Computing Stuff ****/
	Wing w=Wing(params);

	/**** Outputting Results ****/
	std::ofstream fileStream(outputFile);
	cout<<w.subdivisions<<endl;
	if(fileOutput){
		fileStream<<w.subdivisions<<endl;
	}

	/**** Airfoil Lift coefficient for each section ****/
	MatrixXd lift_distr=MatrixXd(w.local_lift_coeff_.size(),2);
	lift_distr<<-w.quarter_y_pos_,w.local_lift_coeff_;
	for(int i=0;i<lift_distr.col(0).size();++i){
		cout<<lift_distr.row(i)[0]<<"\t"<<lift_distr.row(i)[1]<<endl;
		if(fileOutput){
			fileStream<<lift_distr.row(i)[0]<<"\t"<<lift_distr.row(i)[1]<<endl;
		}
	}

	/**** Various other coefficients ****/
	cout<<"CL:"<<w.lift_coeff_<<endl;
	cout<<"CMql:"<<w.moment_coeff_<<endl;
	cout<<"CDi:"<<w.drag_coeff_<<endl;
	if(fileOutput){
		fileStream<<char(29);
		fileStream<<w.lift_coeff_<<";";
		fileStream<<w.moment_coeff_<<";";
		fileStream<<w.drag_coeff_<<";"<<char(29)<<endl;
	}
}
