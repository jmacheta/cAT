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

static char ack_results[512];

static int8_t var1;
static int8_t var2;

static char const *input_text;
static size_t input_index;

static cat_return_state cmd_write(const cat_command *cmd, const char *data, size_t data_size, size_t args_num)
{
        (void)cmd; // Unused
        (void)data; // Unused
        (void)data_size; // Unused
        (void)args_num; // Unused
        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state cmd_read(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        (void)cmd; // Unused
        (void)data; // Unused
        (void)data_size; // Unused
        (void)max_data_size; // Unused
        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state cmd_test(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        (void)cmd; // Unused
        (void)data; // Unused
        (void)data_size; // Unused
        (void)max_data_size; // Unused
        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state cmd_run(const cat_command *cmd)
{
        (void)cmd; // Unused

        return CAT_RETURN_STATE_DATA_OK;
}

static int var1_write(const cat_variable *var, size_t write_size)
{
        (void)var; // Unused
        (void)write_size; // Unused
        return 0;
}

static int var2_write(const cat_variable *var, size_t write_size)
{
        (void)var; // Unused
        (void)write_size; // Unused
        return 0;
}

static cat_return_state print_cmd_list(const cat_command *cmd)
{
        (void)cmd; // Unused

        return CAT_RETURN_STATE_PRINT_CMD_LIST_OK;
}

static cat_variable vars[] = { { .type = CAT_VAR_INT_DEC, .data = &var1, .data_size = sizeof(var1), .write = var1_write } };

static cat_variable vars_ro[] = {
        { .type = CAT_VAR_INT_DEC, .data = &var1, .data_size = sizeof(var1), .access = CAT_VAR_ACCESS_READ_ONLY, .write = var1_write }
};

static cat_variable vars_wo[] = {
        { .type = CAT_VAR_INT_DEC, .data = &var1, .data_size = sizeof(var1), .access = CAT_VAR_ACCESS_WRITE_ONLY, .write = var1_write }
};

static cat_variable vars2[] = { { .type = CAT_VAR_INT_DEC, .data = &var1, .data_size = sizeof(var1), .write = var1_write },
                                { .type = CAT_VAR_INT_DEC, .data = &var2, .data_size = sizeof(var2), .write = var2_write } };

static cat_variable vars2_ro[] = {
        { .type = CAT_VAR_INT_DEC, .data = &var1, .data_size = sizeof(var1), .access = CAT_VAR_ACCESS_READ_ONLY, .write = var1_write },
        { .type = CAT_VAR_INT_DEC, .data = &var2, .data_size = sizeof(var2), .write = var2_write }
};

static cat_variable vars2_wo[] = {
        { .type = CAT_VAR_INT_DEC, .data = &var1, .data_size = sizeof(var1), .access = CAT_VAR_ACCESS_WRITE_ONLY, .write = var1_write },
        { .type = CAT_VAR_INT_DEC, .data = &var2, .data_size = sizeof(var2), .write = var2_write }
};

static cat_command cmds[] = {
        { .name = "+V1", .var = vars, .var_num = sizeof(vars) / sizeof(vars[0]) },
        { .name = "+V1RO", .var = vars_ro, .var_num = sizeof(vars_ro) / sizeof(vars_ro[0]) },
        { .name = "+V1RW", .var = vars_wo, .var_num = sizeof(vars_wo) / sizeof(vars_wo[0]) },
        { .name = "+V11", .var = vars2, .var_num = sizeof(vars2) / sizeof(vars2[0]) },
        { .name = "+V11RO", .var = vars2_ro, .var_num = sizeof(vars2_ro) / sizeof(vars2_ro[0]) },
        { .name = "+V11WO", .var = vars2_wo, .var_num = sizeof(vars2_wo) / sizeof(vars2_wo[0]) },
        { .name = "+V2", .write = cmd_write, .var = vars, .var_num = sizeof(vars) / sizeof(vars[0]) },
        { .name = "+V3", .write = cmd_write, .read = cmd_read, .var = vars, .var_num = sizeof(vars) / sizeof(vars[0]) },
        { .name = "+V4", .write = cmd_write, .read = cmd_read, .test = cmd_test, .var = vars, .var_num = sizeof(vars) / sizeof(vars[0]) },
        { .name = "+V5", .write = cmd_write, .read = cmd_read, .run = cmd_run, .test = cmd_test, .var = vars, .var_num = sizeof(vars) / sizeof(vars[0]) },
        {
                .name = "+S1",
        },
        {
                .name = "+S2",
                .write = cmd_write,
        },
        {
                .name = "+S3",
                .write = cmd_write,
                .read = cmd_read,
        },
        {
                .name = "+S4",
                .write = cmd_write,
                .read = cmd_read,
                .test = cmd_test,
        },
        {
                .name = "+S5",
                .write = cmd_write,
                .read = cmd_read,
                .run = cmd_run,
                .test = cmd_test,

        },
        {
                .name = "+D1",
                .write = cmd_write,
                .read = cmd_read,
                .run = cmd_run,
                .test = cmd_test,
                .disable = true,
        },
        {
                .name = "+T1",
                .write = cmd_write,
                .read = cmd_read,
                .run = cmd_run,
                .test = cmd_test,
                .only_test = true,
        },
        {
                .name = "+T2",
                .only_test = true,
        },
        {
                .name = "#HELP",
                .run = print_cmd_list,
        }
};

static uint8_t buf[512];

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

        memset(ack_results, 0, sizeof(ack_results));
}

TEST(cAT, cmd_list)
{
        cat_object at;

        cat_init(&at, &desc, &iface, NULL);

        prepare_input("\nAT#HELP\n");
        while (cat_service(&at) != 0) {
        };

        assert(strcmp(ack_results,
                      "\nAT+V1?\nAT+V1=\nAT+V1=?\n\nAT+V1RO?\nAT+V1RO=?\n\nAT+V1RW=\nAT+V1RW=?\n\nAT+V11?\nAT+V11=\nAT+V11=?\n\nAT+V11RO?\nAT+V11RO=\nAT+V11RO=?\n\nAT+V11WO?\nAT+V11WO=\nAT+V11WO=?\n\nAT+V2?\nAT+V2=\nAT+V2=?\n\nAT+V3?\nAT+V3=\nAT+V3=?\n\nAT+V4?\nAT+V4=\nAT+V4=?\n\nAT+V5\nAT+V5?\nAT+V5=\nAT+V5=?\n\nAT+S2=\n\nAT+S3?\nAT+S3=\n\nAT+S4?\nAT+S4=\nAT+S4=?\n\nAT+S5\nAT+S5?\nAT+S5=\nAT+S5=?\n\nAT+T1=?\n\nAT#HELP\n\nOK\n") ==
               0);
}
