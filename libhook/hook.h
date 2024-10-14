#pragma once

/// <summary>
/// Insert x86 asm commands to jmp from jumpFrom to jumpTo
/// </summary>
/// <param name="jumpFrom"></param>
/// <param name="jumpTo"></param>
/// <param name="replaceBytes">Number of bytes lost due to commands insertion</param>
bool InsertJumpCode(unsigned int jumpFrom, unsigned int jumpTo, unsigned int replaceBytes = 5);
