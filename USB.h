#ifndef USB_H
#define USB_H
#pragma once

/**
 * @brief USB command event type
 */
typedef void (*com_event)(void *context);

/**
 * @brief Initializes USB CDC ASM
 */
void init_usb();

/**
 * @brief Sets the RGB commmand event
 *
 * @param event Function that will be executed when command to set RGB color will be recieved
 */
void set_rgb_com_event(com_event event);

#endif