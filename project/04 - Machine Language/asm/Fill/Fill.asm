// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

//// Replace this comment with your code.

// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed,
// the screen should be cleared.

//// Replace this comment with your code.

// 计算并存储屏幕的总内存单元数
// 屏幕分辨率为 512x256 像素。
// 每个内存单元 (word) 对应 16 个像素。
// 因此，总单元数 = (512 / 16) * 256 = 32 * 256 = 8192。
@8192           // A = 8192 (将立即数 8192 加载到 A 寄存器)
D=A             // D = A (将 A 的值存入 D 寄存器，此时 D=8192)
@len            // A = 内存地址 "len" (这是一个我们自定义的变量)
M=D             // RAM[len] = D (将 D 的值，即 8192，存入名为 "len" 的内存单元)

// 主循环：持续监听键盘输入
(LOOP1)
    @KBD        // A = 键盘内存地址 (KBD 是一个常量，值为 0x6000)
    D=M         // D = RAM[KBD] (读取键盘的值。有按键时为非零，无按键时为 0)
    
    @WHITE      // A = 标签 "WHITE" 的地址
    D;JEQ       // 如果 D == 0 (无按键)，则跳转到 "WHITE" 标签处执行
    
    @BLACK      // A = 标签 "BLACK" 的地址
    0;JMP       // 无条件跳转到 "BLACK" 标签处执行 (如果有按键，程序会走到这里)

// 白屏填充函数：将屏幕所有像素设置为白色
(WHITE)
    @i          // A = 内存地址 "i" (这是一个循环计数器)
    M=0         // RAM[i] = 0 (初始化计数器 i 为 0)

    // 白屏填充的内部循环
    (LOOP2)
        @len        // A = 内存地址 "len"
        D=M         // D = RAM[len] (D = 8192)
        @i          // A = 内存地址 "i"
        D=D-M       // D = D - RAM[i] (计算 8192 - i)
        
        @LOOP1      // A = 标签 "LOOP1" 的地址
        D;JEQ       // 如果 D == 0 (即 i == 8192)，说明屏幕已填充完毕，跳回主循环 LOOP1
        
        // 计算当前要填充的屏幕内存地址
        @SCREEN     // A = 屏幕起始内存地址 (SCREEN 是一个常量，值为 0x4000)
        D=A         // D = SCREEN (将屏幕起始地址存入 D)
        @i          // A = 内存地址 "i"
        A=D+M       // A = D + RAM[i] (计算当前单元地址 = 屏幕基地址 + 偏移量 i)
        
        M=0         // RAM[A] = 0 (将当前屏幕内存单元设置为 0，对应白色像素)
        
        @i          // A = 内存地址 "i"
        M=M+1       // RAM[i] = RAM[i] + 1 (计数器 i 自增 1)
        
        @LOOP2      // A = 标签 "LOOP2" 的地址
        0;JMP       // 无条件跳回 LOOP2，继续填充下一个单元

// 黑屏填充函数：将屏幕所有像素设置为黑色
(BLACK)
    @i          // A = 内存地址 "i"
    M=0         // RAM[i] = 0 (初始化计数器 i 为 0)

    // 黑屏填充的内部循环
    (LOOP3)
        @len        // A = 内存地址 "len"
        D=M         // D = RAM[len] (D = 8192)
        @i          // A = 内存地址 "i"
        D=D-M       // D = D - RAM[i] (计算 8192 - i)
        
        @LOOP1      // A = 标签 "LOOP1" 的地址
        D;JEQ       // 如果 D == 0 (即 i == 8192)，说明屏幕已填充完毕，跳回主循环 LOOP1
        
        // 计算当前要填充的屏幕内存地址
        @SCREEN     // A = 屏幕起始内存地址
        D=A         // D = SCREEN
        @i          // A = 内存地址 "i"
        A=D+M       // A = D + RAM[i] (计算当前单元地址 = 屏幕基地址 + 偏移量 i)
        
        M=-1        // RAM[A] = -1 (将当前屏幕内存单元设置为 -1。在二进制中，-1 表示为全1，
                    // 即 1111111111111111，对应黑色像素)
        
        @i          // A = 内存地址 "i"
        M=M+1       // RAM[i] = RAM[i] + 1 (计数器 i 自增 1)
        
        @LOOP3      // A = 标签 "LOOP3" 的地址
        0;JMP       // 无条件跳回 LOOP3，继续填充下一个单元