if (DEFINED ENV{PICO_SDK_PATH} AND NOT PICO_SDK_PATH)
  set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()

if (NOT PICO_SDK_PATH)
  include(FetchContent)
  FetchContent_Declare(pico_sdk
    GIT_REPOSITORY https://github.com/raspberrypi/pico-sdk.git
    GIT_TAG master
  )
  FetchContent_MakeAvailable(pico_sdk)
  set(PICO_SDK_PATH ${pico_sdk_SOURCE_DIR})
endif()

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
