RM = rm -rf

USER_OBJS =

LIBS = -lcommons -lpthread

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
../src/Cache13-CascaNuts-ProcessPlanificador.c \
../src/PCB.c \
../src/servidor.c

OBJS += \
./src/Cache13-CascaNuts-ProcessPlanificador.o \
./src/PCB.o \
./src/servidor.o

C_DEPS += \
./src/Cache13-CascaNuts-ProcessPlanificador.d \
./src/PCB.d \
./src/servidor.d

# Add inputs and outputs from these tool invocations to the build variables

# All Target
all: Cache13-CascaNuts-ProcessPlanificador

# Tool invocations
Cache13-CascaNuts-ProcessPlanificador: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "Cache13-CascaNuts-ProcessPlanificador" $(OBJS) $(USER_OBJS) $(LIBS)
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
	$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) Cache13-CascaNuts-ProcessPlanificador
	@echo ' '

.PHONY: all clean dependents
