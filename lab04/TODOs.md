# Calibration Run (mandatory)

Run ts_test.

Touch all 4 corners.

Record raw X/Y min/max → update TS_MIN_X, TS_MAX_X, TS_MIN_Y, TS_MAX_Y.

Find a good PRESSURE_THRESHOLD (test taps vs. light touches).

Clamp Safety (optional, recommended)

Add bounds check in linear_interpolate() to avoid out-of-range scaling.

# Ghost Text (optional)

The “erase text” method works because we print at fixed coords. If we notice leftover characters (e.g., when shorter numbers overwrite longer ones), pad with spaces in sprintf():

sprintf(buffer1, "RAW  x:%4d y:%4d z:%4d", p.x, p.y, p.z);


(we already do this with %4d, so you’re mostly safe.)

# Crosshair Thickness (optional)

If it’s hard to see, draw double-pixel lines instead of single.

# Rotation Consistency (check)

We set tft_setRotation(3) in ts_lcd_init().

If our screen looks “rotated wrong” (e.g., mirrored X/Y), we might wanna change this to 0/1/2.

# Run on Efe's computer using this: 
Make sure these are installed: 

arm-none-eabi-gcc --version
sudo apt-get update
sudo apt-get install -y gcc-arm-none-eabi libnewlib-arm-none-eabi


rm -rf build
mkdir build
cd build

cmake .. -DPICO_SDK_PATH=../pico-sdk

make -j$(nproc)

