#pragma once

struct Config {
    bool head           =  true,
         optional_arg   =  true,
         single_arg     =  false,
         multiple_args  =  false,
         init_list      =  false,
         target         =  false;
};


Config group_0 = {
    .head = false,
    .init_list = true;
}

Config group_1;

Config group_2 = {
    .optional_arg = false,
    .single_arg = true,
};

Config group_3 = {
    .multiple_args = true,
};

Config group_4 = {
    .init_list = true;
}