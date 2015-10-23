################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Interfaces/iAd.c" \
"../Sources/Interfaces/iCpu.c" \
"../Sources/Interfaces/iDac.c" \
"../Sources/Interfaces/iDio.c" \
"../Sources/Interfaces/iFlextimer.c" \
"../Sources/Interfaces/iI2C.c" \
"../Sources/Interfaces/iPit.c" \
"../Sources/Interfaces/iSpi.c" \
"../Sources/Interfaces/iUart.c" \

C_SRCS += \
../Sources/Interfaces/iAd.c \
../Sources/Interfaces/iCpu.c \
../Sources/Interfaces/iDac.c \
../Sources/Interfaces/iDio.c \
../Sources/Interfaces/iFlextimer.c \
../Sources/Interfaces/iI2C.c \
../Sources/Interfaces/iPit.c \
../Sources/Interfaces/iSpi.c \
../Sources/Interfaces/iUart.c \

OBJS += \
./Sources/Interfaces/iAd.o \
./Sources/Interfaces/iCpu.o \
./Sources/Interfaces/iDac.o \
./Sources/Interfaces/iDio.o \
./Sources/Interfaces/iFlextimer.o \
./Sources/Interfaces/iI2C.o \
./Sources/Interfaces/iPit.o \
./Sources/Interfaces/iSpi.o \
./Sources/Interfaces/iUart.o \

C_DEPS += \
./Sources/Interfaces/iAd.d \
./Sources/Interfaces/iCpu.d \
./Sources/Interfaces/iDac.d \
./Sources/Interfaces/iDio.d \
./Sources/Interfaces/iFlextimer.d \
./Sources/Interfaces/iI2C.d \
./Sources/Interfaces/iPit.d \
./Sources/Interfaces/iSpi.d \
./Sources/Interfaces/iUart.d \

OBJS_QUOTED += \
"./Sources/Interfaces/iAd.o" \
"./Sources/Interfaces/iCpu.o" \
"./Sources/Interfaces/iDac.o" \
"./Sources/Interfaces/iDio.o" \
"./Sources/Interfaces/iFlextimer.o" \
"./Sources/Interfaces/iI2C.o" \
"./Sources/Interfaces/iPit.o" \
"./Sources/Interfaces/iSpi.o" \
"./Sources/Interfaces/iUart.o" \

C_DEPS_QUOTED += \
"./Sources/Interfaces/iAd.d" \
"./Sources/Interfaces/iCpu.d" \
"./Sources/Interfaces/iDac.d" \
"./Sources/Interfaces/iDio.d" \
"./Sources/Interfaces/iFlextimer.d" \
"./Sources/Interfaces/iI2C.d" \
"./Sources/Interfaces/iPit.d" \
"./Sources/Interfaces/iSpi.d" \
"./Sources/Interfaces/iUart.d" \

OBJS_OS_FORMAT += \
./Sources/Interfaces/iAd.o \
./Sources/Interfaces/iCpu.o \
./Sources/Interfaces/iDac.o \
./Sources/Interfaces/iDio.o \
./Sources/Interfaces/iFlextimer.o \
./Sources/Interfaces/iI2C.o \
./Sources/Interfaces/iPit.o \
./Sources/Interfaces/iSpi.o \
./Sources/Interfaces/iUart.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Interfaces/iAd.o: ../Sources/Interfaces/iAd.c
	@echo 'Building file: $<'
	@echo 'Executing target #15 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Interfaces/iAd.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Interfaces/iAd.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Interfaces/iCpu.o: ../Sources/Interfaces/iCpu.c
	@echo 'Building file: $<'
	@echo 'Executing target #16 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Interfaces/iCpu.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Interfaces/iCpu.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Interfaces/iDac.o: ../Sources/Interfaces/iDac.c
	@echo 'Building file: $<'
	@echo 'Executing target #17 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Interfaces/iDac.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Interfaces/iDac.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Interfaces/iDio.o: ../Sources/Interfaces/iDio.c
	@echo 'Building file: $<'
	@echo 'Executing target #18 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Interfaces/iDio.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Interfaces/iDio.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Interfaces/iFlextimer.o: ../Sources/Interfaces/iFlextimer.c
	@echo 'Building file: $<'
	@echo 'Executing target #19 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Interfaces/iFlextimer.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Interfaces/iFlextimer.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Interfaces/iI2C.o: ../Sources/Interfaces/iI2C.c
	@echo 'Building file: $<'
	@echo 'Executing target #20 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Interfaces/iI2C.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Interfaces/iI2C.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Interfaces/iPit.o: ../Sources/Interfaces/iPit.c
	@echo 'Building file: $<'
	@echo 'Executing target #21 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Interfaces/iPit.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Interfaces/iPit.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Interfaces/iSpi.o: ../Sources/Interfaces/iSpi.c
	@echo 'Building file: $<'
	@echo 'Executing target #22 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Interfaces/iSpi.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Interfaces/iSpi.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Interfaces/iUart.o: ../Sources/Interfaces/iUart.c
	@echo 'Building file: $<'
	@echo 'Executing target #23 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Interfaces/iUart.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Interfaces/iUart.o"
	@echo 'Finished building: $<'
	@echo ' '


