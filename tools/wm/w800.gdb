#target remote :1025

set confirm off
set print pretty on
set print array-indexes on

#set mem inaccessible-by-default on
#mem 0x00000000 0x07FFFFFF ro
#mem 0x1FF00000 0x1FF0FFFF ro
#mem 0x20000000 0x20027FFF rw
#mem 0x20028000 0x20047FFF rw
#mem 0x30000000 0x307FFFFF rw
#mem 0x40000000 0x5FFFFFFF rw
#mem 0x08000000 0x0FFFFFFF ro
#4M flash
#mem 0x08000000 0x08400000 ro

define dump_freertos_task_bt
    set $taskName = ((TCB_t *)$arg0)->pcTaskName
    set $pxTopOfStack = ((TCB_t *)$arg0)->pxTopOfStack
    set $taskPriority = ((TCB_t *)$arg0)->uxPriority
    printf "task index: %d, name: %s, priority: %d", $arg1, $taskName, $taskPriority
    if $arg2 == 0
        printf "\r\n"
    end
    if $arg2 == 1
        printf ", state: ready\r\n"
    end
    if $arg2 == 2
        printf ", state: blocked\r\n"
    end
    if $arg2 == 3
        printf ", state: deleted\r\n"
    end
    if $arg2 == 4
        printf ", state: suspended\r\n"
    end
    set $sp   = $pxTopOfStack + 49
    set $pc   = $pxTopOfStack[48]
    set $psr  = $pxTopOfStack[47]
    set $r15  = $pxTopOfStack[46]
    set $r13  = $pxTopOfStack[45]
    set $r12  = $pxTopOfStack[44]
    set $r11  = $pxTopOfStack[43]
    set $r10  = $pxTopOfStack[42]
    set $r9   = $pxTopOfStack[41]
    set $r8   = $pxTopOfStack[40]
    set $r7   = $pxTopOfStack[39]
    set $r6   = $pxTopOfStack[38]
    set $r5   = $pxTopOfStack[37]
    set $r4   = $pxTopOfStack[36]
    set $r3   = $pxTopOfStack[35]
    set $r2   = $pxTopOfStack[34]
    set $r1   = $pxTopOfStack[33]
    set $r0   = $pxTopOfStack[32]
    set $r31  = $pxTopOfStack[31]
    set $r30  = $pxTopOfStack[30]
    set $r29  = $pxTopOfStack[29]
    set $r28  = $pxTopOfStack[28]
    set $r27  = $pxTopOfStack[27]
    set $r26  = $pxTopOfStack[26]
    set $r25  = $pxTopOfStack[25]
    set $r24  = $pxTopOfStack[24]
    set $r23  = $pxTopOfStack[23]
    set $r22  = $pxTopOfStack[22]
    set $r21  = $pxTopOfStack[21]
    set $r20  = $pxTopOfStack[20]
    set $r19  = $pxTopOfStack[19]
    set $r18  = $pxTopOfStack[18]
    set $r17  = $pxTopOfStack[17]
    set $r16  = $pxTopOfStack[16]
    set $vr15 = $pxTopOfStack[15]
    set $vr14 = $pxTopOfStack[14]
    set $vr13 = $pxTopOfStack[13]
    set $vr12 = $pxTopOfStack[12]
    set $vr11 = $pxTopOfStack[11]
    set $vr10 = $pxTopOfStack[10]
    set $vr9  = $pxTopOfStack[9]
    set $vr8  = $pxTopOfStack[8]
    set $vr7  = $pxTopOfStack[7]
    set $vr6  = $pxTopOfStack[6]
    set $vr5  = $pxTopOfStack[5]
    set $vr4  = $pxTopOfStack[4]
    set $vr3  = $pxTopOfStack[3]
    set $vr2  = $pxTopOfStack[2]
    set $vr1  = $pxTopOfStack[1]
    set $vr0  = $pxTopOfStack[0] 
    bt
end

define iter_freertos_list
    set $itemNum = ((List_t *)$arg0)->uxNumberOfItems
    set $index = ((List_t *)$arg0)->pxIndex
    set $next = (struct xLIST_ITEM *)($index->pxNext)
    while $itemNum
        if $next->pvOwner != 0
            dump_freertos_task_bt $next->pvOwner $task_index $arg1
            set $itemNum = $itemNum - 1
            eval "set $task_index_%d_tcb = $next->pvOwner", $task_index
            set $task_index = $task_index + 1
        end

        set $next = (struct xLIST_ITEM *)($next->pxNext)
        printf "\r\n"
    end
end

define wmbt
    printf "\r\ndisplay freertos task stacks bgein\r\n\r\n"
    set $spTemp   = $sp
    set $pcTemp   = $pc
    set $psrTemp  = $psr
    set $r15Temp  = $r15
    set $r13Temp  = $r13
    set $r12Temp  = $r12
    set $r11Temp  = $r11
    set $r10Temp  = $r10
    set $r9Temp   = $r9
    set $r8Temp   = $r8
    set $r7Temp   = $r7
    set $r6Temp   = $r6
    set $r5Temp   = $r5
    set $r4Temp   = $r4
    set $r3Temp   = $r3
    set $r2Temp   = $r2
    set $r1Temp   = $r1
    set $r0Temp   = $r0
    set $r31Temp  = $r31
    set $r30Temp  = $r30
    set $r29Temp  = $r29
    set $r28Temp  = $r28
    set $r27Temp  = $r27
    set $r26Temp  = $r26
    set $r25Temp  = $r25
    set $r24Temp  = $r24
    set $r23Temp  = $r23
    set $r22Temp  = $r22
    set $r21Temp  = $r21
    set $r20Temp  = $r20
    set $r19Temp  = $r19
    set $r18Temp  = $r18
    set $r17Temp  = $r17
    set $r16Temp  = $r16
    set $vr15Temp = $vr15
    set $vr14Temp = $vr14
    set $vr13Temp = $vr13
    set $vr12Temp = $vr12
    set $vr11Temp = $vr11
    set $vr10Temp = $vr10
    set $vr9Temp  = $vr9
    set $vr8Temp  = $vr8
    set $vr7Temp  = $vr7
    set $vr6Temp  = $vr6
    set $vr5Temp  = $vr5
    set $vr4Temp  = $vr4
    set $vr3Temp  = $vr3
    set $vr2Temp  = $vr2
    set $vr1Temp  = $vr1
    set $vr0Temp  = $vr0

    if pxCurrentTCB != 0
        set $vec = ($epsr >> 16) & 0xff
        if $vec == 10 || $vec >= 32
            printf "Current stack in the interrupt %d\r\n", $vec - 32
        end
    end
    printf "Current task name: %s\r\n", ((TCB_t *)pxCurrentTCB)->pcTaskName
    printf "Current stack:\r\n"

    set $vec = ($psr >> 16) & 0xff
    if $vec == 30 || ($vec > 0 && $vec < 10)
        set $pc = $epc
        set $psr = $epsr
        set $r15 = $sp + 60
        bt
    else 
        bt
    end
    #printf "\r\n"

    set $task_index = 0
    set $uxTopPriority = 0
    while $uxTopPriority < (sizeof(pxReadyTasksLists) / sizeof(List_t))
        set $item = &pxReadyTasksLists[ $uxTopPriority ]
        iter_freertos_list $item 1
        set $uxTopPriority = $uxTopPriority + 1
    end
    iter_freertos_list pxDelayedTaskList 2
    iter_freertos_list pxOverflowDelayedTaskList 2
    iter_freertos_list &xTasksWaitingTermination 3
    iter_freertos_list &xSuspendedTaskList 4

    set $sp   = $spTemp  
    set $pc   = $pcTemp  
    set $psr  = $psrTemp 
    set $r15  = $r15Temp 
    set $r13  = $r13Temp 
    set $r12  = $r12Temp 
    set $r11  = $r11Temp 
    set $r10  = $r10Temp 
    set $r9   = $r9Temp  
    set $r8   = $r8Temp  
    set $r7   = $r7Temp  
    set $r6   = $r6Temp  
    set $r5   = $r5Temp  
    set $r4   = $r4Temp  
    set $r3   = $r3Temp  
    set $r2   = $r2Temp  
    set $r1   = $r1Temp  
    set $r0   = $r0Temp  
    set $r31  = $r31Temp 
    set $r30  = $r30Temp 
    set $r29  = $r29Temp 
    set $r28  = $r28Temp 
    set $r27  = $r27Temp 
    set $r26  = $r26Temp 
    set $r25  = $r25Temp 
    set $r24  = $r24Temp 
    set $r23  = $r23Temp 
    set $r22  = $r22Temp 
    set $r21  = $r21Temp 
    set $r20  = $r20Temp 
    set $r19  = $r19Temp 
    set $r18  = $r18Temp 
    set $r17  = $r17Temp 
    set $r16  = $r16Temp 
    set $vr15 = $vr15Temp
    set $vr14 = $vr14Temp
    set $vr13 = $vr13Temp
    set $vr12 = $vr12Temp
    set $vr11 = $vr11Temp
    set $vr10 = $vr10Temp
    set $vr9  = $vr9Temp 
    set $vr8  = $vr8Temp 
    set $vr7  = $vr7Temp 
    set $vr6  = $vr6Temp 
    set $vr5  = $vr5Temp 
    set $vr4  = $vr4Temp 
    set $vr3  = $vr3Temp 
    set $vr2  = $vr2Temp 
    set $vr1  = $vr1Temp 
    set $vr0  = $vr0Temp 
    printf "display freertos task stacks end\r\n\r\n"
end

define wmst
    if $argc != 0
        eval "set $task_tcb = $task_index_%d_tcb", $arg0
        if $task_tcb != 0
            dump_freertos_task_bt $task_tcb $arg0 0
        end
    end
end

define wmbk
    set $sp   = $spTemp  
    set $pc   = $pcTemp  
    set $psr  = $psrTemp 
    set $r15  = $r15Temp 
    set $r13  = $r13Temp 
    set $r12  = $r12Temp 
    set $r11  = $r11Temp 
    set $r10  = $r10Temp 
    set $r9   = $r9Temp  
    set $r8   = $r8Temp  
    set $r7   = $r7Temp  
    set $r6   = $r6Temp  
    set $r5   = $r5Temp  
    set $r4   = $r4Temp  
    set $r3   = $r3Temp  
    set $r2   = $r2Temp  
    set $r1   = $r1Temp  
    set $r0   = $r0Temp  
    set $r31  = $r31Temp 
    set $r30  = $r30Temp 
    set $r29  = $r29Temp 
    set $r28  = $r28Temp 
    set $r27  = $r27Temp 
    set $r26  = $r26Temp 
    set $r25  = $r25Temp 
    set $r24  = $r24Temp 
    set $r23  = $r23Temp 
    set $r22  = $r22Temp 
    set $r21  = $r21Temp 
    set $r20  = $r20Temp 
    set $r19  = $r19Temp 
    set $r18  = $r18Temp 
    set $r17  = $r17Temp 
    set $r16  = $r16Temp 
    set $vr15 = $vr15Temp
    set $vr14 = $vr14Temp
    set $vr13 = $vr13Temp
    set $vr12 = $vr12Temp
    set $vr11 = $vr11Temp
    set $vr10 = $vr10Temp
    set $vr9  = $vr9Temp 
    set $vr8  = $vr8Temp 
    set $vr7  = $vr7Temp 
    set $vr6  = $vr6Temp 
    set $vr5  = $vr5Temp 
    set $vr4  = $vr4Temp 
    set $vr3  = $vr3Temp 
    set $vr2  = $vr2Temp 
    set $vr1  = $vr1Temp 
    set $vr0  = $vr0Temp
    bt
end

define wmheap
    set $heap_remain = (size_t)wm_heaps.free_bytes_remaining
    printf "heap remain %u (%ukb):\n", $heap_remain, $heap_remain / 1024
    set $heap_index = 0
    while $heap_index < wm_soc_memory_count
        set $_memory = (wm_memory_layout_t *)&wm_soc_memory[$heap_index]
        set $heap_remain = (size_t)($_memory->heap.free_bytes_remaining)
            printf "    %s remain %u (%ukb)\n", $_memory->mem_name, $heap_remain, $heap_remain / 1024
        set $heap_index = $heap_index + 1
    end
    printf "\r\n"
    set $alloc_index = 0
    set $_block = (struct wm_heap_block *)(heap_used_block_list.next_free_block)
    while $_block
        set $_heap = (wm_heap_t *)($_block->heap)
        printf "%u\t%p\t%s:%d\t%u\n", $alloc_index, (uint8_t *)($_block + wm_heaps.heap_struct_size), $_block->file ? $_block->file : '\0', $_block->line >= 0 ? $_block->line : -1, $_block->block_size & (~$_heap->block_allocated_bit)
        set $alloc_index = $alloc_index + 1
        set $_block = (struct wm_heap_block *)($_block->next_free_block)
    end
    printf "summary: malloc size %u, malloc count %u\n", heap_used_block_list.block_size, (size_t)heap_used_block_list.file
end

document wmbt
    get all freertos task
    usage: wmbt
end

document wmst
    switch to freertos task
    usage: wmst <task index>
end

document wmbk
    back to the abnormal scene
    usage: wmbk
end

document wmheap
    get allocation information in all heaps
    usage: wmheap
end
