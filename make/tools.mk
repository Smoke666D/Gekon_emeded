#PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
#ifdef GCC_PATH
#CC = $(GCC_PATH)/$(PREFIX)gcc
#AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
#CP = $(GCC_PATH)/$(PREFIX)objcopy
#SZ = $(GCC_PATH)/$(PREFIX)size
#else
#CC = $(PREFIX)gcc
#AS = $(PREFIX)gcc -x assembler-with-cpp
#CP = $(PREFIX)objcopy
#SZ = $(PREFIX)size
#endif


CROSS_COMPILE ?= arm-none-eabi-
CC		    = $(CROSS_COMPILE)gcc
OBJCOPY		= $(CROSS_COMPILE)objcopy
SIZE		  = $(CROSS_COMPILE)size
AS        = $(CROSS_COMPILE)gcc -x assembler-with-cpp
STFLASH		= st-flash
STUTIL		= st-util
OPENOCD   = openocd
CPPCHECK	= cppcheck