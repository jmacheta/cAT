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
#include <inttypes.h>
#include <assert.h>
#include <inttypes.h>
#include <cat/cat.h>
#include <gtest/gtest.h>
static char write_results[256];
static char ack_results[256];

static int8_t var1, var2, var3;
static int8_t var1b, var2b, var3b;

static char const *input_text;
static size_t input_index;

static cat_return_state cmd_write1(const cat_command *, const char *data, size_t data_size, size_t args_num)
{
        char tmp[32];
        sprintf(tmp, " CMD1_%" PRIuMAX ":", static_cast<uintmax_t>(args_num));
        strcat(write_results, tmp);
        strncat(write_results, data, data_size);
        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state cmd_write3(const cat_command *, const char *data, size_t data_size, size_t args_num)
{
        char tmp[32];
        sprintf(tmp, " CMD3_%" PRIuMAX ":", static_cast<uintmax_t>(args_num));
        strcat(write_results, tmp);
        strncat(write_results, data, data_size);
        return CAT_RETURN_STATE_DATA_OK;
}

static int var1_write(const cat_variable *var, size_t write_size)
{
        assert(write_size == 1);
        var1b = *(int8_t *)(var->data);
        return 0;
}

static int var2_write(const cat_variable *var, size_t write_size)
{
        assert(write_size == 1);
        var2b = *(int8_t *)(var->data);
        return 0;
}

static int var3_write(const cat_variable *var, size_t write_size)
{
        assert(write_size == 1);
        var3b = *(int8_t *)(var->data);
        return 0;
}

static cat_variable vars[] = { { .type = CAT_VAR_INT_DEC, .data = &var1, .data_size = sizeof(var1), .write = var1_write },
                               { .type = CAT_VAR_INT_DEC, .data = &var2, .data_size = sizeof(var2), .write = var2_write },
                               { .type = CAT_VAR_INT_DEC, .data = &var3, .data_size = sizeof(var3), .write = var3_write } };

static cat_command cmds[] = { { .name = "+SET1",
                                .write = cmd_write1,

                                .var = vars,
                                .var_num = sizeof(vars) / sizeof(vars[0]) },
                              { .name = "+SET3",
                                .write = cmd_write3,

                                .var = vars,
                                .var_num = sizeof(vars) / sizeof(vars[0]) },
                              { .name = "+SETALL",
                                .write = cmd_write3,

                                .var = vars,
                                .var_num = sizeof(vars) / sizeof(vars[0]),
                                .need_all_vars = true } };

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

        var1 = 1;
        var2 = 2;
        var3 = 3;

        memset(ack_results, 0, sizeof(ack_results));
        memset(write_results, 0, sizeof(write_results));
}

static const char test_case_1[] = "\nAT+SET=-10,-20,-30\r\nAT+SET1=-10,-20,-30\r\nAT+SET1=-1\r\n";
static const char test_case_2[] = "\nAT+SET3=-1,-2,-3,0\nAT+SET3=-1,-2,-3\nAT+SET3=-100\n";
static const char test_case_3[] = "\nAT+SETALL=-11,-22,-33\nAT+SETALL=-1,-2,-3\nAT+SETALL=100\n";

TEST(cAT, write_parse)
{
        cat_object at;

        cat_init(&at, &desc, &iface, NULL);

        prepare_input(test_case_1);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\r\nERROR\r\n\r\nOK\r\n\r\nOK\r\n");
        EXPECT_STREQ(write_results, " CMD1_3:-10,-20,-30 CMD1_1:-1");

        EXPECT_EQ(var1, -1);
        EXPECT_EQ(var2, -20);
        EXPECT_EQ(var3, -30);
        EXPECT_EQ(var1b, -1);
        EXPECT_EQ(var2b, -20);
        EXPECT_EQ(var3b, -30);

        prepare_input(test_case_2);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nERROR\n\nOK\n\nOK\n");
        EXPECT_STREQ(write_results, " CMD3_3:-1,-2,-3 CMD3_1:-100");

        EXPECT_EQ(var1, -100);
        EXPECT_EQ(var2, -2);
        EXPECT_EQ(var3, -3);
        EXPECT_EQ(var1b, -100);
        EXPECT_EQ(var2b, -2);
        EXPECT_EQ(var3b, -3);

        prepare_input(test_case_3);
        while (cat_service(&at) != 0) {
        };

        EXPECT_STREQ(ack_results, "\nOK\n\nOK\n\nERROR\n");
        EXPECT_STREQ(write_results, " CMD3_3:-11,-22,-33 CMD3_3:-1,-2,-3");

        EXPECT_EQ(var1, 100);
        EXPECT_EQ(var2, -2);
        EXPECT_EQ(var3, -3);
        EXPECT_EQ(var1b, 100);
        EXPECT_EQ(var2b, -2);
        EXPECT_EQ(var3b, -3);
}