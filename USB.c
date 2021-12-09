#include "USB.h"

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "nrf_log.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"
#include "app_usbd_cdc_acm.h"

#include "commands.h"


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
static char cmd_buf[COMMAND_LENGTH];

/**
 * @brief Current command buffer index
 */
static size_t cmd_buf_index = 0;

static char m_rx_buffer[READ_SIZE];

/**
 * @brief Erases data in command buffer
 */
static void wipe_cmd_buf()
{
    memset(cmd_buf, 0, COMMAND_LENGTH);
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
                    cmd_parse(cmd_buf);
                    wipe_cmd_buf();
                    cmd_buf_index = 0;

                    ret = app_usbd_cdc_acm_write(&usb_cdc_acm, "\r\n", 2);
                }
                else
                {
                    cmd_buf[cmd_buf_index] = m_rx_buffer[0];
                    cmd_buf_index = (cmd_buf_index + 1) % COMMAND_LENGTH;

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
}