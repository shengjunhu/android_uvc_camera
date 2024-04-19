package android.camera.uvc;

import android.os.Parcel;
import android.os.Parcelable;

import androidx.annotation.FloatRange;
import androidx.annotation.IntDef;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * @Author:
 * @Date:2024/4/3
 * @Desc:
 */
public final class SupportInfo implements Parcelable {

    /**
     * Callback frame format.
     * @see SupportInfo#format
     */
    @IntDef({FRAME_FORMAT_ORIGIN, FRAME_FORMAT_I420, FRAME_FORMAT_RGB})
    @Retention(RetentionPolicy.SOURCE)
    public @interface FrameFormat {}
    /**
     * Callback original data from camera
     * and FrameRotate, FrameFlip, Picture and Video are unavailable.
     */
    public static final int FRAME_FORMAT_ORIGIN = 0x0;
    /**
     * Callback format:
     * @see android.graphics.ImageFormat#YUV_420_888 (YUV420P: YYYY U V).
     */
    public static final int FRAME_FORMAT_I420   = 0xE;
    /**
     * Callback format:
     * @see android.graphics.ImageFormat#FLEX_RGB_888 (RGB: R8 B8 G8).
     */
    public static final int FRAME_FORMAT_RGB    = 0xF;

    /**
     * Callback frame rotate.
     * @see SupportInfo#rotate
     */
    @IntDef({FRAME_ROTATE_NONE, FRAME_ROTATE_90, FRAME_ROTATE_180, FRAME_ROTATE_270})
    @Retention(RetentionPolicy.SOURCE)
    public @interface FrameRotate {}
    /** No rotate of every frame. */
    public static final int FRAME_ROTATE_NONE   = 0x00;
    /** Rotate 90 clockwise of every frame. */
    public static final int FRAME_ROTATE_90     = 0x5A;
    /** Rotate 18 clockwise of every frame. */
    public static final int FRAME_ROTATE_180    = 0xB4;
    /** Rotate 270 clockwise of every frame. */
    public static final int FRAME_ROTATE_270    = 0x10E;

    /**
     * Callback frame flip.
     * @see SupportInfo#flip
     */
    @IntDef({FRAME_FLIP_NONE, FRAME_FLIP_H, FRAME_FLIP_V})
    @Retention(RetentionPolicy.SOURCE)
    public @interface FrameFlip {}
    /** No frame will to be flip. */
    public static final int FRAME_FLIP_NONE     = 0x0;
    /** Flip left and right of every frame. */
    public static final int FRAME_FLIP_H        =-0x1;
    /** Flip Top and bottom of every frame. */
    public static final int FRAME_FLIP_V        = 0x1;

    /**
     * Video format.
     * @see SupportInfo#video
     */
    @IntDef({VIDEO_FORMAT_MP4, VIDEO_FORMAT_H264})
    @Retention(RetentionPolicy.SOURCE)
    public @interface VideoFormat {}
    public static final int VIDEO_FORMAT_MP4    = 0x0;
    public static final int VIDEO_FORMAT_H264   = 0x1;

    /** support index */
    final int id;
    /** support format */
    final int fmt;
    /** support name */
    final String name;
    /** support width */
    final int width;
    /** support height */
    final int height;
    /** support fps */
    final int fps;

    /**
     * USB data transfer bandwidth ratio
     * bandwidth ∈ [0.10f, 1.00f].
     */
    @FloatRange(from = 0.10f, to = 1.00f)
    float bandwidth         = 1.00f;
    /**
     * Callback data format:
     */
    @FrameFormat int format = FRAME_FORMAT_ORIGIN;
    /**
     * Callback data orientation:
     */
    @FrameRotate int rotate = FRAME_ROTATE_NONE;
    /**
     * Callback data flip:
     */
    @FrameFlip int flip     = FRAME_FLIP_NONE;
    /**
     * Record video format:
     */
    @VideoFormat int video  = VIDEO_FORMAT_MP4;
    /**
     * Record video save dir:
     * video name: ${dir}/VID_yyyMMdd_HHmmss.mp4
     * @see CameraDevice#startVideo()
     */
    String dir;

    /**
     * Support information of extended cameras
     * @param id     support index
     * @param fmt    support format
     * @param name   support format name
     * @param width  support width
     * @param height support height
     * @param fps    support fps
     */
    SupportInfo(int id, int fmt, String name, int width, int height, int fps) {
        this.id = id;
        this.fmt = fmt;
        this.name = name;
        this.width = width;
        this.height = height;
        this.fps = fps;
    }

    public static final Creator<SupportInfo> CREATOR = new Creator<SupportInfo>() {
        @Override
        public SupportInfo createFromParcel(Parcel source) {
            return new SupportInfo(source);
        }

        @Override
        public SupportInfo[] newArray(int size) {
            return new SupportInfo[size];
        }
    };

    @Override
    public void writeToParcel(Parcel parcel, int flags) {
        parcel.writeInt(id);
        parcel.writeInt(fmt);
        parcel.writeString(name);
        parcel.writeInt(width);
        parcel.writeInt(height);
        parcel.writeInt(fps);
        parcel.writeFloat(bandwidth);
        parcel.writeInt(format);
        parcel.writeInt(rotate);
        parcel.writeInt(video);
        parcel.writeString(dir);
    }

    protected SupportInfo(Parcel in) {
        this.id = in.readInt();
        this.fmt = in.readInt();
        this.name = in.readString();
        this.width = in.readInt();
        this.height = in.readInt();
        this.fps = in.readInt();
        this.bandwidth = in.readInt();
        this.format = in.readInt();
        this.rotate = in.readInt();
        this.video = in.readInt();
        this.dir = in.readString();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public String toString() {
        return "SupportInfo{" +
                "id=" + id +
                ", fmt=" + fmt +
                ", name='" + name + '\'' +
                ", width=" + width +
                ", height=" + height +
                ", fps=" + fps +
                ", bandwidth=" + bandwidth +
                ", format=" + format +
                ", rotate=" + rotate +
                ", flip=" + flip +
                ", video=" + video +
                ", dir='" + dir + '\'' +
                '}';
    }

}
