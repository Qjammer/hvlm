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

std::vector<double> parseFile(std::string file){
	std::vector<double> outv(PARAMS,-1);
	std::ifstream filestream(file);
	if(!filestream.good()){
		cout<<"Could not open file."<<endl;
		return outv;
	}
	std::string s;
	for(int i=0;i<PARAMS&&filestream.good();++i){
		getline(filestream,s,'\n');
		outv[i]=atof(s.c_str());
	}
	if(!filestream.good()){ outv[0]=-1; }
	return outv;
}

std::vector<double> parseStdIn(int starting, char** argv){
	std::vector<double> outv(PARAMS,-1);
	std::string s;
	for(int i=0;i<PARAMS;++i){ outv[i]=atof(argv[starting+i]); }
	return outv;
}

int main(int argc, char** argv){
	cout<<std::setprecision(10);
	std::stringstream help_str;
	help_str<<"Usage: hvm [OPTION...]"<<endl<<//Help message
	"\t-f file,\tuse this file as input parameters, separated by \\n"<<endl<<
	"\t-o file,\tuse this file as ouput"<<endl<<
	"\t-p [params],\tuse the next 12 parameters as program input"<<endl<<
	"\t-d,\t\tuse degrees instead of radians for angle input"<<endl<<
	"Parameters must be given in the following order, both in the file or the commmand line:"<<endl<<
	"<Wingspan> <Aspect Ratio> <Taper Ratio> <Sweep Angle> <Zero Lift at Root> <Zero Lift at Tip>"<<endl<<
	"<Twist at Root> <Twist at Tip> <Start of Flap as Fraction> <End of Flap as Fraction>"<<endl<<
	"<Flap Zero Lift Increment> <Subdivisions> <Air Speed> <Angle of Attack>"<<endl;

	/****Default Values, in case something goes wrong****/
	double wingspan=1.0;
	double ar=5.0;
	double tr=1.0;
	double sweep_angle=0.0;
	double root_zero_lift=0.0;
	double tip_zero_lift=0.0;
	double root_eps=0.0;
	double tip_eps=0.0;
	double flap_x=1.0;
	double flap_y_start=0.0;
	double flap_y_end=0.0;
	double flap_angle=0.0;
	unsigned int subdiv=50;
	double airspeed=10;
	double aoa=5*M_PI/180;

	/****Getting inputs****/
	std::vector<double> bundle(PARAMS,-1);
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
						std::cerr<<"Parameters already loaded. Terminating."<<endl;
						return 1;
					} else {
						bundle=parseFile(argv[curr+1]);
						paramsLoaded=true;
						curr+=2;
					}
				}else if(argv[curr][1]=='o'){
					if(fileOutput){
						std::cerr<<"File already loaded. Terminating."<<endl;
						return 1;
					} else {
						std::cerr<<"Output File detected"<<endl;
						fileOutput=true;
						outputFile=argv[curr+1];
						curr+=2;
					}
				} else if(argv[curr][1]=='p'){
					if(paramsLoaded){
						std::cerr<<"Parameters already loaded. Terminating."<<endl;
						return 1;
					} else {
						if(curr+PARAMS<argc){
							bundle=parseStdIn(curr+1,argv);
							paramsLoaded=true;
							curr+=(PARAMS+1);
						} else {
							std::cerr<<"Not enough arguments provided. Terminating."<<endl;
							return 1;
						}
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
	if(bundle[0]==0.0){
		cout<<"Could not retrieve the arguments. Terminating."<<endl;
	}
	/****Unpacking parameters****/
	wingspan=bundle[0];
	ar=bundle[1];
	tr=bundle[2];
	sweep_angle=bundle[3];
	root_zero_lift=bundle[4];
	tip_zero_lift=bundle[5];
	root_eps=bundle[6];
	tip_eps=bundle[7];
	flap_x=bundle[8];
	flap_y_start=bundle[9];
	flap_y_end=bundle[10];
	flap_angle=bundle[11];
	subdiv=bundle[12];
	airspeed=bundle[13];
	aoa=bundle[14];
	if(deg){
		constexpr double k=M_PI/180.0;
		sweep_angle*=k;
		root_zero_lift*=k;
		tip_zero_lift*=k;
		root_eps*=k;
		tip_eps*=k;
		flap_angle*=k;
		aoa*=k;
	}
	if(subdiv<50){
		std::cerr<<"Too few subdivisions. Results are not guaranteed to be accurate."<<endl;
	}
	Vector3d airspeed_vec=airspeed*Vector3d({cos(aoa),0,sin(aoa)});

	/****Actual Computing Stuff****/
	Wing w=Wing(wingspan,ar,tr,sweep_angle,root_zero_lift,tip_zero_lift,root_eps,tip_eps,
				flap_x,flap_y_start,flap_y_end,flap_angle,subdiv,airspeed_vec);

	/****Outputting Results****/
	std::ofstream fileStream(outputFile);
	cout<<w.subdivisions/2<<endl;
	if(fileOutput){
		fileStream<<w.subdivisions/2<<endl;
	}

	MatrixXd lift_distr=MatrixXd(w.local_lift_coeff_.size(),2);
	lift_distr<<-w.quarter_y_pos_,w.local_lift_coeff_;
	for(int i=0;i<lift_distr.col(0).size();++i){
		cout<<lift_distr.row(i)[0]<<"\t"<<lift_distr.row(i)[1]<<endl;
		if(fileOutput){
			fileStream<<lift_distr.row(i)[0]<<"\t"<<lift_distr.row(i)[1]<<endl;
		}
	}

	cout<<"CL:"<<w.lift_coeff_<<endl;
	cout<<"CMle:"<<w.moment_coeff_<<endl;
	cout<<"CDi:"<<w.drag_coeff_<<endl;
	if(fileOutput){
		fileStream<<char(29);
		fileStream<<w.lift_coeff_<<";";
		fileStream<<w.moment_coeff_<<";";
		fileStream<<w.drag_coeff_<<";"<<char(29)<<endl;
	}
}
