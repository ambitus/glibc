/* z/OS TODO: dummy. PLT trampolines go here.  */

	.text
	.globl _dl_runtime_resolve
	.hidden _dl_runtime_resolve
	.type _dl_runtime_resolve, @function
	.align 16
_dl_runtime_resolve:
	br %r14

	.globl _dl_runtime_profile
	.hidden _dl_runtime_profile
	.type _dl_runtime_profile, @function
	.align 16
_dl_runtime_profile:
	br %r14
