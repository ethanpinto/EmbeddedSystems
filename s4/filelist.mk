########################################################################
# LIBRARY SOURCES - MUST BE IN THE SAME FOLDER as main.c (DO NOT CHANGE)
########################################################################

# Including memory heap model
LIBSRCS += $(FREERTOS_PATH)/portable/MemMang/heap_1.c

# Set mylib folder path.
# Do not change the MYLIB_PATH variable name.
# ONLY use relative file paths that start with $(SOURCELIB_ROOT)../
# DO NOT USE absolute file paths (e.g. /home/users/myuser/mydir)
# e.g. MYLIBPATH=$(SOURCELIB_ROOT)/../mylib
MYLIB_PATH=$(SOURCELIB_ROOT)/../repo/mylib

# Set folder path with header files to include.
# ONLY use relative file paths that start with $(SOURCELIB_ROOT)../
# DO NOT USE absolute file paths (e.g. /home/users/myuser/mydir)
CFLAGS += -I$(MYLIB_PATH)

# List all c files locations that must be included (use space as separate e.g. LIBSRCS += path_to/file1.c path_to/file2.c)
# ONLY use relative file paths that start with $(SOURCELIB_ROOT)../
# DO NOT USE absolute file paths (e.g. /home/users/myuser/mydir)
LIBSRCS += $(MYLIB_PATH)/s4642286_oled.c $(MYLIB_PATH)/s4642286_joystick.c 

SSD1306_PATH=$(SOURCELIB_ROOT)/components/peripherals/ssd1306/nucleo-f429

CFLAGS += -I$(SSD1306_PATH)

LIBSRCS += $(wildcard $(SSD1306_PATH)/*.c)
