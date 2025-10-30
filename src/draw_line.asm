SECTION code_clib   ; put code in the normal code section for newlib

EXTERN asm_zx_saddrpdown ; hl=screen address -> hl=screen address(y+1)
EXTERN asm_zx_saddrpup ; hl=screen address -> hl=screen address(y-1)
EXTERN asm_zx_pxy2saddr ; l=x,h=y -> hl=screen address
EXTERN asm_zx_px2bitmask ; l=x -> l=bitmask for x

PUBLIC _draw_line_asm
; void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
_draw_line_asm:
    ; Stack layout: ret_addr(2), x1(1), y1(1), x2(1), y2(1)
    ld ix,2
    add ix,sp
    ; (ix+0)=x0, (ix+1)=y0, (ix+2)=x1, (ix+3)=y1

clip_y0:
    ld a,(ix+1) ; a = y0
    cp 192
    jr c, clip_y1
    ; For now just ignore any line with y0 out of bounds
    ret

clip_y1:
    ld a, (ix+3) ; a = y1
    cp 192
    jr c, calculate_dx
    ; For now just ignore any line with y0 out of bounds
    ret

calculate_dx:
    ld a,(ix+2) ; a=x1
    sub (ix+0) ; a=x1-x0, Z set if A=0 (x0==x1)
    ld b,+1 ; b=+1 id dx positive
    jr nc, dx_positive
    ld b,-1 ; b=-1 if dx negative
    neg
dx_positive:
    ld (dx),a ; dx=abs(x1-x0)
    ld a,b
    ld (sx),a ; sx=sign(x1-x0)

calculate_dy:
    ld a,(ix+3) ; a=y1
    sub (ix+1) ; a=y1-y0, Z set if A=0 (y0==y1)
    ld b,+1 ; b=+1 if dy positive
    jr nc, dy_positive
    ld b,-1 ; b=-1 if dy negative
    neg
dy_positive:
    ld (dy),a ; dy=abs(y1-y0)
    ld a,b
    ld (sy),a ; sx=sign(y1-y0)

determine_major_axis:
    ld a,(dy)
    ld d,a ; save dy
    ld a,(dx)
    sub d ; a=(dx-dy)
    jr nc,dx_major ; if dx >= dy, dx is major axis

dy_major: ; abs(dy) > abs(dx)
    ld a,(sy) ; a=sign(y1-y0)=+1/-1
    dec a
    jr z,dy_start ; if sy == -1, swap args before dy_start
dy_swap_args:
    pop hl
    pop de 
    pop bc 
    push de
    push bc
    push hl 
    ld a,(sy)
    neg a
    ld (sy),a ; sy = -sy
    ld a,(sx)
    neg a
    ld (sx),a ; sx = -sx
    jr dy_start

dx_major: ; abs(dx) >= abs(dyx)
    ld a,(sx) ; a=sign(x1-x0)=+1/-1
    dec a
    jr z,dx_start ; if sx == -1, swap args before dx_start
dx_swap_args:
    pop hl
    pop de 
    pop bc 
    push de
    push bc
    push hl 
    ld a,(sy)
    neg a
    ld (sy),a ; sy = -sy
    ld a,(sx)
    neg a
    ld (sx),a ; sx = -sx
    jr dx_start


; dx is the major axis and increment is in x direction    
dx_start:
    ; hl=screen address, c=bitmask
    call get_screen_address_and_mask

    ; Optimize for horizontal line
    ld a,(dy)
    or a
    jr z, horizontal_start

    ; Draw two lines if dx > 127 so we can use a byte for decision variable
    ld a,(dx)
    cp 128
    jp nc, dx_split_line
    
dx_single_line:
    ; Initialize decision variable
    ld a,(dx) ; a=dx
    srl a   ; a=dx/2
    ld d,a ; d=dx/2
    ld a,(dy) ; a=dy
    sub d  ;  a=(dy-dx/2)
    ld d,a ; d=(dy-dx/2) (D=(dy-dx/2) instead of D=(2dy-dx) so it's 0-255))

    ; Initialize decision variable increment for each y step
    ld a,(dx)
    ld e,a
    ld a,(dy)
    sub e  ; a=(dy-dx)
    ld e,a ; e=(dy-dx)

    ; Loop counter in b=dx+1
    ld a,(dx)
    ld b,a
    inc b ; b=dx+1
    jr dx_loop


; dy is the major axis and increment is in y direction    
dy_start:
    ; hl=screen address, c=bitmask
    call get_screen_address_and_mask

    ; Optimize for vertical line
    ld a,(dx)
    or a
    jr z, vertical_start

    ; Draw two lines if dy > 127 so we can use a byte for decision variable
    ld a,(dy)
    cp 128
    jp nc, dy_split_line

dy_single_line:
    ; Initialize decision variable
    ld a,(dy) ; a=dy
    srl a   ; a=dy/2
    ld d,a ; d=dy/2
    ld a,(dx) ; a=dx
    sub d  ;  a=(dx-y/2)
    ld d,a ; d=(dx-dy/2) (D=(dx-dy/2) instead of D=(2dx-dy) so it's 0-255))

    ; Initialize decision variable increment for each x step
    ld a,(dy)
    ld e,a
    ld a,(dx)
    sub e  ; a=(dx-dy)
    ld e,a ; e=(dx-dy)

    ; Loop counter in b=dy+1
    ld a,(dy)
    ld b,a
    inc b ; b=dy+1
    jr dy_loop
    

horizontal_start:
    ; hl=screen address,c=bitmask
    ld a,(dx)
    ld b,a
    inc b ; b=dx+1
    ld a,(hl) ; a=screen byte at (x0,y0)
horizontal_loop:
    or c ; set bit for x0
    rrc c
    jr nc,horizontal_next_bit
horizontal_next_byte:
    ld (hl),a
    inc l
    ld a,(hl)
horizontal_next_bit:
    djnz horizontal_loop
    ld (hl),a ; flush last byte
    ret


vertical_start:
    ; Set up loop counter in b=dy+1
    ld a,(dy)
    ld b,a
    inc b ; b=dy+1
vertical_loop:
    ; hl=screen address, c=bitmask
    ld a,(hl) ; a=screen byte at (x0,y0)
    or c ; set bit for x0
    ld (hl),a ; *(screen_ptr) = a   
    call asm_zx_saddrpdown ; hl=next line down
    djnz vertical_loop
    ret



dx_loop:
    ld a,(hl) ; a=screen byte at (x0,y0)
    or c ; set bit for x0
    ld (hl),a ; *(screen_ptr) = a   

    bit 7,d ; check sign of D
    jr NZ, dx_loop_no_change_y ; if D<0, no change in y

dx_loop_change_y: ; if D >= 0, change y by sy(+1/-1)
    ld a,(sy)
    dec a
    jr z, dx_loop_increment_y
dx_loop_decrement_y:
    call asm_zx_saddrpup ; hl=next line up
    jr dx_loop_change_y_done
dx_loop_increment_y:
    call asm_zx_saddrpdown ; hl=next line down
dx_loop_change_y_done:
    ld a,e    ; a=(dy-dx)
    add a,d   ; a=D+(dy-dx)
    ld d,a    ; D'=D+(dy-dx) (instead of D'=D+2(dy-dx))
    jr dx_loop_increment_x

dx_loop_no_change_y: ; else D < 0
    ld a,(dy) ; a=dy
    add d  ; a=D+dy
    ld d,a ; D'=D+dy  (instead of D'=D+2dy)

dx_loop_increment_x:
    rrc c ; shift bitmask to next x position
    jr nc,dx_loop_next
    inc l ; move to next byte in screen memory

dx_loop_next:
    djnz dx_loop
    ret


dy_loop:
    ld a,(hl) ; a=screen byte at (x0,y0)
    or c ; set bit for x0
    ld (hl),a ; *(screen_ptr) = a   

    bit 7,d ; check sign of D
    jr NZ, dy_loop_no_change_x ; if D<0, no change in yx

dy_loop_change_x: ; if D >= 0, change x by sx(+1/-1)
    ld a,(sx)
    dec a
    jr z, dy_loop_increment_x
dy_loop_decrement_x:
    rlc c ; shift bitmask to previous x position
    jr nc,dy_loop_change_x_done
    dec l ; move to previous byte in screen memory
    jr dy_loop_change_x_done
dy_loop_increment_x:
    rrc c ; shift bitmask to next x position
    jr nc,dy_loop_change_x_done
    inc l ; move to next byte in screen memory
dy_loop_change_x_done:
    ld a,e    ; a=(dx-dy)
    add a,d   ; a=D+(dx-dy) 
    ld d,a    ; D'=D+(dx-dy) (instead of D'=D+2(dx-dy))
    jr dy_loop_increment_y

dy_loop_no_change_x: ; else D < 0
    ld a,(dx) ; a=D
    add d  ; a=D+dx
    ld d,a ; D'=D+dx  (instead of D'=D+2dx)

dy_loop_increment_y:
    call asm_zx_saddrpdown ; hl=next line down
    djnz dy_loop
    ret


dx_split_line:
    ; Draw first half
    ld d,(ix+2) ; d=x1
    ld e,(ix+3) ; e=y1
    push de
    ld de,(dx) ; e=dx, d=dy
    push de
    srl d ; d=dy/2
    srl e ; e=dx/2
    ld (dx),de ; dx=dx/2, dy=dy/2
    ld a,(ix+0) ; a=x0
    add e ; a=x0+dx/2
    ld (ix+2),a ; x1=x0+dx/2
    ld a,(sy)
    dec a
    jr z, dx_split_line_down
dx_split_line_up:
    ld a,(ix+1) ; a=y0
    sub d ; a=y0-dy/2   
    ld (ix+3),a ; y1=y0-dy/2
    jr dx_split_line_draw_first_half
dx_split_line_down:
    ld a,(ix+1) ; a=y0
    add d ; a=y0+dy/2
    ld (ix+3),a ; y1=y0+dy/2

dx_split_line_draw_first_half:
    call dx_single_line
    
    ; Draw second half
    ld a,(ix+2) ; a=x-midpoint
    ld (ix+0),a ; x0=x-midpoint
    ld a,(ix+3) ; a=y-midpoint
    ld (ix+1),a ; y0=y-midpoint

    pop de ; e=dx, d=dy
    ld a,(dx)
    ld b,a
    ld a,e
    sub b
    ld (dx),a ; dx=original dx - dx/2
    ld a,(dy)
    ld b,a
    ld a,d
    sub b
    ld (dy),a ; dy=original dy - dy/2

    pop de ; d=x1, e=y1
    ld (ix+2),d ; restore original x1
    ld (ix+3),e ; restore original y1

    ; hl=screen address, c=bitmask
    call get_screen_address_and_mask
    jp dx_single_line


dy_split_line:
    ; Draw first half
    ld d,(ix+2) ; d=x1
    ld e,(ix+3) ; e=y1
    push de
    ld de,(dx) ; e=dx, d=dy
    push de
    srl d ; d=dy/2
    srl e ; e=dx/2
    ld (dx),de ; dx=dx/2, dy=dy/2
    ld a,(ix+1) ; a=y0
    add d ; a=y0+dx/2
    ld (ix+3),a ; y1=y0+dy/2
    ld a,(sx)
    dec a
    jr z, dy_split_line_right
dy_split_line_left:
    ld a,(ix+0) ; a=x0
    sub e ; a=x0-dx/2   
    ld (ix+2),a ; x1=x0-dx/2
    jr dy_split_line_draw_first_half
dy_split_line_right:
    ld a,(ix+0) ; a=x1
    add e ; a=x1+dx/2
    ld (ix+2),a ; x1=x0+xy/2

dy_split_line_draw_first_half:
    call dy_single_line

    ; Draw second half
    ld a,(ix+2) ; a=x-midpoint
    ld (ix+0),a ; x0=x-midpoint
    ld a,(ix+3) ; a=y-midpoint
    ld (ix+1),a ; y0=y-midpoint

    pop de ; e=dx, d=dy
    ld a,(dx)
    ld b,a
    ld a,e
    sub b
    ld (dx),a ; dx=original dx - dx/2
    ld a,(dy)
    ld b,a
    ld a,d
    sub b
    ld (dy),a ; dy=original dy - dy/2

    pop de ; d=x1, e=y1
    ld (ix+2),d ; restore original x1
    ld (ix+3),e ; restore original y1

    ; hl=screen address, c=bitmask
    call get_screen_address_and_mask
    jp dy_single_line


; hl=screen address for (x0,y0), c=bitmask for x0
get_screen_address_and_mask:
    ; Now (ix+0)=x0, (ix+1)=y0, (ix+2)=x1, (ix+3)=y1 with x0<=x1

    ; Calculate starting bit mask
    ld l,(ix+0) ; l=x0
    call asm_zx_px2bitmask ; l=x0 -> l=bitmask for x0
    ld c,l ; c=bitmask for x0

    ; Calculate starting screen address
    ld l,(ix+0) ; l=x0
    ld h,(ix+1) ; h=y0
    call asm_zx_pxy2saddr ; l=x0,h=y0 -> hl=screen address

    ; Now hl=screen address of (x0,y0), c=bitmask for x0
    ret

SECTION bss_user
sx:    defb 0 ; sign(x1-x0)
sy:    defb 0 ; sign(y1-y0)
dx:    defb 0 ; abs(x1-x0)
dy:    defb 0 ; abs(y1-y0)

dummy: defb 0 ; Dezog workaround

