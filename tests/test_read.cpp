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
static char run_results[256];
static char read_results[256];
static char ack_results[256];

static char const *input_text;
static size_t input_index;

static cat_return_state a_run(const cat_command *cmd)
{
        strcat(run_results, " A_");
        strcat(run_results, cmd->name);
        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state a_read(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        strcat(read_results, " A:");
        strcat(read_results, cmd->name);

        snprintf(data, max_data_size, "%s=A-val", cmd->name);
        *data_size = strlen(data);

        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state ap_read(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        (void)data; // Unused
        (void)data_size; // Unused
        (void)max_data_size; // Unused
        strcat(read_results, " AP:");
        strcat(read_results, cmd->name);

        *data_size = 0;

        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state test_read(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        (void)data; // Mask as unused
        (void)data_size; // Mask as unused
        (void)max_data_size; // Mask as unused
        strcat(read_results, " +TEST:");
        strcat(read_results, cmd->name);

        return CAT_RETURN_STATE_ERROR;
}

static cat_command cmds[] = { { .name = "A", .read = a_read, .run = a_run },
                              { .name = "AP", .read = ap_read },
                              { .name = "+TEST", .read = test_read },
                              { .name = "+EMPTY" } };

static uint8_t buf[128];

static cat_command_group cmd_group = {
        .cmd = cmds,
        .cmd_num = sizeof(cmds) / sizeof(cmds[0]),
};

static cat_command_group *cmd_desc[] = { &cmd_group };

static cat_descriptor desc = { .cmd_group = cmd_desc,
                               .cmd_group_num = sizeof(cmd_desc) / sizeof(cmd_desc[0]),

                               .buf = buf,
                               .buf_size = sizeof(buf) };

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

        memset(run_results, 0, sizeof(run_results));
        memset(ack_results, 0, sizeof(ack_results));
        memset(read_results, 0, sizeof(read_results));
}

static const char test_case_1[] = "\nAT\r\nAT+\nAT+?\nATA?\r\nATAP\nATAP?\nATAPA?\nAT+TEST?\nAT+te?\nAT+e?\nAT+empTY?\r\nATA\r\n";

TEST(cAT, read)
{
        cat_object at;

        cat_init(&at, &desc, &iface, NULL);

        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results,
                     "\r\nOK\r\n\nERROR\n\nERROR\n\r\nA=A-val\r\n\r\nOK\r\n\nERROR\n\nAP=\n\nOK\n\nERROR\n\nERROR\n\nERROR\n\nERROR\n\r\nERROR\r\n\r\nOK\r\n");
        EXPECT_STREQ(run_results, " A_A");
        EXPECT_STREQ(read_results, " A:A AP:AP +TEST:+TEST +TEST:+TEST");
}
