/*******************************************************************************
  Copyright(c) 2016 Magnus W. Eriksen. All rights reserved.

  Arduino Cap, Driver for DIY servo driven dustcaps.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.

  The full GNU General Public License is included in this distribution in the
  file called LICENSE.
*******************************************************************************/

#ifndef ARDUINO_CAP_H
#define ARDUINO_CAP_H

#include "parkdata.h"

#include "defaultdevice.h"
#include "indidustcapinterface.h"
#include "indilightboxinterface.h"

#include <cstring>

class ArduinoCap : public INDI::DefaultDevice, public INDI::DustCapInterface, public INDI::LightBoxInterface
{
    public:

    ArduinoCap();
    virtual ~ArduinoCap();

    virtual bool initProperties();
    virtual void ISGetProperties(const char *dev);
    virtual bool updateProperties();

    virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n);
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n);
    virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n);
    virtual bool ISSnoopDevice(XMLEle *root);

    protected:

    //  Generic indi device entries
    bool Connect();
    bool Disconnect();
    const char *getDefaultName();

    virtual bool saveConfigItems(FILE *fp);
    void TimerHit();

    // From Dust Cap
    virtual IPState ParkCap();
    virtual IPState UnParkCap();

    // From Light Box
    virtual bool SetLightBoxBrightness(uint16_t value);
    virtual bool EnableLightBox(bool enable);

    private:

    bool isConnecting;
    bool isMoving;
    bool isMoveStep;
    bool isClosing;
    double moveToABS;
    double moveToABS2;
    Parkdata parkData;

    void SetupParams();
    void SetOKParkStatus();
    bool Move();
    bool MoveToABS(double toMove);
    bool DoMove();
    double getFullABS(bool closed);
    void setABS(double pos);
    // for second servo:
    bool Move2();
    bool MoveToABS2(double toMove);
    bool DoMove2();
    double getFullABS2(bool closed);
    void setABS2(double pos);
    bool hasSecondServo() {
      return HasSecondServoS[0].s;
    }


    int PopenInt(const char* cmd);

    // Main tab
    INumberVectorProperty MoveSteppNP;
    INumber MoveSteppN[1];

    INumberVectorProperty AbsolutePosNP;
    INumber AbsolutePosN[1];

    ITextVectorProperty StatusTP;
    IText StatusT[2];

    // Options tab
    ISwitchVectorProperty LightTypeSP;
    ISwitch LightTypeS[2];

    ITextVectorProperty DevicePathTP;
    IText DevicePathT[1];

    // Config tab
    INumberVectorProperty ServoIDNP;
    INumber ServoIDN[1];

    INumberVectorProperty LightSwitchNP;
    INumber LightSwitchN[1];

    INumberVectorProperty ServoTravelNP;
    INumber ServoTravelN[2];
    INumber ServoTravelFlip[1];

    
    INumberVectorProperty ServoLimitNP;
    INumber ServoLimitN[2];

    // Second servo
    ISwitchVectorProperty HasSecondServoSNP;
    ISwitch HasSecondServoS[1];

    INumberVectorProperty Servo2DelayNP;
    INumber Servo2DelayN[1];

    INumberVectorProperty Servo2IDNP;
    INumber Servo2IDN[1];

    INumberVectorProperty Servo2TravelNP;
    INumber Servo2TravelN[2];
    INumber Servo2TravelFlip[1];

    INumberVectorProperty AbsolutePos2NP;
    INumber AbsolutePos2N[1];

};
#endif
