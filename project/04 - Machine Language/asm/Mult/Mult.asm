// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

//// Replace this comment with your code.

// 初始化 R2 为 0，准备存储结果
@2
M=0         // RAM[2] = 0

// 定义循环的起始标签
(LOOP)
    // 检查 R1 (计数器) 是否为 0
    @1
    D=M       // D = RAM[1]
    @END
    D;JEQ     // 如果 D == 0，跳转到 END 标签

    // 执行 R2 = R2 + R0
    @2
    D=M       // D = RAM[2] (当前累加和)
    @0
    D=D+M     // D = D + RAM[0] (加上 R0 的值)
    @2
    M=D       // RAM[2] = D (更新累加和)

    // 执行 R1 = R1 - 1
    @1
    D=M       // D = RAM[1] (当前计数值)
    D=D-1     // D = D - 1 (计数值减 1)
    @1
    M=D       // RAM[1] = D (更新计数值)

    // 无条件跳回 LOOP 标签，继续下一次循环
    @LOOP
    0;JMP

// 定义循环的结束标签
(END)
    // 无限循环，让程序在这里停止
    @END
    0;JMP

// 注意：还可以用位运算实现，而且这是计算机底层实际使用的方法之一，效率远高于重复加法。