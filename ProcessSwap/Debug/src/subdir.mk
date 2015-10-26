################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/LibSwap.c \
../src/ProcessSwap.c \
../src/config_swat.c \
../src/particion_swat.c \
../src/protocolo_swat.c \
../src/servidor_swat.c 

OBJS += \
./src/LibSwap.o \
./src/ProcessSwap.o \
./src/config_swat.o \
./src/particion_swat.o \
./src/protocolo_swat.o \
./src/servidor_swat.o 

C_DEPS += \
./src/LibSwap.d \
./src/ProcessSwap.d \
./src/config_swat.d \
./src/particion_swat.d \
./src/protocolo_swat.d \
./src/servidor_swat.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


