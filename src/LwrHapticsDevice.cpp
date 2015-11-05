#include <HAPI/LwrHapticsDevice.h>
#include <H3DUtil/DynamicLibrary.h>

#include <sstream>

using namespace HAPI;

HAPIHapticsDevice::HapticsDeviceRegistration
LwrHapticsDevice::device_registration("Lwr", &(newInstance< LwrHapticsDevice >));

bool LwrHapticsDevice::initHapticsDevice( int _thread_frequency ) {
  device_id = 1; //hack

  com_thread = new H3DUtil::PeriodicThread( H3DUtil::ThreadBase::HIGH_PRIORITY, 1000 );
  com_thread->setThreadName( "Lwr com thread" );

  com_func_cb_handle = com_thread->asynchronousCallback( com_func, this );

  return true;
}

bool LwrHapticsDevice::releaseHapticsDevice() {
  HAPIHapticsDevice::disableDevice();
  if( device_id != -1 ) {
    if( com_thread ) {
      if( com_func_cb_handle != -1 ) {
        com_thread->removeAsynchronousCallback( com_func_cb_handle );
        com_func_cb_handle = -1;
      }
      delete com_thread;
      com_thread = NULL;
    }

    device_id = -1;
  }
  return true;
}

void LwrHapticsDevice::updateDeviceValues( DeviceValues &dv, HAPITime dt ) {
  HAPIHapticsDevice::updateDeviceValues( dv, dt );

  if( device_id != -1 ) {
    com_lock.lock();
    dv.position = current_values.position;
    dv.velocity = current_values.velocity;
    dv.orientation = current_values.orientation;
    com_lock.unlock();
  }
}

void LwrHapticsDevice::sendOutput( DeviceOutput &dv, HAPITime dt ) {
  if( device_id != -1 ) {
    com_lock.lock();
    current_values.force = dv.force;
    current_values.torque = dv.torque;
    com_lock.unlock();
  }
}

void LwrHapticsDevice::updateValues(Vec3 pos, Vec3 vel, Rotation rot) {
  if( device_id != -1 ) {
    com_lock.lock();
    current_values.position = pos;
    current_values.velocity = vel;
    current_values.orientation = rot;
    com_lock.unlock();
  }
}

H3DUtil::PeriodicThread::CallbackCode
LwrHapticsDevice::com_func( void *data ) {
  LwrHapticsDevice *hd = static_cast< LwrHapticsDevice * >( data );

  if( hd->device_id != -1 ) {
    //do update stuff here?
    /*double x, y, z, rx, ry, rz, vx, vy, vz;
    dhdGetPosition( &z, &x, &y, hd->device_id );
    dhdGetOrientationRad( &rz, &rx, &ry, hd->device_id );
    dhdGetLinearVelocity( &vz, &vx, &vy, hd->device_id );

    // TODO: multiple buttons
    bool button = (dhdGetButton( 0, hd->device_id ) == DHD_ON);

    Vec3 position = Vec3( x, y, z );
    Vec3 velocity = Vec3( vx, vy, vz );

    Rotation orientation =
      Rotation( 1, 0, 0, (float)( H3DUtil::Constants::pi / 4 ) ) *
      Rotation( 0, 0, 1, (float)rz ) *
      Rotation( 1, 0, 0, (float)rx ) *
      Rotation( 0, 1, 0, (float)ry ) *
      Rotation( 1, 0, 0, (float)( -H3DUtil::Constants::pi / 2 ) );

    hd->com_lock.lock();

    hd->current_values.position = position;
    hd->current_values.velocity = velocity;
    hd->current_values.button_status = button;
    hd->current_values.orientation = orientation;

    Vec3 force = hd->current_values.force;
    Vec3 torque = hd->current_values.torque;
    hd->com_lock.unlock();

    dhdSetForceAndTorque( force.z, force.x, force.y,
                          torque.z, torque.x, torque.y,
                          hd->device_id );*/
  }

  return H3DUtil::PeriodicThread::CALLBACK_CONTINUE;
}
