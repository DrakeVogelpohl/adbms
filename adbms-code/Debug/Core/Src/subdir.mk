################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adbms_driver.c \
../Core/Src/adbms_interface.c \
../Core/Src/bms.c \
../Core/Src/bms_can.c \
../Core/Src/current_driver.c \
../Core/Src/main.c \
../Core/Src/nfr_can_driver.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/thermistor_driver.c \
../Core/Src/virtual_timer.c 

OBJS += \
./Core/Src/adbms_driver.o \
./Core/Src/adbms_interface.o \
./Core/Src/bms.o \
./Core/Src/bms_can.o \
./Core/Src/current_driver.o \
./Core/Src/main.o \
./Core/Src/nfr_can_driver.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/thermistor_driver.o \
./Core/Src/virtual_timer.o 

C_DEPS += \
./Core/Src/adbms_driver.d \
./Core/Src/adbms_interface.d \
./Core/Src/bms.d \
./Core/Src/bms_can.d \
./Core/Src/current_driver.d \
./Core/Src/main.d \
./Core/Src/nfr_can_driver.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/thermistor_driver.d \
./Core/Src/virtual_timer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adbms_driver.cyclo ./Core/Src/adbms_driver.d ./Core/Src/adbms_driver.o ./Core/Src/adbms_driver.su ./Core/Src/adbms_interface.cyclo ./Core/Src/adbms_interface.d ./Core/Src/adbms_interface.o ./Core/Src/adbms_interface.su ./Core/Src/bms.cyclo ./Core/Src/bms.d ./Core/Src/bms.o ./Core/Src/bms.su ./Core/Src/bms_can.cyclo ./Core/Src/bms_can.d ./Core/Src/bms_can.o ./Core/Src/bms_can.su ./Core/Src/current_driver.cyclo ./Core/Src/current_driver.d ./Core/Src/current_driver.o ./Core/Src/current_driver.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/nfr_can_driver.cyclo ./Core/Src/nfr_can_driver.d ./Core/Src/nfr_can_driver.o ./Core/Src/nfr_can_driver.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/thermistor_driver.cyclo ./Core/Src/thermistor_driver.d ./Core/Src/thermistor_driver.o ./Core/Src/thermistor_driver.su ./Core/Src/virtual_timer.cyclo ./Core/Src/virtual_timer.d ./Core/Src/virtual_timer.o ./Core/Src/virtual_timer.su

.PHONY: clean-Core-2f-Src

