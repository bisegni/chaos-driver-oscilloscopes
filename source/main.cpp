//
//  main.m
//  cocos2d-mac
//
//  Created by Ricardo Quesada on 8/17/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//


#include "Tektronicks.h"

#include <string>

#include <chaos/cu_toolkit/ChaosCUToolkit.h>



using namespace chaos::cu;

#define OPT_CUSTOM_DEVICE_ID "device_id"

int main(int argc, char *argv[])
{
	string tmp_device_id;
	try {
		//add custom paramter
		ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_CUSTOM_DEVICE_ID, po::value<string>(), "Identification string for Tektronicks control unit");
		
		//start the control unit toolkit
		ChaosCUToolkit::getInstance()->init(argc, argv);
		
		//chec if a device id as been setup
		if(ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CUSTOM_DEVICE_ID)) {
			//id has been passed
			tmp_device_id = ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<string>(OPT_CUSTOM_DEVICE_ID);
			
			//instance new control unit with associated id
			ChaosCUToolkit::getInstance()->addControlUnit(new Tektronicks(tmp_device_id));
		} else {
			std::cerr<< "No device id has been passed";
			return 1;
		}
		
		//start control unit toolkit until an event or a signal shutdown it
		ChaosCUToolkit::getInstance()->start();
	} catch (chaos::CException& e) {
		cerr<<"Exception::"<<endl;
		std::cerr<< "in:"<<e.errorDomain << std::endl;
		std::cerr<< "cause:"<<e.errorMessage << std::endl;
	} catch (program_options::error &e){
		cerr << "Unable to parse command line: " << e.what() << endl;
	} catch (...){
		cerr << "unexpected exception caught.. " << endl;
	}
	return 0;
}
