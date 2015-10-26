#ifndef __LWRHAPTICSDEVICE_H__
#define __LWRHAPTICSDEVICE_H__

#include <HAPI/HAPIHapticsDevice.h>

#ifdef HAVE_DHDAPI

namespace HAPI {

  /// \ingroup HapticsDevices
  /// \class ForceDimensionHapticsDevice
  /// \brief Interface to all haptics devices from ForceDimension, including
  /// the Delta and Omega devices.
  ///
  /// Uses the DHD API to interface to the haptics devices, so every
  /// device supported by DHD API is supported by this class.
  class HAPI_API LwrHapticsDevice: public HAPIHapticsDevice {
  public:
    /// Constructor.
    LwrHapticsDevice():
      device_id( -1 ),
      com_thread( NULL ),
      com_func_cb_handle( -1 ) {

    }

    /// Destructor.
    virtual ~LwrHapticsDevice() {}

    /// Register this renderer to the haptics renderer database.
    static HapticsDeviceRegistration device_registration;
  protected:
    virtual void updateDeviceValues( DeviceValues &dv, HAPITime dt );

    virtual void sendOutput( DeviceOutput &dv,
                             HAPITime dt );

    /// Initialize the haptics device. Use the HapticThread class in Threads.h
    /// as the thread for haptic rendering.
    /// \param _thread_frequency is the desired haptic frequency.
    /// 1000 is the maximum allowed frequency that can be specified. Setting
    /// this parameter to -1 means run as fast as possible. It is recommended
    /// to use the default value for most users.
    /// \returns true if succeeded.
    virtual bool initHapticsDevice( int _thread_frequency = 1000 );

    /// Releases all resources allocated in initHapticsDevice.
    virtual bool releaseHapticsDevice();

    /// Callback function for communication thread
    static H3DUtil::PeriodicThread::CallbackCode com_func( void *data );

    /// Callback handle to the com_func callback that is set up
    int com_func_cb_handle;

    /// Thread used to do communication with the haptics device
    H3DUtil::PeriodicThread *com_thread;

    /// Lock for exchanging data with the communication thread.
    H3DUtil::MutexLock com_lock;

    /// The current device values updated in the communicataion thread.
    /// Access to this structure must be contained within locking with
    /// com_lock.
    DeviceValues current_values;
  };
}

#endif

#endif