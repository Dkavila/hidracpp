#include "periclesmachine.h"

PericlesMachine::PericlesMachine()
{
    identifier = "PRC";

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers.append(new Register( "A", "....00..", 8));
    registers.append(new Register( "B", "....01..", 8));
    registers.append(new Register( "X", "....10..", 8));
    registers.append(new Register("PC", "", 12));

    PC = registers.last();



    //////////////////////////////////////////////////
    // Initialize memory
    //////////////////////////////////////////////////

    setMemorySize(4096);



    //////////////////////////////////////////////////
    // Initialize flags
    //////////////////////////////////////////////////

    flags.append(new Flag(Flag::NEGATIVE, "N"));
    flags.append(new Flag(Flag::ZERO, "Z", true));
    flags.append(new Flag(Flag::CARRY_NOT_BORROW, "C"));



    //////////////////////////////////////////////////
    // Initialize instructions
    //////////////////////////////////////////////////

    instructions.append(new Instruction(1, "0000....", Instruction::NOP, "nop"));
    instructions.append(new Instruction(0, "0001....", Instruction::STR, "str r a"));
    instructions.append(new Instruction(0, "0010....", Instruction::LDR, "ldr r a"));
    instructions.append(new Instruction(0, "0011....", Instruction::ADD, "add r a"));
    instructions.append(new Instruction(0, "0100....", Instruction::OR,  "or r a"));
    instructions.append(new Instruction(0, "0101....", Instruction::AND, "and r a"));
    instructions.append(new Instruction(1, "0110....", Instruction::NOT, "not r"));
    instructions.append(new Instruction(0, "0111....", Instruction::SUB, "sub r a"));
    instructions.append(new Instruction(0, "1000....", Instruction::JMP, "jmp a"));
    instructions.append(new Instruction(0, "1001....", Instruction::JN,  "jn a"));
    instructions.append(new Instruction(0, "1010....", Instruction::JZ,  "jz a"));
    instructions.append(new Instruction(0, "1011....", Instruction::JC,  "jc a"));
    instructions.append(new Instruction(0, "1100....", Instruction::JSR, "jsr a"));
    instructions.append(new Instruction(1, "1101....", Instruction::NEG, "neg r"));
    instructions.append(new Instruction(1, "1110....", Instruction::SHR, "shr r"));
    instructions.append(new Instruction(1, "1111....", Instruction::HLT, "hlt"));



    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(new AddressingMode("......00", AddressingMode::DIRECT,       AddressingMode::NO_PATTERN));
    addressingModes.append(new AddressingMode("......01", AddressingMode::INDIRECT,     "(.*),i"));
    addressingModes.append(new AddressingMode("......10", AddressingMode::IMMEDIATE,    "#(.*)"));
    addressingModes.append(new AddressingMode("......11", AddressingMode::INDEXED_BY_X, "(.*),x"));
}

int PericlesMachine::reserveAssemblerMemory(const Instruction *instruction, QString arguments)
{
    static QRegExp whitespace("\\s+");

    QStringList argumentList = arguments.split(whitespace, QString::SkipEmptyParts);
    int numberOfArguments = instruction->getArguments().size();
    AddressingMode::AddressingModeCode addressingModeCode;

    // Check if correct number of arguments:
    if (argumentList.size() != numberOfArguments)
        throw wrongNumberOfArguments;

    extractArgumentAddressingModeCode(argumentList.last(), addressingModeCode); // Removes addressing mode from argument

    int bytesToReserve;
    // If the addressing mode is immediate reserve one byte for the instruction and one fore the number, otherwise reserve one for instruction and two for address.
    if (addressingModeCode == AddressingMode::IMMEDIATE)
        bytesToReserve = 2;
    else
        bytesToReserve = 3;

    Machine::reserveAssemblerMemory(bytesToReserve);
    return bytesToReserve;
}

bool PericlesMachine::customAddressWrite(QString argument, bool isImmediate)
{
    if (isImmediate)
        return false;

    int address = Machine::argumentToValue(argument, false);

    int leastSignificantByte = address & 0xFF;
    Machine::assemblerMemory[PC->getValue()]->setValue(leastSignificantByte);
    PC->incrementValue();

    int mostSignificantByte = (address >> 8) & 0x0F;
    Machine::assemblerMemory[PC->getValue()]->setValue(mostSignificantByte);
    PC->incrementValue();

    return true;
}