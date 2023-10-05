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
static char test_results[256];
static char ack_results[256];

static char const *input_text;
static size_t input_index;

static cat_return_state a_test(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        strcat(test_results, " A:");
        strcat(test_results, cmd->name);

        snprintf(data, max_data_size, "%s=A-val", cmd->name);
        *data_size = strlen(data);

        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state ap_test(const cat_command *cmd, char *, size_t *data_size, size_t )
{
        strcat(test_results, " AP:");
        strcat(test_results, cmd->name);

        *data_size = 0;

        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state ap_write(const cat_command *cmd, const char *data, size_t data_size, size_t args_num)
{
        strcat(test_results, " AP_W:");
        strcat(test_results, cmd->name);

        assert(args_num == 0);
        assert(data[0] == 'a');
        assert(data_size == 1);

        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state apw_write(const cat_command *cmd, const char *data, size_t data_size, size_t args_num)
{
        strcat(test_results, " APW:");
        strcat(test_results, cmd->name);

        assert(args_num == 0);
        assert(data[0] == '?');
        assert(data_size == 1);

        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state test_test(const cat_command *cmd, char *, size_t *, size_t )
{
        strcat(test_results, " +TEST:");
        strcat(test_results, cmd->name);

        return CAT_RETURN_STATE_ERROR;
}

static cat_command cmds[] = { { .name = "A", .test = a_test },
                              { .name = "AP", .write = ap_write, .test = ap_test },
                              { .name = "APW", .write = apw_write },
                              { .name = "+TEST", .test = test_test },
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

        memset(ack_results, 0, sizeof(ack_results));
        memset(test_results, 0, sizeof(test_results));
}

static const char test_case_1[] = "\nAT\r\nAT\nATAP=?\nATAP=?a\nATAP=a\nATAPW=?\nAT+TEST=?\nATA=?\nAT+EMPTY=?\n";

TEST(cAT, test)
{
        cat_object at;

        cat_init(&at, &desc, &iface, NULL);

        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\r\nOK\r\n\nOK\n\nAP=\n\nOK\n\nERROR\n\nOK\n\nOK\n\nERROR\n\nA=A-val\n\nOK\n\nERROR\n");
        EXPECT_STREQ(test_results, " AP:AP AP_W:AP APW:APW +TEST:+TEST A:A");
}
