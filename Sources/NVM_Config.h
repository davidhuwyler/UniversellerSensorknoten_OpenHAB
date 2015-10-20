/*
 * NVM_Config.h
 *
 *  Created on: 25.04.2015
 *      Author: David Schwegler
 */

#ifndef NVM_CONFIG_H_
#define NVM_CONFIG_H_


#include "Platform.h"

  #define NVMC_FLASH_START_ADDR    0x3F800
  /*!< NVRM_Config, start address of configuration data in flash */



/* macros for identify erased flash memory */
#define NVMC_FLASH_ERASED_UINT8  0xFF
  /*!< erased byte in flash */
#define NVMC_FLASH_ERASED_UINT16 0xFFFF
  /*!< erased word in flash */

#define NVMC_NODE_DATA_START_ADDR  (NVMC_FLASH_START_ADDR)
#define NVMC_NODE_DATA_SIZE        (4*1) /* maximum two uint8 ID + one uint16 Sensor config values */
#define NVMC_NODE_END_ADDR         (NVMC_NODE_DATA_START_ADDR+NVMC_NODE_DATA_SIZE)



/*!
 * \brief Saves the reflectance calibration data
 * \param data Pointer to data
 * \param dataSize Size of data in bytes
 * \return Error code, ERR_OK if everything is fine
 */
uint8_t NVMC_SaveIdentificationData(void *data, uint16_t dataSize);

/*!
 * \brief Returns the reflectance calibration data
 * \return Pointer to data, or NULL for failure
 */
void *NVMC_GetIdentifigationeData(void);

/*! \brief Driver initialization  */
void NVMC_Init(void);

/*! \brief Driver de-initialization  */
void NVMC_Deinit(void);




#endif /* NVM_CONFIG_H_ */
