package android.camera.uvc;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.util.Log;

import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/**
 * @Author:
 * @Date:2024/4/3
 * @Desc:
 */
public final class CameraManager {

    private static final String TAG = "CameraManager";
    private DeviceWatcher watcher;
    private UsbManager usbManager;
    private PendingIntent pi;
    private Context context;

    public interface DeviceWatcher {
        void onAttach(UsbDevice device);
        void onActive(UsbDevice device, @Nullable UsbDeviceConnection udc);
        void onDetach(UsbDevice device);
    }

    /**
     * @param ctx {@link android.content.Context}
     * @param watcher {@link DeviceWatcher}
     */
    private CameraManager(Context ctx, DeviceWatcher watcher) {
        this.context = ctx;
        this.watcher = watcher;
        // PendingIntent
        Intent intent = new Intent(ACTION_PERMISSION);
        this.pi = PendingIntent.getBroadcast(ctx, 0, intent, 0);
        this.usbManager = (UsbManager)ctx.getSystemService(Context.USB_SERVICE);
        // BroadcastReceiver
        IntentFilter filter = new IntentFilter(ACTION_PERMISSION);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
        ctx.registerReceiver(usbReceiver, filter);
    }

    /**
     * Filter {@link UsbDevice} by vid and pid
     * @param vid {@link UsbDevice#getVendorId()}
     * @param pid {@link UsbDevice#getProductId()}
     * @return not null
     */
    public List<UsbDevice> queryDevice(int vid, int pid) {
        List<UsbDevice> list = new ArrayList<>();
        Collection<UsbDevice> devices = usbManager.getDeviceList().values();
        for (UsbDevice device: devices) {
            if (vid == device.getVendorId() && pid == device.getProductId()) {
                list.add(device);
            }
        }
        return list;
    }

    /**
     * Query all {@link UsbDevice}
     * @return not null
     */
    public List<UsbDevice> queryDevice() {
        Collection<UsbDevice> devices = usbManager.getDeviceList().values();
        return new ArrayList<>(devices);
    }

    /**
     * Query video {@link UsbDevice}
     * @return not null
     */
    public List<UsbDevice> queryCamera() {
        List<UsbDevice> list = new ArrayList<>();
        Collection<UsbDevice> devices = usbManager.getDeviceList().values();
        for (UsbDevice device: devices) {
            if (device.getDeviceClass() == UsbConstants.USB_CLASS_VIDEO) {
                list.add(device);
            }
        }
        return list;
    }

    /**
     * Request accessing UsbDevice permission.
     * Result callback on {@link DeviceWatcher#onActive(UsbDevice, UsbDeviceConnection)}.
     * @param device {@link android.hardware.usb.UsbDevice}
     */
    public void request(UsbDevice device) {
        if (device == null) {
            Log.e(TAG, "openCamera(): device is null.");
        } else if (watcher == null) {
            Log.e(TAG, "openCamera(): watcher is null.");
        } else if (usbManager.hasPermission(device)) {
            watcher.onActive(device, usbManager.openDevice(device));
        } else {
            usbManager.requestPermission(device, pi);
        }
    }

    /**
     * Request permission to access UsbDevice that filter target device by vid and pid.
     * Result callback on {@link DeviceWatcher#onActive(UsbDevice, UsbDeviceConnection)}.
     * @param vid {@link UsbDevice#getVendorId()}
     * @param pid {@link UsbDevice#getProductId()}
     */
    public void request(int vid, int pid) {
        List<UsbDevice> devices = queryDevice(vid, pid);
        for (UsbDevice device : devices) request(device);
    }

    /**
     * If call it, can't receive any {@link android.hardware.usb.UsbDevice} state
     */
    public void release() {
        if (this.context != null) {
            this.context.unregisterReceiver(usbReceiver);
        }
        this.watcher = null;
    }

//=================================================================================================

    private final String ACTION_PERMISSION = "android.camera.uvc.USB_PERMISSION." + hashCode();

    private final BroadcastReceiver usbReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (watcher == null) {
                Log.e(TAG, "onReceive(): watcher is null.");
            } else if (ACTION_PERMISSION.equals(action)) {
                UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                    watcher.onActive(device, usbManager.openDevice(device));
                } else {
                    watcher.onActive(device, null);
                }
            } else if (UsbManager.ACTION_USB_DEVICE_ATTACHED.equals(action)) {
                watcher.onAttach(intent.getParcelableExtra(UsbManager.EXTRA_DEVICE));
            } else if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
                watcher.onDetach(intent.getParcelableExtra(UsbManager.EXTRA_DEVICE));
            }
        }
    };

}
