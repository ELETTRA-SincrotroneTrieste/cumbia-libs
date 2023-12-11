/*----- PROTECTED REGION ID(CumbiaTango.cpp) ENABLED START -----*/
static const char *RcsId = "$Id: CumbiaTango.cpp,v 1.1 2017-03-14 13:11:06 giacomo Exp $";
//=============================================================================
//
// file :        CumbiaTango.cpp
//
// description : C++ source for the CumbiaTango class and its commands.
//               The class is derived from Device. It represents the
//               CORBA servant object which will be accessed from the
//               network. All commands which can be executed on the
//               CumbiaTango are implemented in this file.
//
// project :     
//
// This file is part of Tango device class.
// 
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
// 
// $Author: giacomo $
//
// $Revision: 1.1 $
// $Date: 2017-03-14 13:11:06 $
//
// $HeadURL:  $
//
//=============================================================================
//                This file is generated by POGO
//        (Program Obviously used to Generate tango Object)
//=============================================================================


#include <CumbiaTango.h>
#include <CumbiaTangoClass.h>
#include <cudata.h>
#include <cutangothreadlistener.h>
#include "activities.h"
#include <cumacros.h>

/*----- PROTECTED REGION END -----*/	//	CumbiaTango.cpp

/**
 *  CumbiaTango class description:
 *    
 */

//================================================================
//  The following table gives the correspondence
//  between command and method names.
//
//  Command name  |  Method name
//================================================================
//  State         |  Inherited (no method)
//  Status        |  Inherited (no method)
//================================================================

//================================================================
//  Attributes managed are:
//================================================================
//  double_scalar1  |  Tango::DevDouble	Scalar
//  double_scalar2  |  Tango::DevDouble	Scalar
//================================================================

namespace CumbiaTango_ns
{
/*----- PROTECTED REGION ID(CumbiaTango::namespace_starting) ENABLED START -----*/

//	static initializations

/*----- PROTECTED REGION END -----*/	//	CumbiaTango::namespace_starting

//--------------------------------------------------------
/**
 *	Method      : CumbiaTango::CumbiaTango()
 *	Description : Constructors for a Tango device
 *                implementing the classCumbiaTango
 */
//--------------------------------------------------------
CumbiaTango::CumbiaTango(Tango::DeviceClass *cl, string &s)
 : CumbiaAbstractDevice(cl, s.c_str())
{
	/*----- PROTECTED REGION ID(CumbiaTango::constructor_1) ENABLED START -----*/
	init_device();
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::constructor_1
}
//--------------------------------------------------------
CumbiaTango::CumbiaTango(Tango::DeviceClass *cl, const char *s)
 : CumbiaAbstractDevice(cl, s)
{
	/*----- PROTECTED REGION ID(CumbiaTango::constructor_2) ENABLED START -----*/
	init_device();
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::constructor_2
}
//--------------------------------------------------------
CumbiaTango::CumbiaTango(Tango::DeviceClass *cl, const char *s, const char *d)
 : CumbiaAbstractDevice(cl, s, d)
{
	/*----- PROTECTED REGION ID(CumbiaTango::constructor_3) ENABLED START -----*/
	init_device();
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::constructor_3
}

//--------------------------------------------------------
/**
 *	Method      : CumbiaTango::delete_device()
 *	Description : will be called at device destruction or at init command
 */
//--------------------------------------------------------
void CumbiaTango::delete_device()
{
	DEBUG_STREAM << "CumbiaTango::delete_device() " << device_name << endl;
	/*----- PROTECTED REGION ID(CumbiaTango::delete_device) ENABLED START -----*/
	
	//	Delete device allocated objects
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::delete_device
	delete[] attr_double_scalar1_read;
    delete[] attr_double_scalar2_read;

	if (Tango::Util::instance()->is_svr_shutting_down()==false  &&
		Tango::Util::instance()->is_device_restarting(device_name)==false &&
		Tango::Util::instance()->is_svr_starting()==false)
	{
		//	If not shutting down call delete device for inherited object
		CumbiaAbstractDevice_ns::CumbiaAbstractDevice::delete_device();
	}
}

//--------------------------------------------------------
/**
 *	Method      : CumbiaTango::init_device()
 *	Description : will be called at device initialization.
 */
//--------------------------------------------------------
void CumbiaTango::init_device()
{
	DEBUG_STREAM << "CumbiaTango::init_device() create device " << device_name << endl;
	/*----- PROTECTED REGION ID(CumbiaTango::init_device_before) ENABLED START -----*/
	
	//	Initialization before get_device_property() call
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::init_device_before
	
	if (Tango::Util::instance()->is_svr_starting() == false  &&
		Tango::Util::instance()->is_device_restarting(device_name)==false)
	{
		//	If not starting up call init device for inherited object
		CumbiaAbstractDevice_ns::CumbiaAbstractDevice::init_device();
	}
	//	No device property to be read from database
	
	attr_double_scalar1_read = new Tango::DevDouble[1];
    attr_double_scalar2_read = new Tango::DevDouble[1];
	/*----- PROTECTED REGION ID(CumbiaTango::init_device) ENABLED START -----*/
	
	//	Initialize device
    CuTangoThreadListener *l = new CuTangoThreadListener(this,
                                  CuData("name", "read_listener_1"));
    CuTActivity *a1 = new CuTActivity(CuData("device", "test/device/1"));
    CuTActivity *a2 = new CuTActivity(CuData("device", "test/device/2"));
    this->registerActivity(a1, l);
    this->registerActivity(a2, l);
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::init_device
}


//--------------------------------------------------------
/**
 *	Method      : CumbiaTango::always_executed_hook()
 *	Description : method always executed before any command is executed
 */
//--------------------------------------------------------
void CumbiaTango::always_executed_hook()
{
	DEBUG_STREAM << "CumbiaTango::always_executed_hook()  " << device_name << endl;
	/*----- PROTECTED REGION ID(CumbiaTango::always_executed_hook) ENABLED START -----*/
	
	//	code always executed before all requests
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::always_executed_hook
}

//--------------------------------------------------------
/**
 *	Method      : CumbiaTango::read_attr_hardware()
 *	Description : Hardware acquisition for attributes
 */
//--------------------------------------------------------
void CumbiaTango::read_attr_hardware(TANGO_UNUSED(vector<long> &attr_list))
{
	DEBUG_STREAM << "CumbiaTango::read_attr_hardware(vector<long> &attr_list) entering... " << endl;
	/*----- PROTECTED REGION ID(CumbiaTango::read_attr_hardware) ENABLED START -----*/
	
	//	Add your own code
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::read_attr_hardware
}
//--------------------------------------------------------
/**
 *	Method      : CumbiaTango::write_attr_hardware()
 *	Description : Hardware writing for attributes
 */
//--------------------------------------------------------
void CumbiaTango::write_attr_hardware(TANGO_UNUSED(vector<long> &attr_list))
{
	DEBUG_STREAM << "CumbiaTango::write_attr_hardware(vector<long> &attr_list) entering... " << endl;
	/*----- PROTECTED REGION ID(CumbiaTango::write_attr_hardware) ENABLED START -----*/
	
	//	Add your own code
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::write_attr_hardware
}

//--------------------------------------------------------
/**
 *	Read attribute double_scalar1 related method
 *	Description: 
 *
 *	Data type:	Tango::DevDouble
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void CumbiaTango::read_double_scalar1(Tango::Attribute &attr)
{
	DEBUG_STREAM << "CumbiaTango::read_double_scalar1(Tango::Attribute &attr) entering... " << endl;
	/*----- PROTECTED REGION ID(CumbiaTango::read_double_scalar1) ENABLED START -----*/
	//	Set the attribute value
    pr_thread();
    printf("reading double scalar_1 in thread 0x%lx...\n", pthread_self());
	attr.set_value(attr_double_scalar1_read);
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::read_double_scalar1
}
//--------------------------------------------------------
/**
 *	Write attribute double_scalar1 related method
 *	Description: 
 *
 *	Data type:	Tango::DevDouble
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void CumbiaTango::write_double_scalar1(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "CumbiaTango::write_double_scalar1(Tango::WAttribute &attr) entering... " << endl;
	//	Retrieve write value
	Tango::DevDouble	w_val;
	attr.get_write_value(w_val);
	/*----- PROTECTED REGION ID(CumbiaTango::write_double_scalar1) ENABLED START -----*/
    CuData in(CuDType::Device, "test/device/1");  // CuData in("device", "test/device/1")
    in["double_scalar"] = w_val;
    WriteActivity *wa = new WriteActivity(in);
    registerActivity(wa, new CuTangoThreadListener(this, CuData(CuDType::Name, "write_double_scalar_1_listener")));  // , CuData("name", "write_double_scalar_1_listener")
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::write_double_scalar1
}
//--------------------------------------------------------
/**
 *	Read attribute double_scalar2 related method
 *	Description: 
 *
 *	Data type:	Tango::DevDouble
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void CumbiaTango::read_double_scalar2(Tango::Attribute &attr)
{
	DEBUG_STREAM << "CumbiaTango::read_double_scalar2(Tango::Attribute &attr) entering... " << endl;
	/*----- PROTECTED REGION ID(CumbiaTango::read_double_scalar2) ENABLED START -----*/
	//	Set the attribute value
    printf("\e[1;33mreading double scalar_2 in thread 0x%lx...\e[0m\n", pthread_self());
	attr.set_value(attr_double_scalar2_read);
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::read_double_scalar2
}
//--------------------------------------------------------
/**
 *	Write attribute double_scalar2 related method
 *	Description: 
 *
 *	Data type:	Tango::DevDouble
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void CumbiaTango::write_double_scalar2(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "CumbiaTango::write_double_scalar2(Tango::WAttribute &attr) entering... " << endl;
	//	Retrieve write value
	Tango::DevDouble	w_val;
	attr.get_write_value(w_val);
	/*----- PROTECTED REGION ID(CumbiaTango::write_double_scalar2) ENABLED START -----*/
	
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::write_double_scalar2
}

//--------------------------------------------------------
/**
 *	Method      : CumbiaTango::add_dynamic_attributes()
 *	Description : Create the dynamic attributes if any
 *                for specified device.
 */
//--------------------------------------------------------
void CumbiaTango::add_dynamic_attributes()
{
	/*----- PROTECTED REGION ID(CumbiaTango::add_dynamic_attributes) ENABLED START -----*/
	
	//	Add your own code to create and add dynamic attributes if any
	
    /*----- PROTECTED REGION END -----*/	//	CumbiaTango::add_dynamic_attributes
}

//--------------------------------------------------------
/**
 *	Method      : CumbiaTango::add_dynamic_commands()
 *	Description : Create the dynamic commands if any
 *                for specified device.
 */
//--------------------------------------------------------
void CumbiaTango::add_dynamic_commands()
{
	/*----- PROTECTED REGION ID(CumbiaTango::add_dynamic_commands) ENABLED START -----*/
	
	//	Add your own code to create and add dynamic commands if any
	
	/*----- PROTECTED REGION END -----*/	//	CumbiaTango::add_dynamic_commands
}

/*----- PROTECTED REGION ID(CumbiaTango::namespace_ending) ENABLED START -----*/

//	Additional Methods
void CumbiaTango::onProgress(int step, int total, const CuData& data)
{
    printf("CumbiaTango.onProgress: step %d total %d data as string \e[1;32m%s\e[0m]..................this thread \e[1;31m0x%lx\e[0m...........\n",
           step, total,data[CuDType::Device].toString().c_str(),  pthread_self());  // data["device"]
}

void CumbiaTango::onResult(const CuData& data)
{
    std::string dev = data[CuDType::Device].toString();  // data["device"]
    Tango::DevDouble d = data["double_scalar"].toDouble();
    if(dev == "test/device/1")
        *attr_double_scalar1_read = d;
    else if(dev == "test/device/2")
        *attr_double_scalar2_read = d;
    if(!data["error"].isNull())
        perr("CumbiaTango.onResult: error reading data from %s: %s\n",
             data[CuDType::Device].toString().c_str(),  data["error"].toString().c_str() );  // data["device"]
}

CuData CumbiaTango::getToken() const
{

}

/*----- PROTECTED REGION END -----*/	//	CumbiaTango::namespace_ending
} //	namespace
