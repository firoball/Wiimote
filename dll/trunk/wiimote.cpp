/*
 *******************************************************************************
 * wiimote.cpp
 * Creation date: 2007
 * Author:        Firoball
 *
 *******************************************************************************
 * $Date$
 * $Revision$
 * $Author$
 *
 *******************************************************************************
 * Description
 * 
 * Driver for Wiimote and its extension devices
 *
 * Comments
 * 
 * cWiimote 0.2 by Kevin Forbes (http://simulatedcomicproduct.com)
 * improved by Robert Jäger (http://www.firoball.de)
 * This code is public domain, and comes with no warranty. The user takes full 
 * responsibility for anything that happens as a result from using this code.
 *
 * information taken from: http://wiibrew.org and http://wiili.org
 *
 *******************************************************************************
 */

#include <stdio.h>
#include "wiimote.h"

#ifdef DEBUG
#include <stdio.h>
#include <string.h>
#include <windows.h>
char txt[100];
#endif


//output channels
const unsigned char OUTPUT_CHANNEL_FORCE_FEEDBACK = 0x13;
const unsigned char OUTPUT_CHANNEL_LED = 0x11;
const unsigned char OUTPUT_CHANNEL_REPORT = 0x12;
const unsigned char OUTPUT_READ_MEMORY = 0x17;
const unsigned char OUTPUT_WRITE_MEMORY = 0x16;
const unsigned char OUTPUT_REQUEST_EXPANSION = 0x15;

const unsigned char OUTPUT_ENABLE_IR = 0x13;
const unsigned char OUTPUT_ENABLE_IR2 = 0x1a;

//report request types
const unsigned char REQUEST_CONTINUOUS_REPORTS = 0x4;
const unsigned char REQUEST_SINGLE_REPORTS = 0x0;

//input channels
const unsigned char INPUT_CHANNEL_BUTTONS_ONLY = 0x30;
const unsigned char INPUT_CHANNEL_BUTTONS_EXT8 = 0x32;
const unsigned char INPUT_CHANNEL_BUTTONS_EXT19 = 0x34;
const unsigned char INPUT_CHANNEL_BUTTONS_IR_EXT9 = 0x36;
const unsigned char INPUT_CHANNEL_BUTTONS_MOTION = 0x31;
const unsigned char INPUT_CHANNEL_MOTION_IR = 0x33;
const unsigned char INPUT_CHANNEL_MOTION_IR_EXT6 = 0x37;
const unsigned char INPUT_CHANNEL_MOTION_EXT16 = 0x35;
const unsigned char INPUT_CHANNEL_WRITE_CONFIRM = 0x22;
const unsigned char INPUT_CHANNEL_READ_DATA = 0x21;
const unsigned char INPUT_CHANNEL_EXPANSION_PORT = 0x20;

//the ID values for a wiimote
const unsigned short mVendorID = 0x057E;
const unsigned short mDeviceID = 0x0306;

//how to find the calibration data for the wiimote
const unsigned short CALIBRATION_ADDRESS = 0x16;
const unsigned short CALIBRATION_DATA_LENGTH = 7;

//main device type identifier (Wiimote or Balance Board)
const unsigned short MAIN_DEV_UNKNOWN = 0;
const unsigned short MAIN_DEV_WIIMOTE = 1;
const unsigned short MAIN_DEV_BALANCE = 2;

//expansion device constants (encrypted)
const unsigned short EXP_DEV_NONE = 0x0000;
const unsigned short EXP_DEV_ERROR = 0xFFFF;
const unsigned short EXP_DEV_NUNCHUK = 0xFEFE;
const unsigned short EXP_DEV_CLASSIC = 0xFDFD;
const unsigned short EXP_DEV_GUITAR = 0xFBFD;
const unsigned short EXP_DEV_BALANCE = 0x0204;

//expansion constants
const unsigned long EXPANSION_INIT_ADDRESS = 0x04A40040;
const unsigned long EXPANSION_STATUS_ADDRESS = 0x04A400FE; //extension type/status address (2 byte)
const unsigned long EXPANSION_CALIBRATION_ADDRESS = 0x04A40020;
const unsigned long BALANCEBOARD_CALIBRATION_ADDRESS = 0x04A40024;
const unsigned char EXPANSION_INIT_VAL = 0x0;

//IR constants
const unsigned long IR_REG_1 = 0x04B00030;
const unsigned long IR_REG_2 = 0x04B00033;
const unsigned long IR_SENS_ADDR_1 = 0x04B00000;
const unsigned long IR_SENS_ADDR_2 = 0x04B0001A;

const unsigned char IR_SENSITIVITY_PART1[5][9] = { //taken from http://wiibrew.org/
	{0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x64, 0x00, 0xFE},
	{0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x96, 0x00, 0xB4},
	{0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xAA, 0x00, 0x64},
	{0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xC8, 0x00, 0x36},
	{0x07, 0x00, 0x00, 0x71, 0x01, 0x00, 0x72, 0x00, 0x20}
};
const unsigned char IR_SENSITIVITY_PART2[5][2] = { //taken from http://wiibrew.org/
	{0xFD, 0x05},
	{0xB3, 0x04},    
	{0x63, 0x03},
	{0x35, 0x03},
	{0x1F, 0x03}
};

const unsigned char IR_MODE_OFF = 0;
const unsigned char IR_MODE_STD = 1;    // 10 Byte IR data
const unsigned char IR_MODE_EXT = 3;    // 12 Byte IR data
const unsigned char IR_MODE_FULL = 5;   // 36 Byte IR data

cWiiMote::cWiiMote()
{
	Init();
}

cWiiMote::~cWiiMote()
{
	Disconnect();
}

void cWiiMote::Init()
{
	mReportMode = REPORT_MODE_EVENT_BUTTONS;

	mLastButtonStatus.Init();
	mLastExpansionReport.Init();
	mLastMotionReport.Init();
	mLastClassicReport.Init();
	mLastGuitarReport.Init();
	mLastBalanceBoardReport.Init();
	mLastIRReport.Init();

	mLastAngle.Init();
	mLastChukAngle.Init();

	mOutputControls.Init();
	mReadInfo.Init();

	mAccelCalibrationData.Init();
	mNunchuckAccelCalibrationData.Init();
	mNunchuckStickCalibrationData.Init();
	mClassicLeftStickCalibrationData.Init();
	mClassicRightStickCalibrationData.Init();
	mClassicShoulderCalibrationData.Init();
	mGuitarStickCalibrationData.Init();
	mGuitarAnalogCalibrationData.Init();
	mBalanceBoardCalibrationData.Init();

	mNunchuckAttached     = false;
	mClassicAttached      = false;
	mGuitarAttached       = false;
	mBalanceBoardAttached = false;

	mIRRunning            = false;
	mDataStreamRunning    = false;
	
	mRequestBattery       = false;
	mRequestIR            = false;
	
	mDeviceType           = MAIN_DEV_WIIMOTE;
	mExpansionType        = EXP_DEV_NONE;
	
	mSmoothFac            = 0.5f;	
	mSensitivityLevel     = 2;
}

bool cWiiMote::SetReportMode(eReportMode mode)
{
	mReportMode = mode;
	return SendReportMode();
}

bool cWiiMote::SendReportMode()
{
    //continuous report or only report on change
	bool continuous = true;
	//report channel
	unsigned char channel = INPUT_CHANNEL_BUTTONS_ONLY;
	
	switch (mReportMode)
	{
	case REPORT_MODE_MOTION_IR:
		channel = INPUT_CHANNEL_MOTION_IR;
		break;
	case REPORT_MODE_MOTION_EXT_IR:
		channel = INPUT_CHANNEL_MOTION_IR_EXT6;
		break;
	case REPORT_MODE_MOTION_EXT:
		channel = INPUT_CHANNEL_MOTION_EXT16;
		break;
	case REPORT_MODE_MOTION:
		channel = INPUT_CHANNEL_BUTTONS_MOTION;
		break;
	case REPORT_MODE_BUTTONS_EXT:
		channel = INPUT_CHANNEL_BUTTONS_EXT8;
		break;
	case REPORT_MODE_EVENT_BUTTONS:
		channel = INPUT_CHANNEL_BUTTONS_ONLY;
		continuous = false;
		break;
	default:
		break;
	}

	bool retval = SelectInputChannel(continuous,channel);
	return retval;
}

int cWiiMote::GetDevices()
{
	cHIDDevice mHIDDevice;
	unsigned char mOutputBuffer[mOutputBufferSize];

	memset(mOutputBuffer,0, mOutputBufferSize);
	mOutputBuffer[0] = OUTPUT_REQUEST_EXPANSION;

	return  (mHIDDevice.GetDevices(mDeviceID, mVendorID, mOutputBuffer, mOutputBufferSize));
}

bool cWiiMote::ConnectToDevice(int index)
{
	Init();

	const bool retval = mHIDDevice.Connect(mDeviceID,mVendorID,index) && 
						SetReportMode(REPORT_MODE_EVENT_BUTTONS) && 
						UpdateOutput() &&
						ReadCalibrationData();


	if (retval)
	{
        InitExpansion();
	}
	return retval;
}

bool cWiiMote::Disconnect()
{
	bool retval = false;
	StopDataStream();

	if (mHIDDevice.IsConnected())
	{
		retval = mHIDDevice.Disconnect();
	}
	return retval;
}

bool cWiiMote::SetVibration(bool vib_on)
{
	bool retval = true;

	//activate vibration only if no balance board is found
	if (mDeviceType != MAIN_DEV_BALANCE)
	{
		if (mOutputControls.mVibration != vib_on)
		{
			mOutputControls.mVibration = vib_on;
			retval = UpdateOutput();
		}
	}
	else
	{
		retval = false;
	}
	return retval;
}

void cWiiMote::ClearBuffer()
{
	memset(mOutputBuffer,0, mOutputBufferSize);
}

bool cWiiMote::SetLEDs(bool led1, bool led2, bool led3, bool led4)
{
	//balance board only supports LED1
	if (mDeviceType != MAIN_DEV_BALANCE)
	{
		const bool no_change = 	mOutputControls.mLED1 == led1 &&
								mOutputControls.mLED2 == led2 &&
								mOutputControls.mLED3 == led3 &&
								mOutputControls.mLED4 == led4;
		if (no_change)
		{
			return true;
		}
	}
	else
	{
		const bool no_change = 	mOutputControls.mLED1 == led1;
		if (no_change)
		{
			return true;
		}
	}
	

	mOutputControls.mLED1 = led1;
	if (mDeviceType != MAIN_DEV_BALANCE)
	{
		mOutputControls.mLED2 = led2;	
		mOutputControls.mLED3 = led3;
		mOutputControls.mLED4 = led4;
	}
	else
	{
		mOutputControls.mLED2 = false;	
		mOutputControls.mLED3 = false;
		mOutputControls.mLED4 = false;
	}
	return UpdateOutput();
}

bool cWiiMote::UpdateOutput()
{
	ClearBuffer();
	mOutputBuffer[0] = OUTPUT_CHANNEL_LED;
	mOutputBuffer[1] =  (mOutputControls.mVibration ? 0x1 : 0x0) |
						(mOutputControls.mLED1 ? 0x1 : 0x0) << 4 | 
						(mOutputControls.mLED2 ? 0x1 : 0x0) << 5 | 
						(mOutputControls.mLED3 ? 0x1 : 0x0) << 6 | 
						(mOutputControls.mLED4 ? 0x1 : 0x0) << 7; 

	return mHIDDevice.WriteToDevice(mOutputBuffer,mOutputBufferSize);
}

bool cWiiMote::HeartBeat(int timeout)
{
	bool retval = true;
	int bytes_read = 0;

	if (mHIDDevice.ReadFromDevice(mInputBuffer,mInputBufferSize,bytes_read) && bytes_read > 0,timeout)
	{
		const int channel = mInputBuffer[0];
		switch (channel)
		{

			case INPUT_CHANNEL_EXPANSION_PORT:   // 0x20 Status Information
                                ParseButtonReport(&mInputBuffer[1]);
				ParseExpansionReport(&mInputBuffer[3]);

                //if report is only request for reading battery level, don't restart datastream
				if (mDataStreamRunning && !mRequestBattery)
				{
                    StopDataStream();
                    InitExpansion();
                    retval = StartDataStream();
                }
                mRequestBattery = false;
			    break;

			case INPUT_CHANNEL_READ_DATA:   // 0x21 Read Data
				ParseButtonReport(&mInputBuffer[1]);
				ParseReadData(&mInputBuffer[3]);
				break;

            /* not yet implemented */
			case INPUT_CHANNEL_WRITE_CONFIRM:    // 0x22 Write Data
			    break;

			case INPUT_CHANNEL_BUTTONS_ONLY: // 0x30 Core Buttons
				ParseButtonReport(&mInputBuffer[1]);
			    break;

			case INPUT_CHANNEL_BUTTONS_MOTION:   // 0x31 Core Buttons and Accelerometer
				ParseButtonReport(&mInputBuffer[1]);
				ParseMotionReport(&mInputBuffer[3]);
			    break;

			case INPUT_CHANNEL_MOTION_IR:    // 0x33 Core Buttons and Accelerometer with 12 IR bytes
				ParseButtonReport(&mInputBuffer[1]);
				ParseMotionReport(&mInputBuffer[3]);
				ParseIRReport(&mInputBuffer[6], IR_MODE_EXT);
			    break;

			case INPUT_CHANNEL_MOTION_IR_EXT6:  // 0x37 Core Buttons and Accelerometer with 10 IR bytes and 6 Extension Bytes
				ParseButtonReport(&mInputBuffer[1]);
				ParseMotionReport(&mInputBuffer[3]);
				ParseIRReport(&mInputBuffer[6], IR_MODE_STD);
				ParseExpansionDevice(&mInputBuffer[16]);
			    break;

			case INPUT_CHANNEL_MOTION_EXT16: // 0x35 Core Buttons and Accelerometer with 16 Extension Bytes
				ParseButtonReport(&mInputBuffer[1]);
				ParseMotionReport(&mInputBuffer[3]);
				ParseExpansionDevice(&mInputBuffer[6]);
			    break;

			case INPUT_CHANNEL_BUTTONS_EXT8:   // 0x32 Core Buttons with 8 Extension bytes
				ParseButtonReport(&mInputBuffer[1]);
				ParseExpansionDevice(&mInputBuffer[3]);
			    break;

			case INPUT_CHANNEL_BUTTONS_EXT19:   // 0x34 Core Buttons with 19 Extension bytes
				ParseButtonReport(&mInputBuffer[1]);
				ParseExpansionDevice(&mInputBuffer[3]);
			    break;

            case INPUT_CHANNEL_BUTTONS_IR_EXT9:  // 0x36 Core Buttons with 10 IR bytes and 9 Extension Bytes
				ParseButtonReport(&mInputBuffer[1]);
				ParseIRReport(&mInputBuffer[3], IR_MODE_STD);
				ParseExpansionDevice(&mInputBuffer[13]);
                break;

            /* not yet implemented */
			case 0x3d:   // 21 Extension Bytes from extension controller
                break;
            
            /* not yet implemented */
			case 0x3e:   // Interleaved Core Buttons and Accelerometer with 36 IR bytes
			case 0x3f:
                /* Both 0x3e and 0x3f are equivalent, and return data alternately
                through report IDs 0x3e and 0x3f. The data is interleaved, and
                is returned at half the speed of other modes (as two reports are
                needed for a single data unit). This mode returns data from the
                buttons, accelerometer, and IR camera in the Wiimote */
				ParseButtonReport(&mInputBuffer[1]);
			    break;
			
			default:
                retval = false;
				//unknown report
			    break;


		}
		
	}

	return retval;
}

void cWiiMote::ParseExpansionReport(const unsigned char *data)
{
	//four bytes long
	mLastExpansionReport.mAttachmentPluggedIn = (data[0] & 0x02) != 0;
	mLastExpansionReport.mIREnabled = (data[0] & 0x08) != 0;
	mLastExpansionReport.mSpeakerEnabled = (data[0] & 0x04) != 0;
	mLastExpansionReport.mLED1On = (data[0] & 0x10) != 0;
	mLastExpansionReport.mLED2On = (data[0] & 0x20) != 0;
	mLastExpansionReport.mLED3On = (data[0] & 0x40) != 0;
	mLastExpansionReport.mLED4On = (data[0] & 0x80) != 0;
	
	//battery level
	mLastExpansionReport.mBatteryLevel = data[3];
}

void cWiiMote::ParseButtonReport(const unsigned char * data)
{
	//two bytes long

	//Balance Board does not report complete button data
	if (mDeviceType != MAIN_DEV_BALANCE)
	{
		mLastButtonStatus.mA     = (data[1] & 0x08) != 0;
 		mLastButtonStatus.mB     = (data[1] & 0x04) != 0;
 		mLastButtonStatus.m1     = (data[1] & 0x02) != 0;
 		mLastButtonStatus.m2     = (data[1] & 0x01) != 0;
 		mLastButtonStatus.mPlus  = (data[0] & 0x10) != 0;
 		mLastButtonStatus.mMinus = (data[1] & 0x10) != 0;
 		mLastButtonStatus.mHome  = (data[1] & 0x80) != 0;
 		mLastButtonStatus.mUp    = (data[0] & 0x08) != 0;
 		mLastButtonStatus.mDown  = (data[0] & 0x04) != 0;
 		mLastButtonStatus.mLeft  = (data[0] & 0x01) != 0;
 		mLastButtonStatus.mRight = (data[0] & 0x02) != 0;
	}
	else
	{
		mLastButtonStatus.mA     = (data[1] & 0x08) != 0; //balance board maps to 'A' button
 		mLastButtonStatus.mB     = false;
 		mLastButtonStatus.m1     = false;
 		mLastButtonStatus.m2     = false;
 		mLastButtonStatus.mPlus  = false;
 		mLastButtonStatus.mMinus = false;
 		mLastButtonStatus.mHome  = false;
 		mLastButtonStatus.mUp    = false;
 		mLastButtonStatus.mDown  = false;
 		mLastButtonStatus.mLeft  = false;
 		mLastButtonStatus.mRight = false;
	}
}

void cWiiMote::ParseMotionReport(const unsigned char * data)
{
	//three bytes long
	mLastMotionReport.mX = data[0];
	mLastMotionReport.mY = data[1];
	mLastMotionReport.mZ = data[2];
}

bool cWiiMote::RequestExpansionReport()
{
    bool retval;

    //only restart datastream if it's not yet running
    if (mDataStreamRunning)
    	retval = true;
    else
        retval = StartDataStream();

    if (retval)
    {
    	ClearBuffer();
        mOutputBuffer[0] = OUTPUT_REQUEST_EXPANSION;
		mOutputBuffer[1] = (mOutputControls.mVibration ? 0x1 : 0x0);
        retval = mHIDDevice.WriteToDevice(mOutputBuffer, mOutputBufferSize);
    }
	return retval;
} 

#ifdef PRINT_STATUS
#include <stdio.h>
void cWiiMote::PrintStatus() /*const*/
{
	float wX,wY,wZ;
	float wP,wT,wR = 0.f;
	float cX,cY,cZ;
	float sX,sY;
	float cP,cT,cR = 0.f;
	float irX,irY;
    float sLX, sLY, sRX, sRY, aL, aR;
    float pTL, pTR, pBL, pBR;
	
	wX=wY=wZ=cX=cY=cZ=sX=sY=sLX=sLY=sRX=sRY=aL=aR=pTL=pTR=pBL=pBR=irX=irY=0.f;

	if (mDeviceType == MAIN_DEV_WIIMOTE)
	{
		//standard Wiimote report
		GetCalibratedAcceleration(wX,wY,wZ);
		printf("W:[%+1.2f %+1.2f %+1.2f] ",wX,wY,wZ);

    	GetOrientation(wP,wT,wR);
		printf("WAng:[%+1.2f %+1.2f %+1.2f] ",wP*360/(2*PI),wT*360/(2*PI),wR*360/(2*PI));
	}
	else
	{
		//Balance Board report
		GetCalibratedBalanceBoardSensors(pTL, pTR, pBL, pBR);
		printf("BPressure:[%+1.2f %+1.2f %+1.2f %+1.2f] Weight[%1.2f]",pTL,pTR,pBL,pBR,pTL+pTR+pBL+pBR);		
	}
	
	if (mNunchuckAttached)
	{
		GetCalibratedChuckAcceleration(cX,cY,cZ);
		printf("N:[%+1.2f %+1.2f %+1.2f] ",cX,cY,cZ);

		GetCalibratedChuckStick(sX,sY);
		printf("S:[%+1.2f %+1.2f] ",sX,sY);

		GetChukOrientation(cP,cT,cR);
	    printf("NAng:[%+1.2f %+1.2f %+1.2f] ",cP*360/(2*PI),cT*360/(2*PI),cR*360/(2*PI));
	}

	if (mClassicAttached)
	{
		GetCalibratedLeftClassicStick(sLX,sLY);
		printf("SL:[%+1.2f %+1.2f] ",sLX,sLY);
		GetCalibratedRightClassicStick(sRX,sRY);
		printf("SR:[%+1.2f %+1.2f] ",sRX,sRY);
        GetCalibratedLeftClassicShoulder(aL);
        GetCalibratedRightClassicShoulder(aR);
		printf("A:[%+1.2f %+1.2f] ",aL,aR);
	}

	if (mGuitarAttached)
	{
		GetCalibratedGuitarStick(sLX,sLY);
		printf("G:[%+1.2f %+1.2f] ",sLX,sLY);
        GetCalibratedGuitarAnalog(aL);
		printf("A:[%+1.2f] ",aL);
	}

	if (mIRRunning)
	{
		if (GetIRP(irX,irY, 0))
		{
			printf("P1:[%+1.2f %+1.2f]",irX,irY);
		}
		if (GetIRP(irX,irY,1))
		{
			printf("P2:[%+1.2f %+1.2f]",irX,irY);
		}
	}


	//print the button status
	if (mLastButtonStatus.m1)
		printf("1");
	if (mLastButtonStatus.m2)
		printf("2");
	if (mLastButtonStatus.mA)
		printf("A");
	if (mLastButtonStatus.mB)
		printf("B");
	if (mLastButtonStatus.mPlus)
		printf("+");
	if (mLastButtonStatus.mMinus)
		printf("-");
	if (mLastButtonStatus.mUp)
		printf("U");
	if (mLastButtonStatus.mDown)
		printf("D");
	if (mLastButtonStatus.mLeft)
		printf("L");
	if (mLastButtonStatus.mRight)
		printf("R");
	if (mLastButtonStatus.mHome)
		printf("H");

	if (mNunchuckAttached)
	{
		if (mLastChuckReport.mButtonZ)
			printf("Z");
		if (mLastChuckReport.mButtonC)
			printf("C");
	}

    if (mClassicAttached)
    {
	   if (mLastClassicReport.mL)
	       printf("L");
	   if (mLastClassicReport.mR)
	       printf("R");
	   if (mLastClassicReport.mZL)
	       printf("Zl");
	   if (mLastClassicReport.mZR)
	       printf("Zr");
	   if (mLastClassicReport.mX)
	       printf("X");
	   if (mLastClassicReport.mY)
	       printf("Y");
	   if (mLastClassicReport.mA)
	       printf("A");
	   if (mLastClassicReport.mB)
	       printf("B");
	   if (mLastClassicReport.mPlus)
	       printf("+");
	   if (mLastClassicReport.mMinus)
	       printf("-");
	   if (mLastClassicReport.mUp)
	       printf("u");
	   if (mLastClassicReport.mDown)
	       printf("d");
	   if (mLastClassicReport.mLeft)
	       printf("l");
	   if (mLastClassicReport.mRight)
	       printf("r");
	   if (mLastClassicReport.mHome)
	       printf("H");
    }

    if (mGuitarAttached)
    {
	   if (mLastGuitarReport.mG)
	       printf("G");
	   if (mLastGuitarReport.mR)
	       printf("R");
	   if (mLastGuitarReport.mY)
	       printf("Y");
	   if (mLastGuitarReport.mB)
	       printf("B");
	   if (mLastGuitarReport.mO)
	       printf("O");
	   if (mLastGuitarReport.mPlus)
	       printf("+");
	   if (mLastGuitarReport.mMinus)
	       printf("-");
	   if (mLastGuitarReport.mUp)
	       printf("u");
	   if (mLastGuitarReport.mDown)
	       printf("d");
    }
	printf("\n");

}
#endif

bool cWiiMote::SelectInputChannel(bool continuous, unsigned char channel)
{
	ClearBuffer();
	mOutputBuffer[0] = OUTPUT_CHANNEL_REPORT;
	mOutputBuffer[1] = (continuous ? REQUEST_CONTINUOUS_REPORTS : REQUEST_SINGLE_REPORTS) | (mOutputControls.mVibration ? 0x1 : 0x0);
	mOutputBuffer[2] = channel;
	return mHIDDevice.WriteToDevice(mOutputBuffer,mOutputBufferSize);
}


//this may or may not work to read buffers greater than 16 bytes. . . .
bool cWiiMote::IssueReadRequest(unsigned int address, unsigned short size, unsigned char * buffer)
{
	bool retval = false;
	if (mReadInfo.mReadStatus != tMemReadInfo::READ_PENDING)
	{
		ClearBuffer();
		mOutputBuffer[0] = OUTPUT_READ_MEMORY;
		mOutputBuffer[1] = (((address & 0xff000000) >> 24) & 0xFE) | (mOutputControls.mVibration ? 0x1 : 0x0);
		mOutputBuffer[2] = (address & 0x00ff0000) >> 16;
		mOutputBuffer[3] = (address & 0x0000ff00) >> 8;
		mOutputBuffer[4] = (address & 0xff);
		
		mOutputBuffer[5] = (size & 0xff00) >> 8;
		mOutputBuffer[6] = (size & 0xff);
		
		if (mHIDDevice.WriteToDevice(mOutputBuffer,mOutputBufferSize))
		{
			mReadInfo.mReadStatus = tMemReadInfo::READ_PENDING;
			mReadInfo.mReadBuffer = buffer;
			mReadInfo.mTotalBytesToRead = size;
			mReadInfo.mBytesRead = 0;
			mReadInfo.mBaseAddress = (unsigned short)(address & 0xFFFF);
			retval = true;
		}
	}

	return retval;
}

void cWiiMote::ParseReadData(const unsigned char * data)
{
	if(mReadInfo.mReadStatus == tMemReadInfo::READ_PENDING)
	{
		const bool error = (data[0] & 0x0F) != 0;
		if (error)
		{
			mReadInfo.mReadStatus = tMemReadInfo::READ_ERROR;
		}
		else
		{
			unsigned char bytes = (data[0] >> 4)+1;
			unsigned short offset = ((unsigned short)data[1] << 8) + data[2];
			unsigned int space_left_in_buffer = mReadInfo.mTotalBytesToRead -  mReadInfo.mBytesRead;
			if (offset == mReadInfo.mBytesRead + mReadInfo.mBaseAddress &&
				space_left_in_buffer >= bytes)
			{
				memcpy(&mReadInfo.mReadBuffer[mReadInfo.mBytesRead],&data[3],bytes);
				
				mReadInfo.mBytesRead+= bytes;
				if (mReadInfo.mBytesRead >= mReadInfo.mTotalBytesToRead)
				{
					mReadInfo.mReadStatus = tMemReadInfo::READ_COMPLETE;
				}
			}
		}
	}

}

bool cWiiMote::ReadData(unsigned int address, unsigned short size, unsigned char * buffer)
{

	if (IssueReadRequest(address, size,buffer))
	{
		while (mReadInfo.mReadStatus == tMemReadInfo::READ_PENDING)
		{
			if (!HeartBeat(1000))
			{
				break;
			}
		}
	}
	return mReadInfo.mReadStatus == tMemReadInfo::READ_COMPLETE;
}

bool cWiiMote::ReadCalibrationData()
{
	bool retval = false;
	unsigned char buffer[CALIBRATION_DATA_LENGTH];
	//Balance Board does not carry calibration data here
	if (mDeviceType != MAIN_DEV_BALANCE)
	{
		if (ReadData(CALIBRATION_ADDRESS, CALIBRATION_DATA_LENGTH,buffer))
		{
			mAccelCalibrationData.mXZero = buffer[0];
			mAccelCalibrationData.mYZero = buffer[1];
			mAccelCalibrationData.mZZero = buffer[2];
			mAccelCalibrationData.mXG    = buffer[4];
			mAccelCalibrationData.mYG    = buffer[5];
			mAccelCalibrationData.mZG    = buffer[6];
			retval = true;
		}		
	}
	else
	{
		retval = true;
	}
	
	return retval;
}

void cWiiMote::GetCalibratedAcceleration(float & x, float & y, float &z) const
{
 	//Balance Board does not deliver angle and force information
 	if (mDeviceType == MAIN_DEV_BALANCE)
 	{
		x = y = z = 0.0f;
		return;
	}

 	x = (mLastMotionReport.mX - mAccelCalibrationData.mXZero) / (float)(mAccelCalibrationData.mXG- mAccelCalibrationData.mXZero);
	y = (mLastMotionReport.mY - mAccelCalibrationData.mYZero) / (float)(mAccelCalibrationData.mYG- mAccelCalibrationData.mYZero);
	z = (mLastMotionReport.mZ - mAccelCalibrationData.mZZero) / (float)(mAccelCalibrationData.mZG- mAccelCalibrationData.mZZero);
}

void cWiiMote::GetOrientation(float & pan, float & tilt, float & roll) /*const*/
{
	float x, y, z;

 	//Balance Board does not deliver angle and force information
 	if (mDeviceType == MAIN_DEV_BALANCE)
 	{
		pan = tilt = roll = 0.0f;
		return;
	}
	//pan cannot be determined through acclereration data
	pan = 0;
	GetCalibratedAcceleration(x, y, z);

	//remote is most likely not accelerated
	if (VECLEN(x,y,z) <= 1.05)
    {
    	//limit accleration values to avoid undefined results
		x = CLAMP(x, -1, 1);
        y = CLAMP(y, -1, 1);
        z = CLAMP(z, -1, 1);

		roll = atan2f(x, sqrt(y*y + z*z));
		tilt = atan2f(y, sqrt(x*x + z*z));
		if(z < 0) 
        {
			tilt = (y < 0) ? -PI - tilt :  PI - tilt;
			roll = (x < 0) ? -PI - roll :  PI - roll;
		}
		
        //simple angle smoothing - don't smooth over angle differences > 45°
		if (fabsf(mLastAngle.mTilt - tilt) < PI/4.0f)
        	tilt = (1.0f - mSmoothFac) * tilt + mSmoothFac * mLastAngle.mTilt;
		if (fabsf(mLastAngle.mRoll - roll) < PI/4.0f)
	        roll = (1.0f - mSmoothFac) * roll + mSmoothFac * mLastAngle.mRoll;

        mLastAngle.mPan  = pan;
        mLastAngle.mTilt = tilt;
        mLastAngle.mRoll = roll;
    }
    else
    //remote is accelerating - use old values
    {
    	pan  = mLastAngle.mPan;
        tilt = mLastAngle.mTilt;
        roll = mLastAngle.mRoll;
	}
}

void cWiiMote::GetChukOrientation(float & pan, float & tilt, float & roll) /*const*/
{
	float x, y, z;
    
 	if (!mNunchuckAttached)
	{
		pan = tilt = roll = 0.f;
		return;
	}
    //pan cannot be determined through acclereration data
    pan = 0;
    GetCalibratedChuckAcceleration(x, y, z);

    //Nunchuk is most likely not accelerated
    if (VECLEN(x,y,z) <= 1.05)
    {
    	//limit accleration values to avoid undefined results
        x = CLAMP(x,-1,1);
        y = CLAMP(y,-1,1);
        z = CLAMP(z,-1,1);

		roll = atan2f(x, sqrt(y*y + z*z));
		tilt = atan2f(y, sqrt(x*x + z*z));
		if(z < 0) 
        {
			tilt = (y < 0) ? -PI - tilt :  PI - tilt;
			roll = (x < 0) ? -PI - roll :  PI - roll;
		}
		
        //simple angle smoothing - don't smooth over angle differences > 45°
		if (fabsf(mLastChukAngle.mTilt - tilt) < PI/4.0f)
        	tilt = (1.0f - mSmoothFac) * tilt + mSmoothFac * mLastChukAngle.mTilt;
		if (fabsf(mLastChukAngle.mRoll - roll) < PI/4.0f)
	        roll = (1.0f - mSmoothFac) * roll + mSmoothFac * mLastChukAngle.mRoll;

        mLastChukAngle.mPan  = pan;
        mLastChukAngle.mTilt = tilt;
        mLastChukAngle.mRoll = roll;
    }
    else
    //remote is accelerating - use old values
    {
        pan =  mLastChukAngle.mPan;
        tilt = mLastChukAngle.mTilt;
        roll = mLastChukAngle.mRoll;
    }
}

void cWiiMote::GetCalibratedChuckAcceleration(float & x, float & y, float &z) const
{
	if (!mNunchuckAttached)
	{
		x = y = z = 0.f;
		return;
	}

	x = (mLastChuckReport.mAccelX - mNunchuckAccelCalibrationData.mXZero) / (float)(mNunchuckAccelCalibrationData.mXG- mNunchuckAccelCalibrationData.mXZero);
	y = (mLastChuckReport.mAccelY - mNunchuckAccelCalibrationData.mYZero) / (float)(mNunchuckAccelCalibrationData.mYG- mNunchuckAccelCalibrationData.mYZero);
	z = (mLastChuckReport.mAccelZ - mNunchuckAccelCalibrationData.mZZero) / (float)(mNunchuckAccelCalibrationData.mZG- mNunchuckAccelCalibrationData.mZZero);
}

void cWiiMote::GetCalibratedChuckStick(float & x, float & y) const
{
	if (!mNunchuckAttached)
	{
		x = y = 0.f;
		return;
	}

	if (mLastChuckReport.mStickX < mNunchuckStickCalibrationData.mXmid)
	{
		x = ((mLastChuckReport.mStickX - mNunchuckStickCalibrationData.mXmin) / (float)(mNunchuckStickCalibrationData.mXmid - mNunchuckStickCalibrationData.mXmin)) -  1.f;
	}
	else
	{
		x = ((mLastChuckReport.mStickX - mNunchuckStickCalibrationData.mXmid) / (float)(mNunchuckStickCalibrationData.mXmax - mNunchuckStickCalibrationData.mXmid));
	}

	if (mLastChuckReport.mStickY < mNunchuckStickCalibrationData.mYmid)
	{
		y = ((mLastChuckReport.mStickY - mNunchuckStickCalibrationData.mYmin) / (float)(mNunchuckStickCalibrationData.mYmid - mNunchuckStickCalibrationData.mYmin)) -  1.f;
	}
	else
	{
		y = ((mLastChuckReport.mStickY - mNunchuckStickCalibrationData.mYmid) / (float)(mNunchuckStickCalibrationData.mYmax - mNunchuckStickCalibrationData.mYmid));
	}
}

void cWiiMote::GetCalibratedLeftClassicStick(float & x, float & y) const
{
	if (!mClassicAttached)
	{
		x = y = 0.f;
		return;
	}

	if (mLastClassicReport.mLeftStickX  < mClassicLeftStickCalibrationData.mXmid)
	{
		x = ((mLastClassicReport.mLeftStickX - mClassicLeftStickCalibrationData.mXmin) / (float)(mClassicLeftStickCalibrationData.mXmid - mClassicLeftStickCalibrationData.mXmin)) -  1.f;
	}
	else
	{
		x = ((mLastClassicReport.mLeftStickX - mClassicLeftStickCalibrationData.mXmid) / (float)(mClassicLeftStickCalibrationData.mXmax - mClassicLeftStickCalibrationData.mXmid));
	}

	if (mLastClassicReport.mLeftStickY < mClassicLeftStickCalibrationData.mYmid)
	{
		y = ((mLastClassicReport.mLeftStickY - mClassicLeftStickCalibrationData.mYmin) / (float)(mClassicLeftStickCalibrationData.mYmid - mClassicLeftStickCalibrationData.mYmin)) -  1.f;
	}
	else
	{
		y = ((mLastClassicReport.mLeftStickY - mClassicLeftStickCalibrationData.mYmid) / (float)(mClassicLeftStickCalibrationData.mYmax - mClassicLeftStickCalibrationData.mYmid));
	}
}

void cWiiMote::GetCalibratedRightClassicStick(float & x, float & y) const
{
	if (!mClassicAttached)
	{
		x = y = 0.f;
		return;
	}

	if (mLastClassicReport.mRightStickX  < mClassicRightStickCalibrationData.mXmid)
	{
		x = ((mLastClassicReport.mRightStickX - mClassicRightStickCalibrationData.mXmin) / (float)(mClassicRightStickCalibrationData.mXmid - mClassicRightStickCalibrationData.mXmin)) -  1.f;
	}
	else
	{
		x = ((mLastClassicReport.mRightStickX - mClassicRightStickCalibrationData.mXmid) / (float)(mClassicRightStickCalibrationData.mXmax - mClassicRightStickCalibrationData.mXmid));
	}

	if (mLastClassicReport.mRightStickX < mClassicRightStickCalibrationData.mYmid)
	{
		y = ((mLastClassicReport.mRightStickY - mClassicRightStickCalibrationData.mYmin) / (float)(mClassicRightStickCalibrationData.mYmid - mClassicRightStickCalibrationData.mYmin)) -  1.f;
	}
	else
	{
		y = ((mLastClassicReport.mRightStickY - mClassicRightStickCalibrationData.mYmid) / (float)(mClassicRightStickCalibrationData.mYmax - mClassicRightStickCalibrationData.mYmid));
	}
}

void cWiiMote::GetCalibratedLeftClassicShoulder(float & b) const
{
	if (!mClassicAttached)
	{
		b = 0.f;
		return;
	}
	
	if (mLastClassicReport.mAnalogL < mClassicShoulderCalibrationData.mMin)
		b = mClassicShoulderCalibrationData.mMin;
	else
	{
		if (mLastClassicReport.mAnalogL > mClassicShoulderCalibrationData.mMax)
			b = mClassicShoulderCalibrationData.mMax;
		else
			b = mLastClassicReport.mAnalogL;
	}
	b = (b - mClassicShoulderCalibrationData.mMin) / (float)(mClassicShoulderCalibrationData.mMax - mClassicShoulderCalibrationData.mMin);
}

void cWiiMote::GetCalibratedRightClassicShoulder(float & b) const
{
	if (!mClassicAttached)
	{
		b = 0.f;
		return;
	}
	
	if (mLastClassicReport.mAnalogR < mClassicShoulderCalibrationData.mMin)
		b = mClassicShoulderCalibrationData.mMin;
	else
	{
		if (mLastClassicReport.mAnalogR > mClassicShoulderCalibrationData.mMax)
			b = mClassicShoulderCalibrationData.mMax;
		else
			b = mLastClassicReport.mAnalogR;
	}
	b = (b - mClassicShoulderCalibrationData.mMin) / (float)(mClassicShoulderCalibrationData.mMax - mClassicShoulderCalibrationData.mMin);
}

void cWiiMote::GetCalibratedGuitarStick(float & x, float & y) const
{
	if (!mGuitarAttached)
	{
		x = y = 0.f;
		return;
	}

	if (mLastGuitarReport.mStickX  < mGuitarStickCalibrationData.mXmid)
	{
		x = ((mLastGuitarReport.mStickX - mGuitarStickCalibrationData.mXmin) / (float)(mGuitarStickCalibrationData.mXmid - mGuitarStickCalibrationData.mXmin)) -  1.f;
	}
	else
	{
		x = ((mLastGuitarReport.mStickX - mGuitarStickCalibrationData.mXmid) / (float)(mGuitarStickCalibrationData.mXmax - mGuitarStickCalibrationData.mXmid));
	}

	if (mLastGuitarReport.mStickY < mGuitarStickCalibrationData.mYmid)
	{
		y = ((mLastGuitarReport.mStickY - mGuitarStickCalibrationData.mYmin) / (float)(mGuitarStickCalibrationData.mYmid - mGuitarStickCalibrationData.mYmin)) -  1.f;
	}
	else
	{
		y = ((mLastGuitarReport.mStickY - mGuitarStickCalibrationData.mYmid) / (float)(mGuitarStickCalibrationData.mYmax - mGuitarStickCalibrationData.mYmid));
	}
}

void cWiiMote::GetCalibratedGuitarAnalog(float & b) const
{
	if (!mGuitarAttached)
	{
		b = 0.f;
		return;
	}
	
	if (mLastGuitarReport.mAnalog < mGuitarAnalogCalibrationData.mMin)
		b = mClassicShoulderCalibrationData.mMin;
	else
	{
		if (mLastGuitarReport.mAnalog > mGuitarAnalogCalibrationData.mMax)
			b = mGuitarAnalogCalibrationData.mMax;
		else
			b = mLastGuitarReport.mAnalog;
	}
	b = (b - mGuitarAnalogCalibrationData.mMin) / (float)(mGuitarAnalogCalibrationData.mMax - mGuitarAnalogCalibrationData.mMin);
}

void cWiiMote::GetCalibratedBalanceBoardSensors(float & pTR, float & pBR, float & pTL, float & pBL) const
{
	if (!mBalanceBoardAttached)
	{
		pTL = pTR = pBL = pBR = 0.f;
		return;
	}

	//balance board sensor values are returned in kg unit
	//calibration data is stored for 0, 17, 34kg --> multiplier of 17 for 17kg steps
	
	// top right sensor
	if (mLastBalanceBoardReport.mTopRight < mBalanceBoardCalibrationData.mTopRight0kg)
	{
		// avoid negative weight
		pTR = 0.0f;
	}
	else if (mLastBalanceBoardReport.mTopRight <= mBalanceBoardCalibrationData.mTopRight17kg)
	{
		pTR = 17.0f * (mLastBalanceBoardReport.mTopRight - mBalanceBoardCalibrationData.mTopRight0kg) /  (float)(mBalanceBoardCalibrationData.mTopRight17kg - mBalanceBoardCalibrationData.mTopRight0kg);
	}
	else
	{
		pTR = 17.0f + 17.0f * (mLastBalanceBoardReport.mTopRight - mBalanceBoardCalibrationData.mTopRight17kg) /  (float)(mBalanceBoardCalibrationData.mTopRight34kg - mBalanceBoardCalibrationData.mTopRight17kg);
	}	

	// bottom right sensor
	if (mLastBalanceBoardReport.mBottomRight < mBalanceBoardCalibrationData.mBottomRight0kg)
	{
		// avoid negative weight
		pBR = 0.0f;
	}
	else if (mLastBalanceBoardReport.mBottomRight <= mBalanceBoardCalibrationData.mBottomRight17kg)
	{
		pBR = 17.0f *(mLastBalanceBoardReport.mBottomRight - mBalanceBoardCalibrationData.mBottomRight0kg) /  (float)(mBalanceBoardCalibrationData.mBottomRight17kg - mBalanceBoardCalibrationData.mBottomRight0kg);
	}
	else
	{
		pBR = 17.0f + 17.0f *(mLastBalanceBoardReport.mBottomRight - mBalanceBoardCalibrationData.mBottomRight17kg) /  (float)(mBalanceBoardCalibrationData.mBottomRight34kg - mBalanceBoardCalibrationData.mBottomRight17kg);
	}	

	// top left sensor
	if (mLastBalanceBoardReport.mTopLeft < mBalanceBoardCalibrationData.mTopLeft0kg)
	{
		// avoid negative weight
		pTL = 0.0f;
	}
	else if (mLastBalanceBoardReport.mTopLeft <= mBalanceBoardCalibrationData.mTopLeft17kg)
	{
		pTL = 17.0f * (mLastBalanceBoardReport.mTopLeft - mBalanceBoardCalibrationData.mTopLeft0kg) /  (float)(mBalanceBoardCalibrationData.mTopLeft17kg - mBalanceBoardCalibrationData.mTopLeft0kg);
	}
	else
	{
		pTL = 17.0f + 17.0f * (mLastBalanceBoardReport.mTopLeft - mBalanceBoardCalibrationData.mTopLeft17kg) /  (float)(mBalanceBoardCalibrationData.mTopLeft34kg - mBalanceBoardCalibrationData.mTopLeft17kg);
	}	

	// bottom left sensor
	if (mLastBalanceBoardReport.mBottomLeft < mBalanceBoardCalibrationData.mBottomLeft0kg)
	{
		// avoid negative weight
		pBL = 0.0f;
	}
	else if (mLastBalanceBoardReport.mBottomLeft <= mBalanceBoardCalibrationData.mBottomLeft17kg)
	{
		pBL = 17.0f * (mLastBalanceBoardReport.mBottomLeft - mBalanceBoardCalibrationData.mBottomLeft0kg) /  (float)(mBalanceBoardCalibrationData.mBottomLeft17kg - mBalanceBoardCalibrationData.mBottomLeft0kg);
	}
	else
	{
		pBL = 17.0f + 17.0f *(mLastBalanceBoardReport.mBottomLeft - mBalanceBoardCalibrationData.mBottomLeft17kg) /  (float)(mBalanceBoardCalibrationData.mBottomLeft34kg - mBalanceBoardCalibrationData.mBottomLeft17kg);
	}	

	//make values insensitive to weights smaller than 1kg
	if (pTR < 1.0f) pTR = 0.0f;
	if (pBR < 1.0f) pBR = 0.0f;
	if (pTL < 1.0f) pTL = 0.0f;
	if (pBL < 1.0f) pBL = 0.0f;
}

bool cWiiMote::WriteMemory(unsigned int address, unsigned char size, const unsigned char * buffer)
{
	bool retval = false;
	if (size <= 16)
	{
		ClearBuffer();
		mOutputBuffer[0] = OUTPUT_WRITE_MEMORY;
		mOutputBuffer[1] = (address & 0xff000000) >> 24 | (mOutputControls.mVibration ? 0x1 : 0x0);
		mOutputBuffer[2] = (address & 0x00ff0000) >> 16;
		mOutputBuffer[3] = (address & 0x0000ff00) >> 8;
		mOutputBuffer[4] = (address & 0xff);
		mOutputBuffer[5] = size;
		memcpy(&mOutputBuffer[6],buffer,size);
		retval = mHIDDevice.WriteToDevice(mOutputBuffer,mOutputBufferSize);
	}

	return retval;
}

bool cWiiMote::InitExpansion()
{
    bool retval = false;
	unsigned char buffer[2];

    WriteMemory(EXPANSION_INIT_ADDRESS,1,&EXPANSION_INIT_VAL);
    Sleep(20);

    mNunchuckAttached = false;
    mClassicAttached = false;
	mGuitarAttached = false;
	// don't reset balance board flag in case of a balance board is attached
	// this is necessary since balance board always has an expansion device connected
	// resetting this flag for a short time can lead to false recognition of the device as Wiimote
	if (mDeviceType != MAIN_DEV_BALANCE)
	{
		mBalanceBoardAttached = false;
	}

    if (ReadData(EXPANSION_STATUS_ADDRESS,2,buffer))
    {
    	mExpansionType = (buffer[1] << 8) | buffer[0];
#ifdef DEBUG
printf ("Expansion Type: 0x%0x\n", mExpansionType); 
#endif
 		// main device type is also evaluated here because it is retrieved from the expansion device type
        switch(mExpansionType)
        {
            case EXP_DEV_NUNCHUK:
                retval = InitNunchuck();
				mDeviceType = MAIN_DEV_WIIMOTE;
                break;

            case EXP_DEV_CLASSIC:
                retval = InitClassic();
				mDeviceType = MAIN_DEV_WIIMOTE;
                break;

            case EXP_DEV_GUITAR:
				retval = InitGuitar();
				mDeviceType = MAIN_DEV_WIIMOTE;
				break;
				
			case EXP_DEV_BALANCE:
				retval = InitBalanceBoard();
				mDeviceType = MAIN_DEV_BALANCE;
				break;
					
            case EXP_DEV_NONE:
                retval = false;
				mDeviceType = MAIN_DEV_WIIMOTE;
                break;

            case EXP_DEV_ERROR:
            default:
				retval = false;
				mDeviceType = MAIN_DEV_UNKNOWN;
                break;
        }
    }
    return retval;
}

void cWiiMote::ParseExpansionDevice(const unsigned char * data)
{
    switch(mExpansionType)
    {

        case EXP_DEV_NUNCHUK:
            ParseChuckReport(data);
            break;

        case EXP_DEV_CLASSIC:
            ParseClassicReport(data);
            break;

        case EXP_DEV_GUITAR:
			ParseGuitarReport(data);
			break;
			
		case EXP_DEV_BALANCE:
			ParseBalanceBoardReport(data);
			break;
			
        case EXP_DEV_NONE:
        case EXP_DEV_ERROR:
            break;

        default:
            break;
    }
}

bool cWiiMote::InitNunchuck()
{

	bool retval = false;
	
	//init nunchuck, if it is present

    unsigned char buffer[16];
    ReadData(EXPANSION_CALIBRATION_ADDRESS,16,buffer);   //dummy read -- ??
    //now try to read the nunchuck's calibration data
    if (ReadData(EXPANSION_CALIBRATION_ADDRESS,16,buffer))
    {
        mNunchuckAccelCalibrationData.mXZero = ExpansionByte(buffer[0]);
		mNunchuckAccelCalibrationData.mYZero = ExpansionByte(buffer[1]);
		mNunchuckAccelCalibrationData.mZZero = ExpansionByte(buffer[2]);

		mNunchuckAccelCalibrationData.mXG = ExpansionByte(buffer[4]);
		mNunchuckAccelCalibrationData.mYG = ExpansionByte(buffer[5]);
		mNunchuckAccelCalibrationData.mZG = ExpansionByte(buffer[6]);

		mNunchuckStickCalibrationData.mXmax = ExpansionByte(buffer[8]);
		mNunchuckStickCalibrationData.mXmin = ExpansionByte(buffer[9]);
		mNunchuckStickCalibrationData.mXmid = ExpansionByte(buffer[10]);
		mNunchuckStickCalibrationData.mYmax = ExpansionByte(buffer[11]);
		mNunchuckStickCalibrationData.mYmin = ExpansionByte(buffer[12]);
		mNunchuckStickCalibrationData.mYmid = ExpansionByte(buffer[13]);

		retval = true;

    }
	mNunchuckAttached = retval;
	return retval;
}

bool cWiiMote::InitClassic()
{

	bool retval = false;
	
	//init classic controller, if it is present

	unsigned char buffer[16];
    ReadData(EXPANSION_CALIBRATION_ADDRESS,16,buffer);   //dummy read -- ??
	//now try to read the classic controller's calibration data
	if (ReadData(EXPANSION_CALIBRATION_ADDRESS,16,buffer))
	{
#ifdef DEBUG
        printf("Cal Classic [%1.0f] [%1.0f] [%1.0f] [%1.0f]\n", (float)ExpansionByte(buffer[12]), (float)ExpansionByte(buffer[13]), (float)ExpansionByte(buffer[14]), (float)ExpansionByte(buffer[15]));
        printf("Cal Classic [%1.0f] [%1.0f] [%1.0f] [%1.0f]\n", (float)ExpansionByte(buffer[12])/8, (float)ExpansionByte(buffer[13])/8, (float)ExpansionByte(buffer[14])/8, (float)ExpansionByte(buffer[15])/8);
        printf("Cal Classic [%0x] [%0x] [%0x] [%0x]\n", ExpansionByte(buffer[12]), ExpansionByte(buffer[13]), ExpansionByte(buffer[14]), ExpansionByte(buffer[15]));
#endif
        mClassicLeftStickCalibrationData.mXmax = ExpansionByte(buffer[0]);
        mClassicLeftStickCalibrationData.mXmin = ExpansionByte(buffer[1]);
		mClassicLeftStickCalibrationData.mXmid = ExpansionByte(buffer[2]);
		mClassicLeftStickCalibrationData.mYmax = ExpansionByte(buffer[3]);
		mClassicLeftStickCalibrationData.mYmin = ExpansionByte(buffer[4]);
		mClassicLeftStickCalibrationData.mYmid = ExpansionByte(buffer[5]);

		mClassicRightStickCalibrationData.mXmax = ExpansionByte(buffer[6]);
		mClassicRightStickCalibrationData.mXmin = ExpansionByte(buffer[7]);
		mClassicRightStickCalibrationData.mXmid = ExpansionByte(buffer[8]);
		mClassicRightStickCalibrationData.mYmax = ExpansionByte(buffer[9]);
		mClassicRightStickCalibrationData.mYmin = ExpansionByte(buffer[10]);
		mClassicRightStickCalibrationData.mYmid = ExpansionByte(buffer[11]);

        //this might or might not be right...
		mClassicShoulderCalibrationData.mMax = ExpansionByte(buffer[15]);
		mClassicShoulderCalibrationData.mMin = ExpansionByte(buffer[13]);
		retval = true;

	}
	mClassicAttached = retval;
	return retval;
}

bool cWiiMote::InitGuitar()
{

	bool retval = false;
	
	//init guitar controller, if it is present

	unsigned char buffer[16];
    ReadData(EXPANSION_CALIBRATION_ADDRESS,16,buffer);   //dummy read -- ??
	//now try to read the guitar controller's calibration data
	if (ReadData(EXPANSION_CALIBRATION_ADDRESS,16,buffer))
	{
#ifdef DEBUG
        printf("Cal Guitar [%1.0f] [%1.0f] [%1.0f]\n", (float)ExpansionByte(buffer[0]), (float)ExpansionByte(buffer[1]), (float)ExpansionByte(buffer[2]));
        printf("Cal Guitar [%1.0f] [%1.0f] [%1.0f]\n", (float)ExpansionByte(buffer[3]), (float)ExpansionByte(buffer[4]), (float)ExpansionByte(buffer[5]));
        printf("Cal Guitar [%1.0f] [%1.0f] [%1.0f]\n", (float)ExpansionByte(buffer[6]), (float)ExpansionByte(buffer[7]), (float)ExpansionByte(buffer[8]));
        printf("Cal Guitar [%1.0f] [%1.0f] [%1.0f]\n", (float)ExpansionByte(buffer[9]), (float)ExpansionByte(buffer[10]), (float)ExpansionByte(buffer[11]));
        printf("Cal Guitar [%1.0f] [%1.0f] [%1.0f]\n", (float)ExpansionByte(buffer[12]), (float)ExpansionByte(buffer[13]), (float)ExpansionByte(buffer[14]));
#endif
		//guitar controller doesn't seem to return calibration data - use default values
		//guitar analog values behave very strange and don't make proper use of full 8 bit resolution
        mGuitarStickCalibrationData.mXmax = 0xFC;//ExpansionByte(buffer[0]);
        mGuitarStickCalibrationData.mXmin = 0xC3;//ExpansionByte(buffer[1]);
		mGuitarStickCalibrationData.mXmid = 0xE0;//ExpansionByte(buffer[2]);
		mGuitarStickCalibrationData.mYmax = 0xFC;//ExpansionByte(buffer[3]);
		mGuitarStickCalibrationData.mYmin = 0xC3;//ExpansionByte(buffer[4]);
		mGuitarStickCalibrationData.mYmid = 0xE0;//ExpansionByte(buffer[5]);

        //this might or might not be right...
		mGuitarAnalogCalibrationData.mMax = 0xF9;//ExpansionByte(buffer[15]);
		mGuitarAnalogCalibrationData.mMin = 0xEF;//ExpansionByte(buffer[13]);
		retval = true;

	}
	mGuitarAttached = retval;
	return retval;
}

bool cWiiMote::InitBalanceBoard()
{

	bool retval = false;
	
	//init balance board, if it is present

	unsigned char buffer[24];
    ReadData(BALANCEBOARD_CALIBRATION_ADDRESS,24,buffer);   //dummy read -- ??
	//now try to read the balance board's calibration data
	if (ReadData(BALANCEBOARD_CALIBRATION_ADDRESS,24,buffer))
	{
		mBalanceBoardCalibrationData.mTopRight0kg     = buffer[1]  | (buffer[0]  << 8);
		mBalanceBoardCalibrationData.mBottomRight0kg  = buffer[3]  | (buffer[2]  << 8);
		mBalanceBoardCalibrationData.mTopLeft0kg      = buffer[5]  | (buffer[4]  << 8);
		mBalanceBoardCalibrationData.mBottomLeft0kg   = buffer[7]  | (buffer[6]  << 8);
		
		mBalanceBoardCalibrationData.mTopRight17kg    = buffer[9]  | (buffer[8]  << 8);
		mBalanceBoardCalibrationData.mBottomRight17kg = buffer[11] | (buffer[10] << 8);
		mBalanceBoardCalibrationData.mTopLeft17kg     = buffer[13] | (buffer[12] << 8);
		mBalanceBoardCalibrationData.mBottomLeft17kg  = buffer[15] | (buffer[14] << 8);

		mBalanceBoardCalibrationData.mTopRight34kg    = buffer[17] | (buffer[16] << 8);
		mBalanceBoardCalibrationData.mBottomRight34kg = buffer[19] | (buffer[18] << 8);
		mBalanceBoardCalibrationData.mTopLeft34kg     = buffer[21] | (buffer[20] << 8);
		mBalanceBoardCalibrationData.mBottomLeft34kg  = buffer[23] | (buffer[22] << 8);

#ifdef DEBUG
        printf("Cal Board [%d] [%d] [%d] [%d]\n", mBalanceBoardCalibrationData.mTopRight0kg, mBalanceBoardCalibrationData.mBottomRight0kg, mBalanceBoardCalibrationData.mTopLeft0kg, mBalanceBoardCalibrationData.mBottomLeft0kg);
        printf("Cal Board [%d] [%d] [%d] [%d]\n", mBalanceBoardCalibrationData.mTopRight17kg, mBalanceBoardCalibrationData.mBottomRight17kg, mBalanceBoardCalibrationData.mTopLeft17kg, mBalanceBoardCalibrationData.mBottomLeft17kg);
        printf("Cal Board [%d] [%d] [%d] [%d]\n", mBalanceBoardCalibrationData.mTopRight34kg, mBalanceBoardCalibrationData.mBottomRight34kg, mBalanceBoardCalibrationData.mTopLeft34kg, mBalanceBoardCalibrationData.mBottomLeft34kg);
#endif
		retval = true;
	}
	mBalanceBoardAttached = retval;
	return retval;
}

void cWiiMote::ParseChuckReport(const unsigned char * data)
{
	mLastChuckReport.mStickX  = ExpansionByte(data[0]);
	mLastChuckReport.mStickY  = ExpansionByte(data[1]);
	mLastChuckReport.mAccelX  = ExpansionByte(data[2]);
	mLastChuckReport.mAccelY  = ExpansionByte(data[3]);
	mLastChuckReport.mAccelZ  = ExpansionByte(data[4]);
	mLastChuckReport.mButtonC = (ExpansionByte(data[5]) & 0x2) == 0;
	mLastChuckReport.mButtonZ = (ExpansionByte(data[5]) & 0x1) == 0;
}

void cWiiMote::ParseClassicReport(const unsigned char * data)
{
    //Left stick has only 6 bit resolution, adapt to 8 bit --> factor 4
	mLastClassicReport.mLeftStickX  = 4 * (ExpansionByte(data[0]) & 0x3F);
	mLastClassicReport.mLeftStickY  = 4 * (ExpansionByte(data[1]) & 0x3F);
    //Right stick has only 5 bit resolution, adapt to 8 bit --> factor 8
	mLastClassicReport.mRightStickX = 8 * (((ExpansionByte(data[2])>>7) & 0x01) | ((ExpansionByte(data[1])>>5) & 0x06) | ((ExpansionByte(data[0])>>3) & 0x18));
	mLastClassicReport.mRightStickY = 8 * (ExpansionByte(data[2]) & 0x1F);

    //shoulder buttons only have 5 bit resolution, adapt to 8 bit --> factor 8
    mLastClassicReport.mAnalogL     = 8 * (((ExpansionByte(data[3])>>5) & 0x07) | ((ExpansionByte(data[2])>>2) & 0x18));
    mLastClassicReport.mAnalogR     = 8 * (ExpansionByte(data[3]) & 0x1F);
 	mLastClassicReport.mL           = (ExpansionByte(data[4]) & 0x20) == 0;
 	mLastClassicReport.mR           = (ExpansionByte(data[4]) & 0x02) == 0;

	mLastClassicReport.mA           = (ExpansionByte(data[5]) & 0x10) == 0;
 	mLastClassicReport.mB           = (ExpansionByte(data[5]) & 0x40) == 0;
 	mLastClassicReport.mX           = (ExpansionByte(data[5]) & 0x08) == 0;
 	mLastClassicReport.mY           = (ExpansionByte(data[5]) & 0x20) == 0;
 	mLastClassicReport.mZL          = (ExpansionByte(data[5]) & 0x80) == 0;
 	mLastClassicReport.mZR          = (ExpansionByte(data[5]) & 0x04) == 0;
 	mLastClassicReport.mPlus        = (ExpansionByte(data[4]) & 0x04) == 0;
 	mLastClassicReport.mMinus       = (ExpansionByte(data[4]) & 0x10) == 0;
 	mLastClassicReport.mHome        = (ExpansionByte(data[4]) & 0x08) == 0;
 	mLastClassicReport.mUp          = (ExpansionByte(data[5]) & 0x01) == 0;
 	mLastClassicReport.mDown        = (ExpansionByte(data[4]) & 0x40) == 0;
 	mLastClassicReport.mLeft        = (ExpansionByte(data[5]) & 0x02) == 0;
 	mLastClassicReport.mRight       = (ExpansionByte(data[4]) & 0x80) == 0;
}

void cWiiMote::ParseGuitarReport(const unsigned char * data)
{
	mLastGuitarReport.mStickX = ExpansionByte(data[0]);
	mLastGuitarReport.mStickY = ExpansionByte(data[1]);
    mLastGuitarReport.mAnalog = ExpansionByte(data[3]);

	mLastGuitarReport.mG      = (ExpansionByte(data[5]) & 0x10) == 0;
 	mLastGuitarReport.mR      = (ExpansionByte(data[5]) & 0x40) == 0;
 	mLastGuitarReport.mY      = (ExpansionByte(data[5]) & 0x08) == 0;
 	mLastGuitarReport.mB      = (ExpansionByte(data[5]) & 0x20) == 0;
 	mLastGuitarReport.mO      = (ExpansionByte(data[5]) & 0x80) == 0;
 	mLastGuitarReport.mPlus   = (ExpansionByte(data[4]) & 0x04) == 0;
 	mLastGuitarReport.mMinus  = (ExpansionByte(data[4]) & 0x10) == 0;
 	mLastGuitarReport.mUp     = (ExpansionByte(data[5]) & 0x01) == 0;
 	mLastGuitarReport.mDown   = (ExpansionByte(data[4]) & 0x40) == 0;
}

void cWiiMote::ParseBalanceBoardReport(const unsigned char * data)
{
	//balance board does not use expansion byte encryption
	mLastBalanceBoardReport.mTopRight    = data[1] | (data[0] << 8);
	mLastBalanceBoardReport.mBottomRight = data[3] | (data[2] << 8);
	mLastBalanceBoardReport.mTopLeft     = data[5] | (data[4] << 8);
	mLastBalanceBoardReport.mBottomLeft  = data[7] | (data[6] << 8);
}

bool cWiiMote::EnableIR()
{
	bool retval = false;
	
//	DisableIR();
	if (!mIRRunning)
	{
		ClearBuffer();
		mOutputBuffer[0] = OUTPUT_ENABLE_IR;
		mOutputBuffer[1] = 0x4 | (mOutputControls.mVibration ? 0x1 : 0x0);
		retval = mHIDDevice.WriteToDevice(mOutputBuffer,mOutputBufferSize);
                Sleep(20);  //short delay to make sure IR works properly
		
		if (retval)
		{
			mOutputBuffer[0] = OUTPUT_ENABLE_IR2;
			mOutputBuffer[1] = 0x4 | (mOutputControls.mVibration ? 0x1 : 0x0);
			retval = mHIDDevice.WriteToDevice(mOutputBuffer,mOutputBufferSize);
                Sleep(20);  //short delay to make sure IR works properly
		}

		if (retval)
		{
			unsigned char val = 0x1;
			retval = WriteMemory(IR_REG_1,1,&val);
            Sleep(20);  //short delay to make sure IR works properly
		}
		
		if (retval)
		{
			retval = WriteMemory(IR_SENS_ADDR_1,9,IR_SENSITIVITY_PART1[mSensitivityLevel]);
            Sleep(20);  //short delay to make sure IR works properly
		}

		if (retval)
		{
			retval = WriteMemory(IR_SENS_ADDR_2,2,IR_SENSITIVITY_PART2[mSensitivityLevel]);
            Sleep(20);  //short delay to make sure IR works properly
		}


		if (retval)
		{
        	switch (mReportMode)
            {
            	case REPORT_MODE_MOTION_EXT_IR:
			        retval = WriteMemory(IR_REG_2,1,&IR_MODE_STD);
                    Sleep(20);  //short delay to make sure IR works properly
                    break;

                case REPORT_MODE_MOTION_IR:
			        retval = WriteMemory(IR_REG_2,1,&IR_MODE_EXT);
                    Sleep(20);  //short delay to make sure IR works properly
			        break;

                default:
                    retval = false;
                    break;
            }
		}

		if (retval)
		{
			unsigned char val = 0x8;
			retval = WriteMemory(IR_REG_1,1,&val);
		}


		mIRRunning = retval;
	}
	return retval;

}

bool cWiiMote::DisableIR()
{
	bool retval = false;

	if (mIRRunning)
	{
		ClearBuffer();
		mOutputBuffer[0] = OUTPUT_ENABLE_IR;
		mOutputBuffer[1] = (mOutputControls.mVibration ? 0x1 : 0x0);
		retval = mHIDDevice.WriteToDevice(mOutputBuffer,mOutputBufferSize);
        Sleep(20);  //short delay to make sure IR works properly
		
		if (retval)
		{
			mOutputBuffer[0] = OUTPUT_ENABLE_IR2;
			mOutputBuffer[1] = (mOutputControls.mVibration ? 0x1 : 0x0);
			retval = mHIDDevice.WriteToDevice(mOutputBuffer,mOutputBufferSize);
		}

		mIRRunning = false;
	}
	return retval;

}

void cWiiMote::ParseIRReport(const unsigned char * data, const unsigned char mode)
{
    switch (mode)
    {
        case IR_MODE_STD:
			mLastIRReport.mPX[0] = data[0] | ((data[2]>>4)&3) << 8;
			mLastIRReport.mPY[0] = data[1] | ((data[2]>>6)&3) << 8;
			mLastIRReport.mPSize[0] = 0;  // size is not transmitted in basic mode
			mLastIRReport.mPFound[0] =  !(data[0] == 0xff && data[1] == 0xff && ((data[2]>>4)&0xf) == 0xf);

			mLastIRReport.mPX[1] = data[3] | (data[2]&3) << 8;
			mLastIRReport.mPY[1] = data[4] | ((data[2]>>2)&3) << 8;
			mLastIRReport.mPSize[1] = 0;  // size is not transmitted in basic mode
			mLastIRReport.mPFound[1] =  !(data[3] == 0xff && data[4] == 0xff && (data[2]&0xf) == 0xf);

			mLastIRReport.mPX[2] = data[5] | ((data[7]>>4)&3) << 8;
			mLastIRReport.mPY[2] = data[6] | ((data[7]>>6)&3) << 8;
			mLastIRReport.mPSize[2] = 0;  // size is not transmitted in basic mode
			mLastIRReport.mPFound[2] =  !(data[5] == 0xff && data[6] == 0xff && ((data[7]>>4)&0xf) == 0xf);

			mLastIRReport.mPX[3] = data[8] | (data[7]&3) << 8;
			mLastIRReport.mPY[3] = data[9] | ((data[7]>>2)&3) << 8;
			mLastIRReport.mPSize[3] = 0;  // size is not transmitted in basic mode
			mLastIRReport.mPFound[3] =  !(data[8] == 0xff && data[9] == 0xff && (data[7]&0xf) == 0xf);
			sortIRP();
			break;

        case IR_MODE_EXT:
			mLastIRReport.mPX[0] = data[0] | ((data[2]>>4)&3) << 8;
			mLastIRReport.mPY[0] = data[1] | ((data[2]>>6)&3) << 8;
			mLastIRReport.mPSize[0] = data[2] & 0xf;
			mLastIRReport.mPFound[0] =  !(data[0] == 0xff && data[1] == 0xff && data[2] == 0xff);

			mLastIRReport.mPX[1] = data[3] | ((data[5]>>4)&3) << 8;
			mLastIRReport.mPY[1] = data[4] | ((data[5]>>6)&3) << 8;
			mLastIRReport.mPSize[1] = data[5] & 0xf;
			mLastIRReport.mPFound[1] =  !(data[3] == 0xff && data[4] == 0xff && data[5] == 0xff);

			mLastIRReport.mPX[2] = data[6] | ((data[8]>>4)&3) << 8;
			mLastIRReport.mPY[2] = data[7] | ((data[8]>>6)&3) << 8;
			mLastIRReport.mPSize[2] = data[8] & 0xf;
			mLastIRReport.mPFound[2] =  !(data[6] == 0xff && data[7] == 0xff && data[8] == 0xff);

			mLastIRReport.mPX[3] = data[9] | ((data[11]>>4)&3) << 8;
			mLastIRReport.mPY[3] = data[10] | ((data[11]>>6)&3) << 8;
			mLastIRReport.mPSize[3] = data[11] & 0xf;
			mLastIRReport.mPFound[3] =  !(data[9] == 0xff && data[10] == 0xff && data[11] == 0xff);
			sortIRP();
			break;
            
        case IR_MODE_FULL:
			/* not yet implemented */
			break;
        
        case IR_MODE_OFF:
        default:
			break;
    }

}

void cWiiMote::sortIRP(void)
{
    static int i, j;
    for (i = 0; i < 3; i++)
    {
        if  (!mLastIRReport.mPFound[i])
        {
            for (j = i + 1; j < 4; j++)
            {
                /* move dot to empty position */
                if  (mLastIRReport.mPFound[j])
                {
                    mLastIRReport.mPX[i] = mLastIRReport.mPX[j];
                    mLastIRReport.mPY[i] = mLastIRReport.mPY[j];
                    mLastIRReport.mPSize[i] = mLastIRReport.mPSize[j];
                    mLastIRReport.mPFound[i] = true;
                    mLastIRReport.mPFound[j] = false;
                    break;
                }
            }
        }
    }
}

bool cWiiMote::GetIRP(float &x, float &y, float &s, int no) const
{   
    bool retval = false;
    if (mIRRunning && mLastIRReport.mPFound[no])
    {
        x = mLastIRReport.mPX[no] / 1023.f;
        y = mLastIRReport.mPY[no] / 767.f;
		s = mLastIRReport.mPSize[no];
        retval = true;
    }
    else
    {
		x = y = s = 0.f;
	}
    return retval;
}

bool cWiiMote::StartDataStream()
{
	bool retval;
	
	//only enable IR if wanted
	if (mRequestIR)
	{
		if (mLastExpansionReport.mAttachmentPluggedIn)
		{
			retval = SetReportMode(REPORT_MODE_MOTION_EXT_IR);
		}
		else
		{
			retval = SetReportMode(REPORT_MODE_MOTION_IR);
		}

		if (retval)
			EnableIR();
	}
	else
	{
		if (mLastExpansionReport.mAttachmentPluggedIn)
		{
			//set different report mode for Balance Board
			if (mExpansionType != EXP_DEV_BALANCE)
			{
				retval = SetReportMode(REPORT_MODE_MOTION_EXT);
			}
			else
			{
				retval = SetReportMode(REPORT_MODE_BUTTONS_EXT);
			}
			
		}
		else
		{
			retval = SetReportMode(REPORT_MODE_MOTION);
		}
		if (retval)
			DisableIR();
	}
    
	mDataStreamRunning = retval;
	return retval;
}


bool cWiiMote::StopDataStream()
{
	if (mDataStreamRunning)
	{
		mDataStreamRunning = false;
		DisableIR();
		SetReportMode(REPORT_MODE_EVENT_BUTTONS);
	}
	return true;
}

char cWiiMote::GetRawBatteryLevel()
{
    mRequestBattery = true;
    RequestExpansionReport();
    
    if (mLastExpansionReport.mBatteryLevel)
        return mLastExpansionReport.mBatteryLevel;
    else
        return 0;
}

char cWiiMote::GetBatteryPercent()
{
    float percent = 0.0f;
    mRequestBattery = true;
    RequestExpansionReport();
        
    if (mLastExpansionReport.mBatteryLevel)
    {
        percent = ((float)(100 * mLastExpansionReport.mBatteryLevel) / 0xC0);
        if (percent > 100.f) percent = 100.f;
    }
    return (char)percent;
}

unsigned short cWiiMote::GetDeviceID()
{
	return mDeviceType;
}

void cWiiMote::ActivateIR()
{
	//Balance Board does not support IR camera
	if (mExpansionType != EXP_DEV_BALANCE)
	{
    	mRequestIR = true;
    	RequestExpansionReport();
	}
}

void cWiiMote::DeactivateIR()
{
	//Balance Board does not support IR camera
	if (mExpansionType != EXP_DEV_BALANCE)
	{
	    mRequestIR = false;
	    RequestExpansionReport();    
	}
}

void cWiiMote::SetSmoothFac(float fac)
{
	//limit factor to 0..1
	mSmoothFac = min(1.0f, max(fac, 0.0f));
}

void cWiiMote::SetIRSensitivity(int level)
{
	//limit level to 0..4
	mSensitivityLevel = min(4, max(level, 0));
}

bool cWiiMote::ir_active(void) const
{
    return mIRRunning;
}

bool cWiiMote::nunchuk_active(void) const
{
    return mNunchuckAttached;
}

bool cWiiMote::classic_active(void) const
{
    return mClassicAttached;
}

bool cWiiMote::guitar_active(void) const
{
    return mGuitarAttached;
}

bool cWiiMote::balanceboard_active(void) const
{
	return mBalanceBoardAttached;
}

bool cWiiMote::vibration_active(void) const
{
    return mOutputControls.mVibration;
}
