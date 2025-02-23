/*
MIT License

Copyright (c) 2019 Marcin Borowicz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <assert.h>

#include <cat/cat.h>
#include <gtest/gtest.h>
static char cmd_results[256];
static char var_read_results[256];
static char ack_results[256];

static char const *input_text;
static size_t input_index;

static int var_x, var_u1, var_u2;
static cat_object at;

static cat_return_state cmd1_read(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size);
static cat_return_state cmd2_read(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size);
static int var_read(const cat_variable *var);

static cat_variable u_vars[] = { { .name = "U1", .type = CAT_VAR_INT_DEC, .data = &var_u1, .data_size = sizeof(var_u1), .read = var_read },
                                 { .name = "U2", .type = CAT_VAR_INT_DEC, .data = &var_u2, .data_size = sizeof(var_u2), .read = var_read } };

static cat_command u_cmds[] = { {
                                        .name = "+U1CMD",
                                        .read = cmd1_read,
                                        .var = &u_vars[0],
                                        .var_num = 1,
                                },
                                {
                                        .name = "+U2CMD",
                                        .read = cmd2_read,
                                        .var = &u_vars[1],
                                        .var_num = 1,
                                } };
static cat_return_state cmd_write(const cat_command *cmd, const char *data, size_t data_size, size_t args_num)
{
        (void)data; // Unused
        (void)data_size; // Unused
        (void)args_num; // Unused
        cat_status s;

        strcat(cmd_results, " write:");
        strcat(cmd_results, cmd->name);

        if (var_x < 2) {
                s = cat_trigger_unsolicited_read(&at, &u_cmds[var_x]);
                assert(s == CAT_STATUS_OK);

                return CAT_RETURN_STATE_HOLD;
        }

        return CAT_RETURN_STATE_ERROR;
}

static cat_return_state cmd1_read(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        (void)data; // Unused
        (void)data_size; // Unused
        (void)max_data_size; // Unused
        cat_status s;

        strcat(cmd_results, " read1:");
        strcat(cmd_results, cmd->name);

        if (var_u1 > 0) {
                var_u1--;
                s = cat_trigger_unsolicited_read(&at, cmd);
                assert(s == CAT_STATUS_OK);
                return CAT_RETURN_STATE_DATA_OK;
        }

        return CAT_RETURN_STATE_HOLD_EXIT_OK;
}

static cat_return_state cmd2_read(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        (void)data; // Unused
        (void)data_size; // Unused
        (void)max_data_size; // Unused
        cat_status s;

        strcat(cmd_results, " read2:");
        strcat(cmd_results, cmd->name);

        if (var_u2 > 0) {
                var_u2--;
                s = cat_trigger_unsolicited_read(&at, cmd);
                assert(s == CAT_STATUS_OK);
                return CAT_RETURN_STATE_DATA_OK;
        }

        cat_hold_exit(&at, CAT_STATUS_OK);
        return CAT_RETURN_STATE_DATA_OK;
}

static int var_read(const cat_variable *var)
{
        strcat(var_read_results, " var_read:");
        strcat(var_read_results, var->name);

        return 0;
}

static cat_variable vars[] = { { .name = "X", .type = CAT_VAR_INT_DEC, .data = &var_x, .data_size = sizeof(var_x), .read = var_read } };

static cat_command cmds[] = { {
        .name = "+CMD",
        .write = cmd_write,
        .var = vars,
        .var_num = sizeof(vars) / sizeof(vars[0]),
} };

static uint8_t buf[128];

static cat_command_group cmd_group = {
        .cmd = cmds,
        .cmd_num = sizeof(cmds) / sizeof(cmds[0]),
};

static cat_command_group *cmd_desc[] = { &cmd_group };

static cat_descriptor desc = {
        .cmd_group = cmd_desc,
        .cmd_group_num = sizeof(cmd_desc) / sizeof(cmd_desc[0]),

        .buf = buf,
        .buf_size = sizeof(buf),
};

static int write_char(char ch)
{
        char str[2];
        str[0] = ch;
        str[1] = 0;
        strcat(ack_results, str);
        return 1;
}

static int read_char(char *ch)
{
        if (input_index >= strlen(input_text))
                return 0;

        *ch = input_text[input_index];
        input_index++;
        return 1;
}

static cat_io_interface iface = { .write = write_char, .read = read_char };

static void prepare_input(const char *text)
{
        input_text = text;
        input_index = 0;

        var_u1 = 2;
        var_u2 = 3;

        memset(ack_results, 0, sizeof(ack_results));
        memset(cmd_results, 0, sizeof(cmd_results));
        memset(var_read_results, 0, sizeof(var_read_results));
}

static const char test_case_1[] = "\nAT+CMD=0\n\nAT+CMD=1\n";

TEST(cAT, hold_state)
{
        cat_init(&at, &desc, &iface, NULL);

        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\n+U1CMD=2\n\n+U1CMD=1\n\nOK\n\n+U2CMD=3\n\n+U2CMD=2\n\n+U2CMD=1\n\n+U2CMD=0\n\nOK\n");
        EXPECT_STREQ(cmd_results, " write:+CMD read1:+U1CMD read1:+U1CMD read1:+U1CMD write:+CMD read2:+U2CMD read2:+U2CMD read2:+U2CMD read2:+U2CMD");
        EXPECT_STREQ(var_read_results, " var_read:U1 var_read:U1 var_read:U1 var_read:U2 var_read:U2 var_read:U2 var_read:U2");
}
