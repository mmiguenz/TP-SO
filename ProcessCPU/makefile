RM = rm -rf

USER_OBJS =

LIBS = -lpthread -lcommons

O_SRCS :=
C_SRCS :=
S_UPPER_SRCS :=
OBJ_SRCS :=
ASM_SRCS :=
OBJS :=
C_DEPS :=
EXECUTABLES :=

# Every subdirectory with source files must be described here
SUBDIRS := \
src \

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../src/ProcessCPU.c \
../src/cliente.c \
../src/hilito.c

OBJS += \
./src/ProcessCPU.o \
./src/cliente.o \
./src/hilito.o

C_DEPS += \
./src/ProcessCPU.d \
./src/cliente.d \
./src/hilito.d

# All Target
all: ProcessCPU

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Tool invocations
ProcessCPU: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "ProcessCPU" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) ProcessCPU
	@echo ' '

.PHONY: all clean dependents
