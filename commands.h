#ifndef CMDS_H
#define CMDS_H
#pragma once

/**
 * @brief Types of command parsing result
 */
typedef enum
{
    RES_OK,
    RES_CMD_NOT_FOUND,
    RES_INCORRECT_SYNTAX,
    RES_INCORRECT_ARG
} cmd_res_t;

/**
 * @brief Tries to parse command from @param cmd_buf and executes it upon success.
 * Also outputs result message
 *
 * @param cmd_buf String buffer with possible command
 */
void cmd_parse(char* cmd_buf);

/**
 * @brief Prints result info based on @param res value
 *
 * @param res Kind of result
 */
void cmd_print_res_info(const cmd_res_t res);

#endif