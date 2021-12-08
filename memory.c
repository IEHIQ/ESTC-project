#include "memory.h"
#include "nrf_nvmc.h"
#include "nrfx_nvmc.h"
#include "nrf_log.h"
#include "nrf_dfu_types.h"
#include "nrf_bootloader_info.h"

/* Start address of available memory */
#define NVM_START BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE
/* End address of available memory */
#define NVM_END   BOOTLOADER_START_ADDR

#define PAGE_SIZE_BYTES 4096
#define WORD_SIZE_BYTES 32
#define EMPTY_WORD 0xFFFFFFFF

/* Current memory address */
static uint32_t address;

void init_memory()
{
    address = NVM_START;
}

/**
 * @brief Erases three pages that are available to using
 */
static void erase_pages()
{
    for (size_t i = 0; i < 3; i++)
    {
        nrfx_nvmc_page_erase(NVM_START + i * PAGE_SIZE_BYTES);
    }
}

void save_HSB(const HSB *color)
{
    NRF_LOG_DEBUG("Saving...");
    uint32_t word = HSB_to_word(color);
    while(address < NVM_END && !nrfx_nvmc_word_writable_check(address, word))
    {
        address += WORD_SIZE_BYTES;
    }
    if (address >= NVM_END)
    {
        erase_pages();
        address = NVM_START;
    }
    nrfx_nvmc_word_write(address, word);
    while (!nrfx_nvmc_write_done_check())
    {}
    NRF_LOG_DEBUG("Saved! Address: %x", address);
}

void load_HSB(HSB *color)
{
    NRF_LOG_DEBUG("Loading...");
    while (address < NVM_END && *((uint32_t *)(address)) != EMPTY_WORD)
    {
        address += WORD_SIZE_BYTES;
    }
    word_to_HSB(*((uint32_t *)(address - WORD_SIZE_BYTES)), color);
    NRF_LOG_DEBUG("Loaded! Address: %x", address);
}