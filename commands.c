#include "commands.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "nrf_log.h"

#include "colors.h"
#include "LED.h"

#define CMDS_COUNT 3

/**
 * @brief USB command handler type
 */
typedef cmd_res_t (*cmd_handler_t)();

/**
 * @brief Type of function that outputs help info for command
 */
typedef void (*cmd_helper_t)();

/**
 * @brief Command descriptor
 */
typedef struct
{
    const char* name;
    cmd_handler_t handler;
    cmd_helper_t helper;
} cmd_descriptor_t;

static cmd_res_t handler_rgb();
static cmd_res_t handler_hsb();
static cmd_res_t handler_help();

static void helper_rgb();
static void helper_hsb();
static void helper_help();

static cmd_descriptor_t cmds[CMDS_COUNT] =
{
    {"rgb", handler_rgb, helper_rgb},
    {"hsb", handler_hsb, helper_hsb},
    {"help", handler_help, helper_help}
};

void cmd_print_res_info(const cmd_res_t res)
{
    switch (res)
    {
        case RES_CMD_NOT_FOUND:
            NRF_LOG_INFO("Unknown command. Try \'help\' to see possible commands.");
            break;

        case RES_INCORRECT_SYNTAX:
            NRF_LOG_INFO("Incorrect command syntax. Try \'help\' to see correct syntax.");
            break;

        case RES_INCORRECT_ARG:
            NRF_LOG_INFO("Incorrect argument. Try \'help\' to see possible argument values.");
            break;

        default:
            break;
    }
}

void cmd_parse(char* cmd_buf)
{
    cmd_res_t res = RES_CMD_NOT_FOUND;
    char *cmd_name = strtok(cmd_buf, " ");
    if (cmd_name != NULL)
    {
        size_t i = 0;
        while (i < CMDS_COUNT && strcmp(cmd_name, cmds[i].name) != 0)
        {
            i++;
        }
        if (i < CMDS_COUNT)
        {
            res = cmds[i].handler();
        }
    }
    cmd_print_res_info(res);
}

/**
 * @brief Event that sets new HSB color and outputs message about it
 *
 * @param new_rgb New RGB color
 */
static void on_rgb_cmd(const RGB_8 *new_rgb)
{
    NRF_LOG_INFO("Color in RGB set to (%d, %d, %d)", new_rgb->R, new_rgb->G, new_rgb->B);
    HSB new_hsb;
    RGB_8_to_HSB(new_rgb, &new_hsb);
    set_current_hsb(&new_hsb);
    save_current_hsb();
}

/**
 * @brief Parses RGB command args values, checks them and invokes RGB color setting event
 * if args is correct
 *
 * @return cmd_res_t Parsing result
 */
static cmd_res_t handler_rgb()
{
    int32_t args[3] = {0, 0, 0};
    char *next_arg;
    for (int i = 0; i < 3; i++)
    {
        next_arg = strtok(NULL, " ");
        if (next_arg == NULL)
        {
            return RES_INCORRECT_SYNTAX;
        }
        args[i] = atoi(next_arg);
        if (args[i] < 0 || args[i] > 255)
        {
            return RES_INCORRECT_ARG;
        }
    }

    RGB_8 new_rgb = {args[0], args[1], args[2]};
    on_rgb_cmd(&new_rgb);

    return RES_OK;
}

/**
 * @brief Event that sets new HSB color and outputs message about it
 *
 * @param new_hsb New HSB color
 */
static void on_hsb_cmd(const HSB *new_hsb)
{
    NRF_LOG_INFO("Color in HSB set to (%d, %d, %d)", new_hsb->H, new_hsb->S, new_hsb->B);
    set_current_hsb(new_hsb);
    save_current_hsb();
}

/**
 * @brief Parses HSB command args values, checks them and invokes HSB color setting event
 * if args is correct
 *
 * @return cmd_res_t Parsing result
 */
static cmd_res_t handler_hsb()
{
    int32_t args[3] = {0, 0, 0};
    char *next_arg;
    for (int i = 0; i < 3; i++)
    {
        next_arg = strtok(NULL, " ");
        if (next_arg == NULL)
        {
            return RES_INCORRECT_SYNTAX;
        }
        args[i] = atoi(next_arg);
        if (args[i] < 0)
        {
            return RES_INCORRECT_ARG;
        }
    }
    if (args[0] > 360 || args[1] > 100 || args[2] > 100)
    {
        return RES_INCORRECT_ARG;
    }

    HSB new_hsb = {args[0], args[1], args[2]};
    on_hsb_cmd(&new_hsb);
    return RES_OK;
}

/**
 * @brief Outputs help information about commands
 *
 * @return cmd_res_t RES_OK
 */
static cmd_res_t handler_help()
{
    NRF_LOG_INFO("<HELP>");

    for (size_t i = 0; i < CMDS_COUNT; i++)
    {
        NRF_LOG_INFO("");
        cmds[i].helper();
        NRF_LOG_INFO("");
    }

    NRF_LOG_INFO("</HELP>");

    return RES_OK;
}

/**
 * @brief Outputs RGB command info
 */
static void helper_rgb()
{
    NRF_LOG_INFO("    rgb <red> <green> <blue>");
    NRF_LOG_INFO("    Sets new color based on entered RGB values and saves it into memory");
    NRF_LOG_INFO("    -- ARGS:");
    NRF_LOG_INFO("    - <red> - red color value in range [0-255]");
    NRF_LOG_INFO("    - <green> - green color value in range [0-255]");
    NRF_LOG_INFO("    - <blue> - blue color value in range [0-255]");
}
/**
 * @brief Outputs HSB command info
 */

static void helper_hsb()
{
    NRF_LOG_INFO("    hsb <hue> <saturation> <brightness>");
    NRF_LOG_INFO("    Sets new color based on entered HSB values and saves it into memory");
    NRF_LOG_INFO("    -- ARGS:");
    NRF_LOG_INFO("    - <hue> - hue color value in range [0-360]");
    NRF_LOG_INFO("    - <saturation> - saturation color value in range [0-100]");
    NRF_LOG_INFO("    - <brightness> - brightness color value in range [0-100]");
}
/**
 * @brief Outputs help command info
 */

static void helper_help()
{
    NRF_LOG_INFO("    help");
    NRF_LOG_INFO("    Shows commands info");
}