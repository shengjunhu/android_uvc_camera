package android.camera.uvc;

import android.hardware.usb.UsbDeviceConnection;
import android.util.Log;
import android.view.Surface;

import java.nio.ByteBuffer;
import java.util.List;

/**
 * @Author:
 * @Date:2024/4/3
 * @Desc:
 */
public final class CameraDevice {

    private static final String TAG = "CameraDevice";

    /** Return Code */
    public static final int STATE_SUCCESS       =  0x0;
    public static final int STATE_FAILURE       = -0x1;
    public static final int STATE_ERROR_MEMORY  = -0x2;
    public static final int STATE_ERROR_ACCESS  = -0x3;
    public static final int STATE_NOT_SUPPORT   = -0x4;
    public static final int STATE_NONE_DEVICE   = -0x5;
    public static final int STATE_ERROR_STATE   = -0x6;
    public static final int STATE_ERROR_PARAM   = -0x7;
    public static final int STATE_NONE_INIT     = -0x8;
    public static final int STATE_ERROR_UNKNOWN = -0x9;

    /**
     * Callback data.
     * @see CameraDevice#setCallback(Callback).
     */
    public interface Callback {
        /** Camera frame callback. */
        void onFrame(ByteBuffer frame, int size);
        /** @see CameraDevice#takePicture() */
        void onPicture(ByteBuffer jpeg, int size);
    }

//========================================= Java API ===============================================

    private final long handle;
    private Callback callback;
    private UsbDeviceConnection udc;
    private volatile boolean streaming;

    public CameraDevice() {
        this.handle = create();
    }

    /**
     * Open Camera.
     * Call after {@link CameraDevice#CameraDevice()}.
     * @param usbfs {@link android.hardware.usb.UsbDevice#getDeviceName()}.
     * @param udc {@link android.hardware.usb.UsbDeviceConnection}.
     * @return {@link CameraDevice#STATE_SUCCESS} is succeed, otherwise is failed.
     */
    public synchronized int openDevice(String usbfs, UsbDeviceConnection udc) {
        if (handle <= 0) {
            Log.e(TAG, "openDevice(): camera destroyed");
            return STATE_NONE_INIT;
        } else if (udc == null) {
            Log.e(TAG, "openDevice(): udc is null");
            return STATE_ERROR_PARAM;
        } else {
            if (this.udc != null) {
                this.udc.close();
                this.udc = null;
            }
            this.udc = udc;
            return open_device(handle, usbfs, udc.getFileDescriptor());
        }
    }

    /**
     * Get UsbDeviceConnection.
     * Call after {@link CameraDevice#openDevice(String, UsbDeviceConnection)} .
     * @return {@link android.hardware.usb.UsbDeviceConnection} .
     */
    public synchronized UsbDeviceConnection getUsbDeviceConnection() {
        return this.udc;
    }

    /**
     * Getting camera support information .
     * Call after {@link CameraDevice#openDevice(String, UsbDeviceConnection)} .
     * @param list not null {@link SupportInfo} .
     * @return {@link CameraDevice#STATE_SUCCESS} is succeed, otherwise is failed.
     */
    public synchronized int getSupport(List<SupportInfo> list) {
        if (handle <= 0) {
            Log.e(TAG, "getSupport(): camera destroyed");
            return STATE_NONE_INIT;
        } else if (list == null) {
            Log.e(TAG, "getSupport(): list=null");
            return STATE_ERROR_PARAM;
        } else {
            return get_support(handle, list);
        }
    }

    /**
     * Setting camera support.
     * Call after {@link CameraDevice#getSupport(List)} .
     * @param support not null {@link SupportInfo} .
     * @return {@link CameraDevice#STATE_SUCCESS} is succeed, otherwise is failed.
     */
    public synchronized int setSupport(SupportInfo support) {
        if (handle <= 0) {
            Log.e(TAG, "setSupport(): camera destroyed");
            return STATE_NONE_INIT;
        } else if (support == null) {
            Log.e(TAG, "setSupport(): support=null");
            return STATE_ERROR_PARAM;
        } else {
            return set_support(handle, support);
        }
    }

    /**
     * If need frame callback or tak picture.
     * Call after {@link CameraDevice#setSupport(SupportInfo)} .
     * and frame will to be callback on {@link CameraDevice.Callback#onFrame(ByteBuffer, int)}.
     * @param callback {@link CameraDevice.Callback} .
     */
    public synchronized void setCallback(Callback callback) {
        if (!streaming) {
            this.callback = callback;
        } else {
            Log.e(TAG, "Call setCallback() before startStream()");
        }
    }

    /**
     * If need preview image.
     * Call after {@link CameraDevice#setSupport(SupportInfo)} .
     * @param surface {@link android.view.Surface} .
     * @return {@link CameraDevice#STATE_SUCCESS} is succeed, otherwise is failed.
     */
    public synchronized int setPreview(Surface surface) {
        if (handle <= 0) {
            Log.e(TAG, "setPreview(): camera destroyed");
            return STATE_NONE_INIT;
        } else if (surface == null) {
            Log.e(TAG, "setPreview(): surface=null");
            return STATE_ERROR_PARAM;
        } else {
            return set_preview(handle, surface);
        }
    }

    /**
     * Start getting frame.
     * Call after {@link CameraDevice#setSupport(SupportInfo)} .
     * @return {@link CameraDevice#STATE_SUCCESS} is succeed, otherwise is failed.
     */
    public synchronized int startStream() {
        if (handle <= 0) {
            Log.e(TAG, "startStream(): camera destroyed");
            return STATE_NONE_INIT;
        } else {
            this.streaming = true;
            return start_stream(handle);
        }
    }

    /**
     * Taking one picture of jpeg format.
     * Call after {@link CameraDevice#startStream()} .
     * and need call {@link CameraDevice#setCallback(Callback)}
     * The picture will be callback to {@link CameraDevice.Callback#onPicture(ByteBuffer, int)} .
     * @return {@link CameraDevice#STATE_SUCCESS} is succeed, otherwise is failed.
     */
    public synchronized int takePicture() {
        if (handle > 0) {
            return take_picture(handle);
        } else {
            Log.e(TAG, "takePicture(): camera destroyed");
            return STATE_NONE_INIT;
        }
    }

    /**
     * Recording video.
     * Call after {@link CameraDevice#startStream()} .
     * IF record video, need config {@link SupportInfo#video} and {@link SupportInfo#dir}
     * Making sure 'dir' exist and be accessing. video will be save to ${dir}/VID_yyyMMdd_HHmmss.mp4.
     * @return {@link CameraDevice#STATE_SUCCESS} is succeed, otherwise is failed.
     */
    public synchronized int startVideo() {
        if (handle > 0) {
            return start_video(handle);
        } else {
            Log.e(TAG, "startVideo(): camera destroyed");
            return STATE_NONE_INIT;
        }
    }

    /**
     * Pausing record video.
     * Call after {@link CameraDevice#startVideo()} .
     * @param pause True is continuing to record video, False is pausing record video.
     * @return {@link CameraDevice#STATE_SUCCESS} is succeed, otherwise is failed.
     */
    public synchronized int pauseVideo(boolean pause) {
        if (handle > 0) {
            return pause_video(handle, pause);
        } else {
            Log.e(TAG, "pauseVideo(): camera destroyed");
            return STATE_NONE_INIT;
        }
    }

    /**
     * Stop recording video.
     * Call after {@link CameraDevice#startVideo()}.
     * @return {@link CameraDevice#STATE_SUCCESS} is succeed, otherwise is failed.
     */
    public synchronized int stopVideo() {
        if (handle > 0) {
            return stop_video(handle);
        } else {
            Log.e(TAG, "stopVideo(): camera destroyed");
            return STATE_NONE_INIT;
        }
    }

    /**
     * Stop getting frame.
     * Call after {@link CameraDevice#startStream()}.
     * @return {@link CameraDevice#STATE_SUCCESS} is succeed, otherwise is failed.
     */
    public synchronized int stopStream() {
        if (handle > 0) {
            this.streaming = false;
            return stop_stream(handle);
        } else {
            Log.e(TAG, "stopStream(): camera destroyed");
            return STATE_NONE_INIT;
        }
    }

    /**
     * Close camera.
     * Call after {@link CameraDevice#openDevice(String, UsbDeviceConnection)} .
     * @return {@link CameraDevice#STATE_SUCCESS} is succeed, otherwise is failed.
     */
    public synchronized int closeDevice() {
        if (this.udc != null) {
            this.udc.close();
            this.udc = null;
        }

        if (handle > 0) {
            return close_device(handle);
        } else {
            Log.e(TAG, "closeDevice(): camera destroyed");
            return STATE_NONE_INIT;
        }
    }

    /**
     * If call it, other method are invalid.
     */
    public synchronized void destroy() {
        if (handle > 0) destroy(handle);
    }

//======================================== JNI to Call =============================================

    private ByteBuffer frame;
    private ByteBuffer picture;

    private void onFrame(int size) {
        if (callback != null) {
            callback.onFrame(frame, size);
        }
    }

    private void onPicture(int size) {
        if (callback != null) {
            callback.onPicture(picture, size);
        }
    }

//======================================= Native Method ============================================

    static {
        System.loadLibrary("camera_uvc");
    }

    private native long create();

    private static native int open_device(long handle, String fs, int fd);

    private static native int get_support(long handle, List<SupportInfo> list);

    private native int set_support(long handle, SupportInfo support);

    private static native int set_preview(long handle, Surface surface);

    private static native int start_stream(long handle);

    private static native int take_picture(long handle);

    private static native int start_video(long handle);

    private static native int pause_video(long handle, boolean pause);

    private static native int stop_video(long handle);

    private static native int stop_stream(long handle);

    private static native int close_device(long handle);

    private native void destroy(long handle);

//==================================================================================================

}
