################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/clientcfg.c 

CPP_SRCS += \
../src/client_config.cpp \
../src/client_device.cpp \
../src/client_loop.cpp \
../src/common.cpp \
../src/homekit_accessories.cpp \
../src/homekit_classes.cpp \
../src/homekit_controller_record.cpp \
../src/homekit_loop.cpp \
../src/homekit_protocol_accessory.cpp \
../src/homekit_protocol_network.cpp \
../src/supla_homekit_bridge.cpp 

OBJS += \
./src/client_config.o \
./src/client_device.o \
./src/client_loop.o \
./src/clientcfg.o \
./src/common.o \
./src/homekit_accessories.o \
./src/homekit_classes.o \
./src/homekit_controller_record.o \
./src/homekit_loop.o \
./src/homekit_protocol_accessory.o \
./src/homekit_protocol_network.o \
./src/supla_homekit_bridge.o 

C_DEPS += \
./src/clientcfg.d 

CPP_DEPS += \
./src/client_config.d \
./src/client_device.d \
./src/client_loop.d \
./src/common.d \
./src/homekit_accessories.d \
./src/homekit_classes.d \
./src/homekit_controller_record.d \
./src/homekit_loop.d \
./src/homekit_protocol_accessory.d \
./src/homekit_protocol_network.d \
./src/supla_homekit_bridge.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__DEBUG=1 -D__NO_USER -D__NO_DATABASE -I$(SSLDIR)/include -I/opt/local/include -I../src/json -O0 -g3 -Wall -fsigned-char  -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -D__DEBUG=1 -D__NO_DATABASE -D__NO_USER -I$(SSLDIR)/include -I../src/json -O0 -g3 -Wall -fsigned-char  -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


