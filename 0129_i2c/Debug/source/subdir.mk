################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/0129_i2c.c \
../source/semihost_hardfault.c 

OBJS += \
./source/0129_i2c.o \
./source/semihost_hardfault.o 

C_DEPS += \
./source/0129_i2c.d \
./source/semihost_hardfault.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DPRINTF_FLOAT_ENABLE=0 -D__USE_CMSIS -DCR_INTEGER_PRINTF -DSDK_DEBUGCONSOLE=1 -D__MCUXPRESSO -DDEBUG -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -I"C:\acch\0129_i2c\board" -I"C:\acch\0129_i2c\source" -I"C:\acch\0129_i2c" -I"C:\acch\0129_i2c\drivers" -I"C:\acch\0129_i2c\CMSIS" -I"C:\acch\0129_i2c\utilities" -I"C:\acch\0129_i2c\startup" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


