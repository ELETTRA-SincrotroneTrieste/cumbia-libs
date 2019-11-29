/*----- PROTECTED REGION ID(CumbiaAbstractDevice.h) ENABLED START -----*/
//=============================================================================
//
// file :        CumbiaAbstractDevice.h
//
// description : Include file for the CumbiaAbstractDevice class
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
// $Date: 2017-03-14 11:25:46 $
//
// $HeadURL:  $
//
//=============================================================================
//                This file is generated by POGO
//        (Program Obviously used to Generate tango Object)
//=============================================================================


#ifndef CumbiaAbstractDevice_H
#define CumbiaAbstractDevice_H

#include <tango.h>
#include <cudata.h>

class Cumbia;
class CuActivity;
class CuTangoThreadListener;

/*----- PROTECTED REGION END -----*/	//	CumbiaAbstractDevice.h

/**
 *  CumbiaAbstractDevice class description:
 *    
 */

namespace CumbiaAbstractDevice_ns
{
/*----- PROTECTED REGION ID(CumbiaAbstractDevice::Additional Class Declarations) ENABLED START -----*/

class CumbiaAbstractDevicePrivate;

//	Additional Class Declarations

/*----- PROTECTED REGION END -----*/	//	CumbiaAbstractDevice::Additional Class Declarations

class CumbiaAbstractDevice : public TANGO_BASE_CLASS
{

/*----- PROTECTED REGION ID(CumbiaAbstractDevice::Data Members) ENABLED START -----*/

//	Add your own data members

/*----- PROTECTED REGION END -----*/	//	CumbiaAbstractDevice::Data Members



//	Constructors and destructors
public:
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	CumbiaAbstractDevice(Tango::DeviceClass *cl,string &s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device Name
	 */
	CumbiaAbstractDevice(Tango::DeviceClass *cl,const char *s);
	/**
	 * Constructs a newly device object.
	 *
	 *	@param cl	Class.
	 *	@param s 	Device name
	 *	@param d	Device description.
	 */
	CumbiaAbstractDevice(Tango::DeviceClass *cl,const char *s,const char *d);
	/**
	 * The device object destructor.
	 */
	~CumbiaAbstractDevice() {delete_device();};


//	Miscellaneous methods
public:
	/*
	 *	will be called at device destruction or at init command.
	 */
	void delete_device();
	/*
	 *	Initialize the device
	 */
	virtual void init_device();
	/*
	 *	Always executed method before execution command method.
	 */
	virtual void always_executed_hook();


//	Attribute methods
public:
	//--------------------------------------------------------
	/*
	 *	Method      : CumbiaAbstractDevice::read_attr_hardware()
	 *	Description : Hardware acquisition for attributes.
	 */
	//--------------------------------------------------------
	virtual void read_attr_hardware(vector<long> &attr_list);


	//--------------------------------------------------------
	/**
	 *	Method      : CumbiaAbstractDevice::add_dynamic_attributes()
	 *	Description : Add dynamic attributes if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_attributes();




//	Command related methods
public:


	//--------------------------------------------------------
	/**
	 *	Method      : CumbiaAbstractDevice::add_dynamic_commands()
	 *	Description : Add dynamic commands if any.
	 */
	//--------------------------------------------------------
	void add_dynamic_commands();

/*----- PROTECTED REGION ID(CumbiaAbstractDevice::Additional Method prototypes) ENABLED START -----*/

//	Additional Method prototypes

    virtual void onProgress(int step, int total, const CuData& data);

    virtual void onResult(const CuData& data);

    virtual CuData getToken() const;

    Cumbia *getCumbia() const;

    void registerActivity(CuActivity *a, CuTangoThreadListener *l);

private:

    CumbiaAbstractDevicePrivate *d;

/*----- PROTECTED REGION END -----*/	//	CumbiaAbstractDevice::Additional Method prototypes
};

/*----- PROTECTED REGION ID(CumbiaAbstractDevice::Additional Classes Definitions) ENABLED START -----*/

//	Additional Classes Definitions


/*----- PROTECTED REGION END -----*/	//	CumbiaAbstractDevice::Additional Classes Definitions

}	//	End of namespace

#endif   //	CumbiaAbstractDevice_H