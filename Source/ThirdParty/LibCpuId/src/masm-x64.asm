; Modified by Yao Wei Tjong for Urho3D

.code
; procedure exec_cpuid
; Signature: void exec_cpuid(uint32_t *regs)
exec_cpuid Proc
	push	rbx
	push	rcx
	push	rdx
	push	rdi
	
	mov	rdi,	rcx
	
	mov	eax,	[rdi]
	mov	ebx,	[rdi+4]
	mov	ecx,	[rdi+8]
	mov	edx,	[rdi+12]
	
	cpuid
	
	mov	[rdi],	eax
	mov	[rdi+4],	ebx
	mov	[rdi+8],	ecx
	mov	[rdi+12],	edx
	pop	rdi
	pop	rdx
	pop	rcx
	pop	rbx
	ret
exec_cpuid endp

; procedure cpu_rdtsc
; Signature: void cpu_rdtsc(uint64_t *result)
cpu_rdtsc Proc
	push	rdx
	rdtsc
	mov	[rcx],	eax
	mov	[rcx+4],	edx
	pop	rdx
	ret
cpu_rdtsc endp

; Urho3D: FIXME dummy implementation
; procedure busy_sse_loop
; Signature: void busy_sse_loop(int cycles)
busy_sse_loop Proc
    ret
busy_sse_loop endp

END
