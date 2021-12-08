#include "USB.h"

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "nrf_log.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"
#include "app_usbd_cdc_acm.h"


/* Make sure that they don't intersect with LOG_BACKEND_USB_CDC_ACM */
#define CDC_ACM_COMM_INTERFACE  2
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN3

#define CDC_ACM_DATA_INTERFACE  3
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN4
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT4

static void usb_ev_handler(app_usbd_class_inst_t const * p_inst,
                           app_usbd_cdc_acm_user_event_t event);

APP_USBD_CDC_ACM_GLOBAL_DEF(usb_cdc_acm,
                            usb_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_NONE);

#define READ_SIZE 1

/**
 * @brief Length of command buffer string
 */
#define COMMAND_LENGTH 32

/**
 * @brief Command buffer string
 */
static char com_buffer[COMMAND_LENGTH];

/**
 * @brief Current command buffer index
 */
static size_t com_buffer_index = 0;

static char m_rx_buffer[READ_SIZE];

/**
 * @brief Empty command event
 */
static void empty_com_event(void *context) {};

/**
 * @brief RGB setting event
 */
static com_event on_rgb_command;

/**
 * @brief Erases data in command buffer
 */
static void wipe_command_buffer()
{
    for (size_t i = 0; i < COMMAND_LENGTH; i++)
    {
        com_buffer[i] = '\0';
    }
}

/**
 * @brief Tries to parse RGB color setting command in format "(<RGB> | <rgb>) <red> <green> <blue>"
 * and invokes RGB setting event upon success
 * @return ret_code_t one of possible error codes:
 * 0 - OK
 * 1 - unknown command
 * 2 - incorrect command syntax
 * 3 - incorrect color value
 */
static ret_code_t try_parse_rgb_command()
{
    if (strncmp(com_buffer, "rgb", 3) != 0 && strncmp(com_buffer, "RGB", 3) != 0)
    {
        return 1;
    }
    else
    {
        int32_t rgb[3] = {0, 0, 0};
        char *next_space = strchr(com_buffer, (int)' ');
        if (next_space == NULL)
        {
            return 2;
        }
        else
        {
            rgb[0] = atoi(next_space);
            if (rgb[0] < 0 || rgb[0] > 255)
            {
                return 3;
            }
            for (int i = 1; i < 3; i++)
            {
                next_space = strchr(next_space + 1, (int)' ');
                if (next_space == NULL)
                {
                    return 2;
                }
                else
                {
                    rgb[i] = atoi(next_space);
                    if (rgb[i] < 0 || rgb[i] > 255)
                    {
                        return 3;
                    }
                }
            }
        }
        on_rgb_command(rgb);
        return 0;
    }
}

/**
 * @brief Tries to parse commands and outputs if something went wrong
 */
static void try_parse_command()
{
    ret_code_t err = try_parse_rgb_command();
    switch (err)
    {
        case 1:
            NRF_LOG_DEBUG("Unknown command");
            break;

        case 2:
            NRF_LOG_DEBUG("Incorrect RGB command. Correct syntax: RGB <red> <green> <blue>");
            break;

        case 3:
            NRF_LOG_DEBUG("Incorrect RGB color range. Should be in [0, 255] range");
            break;

        default:
            break;
    }

}

static void usb_ev_handler(app_usbd_class_inst_t const * p_inst,
                           app_usbd_cdc_acm_user_event_t event)
{
    switch (event)
    {
        case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
        {
            ret_code_t ret;
            ret = app_usbd_cdc_acm_read(&usb_cdc_acm, m_rx_buffer, READ_SIZE);
            UNUSED_VARIABLE(ret);
            break;
        }
        case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
        {
            ret_code_t ret;
            do
            {
                /* It's the simple version of an echo. Note that writing doesn't
                * block execution, and if we have a lot of characters to read and
                * write, some characters can be missed.
                */
                if (m_rx_buffer[0] == '\r' || m_rx_buffer[0] == '\n')
                {
                    try_parse_command();
                    wipe_command_buffer();
                    com_buffer_index = 0;

                    ret = app_usbd_cdc_acm_write(&usb_cdc_acm, "\r\n", 2);
                }
                else
                {
                    com_buffer[com_buffer_index] = m_rx_buffer[0];
                    com_buffer_index = (com_buffer_index + 1) % COMMAND_LENGTH;

                    ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                                m_rx_buffer,
                                                READ_SIZE);
                }

                /* Fetch data until internal buffer is empty */
                ret = app_usbd_cdc_acm_read(&usb_cdc_acm,
                                            m_rx_buffer,
                                            READ_SIZE);
            } while (ret == NRF_SUCCESS);

            break;
        }

        default:
            break;
    }
}

void init_usb()
{
    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&usb_cdc_acm);
    ret_code_t ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);
    on_rgb_command = empty_com_event;
}

void set_rgb_com_event(com_event event)
{
    on_rgb_command = event;
}
