#pragma once

struct MessageA {
    int field1;
    float field2;
};

struct MessageB {
    char field1[10];
    double field2;
};

enum MessagesIDS {
    MESSAGE_A = 1,
    MESSAGE_B = 2,
};

