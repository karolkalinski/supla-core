################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/security/chacha20_simple.c \
../src/security/cstr.c \
../src/security/curve25519-donna.c \
../src/security/ed25519.c \
../src/security/hkdf.c \
../src/security/hmac.c \
../src/security/poly1305.c \
../src/security/sha.c \
../src/security/srp.c \
../src/security/srp6_server.c \
../src/security/t_conf.c \
../src/security/t_conv.c \
../src/security/t_math.c \
../src/security/t_misc.c \
../src/security/t_pw.c \
../src/security/t_read.c \
../src/security/t_truerand.c 

OBJS += \
./src/security/chacha20_simple.o \
./src/security/cstr.o \
./src/security/curve25519-donna.o \
./src/security/ed25519.o \
./src/security/hkdf.o \
./src/security/hmac.o \
./src/security/poly1305.o \
./src/security/sha.o \
./src/security/srp.o \
./src/security/srp6_server.o \
./src/security/t_conf.o \
./src/security/t_conv.o \
./src/security/t_math.o \
./src/security/t_misc.o \
./src/security/t_pw.o \
./src/security/t_read.o \
./src/security/t_truerand.o 

C_DEPS += \
./src/security/chacha20_simple.d \
./src/security/cstr.d \
./src/security/curve25519-donna.d \
./src/security/ed25519.d \
./src/security/hkdf.d \
./src/security/hmac.d \
./src/security/poly1305.d \
./src/security/sha.d \
./src/security/srp.d \
./src/security/srp6_server.d \
./src/security/t_conf.d \
./src/security/t_conv.d \
./src/security/t_math.d \
./src/security/t_misc.d \
./src/security/t_pw.d \
./src/security/t_read.d \
./src/security/t_truerand.d 


# Each subdirectory must supply rules for building sources it contributes
src/security/%.o: ../src/security/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -D__NO_USER -D__NO_DATABASE -I$(SSLDIR)/include -I../src/json -O3 -Wall -fsigned-char  -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


