/*
 *******************************************************************************
 * wiimote.h
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
 * Driver for Wiimote and its extension devices - header
 *
 * Comments
 * 
 * cWiimote 0.2 by Kevin Forbes (http://simulatedcomicproduct.com)
 * improved by Robert Jäger (http://www.firoball.de)
 * This code is public domain, and comes with no warranty. The user takes full 
 * responsibility for anything that happens as a result from using this code.
 *
 *******************************************************************************
 */

#ifndef WIIMOTE_H
#define WIIMOTE_H
#include "hiddevice.h"

#include <math.h>
#define CLAMP(v,l,h)    ((v) < (l) ? (l) : (v) > (h) ? (h) : (v))
#define CYCLE(v,l,h)    ((v) < (l) ? (v+h-l) : (v) >= (h) ? (v-h+l) : (v))
#define VECLEN(x,y,z)   (sqrt((x) * (x) + (y) * (y) + (z) * (z)))
#define PI 3.14159265358979323846

//#define PRINT_STATUS
//#define DEBUG

class cWiiMote
{
public:
	cWiiMote();
	~cWiiMote();
	
	//connection management
	bool ConnectToDevice(int index = 0);
	bool Disconnect();	
	bool IsConnected() const {return mHIDDevice.IsConnected();}
	static int GetDevices();
	
	
	bool StartDataStream();
	bool StopDataStream();
	bool RequestExpansionReport();


	//this is the wiimote message pump. It should probably be called in loop from a thread
	bool HeartBeat(int timeout = 1);
	bool SetVibration(bool vib_on);
	bool SetLEDs(bool led1, bool led2, bool led3, bool led4);



	//Querying functions and structures:
	void GetCalibratedAcceleration(float & x, float & y, float &z) const;
	void GetOrientation(float & pan, float & tilt, float & roll); /*const;*/
	void GetCalibratedChuckAcceleration(float & x, float & y, float &z) const;
	void GetChukOrientation(float & pan, float & tilt, float & roll); /*const;*/
	void GetCalibratedChuckStick(float & x, float & y) const;
	void GetCalibratedLeftClassicStick(float & x, float & y) const;
	void GetCalibratedRightClassicStick(float & x, float & y) const;
	void GetCalibratedLeftClassicShoulder(float & b) const;
	void GetCalibratedRightClassicShoulder(float & b) const;
	void GetCalibratedGuitarStick(float & x, float & y) const;
	void GetCalibratedGuitarAnalog(float & b) const;
	void GetCalibratedBalanceBoardSensors(float & pTL, float & pTR, float & pBL, float & pBR) const;
	bool GetIRP(float &x, float &y, float &s, int no) const;


	char GetBatteryPercent();
	char GetRawBatteryLevel();
	unsigned short GetDeviceID();
	void ActivateIR();
	void DeactivateIR();
	void SetSmoothFac(float fac);
	void SetIRSensitivity(int level);
	bool ir_active(void) const;
	bool nunchuk_active(void) const;
	bool classic_active(void) const;
	bool guitar_active(void) const;
	bool balanceboard_active(void) const;
	bool vibration_active(void) const;
    

	struct tExpansionReport
	{
		bool mAttachmentPluggedIn;
		bool mIREnabled;
		bool mSpeakerEnabled;
		bool mLED1On;
		bool mLED2On;
		bool mLED3On;
		bool mLED4On;
		unsigned char mBatteryLevel;

		void Init()
		{
			mAttachmentPluggedIn = false;
			mIREnabled = false;
			mSpeakerEnabled = false;
			mLED1On = false;
			mLED2On = false;
			mLED3On = false;
			mLED4On = false;
			mBatteryLevel = 0;
		}
	};
	struct tButtonStatus
	{
		bool mA;
		bool mB;
		bool m1;
		bool m2;
		bool mPlus;
		bool mMinus;
		bool mHome;
		bool mUp;
		bool mDown;
		bool mLeft;
		bool mRight;
	
		void Init()
		{
			mA = mB = m1 = m2 = mPlus = mMinus = mHome = mUp = mDown = mLeft = mRight = false;
		}
	};
	struct tMotionReport
	{
		unsigned char mX;
		unsigned char mY;
		unsigned char mZ;
		
		void Init()
		{
			mX = mY = mZ = 0;
		}
	};

	struct tChuckReport
	{
		unsigned char mStickX;
		unsigned char mStickY;
		unsigned char mAccelX;
		unsigned char mAccelY;
		unsigned char mAccelZ;
		bool	mButtonC;
		bool	mButtonZ;
		void Init()
		{
			mStickX = mStickY = mAccelX = mAccelY = mAccelZ = 0;
			mButtonC = mButtonZ = false;
		};
	};

	struct tClassicReport
	{
		unsigned char mLeftStickX;
		unsigned char mLeftStickY;
		unsigned char mRightStickX;
		unsigned char mRightStickY;
		unsigned char mAnalogL;
		unsigned char mAnalogR;
		bool mA;
		bool mB;
		bool mX;
		bool mY;
		bool mPlus;
		bool mMinus;
		bool mHome;
		bool mUp;
		bool mDown;
		bool mLeft;
		bool mRight;
		bool mZL;
		bool mZR;
		bool mL;
		bool mR;
		void Init()
		{
			mLeftStickX = mLeftStickY = mRightStickX = mRightStickY = mAnalogL = mAnalogR = 0;
			mA = mB = mX = mY = mPlus = mMinus = mHome = mUp = mDown = mLeft = mRight = mZL = mZR = mL = mR = false;
		};
	};

	struct tGuitarReport
	{
		unsigned char mStickX;
		unsigned char mStickY;
		unsigned char mAnalog;
		bool mG;
		bool mR;
		bool mY;
		bool mB;
		bool mO;
		bool mPlus;
		bool mMinus;
		bool mUp;
		bool mDown;
		void Init()
		{
			mStickX = mStickY = mAnalog = 0;
			mG = mR = mY = mB = mO = mPlus = mMinus = mUp = mDown = false;
		};
	};

	struct tBalanceBoardReport
	{
		//balance board delivers 16 bit accuracy
		unsigned int mTopRight;
		unsigned int mBottomRight;
		unsigned int mTopLeft;
		unsigned int mBottomLeft;
		void Init()
		{
			mTopRight = mBottomRight = mTopLeft = mBottomLeft = 0;
		}
	};
	
	struct tIRReport
	{
		unsigned short mPX[4];
		unsigned short mPY[4];
		
		unsigned char mPSize[4];

		bool mPFound[4];

		void Init()
		{
                        mPX[0] = mPY[0] = mPSize[0] = 0;
                        mPX[1] = mPY[1] = mPSize[1] = 0;
                        mPX[2] = mPY[2] = mPSize[2] = 0;
                        mPX[3] = mPY[3] = mPSize[3] = 0;
                        mPFound[0] = mPFound[1] = mPFound[2] = mPFound[3] = false;
		}


	};
	const tButtonStatus & GetLastButtonStatus() const {return mLastButtonStatus;}
	const tChuckReport & GetLastChuckReport() const {return mLastChuckReport;}
	const tClassicReport & GetLastClassicReport() const {return mLastClassicReport;}
	const tGuitarReport & GetLastGuitarReport() const {return mLastGuitarReport;}
	const tBalanceBoardReport & GetLastBalanceBoardReport() const {return mLastBalanceBoardReport;}
	const tMotionReport & GetLastMotionReport() const { return mLastMotionReport;}
	const tExpansionReport & GetLastExpansionReport() const { return mLastExpansionReport;}
	const tIRReport & GetLastIRReport() const { return mLastIRReport;}
	
	
    #ifdef PRINT_STATUS
	//debugging functions:
	void PrintStatus(); /*const;*/
    #endif
private:
	
	//parsing functions for input reports
	void ParseExpansionReport(const unsigned char * data);
	void ParseButtonReport(const unsigned char * data);
	void ParseMotionReport(const unsigned char * data);
	void ParseReadData(const unsigned char * data);
	void ParseExpansionDevice(const unsigned char * data);
	void ParseChuckReport(const unsigned char * data);
	void ParseClassicReport(const unsigned char * data);
	void ParseGuitarReport(const unsigned char * data);
	void ParseBalanceBoardReport(const unsigned char * data);
	void ParseIRReport(const unsigned char * data, const unsigned char mode);

	//tell the wiimote how to send data
	enum eReportMode
	{
		REPORT_MODE_EVENT_BUTTONS,
		REPORT_MODE_MOTION, 
		REPORT_MODE_MOTION_EXT,
		REPORT_MODE_MOTION_IR,
		REPORT_MODE_MOTION_EXT_IR,
		REPORT_MODE_BUTTONS_EXT
	};
	bool SetReportMode(eReportMode mode);


	//housekeeping functions
	void Init();
	void ClearBuffer();

	//low level tasks
	bool SelectInputChannel(bool continuous, unsigned char channel);
	bool UpdateOutput();
	bool ReadMemory(unsigned int address, unsigned short size, unsigned char * buffer) const;	
	bool WriteMemory(unsigned int address, unsigned char size, const unsigned char * buffer);

	bool ReadData(unsigned int address, unsigned short size, unsigned char * buffer);
	bool IssueReadRequest(unsigned int address, unsigned short size, unsigned char * buffer);
	bool ReadCalibrationData();
	bool SendReportMode();

	bool InitExpansion();
	bool InitNunchuck();
	bool InitClassic();
	bool InitGuitar();
	bool InitBalanceBoard();
	bool EnableIR();
	bool DisableIR();
	void sortIRP(void);

	static inline unsigned char ExpansionByte(unsigned char in) {return (in ^ 0x17)+0x17;}
	//flash reading vars
	struct tMemReadInfo
	{
		enum eReadStatus
		{
			READ_PENDING,
			READ_NONE,
			READ_COMPLETE,
			READ_ERROR
		} mReadStatus;

		unsigned char * mReadBuffer;
		unsigned short mTotalBytesToRead;
		unsigned short mBytesRead;
		unsigned short mBaseAddress;
		void Init()
		{
			mReadStatus = READ_NONE;
			mReadBuffer = NULL;
			mTotalBytesToRead = 0;
			mBytesRead = 0;
			mBaseAddress = 0;
		}
	} mReadInfo;

	//calibration data for the wiimote
	struct tAccelCalibrationData
	{
		unsigned char mXZero;
		unsigned char mYZero;
		unsigned char mZZero;
		unsigned char mXG;
		unsigned char mYG;
		unsigned char mZG;
		void Init()
		{
			mXZero = mYZero = mZZero = mXG = mYG = mZG = 0;
		}
	} ;

	struct tStickCalibrationData
	{
		unsigned char mXmin;
		unsigned char mXmid;
		unsigned char mXmax;
		unsigned char mYmin;
		unsigned char mYmid;
		unsigned char mYmax;

		void Init()
		{
			mXmax = mYmax = mXmin = mYmin = mXmid = mYmid = 0;
		}
	};

    struct tAnalogButtonCalibrationData
	{
		unsigned char mMin;
		unsigned char mMax;

		void Init()
		{
			mMax = mMin = 0;
		}
	};
    	
	struct tBalanceBoardCalibrationData
	{
		//balance board delivers 16 bit resolution
		unsigned int mTopRight0kg;
		unsigned int mBottomRight0kg;
		unsigned int mTopLeft0kg;
		unsigned int mBottomLeft0kg;
		
		unsigned int mTopRight17kg;
		unsigned int mBottomRight17kg;
		unsigned int mTopLeft17kg;
		unsigned int mBottomLeft17kg;

		unsigned int mTopRight34kg;
		unsigned int mBottomRight34kg;
		unsigned int mTopLeft34kg;
		unsigned int mBottomLeft34kg;
		
		void Init()
		{
			mTopRight0kg  = mBottomRight0kg  = mTopLeft0kg  = mBottomLeft0kg  = 0;
			mTopRight17kg = mBottomRight17kg = mTopLeft17kg = mBottomLeft17kg = 0;
			mTopRight34kg = mBottomRight34kg = mTopLeft34kg = mBottomLeft34kg = 0;
		}
	};
	
	tAccelCalibrationData        mAccelCalibrationData;
	tAccelCalibrationData        mNunchuckAccelCalibrationData;
	tStickCalibrationData        mNunchuckStickCalibrationData;
	tStickCalibrationData        mClassicLeftStickCalibrationData;
	tStickCalibrationData        mClassicRightStickCalibrationData;
	tAnalogButtonCalibrationData mClassicShoulderCalibrationData;
	tStickCalibrationData        mGuitarStickCalibrationData;
	tAnalogButtonCalibrationData mGuitarAnalogCalibrationData;
	tBalanceBoardCalibrationData mBalanceBoardCalibrationData;
    
	//output requests
	struct tOutputControls
	{
		bool mVibration;
		bool mLED1;
		bool mLED2;
		bool mLED3;
		bool mLED4;

		void Init()
		{
			mVibration = mLED1 = mLED2= mLED3= mLED4 = false;
		}
	};

	struct tAngleData
	{
		float mPan;
		float mTilt;
		float mRoll;
        
		void Init()
		{
			mPan = mTilt = mRoll = 0.f;
		}
	};
    
	tAngleData mLastAngle;
	tAngleData mLastChukAngle;
    
	//internal request flags
	bool mRequestBattery;
	bool mRequestIR;
		
	unsigned short mExpansionType;
	unsigned short mDeviceType;
	tOutputControls mOutputControls;
	eReportMode	mReportMode;

	//our communications device
	cHIDDevice mHIDDevice;

	bool mDataStreamRunning;

	//buffers for input/output
	static const int mOutputBufferSize = 22;
	unsigned char mOutputBuffer[mOutputBufferSize];
	static const int mInputBufferSize = 22;
	unsigned char mInputBuffer[mInputBufferSize];

	//smoothing factor for wiimote/Nunchuk angle calculation
	float mSmoothFac;
	//sensity level for IR camera
	int mSensitivityLevel;
	
protected:
	//input states
	tExpansionReport    mLastExpansionReport;
	tButtonStatus       mLastButtonStatus;
	tMotionReport       mLastMotionReport;
	tChuckReport        mLastChuckReport;
	tClassicReport      mLastClassicReport;
	tGuitarReport       mLastGuitarReport;
	tBalanceBoardReport mLastBalanceBoardReport;
	tIRReport           mLastIRReport;

	//output states
	bool mNunchuckAttached;
	bool mClassicAttached;
	bool mGuitarAttached;
	bool mBalanceBoardAttached;
	bool mIRRunning;
};

#endif
