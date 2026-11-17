# idf_component_register(SRCS "main.cpp"
#                     INCLUDE_DIRS "."
#                     REQUIRES CDAudioMidiDevice arduino-esp32)

set(DEPENDENCIES
    arduino-esp32
    Adafruit_SSD1306
    Adafruit-GFX-Library
    Adafruit_BusIO
)

idf_component_register(SRC_DIRS "." "src"
                       INCLUDE_DIRS "." "lib"
                       REQUIRES ${DEPENDENCIES})