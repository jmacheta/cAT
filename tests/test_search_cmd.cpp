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
static cat_return_state ap_read(const cat_command *cmd, char *data, size_t *data_size, size_t max_data_size)
{
        (void)cmd; // Unused
        (void)data; // Unused
        (void)data_size; // Unused
        (void)max_data_size; // Unused
        return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state ap_write(const cat_command *cmd, const char *data, size_t data_size, size_t args_num)
{
        (void)cmd; // Unused
        (void)data; // Unused
        (void)data_size; // Unused
        (void)args_num; // Unused
        return CAT_RETURN_STATE_DATA_OK;
}

static cat_variable vars_ap1[] = { { .name = "var_ap1_1" }, { .name = "var_ap1_2" }, { .name = "var_ap1_3" } };

static cat_variable vars_apx2[] = { { .name = "var_apx2_1" }, { .name = "var_apx2_2" }, { .name = "var_apx2_3" } };

static cat_command cmds[] = {
        { .name = "AP1", .write = ap_write, .var = vars_ap1, .var_num = sizeof(vars_ap1) / sizeof(vars_ap1[0]), .only_test = true },
        { .name = "AP2", .read = ap_read, .only_test = false },
};

static cat_command cmds2[] = {
        { .name = "APX1", .write = ap_write, .only_test = true },
        { .name = "APX2", .read = ap_read, .var = vars_apx2, .var_num = sizeof(vars_apx2) / sizeof(vars_apx2[0]), .only_test = false },
};

static uint8_t buf[128];

static cat_command_group cmd_group1 = {
        .name = "std",

        .cmd = cmds,
        .cmd_num = sizeof(cmds) / sizeof(cmds[0]),
};

static cat_command_group cmd_group2 = {
        .name = "ext",

        .cmd = cmds2,
        .cmd_num = sizeof(cmds2) / sizeof(cmds2[0]),
};

static cat_command_group *cmd_desc[] = { &cmd_group1, &cmd_group2 };

static cat_descriptor desc = { .cmd_group = cmd_desc,
                               .cmd_group_num = sizeof(cmd_desc) / sizeof(cmd_desc[0]),

                               .buf = buf,
                               .buf_size = sizeof(buf) };

static int write_char(char ch)
{
        (void)ch; // Unused
        return 1;
}

static int read_char(char *ch)
{
        (void)ch; // Unused
        return 1;
}

static cat_io_interface iface = { .write = write_char, .read = read_char };

TEST(cAT, search_cmd)
{
        cat_object at;
        cat_command const *cmd;
        cat_command_group const *cmd_group;
        cat_variable const *var;

        cat_init(&at, &desc, &iface, nullptr);

        cmd = cat_search_command_by_name(&at, "A");
        EXPECT_EQ(cmd, nullptr);

        cmd = cat_search_command_by_name(&at, "AP");
        EXPECT_EQ(cmd, nullptr);

        cmd = cat_search_command_by_name(&at, "AP1");
        EXPECT_EQ(cmd, &cmds[0]);

        cmd = cat_search_command_by_name(&at, "AP2");
        EXPECT_EQ(cmd, &cmds[1]);

        cmd = cat_search_command_by_name(&at, "AP3");
        EXPECT_EQ(cmd, nullptr);

        cmd = cat_search_command_by_name(&at, "APX1");
        EXPECT_EQ(cmd, &cmds2[0]);

        cmd = cat_search_command_by_name(&at, "APX2");
        EXPECT_EQ(cmd, &cmds2[1]);

        cmd = cat_search_command_by_name(&at, "APX3");
        EXPECT_EQ(cmd, nullptr);

        cmd_group = cat_search_command_group_by_name(&at, "std");
        EXPECT_EQ(cmd_group, cmd_desc[0]);

        cmd_group = cat_search_command_group_by_name(&at, "ext");
        EXPECT_EQ(cmd_group, cmd_desc[1]);

        cmd_group = cat_search_command_group_by_name(&at, "not");
        EXPECT_EQ(cmd_group, nullptr);

        var = cat_search_variable_by_name(&at, &cmds[0], "v");
        EXPECT_EQ(var, nullptr);

        var = cat_search_variable_by_name(&at, &cmds[0], "var_ap");
        EXPECT_EQ(var, nullptr);

        var = cat_search_variable_by_name(&at, &cmds[0], "var_apx2");
        EXPECT_EQ(var, nullptr);

        var = cat_search_variable_by_name(&at, &cmds[0], "var_ap1_1");
        EXPECT_EQ(var, &vars_ap1[0]);

        var = cat_search_variable_by_name(&at, &cmds[0], "var_ap1_2");
        EXPECT_EQ(var, &vars_ap1[1]);

        var = cat_search_variable_by_name(&at, &cmds[0], "var_ap1_3");
        EXPECT_EQ(var, &vars_ap1[2]);

        var = cat_search_variable_by_name(&at, &cmds[0], "var_ap1_4");
        EXPECT_EQ(var, nullptr);

        var = cat_search_variable_by_name(&at, &cmds[1], "var_ap1_1");
        EXPECT_EQ(var, nullptr);

        var = cat_search_variable_by_name(&at, &cmds2[1], "var_apx2_1");
        EXPECT_EQ(var, &vars_apx2[0]);

        var = cat_search_variable_by_name(&at, &cmds2[1], "var_apx2_2");
        EXPECT_EQ(var, &vars_apx2[1]);

        var = cat_search_variable_by_name(&at, &cmds2[1], "var_apx2_3");
        EXPECT_EQ(var, &vars_apx2[2]);

        var = cat_search_variable_by_name(&at, &cmds2[1], "var_apx2_4");
        EXPECT_EQ(var, nullptr);

        var = cat_search_variable_by_name(&at, &cmds2[0], "var_apx2_1");
        EXPECT_EQ(var, nullptr);
}
