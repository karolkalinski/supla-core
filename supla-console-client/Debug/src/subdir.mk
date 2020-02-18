################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ccronexpr.c \
../src/clientcfg.c 

CPP_SRCS += \
../src/channel.cpp \
../src/client_config.cpp \
../src/client_loop.cpp \
../src/crontab_parser.cpp \
../src/notification.cpp \
../src/notification_loop.cpp \
../src/supla-console-client.cpp 

OBJS += \
./src/ccronexpr.o \
./src/channel.o \
./src/client_config.o \
./src/client_loop.o \
./src/clientcfg.o \
./src/crontab_parser.o \
./src/notification.o \
./src/notification_loop.o \
./src/supla-console-client.o 

C_DEPS += \
./src/ccronexpr.d \
./src/clientcfg.d 

CPP_DEPS += \
./src/channel.d \
./src/client_config.d \
./src/client_loop.d \
./src/crontab_parser.d \
./src/notification.d \
./src/notification_loop.d \
./src/supla-console-client.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -D__DEBUG=1 -DCRON_USE_LOCAL_TIME -D__SSOCKET_WRITE_TO_FILE=$(SSOCKET_WRITE_TO_FILE) -I$(SSLDIR)/include -O0 -g3 -Wall -fsigned-char  -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__DEBUG=1 -D__SSOCKET_WRITE_TO_FILE=$(SSOCKET_WRITE_TO_FILE) -I$(SSLDIR)/include -O0 -g3 -Wall -fsigned-char  -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


