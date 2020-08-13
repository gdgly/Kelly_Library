################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../init/ADC_Init.c" \
"../init/CAN_Init.c" \
"../init/FTM_Init.c" \
"../init/LPIT_Init.c" \
"../init/LUART_Init.c" \
"../init/PDB_Init.c" \
"../init/SPI_Init.c" \
"../init/clocks_and_modes.c" \
"../init/interrupt.c" \

C_SRCS += \
../init/ADC_Init.c \
../init/CAN_Init.c \
../init/FTM_Init.c \
../init/LPIT_Init.c \
../init/LUART_Init.c \
../init/PDB_Init.c \
../init/SPI_Init.c \
../init/clocks_and_modes.c \
../init/interrupt.c \

C_DEPS_QUOTED += \
"./init/ADC_Init.d" \
"./init/CAN_Init.d" \
"./init/FTM_Init.d" \
"./init/LPIT_Init.d" \
"./init/LUART_Init.d" \
"./init/PDB_Init.d" \
"./init/SPI_Init.d" \
"./init/clocks_and_modes.d" \
"./init/interrupt.d" \

OBJS_QUOTED += \
"./init/ADC_Init.o" \
"./init/CAN_Init.o" \
"./init/FTM_Init.o" \
"./init/LPIT_Init.o" \
"./init/LUART_Init.o" \
"./init/PDB_Init.o" \
"./init/SPI_Init.o" \
"./init/clocks_and_modes.o" \
"./init/interrupt.o" \

C_DEPS += \
./init/ADC_Init.d \
./init/CAN_Init.d \
./init/FTM_Init.d \
./init/LPIT_Init.d \
./init/LUART_Init.d \
./init/PDB_Init.d \
./init/SPI_Init.d \
./init/clocks_and_modes.d \
./init/interrupt.d \

OBJS_OS_FORMAT += \
./init/ADC_Init.o \
./init/CAN_Init.o \
./init/FTM_Init.o \
./init/LPIT_Init.o \
./init/LUART_Init.o \
./init/PDB_Init.o \
./init/SPI_Init.o \
./init/clocks_and_modes.o \
./init/interrupt.o \

OBJS += \
./init/ADC_Init.o \
./init/CAN_Init.o \
./init/FTM_Init.o \
./init/LPIT_Init.o \
./init/LUART_Init.o \
./init/PDB_Init.o \
./init/SPI_Init.o \
./init/clocks_and_modes.o \
./init/interrupt.o \


# Each subdirectory must supply rules for building sources it contributes
init/ADC_Init.o: ../init/ADC_Init.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@init/ADC_Init.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "init/ADC_Init.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

init/CAN_Init.o: ../init/CAN_Init.c
	@echo 'Building file: $<'
	@echo 'Executing target #5 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@init/CAN_Init.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "init/CAN_Init.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

init/FTM_Init.o: ../init/FTM_Init.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@init/FTM_Init.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "init/FTM_Init.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

init/LPIT_Init.o: ../init/LPIT_Init.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@init/LPIT_Init.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "init/LPIT_Init.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

init/LUART_Init.o: ../init/LUART_Init.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@init/LUART_Init.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "init/LUART_Init.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

init/PDB_Init.o: ../init/PDB_Init.c
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@init/PDB_Init.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "init/PDB_Init.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

init/SPI_Init.o: ../init/SPI_Init.c
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@init/SPI_Init.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "init/SPI_Init.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

init/clocks_and_modes.o: ../init/clocks_and_modes.c
	@echo 'Building file: $<'
	@echo 'Executing target #11 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@init/clocks_and_modes.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "init/clocks_and_modes.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

init/interrupt.o: ../init/interrupt.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@init/interrupt.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "init/interrupt.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


