################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../src/Control.c" \
"../src/ETS.c" \
"../src/Events.c" \
"../src/Hall.c" \
"../src/Motor.c" \
"../src/Platform.c" \
"../src/TorqueMode.c" \
"../src/Variable.c" \
"../src/main.c" \

C_SRCS += \
../src/Control.c \
../src/ETS.c \
../src/Events.c \
../src/Hall.c \
../src/Motor.c \
../src/Platform.c \
../src/TorqueMode.c \
../src/Variable.c \
../src/main.c \

C_DEPS_QUOTED += \
"./src/Control.d" \
"./src/ETS.d" \
"./src/Events.d" \
"./src/Hall.d" \
"./src/Motor.d" \
"./src/Platform.d" \
"./src/TorqueMode.d" \
"./src/Variable.d" \
"./src/main.d" \

OBJS_QUOTED += \
"./src/Control.o" \
"./src/ETS.o" \
"./src/Events.o" \
"./src/Hall.o" \
"./src/Motor.o" \
"./src/Platform.o" \
"./src/TorqueMode.o" \
"./src/Variable.o" \
"./src/main.o" \

C_DEPS += \
./src/Control.d \
./src/ETS.d \
./src/Events.d \
./src/Hall.d \
./src/Motor.d \
./src/Platform.d \
./src/TorqueMode.d \
./src/Variable.d \
./src/main.d \

OBJS_OS_FORMAT += \
./src/Control.o \
./src/ETS.o \
./src/Events.o \
./src/Hall.o \
./src/Motor.o \
./src/Platform.o \
./src/TorqueMode.o \
./src/Variable.o \
./src/main.o \

OBJS += \
./src/Control.o \
./src/ETS.o \
./src/Events.o \
./src/Hall.o \
./src/Motor.o \
./src/Platform.o \
./src/TorqueMode.o \
./src/Variable.o \
./src/main.o \


# Each subdirectory must supply rules for building sources it contributes
src/Control.o: ../src/Control.c
	@echo 'Building file: $<'
	@echo 'Executing target #22 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/Control.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "src/Control.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ETS.o: ../src/ETS.c
	@echo 'Building file: $<'
	@echo 'Executing target #23 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/ETS.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "src/ETS.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Events.o: ../src/Events.c
	@echo 'Building file: $<'
	@echo 'Executing target #24 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/Events.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "src/Events.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Hall.o: ../src/Hall.c
	@echo 'Building file: $<'
	@echo 'Executing target #25 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/Hall.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "src/Hall.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Motor.o: ../src/Motor.c
	@echo 'Building file: $<'
	@echo 'Executing target #26 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/Motor.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "src/Motor.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Platform.o: ../src/Platform.c
	@echo 'Building file: $<'
	@echo 'Executing target #27 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/Platform.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "src/Platform.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/TorqueMode.o: ../src/TorqueMode.c
	@echo 'Building file: $<'
	@echo 'Executing target #28 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/TorqueMode.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "src/TorqueMode.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Variable.o: ../src/Variable.c
	@echo 'Building file: $<'
	@echo 'Executing target #29 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/Variable.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "src/Variable.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/main.o: ../src/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #30 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/main.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "src/main.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


