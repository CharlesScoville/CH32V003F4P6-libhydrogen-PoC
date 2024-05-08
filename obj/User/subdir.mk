################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v00x_it.c \
../User/main.c \
../User/system_ch32v00x.c \
../User/user_ADC.c 

OBJS += \
./User/ch32v00x_it.o \
./User/main.o \
./User/system_ch32v00x.o \
./User/user_ADC.o 

C_DEPS += \
./User/ch32v00x_it.d \
./User/main.d \
./User/system_ch32v00x.d \
./User/user_ADC.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32ec -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -DCH32V0XX -I"C:\Users\charl\mrs_community_workspace\CH32V003F4P6-libhydrogen-PoC\Debug" -I"C:\Users\charl\mrs_community_workspace\CH32V003F4P6-libhydrogen-PoC\libhydrogen" -I"C:\Users\charl\mrs_community_workspace\CH32V003F4P6-libhydrogen-PoC\Core" -I"C:\Users\charl\mrs_community_workspace\CH32V003F4P6-libhydrogen-PoC\User" -I"C:\Users\charl\mrs_community_workspace\CH32V003F4P6-libhydrogen-PoC\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


