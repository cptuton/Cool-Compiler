#include <iostream>

#include "irparser.h"
#include "exceptions.h"

int main() {
    try{
        IR_Parser irp("test_ir.ir");

        irp.parse();
    } catch (IR_Error &ire) {
        ire.printException();
    }

    return 0;
}