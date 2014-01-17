/*
 *	Tektronicks.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include "Tektronicks.h"
#include "tekfun.h"

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#define BUF_LEN 20000

/*
 Construct a new CU with an identifier
 */
Tektronicks::Tektronicks(std::string& device_id):_device_id(device_id), clink(NULL) {
	
}

/*
 Destructor a new CU with an identifier
 */
Tektronicks::~Tektronicks() {
    
}

/*
 Return the default configuration
 */
void Tektronicks::unitDefineActionAndDataset() throw(chaos::CException) {
	
    
    //add managed device di
    setDeviceID(_device_id);
    
    //insert your definition code here
	//setup the dataset
    addAttributeToDataSet("ch1",
                          "CH1 waveform",
                          DataType::TYPE_BYTEARRAY,
                          DataType::Output,
                          500);
	addAttributeToDataSet("ch2",
                          "CH1 waveform",
                          DataType::TYPE_BYTEARRAY,
                          DataType::Output,
                          500);
	addAttributeToDataSet("ch3",
                          "CH1 waveform",
                          DataType::TYPE_BYTEARRAY,
                          DataType::Output,
                          500);
	addAttributeToDataSet("ch4",
                          "CH1 waveform",
                          DataType::TYPE_BYTEARRAY,
                          DataType::Output,
                          500);
    
	addAttributeToDataSet("ip",
                          "Destination ip",
                          DataType::TYPE_STRING,
                          DataType::Input,
						  20);
	
	
	addInputInt32AttributeToDataSet<Tektronicks>("ch_enable",
												 "The masck to activate/deactivate the channel acquisition",
												 this,
												 &Tektronicks::enableChannels);
}

void Tektronicks::unitDefineDriver(std::vector<chaos::cu::driver_manager::driver::DrvRequestInfo>& neededDriver) {
	
}

/*
 Initialize the Custom Contro Unit and return the configuration
 */
void Tektronicks::unitInit() throw(chaos::CException) {
	int				ret;
	char			cmd[1000];
	RangeValueInfo	attribute_info;
	
	
	
	getAttributeRangeValueInfo("ip", attribute_info);
	
	if(attribute_info.defaultValue.size() == 0) {
		throw chaos::CException(1, "No ip configured", "Tektronicks::unitInit");
	}
	
	clink = new CLINK;
	if(!clink)
		throw chaos::CException(2, "could not allocate memory for CLINK ", "Tektronicks::unitInit");
	
	ret = vxi11_open_device(attribute_info.defaultValue.c_str(),clink);
	
	if (ret != 0) {
		delete clink;
		throw chaos::CException(3, "could not open device ", "Tektronicks::unitInit");
	}
	
	memset(cmd, 0, 1000);
	strcpy(cmd, "DAT:ENC RIB;:DAT:WID 2:;:horiziontal:resolution high;:data:start 0;:data:stop 10000");
	ret = vxi11_send(clink, cmd);
	if (ret != 0) {
		throw chaos::CException(4, "Error setting the waveform caratteristics", "Tektronicks::unitInit");
	}
	
	attribute_info.reset();
	getAttributeRangeValueInfo("ch_enable", attribute_info);
	if(attribute_info.defaultValue.size() != 0) {
		enableChannels(_device_id, boost::lexical_cast<int32_t>(attribute_info.defaultValue));
	}
}

/*
 Execute the work, this is called with a determinated delay, it must be as fast as possible
 */
void Tektronicks::unitStart() throw(chaos::CException) {
	
}

/*
 Execute the Control Unit work
 */
void Tektronicks::unitRun() throw(chaos::CException) {
	boost::mutex::scoped_lock lock(mux);
	char		cmd[1000];
	char		buf[BUF_LEN];
	//short int	i_buff[BUF_LEN];
	long		bytes_returned;
	LAPP_ << sizeof(short int);
	CDataWrapper *acquiredData = getNewDataWrapper();
    if(!acquiredData) return;
	
	if(ch_map & 1) {
		strcpy(cmd, "data:source ch1;:curve?");
		vxi11_send(clink, cmd);
		
		bytes_returned = vxi11_receive(clink, buf, BUF_LEN);
		//memcpy((void*)i_buff, (const void *)(buf+7), bytes_returned-7);
		if (bytes_returned > 0) {
			acquiredData->addBinaryValue("ch1", buf, (int)bytes_returned);
		}
	}
	if(ch_map & 2) {
		strcpy(cmd, "data:source ch2;:curve?");
		vxi11_send(clink, cmd);
		
		bytes_returned = vxi11_receive(clink, buf, BUF_LEN);
		//memcpy((void*)i_buff, (const void *)(buf+7), bytes_returned-7);

		if (bytes_returned > 0) {
			acquiredData->addBinaryValue("ch2", buf, (int)bytes_returned);
		}
	}
	if(ch_map & 4) {
		strcpy(cmd, "data:source ch3;:curve?");
		vxi11_send(clink, cmd);
		
		bytes_returned = vxi11_receive(clink, buf, BUF_LEN);
		if (bytes_returned > 0) {
			acquiredData->addBinaryValue("ch3", buf, (int)bytes_returned);
		}
	}
	if(ch_map & 8) {
		strcpy(cmd, "data:source ch4;:curve?");
		vxi11_send(clink, cmd);
		
		bytes_returned = vxi11_receive(clink, buf, BUF_LEN);
		if (bytes_returned > 0) {
			acquiredData->addBinaryValue("ch4", buf, (int)bytes_returned);
		}
	}
	
	//submit acquired data
	pushDataSet(acquiredData);
}

/*
 Execute the Control Unit work
 */
void Tektronicks::unitStop() throw(chaos::CException) {
}

/*
 Deinit the Control Unit
 */
void Tektronicks::unitDeinit() throw(chaos::CException) {
	int				ret;
	RangeValueInfo	attribute_info;
	getAttributeRangeValueInfo("ip", attribute_info);
	if(clink) {
		ret = vxi11_close_device(attribute_info.defaultValue.c_str(),clink);
		if (ret != 0) {
			throw chaos::CException(1, "could not close device connection", "Tektronicks::unitDeinit");
		}
	}
}

//--------------
void Tektronicks::enableChannels(const std::string& deviceID, const int32_t& channel_mask) {
	ch_map = channel_mask;
	boost::mutex::scoped_lock lock(mux);
	if((ch_map & 1) == 1) {
		enableChannel(1);
	} else {
		disableChannel(1);
	}
	if((ch_map & 2) == 2) {
		enableChannel(2);
	}else {
		disableChannel(2);
	}
	if((ch_map & 4) == 4) {
		enableChannel(3);
	}else {
		disableChannel(3);
	}
	if((ch_map & 8) == 8) {
		enableChannel(4);
	}else {
		disableChannel(4);
	}
}

int Tektronicks::enableChannel(int32_t ch_idx) {
	//std::string msg = boost::str( boost::format("ch%1%:bandwidth full;:ch%1%:coupling DC;:ch%1%:impedance fifty;:ch%1%:invert 0;:ch%1%:offset 0;:ch%1%:position 0;:ch%1%:probe 1;:ch%1%:volts 0.5;:fpanel:press ch%1%") % ch_idx);
	std::string msg = boost::str( boost::format(":fpanel:press ch%1%") % ch_idx);
	return vxi11_send(clink, msg.c_str());
}
int Tektronicks::disableChannel(int32_t ch_idx) {
	//std::string msg = boost::str( boost::format("ch%1%:bandwidth full;:ch%1%:coupling DC;:ch%1%:impedance fifty;:ch%1%:invert 0;:ch%1%:offset 0;:ch%1%:position 0;:ch%1%:probe 1;:ch%1%:volts 0.5;:fpanel:press ch%1%") % ch_idx);
	std::string msg = boost::str( boost::format(":fpanel:press ch%1%;:fpanel:press off") % ch_idx);
	return vxi11_send(clink, msg.c_str());
}

