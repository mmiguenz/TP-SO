

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
../src/AdministradordeMemoria.c \
../src/Cliente.c \
../src/LoggingAdmMem.c \
../src/Memoria.c \
../src/TADConfig.c \
../src/Tlb.c \
../src/servidor.c

OBJS += \
./src/AdministradordeMemoria.o \
./src/Cliente.o \
./src/LoggingAdmMem.o \
./src/Memoria.o \
./src/TADConfig.o \
./src/Tlb.o \
./src/servidor.o

C_DEPS += \
./src/AdministradordeMemoria.d \
./src/Cliente.d \
./src/LoggingAdmMem.d \
./src/Memoria.d \
./src/TADConfig.d \
./src/Tlb.d \
./src/servidor.d

# All Target
all: ProcesoAdministradordeMemoria

# Tool invocations
ProcesoAdministradordeMemoria: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "ProcesoAdministradordeMemoria" $(OBJS) $(USER_OBJS) $(LIBS)
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
	$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) ProcesoAdministradordeMemoria
	@echo ' '

.PHONY: all clean dependents
