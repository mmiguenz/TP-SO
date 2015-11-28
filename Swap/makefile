

RM = rm -rf

USER_OBJS =

LIBS = -lcommons

O_SRCS =
C_SRCS =
S_UPPER_SRCS =
OBJ_SRCS =
ASM_SRCS =
OBJS =
C_DEPS =
EXECUTABLES =

# Every subdirectory with source files must be described here
SUBDIRS := \
src \

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../src/Swap.c \
../src/logueoSwap.c \
../src/particionSwap.c \
../src/servidor.c \
../src/swapConfig.c

OBJS += \
./src/Swap.o \
./src/logueoSwap.o \
./src/particionSwap.o \
./src/servidor.o \
./src/swapConfig.o

C_DEPS += \
./src/Swap.d \
./src/logueoSwap.d \
./src/particionSwap.d \
./src/servidor.d \
./src/swapConfig.d

# All Target
all: Swap

# Tool invocations
Swap: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "Swap" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Other Targets
clean:
	$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) Swap
	@echo ' '

.PHONY: all clean dependents
