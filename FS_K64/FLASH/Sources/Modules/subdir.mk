################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Modules/mAccelMagneto.c" \
"../Sources/Modules/mAd.c" \
"../Sources/Modules/mCpu.c" \
"../Sources/Modules/mDac.c" \
"../Sources/Modules/mDelay.c" \
"../Sources/Modules/mLeds.c" \
"../Sources/Modules/mRS232.c" \
"../Sources/Modules/mSpi.c" \
"../Sources/Modules/mSwitch.c" \
"../Sources/Modules/mTimer.c" \
"../Sources/Modules/mWifi.c" \

C_SRCS += \
../Sources/Modules/mAccelMagneto.c \
../Sources/Modules/mAd.c \
../Sources/Modules/mCpu.c \
../Sources/Modules/mDac.c \
../Sources/Modules/mDelay.c \
../Sources/Modules/mLeds.c \
../Sources/Modules/mRS232.c \
../Sources/Modules/mSpi.c \
../Sources/Modules/mSwitch.c \
../Sources/Modules/mTimer.c \
../Sources/Modules/mWifi.c \

OBJS += \
./Sources/Modules/mAccelMagneto.o \
./Sources/Modules/mAd.o \
./Sources/Modules/mCpu.o \
./Sources/Modules/mDac.o \
./Sources/Modules/mDelay.o \
./Sources/Modules/mLeds.o \
./Sources/Modules/mRS232.o \
./Sources/Modules/mSpi.o \
./Sources/Modules/mSwitch.o \
./Sources/Modules/mTimer.o \
./Sources/Modules/mWifi.o \

C_DEPS += \
./Sources/Modules/mAccelMagneto.d \
./Sources/Modules/mAd.d \
./Sources/Modules/mCpu.d \
./Sources/Modules/mDac.d \
./Sources/Modules/mDelay.d \
./Sources/Modules/mLeds.d \
./Sources/Modules/mRS232.d \
./Sources/Modules/mSpi.d \
./Sources/Modules/mSwitch.d \
./Sources/Modules/mTimer.d \
./Sources/Modules/mWifi.d \

OBJS_QUOTED += \
"./Sources/Modules/mAccelMagneto.o" \
"./Sources/Modules/mAd.o" \
"./Sources/Modules/mCpu.o" \
"./Sources/Modules/mDac.o" \
"./Sources/Modules/mDelay.o" \
"./Sources/Modules/mLeds.o" \
"./Sources/Modules/mRS232.o" \
"./Sources/Modules/mSpi.o" \
"./Sources/Modules/mSwitch.o" \
"./Sources/Modules/mTimer.o" \
"./Sources/Modules/mWifi.o" \

C_DEPS_QUOTED += \
"./Sources/Modules/mAccelMagneto.d" \
"./Sources/Modules/mAd.d" \
"./Sources/Modules/mCpu.d" \
"./Sources/Modules/mDac.d" \
"./Sources/Modules/mDelay.d" \
"./Sources/Modules/mLeds.d" \
"./Sources/Modules/mRS232.d" \
"./Sources/Modules/mSpi.d" \
"./Sources/Modules/mSwitch.d" \
"./Sources/Modules/mTimer.d" \
"./Sources/Modules/mWifi.d" \

OBJS_OS_FORMAT += \
./Sources/Modules/mAccelMagneto.o \
./Sources/Modules/mAd.o \
./Sources/Modules/mCpu.o \
./Sources/Modules/mDac.o \
./Sources/Modules/mDelay.o \
./Sources/Modules/mLeds.o \
./Sources/Modules/mRS232.o \
./Sources/Modules/mSpi.o \
./Sources/Modules/mSwitch.o \
./Sources/Modules/mTimer.o \
./Sources/Modules/mWifi.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Modules/mAccelMagneto.o: ../Sources/Modules/mAccelMagneto.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Modules/mAccelMagneto.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Modules/mAccelMagneto.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Modules/mAd.o: ../Sources/Modules/mAd.c
	@echo 'Building file: $<'
	@echo 'Executing target #5 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Modules/mAd.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Modules/mAd.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Modules/mCpu.o: ../Sources/Modules/mCpu.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Modules/mCpu.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Modules/mCpu.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Modules/mDac.o: ../Sources/Modules/mDac.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Modules/mDac.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Modules/mDac.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Modules/mDelay.o: ../Sources/Modules/mDelay.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Modules/mDelay.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Modules/mDelay.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Modules/mLeds.o: ../Sources/Modules/mLeds.c
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Modules/mLeds.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Modules/mLeds.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Modules/mRS232.o: ../Sources/Modules/mRS232.c
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Modules/mRS232.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Modules/mRS232.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Modules/mSpi.o: ../Sources/Modules/mSpi.c
	@echo 'Building file: $<'
	@echo 'Executing target #11 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Modules/mSpi.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Modules/mSpi.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Modules/mSwitch.o: ../Sources/Modules/mSwitch.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Modules/mSwitch.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Modules/mSwitch.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Modules/mTimer.o: ../Sources/Modules/mTimer.c
	@echo 'Building file: $<'
	@echo 'Executing target #13 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Modules/mTimer.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Modules/mTimer.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Modules/mWifi.o: ../Sources/Modules/mWifi.c
	@echo 'Building file: $<'
	@echo 'Executing target #14 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Modules/mWifi.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Modules/mWifi.o"
	@echo 'Finished building: $<'
	@echo ' '


