; ---------------------------------------------------------------------------
; wait.s
; ---------------------------------------------------------------------------
;
; Wait for interrupt and return

.export  _wait, _stop, _nwait

; ---------------------------------------------------------------------------
; Wait for interrupt:  Forces the assembler to emit a WAI opcode ($CB)
; ---------------------------------------------------------------------------

.segment  "CODE"

.proc _wait: near

           SEI                    ; disable interrupts
.byte      $CB                    ; Inserts a WAI opcode
           RTS                    ; Return to caller

.endproc

; ---------------------------------------------------------------------------
; Stop:  Forces the assembler to emit a STP opcode ($DB)
; ---------------------------------------------------------------------------

.proc _stop: near

.byte      $DB                    ; Inserts a STP opcode

.endproc

.proc _nwait: near

           SEI                    ; disable interrupts
           NOP
.byte      $CB                    ; Inserts a WAI opcode
           RTS                    ; Return to caller

.endproc