/**
 * @brief Rozum Robotics API Source File
 * 
 * @file api.c
 * @author Rozum
 * @date 2018-06-01
 */

/*! \mainpage Rozum Robotics User API
 * 
 * \section intro_section Categories
 * - \ref Common
 * - \ref System_control
 * - \ref Servo_control
 * - \ref Servo_config
 * - \ref Servo_info
 * 
 * \defgroup Common Common functions
 * \defgroup System_control System control functions
 * \defgroup Servo_control Servo control functions
 * \defgroup Servo_config Servo configuration functions
 * \defgroup Servo_info Servo info functions
 */

/* Includes ------------------------------------------------------------------*/
#include "api.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
//! @cond Doxygen_Suppress
#define BIT_SET_UINT_ARRAY(array, bit) ((array)[(bit) / 8] |= (1 << ((bit) % 8)))

#define CHECK_NMT_STATE(x)                                            \
    if(x->nmtState == _CO_NMT_STOPPED || x->nmtState == _CO_NMT_BOOT) \
    {                                                                 \
        return RET_STOPPED;                                           \
    }
//! @endcond

/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static int retSDO(int code)
{
    switch(code)
    {
    case CO_SDO_AB_NONE:
        return RET_OK;

    case CO_SDO_AB_TIMEOUT:
        return RET_TIMEOUT;

    case CO_SDO_AB_TOGGLE_BIT:
    case CO_SDO_AB_CMD:
    case CO_SDO_AB_BLOCK_SIZE:
    case CO_SDO_AB_SEQ_NUM:
    case CO_SDO_AB_CRC:
    case CO_SDO_AB_OUT_OF_MEM:
    case CO_SDO_AB_UNSUPPORTED_ACCESS:
    case CO_SDO_AB_WRITEONLY:
    case CO_SDO_AB_READONLY:
    case CO_SDO_AB_NOT_EXIST:
    case CO_SDO_AB_NO_MAP:
    case CO_SDO_AB_MAP_LEN:
    case CO_SDO_AB_PRAM_INCOMPAT:
    case CO_SDO_AB_DEVICE_INCOMPAT:
    case CO_SDO_AB_HW:
    case CO_SDO_AB_TYPE_MISMATCH:
    case CO_SDO_AB_DATA_LONG:
    case CO_SDO_AB_DATA_SHORT:
    case CO_SDO_AB_SUB_UNKNOWN:
    case CO_SDO_AB_INVALID_VALUE:
    case CO_SDO_AB_VALUE_HIGH:
    case CO_SDO_AB_VALUE_LOW:
    case CO_SDO_AB_MAX_LESS_MIN:
    case CO_SDO_AB_NO_RESOURCE:
    case CO_SDO_AB_GENERAL:
    case CO_SDO_AB_DATA_TRANSF:
    case CO_SDO_AB_DATA_LOC_CTRL:
    case CO_SDO_AB_DATA_DEV_STATE:
    case CO_SDO_AB_DATA_OD:
    case CO_SDO_AB_NO_DATA:
        return RET_ERROR;
    }
}

/**
 * @brief 
 * 
 * @param interface 
 * @param interfaceName 
 * @return int Status code (::RetStatus_t)
 * @ingroup Common
 */
int api_initInterface(CanInterface_t *const interface, const char *interfaceName)
{
	API_DEBUG("Opening %s...\n", interfaceName);
	return usbcan_instance_init(&interface->usbcan, interfaceName) ? RET_OK : RET_ERROR;
}

/**
 * @brief 
 * 
 * @param interface 
 * @param device Device instance 
 * @param id 
 * @return int Status code (::RetStatus_t)
 * @ingroup Common
 */
int api_initServo(const CanInterface_t *interface, CanDevice_t *const device, const uint8_t id)
{
    return RET_OK;
}

/**
 * @brief Reboots device
 * 
 * @param device Device instance 
 * @return int Status code (::RetStatus_t)
 * @ingroup System_control
 */
int api_reboot(const CanDevice_t *device)
{
    write_nmt((device == 0)
                  ? 0
                  : device,
              _CO_NMT_CMD_RESET_NODE);
    return RET_OK;
}

/**
 * @brief Resets device communication
 * 
 * @param device Device instance 
 * @return int Status code (::RetStatus_t)
 * @ingroup System_control
 */
int api_resetCommunication(const CanDevice_t *device)
{
    write_nmt((device == 0)
                  ? 0
                  : device,
              _CO_NMT_CMD_RESET_COMM);
    return RET_OK;
}

/**
 * @brief Sets device/s to the operational state  
 * 
 * @param device Device instance 
 *  If device == 0 > all devices on the bus will be set to the operational state
 * @return int Status code (::RetStatus_t)
 * @ingroup System_control
 */
int api_setStateOperational(const CanDevice_t *device)
{
    write_nmt((device == 0)
                  ? 0
                  : device,
              _CO_NMT_CMD_GOTO_OP);
    return RET_OK;
}

/**
 * @brief Sets device/s to the pre-operational state  
 * 
 * @param device Device instance 
 * @return int Status code (::RetStatus_t)
 * @ingroup System_control
 */
int api_setStatePreOperational(const CanDevice_t *device)
{
    write_nmt((device == 0)
                  ? 0
                  : device,
              _CO_NMT_CMD_GOTO_PREOP);
    return RET_OK;
}

/**
 * @brief Sets device/s to the stopped state  
 * 
 * @param device Device instance 
 * @return int Status code (::RetStatus_t)
 * @ingroup System_control
 */
int api_setStateStopped(const CanDevice_t *device)
{
    write_nmt((device == 0)
                  ? 0
                  : device,
              _CO_NMT_CMD_GOTO_STOPPED);
    return RET_OK;
}

/**
 * @brief Stops the device and releases it
 * 
 * @param device Device instance 
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_stopAndRelease(const CanDevice_t *device)
{
    CHECK_NMT_STATE(device);

    uint8_t data = 0;
    uint8_t sts = write_raw_sdo(device, 0x2010, 0x01, &data, sizeof(data), 1, 100);

    return retSDO(sts);
}

/**
 * @brief Stops the device and make it hold current position
 * 
 * @param device Device instance 
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_stopAndFreeze(const CanDevice_t *device)
{
    CHECK_NMT_STATE(device);

    uint8_t data = 0;
    uint8_t sts = write_raw_sdo(device, 0x2010, 0x02, &data, sizeof(data), 1, 100);

    return retSDO(sts);
}

/**
 * @brief Sets device contol current
 * 
 * @param device Device instance 
 * @param currentA Phase current in Amperes
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_setCurrent(const CanDevice_t *device, const float currentA)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    usb_can_put_float(data, 0, &currentA, 1);
    uint8_t sts = write_raw_sdo(device, 0x2012, 0x01, data, sizeof(data), 1, 100);

    return retSDO(sts);
}

/**
 * @brief Sets device contol velocity
 * 
 * @param device Device instance 
 * @param velocityDegPerSec Flange velocity in degrees/sec
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_setVelocity(const CanDevice_t *device, const float velocityDegPerSec)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    usb_can_put_float(data, 0, &velocityDegPerSec, 1);
    uint8_t sts = write_raw_sdo(device, 0x2012, 0x03, data, sizeof(data), 1, 100);

    return retSDO(sts)
}

/**
 * @brief Sets device contol position with maximum current
 * 
 * @param device Device instance 
 * @param positionDeg Flange position in degrees
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_setPosition(const CanDevice_t *device, const float positionDeg)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    usb_can_put_float(data, 0, &positionDeg, 1);
    uint8_t sts = write_raw_sdo(device, 0x2012, 0x04, data, sizeof(data), 1, 100);

    return retSDO(sts);
}

/**
 * @brief Sets device contol velocity with limited control phase current
 * 
 * @param device Device instance 
 * @param velocityDegPerSec Flange velocity in degrees/sec
 * @param currentA Phase current limit in Amperes
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_setVelocityWithLimits(const CanDevice_t *device, const float velocityDegPerSec, const float currentA)
{
    CHECK_NMT_STATE(device);

    uint8_t data[8];
    usb_can_put_float(data, 0, &velocityDegPerSec, 1);
    usb_can_put_float(data, 0, &currentA, 1);
    uint8_t sts = write_raw_sdo(device, 0x2012, 0x05, data, sizeof(data), 1, 100);

    return retSDO(sts);
}

/**
 * @brief Sets device contol position with 
 * limited control flange velocity and
 * limited control phase current
 * 
 * @param device Device instance 
 * @param positionDeg Flange position in degrees
 * @param velocityDegPerSec Flange velocity in degrees/sec
 * @param currentA Phase current limit in Amperes
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_setPositionWithLimits(const CanDevice_t *device, const float positionDeg, const float velocityDegPerSec, const float currentA)
{
    CHECK_NMT_STATE(device);

    uint8_t data[12];
    usb_can_put_float(data, 0, &positionDeg, 1);
    usb_can_put_float(data, 0, &velocityDegPerSec, 1);
    usb_can_put_float(data, 0, &currentA, 1);
    uint8_t sts = write_raw_sdo(device, 0x2012, 0x06, data, sizeof(data), 1, 100);

    return retSDO(sts);
}

/**
 * @brief Sets device control duty.
 * Duty is the part of the input voltage that is passed to the motor to spin it
 * 
 * @param device Device instance 
 * @param dutyPercent 
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_setDuty(CanDevice_t *device, float dutyPercent)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    usb_can_put_float(data, 0, &dutyPercent, 1);
    uint8_t sts = write_raw_sdo(device, 0x2012, 0x07, data, sizeof(data), 1, 100);

    return retSDO(sts);
}

/**
 * @brief Adds motion PVT point to the device queue
 * 
 * @param device Device instance 
 * @param positionDeg Flange position in degrees
 * @param velocityDeg Flange velocity in degrees/sec
 * @param timeMs Relative point time in milliseconds
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_addMotionPoint(const CanDevice_t *device, const float positionDeg, const float velocityDeg, const uint32_t timeMs)
{
    CHECK_NMT_STATE(device);

    uint8_t data[12];
    usb_can_put_float(data, 0, &positionDeg, 1);
    usb_can_put_float(data + 4, 0, &velocityDeg, 1);
    usb_can_put_uint32_t(data + 8, 0, &timeMs, 1);

    uint32_t sts = write_raw_sdo(device, 0x2200, 2, data, sizeof(data), 1, 200);
    if(sts == CO_SDO_AB_PRAM_INCOMPAT)
    {
        return RET_WRONG_TRAJ;
    }
    else
    {
        return retSDO(sts);
    }
}

/**
 * @brief Starts the device movement by the spline points.
 * This is a broadcast command.
 * Note: if any device is not completed the movement (by spline points) it will send 
 * broadcast "Goto Stopped State" command to the all devices on the bus
 * 
 * @param timestampMs Startup delay in milliseconds. 
 * Default: 0
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_startMotion(uint32_t timestampMs)
{
    write_timestamp(timestampMs);
    return RET_OK;
}

/**
 * @brief Reads error flag array
 * 
 * @param device Device instance 
 * @param array Pointer to the error array
 * @param size Size of the received array
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_info
 */
int api_readErrorStatus(const CanDevice_t *device, uint8_t *array, uint32_t *size)
{
    CHECK_NMT_STATE(device);
    uint8_t sts = read_raw_sdo(device, 0x2000, 0, array, size, 1, 200);
    return retSDO(sts);
}

/**
 * @brief Writes device source array format (activated source indexes)
 * 
 * @param device Device instance 
 * @param requests Pointer to the source index array
 * @param size Size of the source index array
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_info
 */
int api_writeSourcesFormat(const CanDevice_t *device, const uint8_t *requests, const uint32_t size)
{
    CHECK_NMT_STATE(device);

    uint8_t array[10] = {0};

    for(uint32_t i = 0; i < sizeof(device->source)/sizeof(device->source[0]; i++)
    {
        device->source[i].activated = 0;
    }

    for(uint32_t i = 0; i < size; i++)
    {
        device->source[requests[i]].activated = !0;
        device->source[requests[i]].value = 0.0;
        BIT_SET_UINT_ARRAY(array, requests[i]);
    }
    sourceSize = size;

    uint8_t sts = write_raw_sdo(device, 0x2015, 1, array, sizeof(array), 1, 200);

    return retSDO(sts);
}

/**
 * @brief Reads device source array format (activated source indexes)
 * 
 * @param device Device instance 
 * @param requests Pointer to the source index array
 * @param size Size of the array. Contains received size of activated sources in the device
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_info
 */
int api_readSourcesFormat(const CanDevice_t *device, uint8_t *requests, uint32_t *size)
{
    CHECK_NMT_STATE(device);

    uint8_t sts = read_raw_sdo(device, 0x2015, 1, requests, size, 1, 200);

    return retSDO(sts);
}

/**
 * @brief Reads device sources. 
 * Note: source indexes should be programmed with ::api_writeSourcesFormat function
 * 
 * @param device Device instance 
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_info
 */
int api_readSources(const CanDevice_t *device)
{
    CHECK_NMT_STATE(device);

    uint8_t data[256 * 4];
    uint32_t len = sizeof(data);

    if(device->sourceSize == 0)
    {
        return RET_ZERO_SIZE;
    }

    uint8_t sts = read_raw_sdo(device, 0x2014, 0x01, data, len, 1, 100);

    if(sts == CO_SDO_AB_NONE)
    {
        if(len != device->sourceSize * 4)
        {
            return RET_SIZE_MISMATCH;
        }

        uint32_t offset = 0;
        for(uint32_t i = 0; i < device->sourceSize; i++)
        {
            if(device->source[requests[i]].activated != 0)
            {
                usb_can_get_float(data + offset, 0, &device->source[requests[i]].value, 1);
                offset += 4;
            }
        }
        return RET_OK;
    }

    return retSDO(sts);
}

/**
 * @brief Reads device source (information parameter)
 * 
 * @param device Device instance 
 * @param param 
 * @param value 
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_info
 */
int api_readParameter(const CanDevice_t *device, const uint8_t param, const float *value)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    uint32_t size = sizeof(data);

    int sts = read_raw_sdo(device, 0x2013, param, data, &len, 2, 100);
    if(sts == CO_SDO_AB_NONE && len == 4)
    {
        usb_can_get_float(data, 0, &device->source[requests[i]].value, 1);
        return RET_OK;
    }

    return retSDO(sts);
}

/**
 * @brief Erases the whole device motion queue
 * 
 * @param device Device instance 
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_clearPointsAll(const CanDevice_t *device)
{
    CHECK_NMT_STATE(device);
    uint32_t num = 0;
    uint8_t sts = write_raw_sdo(device, 0x2202, 0x01, &num, sizeof(num), 1, 100);
    return retSDO(sts);
}

/**
 * @brief Erases number of cells from the tail of the device motion queue
 * 
 * @param device Device instance 
 * @param numToClear 
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_control
 */
int api_clearPoints(const CanDevice_t *device, const uint32_t numToClear)
{
    CHECK_NMT_STATE(device);
    uint8_t sts = write_raw_sdo(device, 0x2202, 0x01, &numToClear, sizeof(numToClear), 1, 100);
    return retSDO(sts);
}

/**
 * @brief Gets device motion queue occupied cells (queue size)
 * 
 * @param device Device instance 
 * @param num 
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_info
 */
int api_getPointsSize(CanDevice_t *device, uint32_t *num)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    uint32_t len = sizeof(data);
    uint8_t sts = read_raw_sdo(device, 0x2202, 0x02, data, len, 1, 100);

    if(sts == CO_SDO_AB_NONE && len == 4)
    {
        usb_can_get_uint32(data, 0, num, 1);
        return RET_OK;
    }

    return retSDO(sts);
}

/**
 * @brief Gets device remaining motion queue free cell count
 * 
 * @param device Device instance 
 * @param num 
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_info
 */
int api_getPointsFreeSpace(CanDevice_t *device, uint32_t *num)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    uint32_t len = sizeof(data);
    uint8_t sts = read_raw_sdo(device, 0x2202, 0x03, data, len, 1, 100);

    if(sts == CO_SDO_AB_NONE && len == 4)
    {
        usb_can_get_uint32(data, 0, num, 1);
        return RET_OK;
    }

    return retSDO(sts);
}

/**
 * @brief Calculates motion point parameters
 * 
 * @param device Device instance 
 * @param startPositionDeg 
 * @param startVelocityDeg 
 * @param startAccelerationDegPerSec2 
 * @param startTimeMs 
 * @param endPositionDeg 
 * @param endVelocityDeg 
 * @param endAccelerationDegPerSec2 
 * @param endTimeMs 
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_info
 */
int api_invokeTimeCalculation(const CanDevice_t *device,
                              const float startPositionDeg, const float startVelocityDeg, const float startAccelerationDegPerSec2, const uint32_t startTimeMs,
                              const float endPositionDeg, const float endVelocityDeg, const float endAccelerationDegPerSec2, const uint32_t endTimeMs)
{
    CHECK_NMT_STATE(device);

    uint8_t data[8 * 4];

    usb_can_put_float(data, 0, &startPositionDeg, 1);
    usb_can_put_float(data + 4, 0, &startVelocityDeg, 1);
    usb_can_put_float(data + 8, 0, &startAccelerationDegPerSec2, 1);
    usb_can_put_uint32_t(data + 12, 0, &startTimeMs, 1);

    usb_can_put_float(data + 16, 0, &endPositionDeg, 1);
    usb_can_put_float(data + 20, 0, &endVelocityDeg, 1);
    usb_can_put_float(data + 24, 0, &endAccelerationDegPerSec2, 1);
    usb_can_put_uint32_t(data + 28, 0, &endTimeMs, 1);

    uint8_t sts = write_raw_sdo(device, 0x2203, 0x01, data, sizeof(data), 1, 200);

    if(sts == CO_SDO_AB_GENERAL)
    {
        return RET_WRONG_TRAJ;
    }
    else
    {
        return retSDO(sts);
    }
}

/**
 * @brief Gets ::api_invokeTimeCalculation calculated time in milliseconds
 * 
 * @param device Device instance 
 * @param timeMs Pointer to the calculated time in milliseconds
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_info
 */
int api_getTimeCalculationResult(const CanDevice_t *device, uint32_t *timeMs)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    uint32_t len = sizeof(data);
    uint8_t sts = read_raw_sdo(device, 0x2203, 0x02, data, len, 1, 100);

    if(sts == CO_SDO_AB_NONE && len == 4)
    {
        usb_can_get_uint32(data, 0, num, 1);
        return RET_OK;
    }

    return retSDO(sts);
}

/**
 * @brief Sets device zero position
 * 
 * @param device Device instance 
 * @param positionDeg Target position in degrees
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_config
 */
int api_setZeroPosition(const CanDevice_t *device, const float32_t positionDeg)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    usb_can_put_float(data, 0, &positionDeg, 1);
    uint8_t sts = write_raw_sdo(device, 0x2208, 0x01, data, sizeof(data), 0, 200);

    return retSDO(sts);
}

/**
 * @brief Sets device zero position and saves it to the device FLASH memory
 * 
 * @param device Device instance 
 * @param positionDeg Target position in degrees
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_config
 */
int api_setZeroPositionAndSave(const CanDevice_t *device, const float32_t positionDeg)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    usb_can_put_float(data, 0, &positionDeg, 1);
    uint8_t sts = write_raw_sdo(device, 0x2208, 0x02, data, sizeof(data), 0, 200);

    return retSDO(sts);
}

/**
 * @brief Gets the maximum device velocity
 * 
 * @param device Device instance 
 * @param velocityDegPerSec Velocity in degrees/sec
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_info
 */
int api_getMaxVelocity(const CanDevice_t *device, float *velocityDegPerSec)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    unt len = sizeof(data);

    uint8_t sts = read_raw_sdo(device, 0x2207, 0x02, data, len, 1, 100);
    if(sts == CO_SDO_AB_NONE)
    {
        usb_can_put_float(data, 0, &velocityDegPerSec, 1);
    }

    return retSDO(sts);
}

/**
 * @brief Sets the global limit for the device velocity.
 * Note: this is power volatile command
 * 
 * @param device Device instance 
 * @param maxVelocityDegPerSec Flange velocity in degrees/sec
 * @return int Status code (::RetStatus_t)
 * @ingroup Servo_config
 */
int api_setMaxVelocity(const CanDevice_t *device, const float maxVelocityDegPerSec)
{
    CHECK_NMT_STATE(device);

    uint8_t data[4];
    usb_can_put_float(data, 0, &maxVelocityDegPerSec, 1);
    uint8_t sts = write_raw_sdo(device, 0x2300, 0x03, data, sizeof(data), 1, 100);

    return retSDO(sts);
}
