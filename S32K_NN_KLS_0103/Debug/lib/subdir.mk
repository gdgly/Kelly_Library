################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../lib/Bootloader.c" \
"../lib/Err_Sts.c" \
"../lib/FOC.c" \
"../lib/Flash.c" \
"../lib/Math.c" \
"../lib/Pedal.c" \
"../lib/TEMP.c" \
"../lib/Voltage.c" \
"../lib/Voltage2.c" \

C_SRCS += \
../lib/Bootloader.c \
../lib/Err_Sts.c \
../lib/FOC.c \
../lib/Flash.c \
../lib/Math.c \
../lib/Pedal.c \
../lib/TEMP.c \
../lib/Voltage.c \
../lib/Voltage2.c \

C_DEPS_QUOTED += \
"./lib/Bootloader.d" \
"./lib/Err_Sts.d" \
"./lib/FOC.d" \
"./lib/Flash.d" \
"./lib/Math.d" \
"./lib/Pedal.d" \
"./lib/TEMP.d" \
"./lib/Voltage.d" \
"./lib/Voltage2.d" \

OBJS_QUOTED += \
"./lib/Bootloader.o" \
"./lib/Err_Sts.o" \
"./lib/FOC.o" \
"./lib/Flash.o" \
"./lib/Math.o" \
"./lib/Pedal.o" \
"./lib/TEMP.o" \
"./lib/Voltage.o" \
"./lib/Voltage2.o" \

C_DEPS += \
./lib/Bootloader.d \
./lib/Err_Sts.d \
./lib/FOC.d \
./lib/Flash.d \
./lib/Math.d \
./lib/Pedal.d \
./lib/TEMP.d \
./lib/Voltage.d \
./lib/Voltage2.d \

OBJS_OS_FORMAT += \
./lib/Bootloader.o \
./lib/Err_Sts.o \
./lib/FOC.o \
./lib/Flash.o \
./lib/Math.o \
./lib/Pedal.o \
./lib/TEMP.o \
./lib/Voltage.o \
./lib/Voltage2.o \

OBJS += \
./lib/Bootloader.o \
./lib/Err_Sts.o \
./lib/FOC.o \
./lib/Flash.o \
./lib/Math.o \
./lib/Pedal.o \
./lib/TEMP.o \
./lib/Voltage.o \
./lib/Voltage2.o \


# Each subdirectory must supply rules for building sources it contributes
lib/Bootloader.o: ../lib/Bootloader.c
	@echo 'Building file: $<'
	@echo 'Executing target #13 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@lib/Bootloader.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "lib/Bootloader.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

lib/Err_Sts.o: ../lib/Err_Sts.c
	@echo 'Building file: $<'
	@echo 'Executing target #14 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@lib/Err_Sts.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "lib/Err_Sts.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

lib/FOC.o: ../lib/FOC.c
	@echo 'Building file: $<'
	@echo 'Executing target #15 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@lib/FOC.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "lib/FOC.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

lib/Flash.o: ../lib/Flash.c
	@echo 'Building file: $<'
	@echo 'Executing target #16 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@lib/Flash.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "lib/Flash.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

lib/Math.o: ../lib/Math.c
	@echo 'Building file: $<'
	@echo 'Executing target #17 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@lib/Math.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "lib/Math.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

lib/Pedal.o: ../lib/Pedal.c
	@echo 'Building file: $<'
	@echo 'Executing target #18 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@lib/Pedal.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "lib/Pedal.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

lib/TEMP.o: ../lib/TEMP.c
	@echo 'Building file: $<'
	@echo 'Executing target #19 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@lib/TEMP.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "lib/TEMP.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

lib/Voltage.o: ../lib/Voltage.c
	@echo 'Building file: $<'
	@echo 'Executing target #20 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@lib/Voltage.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "lib/Voltage.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

lib/Voltage2.o: ../lib/Voltage2.c
	@echo 'Building file: $<'
	@echo 'Executing target #21 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@lib/Voltage2.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "lib/Voltage2.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


