# Compiling the projects with libutils logging code?
DO_LOGGING           := 1

# Links against the wut implementation of newlib, this is useful for using any function
# from the C standard library
WUT_ENABLE_NEWLIB    := 1

# Links against the wut implementation of stdcpp, this is useful for using any function
# from the C++ standard library. This will enable WUT_ENABLE_NEWLIB if you have not already done so.
WUT_ENABLE_CPP       := 1

# By default newlib will allocate 90% of the default heap for use with sbrk & malloc,
# if this is unacceptable to you then you should use this as it replaces the newlib
# malloc functions which ones which redirect to the CafeOS default heap functions
# such as MEMAllocFromDefaultHeap.
WUT_DEFAULT_MALLOC   := 1

# This links in wutdevoptab which is useful for using the libc file functions with
# paths such as fopen("sd:/file.txt", "r") to read files from the sd card
WUT_ENABLE_DEVOPTAB  := 1

# Target filename
TARGET              := cafeloader.mod

# Source directories
SOURCES             := src

# Data directories
DATA                :=

# Include directories
INCLUDES            := src

#---------------------------------------------------------------------------------
# options for code generation and linking
#---------------------------------------------------------------------------------
# Extra C AND C++ compiler flags
COMMON_CFLAGS       :=
# Extra C compiler flags
CFLAGS              :=
# Extra C++ compiler flags
CXXFLAGS            :=
# Extra linking flags for all linking steps
LDFLAGS             :=

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS             := $(WUPSDIR) $(WUT_ROOT)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS                := -lwups -lutilswut -lcoreinit -lnsysnet

#---------------------------------------------------------------------------------
# Will be added to the final lib paths
# example:
# -L$C:/library1/lib
#---------------------------------------------------------------------------------
EXTERNAL_LIBPATHS   :=

#---------------------------------------------------------------------------------
# Will be added to the final include paths
# -IC:/library1/include
#---------------------------------------------------------------------------------
EXTERNAL_INCLUDE    := -I$(WUT_ROOT)/include/libutilswut
